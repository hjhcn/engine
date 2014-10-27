/*
 * Copyright (C) 2011, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/SelectorQuery.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/css/parser/BisonCSSParser.h"
#include "core/css/SelectorChecker.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/Node.h"
#include "core/dom/StaticNodeList.h"

namespace blink {

PassOwnPtr<SelectorQuery> SelectorQuery::adopt(CSSSelectorList& selectorList)
{
    return adoptPtr(new SelectorQuery(selectorList));
}

SelectorQuery::SelectorQuery(CSSSelectorList& selectorList)
{
    m_selectors.adopt(selectorList);
}

bool SelectorQuery::matches(Element& element) const
{
    return selectorMatches(element, element);
}

PassRefPtr<StaticElementList> SelectorQuery::queryAll(ContainerNode& rootNode) const
{
    Vector<RefPtr<Element> > result;
    for (Element* element = ElementTraversal::firstWithin(rootNode); element; element = ElementTraversal::next(*element, &rootNode)) {
        if (selectorMatches(rootNode, *element))
            result.append(element);
    }
    return StaticElementList::adopt(result);
}

PassRefPtr<Element> SelectorQuery::queryFirst(ContainerNode& rootNode) const
{
    for (Element* element = ElementTraversal::firstWithin(rootNode); element; element = ElementTraversal::next(*element, &rootNode)) {
        if (selectorMatches(rootNode, *element))
            return element;
    }
    return nullptr;
}

bool SelectorQuery::selectorMatches(ContainerNode& rootNode, Element& element) const
{
    SelectorChecker checker(element.document(), SelectorChecker::QueryingRules);
    for (const CSSSelector* selector = m_selectors.first(); selector; selector = CSSSelectorList::next(*selector)) {
        SelectorChecker::SelectorCheckingContext context(*selector, &element);
        context.scope = !rootNode.isDocumentNode() ? &rootNode : 0;
        if (context.scope)
            context.contextFlags = SelectorChecker::ScopeContainsLastMatchedElement;
        if (checker.match(context))
            return true;
    }
    return false;
}

SelectorQuery* SelectorQueryCache::add(const AtomicString& selectors, const Document& document, ExceptionState& exceptionState)
{
    HashMap<AtomicString, OwnPtr<SelectorQuery> >::iterator it = m_entries.find(selectors);
    if (it != m_entries.end())
        return it->value.get();

    BisonCSSParser parser(CSSParserContext(document, 0));
    CSSSelectorList selectorList;
    parser.parseSelector(selectors, selectorList);

    if (!selectorList.first()) {
        exceptionState.throwDOMException(SyntaxError, "'" + selectors + "' is not a valid selector.");
        return 0;
    }

    const unsigned maximumSelectorQueryCacheSize = 256;
    if (m_entries.size() == maximumSelectorQueryCacheSize)
        m_entries.remove(m_entries.begin());

    return m_entries.add(selectors, SelectorQuery::adopt(selectorList)).storedValue->value.get();
}

}
