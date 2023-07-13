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
void RSFilterCacheManager::UpdateCacheStateWithFilterHash(uint32_t filterHash)
{
    if (cacheType_ != CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        return;
    }
    RS_TRACE_FUNC();

    // If we are caching a blurred snapshot, we need to check if the filter hash matches.
    if (filterHash == cachedFilterHash_) {
        return;
    }
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached blurred snapshot %X "
               "does not match filter hash %X.",
        cachedFilterHash_, filterHash);
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion(const RectI& filterRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        return;
    }
    RS_TRACE_FUNC();

    // Test if the filter region is contained in the cached region.
    auto SkAbsRect = SkIRect::MakeLTRB(
        filterRegion.GetLeft(), filterRegion.GetTop(), filterRegion.GetRight(), filterRegion.GetBottom());
    if (cachedImageRegion_.contains(SkAbsRect)) {
        return;
    }
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(const RectI& dirtyRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        return;
    }
    RS_TRACE_FUNC();

    // Use the dirty region to determine if the cache is valid.
    auto SkDirtyRegion =
        SkIRect::MakeLTRB(dirtyRegion.GetLeft(), dirtyRegion.GetTop(), dirtyRegion.GetRight(), dirtyRegion.GetBottom());
    // The underlying image is not affected by the dirty region, cache is valid.
    if (!SkDirtyRegion.intersect(blurRegion_)) {
        return;
    }

    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache  age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache invalidation for %d frames.",
            cacheUpdateInterval_);
    } else {
        ROSEN_LOGD(
            "RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Cache expired. Reason: Dirty region intersects "
            "with cached region.");
        InvalidateCache();
    }
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    // Filter validation is not needed, since it's already done in RSPropertiesPainter::DrawFilter.
    auto clipIBounds = canvas.getDeviceClipBounds();
    if (clipIBounds.isEmpty()) {
        // clipIBounds is empty, no need to draw filter.
        return;
    }
    RS_TRACE_FUNC();

    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();

    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // The cache is expired, take a snapshot again.
        ROSEN_LOGD("RSFilterCacheManager::DrawFilter Cache expired, taking snapshot.");
        TakeSnapshot(canvas, filter);
        ClipVisibleRect(canvas);
        DrawCachedSnapshot(canvas, filter);
        return;
    }

    // Update the cache age, this will ensure that an old cache will be invalidated immediately when intersecting with
    // dirty region.
    if (cacheUpdateInterval_ > 0) {
        --cacheUpdateInterval_;
    }
    if (cacheType_ == CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        // We are caching a blurred snapshot, draw the cached blurred image directly.
        ClipVisibleRect(canvas);
        DrawCachedBlurredSnapshot(canvas);
        return;
    }

    // cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT
    // We are caching a snapshot, check if we should convert it to a blurred snapshot.
    auto filterHash = filter->Hash();
    if (filterHash == cachedFilterHash_ && blurRegion_ == clipIBounds) {
        // Both filter and blurRegion have not changed, increase the counter.
        frameSinceLastBlurChange_++;
    } else {
        // Filter or blurRegion changed, reset the counter.
        frameSinceLastBlurChange_ = 0;
        blurRegion_ = clipIBounds;
        if (filterHash != cachedFilterHash_) {
            filter->PreProcess(cachedImage_);
            cachedFilterHash_ = filterHash;
        }
    }
    // Blur has not changed for more than 3 frames, convert the cache image to a blurred image.
    if (frameSinceLastBlurChange_ >= 3) {
        ROSEN_LOGD("RSFilterCacheManager::DrawFilter The blur filter and region have not changed in the last 3 frames, "
                   "generating a blurred image cache.");
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
    // This function is similar to RSFilterCacheManager::DrawFilter, but does not draw anything on the canvas. Instead,
    // it directly returns the cached image and region. Filter validation is not needed, since it's already done in
    // RSPropertiesPainter::GenerateCachedEffectData.
    RS_TRACE_FUNC();

    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // The cache is expired, so take an image snapshot again and cache it.
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCachedEffectData Cache expired, taking snapshot.");
        TakeSnapshot(canvas, filter);
    }

    // GeneratedCachedEffectData always generates a blurred image cache, with no fancy policy like DrawFilter.
    if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
        ROSEN_LOGD(
            "RSFilterCacheManager::GeneratedCachedEffectData Cache is snapshot, generating blurred image cache.");
        // Hack to make the cached blur region a little larger, to avoid cache invalidation caused by slide.
        blurRegion_ = cachedImageRegion_;
        GenerateBlurredSnapshot(canvas, filter);
    }

    if (cacheType_ != CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        ROSEN_LOGE("RSFilterCacheManager::GeneratedCachedEffectData Cache generation failed.");
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

    // Note: make the snapshot 5 pixels larger than the current clip bounds, to reduce cache invalidation caused by
    // absRect change.
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipIBounds.makeOutset(5, 5);
    // Make sure the clipIPadding is not larger than the canvas size.
    clipIPadding.intersect(SkIRect::MakeSize(canvas.getBaseLayerSize()));

    // Take a screenshot.
    cachedImage_ = skSurface->makeImageSnapshot(clipIPadding);
    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
    filter->PreProcess(cachedImage_);

    // Update the cache state.
    cacheType_ = CacheType::CACHE_TYPE_SNAPSHOT;
    blurRegion_ = clipIBounds;
    cachedFilterHash_ = filter->Hash();
    cachedImageRegion_ = clipIPadding;
    frameSinceLastBlurChange_ = 0;

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <value>`, the default value is 1.
    // [PLANNING]: dynamically adjust the cache update interval according to the cache size / cache size percentage /
    // frame rate / filter radius.
    cacheUpdateInterval_ = (cachedImageRegion_.width() > 100 && cachedImageRegion_.height() > 100) // 100: size threshold
        ? RSSystemProperties::GetFilterCacheUpdateInterval()
        : 0;
}

void RSFilterCacheManager::GenerateBlurredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto surface = canvas.GetSurface();
    if (cacheType_ != CacheType::CACHE_TYPE_SNAPSHOT || surface == nullptr) {
        return;
    }
    // The cache type has been validated, so blurRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    RS_TRACE_FUNC();

    // Create an offscreen canvas with the same size as the blur region.
    auto offscreenSurface = surface->makeSurface(blurRegion_.width(), blurRegion_.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Align the offscreen canvas coordinate system to the blur region.
    offscreenCanvas.translate(-SkIntToScalar(blurRegion_.x()), -SkIntToScalar(blurRegion_.y()));

    // Draw the cached snapshot onto the offscreen canvas, applying the filter.
    DrawCachedSnapshot(offscreenCanvas, filter);

    // Update the cache state with the blurred snapshot.
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

    // The cache type has been validated, so blurRegion_, cachedImage_, and cachedImageRegion_ should be valid. There is
    // no need to check them again.
    auto dstRect = SkRect::Make(blurRegion_);

    // Shrink the srcRect by 1px to avoid edge artifacts, and align it to the cachedImage_ coordinate system.
    auto srcRect = dstRect.makeOutset(-1.0f, -1.0f)
        .makeOffset(-SkIntToScalar(cachedImageRegion_.x()), -SkIntToScalar(cachedImageRegion_.y()));

    filter->DrawImageRect(canvas, cachedImage_, srcRect, dstRect);
    filter->PostProcess(canvas);
}

void RSFilterCacheManager::DrawCachedBlurredSnapshot(RSPaintFilterCanvas& canvas) const
{
    if (cacheType_ != CacheType::CACHE_TYPE_BLURRED_SNAPSHOT) {
        return;
    }
    RS_TRACE_FUNC();

    // The cache type has been validated, so blurRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    auto dstRect = SkRect::Make(blurRegion_);

    SkPaint paint;
    paint.setAntiAlias(true);
#ifdef NEW_SKIA
    canvas.drawImageRect(cachedImage_, dstRect, SkSamplingOptions(), &paint);
 #endif   
}

void RSFilterCacheManager::InvalidateCache()
{
    cacheType_ = CacheType::CACHE_TYPE_NONE;
    cachedFilterHash_ = 0;
    cachedImage_.reset();
    cacheUpdateInterval_ = 0;
    frameSinceLastBlurChange_ = 0;
}

void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas) const
{
    auto visibleIRect = canvas.GetVisibleRect().round();
    auto deviceClipRect = canvas.getDeviceClipBounds();
    if (!visibleIRect.isEmpty() && deviceClipRect.intersect(visibleIRect)) {
#ifdef NEW_SKIA
        canvas.clipIRect(visibleIRect);
#endif 
    }
}
} // namespace Rosen
} // namespace OHOS
