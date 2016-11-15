/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "MinikinFontForTest.h"

#include <minikin/MinikinFont.h>

#include <SkTypeface.h>

#include <cutils/log.h>

namespace minikin {

// static
MinikinFontForTest* MinikinFontForTest::createFromFile(const std::string& font_path) {
    sk_sp<SkTypeface> typeface = SkTypeface::MakeFromFile(font_path.c_str());
    MinikinFontForTest* font = new MinikinFontForTest(font_path, std::move(typeface));
    return font;
}

// static
MinikinFontForTest* MinikinFontForTest::createFromFileWithIndex(const std::string& font_path,
        int index) {
    sk_sp<SkTypeface> typeface = SkTypeface::MakeFromFile(font_path.c_str(), index);
    MinikinFontForTest* font = new MinikinFontForTest(font_path, std::move(typeface));
    return font;
}

MinikinFontForTest::MinikinFontForTest(const std::string& font_path, sk_sp<SkTypeface> typeface) :
        MinikinFont(typeface->uniqueID()),
        mTypeface(std::move(typeface)),
        mFontPath(font_path) {
}

float MinikinFontForTest::GetHorizontalAdvance(uint32_t /* glyph_id */,
        const MinikinPaint& /* paint */) const {
    // TODO: Make mock value configurable if necessary.
    return 10.0f;
}

void MinikinFontForTest::GetBounds(MinikinRect* bounds, uint32_t /* glyph_id */,
        const MinikinPaint& /* paint */) const {
    // TODO: Make mock values configurable if necessary.
    bounds->mLeft = 0.0f;
    bounds->mTop = 0.0f;
    bounds->mRight = 10.0f;
    bounds->mBottom = 10.0f;
}

const void* MinikinFontForTest::GetTable(uint32_t tag, size_t* size,
        MinikinDestroyFunc* destroy) {
    const size_t tableSize = mTypeface->getTableSize(tag);
    *size = tableSize;
    if (tableSize == 0) {
        return nullptr;
    }
    void* buf = malloc(tableSize);
    if (buf == nullptr) {
        return nullptr;
    }
    mTypeface->getTableData(tag, 0, tableSize, buf);
    *destroy = free;
    return buf;
}

}  // namespace minikin
