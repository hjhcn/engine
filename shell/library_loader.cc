// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/base_jni_onload.h"
#include "base/android/base_jni_registrar.h"
#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "base/android/library_loader/library_loader_hooks.h"
#include "base/bind.h"
#include "base/logging.h"
#include "mojo/android/system/core_impl.h"
#include "sky/shell/java_service_provider.h"
#include "sky/shell/platform_view.h"
#include "sky/shell/sky_main.h"

namespace {

base::android::RegistrationMethod kSkyRegisteredMethods[] = {
    {"CoreImpl", mojo::android::RegisterCoreImpl},
    {"JavaServiceProvider", sky::shell::RegisterJavaServiceProvider},
    {"SkyMain", sky::shell::RegisterSkyMain},
    {"PlatformView", sky::shell::PlatformView::Register},
};

bool RegisterJNI(JNIEnv* env) {
  if (!base::android::RegisterJni(env))
    return false;

  return RegisterNativeMethods(env, kSkyRegisteredMethods,
                               arraysize(kSkyRegisteredMethods));
}

}  // namespace

// This is called by the VM when the shared library is first loaded.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  std::vector<base::android::RegisterCallback> register_callbacks;
  register_callbacks.push_back(base::Bind(&RegisterJNI));
  if (!base::android::OnJNIOnLoadRegisterJNI(vm, register_callbacks) ||
      !base::android::OnJNIOnLoadInit(
          std::vector<base::android::InitCallback>())) {
    return -1;
  }

  return JNI_VERSION_1_4;
}
