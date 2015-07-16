/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef SKY_ENGINE_CORE_CSS_FONTFACE_H_
#define SKY_ENGINE_CORE_CSS_FONTFACE_H_

#include "gen/sky/core/CSSPropertyNames.h"
#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/core/css/CSSValue.h"
#include "sky/engine/core/dom/DOMException.h"
#include "sky/engine/platform/fonts/FontTraits.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefCounted.h"
#include "sky/engine/wtf/text/WTFString.h"

namespace blink {

class CSSFontFace;
class CSSValueList;
class Document;
class ExceptionState;
class FontFaceReadyPromiseResolver;
class StylePropertySet;
class StyleRuleFontFace;

class FontFace : public RefCounted<FontFace> {
public:
    enum LoadStatus { Unloaded, Loading, Loaded, Error };

    static PassRefPtr<FontFace> create(Document*, const StyleRuleFontFace*);

    ~FontFace();

    const AtomicString& family() const { return m_family; }
    String style() const;
    String weight() const;
    String stretch() const;
    String unicodeRange() const;
    String variant() const;
    String featureSettings() const;

    String status() const;

    LoadStatus loadStatus() const { return m_status; }
    void setLoadStatus(LoadStatus);
    void setError(PassRefPtr<DOMException> = nullptr);
    DOMException* error() const { return m_error.get(); }
    FontTraits traits() const;
    CSSFontFace* cssFontFace() { return m_cssFontFace.get(); }

    bool hadBlankText() const;

    class LoadFontCallback : public RefCounted<LoadFontCallback> {
    public:
        virtual ~LoadFontCallback() { }
        virtual void notifyLoaded(FontFace*) = 0;
        virtual void notifyError(FontFace*) = 0;
    };
    void loadWithCallback(PassRefPtr<LoadFontCallback>);

private:
    FontFace();
    FontFace(const AtomicString& family);

    void initCSSFontFace(Document*, PassRefPtr<CSSValue> src);
    void initCSSFontFace(const unsigned char* data, unsigned size);
    void setPropertyFromString(const Document*, const String&, CSSPropertyID, ExceptionState* = 0);
    bool setPropertyFromStyle(const StylePropertySet&, CSSPropertyID);
    bool setPropertyValue(PassRefPtr<CSSValue>, CSSPropertyID);
    bool setFamilyValue(CSSValueList*);

    AtomicString m_family;
    RefPtr<CSSValue> m_src;
    RefPtr<CSSValue> m_style;
    RefPtr<CSSValue> m_weight;
    RefPtr<CSSValue> m_stretch;
    RefPtr<CSSValue> m_unicodeRange;
    RefPtr<CSSValue> m_variant;
    RefPtr<CSSValue> m_featureSettings;
    LoadStatus m_status;
    RefPtr<DOMException> m_error;

    OwnPtr<CSSFontFace> m_cssFontFace;
    Vector<RefPtr<LoadFontCallback> > m_callbacks;
};

typedef Vector<RefPtr<FontFace> > FontFaceArray;

} // namespace blink

#endif  // SKY_ENGINE_CORE_CSS_FONTFACE_H_
