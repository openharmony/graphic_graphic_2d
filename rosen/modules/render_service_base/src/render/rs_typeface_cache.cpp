/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <iterator>
#include <unistd.h>
#include "render/rs_typeface_cache.h"
#include "sandbox_utils.h"

// after 5 vsync count, destory it
#define DELAY_DESTROY_VSYNC_COUNT 5

namespace OHOS {
namespace Rosen {
// modify the RSTypefaceCache instance as global to extend life cycle, fix destructor crash
static RSTypefaceCache gRSTypefaceCacheInstance;

RSTypefaceCache& RSTypefaceCache::Instance()
{
    return gRSTypefaceCacheInstance;
}

uint64_t RSTypefaceCache::GenGlobalUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    return (shiftedPid | id);
}

pid_t RSTypefaceCache::GetTypefacePid(uint64_t uniqueId)
{
    // 32 for 64-bit unsignd number shift
    return static_cast<uint32_t>(uniqueId >> 32);
}

uint32_t RSTypefaceCache::GetTypefaceId(uint64_t uniqueId)
{
    return static_cast<uint32_t>(0xFFFFFFFF & uniqueId);
}

void RSTypefaceCache::CacheDrawingTypeface(uint64_t uniqueId,
    std::shared_ptr<Drawing::Typeface> typeface)
{
    if (typeface && uniqueId > 0) {
        // 32 for 64-bit unsignd number shift
        pid_t pid = static_cast<pid_t>(uniqueId >> 32);
        uint32_t typefaceId = static_cast<uint32_t>(0xFFFFFFFF & uniqueId);
        std::lock_guard<std::mutex> lock(mapMutex_);
        drawingTypefaceCache_[pid][typefaceId] = typeface;
    }
}

void RSTypefaceCache::RemoveDrawingTypefaceByGlobalUniqueId(uint64_t globalUniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    pid_t callingPid = GetTypefacePid(globalUniqueId);
    auto it = drawingTypefaceCache_.find(callingPid);
    if (it != drawingTypefaceCache_.end()) {
        auto innerIt = it->second.find(GetTypefaceId(globalUniqueId));
        if (innerIt != it->second.end()) {
            drawingTypefaceCache_[callingPid].erase(innerIt);
        }
    }
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::GetDrawingTypefaceCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = drawingTypefaceCache_.find(GetTypefacePid(uniqueId));
    if (it != drawingTypefaceCache_.end()) {
        auto innerIt = it->second.find(GetTypefaceId(uniqueId));
        if (innerIt != it->second.end()) {
            return innerIt->second;
        }
    }
    return nullptr;
}

void RSTypefaceCache::RemoveDrawingTypefacesByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = drawingTypefaceCache_.find(pid);
    if (it != drawingTypefaceCache_.end()) {
        drawingTypefaceCache_.erase(it);
    }
}
void RSTypefaceCache::AddDelayDestroyQueue(uint64_t globalUniqueId)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    delayDestroyTypefaces_.push_back({globalUniqueId, DELAY_DESTROY_VSYNC_COUNT});
}

void RSTypefaceCache::HandleDelayDestroyQueue()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    for (auto it = delayDestroyTypefaces_.begin(); it != delayDestroyTypefaces_.end();) {
        it->refCount--;
        if (it->refCount == 0) {
            RemoveDrawingTypefaceByGlobalUniqueId(it->globalUniqueId);
            it = delayDestroyTypefaces_.erase(it);
        } else {
            ++it;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
