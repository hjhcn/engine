// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/compositor/layer_host.h"

#include "base/debug/trace_event.h"
#include "base/message_loop/message_loop.h"
#include "mojo/converters/geometry/geometry_type_converters.h"
#include "mojo/gpu/gl_context.h"
#include "mojo/services/public/cpp/surfaces/surfaces_utils.h"
#include "mojo/skia/ganesh_context.h"
#include "sky/compositor/layer.h"

namespace sky {

LayerHost::LayerHost(LayerHostClient* client)
    : client_(client),
      state_(kIdle),
      surface_holder_(this, client->GetShell()),
      gl_context_(mojo::GLContext::Create(client->GetShell())),
      ganesh_context_(gl_context_),
      resource_manager_(gl_context_),
      scheduler_(this, base::MessageLoop::current()->task_runner()) {
  scheduler_.UpdateVSync(
      TimeInterval(base::TimeTicks(), base::TimeDelta::FromSecondsD(1.0 / 60)));
}

LayerHost::~LayerHost() {
}

void LayerHost::SetNeedsAnimate() {
  scheduler_.SetNeedsFrame();
  state_ = kWaitingForBeginFrame;
}

void LayerHost::SetRootLayer(scoped_refptr<Layer> layer) {
  DCHECK(!root_layer_.get());
  root_layer_ = layer;
  root_layer_->set_host(this);
}

void LayerHost::OnSurfaceIdAvailable(mojo::SurfaceIdPtr surface_id) {
  client_->OnSurfaceIdAvailable(surface_id.Pass());

  if (state_ == kWaitingForSurfaceToUploadFrame)
    Upload(root_layer_.get());
}

void LayerHost::ReturnResources(
    mojo::Array<mojo::ReturnedResourcePtr> resources) {
  resource_manager_.ReturnResources(resources.Pass());
}

void LayerHost::BeginFrame(base::TimeTicks frame_time,
                           base::TimeTicks deadline) {

  TRACE_EVENT0("sky", "LayerHost::BeginFrame");

  DCHECK_EQ(state_, kWaitingForBeginFrame);
  state_ = kProducingFrame;
  client_->BeginFrame(frame_time);

  {
    mojo::GaneshContext::Scope scope(&ganesh_context_);
    ganesh_context_.gr()->resetContext();
    root_layer_->Display();
  }

  Upload(root_layer_.get());

  if (state_ == kProducingFrame)
    state_ = kIdle;
}

void LayerHost::Upload(Layer* layer) {
  TRACE_EVENT0("sky", "LayerHost::Upload");

  if (!surface_holder_.IsReadyForFrame()) {
    if (state_ == kProducingFrame) {
      // Currently we use a timer to drive the BeginFrame cycle, which means we
      // can produce frames before the surfaces service is ready to receive
      // frames from us. In that situation, we wait for surfaces before
      // uploading the frame. The upload will actually happen when the surface
      // id is available (i.e., in OnSurfaceIdAvailable). If SetNeedsAnimate is
      // called before then, we'll go back into the kWaitingForBeginFrame state
      // and defer to the timer again.
      //
      // We can avoid this complexity if we use feedback from the surfaces
      // service to drive the BeginFrame cycle. In that approach, we wouldn't
      // get here before we've attached to a surface.
      state_ = kWaitingForSurfaceToUploadFrame;
    }
    return;
  }

  gfx::Size size = layer->size();
  surface_holder_.SetSize(size);

  mojo::FramePtr frame = mojo::Frame::New();
  frame->resources.resize(0u);

  mojo::Rect bounds;
  bounds.width = size.width();
  bounds.height = size.height();
  mojo::PassPtr pass = mojo::CreateDefaultPass(1, bounds);
  pass->quads.resize(0u);
  pass->shared_quad_states.push_back(mojo::CreateDefaultSQS(
      mojo::TypeConverter<mojo::Size, gfx::Size>::Convert(size)));

  mojo::TransferableResourcePtr resource =
      resource_manager_.CreateTransferableResource(layer);

  mojo::QuadPtr quad = mojo::Quad::New();
  quad->material = mojo::MATERIAL_TEXTURE_CONTENT;

  mojo::RectPtr rect = mojo::Rect::New();
  rect->width = size.width();
  rect->height = size.height();
  quad->rect = rect.Clone();
  quad->opaque_rect = rect.Clone();
  quad->visible_rect = rect.Clone();
  quad->needs_blending = true;
  quad->shared_quad_state_index = 0u;

  mojo::TextureQuadStatePtr texture_state = mojo::TextureQuadState::New();
  texture_state->resource_id = resource->id;
  texture_state->premultiplied_alpha = true;
  texture_state->uv_top_left = mojo::PointF::New();
  texture_state->uv_bottom_right = mojo::PointF::New();
  texture_state->uv_bottom_right->x = 1.f;
  texture_state->uv_bottom_right->y = 1.f;
  texture_state->background_color = mojo::Color::New();
  texture_state->background_color->rgba = 0;
  for (int i = 0; i < 4; ++i)
    texture_state->vertex_opacity.push_back(1.f);
  texture_state->flipped = false;

  frame->resources.push_back(resource.Pass());
  quad->texture_quad_state = texture_state.Pass();
  pass->quads.push_back(quad.Pass());

  frame->passes.push_back(pass.Pass());
  surface_holder_.SubmitFrame(frame.Pass());
}

}  // namespace sky
