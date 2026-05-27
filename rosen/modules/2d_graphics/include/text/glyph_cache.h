/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GLYPH_CACHE_H
#define GLYPH_CACHE_H

#include <unordered_map>
#include <list>
#include <mutex>
#include <cstdint>
#include <map>
#include "text/font.h"

namespace OHOS::Rosen::Drawing {

using OrderedFontFeatures = std::map<std::string, double>;

struct GlyphCacheKey {
    int32_t unicode;
    uint32_t typefaceHash;
    OrderedFontFeatures fontFeatures;

    bool operator==(const GlyphCacheKey& other) const
    {
        return unicode == other.unicode &&
               typefaceHash == other.typefaceHash &&
               fontFeatures == other.fontFeatures;
    }
};

struct GlyphCacheKeyHash {
    size_t operator()(const GlyphCacheKey& key) const
    {
        size_t h = std::hash<int32_t>()(key.unicode) ^
                   (std::hash<uint32_t>()(key.typefaceHash) << 1);
        for (const auto& [name, value] : key.fontFeatures) {
            h ^= std::hash<std::string>()(name) ^ std::hash<double>()(value);
        }
        return h;
    }
};

class GlyphCache {
public:
    static GlyphCache& Instance();

    bool Get(const GlyphCacheKey& key, uint16_t& value);
    void Put(const GlyphCacheKey& key, uint16_t value);
    void Clear();
    void SetMaxSize(uint32_t maxSize);
    static OrderedFontFeatures MergeFontFeatures(const DrawingFontFeatures& features);

private:
    GlyphCache() = default;
    ~GlyphCache() = default;
    GlyphCache(const GlyphCache&) = delete;
    GlyphCache& operator=(const GlyphCache&) = delete;

    std::mutex mutex_;
    uint32_t maxSize_ = 5000;

    using CacheEntry = std::pair<GlyphCacheKey, uint16_t>;
    std::list<CacheEntry> lruList_;
    std::unordered_map<GlyphCacheKey, std::list<CacheEntry>::iterator, GlyphCacheKeyHash> cacheMap_;

    void MoveToHead(std::list<CacheEntry>::iterator it);
    void EvictTail();
};

} // namespace OHOS::Rosen::Drawing
#endif // GLYPH_CACHE_H