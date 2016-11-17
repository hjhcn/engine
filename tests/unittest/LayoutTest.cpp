/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "ICUTestBase.h"
#include "minikin/FontCollection.h"
#include "minikin/Layout.h"
#include "../util/FontTestUtils.h"
#include "../util/UnicodeUtils.h"

const char* SYSTEM_FONT_PATH = "/system/fonts/";
const char* SYSTEM_FONT_XML = "/system/etc/fonts.xml";

namespace minikin {

const float UNTOUCHED_MARKER = 1e+38;

static void expectAdvances(std::vector<float> expected, float* advances, size_t length) {
    EXPECT_LE(expected.size(), length);
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], advances[i])
                << i << "th element is different. Expected: " << expected[i]
                << ", Actual: " << advances[i];
    }
    EXPECT_EQ(UNTOUCHED_MARKER, advances[expected.size()]);
}

static void resetAdvances(float* advances, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        advances[i] = UNTOUCHED_MARKER;
    }
}

class LayoutTest : public ICUTestBase {
protected:
    LayoutTest() : mCollection(nullptr) {
    }

    virtual ~LayoutTest() {}

    virtual void SetUp() override {
        mCollection = getFontCollection(SYSTEM_FONT_PATH, SYSTEM_FONT_XML);
    }

    virtual void TearDown() override {
        mCollection->Unref();
    }

    FontCollection* mCollection;
};

TEST_F(LayoutTest, doLayoutTest) {
    MinikinPaint paint;
    MinikinRect rect;
    const size_t kMaxAdvanceLength = 32;
    float advances[kMaxAdvanceLength];
    std::vector<float> expectedValues;

    Layout layout;
    layout.setFontCollection(mCollection);
    std::vector<uint16_t> text;

    // The mock implementation returns 10.0f advance and 0,0-10x10 bounds for all glyph.
    {
        SCOPED_TRACE("one word");
        text = utf8ToUtf16("oneword");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(70.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(70.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two words");
        text = utf8ToUtf16("two words");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(90.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(90.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("three words");
        text = utf8ToUtf16("three words test");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(160.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(160.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two spaces");
        text = utf8ToUtf16("two  spaces");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(110.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(110.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
}

TEST_F(LayoutTest, doLayoutTest_wordSpacing) {
    MinikinPaint paint;
    MinikinRect rect;
    const size_t kMaxAdvanceLength = 32;
    float advances[kMaxAdvanceLength];
    std::vector<float> expectedValues;
    std::vector<uint16_t> text;

    Layout layout;
    layout.setFontCollection(mCollection);

    paint.wordSpacing = 5.0f;

    // The mock implementation returns 10.0f advance and 0,0-10x10 bounds for all glyph.
    {
        SCOPED_TRACE("one word");
        text = utf8ToUtf16("oneword");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(70.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(70.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two words");
        text = utf8ToUtf16("two words");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(95.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(95.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        EXPECT_EQ(UNTOUCHED_MARKER, advances[text.size()]);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[3] = 15.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("three words test");
        text = utf8ToUtf16("three words test");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(170.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(170.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[5] = 15.0f;
        expectedValues[11] = 15.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two spaces");
        text = utf8ToUtf16("two  spaces");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(120.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(120.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[3] = 15.0f;
        expectedValues[4] = 15.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
}

TEST_F(LayoutTest, doLayoutTest_negativeWordSpacing) {
    MinikinPaint paint;
    MinikinRect rect;
    const size_t kMaxAdvanceLength = 32;
    float advances[kMaxAdvanceLength];
    std::vector<float> expectedValues;

    Layout layout;
    layout.setFontCollection(mCollection);
    std::vector<uint16_t> text;

    // Negative word spacing also should work.
    paint.wordSpacing = -5.0f;

    {
        SCOPED_TRACE("one word");
        text = utf8ToUtf16("oneword");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(70.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(70.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two words");
        text = utf8ToUtf16("two words");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(85.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(85.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[3] = 5.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("three words");
        text = utf8ToUtf16("three word test");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(140.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(140.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[5] = 5.0f;
        expectedValues[10] = 5.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
    {
        SCOPED_TRACE("two spaces");
        text = utf8ToUtf16("two  spaces");
        layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR, FontStyle(), paint);
        EXPECT_EQ(100.0f, layout.getAdvance());
        layout.getBounds(&rect);
        EXPECT_EQ(0.0f, rect.mLeft);
        EXPECT_EQ(0.0f, rect.mTop);
        EXPECT_EQ(100.0f, rect.mRight);
        EXPECT_EQ(10.0f, rect.mBottom);
        resetAdvances(advances, kMaxAdvanceLength);
        layout.getAdvances(advances);
        expectedValues.resize(text.size());
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            expectedValues[i] = 10.0f;
        }
        expectedValues[3] = 5.0f;
        expectedValues[4] = 5.0f;
        expectAdvances(expectedValues, advances, kMaxAdvanceLength);
    }
}

// TODO: Add more test cases, e.g. measure text, letter spacing.

}  // namespace minikin
