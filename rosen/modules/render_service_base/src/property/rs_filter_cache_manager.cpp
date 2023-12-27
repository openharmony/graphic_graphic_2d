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

#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
#define CHECK_CACHE_PROCESS_STATUS                                       \
    do {                                                                 \
        if (cacheProcessStatus_.load() == CacheProcessStatus::WAITING) { \
            return false;                                                \
        }                                                                \
    } while (false)
#ifndef ROSEN_ARKUI_X
const bool RSFilterCacheManager::RSFilterCacheTask::FilterPartialRenderEnabled =
    RSSystemProperties::GetFilterPartialRenderEnabled() && RSUniRenderJudgement::IsUniRender();
#else
const bool RSFilterCacheManager::RSFilterCacheTask::FilterPartialRenderEnabled = false;
#endif
inline static bool IsLargeArea(int width, int height)
{
    // Use configurable threshold to determine if the area is large, and apply different cache policy.
    // [PLANNING]: dynamically adjust the cache policy (e.g. update interval and cache area expansion) according to the
    // cache size / dirty region percentage / current frame rate / filter radius.
    static const auto threshold = RSSystemProperties::GetFilterCacheSizeThreshold();
    return width > threshold && height > threshold;
}

#ifndef USE_ROSEN_DRAWING
inline static bool isEqualRect(const SkIRect& src, const RectI& dst)
{
    return src.x() == dst.GetLeft() && src.y() == dst.GetTop() && src.width() == dst.GetWidth() &&
           src.height() == dst.GetHeight();
}
#else
inline static bool isEqualRect(const Drawing::Rect& src, const RectI& dst)
{
    return src.GetLeft() == dst.GetLeft() && src.GetTop() == dst.GetTop() && src.GetWidth() == dst.GetWidth() &&
           src.GetHeight() == dst.GetHeight();
}
#endif

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(const std::shared_ptr<RSFilter>& filter)
{
    auto filterHash = filter->Hash();
    if (RSFilterCacheTask::FilterPartialRenderEnabled && cachedSnapshot_ != nullptr && filter->IsPartialValid() &&
        cachedFilterHash_ != filterHash && cachedSnapshot_->cachedImage_ != nullptr) {
#ifndef USE_ROSEN_DRAWING
        auto rsFilter = std::static_pointer_cast<RSSkiaFilter>(filter);
#else
        auto rsFilter = std::static_pointer_cast<RSDrawingFilter>(filter);
#endif
        PostPartialFilterRenderTask(rsFilter);
    }
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

#ifdef NEW_SKIA
bool RSFilterCacheManager::RSFilterCacheTask::InitSurface(GrRecordingContext* grContext)
#else
bool RSFilterCacheManager::RSFilterCacheTask::InitSurface(GrContext* grContext)
#endif
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    RS_TRACE_NAME("InitSurface");
    if (cacheSurface_ != nullptr) {
        return true;
    }
    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    SkImageInfo info = SkImageInfo::MakeN32Premul(surfaceSize_.width(), surfaceSize_.height());
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);
    isFirstInit = true;
    return cacheSurface_ != nullptr;
}

