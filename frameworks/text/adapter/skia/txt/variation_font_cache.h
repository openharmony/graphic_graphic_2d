/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_VARIATION_FONT_CACHE_H
#define ROSEN_MODULES_SPTEXT_VARIATION_FONT_CACHE_H

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

#include "modules/skparagraph/include/FontArguments.h"
#include "txt/asset_font_manager.h"

namespace txt {
class VariationFontCache {
public:
    using CacheRemoveCallback = std::function<void(uint32_t)>;

    struct CacheEntry {
        uint32_t originalUniqueId{0};
        uint32_t variationUniqueId{0};
        uint32_t hashKey{0};
        std::shared_ptr<RSTypeface> variationTypeface{nullptr};
        uint64_t lastAccessTime{0};
    };

    VariationFontCache() = default;
    ~VariationFontCache();

    void SetCacheRemoveCallback(CacheRemoveCallback callback);

    /**
     * @brief Register or retrieve a variation typeface from cache
     * @param typeface Original typeface
     * @param fontArgs Font arguments containing variation coordinates
     * @return Registered variation typeface, or original typeface on failure
     */
    std::shared_ptr<RSTypeface> RegisterVariationTypeface(
        std::shared_ptr<RSTypeface> typeface,
        const std::optional<skia::textlayout::FontArguments>& fontArgs);

    /**
     * @brief Remove all cached variations for a given original uniqueId
     * @param originalUniqueId The original typeface's uniqueId
     */
    void RemoveByOriginalUniqueId(uint32_t originalUniqueId);
private:
    VariationFontCache(const VariationFontCache&) = delete;
    VariationFontCache& operator=(const VariationFontCache&) = delete;

    uint32_t CalculateHashKey(uint32_t originalUniqueId,
                              const skia::textlayout::FontArguments& fontArgs) const;
    uint32_t EvictOldest();

    // Must be called with mutex already unlocked
    void NotifyCacheRemoved(uint32_t variationUniqueId);

    std::list<CacheEntry> cacheList_;
    std::unordered_map<uint32_t, std::list<CacheEntry>::iterator> cacheIndex_;
    uint64_t accessCounter_{0};
    CacheRemoveCallback cacheRemoveCallback_{nullptr};
    mutable std::shared_mutex mutex_;
};

} // namespace txt

#endif // ROSEN_MODULES_SPTEXT_VARIATION_FONT_CACHE_H
