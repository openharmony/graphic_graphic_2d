/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_FILTER_CACHE_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_FILTER_CACHE_MANAGER_H

#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_GL)
#include "include/core/SkRect.h"

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSSkiaFilter;
// Note: we don't care about if the filter will be applied to background or foreground, the caller should take care of
// this. This means if both background and foreground need to apply filter, the caller should create two
// RSFilterCacheManager, pass the correct dirty region, and call the DrawFilter() in correct order.
// Warn: Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures.
class RSFilterCacheManager final {
public:
    RSFilterCacheManager() = default;
    ~RSFilterCacheManager() = default;
    RSFilterCacheManager(const RSFilterCacheManager&) = delete;
    RSFilterCacheManager(const RSFilterCacheManager&&) = delete;
    RSFilterCacheManager& operator=(const RSFilterCacheManager&) = delete;
    RSFilterCacheManager& operator=(const RSFilterCacheManager&&) = delete;

    // Call these functions during the prepare phase to validate the cache state with the filter, if filter region is
    // intersected with cached region, and if cached region is intersected with dirty region.
    void UpdateCacheStateWithFilterHash(const std::shared_ptr<RSFilter>& filter);
    void UpdateCacheStateWithFilterRegion(); // call when filter region out of cached region.
    void UpdateCacheStateWithDirtyRegion(); // call when dirty region intersects with cached region.
    const RectI& GetCachedImageRegion() const;

    // Call this function during the process phase to apply the filter. Depending on the cache state, it may either
    // regenerate the cache or reuse the existing cache.
    // Note: If srcRect or dstRect is empty, we'll use the DeviceClipRect as the corresponding rect.
    void DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
        const std::optional<SkIRect>& srcRect = std::nullopt, const std::optional<SkIRect>& dstRect = std::nullopt);

    // This function is similar to DrawFilter(), but instead of drawing anything on the canvas, it simply returns the
    // cache data. This is used with effect component in RSPropertiesPainter::DrawBackgroundEffect.
    const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> GeneratedCachedEffectData(RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect = std::nullopt,
        const std::optional<SkIRect>& dstRect = std::nullopt);

    enum CacheType : uint8_t {
        CACHE_TYPE_NONE              = 0,
        CACHE_TYPE_SNAPSHOT          = 1,
        CACHE_TYPE_FILTERED_SNAPSHOT = 2,
        CACHE_TYPE_BOTH              = 3,
    };

    // Call this function to manually invalidate the cache. The next time DrawFilter() is called, it will regenerate the
    // cache.
    void InvalidateCache(CacheType cacheType = CacheType::CACHE_TYPE_BOTH);

    inline bool IsCacheValid() const
    {
        return cachedSnapshot_ != nullptr || cachedFilteredSnapshot_ != nullptr;
    }

private:
    void TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& srcRect);
    void GenerateFilteredSnapshot(
        RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect);
    void DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas, const SkIRect& dstRect) const;
    inline static void ClipVisibleRect(RSPaintFilterCanvas& canvas);
    // Check if the cache is valid in current GrContext, since FilterCache will never be used in multi-thread
    // environment, we don't need to attempt to reattach SkImages.
    void CheckCachedImages(RSPaintFilterCanvas& canvas);
    // To reduce memory usage, clear one of the cached images.
    inline void CompactCache(bool shouldClearFilteredCache);
    // Validate the input srcRect and dstRect, and return the validated rects.
    std::tuple<SkIRect, SkIRect> ValidateParams(RSPaintFilterCanvas& canvas,
        const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect);

    // We keep both the snapshot and filtered snapshot in the cache, and clear unneeded one in next frame.
    // Note: rect in cachedSnapshot_ and cachedFilteredSnapshot_ is in device coordinate.
    std::unique_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot_ = nullptr;

    // Hash of previous filter, used to determine if we need to invalidate cachedFilteredSnapshot_.
    uint32_t cachedFilterHash_ = 0;
    // Cache age, used to determine if we can delay the cache update.
    int cacheUpdateInterval_ = 0;
    // Region of the cached image, used to determine if we need to invalidate the cache.
    RectI snapshotRegion_; // Note: in device coordinate.
};
} // namespace Rosen
} // namespace OHOS
#endif

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_FILTER_CACHE_MANAGER_H
