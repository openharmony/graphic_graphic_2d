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

#ifndef RENDER_GROUP_CACHE_DRAWABLE_H
#define RENDER_GROUP_CACHE_DRAWABLE_H

#include <atomic>
#include <mutex>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "draw/surface.h"
#include "image/image.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderGroupCacheDrawable {
public:
    RSRenderGroupCacheDrawable() = default;
    ~RSRenderGroupCacheDrawable() = default;

    RSRenderGroupCacheDrawable(const RSRenderGroupCacheDrawable& other) = delete;
    RSRenderGroupCacheDrawable(RSRenderGroupCacheDrawable&& other) = delete;
    RSRenderGroupCacheDrawable& operator=(const RSRenderGroupCacheDrawable& other) = delete;
    RSRenderGroupCacheDrawable& operator=(RSRenderGroupCacheDrawable&& other) = delete;

    static void SetDrawBlurForCache(bool value);
    static bool IsDrawingBlurForCache();

    static void SetDrawExcludedSubTreeForCache(bool value);
    static bool IsDrawingExcludedSubTreeForCache();

    void SetLastFrameCacheRootHasExcludedChild(bool hasFilter);
    bool IsLastFrameCacheRootHasExcludedChild() const
    {
        return isLastFrameCacheRootHasExcludedChild_;
    }

    void SetShouldClipHole(bool value)
    {
        shouldClipHole_ = value;
    }

    bool ShouldClipHole() const
    {
        return shouldClipHole_;
    }

    void SetRenderGroupCachedSurface(const std::shared_ptr<Drawing::Surface>& surface);
    std::shared_ptr<Drawing::Surface>& GetRenderGroupCachedSurface();

    void SetRenderGroupCachedImage(const std::shared_ptr<Drawing::Image>& image);
    std::shared_ptr<Drawing::Image>& GetRenderGroupCachedImage();

    void ClearRenderGroupResource();

    pid_t GetRenderGroupCacheThreadId() const;
    std::atomic<pid_t>& GetMutableRenderGroupCacheThreadId();

    void SetRenderGroupDrawableCacheType(DrawableCacheType drawableCacheType);
    DrawableCacheType GetRenderGroupDrawableCacheType() const;

    std::scoped_lock<std::recursive_mutex> RenderGroupCacheScopedLock() const;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    void SetCachedBackendTexture(const Drawing::BackendTexture& texture);
    const Drawing::BackendTexture& GetCachedBackendTexture() const;
#endif

private:
    mutable std::recursive_mutex cacheMutex_;
    std::shared_ptr<Drawing::Surface> renderGroupCachedSurface_ = nullptr;
    std::shared_ptr<Drawing::Image> renderGroupCachedImage_ = nullptr;
    std::atomic<pid_t> renderGroupCacheThreadId_ = 0;
    std::atomic<DrawableCacheType> drawableCacheType_ = DrawableCacheType::NONE;
    bool isLastFrameCacheRootHasExcludedChild_ = false;
    bool shouldClipHole_ = false;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::BackendTexture cachedBackendTexture_;
#endif

    static thread_local bool drawBlurForCache_;
    static thread_local bool drawExcludedSubTreeForCache_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_GROUP_CACHE_DRAWABLE_H