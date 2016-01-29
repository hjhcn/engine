// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include "mojo/skia/ganesh_image_factory.h"

#include "base/logging.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTextureProvider.h"
#include "third_party/skia/include/gpu/gl/GrGLTypes.h"

namespace mojo {
namespace skia {
namespace {
void ReleaseThunk(void* data) {
  auto release_callback = static_cast<base::Closure*>(data);
  release_callback->Run();
  delete release_callback;
}
}  // namespace

::skia::RefPtr<SkImage> CreateImageFromTexture(
    const GaneshContext::Scope& scope,
    uint32_t texture_id,
    uint32_t width,
    uint32_t height,
    const base::Closure& release_callback) {
  DCHECK(texture_id);
  DCHECK(width);
  DCHECK(height);

  // TODO(jeffbrown): Give the caller more control over these parameters.
  GrGLTextureInfo info;
  info.fTarget = GL_TEXTURE_2D;
  info.fID = texture_id;

  GrBackendTextureDesc desc;
  desc.fFlags = kRenderTarget_GrBackendTextureFlag;
  desc.fWidth = width;
  desc.fHeight = height;
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fOrigin = kTopLeft_GrSurfaceOrigin;
  desc.fTextureHandle = reinterpret_cast<GrBackendObject>(&info);
  return ::skia::AdoptRef(SkImage::NewFromTexture(
      scope.gr_context(), desc, kPremul_SkAlphaType, &ReleaseThunk,
      new base::Closure(release_callback)));
}

MailboxTextureImageGenerator::MailboxTextureImageGenerator(
    const GLbyte mailbox_name[GL_MAILBOX_SIZE_CHROMIUM],
    GLuint sync_point,
    uint32_t width,
    uint32_t height)
    : SkImageGenerator(SkImageInfo::MakeN32Premul(width, height)),
      sync_point_(sync_point) {
  DCHECK(mailbox_name);
  memcpy(mailbox_name_, mailbox_name, GL_MAILBOX_SIZE_CHROMIUM);
}

MailboxTextureImageGenerator::~MailboxTextureImageGenerator() {}

GrTexture* MailboxTextureImageGenerator::onGenerateTexture(
    GrContext* context,
    const SkIRect* subset) {
  if (sync_point_)
    glWaitSyncPointCHROMIUM(sync_point_);

  GLuint texture_id =
      glCreateAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, mailbox_name_);
  if (!texture_id)
    return nullptr;

  // TODO(jeffbrown): Give the caller more control over these parameters.
  GrGLTextureInfo info;
  info.fTarget = GL_TEXTURE_2D;
  info.fID = texture_id;

  GrBackendTextureDesc desc;
  desc.fFlags = kRenderTarget_GrBackendTextureFlag;
  desc.fWidth = getInfo().width();
  desc.fHeight = getInfo().height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fOrigin = kTopLeft_GrSurfaceOrigin;
  desc.fTextureHandle = reinterpret_cast<GrBackendObject>(&info);
  return context->textureProvider()->wrapBackendTexture(desc,
                                                        kAdopt_GrWrapOwnership);
}

}  // namespace skia
}  // namespace mojo
