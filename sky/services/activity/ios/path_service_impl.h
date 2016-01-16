// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_SERVICES_ACTIVITY_IOS_PATH_SERVICE_IMPL_H_
#define SKY_SERVICES_ACTIVITY_IOS_PATH_SERVICE_IMPL_H_

#include "base/macros.h"
#include "mojo/public/cpp/application/interface_factory.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "sky/services/activity/activity.mojom.h"

namespace sky {
namespace services {
namespace path {

class PathServiceImpl : public activity::PathService {
 public:
  explicit PathServiceImpl(
      mojo::InterfaceRequest<activity::PathService> request);
  ~PathServiceImpl() override;

  void GetAppDataDir(const GetAppDataDirCallback& callback) override;
  void GetFilesDir(const GetFilesDirCallback& callback) override;
  void GetCacheDir(const GetCacheDirCallback& callback) override;

 private:
  mojo::StrongBinding<activity::PathService> binding_;

  DISALLOW_COPY_AND_ASSIGN(PathServiceImpl);
};

class PathServiceFactory
    : public mojo::InterfaceFactory<activity::PathService> {
 public:
  void Create(mojo::ApplicationConnection* connection,
              mojo::InterfaceRequest<activity::PathService> request) override;
};

}  // namespace path
}  // namespace services
}  // namespace sky

#endif  // SKY_SERVICES_ACTIVITY_IOS_PATH_SERVICE_IMPL_H_
