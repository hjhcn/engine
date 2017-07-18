// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content_handler/vulkan_rasterizer.h"

#include <fcntl.h>
#include <magenta/device/vfs.h>
#include <mxio/watcher.h>
#include <unistd.h>

#include <chrono>
#include <thread>
#include <utility>

#include "flutter/common/threads.h"
#include "flutter/glue/trace_event.h"
#include "lib/ftl/files/unique_fd.h"

namespace flutter_runner {

constexpr char kDisplayDriverClass[] = "/dev/class/display";

static mx_status_t DriverWatcher(int dirfd,
                                 int event,
                                 const char* fn,
                                 void* cookie) {
  if (event == WATCH_EVENT_ADD_FILE && !strcmp(fn, "000")) {
    return MX_ERR_STOP;
  }
  return MX_OK;
}

bool WaitForFirstDisplayDriver() {
  ftl::UniqueFD fd(open(kDisplayDriverClass, O_DIRECTORY | O_RDONLY));
  if (fd.get() < 0) {
    FTL_DLOG(ERROR) << "Failed to open " << kDisplayDriverClass;
    return false;
  }

  mx_status_t status = mxio_watch_directory(
      fd.get(), DriverWatcher, mx_deadline_after(MX_SEC(1)), nullptr);
  return status == MX_ERR_STOP;
}

VulkanRasterizer::VulkanRasterizer() : compositor_context_(nullptr) {
  valid_ = WaitForFirstDisplayDriver();
}

VulkanRasterizer::~VulkanRasterizer() = default;

bool VulkanRasterizer::IsValid() const {
  return valid_;
}

void VulkanRasterizer::SetSession(
    fidl::InterfaceHandle<mozart2::Session> session,
    mx::eventpair import_token) {
  ASSERT_IS_GPU_THREAD;
  FTL_DCHECK(valid_ && !session_connection_);
  session_connection_ = std::make_unique<SessionConnection>(
      std::move(session), std::move(import_token));
}

void VulkanRasterizer::Draw(std::unique_ptr<flow::LayerTree> layer_tree,
                            ftl::Closure callback) {
  ASSERT_IS_GPU_THREAD;
  FTL_DCHECK(callback != nullptr);

  if (layer_tree == nullptr) {
    FTL_LOG(ERROR) << "Layer tree was not valid.";
    callback();
    return;
  }

  if (!session_connection_) {
    FTL_LOG(ERROR) << "Session was not valid.";
    callback();
    return;
  }

  compositor_context_.engine_time().SetLapTime(layer_tree->construction_time());

  flow::CompositorContext::ScopedFrame frame = compositor_context_.AcquireFrame(
      nullptr, nullptr, true /* instrumentation enabled */);
  {
    // Preroll the Flutter layer tree. This allows Flutter to perform pre-paint
    // optimizations.
    TRACE_EVENT0("flutter", "Preroll");
    layer_tree->Preroll(frame);
  }

  {
    // Traverse the Flutter layer tree so that the necessary session ops to
    // represent the frame are enqueued in the underlying session.
    TRACE_EVENT0("flutter", "UpdateScene");
    layer_tree->UpdateScene(session_connection_->scene_update_context(),
                            session_connection_->root_node());
  }

  {
    // Flush all pending session ops.
    TRACE_EVENT0("flutter", "SessionPresent");
    session_connection_->Present(
        frame, [callback = std::move(callback)]() { callback(); });
  }
}

}  // namespace flutter_runner
