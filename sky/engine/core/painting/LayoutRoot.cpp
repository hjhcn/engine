// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/painting/LayoutRoot.h"

#include "sky/engine/core/dom/Document.h"
#include "sky/engine/core/dom/Element.h"
#include "sky/engine/core/frame/FrameView.h"
#include "sky/engine/core/frame/LocalFrame.h"
#include "sky/engine/core/frame/Settings.h"
#include "sky/engine/core/painting/Canvas.h"
#include "sky/engine/core/rendering/RenderView.h"
#include "sky/engine/platform/geometry/IntRect.h"
#include "third_party/skia/include/core/SkCanvas.h"

namespace blink {

PassRefPtr<LayoutRoot> LayoutRoot::create()
{
    return adoptRef(new LayoutRoot);
}

LayoutRoot::LayoutRoot()
    : m_minWidth(0)
    , m_maxWidth(0)
    , m_minHeight(0)
    , m_maxHeight(0)
{
    m_settings = Settings::create();
    m_frameHost = FrameHost::createDummy(m_settings.get());
    m_frame = LocalFrame::create(nullptr, m_frameHost.get());
    m_frame->createView(IntSize(), Color::white, false);
}

LayoutRoot::~LayoutRoot()
{
    if (!m_document->needsAttach())
        m_document->detach();
}

Element* LayoutRoot::rootElement() const
{
    if (!m_document)
        return nullptr;
    return m_document->firstElementChild();
}

void LayoutRoot::setRootElement(Element* root)
{
    m_document = &root->document();
    m_frame->setDocument(m_document.get());
    m_document->setFrame(m_frame.get());

    m_document->attach();
    m_document->setChild(root, ASSERT_NO_EXCEPTION);

    m_document->setFrame(nullptr);
    m_frame->setDocument(nullptr);
}

void LayoutRoot::layout()
{
    m_frame->setDocument(m_document.get());
    m_document->setFrame(m_frame.get());

    LayoutUnit maxWidth = std::max(m_minWidth, m_maxWidth);
    LayoutUnit maxHeight = std::max(m_minHeight, m_maxHeight);
    m_frame->view()->renderView()->setFrameViewSize(IntSize(maxWidth, maxHeight));

    m_document->updateLayout();

    m_document->setFrame(nullptr);
    m_frame->setDocument(nullptr);
}

void LayoutRoot::paint(Canvas* canvas)
{
    if (m_document && rootElement() && canvas && canvas->skCanvas())
        rootElement()->paint(canvas);
}

} // namespace blink
