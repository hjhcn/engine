// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/painting/ImageShader.h"

#include "sky/engine/tonic/dart_args.h"
#include "sky/engine/tonic/dart_binding_macros.h"
#include "sky/engine/tonic/dart_converter.h"
#include "sky/engine/tonic/dart_library_natives.h"

namespace blink {

static void ImageShader_constructor(Dart_NativeArguments args) {
  DartCallConstructor(&ImageShader::create, args);
}

IMPLEMENT_WRAPPERTYPEINFO(ui, ImageShader);

#define FOR_EACH_BINDING(V) \
  V(ImageShader, initWithImage)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void ImageShader::RegisterNatives(DartLibraryNatives* natives) {
  natives->Register({
    { "ImageShader_constructor", ImageShader_constructor, 1, true },
FOR_EACH_BINDING(DART_REGISTER_NATIVE)
  });
}

scoped_refptr<ImageShader> ImageShader::create() {
  return new ImageShader();
}

void ImageShader::initWithImage(CanvasImage* image,
                                SkShader::TileMode tmx,
                                SkShader::TileMode tmy,
                                const Float64List& matrix4) {
  DCHECK(image != NULL);
  SkMatrix sk_matrix = toSkMatrix(matrix4);
  SkBitmap bitmap;
  image->image()->asLegacyBitmap(&bitmap, SkImage::kRO_LegacyBitmapMode);

  set_shader(SkShader::MakeBitmapShader(bitmap, tmx, tmy, &sk_matrix));
}

ImageShader::ImageShader() : Shader(nullptr) {
}

ImageShader::~ImageShader() {
}

}  // namespace blink
