/*
 * Copyright (C) 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 David Smith <catfish.man@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SKY_ENGINE_CORE_DOM_ELEMENTRAREDATA_H_
#define SKY_ENGINE_CORE_DOM_ELEMENTRAREDATA_H_

#include "sky/engine/core/css/PropertySetCSSStyleDeclaration.h"
#include "sky/engine/core/dom/DOMTokenList.h"
#include "sky/engine/core/dom/NodeRareData.h"
#include "sky/engine/core/rendering/style/StyleInheritedData.h"
#include "sky/engine/wtf/OwnPtr.h"

namespace blink {

class HTMLElement;

class ElementRareData : public NodeRareData {
public:
    static ElementRareData* create(RenderObject* renderer)
    {
        return new ElementRareData(renderer);
    }

    short tabIndex() const { return m_tabindex; }
    bool hasTabIndex() const { return m_hasTabIndex; }

    void setTabIndex(short index)
    {
        m_tabindex = index;
        m_hasTabIndex = true;
    }

    void clearTabIndex()
    {
        m_tabindex = 0;
        m_hasTabIndex = false;
    }

    CSSStyleDeclaration& ensureInlineCSSStyleDeclaration(Element* ownerElement);

    RenderStyle* computedStyle() const { return m_computedStyle.get(); }
    void setComputedStyle(PassRefPtr<RenderStyle> computedStyle) { m_computedStyle = computedStyle; }
    void clearComputedStyle() { m_computedStyle = nullptr; }

    DOMTokenList* classList() const { return m_classList.get(); }
    void setClassList(PassOwnPtr<DOMTokenList> classList) { m_classList = classList; }

private:
    unsigned m_tabindex : 16;
    unsigned m_hasTabIndex : 1;

    OwnPtr<DOMTokenList> m_classList;
    OwnPtr<InlineCSSStyleDeclaration> m_cssomWrapper;

    RefPtr<RenderStyle> m_computedStyle;

    explicit ElementRareData(RenderObject*);
};

inline ElementRareData::ElementRareData(RenderObject* renderer)
    : NodeRareData(renderer)
    , m_tabindex(0)
    , m_hasTabIndex(false)
{
    m_isElementRareData = true;
}

} // namespace

#endif  // SKY_ENGINE_CORE_DOM_ELEMENTRAREDATA_H_
