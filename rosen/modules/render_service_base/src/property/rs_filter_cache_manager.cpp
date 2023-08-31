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

#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_GL)
#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
inline static bool IsLargeArea(int width, int height)
{
    // Use configurable threshold to determine if the area is large, and apply different cache policy.
    // [PLANNING]: dynamically adjust the cache policy (e.g. update interval and cache area expansion) according to the
    // cache size / dirty region percentage / current frame rate / filter radius.
    static const auto threshold = RSSystemProperties::GetFilterCacheSizeThreshold();
    return width > threshold && height > threshold;
}

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(const std::shared_ptr<RSFilter>& filter)
{
    if (cachedFilteredSnapshot_ == nullptr) {
        return;
    }
    auto filterHash = filter == nullptr ? 0u : filter->Hash();
    RS_OPTIONAL_TRACE_FUNC();

    if (filterHash == cachedFilterHash_) {
        // filter not changed, clear the unfiltered snapshot.
        InvalidateCache(CacheType::CACHE_TYPE_SNAPSHOT);
    } else {
        // filter changed, clear the filtered snapshot.
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered hash "
                   "%{public}X does not match new hash %{public}X.",
            cachedFilterHash_, filterHash);
        InvalidateCache(CacheType::CACHE_TYPE_FILTERED_SNAPSHOT);
    }
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
                   "invalidation for %{public}d frames.",
            cacheUpdateInterval_);
    } else {
        InvalidateCache();
    }
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
    const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect)
{
    // Filter validation is not needed, since it's already done in RSPropertiesPainter::DrawFilter.
    auto clipIBounds = canvas.getDeviceClipBounds();
    if (clipIBounds.isEmpty()) {
        // clipIBounds is empty, no need to draw filter.
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.isEmpty() || dst.isEmpty()) {
        return;
    }

    RS_OPTIONAL_TRACE_FUNC();

    // Try to reattach cached image to recording context if needed, if failed, we'll invalidate the cache.
    // Planning: Cache state should be calculated in prepare phase, this may be too late.
    ReattachCachedImage(canvas);

    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else {
        --cacheUpdateInterval_;
    }

    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        // If the cached filtered snapshot is not valid, regenerate it.
        GenerateFilteredSnapshot(canvas, filter, dst);
    } else {
        // If the cached filtered snapshot from previous frame is valid, the unfiltered snapshot is unnecessary. clear
        // it to save memory.
        InvalidateCache(CacheType::CACHE_TYPE_SNAPSHOT);
    }

    DrawCachedFilteredSnapshot(canvas, dst);
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect,
    const std::optional<SkIRect>& dstRect)
{
    // This function is similar to RSFilterCacheManager::DrawFilter, but does not draw anything on the canvas. Instead,
    // it directly returns the cached image and region. Filter validation is not needed, since it's already done in
    // RSPropertiesPainter::GenerateCachedEffectData.
    auto clipIBounds = canvas.getDeviceClipBounds();
    if (clipIBounds.isEmpty()) {
        // clipIBounds is empty, no need to draw filter.
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.isEmpty() || dst.isEmpty()) {
        return nullptr;
    }

    RS_OPTIONAL_TRACE_FUNC();

    // Try to reattach cached image to recording context if needed, if failed, we'll invalidate the cache.
    // Planning: Cache state should be calculated in prepare phase, this may be too late.
    ReattachCachedImage(canvas);

    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else {
        --cacheUpdateInterval_;
    }

    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        // If the cached filtered snapshot is not valid, regenerate it.
        GenerateFilteredSnapshot(canvas, filter, dst);
    } else {
        // If the cached filtered snapshot from previous frame is valid, the unfiltered snapshot is unnecessary. clear
        // it to save memory.
        InvalidateCache(CacheType::CACHE_TYPE_SNAPSHOT);
    }

    return cachedFilteredSnapshot_;
}

void RSFilterCacheManager::TakeSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& srcRect)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    auto snapshotIBounds = srcRect.makeOutset(-1, -1);

    // Take a screenshot.
    auto snapshot = skSurface->makeImageSnapshot(snapshotIBounds);
    if (snapshot == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(snapshot->width(), snapshot->height())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).", snapshot->width(),
            snapshot->height());
        as_IB(snapshot)->hintCacheGpuResource();
    }
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height());
    cachedSnapshot_ =
        std::make_unique<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), std::move(snapshotIBounds));

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    bool isLargeArea = IsLargeArea(srcRect.width(), srcRect.height());
    cacheUpdateInterval_ =
        isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
}

