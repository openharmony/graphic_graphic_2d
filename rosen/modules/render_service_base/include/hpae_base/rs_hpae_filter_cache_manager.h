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

#ifndef RENDER_SERVICE_BASE_INCLUDE_HPAE_FILTER_CACHE_MANAGER_H
#define RENDER_SERVICE_BASE_INCLUDE_HPAE_FILTER_CACHE_MANAGER_H

#include <condition_variable>
#include <deque>
#include <mutex>

#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_base_types.h"

#include "common/rs_rect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_filter.h"
#include "render/rs_pixel_stretch_params.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class RSDrawingFilter;

class RSB_EXPORT RSHpaeFilterCacheManager final {
public:
    RSHpaeFilterCacheManager() = default;
    ~RSHpaeFilterCacheManager() = default;
    RSHpaeFilterCacheManager(const RSHpaeFilterCacheManager&) = delete;
    RSHpaeFilterCacheManager(const RSHpaeFilterCacheManager&&) = delete;
    RSHpaeFilterCacheManager& operator=(const RSHpaeFilterCacheManager&) = delete;
    RSHpaeFilterCacheManager& operator=(const RSHpaeFilterCacheManager&&) = delete;

    int DrawBackgroundToCanvas(RSPaintFilterCanvas& canvas);
    int DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSFilter>& filter,
        bool shouldClearFilteredCache,
        const std::optional<Drawing::RectI>& srcRect = std::nullopt,
        const std::optional<Drawing::RectI>& dstRect = std::nullopt); // for blur background

    int DrawFilterImpl(const std::shared_ptr<RSDrawingFilter>& filter,
        const Drawing::RectI& clipBounds, const Drawing::RectI& src, bool shouldClearFilteredCache);

    bool IsCacheValid();
    FilterCacheType GetCachedType() const;

    void TakeSnapshot(RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& srcRect);

    int ProcessHpaeBlur(const Drawing::RectI& clipBounds, const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& src);

    HpaeTask GenerateHianimationTask(const HpaeBufferInfo &inputBuffer, const HpaeBufferInfo &outputBuffer,
        float radius, const std::shared_ptr<RSDrawingFilter> &filter);

    int ProcessHianimationBlur(const std::shared_ptr<RSDrawingFilter>& filter, float radius);
    int ProcessGpuBlur(const HpaeBufferInfo &inputBuffer,
        const HpaeBufferInfo &outputBuffer, const std::shared_ptr<RSDrawingFilter>& filter, float radius);

    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> GetCachedSnapshot() const { return cachedSnapshot_; }
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> GetCachedFilteredSnapshot() const {
        return cachedFilteredSnapshot_;
    }
    RectI GetSnapshotRegion() const { return snapshotRegion_; }
    void ClearFilterCacheIfNeed(std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot,
        std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot) {
        if (cachedFilteredSnapshot) {
            cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(
                cachedFilteredSnapshot->cachedImage_, cachedFilteredSnapshot->cachedRect_);
        } else {
            cachedFilteredSnapshot_.reset();
        }
        if (cachedSnapshot) {
            cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(
                cachedSnapshot->cachedImage_, cachedSnapshot->cachedRect_);
        } else {
            cachedSnapshot_.reset();
        }
    }

    void ResetFilterCache(std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot,
        std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot, RectI snapshotRegion);
    void InvalidateFilterCache(FilterCacheType clearType);

    bool BlurContentChanged() {
        return blurContentChanged_;
    }

private:
    bool CheckIfUsingGpu();
    void SetBlurOutput(HpaeBackgroundCacheItem &outputBuffer);
    HpaeBackgroundCacheItem GetBlurOutput();

    int BlurWithoutSnapshot(const Drawing::RectI& clipBounds,
        const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<RSDrawingFilter>& filter,
        const Drawing::RectI& src,
        bool shouldClearFilteredCache);

    int BlurUsingFilteredSnapshot();

    // Region of the cached image, used to determine if we need to invalidate the cache.
    RectI snapshotRegion_; // Note: in device coordinate.

    // We keep both the snapshot and filtered snapshot in the cache, and clear unneeded one in next frame.
    // Note: rect in cachedSnapshot_ and cachedFilteredSnapshot_ is in device coordinate.
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot_ = nullptr;

    HpaeBufferInfo inputBufferInfo_;
    HpaeBufferInfo outputBufferInfo_;

    // blur output deque
    std::mutex blurOutMutex_;
    std::deque<HpaeBackgroundCacheItem> hpaeBlurOutputQueue_;

    bool drawUsingGpu_ = false;
    float curRadius_ = 0.f;
    float backgroundRadius_ = 0.f;
    bool blurContentChanged_ = false;
    std::shared_ptr<Drawing::Image> prevBlurImage_ = nullptr;
};

} // Rosen
} // OHOS

#endif // RENDER_SERVICE_BASE_INCLUDE_HPAE_FILTER_CACHE_MANAGER_H