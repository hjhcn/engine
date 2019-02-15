// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/fml/mapping.h"
#include "flutter/fml/paths.h"
#include "flutter/fml/thread.h"
#include "flutter/runtime/dart_isolate.h"
#include "flutter/runtime/dart_vm.h"
#include "flutter/runtime/dart_vm_lifecycle.h"
#include "flutter/testing/testing.h"
#include "flutter/testing/thread_test.h"
#include "third_party/tonic/scopes/dart_isolate_scope.h"

namespace blink {

using DartIsolateTest = ::testing::ThreadTest;

TEST_F(DartIsolateTest, RootIsolateCreationAndShutdown) {
  Settings settings = {};
  settings.task_observer_add = [](intptr_t, fml::closure) {};
  settings.task_observer_remove = [](intptr_t) {};
  auto vm = DartVMRef::Create(settings);
  ASSERT_TRUE(vm);
  auto vm_data = vm->GetVMData();
  ASSERT_TRUE(vm_data);
  TaskRunners task_runners(testing::GetCurrentTestName(),  //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner()          //
  );
  auto weak_isolate = DartIsolate::CreateRootIsolate(
      vm_data->GetSettings(),         // settings
      vm_data->GetIsolateSnapshot(),  // isolate snapshot
      vm_data->GetSharedSnapshot(),   // shared snapshot
      std::move(task_runners),        // task runners
      nullptr,                        // window
      {},                             // snapshot delegate
      {},                             // io manager
      "main.dart",                    // advisory uri
      "main"                          // advisory entrypoint
  );
  auto root_isolate = weak_isolate.lock();
  ASSERT_TRUE(root_isolate);
  ASSERT_EQ(root_isolate->GetPhase(), DartIsolate::Phase::LibrariesSetup);
  ASSERT_TRUE(root_isolate->Shutdown());
}

TEST_F(DartIsolateTest, IsolateShutdownCallbackIsInIsolateScope) {
  Settings settings = {};
  settings.task_observer_add = [](intptr_t, fml::closure) {};
  settings.task_observer_remove = [](intptr_t) {};
  auto vm = DartVMRef::Create(settings);
  ASSERT_TRUE(vm);
  auto vm_data = vm->GetVMData();
  ASSERT_TRUE(vm_data);
  TaskRunners task_runners(testing::GetCurrentTestName(),  //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner(),         //
                           GetCurrentTaskRunner()          //
  );
  auto weak_isolate = DartIsolate::CreateRootIsolate(
      vm_data->GetSettings(),         // settings
      vm_data->GetIsolateSnapshot(),  // isolate snapshot
      vm_data->GetSharedSnapshot(),   // shared snapshot
      std::move(task_runners),        // task runners
      nullptr,                        // window
      {},                             // snapshot delegate
      {},                             // io manager
      "main.dart",                    // advisory uri
      "main"                          // advisory entrypoint
  );
  auto root_isolate = weak_isolate.lock();
  ASSERT_TRUE(root_isolate);
  ASSERT_EQ(root_isolate->GetPhase(), DartIsolate::Phase::LibrariesSetup);
  size_t destruction_callback_count = 0;
  root_isolate->AddIsolateShutdownCallback([&destruction_callback_count]() {
    ASSERT_NE(Dart_CurrentIsolate(), nullptr);
    destruction_callback_count++;
  });
  ASSERT_TRUE(root_isolate->Shutdown());
  ASSERT_EQ(destruction_callback_count, 1u);
}

class AutoIsolateShutdown {
 public:
  AutoIsolateShutdown(blink::DartVMRef vm,
                      std::shared_ptr<blink::DartIsolate> isolate)
      : vm_(std::move(vm)), isolate_(std::move(isolate)) {
    FML_CHECK(vm_);
  }

  ~AutoIsolateShutdown() {
    if (isolate_) {
      FML_LOG(INFO) << "Shutting down isolate.";
      if (!isolate_->Shutdown()) {
        FML_LOG(ERROR) << "Could not shutdown isolate.";
      }
    }
  }

  bool IsValid() const { return isolate_ != nullptr; }

  FML_WARN_UNUSED_RESULT
  bool RunInIsolateScope(std::function<bool(void)> closure) {
    if (!isolate_) {
      return false;
    }
    tonic::DartIsolateScope scope(isolate_->isolate());
    tonic::DartApiScope api_scope;
    if (closure) {
      return closure();
    }
    return true;
  }

