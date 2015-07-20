/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
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

#ifndef SKY_ENGINE_CORE_DOM_USERACTIONELEMENTSET_H_
#define SKY_ENGINE_CORE_DOM_USERACTIONELEMENTSET_H_

#include "sky/engine/platform/heap/Handle.h"
#include "sky/engine/wtf/HashMap.h"
#include "sky/engine/wtf/PassOwnPtr.h"
#include "sky/engine/wtf/RefPtr.h"

namespace blink {

class Node;
class Element;

class UserActionElementSet final {
    DISALLOW_ALLOCATION();
public:
    bool isActive(const Node* node) { return hasFlags(node, IsActiveFlag); }
    bool isInActiveChain(const Node* node) { return hasFlags(node, InActiveChainFlag); }
    bool isHovered(const Node* node) { return hasFlags(node, IsHoveredFlag); }
    void setActive(Node* node, bool enable) { setFlags(node, enable, IsActiveFlag); }
    void setInActiveChain(Node* node, bool enable) { setFlags(node, enable, InActiveChainFlag); }
    void setHovered(Node* node, bool enable) { setFlags(node, enable, IsHoveredFlag); }

    UserActionElementSet();
    ~UserActionElementSet();

    void didDetach(Node*);

#if !ENABLE(OILPAN)
    void documentDidRemoveLastRef();
#endif

private:
    enum ElementFlags {
        IsActiveFlag      = 1 ,
        InActiveChainFlag = 1 << 1,
        IsHoveredFlag     = 1 << 2,
    };

    void setFlags(Node* node, bool enable, unsigned flags) { enable ? setFlags(node, flags) : clearFlags(node, flags); }
    void setFlags(Node*, unsigned);
    void clearFlags(Node*, unsigned);
    bool hasFlags(const Node*, unsigned flags) const;

    void setFlags(Element*, unsigned);
    void clearFlags(Element*, unsigned);
    bool hasFlags(const Element*, unsigned flags) const;

    typedef HashMap<RefPtr<Element>, unsigned> ElementFlagMap;
    ElementFlagMap m_elements;
};

} // namespace

#endif  // SKY_ENGINE_CORE_DOM_USERACTIONELEMENTSET_H_
