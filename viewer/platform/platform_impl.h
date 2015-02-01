// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_VIEWER_PLATFORM_PLATFORM_IMPL_H_
#define SKY_VIEWER_PLATFORM_PLATFORM_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/threading/thread_local_storage.h"
#include "base/timer/timer.h"
#include "mojo/services/network/public/interfaces/network_service.mojom.h"
#include "sky/engine/public/platform/Platform.h"

namespace mojo {
class ApplicationImpl;
}

namespace sky {

class PlatformImpl : public blink::Platform {
 public:
  explicit PlatformImpl(mojo::ApplicationImpl* app);
  virtual ~PlatformImpl();

  // blink::Platform methods:
  virtual blink::WebString defaultLocale();
  virtual base::SingleThreadTaskRunner* mainThreadTaskRunner();
  virtual mojo::NetworkService* networkService();
  virtual blink::WebURLLoader* createURLLoader();
  virtual blink::WebURLError cancelledError(const blink::WebURL& url) const;

 private:
  mojo::NetworkServicePtr network_service_;
  base::MessageLoop* main_loop_;
  scoped_refptr<base::SingleThreadTaskRunner> main_thread_task_runner_;

  DISALLOW_COPY_AND_ASSIGN(PlatformImpl);
};

}  // namespace sky

#endif  // SKY_VIEWER_PLATFORM_PLATFORM_IMPL_H_
