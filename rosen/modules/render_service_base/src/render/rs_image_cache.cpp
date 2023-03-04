/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_image_cache.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
// modify the RSImageCache instance as global to extend life cycle, fix destructor crash
static RSImageCache gRSImageCacheInstance;

RSImageCache& RSImageCache::Instance()
{
    return gRSImageCacheInstance;
}

void RSImageCache::CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (img && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        skiaImageCache_.emplace(uniqueId, std::make_pair(img, 0));
    }
}

sk_sp<SkImage> RSImageCache::GetSkiaImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}

void RSImageCache::IncreaseSkiaImageCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        it->second.second++;
    }
}

void RSImageCache::ReleaseSkiaImageCache(uint64_t uniqueId)
{
    // release the skImage if no RSImage holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        it->second.second--;
        if (it->second.first == nullptr || it->second.second == 0) {
            skiaImageCache_.erase(it);
        }
    }
}

void RSImageCache::CachePixelMap(uint64_t uniqueId, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (pixelMap && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        pixelMapCache_.emplace(uniqueId, std::make_pair(pixelMap, 0));
    }
}

std::shared_ptr<Media::PixelMap> RSImageCache::GetPixelMapCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pixelMapCache_.find(uniqueId);
    if (it != pixelMapCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}

void RSImageCache::IncreasePixelMapCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pixelMapCache_.find(uniqueId);
    if (it != pixelMapCache_.end()) {
        it->second.second++;
    }
}

void RSImageCache::ReleasePixelMapCache(uint64_t uniqueId)
{
    // release the pixelMap if no RSImage holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pixelMapCache_.find(uniqueId);
    if (it != pixelMapCache_.end()) {
        it->second.second--;
        if (it->second.first == nullptr || it->second.second == 0) {
            pixelMapCache_.erase(it);
            ReleaseSkiaImageCacheByPixelMapId(uniqueId);
        }
    }
}

void RSImageCache::CacheRenderSkiaImageByPixelMapId(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedSkiaImageCache_.emplace(std::make_pair(uniqueId, img));
    }
}

sk_sp<SkImage> RSImageCache::GetRenderSkiaImageCacheByPixelMapId(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedSkiaImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}

void RSImageCache::ReleaseSkiaImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedSkiaImageCache_.end()) {
        pixelMapIdRelatedSkiaImageCache_.erase(it);
    }
}
} // namespace Rosen
} // namespace OHOS
