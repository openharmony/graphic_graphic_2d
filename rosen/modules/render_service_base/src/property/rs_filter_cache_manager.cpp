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

#ifndef USE_ROSEN_DRAWING
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"
#include "src/image/SkImage_Base.h"

#ifdef RS_ENABLE_GL
#include "include/gpu/GrBackendSurface.h"
#endif

namespace OHOS {
namespace Rosen {
inline bool IsLargeArea(int width, int height)
{
    // Use configurable threshold to determine if the area is large, and apply different cache policy.
    // [PLANNING]: dynamically adjust the cache policy (e.g. update interval and cache area expansion) according to the
    // cache size / dirty region percentage / current frame rate / filter radius.
    static auto threshold = RSSystemProperties::GetFilterCacheSizeThreshold();
    return width > threshold && height > threshold;
}

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(uint32_t filterHash)
{
    if (cacheType_ != CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        return;
    }
    // If we are caching a filtered snapshot, we need to check if the filter hash matches.
    if (filterHash == cachedFilterHash_) {
        return;
    }

    RS_OPTIONAL_TRACE_FUNC();
    ROSEN_LOGD(
        "RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered snapshot"
        "%{public}X does not match filter hash %{public}X.",
        cachedFilterHash_, filterHash);
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion(const RectI& filterRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        return;
    }

    // Test if the filter region is contained in the cached region.
    auto skFilterRegion = SkIRect::MakeLTRB(
        filterRegion.GetLeft(), filterRegion.GetTop(), filterRegion.GetRight(), filterRegion.GetBottom());
    if (cachedImageRegion_.contains(skFilterRegion)) {
        filterRegion_ = skFilterRegion;
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion(bool isCachedRegionCannotCoverFilterRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE || isCachedRegionCannotCoverFilterRegion == false) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(const RectI& dirtyRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        return;
    }

    // Use the dirty region to determine if the cache is valid.
    auto SkDirtyRegion =
        SkIRect::MakeLTRB(dirtyRegion.GetLeft(), dirtyRegion.GetTop(), dirtyRegion.GetRight(), dirtyRegion.GetBottom());
    // The underlying image is not affected by the dirty region, cache is valid.
    if (!SkDirtyRegion.intersect(filterRegion_)) {
        return;
    }

    RS_OPTIONAL_TRACE_FUNC();
    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache  age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache"
            "invalidation for %{public}d frames.", cacheUpdateInterval_);
    } else {
        InvalidateCache();
    }
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(bool isIntersectedWithDirtyRegion)
{
    if (cacheType_ == CacheType::CACHE_TYPE_NONE || isIntersectedWithDirtyRegion == false) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
            "invalidation for %{public}d frames.", cacheUpdateInterval_);
    } else {
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
    RS_OPTIONAL_TRACE_FUNC();

    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();

    ReattachCachedImage(canvas);

    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // The cache is expired, take a snapshot again.
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
    if (cacheType_ == CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        // We are caching a filtered snapshot, draw the cached filtered image directly.
        ClipVisibleRect(canvas);
        DrawCachedFilteredSnapshot(canvas);
        return;
    }

    // cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT
    // We are caching a snapshot, check if we should convert it to a filtered snapshot.
    auto filterHash = filter->Hash();
    if (filterHash == cachedFilterHash_ && filterRegion_ == clipIBounds) {
        // Both filter and filterRegion have not changed, increase the counter.
        frameSinceLastFilterChange_++;
    } else {
        // Filter or filterRegion changed, reset the counter.
        frameSinceLastFilterChange_ = 0;
        filterRegion_ = clipIBounds;
        if (filterHash != cachedFilterHash_) {
            filter->PreProcess(cachedImage_);
            cachedFilterHash_ = filterHash;
        }
    }
    // filter has not changed for more than 3 frames, convert the cache image to a filtered image.
    if (frameSinceLastFilterChange_ >= 3) {
        ROSEN_LOGD(
            "RSFilterCacheManager::DrawFilter The filter filter and region have not changed in the last 3 frames, "
            "generating a filtered image cache.");
        GenerateFilteredSnapshot(canvas, filter);
        ClipVisibleRect(canvas);
        DrawCachedFilteredSnapshot(canvas);
        return;
    }

    ClipVisibleRect(canvas);
    DrawCachedSnapshot(canvas, filter);
}

std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    // This function is similar to RSFilterCacheManager::DrawFilter, but does not draw anything on the canvas. Instead,
    // it directly returns the cached image and region. Filter validation is not needed, since it's already done in
    // RSPropertiesPainter::GenerateCachedEffectData.
    RS_OPTIONAL_TRACE_FUNC();

    ReattachCachedImage(canvas);

    if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
        // The cache is expired, so take an image snapshot again and cache it.
        ROSEN_LOGD("RSFilterCacheManager::GeneratedCachedEffectData Cache expired, taking snapshot.");
        TakeSnapshot(canvas, filter);
    }

    // The GeneratedCachedEffectData function generates a filtered image cache, but it does not use any cache policies
    // like DrawFilter.
    if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
        ROSEN_LOGD(
            "RSFilterCacheManager::GeneratedCachedEffectData Cache is snapshot, generating filtered image cache.");
        filterRegion_ = cachedImageRegion_;
        GenerateFilteredSnapshot(canvas, filter);
    }

    if (cacheType_ != CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        ROSEN_LOGE("RSFilterCacheManager::GeneratedCachedEffectData Cache generation failed.");
        return {};
    }

    return std::make_shared<RSPaintFilterCanvas::CachedEffectData>(cachedImage_, cachedImageRegion_);
}

