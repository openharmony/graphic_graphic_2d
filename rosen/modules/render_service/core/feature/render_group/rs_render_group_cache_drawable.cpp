/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_render_group_cache_drawable.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

thread_local bool RSRenderGroupCacheDrawable::drawBlurForCache_ = false;
thread_local bool RSRenderGroupCacheDrawable::drawExcludedSubTreeForCache_ = false;

void RSRenderGroupCacheDrawable::SetDrawBlurForCache(bool value)
{
    drawBlurForCache_ = value;
}
bool RSRenderGroupCacheDrawable::IsDrawingBlurForCache()
{
    return drawBlurForCache_;
}

void RSRenderGroupCacheDrawable::SetDrawExcludedSubTreeForCache(bool value)
{
    drawExcludedSubTreeForCache_ = value;
}

bool RSRenderGroupCacheDrawable::IsDrawingExcludedSubTreeForCache()
{
    return drawExcludedSubTreeForCache_;
}

void RSRenderGroupCacheDrawable::SetLastFrameCacheRootHasExcludedChild(bool hasFilter)
{
    isLastFrameCacheRootHasExcludedChild_ = hasFilter;
}

void RSRenderGroupCacheDrawable::SetRenderGroupCachedSurface(const std::shared_ptr<Drawing::Surface>& surface)
{
    renderGroupCachedSurface_ = surface;
}

std::shared_ptr<Drawing::Surface>& RSRenderGroupCacheDrawable::GetRenderGroupCachedSurface()
{
    return renderGroupCachedSurface_;
}

void RSRenderGroupCacheDrawable::SetRenderGroupCachedImage(const std::shared_ptr<Drawing::Image>& image)
{
    renderGroupCachedImage_ = image;
}

std::shared_ptr<Drawing::Image>& RSRenderGroupCacheDrawable::GetRenderGroupCachedImage()
{
    return renderGroupCachedImage_;
}

pid_t RSRenderGroupCacheDrawable::GetRenderGroupCacheThreadId() const
{
    return renderGroupCacheThreadId_.load();
}

std::atomic<pid_t>& RSRenderGroupCacheDrawable::GetMutableRenderGroupCacheThreadId()
{
    return renderGroupCacheThreadId_;
}

void RSRenderGroupCacheDrawable::ClearRenderGroupResource()
{
    renderGroupCachedSurface_ = nullptr;
    renderGroupCachedImage_ = nullptr;
}

void RSRenderGroupCacheDrawable::SetRenderGroupDrawableCacheType(DrawableCacheType drawableCacheType)
{
    drawableCacheType_ = drawableCacheType;
}

DrawableCacheType RSRenderGroupCacheDrawable::GetRenderGroupDrawableCacheType() const
{
    return drawableCacheType_;
}

std::unique_lock<std::recursive_mutex> RSRenderGroupCacheDrawable::RenderGroupCacheLock() const
{
    return std::unique_lock<std::recursive_mutex>(cacheMutex_);
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
void RSRenderGroupCacheDrawable::SetCachedBackendTexture(const Drawing::BackendTexture& texture)
{
    cachedBackendTexture_ = texture;
}

const Drawing::BackendTexture& RSRenderGroupCacheDrawable::GetCachedBackendTexture() const
{
    return cachedBackendTexture_;
}
#endif

std::optional<RSRenderGroupCacheDrawable::ContinuousUpdateInfo> RSRenderGroupCacheDrawable::GetContinuousUpdateInfo(
    NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(continuousUpdateTimeMapMutex_);
    auto iter = continuousUpdateTimeMap_.find(nodeId);
    if (iter != continuousUpdateTimeMap_.end()) {
        return iter->second;
    }
    return std::nullopt;
}

void RSRenderGroupCacheDrawable::SetContinuousUpdateInfo(NodeId nodeId, int32_t count, uint64_t vsyncId)
{
    std::lock_guard<std::mutex> lock(continuousUpdateTimeMapMutex_);
    continuousUpdateTimeMap_[nodeId] = {count, vsyncId};
}

void RSRenderGroupCacheDrawable::ClearContinuousUpdateCount(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(continuousUpdateTimeMapMutex_);
    continuousUpdateTimeMap_.erase(nodeId);
}

void RSRenderGroupCacheDrawable::UpdateContinuousUpdateCount(NodeId nodeId, uint64_t vsyncId)
{
    std::lock_guard<std::mutex> lock(continuousUpdateTimeMapMutex_);
    auto& info = continuousUpdateTimeMap_[nodeId];
    // A node may be visited multiple times per vsync (e.g. layer cache + normal draw);
    // only increment once per frame so count reflects consecutive VSYNC frames.
    if (info.vsyncId != vsyncId) {
        info.count++;
        info.vsyncId = vsyncId;
    }
}

int32_t RSRenderGroupCacheDrawable::GetOrClearContinuousUpdateCount(
    NodeId nodeId, uint64_t currentVsyncId, bool needUpdateCache)
{
    std::lock_guard<std::mutex> lock(continuousUpdateTimeMapMutex_);
    auto iter = continuousUpdateTimeMap_.find(nodeId);
    if (iter == continuousUpdateTimeMap_.end()) {
        return 0;
    }
    auto& info = iter->second;
    if (info.vsyncId != currentVsyncId && !needUpdateCache) {
        continuousUpdateTimeMap_.erase(iter);
        return 0;
    }
    return info.count;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen