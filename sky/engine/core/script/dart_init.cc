// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/script/dart_init.h"

#include <dlfcn.h>

#include "base/bind.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "dart/runtime/bin/embedded_dart_io.h"
#include "dart/runtime/include/dart_mirrors_api.h"
#include "mojo/public/platform/dart/dart_handle_watcher.h"
#include "sky/engine/bindings/dart_mojo_internal.h"
#include "sky/engine/bindings/dart_runtime_hooks.h"
#include "sky/engine/bindings/dart_ui.h"
#include "sky/engine/core/script/dart_debugger.h"
#include "sky/engine/core/script/dart_service_isolate.h"
#include "sky/engine/core/script/dom_dart_state.h"
#include "sky/engine/public/platform/sky_settings.h"
#include "sky/engine/tonic/dart_api_scope.h"
#include "sky/engine/tonic/dart_class_library.h"
#include "sky/engine/tonic/dart_dependency_catcher.h"
#include "sky/engine/tonic/dart_error.h"
#include "sky/engine/tonic/dart_invoke.h"
#include "sky/engine/tonic/dart_io.h"
#include "sky/engine/tonic/dart_isolate_scope.h"
#include "sky/engine/tonic/dart_library_loader.h"
#include "sky/engine/tonic/dart_snapshot_loader.h"
#include "sky/engine/tonic/dart_state.h"
#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/tonic/uint8_list.h"

#ifdef OS_ANDROID
#include "sky/engine/bindings/jni/dart_jni.h"
#endif

namespace dart {
namespace observatory {

// These two symbols are defined in |observatory_archive.cc| which is generated
// by the |//dart/runtime/observatory:archive_observatory| rule. Both of these
// symbols will be part of the data segment and therefore are read only.
extern unsigned int observatory_assets_archive_len;
extern const uint8_t* observatory_assets_archive;

}  // namespace observatory
}  // namespace dart

