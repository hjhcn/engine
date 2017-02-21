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

#include "HbFontCache.h"

#include <log/log.h>
#include <utils/LruCache.h>

#include <hb.h>
#include <hb-ot.h>

#include <minikin/MinikinFont.h>
#include "MinikinInternal.h"

namespace minikin {

class HbFontCache : private android::OnEntryRemoved<int32_t, hb_font_t*> {
public:
    HbFontCache() : mCache(kMaxEntries) {
        mCache.setOnEntryRemovedListener(this);
    }

    // callback for OnEntryRemoved
    void operator()(int32_t& /* key */, hb_font_t*& value) {
        hb_font_destroy(value);
    }

    hb_font_t* get(int32_t fontId) {
        return mCache.get(fontId);
    }

    void put(int32_t fontId, hb_font_t* font) {
        mCache.put(fontId, font);
    }

    void clear() {
        mCache.clear();
    }

    void remove(int32_t fontId) {
        mCache.remove(fontId);
    }

private:
    static const size_t kMaxEntries = 100;

    android::LruCache<int32_t, hb_font_t*> mCache;
};

class HbFontCacheHolder {
public:
    HbFontCacheHolder() {}

    hb_font_t* getCachedFont(const MinikinFont* minikinFont) {
        assertLocked(gLock);

        // TODO: get rid of nullFaceFont
        static hb_font_t* nullFaceFont = nullptr;
        if (minikinFont == nullptr) {
            if (nullFaceFont == nullptr) {
                nullFaceFont = hb_font_create(nullptr);
            }
            return hb_font_reference(nullFaceFont);
        }

        const int32_t fontId = minikinFont->GetUniqueId();
        hb_font_t* font = mFontCache.get(fontId);
        if (font != nullptr) {
            return hb_font_reference(font);
        }

        hb_face_t* face;
        const void* buf = minikinFont->GetFontData();
        size_t size = minikinFont->GetFontSize();
        hb_blob_t* blob = hb_blob_create(reinterpret_cast<const char*>(buf), size,
                HB_MEMORY_MODE_READONLY, nullptr, nullptr);
        face = hb_face_create(blob, minikinFont->GetFontIndex());
        hb_blob_destroy(blob);

        hb_font_t* parent_font = hb_font_create(face);
        hb_ot_font_set_funcs(parent_font);

        unsigned int upem = hb_face_get_upem(face);
        hb_font_set_scale(parent_font, upem, upem);

        font = hb_font_create_sub_font(parent_font);
        hb_font_destroy(parent_font);
        hb_face_destroy(face);
        mFontCache.put(fontId, font);
        return hb_font_reference(font);
    }

    void clearFontCache() {
        assertLocked(gLock);
        mFontCache.clear();
    }

    void removeFont(const MinikinFont* minikinFont) {
        assertLocked(gLock);
        mFontCache.remove(minikinFont->GetUniqueId());
    }

private:
    HbFontCache mFontCache;
};

static HbFontCacheHolder& getInstance() {
    static HbFontCacheHolder instance;
    return instance;
}

void purgeHbFontCacheLocked() {
    getInstance().clearFontCache();
}

void purgeHbFontLocked(const MinikinFont* minikinFont) {
    getInstance().removeFont(minikinFont);
}

// Returns a new reference to a hb_font_t object, caller is
// responsible for calling hb_font_destroy() on it.
hb_font_t* getHbFontLocked(const MinikinFont* minikinFont) {
    return getInstance().getCachedFont(minikinFont);
}

}  // namespace minikin
