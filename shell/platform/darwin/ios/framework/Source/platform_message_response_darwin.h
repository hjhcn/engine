// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_
#define FLUTTER_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_

#include <Foundation/Foundation.h>

#include "flutter/fml/macros.h"
#include "flutter/fml/make_copyable.h"
#include "flutter/fml/platform/darwin/scoped_block.h"
#include "flutter/fml/task_runner.h"
#include "flutter/lib/ui/window/platform_message_response.h"
#include "flutter/shell/platform/darwin/common/buffer_conversions.h"

typedef void (^PlatformMessageResponseCallback)(NSData*);

namespace shell {

class PlatformMessageResponseDarwin : public blink::PlatformMessageResponse {
 public:
  void Complete(std::unique_ptr<fml::Mapping> data) override;

  void CompleteEmpty() override;

 private:
  explicit PlatformMessageResponseDarwin(PlatformMessageResponseCallback callback,
                                         fml::RefPtr<fml::TaskRunner> platform_task_runner);

  ~PlatformMessageResponseDarwin() override;

  fml::ScopedBlock<PlatformMessageResponseCallback> callback_;
  fml::RefPtr<fml::TaskRunner> platform_task_runner_;

  FML_FRIEND_MAKE_REF_COUNTED(PlatformMessageResponseDarwin);
};

}  // namespace shell

#endif  // FLUTTER_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_
