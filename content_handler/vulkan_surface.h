// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>
#include "apps/mozart/lib/scene/client/resources.h"
#include "flutter/flow/scene_update_context.h"
#include "flutter/vulkan/vulkan_handle.h"
#include "flutter/vulkan/vulkan_proc_table.h"
#include "lib/ftl/macros.h"
#include "lib/mtl/tasks/message_loop.h"
#include "lib/mtl/tasks/message_loop_handler.h"
#include "mx/event.h"
#include "mx/vmo.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"

namespace flutter_runner {

class VulkanSurface : public flow::SceneUpdateContext::SurfaceProducerSurface,
                      public mtl::MessageLoopHandler {
 public:
  VulkanSurface(vulkan::VulkanProcTable& p_vk,
                sk_sp<GrContext> context,
                sk_sp<GrVkBackendContext> backend_context,
                mozart::client::Session* session,
                const SkISize& size);

  ~VulkanSurface() override;

  size_t AdvanceAndGetAge() override;

  bool FlushSessionAcquireAndReleaseEvents() override;

  bool IsValid() const override;

  SkISize GetSize() const override;

  // Note: It is safe for the caller to collect the surface in the
  // |on_writes_committed| callback.
  void SignalWritesFinished(
      std::function<void(void)> on_writes_committed) override;

  // |flow::SceneUpdateContext::SurfaceProducerSurface|
  mozart::client::Image* GetImage() override;

  // |flow::SceneUpdateContext::SurfaceProducerSurface|
  sk_sp<SkSurface> GetSkiaSurface() const override;

 private:
  vulkan::VulkanProcTable& vk_;
  sk_sp<GrVkBackendContext> backend_context_;
  mozart::client::Session* session_;
  vulkan::VulkanHandle<VkImage> vk_image_;
  vulkan::VulkanHandle<VkDeviceMemory> vk_memory_;
  sk_sp<SkSurface> sk_surface_;
  std::unique_ptr<mozart::client::Image> session_image_;
  mx::event acquire_event_;
  mx::event release_event_;
  mtl::MessageLoop::HandlerKey event_handler_key_ = 0;
  std::function<void(void)> pending_on_writes_committed_;
  size_t age_ = 0;
  bool valid_ = false;

  // |mtl::MessageLoopHandler|
  void OnHandleReady(mx_handle_t handle,
                     mx_signals_t pending,
                     uint64_t count) override;

  bool AllocateDeviceMemory(sk_sp<GrContext> context,
                            const SkISize& size,
                            mx::vmo& exported_vmo);

  bool SetupSkiaSurface(sk_sp<GrContext> context,
                        const SkISize& size,
                        const VkImageCreateInfo& image_create_info,
                        const VkMemoryRequirements& memory_reqs);

  bool CreateFences();

  bool PushSessionImageSetupOps(mozart::client::Session* session,
                                mx::vmo exported_vmo);

  void Reset();

  FTL_DISALLOW_COPY_AND_ASSIGN(VulkanSurface);
};

}  // namespace flutter_runner
