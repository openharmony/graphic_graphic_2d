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

#ifndef USE_ROSEN_DRAWING
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
class RSFilterCacheManager final {
public:
    RSFilterCacheManager() = default;
    ~RSFilterCacheManager() = default;
    RSFilterCacheManager(const RSFilterCacheManager&) = delete;
    RSFilterCacheManager(const RSFilterCacheManager&&) = delete;
    RSFilterCacheManager& operator=(const RSFilterCacheManager&) = delete;
    RSFilterCacheManager& operator=(const RSFilterCacheManager&&) = delete;

    // Call these functions during the prepare phase to validate the cache state with the filter hash, filter region,
    // and dirty region.
    void UpdateCacheStateWithFilterHash(uint32_t filterHash);
    void UpdateCacheStateWithFilterRegion(const RectI& filterRegion);
    void UpdateCacheStateWithDirtyRegion(const RectI& dirtyRegion);

    // Similar to UpdateCacheStateWithFilterRegion and UpdateCacheStateWithDirtyRegion, but instead of passing a filter
    // region or dirty region, we directly pass the test result of if the cached region cannot fully cover the filter
    // region or if the cached region is intersected with dirty region.
    void UpdateCacheStateWithFilterRegion(bool isCachedRegionCannotCoverFilterRegion);
    void UpdateCacheStateWithDirtyRegion(bool isCachedRegionIntersectedWithDirtyRegion);
    const SkIRect& GetCachedImageRegion() const;

    // Call this function during the process phase to apply the filter. Depending on the cache state, it may either
    // regenerate the cache or reuse the existing cache.
    // Note: The caller should clip the canvas before calling this method, we'll use the DeviceClipRect as the filtered
    // and cached region.
    void DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);

    // This function is similar to DrawFilter(), but instead of drawing anything on the canvas, it simply returns the
    // cache data. This is used with effect component in RSPropertiesPainter::DrawBackgroundEffect.
    CachedEffectData GeneratedCachedEffectData(
        RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);

    // Call this function to manually invalidate the cache. The next time DrawFilter() is called, it will regenerate the
    // cache.
    void InvalidateCache();

    bool IsCacheValid() const
    {
        return cacheType_ != CacheType::CACHE_TYPE_NONE;
    }

private:
    // TakeSnapshot won't apply the filter, but we need to call filter::PreProcess()
    void TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);
    // GenerateFilteredSnapshot will call DrawCachedSnapshot to generate filtered snapshot and cache it.
    void GenerateFilteredSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);
    void DrawCachedSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter) const;
    void DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas) const;
    void ClipVisibleRect(RSPaintFilterCanvas& canvas) const;

    enum class CacheType : uint8_t {
        CACHE_TYPE_NONE,
        CACHE_TYPE_SNAPSHOT,
        CACHE_TYPE_FILTERED_SNAPSHOT,
    };

    CacheType cacheType_ = CacheType::CACHE_TYPE_NONE;
    sk_sp<SkImage> cachedImage_ = nullptr;

    // for automatically converting cached snapshot to filtered snapshot if the filter is persistent
    uint32_t cachedFilterHash_ = 0;
    int frameSinceLastFilterChange_ = 0;

    // for delaying cached snapshot update even if the cached area is intersected with dirty region.
    int cacheUpdateInterval_ = 0;

    // Note: all rects should be in device coordinate space
    SkIRect cachedImageRegion_; // region of cached image
    SkIRect filterRegion_;      // region of previous filter region
};

} // namespace Rosen
} // namespace OHOS
#endif

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_FILTER_CACHE_MANAGER_H