bool RSFilterCacheManager::RSFilterCacheTask::Render()
{
    RS_TRACE_NAME_FMT("RSFilterCacheManager::RSFilterCacheTask::Render:%p", this);
    ROSEN_LOGD("RSFilterCacheManager::RSFilterCacheTask::Render:%{public}p", this);
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    if (cacheSurface_ == nullptr || filter_ == nullptr) {
        return false;
    }
    CHECK_CACHE_PROCESS_STATUS;
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    if (cacheCanvas == nullptr) {
        ROSEN_LOGD("RSFilterCacheManager::Render: cacheCanvas is null");
        return false;
    }
    auto threadImage = SkImage::MakeFromTexture(cacheCanvas->recordingContext(), cacheBackendTexture_,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
    auto src = SkRect::MakeSize(SkSize::Make(surfaceSize_));
    auto dst = SkRect::MakeSize(SkSize::Make(surfaceSize_));
    filter_->DrawImageRect(*cacheCanvas, threadImage, src, dst);
    filter_->PostProcess(*cacheCanvas);
    CHECK_CACHE_PROCESS_STATUS;
    if (isFirstInit) {
        cacheSurface_->flushAndSubmit(true);
        isFirstInit = false;
    } else {
        cacheSurface_->flush();
    }
    CHECK_CACHE_PROCESS_STATUS;
    resultBackendTexture_ =
        cacheSurface_->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
    CHECK_CACHE_PROCESS_STATUS;
    cacheProcessStatus_.store(CacheProcessStatus::DONE);
    Notify();
    return true;
}

bool RSFilterCacheManager::RSFilterCacheTask::WaitTaskFinished()
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    if (GetStatus() != CacheProcessStatus::DOING) {
        return GetStatus() == CacheProcessStatus::DONE;
    }
    const long TIME_OUT = 2; // 2 milliseconds
    if (!cvParallelRender_.wait_for(lock, std::chrono::milliseconds(TIME_OUT),
        [this] { return cacheProcessStatus_ == CacheProcessStatus::DONE; })) {
        RS_OPTIONAL_TRACE_NAME("WaitTaskFinished, wait failed, CacheProcessStatus_:" +
            std::to_string(static_cast<unsigned int>(cacheProcessStatus_.load())));
        return false;
    }
    RS_OPTIONAL_TRACE_NAME("WaitTaskFinished, wait success, CacheProcessStatus_:" +
        std::to_string(static_cast<unsigned int>(cacheProcessStatus_.load())));
    return true;
}

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::PostPartialFilterRenderTask(const std::shared_ptr<RSSkiaFilter>& filter)
#else
void RSFilterCacheManager::PostPartialFilterRenderTask(const std::shared_ptr<RSDrawingFilter>& filter)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    // Prepare a backup of common resources for threads
    if (RSFilter::postTask != nullptr && task_->GetStatus() == CacheProcessStatus::WAITING) {
        // Because the screenshot is zoomed out, here you need to zoom in
#ifndef USE_ROSEN_DRAWING
        auto dstCopy = cachedSnapshot_->cachedRect_.makeOutset(1, 1);
#else
        auto dstCopy = cachedSnapshot_->cachedRect_.MakeOutset(1, 1);
#endif
        task_->InitTask(filter, cachedSnapshot_, dstCopy.size());
        task_->SetStatus(CacheProcessStatus::DOING);
        RSFilter::postTask(task_);
    } else {
        ROSEN_LOGD("RSFilterCacheManager::PostPartialFilterRenderTask: postTask is null");
    }
}

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
    const bool needSnapshotOutset, const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect)
#else
void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
    const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.getDeviceClipBounds().isEmpty()) {
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.isEmpty() || dst.isEmpty()) {
        return;
    }
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
#ifndef USE_ROSEN_DRAWING
        TakeSnapshot(canvas, filter, src, needSnapshotOutset);
#else
        TakeSnapshot(canvas, filter, src);
#endif
    } else {
        --cacheUpdateInterval_;
    }

    bool shouldClearFilteredCache = false;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        auto previousFilterHash = cachedFilterHash_;
        if (RSFilterCacheTask::FilterPartialRenderEnabled && task_->WaitTaskFinished()) {
            auto filteredSnapshot = SkImage::MakeFromTexture(canvas.recordingContext(), task_->GetResultTexture(),
                kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            auto filteredRect = dst;
            if (RSSystemProperties::GetImageGpuResourceCacheEnable(
                filteredSnapshot->width(), filteredSnapshot->height())) {
                ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
                    filteredSnapshot->width(), filteredSnapshot->height());
                as_IB(filteredSnapshot)->hintCacheGpuResource();
            }
            cachedFilteredSnapshot_ =
                std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), filteredRect);
            cachedFilterHash_ = filter->Hash();
            task_->SetStatus(CacheProcessStatus::WAITING);
        } else {
            task_->SetStatus(CacheProcessStatus::WAITING);
            GenerateFilteredSnapshot(canvas, filter, dst);
        }
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
    }
    DrawCachedFilteredSnapshot(canvas, dst);
    // To reduce the memory consumption, we only keep either the cached snapshot or the filtered image.
    CompactCache(shouldClearFilteredCache);
}

