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

#define LOG_TAG "Minikin"
#include <cutils/log.h>

#include "minikin/WordBreaker.h"

#include <unicode/uchar.h>
#include <unicode/utf16.h>

namespace android {

const uint32_t CHAR_SOFT_HYPHEN = 0x00AD;

void WordBreaker::setLocale(const icu::Locale& locale) {
    UErrorCode status = U_ZERO_ERROR;
    mBreakIterator.reset(icu::BreakIterator::createLineInstance(locale, status));
    // TODO: handle failure status
    if (mText != nullptr) {
        mBreakIterator->setText(&mUText, status);
    }
    mIteratorWasReset = true;
}

void WordBreaker::setText(const uint16_t* data, size_t size) {
    mText = data;
    mTextSize = size;
    mIteratorWasReset = false;
    mLast = 0;
    mCurrent = 0;
    UErrorCode status = U_ZERO_ERROR;
    utext_openUChars(&mUText, data, size, &status);
    mBreakIterator->setText(&mUText, status);
    mBreakIterator->first();
}

ssize_t WordBreaker::current() const {
    return mCurrent;
}

ssize_t WordBreaker::next() {
    int32_t result;
    mLast = mCurrent;
    do {
        if (mIteratorWasReset) {
            result = mBreakIterator->following(mCurrent);
            mIteratorWasReset = false;
        } else {
            result = mBreakIterator->next();
        }
    } while (result != icu::BreakIterator::DONE && (size_t)result != mTextSize
             && mText[result - 1] == CHAR_SOFT_HYPHEN);
    mCurrent = (ssize_t)result;
    return mCurrent;
}

ssize_t WordBreaker::wordStart() const {
    ssize_t result = mLast;
    while (result < mCurrent) {
        UChar32 c;
        ssize_t ix = result;
        U16_NEXT(mText, ix, mCurrent, c);
        int32_t lb = u_getIntPropertyValue(c, UCHAR_LINE_BREAK);
        // strip leading punctuation, defined as OP and QU line breaking classes,
        // see UAX #14
        if (!(lb == U_LB_OPEN_PUNCTUATION || lb == U_LB_QUOTATION)) {
            break;
        }
        result = ix;
    }
    return result;
}

ssize_t WordBreaker::wordEnd() const {
    ssize_t result = mCurrent;
    while (result > mLast) {
        UChar32 c;
        ssize_t ix = result;
        U16_PREV(mText, mLast, ix, c);
        int32_t gc_mask = U_GET_GC_MASK(c);
        // strip trailing space and punctuation
        if ((gc_mask & (U_GC_ZS_MASK | U_GC_P_MASK)) == 0) {
            break;
        }
        result = ix;
    }
    return result;
}

void WordBreaker::finish() {
    mText = nullptr;
    // Note: calling utext_close multiply is safe
    utext_close(&mUText);
}

}  // namespace android
