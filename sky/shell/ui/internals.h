// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_SHELL_UI_INTERNALS_H_
#define SKY_SHELL_UI_INTERNALS_H_

#include "base/supports_user_data.h"
#include "dart/runtime/include/dart_api.h"
#include "mojo/public/cpp/application/interface_factory.h"
#include "mojo/public/cpp/application/service_provider_impl.h"
#include "mojo/public/interfaces/application/service_provider.mojom.h"
#include "mojo/services/asset_bundle/interfaces/asset_bundle.mojom.h"
#include "sky/services/engine/sky_engine.mojom.h"

namespace mojo {
class ApplicationConnection;
}

namespace sky {
namespace shell {

class Internals
  : public base::SupportsUserData::Data,
    public mojo::InterfaceFactory<mojo::asset_bundle::AssetUnpacker> {
 public:
  ~Internals() override;

  static void Create(Dart_Isolate isolate,
                     ServicesDataPtr services,
                     mojo::asset_bundle::AssetBundlePtr root_bundle);

  mojo::Handle TakeShellProxy();
  mojo::Handle TakeServicesProvidedByEmbedder();
  mojo::Handle TakeRootBundleHandle();
  mojo::Handle TakeServicesProvidedToEmbedder();

 private:
  explicit Internals(ServicesDataPtr services,
                     mojo::asset_bundle::AssetBundlePtr root_bundle);

  // |mojo::InterfaceFactory<mojo::asset_bundle::AssetUnpacker>| implementation:
  void Create(
      mojo::ApplicationConnection* connection,
      mojo::InterfaceRequest<mojo::asset_bundle::AssetUnpacker>) override;

  ServicesDataPtr services_;
  mojo::asset_bundle::AssetBundlePtr root_bundle_;

  mojo::ServiceProviderPtr service_provider_;
  mojo::ServiceProviderImpl service_provider_impl_;

  // We need to hold this object to work around
  // https://github.com/domokit/mojo/issues/536
  mojo::ServiceProviderPtr services_provided_to_embedder;

  // A ServiceProvider supplied by the application that exposes services to
  // the embedder.
  mojo::InterfaceRequest<mojo::ServiceProvider>
      services_provided_to_embedder_;

  MOJO_DISALLOW_COPY_AND_ASSIGN(Internals);
};

}  // namespace shell
}  // namespace sky

#endif  // SKY_SHELL_UI_INTERNALS_H_
