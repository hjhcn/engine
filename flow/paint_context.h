// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_PAINT_CONTEXT_H_
#define FLOW_PAINT_CONTEXT_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/logging.h"
#include "flow/instrumentation.h"
#include "flow/raster_cache.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"

namespace flow {

class PaintContext {
 public:
  class ScopedFrame {
   public:
    SkCanvas& canvas() { return *canvas_; }
    PaintContext& context() const { return context_; }
    GrContext* gr_context() const { return gr_context_; }

    ScopedFrame(ScopedFrame&& frame);
    ~ScopedFrame();

   private:
    PaintContext& context_;
    GrContext* gr_context_;
    SkCanvas* canvas_;
    const bool instrumentation_enabled_;

    ScopedFrame(PaintContext& context,
                GrContext* gr_context,
                SkCanvas& canvas,
                bool instrumentation_enabled);

    friend class PaintContext;

    DISALLOW_COPY_AND_ASSIGN(ScopedFrame);
  };

  PaintContext();
  ~PaintContext();

  ScopedFrame AcquireFrame(GrContext* gr_context,
                           SkCanvas& canvas,
                           bool instrumentation_enabled = true);

  RasterCache& raster_cache() { return raster_cache_; }
  const instrumentation::Counter& frame_count() const { return frame_count_; }
  const instrumentation::Stopwatch& frame_time() const { return frame_time_; }
  instrumentation::Stopwatch& engine_time() { return engine_time_; };

 private:
  RasterCache raster_cache_;

  instrumentation::Counter frame_count_;
  instrumentation::Stopwatch frame_time_;
  instrumentation::Stopwatch engine_time_;

  void beginFrame(ScopedFrame& frame, bool enableInstrumentation);
  void endFrame(ScopedFrame& frame, bool enableInstrumentation);

  DISALLOW_COPY_AND_ASSIGN(PaintContext);
};

}  // namespace flow

#endif  // FLOW_PAINT_CONTEXT_H_