#ifndef USE_ROSEN_DRAWING
const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect,
    const std::optional<SkIRect>& dstRect)
#else
const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
#ifndef USE_ROSEN_DRAWING
    if (canvas.getDeviceClipBounds().isEmpty()) {
#else
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
#endif
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
#ifndef USE_ROSEN_DRAWING
    if (src.isEmpty() || dst.isEmpty()) {
#else
    if (src.IsEmpty() || dst.IsEmpty()) {
#endif
        return nullptr;
    }
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else {
        --cacheUpdateInterval_;
    }

    bool shouldClearFilteredCache = false;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        auto previousFilterHash = cachedFilterHash_;
        GenerateFilteredSnapshot(canvas, filter, dst);
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
    }
    // Keep a reference to the cached image, since CompactCache may invalidate it.
    auto cachedFilteredSnapshot = cachedFilteredSnapshot_;
    // To reduce the memory consumption, we only keep either the cached snapshot or the filtered image.
    CompactCache(shouldClearFilteredCache);
    return cachedFilteredSnapshot;
}

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::TakeSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& srcRect,
    const bool needSnapshotOutset)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    SkIRect snapshotIBounds;
    if (needSnapshotOutset) {
        snapshotIBounds = srcRect.makeOutset(-1, -1);
    } else {
        snapshotIBounds = srcRect;
    }

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
    cachedSnapshot_ = std::make_unique<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    bool isLargeArea = IsLargeArea(srcRect.width(), srcRect.height());
    cacheUpdateInterval_ =
        isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
    cachedFilterHash_ = 0;
}
#else
void RSFilterCacheManager::TakeSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& srcRect)
{
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    auto snapshotIBounds = srcRect;
    snapshotIBounds.MakeOutset(-1, -1);

    // Take a screenshot.
    auto snapshot = drawingSurface->MakeImageSnapshot(snapshotIBounds);
    if (snapshot == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(snapshot->GetWidth(), snapshot->GetHeight())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).", snapshot->GetWidth(),
            snapshot->GetHeight());
    }
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight());
    cachedSnapshot_ = std::make_unique<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    bool isLargeArea = IsLargeArea(srcRect.GetWidth(), srcRect.GetHeight());
    cacheUpdateInterval_ =
        isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
    cachedFilterHash_ = 0;
}
#endif

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
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), offscreenRect);
    cachedFilterHash_ = filter->Hash();
}

#ifndef USE_ROSEN_DRAWING
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
#else
void RSFilterCacheManager::DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect) const
{
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // Draw in device coordinates.
    RSAutoCanvasRestore autoRestore(&canvas);
    canvas.ResetMatrix();

    // Only draw within the visible rect.
    ClipVisibleRect(canvas);

    // The cache type and parameters has been validated, dstRect must be subset of cachedFilteredSnapshot_->cachedRect_.
    Drawing::Rect dst = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    Drawing::Rect src = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    src.Offset(-cachedFilteredSnapshot_->cachedRect_.GetLeft(), -cachedFilteredSnapshot_->cachedRect_.GetTop());

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*cachedFilteredSnapshot_->cachedImage_, src, dst, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}
#endif

void RSFilterCacheManager::InvalidateCache(CacheType cacheType)
{
    // bitwise test
    if (cacheType & CacheType::CACHE_TYPE_SNAPSHOT) {
        cachedSnapshot_.reset();
    }
    if (cacheType & CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        cachedFilteredSnapshot_.reset();
    }
    task_->SetStatus(CacheProcessStatus::WAITING);
}

