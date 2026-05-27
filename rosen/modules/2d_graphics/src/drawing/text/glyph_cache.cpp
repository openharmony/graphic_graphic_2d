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

#include "text/glyph_cache.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS::Rosen::Drawing {

GlyphCache& GlyphCache::Instance()
{
    static GlyphCache instance;
    return instance;
}

bool GlyphCache::Get(const GlyphCacheKey& key, uint16_t& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cacheMap_.find(key);
    if (it != cacheMap_.end()) {
        value = it->second->second;
        MoveToHead(it->second);
        return true;
    }
    return false;
}

void GlyphCache::Put(const GlyphCacheKey& key, uint16_t value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (maxSize_ == 0) {
        return;
    }
    
    auto it = cacheMap_.find(key);
    if (it != cacheMap_.end()) {
        it->second->second = value;
        MoveToHead(it->second);
        return;
    }

    if (cacheMap_.size() >= maxSize_) {
        EvictTail();
    }

    lruList_.emplace_front(key, value);
    cacheMap_.emplace(key, lruList_.begin());
}

void GlyphCache::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMap_.clear();
    lruList_.clear();
}

void GlyphCache::SetMaxSize(uint32_t maxSize)
{
    std::lock_guard<std::mutex> lock(mutex_);
    maxSize_ = maxSize;
    while (cacheMap_.size() > maxSize_) {
        EvictTail();
    }
}

void GlyphCache::MoveToHead(std::list<CacheEntry>::iterator it)
{
    lruList_.splice(lruList_.begin(), lruList_, it);
}

void GlyphCache::EvictTail()
{
    if (lruList_.empty()) {
        return;
    }
    CacheEntry tail = lruList_.back();
    cacheMap_.erase(tail.first);
    lruList_.pop_back();
}

OrderedFontFeatures GlyphCache::MergeFontFeatures(const DrawingFontFeatures& features)
{
    OrderedFontFeatures merged;
    for (const auto& featureMap : features) {
        for (const auto& [name, value] : featureMap) {
            merged[name] = value;
        }
    }
    return merged;
}

} // namespace OHOS::Rosen::Drawing