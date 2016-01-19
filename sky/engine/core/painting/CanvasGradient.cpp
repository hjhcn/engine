// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/painting/CanvasGradient.h"

#include "sky/engine/tonic/dart_args.h"
#include "sky/engine/tonic/dart_binding_macros.h"
#include "sky/engine/tonic/dart_converter.h"
#include "sky/engine/tonic/dart_library_natives.h"

namespace blink {

typedef CanvasGradient Gradient; // Because the C++ name doesn't match the Dart name.

static void Gradient_constructor(Dart_NativeArguments args) {
  DartCallConstructor(&CanvasGradient::create, args);
}

IMPLEMENT_WRAPPERTYPEINFO(ui, Gradient);

#define FOR_EACH_BINDING(V) \
  V(Gradient, initLinear) \
  V(Gradient, initRadial)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void CanvasGradient::RegisterNatives(DartLibraryNatives* natives) {
  natives->Register({
    { "Gradient_constructor", Gradient_constructor, 1, true },
FOR_EACH_BINDING(DART_REGISTER_NATIVE)
  });
}

PassRefPtr<CanvasGradient> CanvasGradient::create() {
  return adoptRef(new CanvasGradient());
}

void CanvasGradient::initLinear(const std::vector<Point>& end_points,
                                const std::vector<CanvasColor>& colors,
                                const std::vector<float>& color_stops,
                                SkShader::TileMode tile_mode) {
  ASSERT(end_points.size() == 2);
  ASSERT(colors.size() == color_stops.size() || color_stops.data() == nullptr);
  SkPoint sk_end_points[2];
  for (int i = 0; i < 2; ++i)
    sk_end_points[i] = end_points[i].sk_point;

  std::vector<SkColor> sk_colors;
  sk_colors.reserve(colors.size());
  for (const CanvasColor& color : colors)
    sk_colors.push_back(color);

  SkShader* shader = SkGradientShader::CreateLinear(
      sk_end_points, sk_colors.data(), color_stops.data(), sk_colors.size(),
      tile_mode);
  set_shader(adoptRef(shader));
}

void CanvasGradient::initRadial(const Point& center,
                                double radius,
                                const std::vector<CanvasColor>& colors,
                                const std::vector<float>& color_stops,
                                SkShader::TileMode tile_mode) {
  ASSERT(colors.size() == color_stops.size() || color_stops.data() == nullptr);

  std::vector<SkColor> sk_colors;
  sk_colors.reserve(colors.size());
  for (const CanvasColor& color : colors)
    sk_colors.push_back(color);

  SkShader* shader = SkGradientShader::CreateRadial(
      center.sk_point, radius, sk_colors.data(), color_stops.data(),
      sk_colors.size(), tile_mode);
  set_shader(adoptRef(shader));
}

CanvasGradient::CanvasGradient()
    : Shader(nullptr)
{
}

CanvasGradient::~CanvasGradient()
{
}

}  // namespace blink
