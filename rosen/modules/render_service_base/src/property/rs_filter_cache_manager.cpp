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
    auto filterHash = filter->Hash();
    if (cachedFilteredSnapshot_ == nullptr || cachedFilterHash_ == filterHash) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // filter changed, clear the filtered snapshot.
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered hash "
               "%{public}X does not match new hash %{public}X.",
        cachedFilterHash_, filterHash);
    cachedFilteredSnapshot_.reset();
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
    RS_OPTIONAL_TRACE_FUNC();
    // Filter validation is not needed, since it's already done in RSPropertiesPainter::DrawFilter.
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures, we just refuse to
    // work.
    if (canvas.getDeviceClipBounds().isEmpty() || canvas.GetIsParallelCanvas()) {
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.isEmpty() || dst.isEmpty()) {
        return;
    }
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else {
        --cacheUpdateInterval_;
    }
    bool isFilterHashChanged = filter->Hash() != cachedFilterHash_;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        GenerateFilteredSnapshot(canvas, filter, dst);
    }
    DrawCachedFilteredSnapshot(canvas, dst);
    CompactCache(isFilterHashChanged);
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect,
    const std::optional<SkIRect>& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    // Filter validation is not needed, since it's already done in RSPropertiesPainter::GenerateCachedEffectData.
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures, we just refuse to
    // work.
    if (canvas.getDeviceClipBounds().isEmpty() || canvas.GetIsParallelCanvas()) {
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.isEmpty() || dst.isEmpty()) {
        return nullptr;
    }
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else {
        --cacheUpdateInterval_;
    }
    bool isFilterHashChanged = filter->Hash() != cachedFilterHash_;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        GenerateFilteredSnapshot(canvas, filter, dst);
    }
    // Keep a reference to the cached image, since CompactCache may invalidate it.
    auto cachedFilteredSnapshot = cachedFilteredSnapshot_;
    CompactCache(isFilterHashChanged);
    return cachedFilteredSnapshot;
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
    cachedFilterHash_ = 0;
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

inline static bool IsCacheInvalid(const RSPaintFilterCanvas::CachedEffectData& cache, RSPaintFilterCanvas& canvas)
{
    return cache.cachedImage_ == nullptr || !cache.cachedImage_->isValid(canvas.recordingContext());
}

void RSFilterCacheManager::CheckCachedImages(RSPaintFilterCanvas& canvas)
{
    if (cachedSnapshot_ != nullptr && IsCacheInvalid(*cachedSnapshot_, canvas)) {
        cachedSnapshot_.reset();
    }
    if (cachedFilteredSnapshot_ != nullptr && IsCacheInvalid(*cachedFilteredSnapshot_, canvas)) {
        cachedFilteredSnapshot_.reset();
    }
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

inline void RSFilterCacheManager::CompactCache(bool isFilterHashChanged)
{
    if (isFilterHashChanged) {
        cachedFilteredSnapshot_.reset();
    } else {
        cachedSnapshot_.reset();
    }
}
} // namespace Rosen
} // namespace OHOS
#endif
