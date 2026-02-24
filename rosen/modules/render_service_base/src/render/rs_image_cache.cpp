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

#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"
#include "memory/rs_memory_snapshot.h"
#include "render/rs_image_cache.h"
#include "rs_trace.h"
#include "pixel_map.h"
#include "params/rs_render_params.h"

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
    ReleaseEditablePixelMapCache();
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

bool RSImageCache::CheckRefCntAndReleaseImageCache(uint64_t uniqueId, std::shared_ptr<Media::PixelMap>& pixelMapIn)
{
    if (!pixelMapIn) {
        return false;
    }
    {
        // release the pixelMap if no RSImage holds it
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pixelMapCache_.find(uniqueId);
        if (it == pixelMapCache_.end()) {
            return false;
        }
        if (it->second.first != pixelMapIn || it->second.second > 1) {
            return false; // skip purge if pixelMap mismatch
        }
        ReleaseDrawingImageCacheByPixelMapId(uniqueId);
    }
    return true;
}

void RSImageCache::CacheEditablePixelMap(uint64_t uniqueId, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (pixelMap && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
        editablePixelMapCache_.emplace(uniqueId, std::make_pair(pixelMap, 0));
    }
}

std::shared_ptr<Media::PixelMap> RSImageCache::GetEditablePixelMapCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
    auto it = editablePixelMapCache_.find(uniqueId);
    if (it != editablePixelMapCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}

void RSImageCache::IncreaseEditablePixelMapCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
    auto it = editablePixelMapCache_.find(uniqueId);
    if (it != editablePixelMapCache_.end()) {
        it->second.second++;
    }
}

void RSImageCache::DiscardEditablePixelMapCache(uint64_t uniqueId)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
        auto it = editablePixelMapCache_.find(uniqueId);
        if (it == editablePixelMapCache_.end()) {
            return;
        }
        if (UNLIKELY(it->second.first == nullptr)) {
            editablePixelMapCache_.erase(it);
            return;
        }
        pixelMap = it->second.first;
        editablePixelMapCache_.erase(it);
    }
    {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheToReleaseMutex_);
        editablePixelMapCacheToRelease_.emplace_back(pixelMap);
    }
}

void RSImageCache::DecreaseRefCountAndDiscardEditablePixelMapCache(uint64_t uniqueId)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
        auto it = editablePixelMapCache_.find(uniqueId);
        if (it == editablePixelMapCache_.end()) {
            return;
        }
        if (UNLIKELY(it->second.first == nullptr)) {
            editablePixelMapCache_.erase(it);
            return;
        }
        if (--it->second.second == 0) {
            pixelMap = it->second.first;
            editablePixelMapCache_.erase(it);
        }
    }
    if (pixelMap != nullptr) {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheToReleaseMutex_);
        editablePixelMapCacheToRelease_.emplace_back(pixelMap);
    }
}

void RSImageCache::DecreaseRefCountAndReleaseEditablePixelMapCache(uint64_t uniqueId)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheMutex_);
        auto it = editablePixelMapCache_.find(uniqueId);
        if (it == editablePixelMapCache_.end()) {
            return;
        }
        if (UNLIKELY(it->second.first == nullptr)) {
            editablePixelMapCache_.erase(it);
            return;
        }
        if (--it->second.second == 0) {
            pixelMap = it->second.first;
            editablePixelMapCache_.erase(it);
        }
    }
    pixelMap.reset();
}

void RSImageCache::ReleaseEditablePixelMapCache()
{
    std::vector<std::shared_ptr<Media::PixelMap>> editablePixelMapCacheToRelease;
    {
        std::lock_guard<std::mutex> lock(editablePixelMapCacheToReleaseMutex_);
        std::swap(editablePixelMapCacheToRelease, editablePixelMapCacheToRelease_);
    }
    editablePixelMapCacheToRelease.clear();
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
        bool isEnable = RSImageDetailEnhancerThread::Instance().GetEnabled();
        if (isEnable && RSImageDetailEnhancerThread::Instance().GetScaledImage(uniqueId)) {
            RSImageDetailEnhancerThread::Instance().ReleaseScaledImage(uniqueId);
            RSImageDetailEnhancerThread::Instance().SetProcessStatus(uniqueId, false);
        }
    }
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSImageCache::ReserveImageInfo(std::shared_ptr<RSImage> rsImage,
    NodeId nodeId, std::weak_ptr<RSExtendImageObject> drawCmd)
{
    if (!RSSystemProperties::GetDefaultMemClearEnabled()) {
        return;
    }
    if (rsImage != nullptr) {
        auto drawableAdapter = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(nodeId);
        if (drawableAdapter == nullptr) {
            return;
        }
        NodeId surfaceNodeId = drawableAdapter->GetRenderParams()->GetFirstLevelNodeId();
        std::weak_ptr<RSImage> rsImage_weak = rsImage;
        rsImageInfoMap[surfaceNodeId].push_back(std::make_pair(rsImage_weak, drawCmd));
    }
}

void RSImageCache::RemoveImageMemForWindow(NodeId surfaceNodeId)
{
    if (!RSSystemProperties::GetDefaultMemClearEnabled()) {
        return;
    }
    RS_TRACE_NAME_FMT_DEBUG("", "RSImageCache::RemoveImageMemForWindow surfaceNodeId:%" PRIu64,
        surfaceNodeId);
    ImageContent& rsImageVec = rsImageInfoMap[surfaceNodeId];
    for (auto& [img_wptr, imageOp] : rsImageVec) {
        if (auto img = img_wptr.lock()) {
            ReleaseDrawingImageCacheByPixelMapId(img->GetUniqueId());
            ReleaseDrawingImageCache(img->GetUniqueId());
            img->Purge();
        }
        if (auto imgOP_ptr = imageOp.lock()) {
            imgOP_ptr->PurgeMipmapMem();
        }
    }
    rsImageInfoMap.erase(surfaceNodeId);
}
#endif
} // namespace Rosen
} // namespace OHOS