void RSFilterCacheManager::GenerateFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || cachedSnapshot_ == nullptr || cachedSnapshot_->cachedImage_ == nullptr) {
        return;
    }
    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    RS_OPTIONAL_TRACE_FUNC();

    // Create an offscreen canvas with the same size as the filter region.
    auto offscreenRect = dstRect;
    auto offscreenSurface = surface->makeSurface(offscreenRect.width(), offscreenRect.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Src rect and dst rect, with origin at (0, 0).
    auto src = SkRect::MakeSize(SkSize::Make(cachedSnapshot_->cachedRect_.size()));
    auto dst = SkRect::MakeSize(SkSize::Make(offscreenRect.size()));

    // Draw the cached snapshot on the offscreen canvas, apply the filter, and post-process.
    filter->DrawImageRect(offscreenCanvas, cachedSnapshot_->cachedImage_, src, dst);
    filter->PostProcess(offscreenCanvas);

    // Update the cache state with the filtered snapshot.
    auto filteredSnapshot = offscreenSurface->makeImageSnapshot();
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(filteredSnapshot->width(), filteredSnapshot->height())) {
        ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            filteredSnapshot->width(), filteredSnapshot->height());
        as_IB(filteredSnapshot)->hintCacheGpuResource();
    }
    cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), std::move(offscreenRect));
    cachedFilterHash_ = filter->Hash();
}

void RSFilterCacheManager::DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas, const SkIRect& dstRect) const
{
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // Draw in device coordinates.
    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();

    // Only draw within the visible rect.
    ClipVisibleRect(canvas);

    // The cache type and parameters has been validated, dstRect must be subset of cachedFilteredSnapshot_->cachedRect_.
    auto dst = SkRect::Make(dstRect);
    auto src = SkRect::Make(dstRect.makeOffset(-cachedFilteredSnapshot_->cachedRect_.topLeft()));

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.drawImageRect(cachedFilteredSnapshot_->cachedImage_, src, dst, SkSamplingOptions(), &paint,
        SkCanvas::kFast_SrcRectConstraint);
}

void RSFilterCacheManager::InvalidateCache(CacheType cacheType)
{
    // bitwise test
    if (cacheType & CacheType::CACHE_TYPE_SNAPSHOT) {
        cachedSnapshot_.reset();
    }
    if (cacheType & CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        cachedFilteredSnapshot_.reset();
        cachedFilterHash_ = 0;
    }
}

inline void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleIRect = canvas.GetVisibleRect().round();
    auto deviceClipRect = canvas.getDeviceClipBounds();
    if (!visibleIRect.isEmpty() && deviceClipRect.intersect(visibleIRect)) {
        canvas.clipIRect(visibleIRect);
    }
}

const RectI& RSFilterCacheManager::GetCachedImageRegion() const
{
    static const auto emptyRect = RectI();
    return IsCacheValid() ? snapshotRegion_ : emptyRect;
}

void RSFilterCacheManager::ReattachCachedImage(RSPaintFilterCanvas& canvas)
{
    if (cachedSnapshot_ != nullptr && !ReattachCachedImageImpl(canvas, *cachedSnapshot_)) {
        InvalidateCache(CacheType::CACHE_TYPE_SNAPSHOT);
    }
    if (cachedFilteredSnapshot_ != nullptr && !ReattachCachedImageImpl(canvas, *cachedFilteredSnapshot_)) {
        InvalidateCache(CacheType::CACHE_TYPE_FILTERED_SNAPSHOT);
    }
}

bool RSFilterCacheManager::ReattachCachedImageImpl(
    RSPaintFilterCanvas& canvas, RSPaintFilterCanvas::CachedEffectData& effectData)
{
    if (effectData.cachedImage_->isValid(canvas.recordingContext())) {
        return true;
    }
    RS_OPTIONAL_TRACE_FUNC();

    auto sharedBackendTexture = effectData.cachedImage_->getBackendTexture(false);
    if (!sharedBackendTexture.isValid()) {
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to get backend texture.");
        return false;
    }
    auto reattachedCachedImage =
        SkImage::MakeFromTexture(canvas.recordingContext(), sharedBackendTexture, kBottomLeft_GrSurfaceOrigin,
            effectData.cachedImage_->colorType(), effectData.cachedImage_->alphaType(), nullptr);
    if (reattachedCachedImage == nullptr || !reattachedCachedImage->isValid(canvas.recordingContext())) {
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to create SkImage from backend texture.");
        return false;
    }
    effectData.cachedImage_ = std::move(reattachedCachedImage);
    return false;
}

std::tuple<SkIRect, SkIRect> RSFilterCacheManager::ValidateParams(
    RSPaintFilterCanvas& canvas, const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect)
{
    SkIRect src;
    SkIRect dst;
    auto deviceRect = SkIRect::MakeWH(canvas.imageInfo().width(), canvas.imageInfo().height());
    if (!srcRect.has_value()) {
        src = canvas.getDeviceClipBounds();
    } else {
        src = srcRect.value();
        src.intersect(deviceRect);
    }
    if (!dstRect.has_value()) {
        dst = src;
    } else {
        dst = dstRect.value();
        dst.intersect(deviceRect);
    }
    return { src, dst };
}
} // namespace Rosen
} // namespace OHOS
#endif