void RSFilterCacheManager::ReleaseCacheOffTree()
{
    RS_TRACE_NAME_FMT("RSFilterCacheManager::ReleaseCacheOffTree:%p", this);
    cachedSnapshot_.reset();
    cachedFilteredSnapshot_.reset();
    task_->SetStatus(CacheProcessStatus::WAITING);
    task_->Reset();
    if (task_->GetHandler() != nullptr) {
        auto task_tmp = task_;
        task_->GetHandler()->PostTask(
            [task_tmp]() { task_tmp->ResetGrContext(); }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#ifndef USE_ROSEN_DRAWING
inline void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleIRect = canvas.GetVisibleRect().round();
    auto deviceClipRect = canvas.getDeviceClipBounds();
    if (!visibleIRect.isEmpty() && deviceClipRect.intersect(visibleIRect)) {
        canvas.clipIRect(visibleIRect);
    }
}
#else
inline void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleRectF = canvas.GetVisibleRect();
    visibleRectF.Round();
    Drawing::RectI visibleIRect = {(int)visibleRectF.GetLeft(), (int)visibleRectF.GetTop(),
                                   (int)visibleRectF.GetRight(), (int)visibleRectF.GetBottom()}
    auto deviceClipRect = canvas.GetDeviceClipBounds();
    if (!visibleIRect.IsEmpty() && deviceClipRect.Intersect(visibleIRect)) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
}
#endif

const RectI& RSFilterCacheManager::GetCachedImageRegion() const
{
    static const auto emptyRect = RectI();
    return IsCacheValid() ? snapshotRegion_ : emptyRect;
}

inline static bool IsCacheInvalid(const RSPaintFilterCanvas::CachedEffectData& cache, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    return cache.cachedImage_ == nullptr || !cache.cachedImage_->isValid(canvas.recordingContext());
#else
    return cache.cachedImage_ == nullptr || !cache.cachedImage_->IsValid(canvas.GetGPUContext().get());
#endif
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

#ifndef USE_ROSEN_DRAWING
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
    if (snapshotRegion_.GetLeft() > dst.x() || snapshotRegion_.GetRight() < dst.right() ||
        snapshotRegion_.GetTop() > dst.y() || snapshotRegion_.GetBottom() < dst.bottom()) {
        // dst region is out of snapshot region, cache is invalid.
        // It should already be checked by UpdateCacheStateWithFilterRegion in prepare phase, we should never be here.
        ROSEN_LOGD("RSFilterCacheManager::ValidateParams Cache expired. Reason: dst region is out of snapshot region.");
        InvalidateCache();
    }
    return { src, dst };
}
#else
std::tuple<Drawing::RectI, Drawing::RectI> RSFilterCacheManager::ValidateParams(
    RSPaintFilterCanvas& canvas, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    Drawing::RectI src;
    Drawing::RectI dst;
    auto deviceRect = Drawing::RectI(0, 0, canvas.GetImageInfo().GetWidth(), canvas.GetImageInfo().GetHight());
    if (!srcRect.has_value()) {
        src = canvas.GetDeviceClipBounds();
    } else {
        src = srcRect.value();
        src.Intersect(deviceRect);
    }
    if (!dstRect.has_value()) {
        dst = src;
    } else {
        dst = dstRect.value();
        dst.Intersect(deviceRect);
    }
    if (snapshotRegion_.GetLeft() > dst.GetLeft() || snapshotRegion_.GetRight() < dst.GetRight() ||
        snapshotRegion_.GetTop() > dst.GetTop() || snapshotRegion_.GetBottom() < dst.GetBottom()) {
        // dst region is out of snapshot region, cache is invalid.
        // It should already be checked by UpdateCacheStateWithFilterRegion in prepare phase, we should never be here.
        ROSEN_LOGD("RSFilterCacheManager::ValidateParams Cache expired. Reason: dst region is out of snapshot region.");
        InvalidateCache();
    }
    return { src, dst };
}
#endif

inline void RSFilterCacheManager::CompactCache(bool shouldClearFilteredCache)
{
    if (shouldClearFilteredCache) {
        cachedFilteredSnapshot_.reset();
    } else {
        task_->Reset();
        cachedSnapshot_.reset();
    }
}
} // namespace Rosen
} // namespace OHOS
#endif