  blink::DartIsolate* get() {
    FML_CHECK(isolate_);
    return isolate_.get();
  }

 private:
  blink::DartVMRef vm_;
  std::shared_ptr<blink::DartIsolate> isolate_;

  FML_DISALLOW_COPY_AND_ASSIGN(AutoIsolateShutdown);
};

std::unique_ptr<AutoIsolateShutdown> RunDartCodeInIsolate(
    fml::RefPtr<fml::TaskRunner> task_runner,
    std::string entrypoint) {
  Settings settings = {};
  settings.enable_observatory = true;
  settings.task_observer_add = [](intptr_t, fml::closure) {};
  settings.task_observer_remove = [](intptr_t) {};

  auto vm = DartVMRef::Create(settings);
  if (!vm) {
    return {};
  }

  auto vm_data = vm->GetVMData();

  TaskRunners task_runners(testing::GetCurrentTestName(),  //
                           task_runner,                    //
                           task_runner,                    //
                           task_runner,                    //
                           task_runner                     //
  );

  auto weak_isolate = DartIsolate::CreateRootIsolate(
      vm_data->GetSettings(),         // settings
      vm_data->GetIsolateSnapshot(),  // isolate snapshot
      vm_data->GetSharedSnapshot(),   // shared snapshot
      std::move(task_runners),        // task runners
      nullptr,                        // window
      {},                             // snapshot delegate
      {},                             // io manager
      "main.dart",                    // advisory uri
      "main"                          // advisory entrypoint
  );

  auto root_isolate =
      std::make_unique<AutoIsolateShutdown>(std::move(vm), weak_isolate.lock());

  if (!root_isolate->IsValid()) {
    FML_LOG(ERROR) << "Could not create isolate.";
    return {};
  }

  if (root_isolate->get()->GetPhase() != DartIsolate::Phase::LibrariesSetup) {
    FML_LOG(ERROR) << "Created isolate is in unexpected phase.";
    return {};
  }

  auto kernel_file_path =
      fml::paths::JoinPaths({testing::GetFixturesPath(), "kernel_blob.bin"});

  if (!fml::IsFile(kernel_file_path)) {
    FML_LOG(ERROR) << "Could not locate kernel file.";
    return {};
  }

  auto kernel_file = fml::OpenFile(kernel_file_path.c_str(), false,
                                   fml::FilePermission::kRead);

  if (!kernel_file.is_valid()) {
    FML_LOG(ERROR) << "Kernel file descriptor was invalid.";
    return {};
  }

  auto kernel_mapping = std::make_unique<fml::FileMapping>(kernel_file);

  if (kernel_mapping->GetMapping() == nullptr) {
    FML_LOG(ERROR) << "Could not setup kernel mapping.";
    return {};
  }

  if (!root_isolate->get()->PrepareForRunningFromKernel(
          std::move(kernel_mapping))) {
    FML_LOG(ERROR)
        << "Could not prepare to run the isolate from the kernel file.";
    return {};
  }

  if (root_isolate->get()->GetPhase() != DartIsolate::Phase::Ready) {
    FML_LOG(ERROR) << "Isolate is in unexpected phase.";
    return {};
  }

  if (!root_isolate->get()->Run(entrypoint)) {
    FML_LOG(ERROR) << "Could not run the method \"" << entrypoint
                   << "\" in the isolate.";
    return {};
  }

  return root_isolate;
}

TEST_F(DartIsolateTest, IsolateCanLoadAndRunDartCode) {
  auto isolate = RunDartCodeInIsolate(GetCurrentTaskRunner(), "main");
  ASSERT_TRUE(isolate);
  ASSERT_EQ(isolate->get()->GetPhase(), DartIsolate::Phase::Running);
}

TEST_F(DartIsolateTest, IsolateCannotLoadAndRunUnknownDartEntrypoint) {
  auto isolate =
      RunDartCodeInIsolate(GetCurrentTaskRunner(), "thisShouldNotExist");
  ASSERT_FALSE(isolate);
}

TEST_F(DartIsolateTest, CanRunDartCodeCodeSynchronously) {
  auto isolate = RunDartCodeInIsolate(GetCurrentTaskRunner(), "main");

  ASSERT_TRUE(isolate);

  ASSERT_TRUE(isolate->RunInIsolateScope([]() -> bool {
    if (tonic::LogIfError(::Dart_Invoke(Dart_RootLibrary(),
                                        tonic::ToDart("sayHi"), 0, nullptr))) {
      return false;
    }
    return true;
  }));
}

}  // namespace blink
