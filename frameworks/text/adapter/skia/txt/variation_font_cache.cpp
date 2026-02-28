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

#include "variation_font_cache.h"

#include "text/typeface.h"
#include "utils/text_log.h"

namespace txt {
constexpr size_t MAX_CACHE_SIZE = 32;

VariationFontCache::~VariationFontCache()
{
    for (const auto& entry : cacheList_) {
        NotifyCacheRemoved(entry.variationUniqueId);
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    cacheList_.clear();
    cacheIndex_.clear();
}

void VariationFontCache::SetCacheRemoveCallback(CacheRemoveCallback callback)
{
    cacheRemoveCallback_ = std::move(callback);
}

void VariationFontCache::NotifyCacheRemoved(uint32_t variationUniqueId)
{
    if (cacheRemoveCallback_ != nullptr && variationUniqueId != 0) {
        cacheRemoveCallback_(variationUniqueId);
    }
}

uint32_t VariationFontCache::CalculateHashKey(uint32_t originalUniqueId,
                                              const skia::textlayout::FontArguments& fontArgs) const
{
    // Combine originalUniqueId with fontArgs to create a unique hash key
    uint32_t hash = 0x9e3779b9;
    // Both 6 and 2 are interference positions
    hash ^= originalUniqueId + (hash << 6) + (hash >> 2);
    // Both 6 and 2 are interference positions
    hash ^= std::hash<skia::textlayout::FontArguments>()(fontArgs) + (hash << 6) + (hash >> 2);
    return hash;
}

uint32_t VariationFontCache::EvictOldest()
{
    // This method must be called with mutex already locked
    if (cacheList_.empty()) {
        return 0;
    }

    // Find the entry with the oldest lastAccessTime
    auto oldestIt = cacheList_.end();
    uint64_t oldestTime = UINT64_MAX;

    for (auto it = cacheList_.begin(); it != cacheList_.end(); ++it) {
        if (it->lastAccessTime < oldestTime) {
            oldestTime = it->lastAccessTime;
            oldestIt = it;
        }
    }

    uint32_t needFreeUniqueId = 0;
    if (oldestIt != cacheList_.end()) {
        needFreeUniqueId = oldestIt->variationUniqueId;
        cacheIndex_.erase(oldestIt->hashKey);
        cacheList_.erase(oldestIt);
    }
    return needFreeUniqueId;
}

std::shared_ptr<RSTypeface> VariationFontCache::RegisterVariationTypeface(
    std::shared_ptr<RSTypeface> typeface,
    const std::optional<skia::textlayout::FontArguments>& fontArgs)
{
    if (typeface == nullptr || !fontArgs.has_value()) {
        return typeface;
    }

    uint32_t needNotifyCacheId = 0;
    std::shared_ptr<RSTypeface> variationTypeface = nullptr;
    uint32_t originalUniqueId = typeface->GetUniqueID();
    uint32_t hashKey = CalculateHashKey(originalUniqueId, fontArgs.value());

    {
        std::unique_lock<std::shared_mutex> uniqueLock(mutex_);
        auto cacheIt = cacheIndex_.find(hashKey);
        if (cacheIt != cacheIndex_.end()) {
            cacheIt->second->lastAccessTime = ++accessCounter_;
            TEXT_LOGI_LIMIT3_MIN("Find variation font cache, originalUniqueId: %{public}u", originalUniqueId);
            return cacheIt->second->variationTypeface;
        }

        variationTypeface = fontArgs->CloneTypeface(typeface);
        if (variationTypeface == nullptr) {
            TEXT_LOGE("Failed to clone variation typeface, originalUniqueId: %{public}u", originalUniqueId);
            return typeface;
        }

        uint32_t variationUniqueId = variationTypeface->GetUniqueID();
        variationTypeface->SetFd(typeface->GetFd());
        variationTypeface->SetRawUniqueId(originalUniqueId);
        auto& registerCallback = RSDrawing::Typeface::GetTypefaceRegisterCallBack();
        if (registerCallback != nullptr) {
            registerCallback(variationTypeface);
        }

        // Evict oldest entry if cache is full
        if (cacheList_.size() >= MAX_CACHE_SIZE) {
            needNotifyCacheId = EvictOldest();
            TEXT_LOGD("EvictOldest typeface");
        }

        CacheEntry entry;
        entry.originalUniqueId = originalUniqueId;
        entry.variationUniqueId = variationUniqueId;
        entry.hashKey = hashKey;
        entry.variationTypeface = variationTypeface;
        entry.lastAccessTime = ++accessCounter_;

        cacheList_.push_back(entry);
        cacheIndex_[hashKey] = std::prev(cacheList_.end());
    }
    NotifyCacheRemoved(needNotifyCacheId);
    return variationTypeface;
}

void VariationFontCache::RemoveByOriginalUniqueId(uint32_t originalUniqueId)
{
    std::vector<uint32_t> freeCacheList;
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = cacheList_.begin();
        while (it != cacheList_.end()) {
            if (it->originalUniqueId == originalUniqueId) {
                // record variationUniqueId to be notified
                freeCacheList.push_back(it->variationUniqueId);
                cacheIndex_.erase(it->hashKey);
                it = cacheList_.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto variationUniqueId : freeCacheList) {
        NotifyCacheRemoved(variationUniqueId);
    }
}
} // namespace txt
