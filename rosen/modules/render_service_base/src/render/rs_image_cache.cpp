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

#include "memory/rs_memory_snapshot.h"
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

void RSImageCache::CacheDrawingImage(uint64_t uniqueId, std::shared_ptr<Drawing::Image> img)
{
    if (img && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        drawingImageCache_.emplace(uniqueId, std::make_pair(img, 0));
    }
}

std::shared_ptr<Drawing::Image> RSImageCache::GetDrawingImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}

void RSImageCache::IncreaseDrawingImageCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        it->second.second++;
    }
}

void RSImageCache::ReleaseDrawingImageCache(uint64_t uniqueId)
{
    // release the Drawing::Image if no RSImage holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        it->second.second--;
        if (it->second.first == nullptr || it->second.second == 0) {
            drawingImageCache_.erase(it);
        }
    }
}

void RSImageCache::CachePixelMap(uint64_t uniqueId, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (pixelMap && uniqueId > 0) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pixelMapCache_.emplace(uniqueId, std::make_pair(pixelMap, 0));
        }
        auto type = pixelMap->GetAllocatorType();
        pid_t pid = uniqueId >> 32; // right shift 32 bit to restore pid
        if (type != Media::AllocatorType::DMA_ALLOC && pid) {
            auto realSize = type == Media::AllocatorType::SHARE_MEM_ALLOC
                ? pixelMap->GetCapacity() / 2 // rs only counts half of the SHARE_MEM_ALLOC memory
                : pixelMap->GetCapacity();
            MemorySnapshot::Instance().AddCpuMemory(pid, realSize);
        }
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

void RSImageCache::CollectUniqueId(uint64_t uniqueId)
{
    std::unique_lock<std::mutex> lock(uniqueIdListMutex_);
    uniqueIdList_.push_back(uniqueId);
}

void RSImageCache::ReleaseUniqueIdList()
{
    std::list<uint64_t> clearList;
    {
        std::unique_lock<std::mutex> lock(uniqueIdListMutex_);
        uniqueIdList_.swap(clearList);
    }
    for (const auto& uniqueId : clearList) {
        ReleasePixelMapCache(uniqueId);
    }
}

bool RSImageCache::CheckUniqueIdIsEmpty()
{
    if (uniqueIdListMutex_.try_lock()) {
        if (uniqueIdList_.empty()) {
            uniqueIdListMutex_.unlock();
            return true;
        }
        uniqueIdListMutex_.unlock();
    }
    return false;
}

void RSImageCache::ReleasePixelMapCache(uint64_t uniqueId)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    {
        // release the pixelMap if no RSImage holds it
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pixelMapCache_.find(uniqueId);
        if (it != pixelMapCache_.end()) {
            it->second.second--;
            if (it->second.first == nullptr || it->second.second == 0) {
                pixelMap = it->second.first;
                bool shouldCount = pixelMap && pixelMap->GetAllocatorType() != Media::AllocatorType::DMA_ALLOC;
                pid_t pid = uniqueId >> 32; // right shift 32 bit to restore pid
                if (shouldCount && pid) {
                    auto realSize = pixelMap->GetAllocatorType() == Media::AllocatorType::SHARE_MEM_ALLOC
                        ? pixelMap->GetCapacity() / 2 // rs only counts half of the SHARE_MEM_ALLOC memory
                        : pixelMap->GetCapacity();
                    MemorySnapshot::Instance().RemoveCpuMemory(pid, realSize);
                }
                pixelMapCache_.erase(it);
                ReleaseDrawingImageCacheByPixelMapId(uniqueId);
            }
        } else {
            ReleaseDrawingImageCacheByPixelMapId(uniqueId);
        }
    }
    pixelMap.reset();
}

bool RSImageCache::CheckRefCntAndReleaseImageCache(
    uint64_t uniqueId, std::shared_ptr<Media::PixelMap>& pixelMapIn, const std::shared_ptr<Drawing::Image>& image)
{
    if (!pixelMapIn) {
        return false;
    }
    auto imageCount = image.use_count();
    auto pixelMapCount = pixelMapIn.use_count();
    /*
     * Image will at least be hold by RSImage, RSImageCache.pixelMapIdRelatedDrawingImageCache_
     * PixelMap will be at least be hold by RSImage, RSImageCache.pixelMapCache_, Image.ReleaseContext
    */
    if (!(imageCount = 2 && pixelMapCount == 3) && !(imageCount = 0 && pixelMapCount == 2)) {
        return false;
    }
    {
        // release the pixelMap if no RSImage holds it
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pixelMapCache_.find(uniqueId);
        if (it == pixelMapCache_.end()) {
            return false;
        }
        if (it->second.first != pixelMapIn) {
            return false; // skip purge if pixelMap mismatch
        }
        ReleaseDrawingImageCacheByPixelMapId(uniqueId);
        imageCount = image.use_count();
        pixelMapCount = pixelMapIn.use_count();
#ifdef ROSEN_OHOS
        /*
        * Image will only be hold by RSImage
        * PixelMap will be be hold by RSImage, RSImageCache.pixelMapCache_, Image.ReleaseContext
        */
        if ((imageCount = 1 && pixelMapCount == 3) || (imageCount = 0 && pixelMapCount == 2)) {
            pixelMapIn->UnMap();
        }
#endif
    }
    return true;
}

void RSImageCache::CacheRenderDrawingImageByPixelMapId(uint64_t uniqueId,
    std::shared_ptr<Drawing::Image> img, pid_t tid)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedDrawingImageCache_[uniqueId][tid] = img;
    }
}

std::shared_ptr<Drawing::Image> RSImageCache::GetRenderDrawingImageCacheByPixelMapId(uint64_t uniqueId, pid_t tid) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedDrawingImageCache_.end()) {
        auto innerIt = it->second.find(tid);
        if (innerIt != it->second.end()) {
            return innerIt->second;
        }
    }
    return nullptr;
}

void RSImageCache::ReleaseDrawingImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedDrawingImageCache_.end()) {
        pixelMapIdRelatedDrawingImageCache_.erase(it);
    }
}
} // namespace Rosen
} // namespace OHOS
