//
//  dart_to_cpp.cc
//  sources
//
//  Created by 对象 on 2019/7/31.
//

#include "dart_to_cpp.h"
#include "flutter/fml/logging.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_class_library.h"
#include "third_party/tonic/dart_class_provider.h"
#include "third_party/tonic/dart_message_handler.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/file_loader/file_loader.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/scopes/dart_api_scope.h"
#include "third_party/tonic/scopes/dart_isolate_scope.h"
#include "third_party/tonic/dart_library_natives.h"
#include "flutter/runtime/dart_isolate.h"

using namespace tonic;

namespace Kraken {
  static DartPersistentValue library_;
  
  void DartToCpp::DidCreateIsolate() {
    library_.Set(DartState::Current(),
                 Dart_LookupLibrary(ToDart("package:kraken/hooks.dart")));
  }
  
  namespace {
    static std::function<void(const std::string&)> dart_to_js_callback_;
    void KrakenCallJS(Dart_NativeArguments args) {
      if (dart_to_js_callback_) {
        Dart_Handle exception = nullptr;
        const std::string data = DartConverter<std::string>::FromArguments(args, 0, exception);
        dart_to_js_callback_(data);
      }
    }
    
    static DartLibraryNatives* g_natives;
    
    Dart_NativeFunction GetNativeFunction(Dart_Handle name,
    int argument_count,
    bool* auto_setup_scope) {
      return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
    }
    
    const uint8_t* GetSymbol(Dart_NativeFunction native_function) {
      return g_natives->GetSymbol(native_function);
    }
    
  }
  
  void DartToCpp::InitBinding(const flutter::Settings& settings) {
    if (!g_natives) {
      g_natives = new DartLibraryNatives();
      dart_to_js_callback_ = settings.dart_to_js_callback;
      g_natives->Register({
        {"Kraken_callJS", KrakenCallJS, 1, true},
      });
      Dart_Handle result = Dart_SetNativeResolver(library_.value(), GetNativeFunction, GetSymbol);
      if (Dart_IsError(result)) {
        Dart_PropagateError(result);
      }
    }
  }
  
  void DartToCpp::invokeDartFromCpp(const char* name, const std::string& arg) {
    std::shared_ptr<tonic::DartState> dart_state = library_.dart_state().lock();
    if (!dart_state)
      return;
    tonic::DartState::Scope scope(dart_state);
    LogIfError(DartInvokeField(library_.value(), name, {
      StdStringToDart(arg),
    }));
  }
  
}