void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    auto clipIBounds = canvas.getDeviceClipBounds();
    auto snapshotIBounds = clipIBounds;
    bool isLargeArea = IsLargeArea(clipIBounds.width(), clipIBounds.height());
    if (isLargeArea) {
        // If the filter region is smaller than the threshold, we will not increase the cache update interval.
        // To reduce the chance of cache invalidation caused by small movements, we expand the snapshot region by 5
        // pixels.
        snapshotIBounds.outset(5, 5); // expand the snapshot region by 5 pixels.
        // Make sure the clipIPadding is not larger than the canvas or screen size.
        snapshotIBounds.intersect(SkIRect::MakeSize(canvas.getBaseLayerSize()));
    }

    // Take a screenshot.
    cachedImage_ = skSurface->makeImageSnapshot(snapshotIBounds);
    if (cachedImage_ == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
#ifdef NEW_SKIA
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(cachedImage_->width(), cachedImage_->height())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            cachedImage_->width(), cachedImage_->height());
        as_IB(cachedImage_)->hintCacheGpuResource();
    }
#endif
    filter->PreProcess(cachedImage_);

    // Update the cache state.
    cacheType_ = CacheType::CACHE_TYPE_SNAPSHOT;
    filterRegion_ = clipIBounds;
    cachedFilterHash_ = filter->Hash();
    cachedImageRegion_ = snapshotIBounds;
    frameSinceLastFilterChange_ = 0;

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    cacheUpdateInterval_ =
        isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
}

void RSFilterCacheManager::GenerateFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter)
{
    auto surface = canvas.GetSurface();
    if (cacheType_ != CacheType::CACHE_TYPE_SNAPSHOT || surface == nullptr) {
        return;
    }
    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    RS_OPTIONAL_TRACE_FUNC();

    // Create an offscreen canvas with the same size as the filter region.
    auto offscreenSurface = surface->makeSurface(filterRegion_.width(), filterRegion_.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Align the offscreen canvas coordinate system to the filter region.
    offscreenCanvas.translate(-SkIntToScalar(filterRegion_.x()), -SkIntToScalar(filterRegion_.y()));

    // Draw the cached snapshot onto the offscreen canvas, applying the filter.
    DrawCachedSnapshot(offscreenCanvas, filter);

    // Update the cache state with the filtered snapshot.
    cacheType_ = CacheType::CACHE_TYPE_FILTERED_SNAPSHOT;
    cachedImage_ = offscreenSurface->makeImageSnapshot();
#ifdef NEW_SKIA
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(cachedImage_->width(), cachedImage_->height())) {
        ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            cachedImage_->width(), cachedImage_->height());
        as_IB(cachedImage_)->hintCacheGpuResource();
    }
#endif
    cachedImageRegion_ = filterRegion_;
}

void RSFilterCacheManager::DrawCachedSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter) const
{
    if (cacheType_ != CacheType::CACHE_TYPE_SNAPSHOT) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // The cache type has been validated, so filterRegion_, cachedImage_, and cachedImageRegion_ should be valid. There
    // is no need to check them again.
    auto dstRect = SkRect::Make(filterRegion_);

    // Shrink the srcRect by 1px to avoid edge artifacts, and align it to the cachedImage_ coordinate system.
    auto srcRect = dstRect.makeOutset(-1.0f, -1.0f);
    srcRect.offset(-SkIntToScalar(cachedImageRegion_.x()), -SkIntToScalar(cachedImageRegion_.y()));

    filter->DrawImageRect(canvas, cachedImage_, srcRect, dstRect);
    filter->PostProcess(canvas);
}

void RSFilterCacheManager::DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas) const
{
    if (cacheType_ != CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    auto dstRect = SkRect::Make(filterRegion_);

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
    frameSinceLastFilterChange_ = 0;
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

const SkIRect& RSFilterCacheManager::GetCachedImageRegion() const
{
    return cachedImageRegion_;
}

void RSFilterCacheManager::ReattachCachedImage(RSPaintFilterCanvas& canvas)
{
#if defined(NEW_SKIA)
    if (cacheType_ == CacheType::CACHE_TYPE_NONE || cachedImage_->isValid(canvas.recordingContext())) {
#else
    if (cacheType_ == CacheType::CACHE_TYPE_NONE || cachedImage_->isValid(canvas.getGrContext())) {
#endif
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

#ifdef RS_ENABLE_GL
    auto sharedBackendTexture = cachedImage_->getBackendTexture(false);
    if (!sharedBackendTexture.isValid()) {
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to get backend texture.");
        InvalidateCache();
        return;
    }
#if defined(NEW_SKIA)
    auto reattachedCachedImage = SkImage::MakeFromTexture(canvas.recordingContext(), sharedBackendTexture,
#else
    auto reattachedCachedImage = SkImage::MakeFromTexture(canvas.getGrContext(), sharedBackendTexture,
#endif
        kBottomLeft_GrSurfaceOrigin, cachedImage_->colorType(), cachedImage_->alphaType(), nullptr);
#if defined(NEW_SKIA)
    if (reattachedCachedImage == nullptr || !reattachedCachedImage->isValid(canvas.recordingContext())) {
#else
    if (reattachedCachedImage == nullptr || !reattachedCachedImage->isValid(canvas.getGrContext())) {
#endif
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to create SkImage from backend texture.");
        InvalidateCache();
        return;
    }
    cachedImage_ = reattachedCachedImage;
#else
    InvalidateCache();
#endif
}
} // namespace Rosen
} // namespace OHOS
#endif
