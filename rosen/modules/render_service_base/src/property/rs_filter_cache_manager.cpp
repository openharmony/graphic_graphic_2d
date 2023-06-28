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

#include "property/rs_filter_cache_manager.h"

#include "rs_trace.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
bool RSFilterCacheManager::UpdateCacheState(const RectI& dirtyRegion, const RectI& absRect, uint32_t filterHash)
{
    RS_TRACE_FUNC();
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        return false;
    }
    // Note: if cacheType_ is not CACHE_TYPE_NONE, cachedImage_ and cachedImageRegion_ must not not empty, no need to check.

    // if we are caching blurred snapshot, we need to check filter hash match.
    if (cacheType_ == CacheType::CACHE_TYPE_BLURRED_SNAPSHOT && filterHash != cachedFilterHash_) {
        ROSEN_LOGD(
            "RSFilterCacheManager::UpdateCacheState cache expired, REASON: cached blurred snapshot %X does not match "
            "filter hash %X.",
            cachedFilterHash_, filterHash);
        InvalidateCache();
        return false;
    }

    // use absRect to determine if cache is valid.
    // Note: absRect may be different from filter region (e.g. node has clipBounds or effectComponent), should be
    // take with care.
    // Our cachedImageRegion_ is shrunk by 1 pixel, we need to shrink SkAbsRect by 1 pixel before compare.
    auto SkAbsRect =
        SkIRect::MakeLTRB(absRect.GetLeft() + 1, absRect.GetTop() + 1, absRect.GetRight() - 1, absRect.GetBottom() - 1);
    if (!cachedImageRegion_->contains(SkAbsRect)) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheState cache expired, REASON: absRect not in cached region.");
        InvalidateCache();
        return false;
    }

    // use dirty region to determine if cache is valid. we will delay the cache invalidation for given frames.
    auto SkDirtyRegion =
        SkIRect::MakeLTRB(dirtyRegion.GetLeft(), dirtyRegion.GetTop(), dirtyRegion.GetRight(), dirtyRegion.GetBottom());
    if (SkDirtyRegion.intersect(*cachedImageRegion_)) {
        --cacheUpdateInterval_;
        if (cacheUpdateInterval_ < 0) {
            ROSEN_LOGD("RSFilterCacheManager::UpdateCacheState cache expired, REASON: dirty region intersect with "
                       "cached region.");
            InvalidateCache();
            return false;
        } else {
            ROSEN_LOGD("RSFilterCacheManager::UpdateCacheState delay cache invalidate for %d frames.",
                cacheUpdateInterval_ + 1);
        }
    }

    // all check passed, cache is valid.
    return true;
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    if (filter == nullptr) {
        return;
    }
    RS_TRACE_FUNC();
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // cache is expired, take snapshot again
        ROSEN_LOGD("RSFilterCacheManager::DrawFilter cache expired, taking snapshot");
        TakeSnapshot(canvas, filter);
    } else if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
        auto filterHash = filter->Hash();
        if (filterHash == cachedFilterHash_) {
            frameSinceFilterChange_++;
        } else {
            frameSinceFilterChange_ = 0;
            filter->PreProcess(cachedImage_);
            cachedFilterHash_ = filterHash;
        }
        // filter not changed in last 3 frames, update cache to blurred image
        if (frameSinceFilterChange_ >= 3) {
            ROSEN_LOGD("RSFilterCacheManager::DrawFilter filter hash %X not changed in last 3 frames, generate blurred "
                       "image cache.",
                filterHash);
            GenerateBlurredSnapshot(canvas, filter);
        }
    }

    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::DrawFilter cachedImage_ is null.");
        return;
    }

    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();
    DrawCache(canvas, cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT ? filter : nullptr);
}

CachedEffectData RSFilterCacheManager::GeneratedCacheEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    if (filter == nullptr) {
        return {};
    }
    RS_TRACE_FUNC();
    // simplified version of RSFilterCacheManager::DrawFilter
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // cache is expired, image snapshot again and cache it
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCacheEffectData cache expired, taking snapshot.");
        TakeSnapshot(canvas, filter);
    }
    // GeneratedCacheEffectData always generate blurred image cache
    if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCacheEffectData cache is snapshot, generate blurred image cache.");
        GenerateBlurredSnapshot(canvas, filter);
    }
    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::GeneratedCacheEffectData cachedImage_ is null.");
        return {};
    }
    // directly return cached image and region
    return { cachedImage_, *cachedImageRegion_ };
}

void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_TRACE_FUNC();
    // PLANNING: make snapshot larger than device clip bounds, to avoid absRect change caused cache invalidation.
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipIBounds.makeOutset(-1, -1);
    cachedImage_ = skSurface->makeImageSnapshot(clipIPadding);
    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make image snapshot.");
        return;
    }
    filter->PreProcess(cachedImage_);
    cachedFilterHash_ = filter->Hash();
    cachedImageRegion_ = clipIPadding;
    cacheType_ = CacheType::CACHE_TYPE_SNAPSHOT;
    frameSinceFilterChange_ = 0;
    cacheUpdateInterval_ = RSSystemProperties::GetFilterCacheUpdateInterval();
}

void RSFilterCacheManager::GenerateBlurredSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        // this should not happen, canvas should be gpu-backed
        return;
    }
    RS_TRACE_FUNC();
    sk_sp<SkSurface> offscreenSurface = surface->makeSurface(cachedImage_->imageInfo());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    auto paint = filter->GetPaint();
#ifdef NEW_SKIA
    offscreenCanvas.drawImage(cachedImage_, 0, 0, SkSamplingOptions(), &paint);
#else
    // not implemented
#endif
    filter->PostProcess(offscreenCanvas);

    cachedImage_ = offscreenSurface->makeImageSnapshot();
    cacheType_ = CacheType::CACHE_TYPE_BLURRED_SNAPSHOT;
}

void RSFilterCacheManager::DrawCache(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter) const
{
    RS_TRACE_FUNC();
    auto visibleIRect = canvas.GetVisibleRect().round();
    SkIRect dstRect;
    SkIRect srcRect;
    // cache validation is already done in DrawFilter, so we can safely use cachedImageRegion_ and cachedImage_
    if (visibleIRect.intersect(*cachedImageRegion_)) {
        dstRect = visibleIRect;
        canvas.clipIRect(dstRect);
        auto visibleIPadding = visibleIRect.makeOutset(-1, -1);
        srcRect = visibleIPadding.makeOffset(-cachedImageRegion_->left(), -cachedImageRegion_->top());
    } else {
        dstRect = cachedImageRegion_->makeOutset(1, 1);
        srcRect = cachedImageRegion_->makeOffset(-cachedImageRegion_->left(), -cachedImageRegion_->top());
    }

    auto paint = filter ? filter->GetPaint() : SkPaint();
#ifdef NEW_SKIA
    canvas.drawImageRect(cachedImage_.get(), SkRect::Make(srcRect), SkRect::Make(dstRect), SkSamplingOptions(), &paint,
        SkCanvas::kStrict_SrcRectConstraint);
#else
    // not implemented
#endif
    if (filter) {
        filter->PostProcess(canvas);
    }
}

void RSFilterCacheManager::InvalidateCache()
{
    cacheType_ = CacheType::CACHE_TYPE_NONE;
    cachedImage_.reset();
    cachedImageRegion_.reset();
    cachedFilterHash_ = 0;
    frameSinceFilterChange_ = 0;
    cacheUpdateInterval_ = 0;
}
} // namespace Rosen
} // namespace OHOS
