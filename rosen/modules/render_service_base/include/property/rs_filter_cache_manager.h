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
class RSFilterCacheManager {
public:
    RSFilterCacheManager() = default;
    ~RSFilterCacheManager() = default;
    RSFilterCacheManager(const RSFilterCacheManager&) = delete;

    bool IsCacheValid() const
    {
        return cacheType_ != CacheType::CACHE_TYPE_NONE && cachedImage_;
    }

    // Call in prepare phase, use intermediate dirty region to determine the cache validity.
    bool UpdateCacheState(const RectI& dirtyRegion, const RectI& absRect, uint32_t filterHash);

    // Call in process phase, apply filter, will regenerate cache or reuse cache depends on the cache state.
    // Note: The caller should clip the canvas before calling this method, we'll use the DeviceClipRect as the blurred
    // and cached region.
    void DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);

    // Similar to DrawFilter(), but not draw anything on canvas, just return the cache data.
    // for effect component adaption in RSPropertiesPainter::DrawBackgroundEffect
    CachedEffectData GeneratedCacheEffectData(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);

    // Clear the cache, next time DrawFilter() will regenerate the cache.
    void InvalidateCache();

private:
    // UpdateSnapshot won't apply the filter, but we need to call filter::Preprocess()
    void TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);
    // Convert cached snapshot to blurred snapshot
    void GenerateBlurredSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter);
    // If filter is nullptr, we'll directly draw the cached image on canvas
    void DrawCache(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter = nullptr) const;

    enum class CacheType : uint8_t {
        CACHE_TYPE_NONE = 0,
        CACHE_TYPE_SNAPSHOT = 1,
        CACHE_TYPE_BLURRED_SNAPSHOT = 2,
    };

    CacheType cacheType_ = CacheType::CACHE_TYPE_NONE;
    std::optional<SkIRect> cachedImageRegion_ = std::nullopt; // Note: this should always be in device coordinate space
    sk_sp<SkImage> cachedImage_ = nullptr ;
    // for automatically converting cached snapshot to blurred snapshot if the filter is persistent
    uint32_t cachedFilterHash_ = 0;
    int frameSinceFilterChange_ = 0;
    // for lowering snapshot frequency even if the snapshot is changed
    int cacheUpdateInterval_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_FILTER_CACHE_MANAGER_H
