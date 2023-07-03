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
    // Note: absRect may be different from filter region (e.g. node has clipBounds or node is effectComponent), should
    // be take with care.
    auto SkAbsRect = SkIRect::MakeLTRB(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom());
    if (!cachedImageRegion_.contains(SkAbsRect)) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheState cache expired, REASON: absRect not in cached region.");
        InvalidateCache();
        return false;
    }

    // use dirty region to determine if cache is valid. we will delay the cache invalidation for given frames.
    auto SkDirtyRegion =
        SkIRect::MakeLTRB(dirtyRegion.GetLeft(), dirtyRegion.GetTop(), dirtyRegion.GetRight(), dirtyRegion.GetBottom());
    if (SkDirtyRegion.intersect(blurRegion_)) {
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
    // filter validation is not needed, since it's already done in RSPropertiesPainter::DrawFilter
    RS_TRACE_FUNC();

    auto clipIBounds = canvas.getDeviceClipBounds();
    if (clipIBounds.isEmpty()) {
        // clipIBounds is empty, no need to draw filter
        return;
    }

    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();

    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // cache is expired, take snapshot again
        ROSEN_LOGD("RSFilterCacheManager::DrawFilter cache expired, taking snapshot");
        TakeSnapshot(canvas, filter);
        ClipVisibleRect(canvas);
        DrawCachedSnapshot(canvas, filter);
        return;
    }

    // Update cache age, make sure cache invalidation immediately after dirty region intersect with cached region.
    --cacheUpdateInterval_;
    if (cacheType_ == CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        // we are caching blurred snapshot, draw cached blurred image directly
        ClipVisibleRect(canvas);
        DrawCachedBlurredSnapshot(canvas);
        return;
    }

    // cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT
    // we are caching snapshot, check if we should convert it to blurred snapshot
    auto filterHash = filter->Hash();
    if (filterHash == cachedFilterHash_ && blurRegion_ == clipIBounds) {
        // both filter and blurRegion are not changed, increase counter
        frameSinceLastBlurChange_++;
    } else {
        // filter or blurRegion changed, reset counter
        frameSinceLastBlurChange_ = 0;
        blurRegion_ = clipIBounds;
        if (filterHash != cachedFilterHash_) {
            filter->PreProcess(cachedImage_);
            cachedFilterHash_ = filterHash;
        }
    }
    // blur not changed for more than 3 frames, convert cache image to blurred image
    if (frameSinceLastBlurChange_ >= 3) {
        ROSEN_LOGD("RSFilterCacheManager::DrawFilter blur not changed in last 3 frames, generate blurred image cache.",
            filterHash);
        GenerateBlurredSnapshot(canvas, filter);
        ClipVisibleRect(canvas);
        DrawCachedBlurredSnapshot(canvas);
        return;
    }

    ClipVisibleRect(canvas);
    DrawCachedSnapshot(canvas, filter);
}

CachedEffectData RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    // similar to RSFilterCacheManager::DrawFilter, but do not draw anything on the canvas, directly return the cached
    // image and region
    // filter validation is not needed, since it's already done in RSPropertiesPainter::GenerateCachedEffectData
    RS_TRACE_FUNC();
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // cache is expired, take image snapshot again and cache it
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCachedEffectData cache expired, taking snapshot.");
        TakeSnapshot(canvas, filter);
    }
    // GeneratedCachedEffectData always generate blurred image cache, no fancy policy like DrawFilter
    if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCachedEffectData cache is snapshot, generate blurred image cache.");
        GenerateBlurredSnapshot(canvas, filter);
    }
    if (cacheType_ != CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        ROSEN_LOGE("RSFilterCacheManager::GeneratedCachedEffectData cache generation failed.");
        return {};
    }
    return { cachedImage_, cachedImageRegion_ };
}

void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_TRACE_FUNC();
    // Note: make snapshot 5 px larger than current clip bounds, to reduce cache invalidation caused by absRect change.
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipIBounds.makeOutset(5, 5);
    clipIPadding.intersect(SkIRect::MakeSize(canvas.getBaseLayerSize()));

    // Take screenshot
    cachedImage_ = skSurface->makeImageSnapshot(clipIPadding);
    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make image snapshot.");
        return;
    }
    filter->PreProcess(cachedImage_);
    // update cache state
    blurRegion_ = clipIBounds;
    cachedFilterHash_ = filter->Hash();
    cachedImageRegion_ = clipIPadding;
    cacheType_ = CacheType::CACHE_TYPE_SNAPSHOT;
    cacheUpdateInterval_ = RSSystemProperties::GetFilterCacheUpdateInterval();
    frameSinceLastBlurChange_ = 0;
}

void RSFilterCacheManager::GenerateBlurredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto surface = canvas.GetSurface();
    if (cacheType_ != CacheType::CACHE_TYPE_SNAPSHOT || surface == nullptr) {
        return;
    }
    // cacheType validated, blurRegion_ / cachedImage_ / cachedImageRegion_ should be valid, no need to check again
    RS_TRACE_FUNC();
    sk_sp<SkSurface> offscreenSurface = surface->makeSurface(blurRegion_.width(), blurRegion_.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // align offscreenCanvas coordinate system to blurRegion_
    offscreenCanvas.translate(-SkIntToScalar(blurRegion_.x()), -SkIntToScalar(blurRegion_.y()));
    DrawCachedSnapshot(offscreenCanvas, filter);

    cacheType_ = CacheType::CACHE_TYPE_BLURRED_SNAPSHOT;
    cachedImage_ = offscreenSurface->makeImageSnapshot();
    cachedImageRegion_ = blurRegion_;
}

void RSFilterCacheManager::DrawCachedSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter) const
{
    if (cacheType_ != CacheType::CACHE_TYPE_SNAPSHOT) {
        return;
    }
    RS_TRACE_FUNC();
    // cacheType validated, blurRegion_ / cachedImage_ / cachedImageRegion_ should be valid, no need to check again
    auto dstRect = SkRect::Make(blurRegion_);
    // shrink the srcRect by 1px to avoid edge artifacts, and align to cachedImage_ coordinate system
    auto srcRect = dstRect.makeOutset(-1.0f, -1.0f)
        .makeOffset(-SkIntToScalar(cachedImageRegion_.x()), -SkIntToScalar(cachedImageRegion_.y())) ;

    filter->DrawImageRect(canvas, cachedImage_, srcRect, dstRect);
    filter->PostProcess(canvas);
}

void RSFilterCacheManager::DrawCachedBlurredSnapshot(RSPaintFilterCanvas& canvas) const
{
    if (cacheType_ != CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        return;
    }
    RS_TRACE_FUNC();
    // cacheType validated, blurRegion_ / cachedImage_ should be valid, no need to check again
    auto dstRect = SkRect::Make(blurRegion_);

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.drawImageRect(cachedImage_, dstRect, SkSamplingOptions(), &paint);
}

void RSFilterCacheManager::InvalidateCache()
{
    cachedFilterHash_ = 0;
    cachedImage_.reset();
    cacheType_ = CacheType::CACHE_TYPE_NONE;
    cacheUpdateInterval_ = 0;
    frameSinceLastBlurChange_ = 0;
}

void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas) const
{
    auto visibleIRect = canvas.GetVisibleRect().round();
    auto deviceClipRect = canvas.getDeviceClipBounds();
    if (!visibleIRect.isEmpty() && deviceClipRect.intersect(visibleIRect)) {
        canvas.clipIRect(visibleIRect);
    }
}
} // namespace Rosen
} // namespace OHOS