namespace blink {

Dart_Handle DartLibraryTagHandler(Dart_LibraryTag tag,
                                  Dart_Handle library,
                                  Dart_Handle url) {
  return DartLibraryLoader::HandleLibraryTag(tag, library, url);
}

namespace {

void CreateEmptyRootLibraryIfNeeded() {
  if (Dart_IsNull(Dart_RootLibrary())) {
    Dart_LoadScript(Dart_NewStringFromCString("dart:empty"), Dart_EmptyString(),
                    0, 0);
  }
}

static const char* kDartArgs[] = {
    "--enable_mirrors=false",
    // Dart assumes ARM devices are insufficiently powerful and sets the
    // default profile period to 100Hz. This number is suitable for older
    // Raspberry Pi devices but quite low for current smartphones.
    "--profile_period=1000",
    // Background compilation isn't quite ready, but this flag turns it on if we
    // want to experiment with it.
    // "--background_compilation",
#if (WTF_OS_IOS || WTF_OS_MACOSX)
    // On platforms where LLDB is the primary debugger, SIGPROF signals
    // overwhelm LLDB.
    "--no-profile",
#endif
};

static const char* kDartPrecompilationArgs[]{
    "--precompilation",
};

static const char* kDartCheckedModeArgs[] = {
    "--enable_asserts",
    "--enable_type_checks",
    "--error_on_bad_type",
    "--error_on_bad_override",
};

void IsolateShutdownCallback(void* callback_data) {
  // TODO(dart)
}

bool IsServiceIsolateURL(const char* url_name) {
  return url_name != nullptr &&
         String(url_name) == DART_VM_SERVICE_ISOLATE_NAME;
}

// TODO(rafaelw): Right now this only supports the creation of the handle
// watcher isolate and the service isolate. Presumably, we'll want application
// isolates to spawn their own isolates.
Dart_Isolate IsolateCreateCallback(const char* script_uri,
                                   const char* main,
                                   const char* package_root,
                                   const char** package_map,
                                   Dart_IsolateFlags* flags,
                                   void* callback_data,
                                   char** error) {
  TRACE_EVENT0("flutter", __func__);
  if (IsServiceIsolateURL(script_uri)) {
    DartState* dart_state = new DartState();
    Dart_Isolate isolate = Dart_CreateIsolate(
        script_uri, "main", reinterpret_cast<const uint8_t*>(
                                DART_SYMBOL(kDartIsolateSnapshotBuffer)),
        nullptr, nullptr, error);
    CHECK(isolate) << error;
    dart_state->SetIsolate(isolate);
    CHECK(Dart_IsServiceIsolate(isolate));
    CHECK(!LogIfError(Dart_SetLibraryTagHandler(DartLibraryTagHandler)));
    {
      DartApiScope dart_api_scope;
      DartIO::InitForIsolate();
      DartUI::InitForIsolate();
      DartMojoInternal::InitForIsolate();
#ifdef OS_ANDROID
      DartJni::InitForIsolate();
#endif
      DartRuntimeHooks::Install(DartRuntimeHooks::DartIOIsolate);
      if (SkySettings::Get().enable_observatory) {
        std::string ip = "127.0.0.1";
        const intptr_t port = 8181;
        const bool service_isolate_booted = DartServiceIsolate::Startup(
            ip, port, DartLibraryTagHandler, IsRunningPrecompiledCode(), error);
        CHECK(service_isolate_booted) << error;
      }
    }
    Dart_ExitIsolate();
    return isolate;
  }

  // Create & start the handle watcher isolate
  // TODO(abarth): Who deletes this DartState instance?
  DartState* dart_state = new DartState();
  Dart_Isolate isolate = Dart_CreateIsolate(
      "sky:handle_watcher", "",
      reinterpret_cast<uint8_t*>(DART_SYMBOL(kDartIsolateSnapshotBuffer)),
      nullptr, dart_state, error);
  CHECK(isolate) << error;
  dart_state->SetIsolate(isolate);

  CHECK(!LogIfError(Dart_SetLibraryTagHandler(DartLibraryTagHandler)));

  {
    DartApiScope dart_api_scope;
    DartIO::InitForIsolate();
    DartUI::InitForIsolate();
    DartMojoInternal::InitForIsolate();
#ifdef OS_ANDROID
    DartJni::InitForIsolate();
#endif

    if (!script_uri)
      CreateEmptyRootLibraryIfNeeded();
  }

  Dart_ExitIsolate();

  CHECK(Dart_IsolateMakeRunnable(isolate));
  return isolate;
}

Dart_Handle GetVMServiceAssetsArchiveCallback() {
  return DartConverter<Uint8List>::ToDart(
      ::dart::observatory::observatory_assets_archive,
      ::dart::observatory::observatory_assets_archive_len);
}

static const char kStdoutStreamId[] = "Stdout";
static const char kStderrStreamId[] = "Stderr";

static bool ServiceStreamListenCallback(const char* stream_id) {
  if (strcmp(stream_id, kStdoutStreamId) == 0) {
    dart::bin::SetCaptureStdout(true);
    return true;
  } else if (strcmp(stream_id, kStderrStreamId) == 0) {
    dart::bin::SetCaptureStderr(true);
    return true;
  }
  return false;
}

static void ServiceStreamCancelCallback(const char* stream_id) {
  if (strcmp(stream_id, kStdoutStreamId) == 0) {
    dart::bin::SetCaptureStdout(false);
  } else if (strcmp(stream_id, kStderrStreamId) == 0) {
    dart::bin::SetCaptureStderr(false);
  }
}

}  // namespace

#if DART_ALLOW_DYNAMIC_RESOLUTION

const char* kDartVmIsolateSnapshotBufferName = "kDartVmIsolateSnapshotBuffer";
const char* kDartIsolateSnapshotBufferName = "kDartIsolateSnapshotBuffer";
const char* kInstructionsSnapshotName = "kInstructionsSnapshot";

const char* kDartApplicationLibraryPath =
    "FlutterApplication.framework/FlutterApplication";

static void* DartLookupSymbolInLibrary(const char* symbol_name,
                                       const char* library) {
  TRACE_EVENT0("flutter", __func__);
  if (symbol_name == nullptr) {
    return nullptr;
  }
  dlerror();  // clear previous errors on thread
  void* library_handle = dlopen(library, RTLD_NOW);
  if (dlerror() != nullptr) {
    return nullptr;
  }
  void* sym = dlsym(library_handle, symbol_name);
  return dlerror() != nullptr ? nullptr : sym;
}

void* _DartSymbolLookup(const char* symbol_name) {
  TRACE_EVENT0("flutter", __func__);
  if (symbol_name == nullptr) {
    return nullptr;
  }

  // First the application library is checked for the valid symbols. This
  // library may not necessarily exist. If it does exist, it is loaded and the
  // symbols resolved. Once the application library is loaded, there is
  // currently no provision to unload the same.
  void* symbol =
      DartLookupSymbolInLibrary(symbol_name, kDartApplicationLibraryPath);
  if (symbol != nullptr) {
    return symbol;
  }

  // Check inside the default library
  return DartLookupSymbolInLibrary(symbol_name, nullptr);
}

static const uint8_t* PrecompiledInstructionsSymbolIfPresent() {
  return reinterpret_cast<uint8_t*>(DART_SYMBOL(kInstructionsSnapshot));
}

bool IsRunningPrecompiledCode() {
  TRACE_EVENT0("flutter", __func__);
  return PrecompiledInstructionsSymbolIfPresent() != nullptr;
}

#else  // DART_ALLOW_DYNAMIC_RESOLUTION

static const uint8_t* PrecompiledInstructionsSymbolIfPresent() {
  return nullptr;
}

bool IsRunningPrecompiledCode() {
  return false;
}

#endif  // DART_ALLOW_DYNAMIC_RESOLUTION

void InitDartVM() {
  TRACE_EVENT0("flutter", __func__);

  {
    TRACE_EVENT0("flutter", "dart::bin::BootstrapDartIo");
    dart::bin::BootstrapDartIo();
  }

  DartMojoInternal::SetHandleWatcherProducerHandle(
      mojo::dart::HandleWatcher::Start());

  bool enable_checked_mode = SkySettings::Get().enable_dart_checked_mode;
#if ENABLE(DART_STRICT)
  enable_checked_mode = true;
#endif

  Vector<const char*> args;
  args.append(kDartArgs, arraysize(kDartArgs));

  if (IsRunningPrecompiledCode())
    args.append(kDartPrecompilationArgs, arraysize(kDartPrecompilationArgs));

  if (enable_checked_mode)
    args.append(kDartCheckedModeArgs, arraysize(kDartCheckedModeArgs));

  CHECK(Dart_SetVMFlags(args.size(), args.data()));

  {
    TRACE_EVENT0("flutter", "DartDebugger::InitDebugger");
    // This should be called before calling Dart_Initialize.
    DartDebugger::InitDebugger();
  }

  DartUI::InitForGlobal();
#ifdef OS_ANDROID
  DartJni::InitForGlobal();
#endif

  {
    TRACE_EVENT0("flutter", "Dart_Initialize");
    CHECK(Dart_Initialize(reinterpret_cast<uint8_t*>(
                              DART_SYMBOL(kDartVmIsolateSnapshotBuffer)),
                          PrecompiledInstructionsSymbolIfPresent(),
                          IsolateCreateCallback,
                          nullptr,  // Isolate interrupt callback.
                          nullptr, IsolateShutdownCallback,
                          // File IO callbacks.
                          nullptr, nullptr, nullptr, nullptr,
                          // Entroy source
                          nullptr,
                          // VM service assets archive
                          GetVMServiceAssetsArchiveCallback) == nullptr);
  }

  // Allow streaming of stdout and stderr by the Dart vm.
  Dart_SetServiceStreamCallbacks(&ServiceStreamListenCallback,
                                 &ServiceStreamCancelCallback);
}

}  // namespace blink
