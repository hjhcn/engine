// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_PAINTING_TYPEFACE_H_
#define SKY_ENGINE_CORE_PAINTING_TYPEFACE_H_

#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefCounted.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace blink {

class Typeface : public RefCounted<Typeface>, public DartWrappable {
  DEFINE_WRAPPERTYPEINFO();
 public:
  ~Typeface() override;

  SkTypeface* typeface() { return typeface_.get(); }
  void set_typeface(PassRefPtr<SkTypeface> typeface) { typeface_ = typeface; }

 protected:
  Typeface(PassRefPtr<SkTypeface> typeface);

 private:
  RefPtr<SkTypeface> typeface_;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_PAINTING_TYPEFACE_H_
