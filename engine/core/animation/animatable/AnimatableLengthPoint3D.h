/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLELENGTHPOINT3D_H_
#define SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLELENGTHPOINT3D_H_

#include "sky/engine/core/animation/animatable/AnimatableValue.h"

namespace blink {

class AnimatableLengthPoint3D final : public AnimatableValue {
public:
    virtual ~AnimatableLengthPoint3D() { }
    static PassRefPtr<AnimatableLengthPoint3D> create(PassRefPtr<AnimatableValue> x, PassRefPtr<AnimatableValue> y, PassRefPtr<AnimatableValue> z)
    {
        return adoptRef(new AnimatableLengthPoint3D(x, y, z));
    }
    const AnimatableValue* x() const { return m_x.get(); }
    const AnimatableValue* y() const { return m_y.get(); }
    const AnimatableValue* z() const { return m_z.get(); }

protected:
    virtual PassRefPtr<AnimatableValue> interpolateTo(const AnimatableValue*, double fraction) const override;

private:
    AnimatableLengthPoint3D(PassRefPtr<AnimatableValue> x, PassRefPtr<AnimatableValue> y, PassRefPtr<AnimatableValue> z)
        : m_x(x)
        , m_y(y)
        , m_z(z)
    {
    }
    virtual AnimatableType type() const override { return TypeLengthPoint3D; }
    virtual bool equalTo(const AnimatableValue*) const override;

    RefPtr<AnimatableValue> m_x;
    RefPtr<AnimatableValue> m_y;
    RefPtr<AnimatableValue> m_z;
};

DEFINE_ANIMATABLE_VALUE_TYPE_CASTS(AnimatableLengthPoint3D, isLengthPoint3D());

} // namespace blink

#endif  // SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLELENGTHPOINT3D_H_
