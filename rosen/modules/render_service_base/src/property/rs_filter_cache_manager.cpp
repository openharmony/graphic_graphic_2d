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
#include "render/rs_filter.h"

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
constexpr float PARALLEL_FILTER_RATIO_THRESHOLD = 0.8f;
constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 5;
const char* RSFilterCacheManager::GetCacheState() const
{
    if (cachedFilteredSnapshot_ != nullptr) {
        return "Filtered image found in cache. Reusing cached result.";
    } else if (cachedSnapshot_ != nullptr) {
        return "Snapshot found in cache. Generating filtered image using cached data.";
    } else {
        return "No valid cache found.";
    }
}

#define CHECK_CACHE_PROCESS_STATUS                      \
    do {                                                \
        if (GetStatus() != CacheProcessStatus::DOING) { \
            return false;                               \
        }                                               \
    } while (false)
#ifndef ROSEN_ARKUI_X
const bool RSFilterCacheManager::RSFilterCacheTask::FilterPartialRenderEnabled =
    RSSystemProperties::GetFilterPartialRenderEnabled() && RSUniRenderJudgement::IsUniRender();
#else
const bool RSFilterCacheManager::RSFilterCacheTask::FilterPartialRenderEnabled = false;
#endif
bool RSFilterCacheManager::SoloTaskPrepare = true;
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
inline static bool isEqualRect(const Drawing::RectI& src, const RectI& dst)
{
    return src.GetLeft() == dst.GetLeft() && src.GetTop() == dst.GetTop() && src.GetWidth() == dst.GetWidth() &&
           src.GetHeight() == dst.GetHeight();
}
#endif

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(const std::shared_ptr<RSFilter>& filter)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto filterHash = filter->Hash();
    if (cachedFilteredSnapshot_ == nullptr || cachedFilterHash_ == filterHash) {
        return;
    }
    // filter changed, clear the filtered snapshot.
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered hash "
               "%{public}X does not match new hash %{public}X.",
        cachedFilterHash_, filterHash);
    cachedFilteredSnapshot_.reset();
}
#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::PostPartialFilterRenderInit(
    const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect, int32_t canvasWidth, int32_t canvasHeight)
#else
void RSFilterCacheManager::PostPartialFilterRenderInit(const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& dstRect, int32_t canvasWidth, int32_t canvasHeight)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    task_->SetCompleted(task_->GetStatus() == CacheProcessStatus::DONE);
    if (task_->GetStatus() == CacheProcessStatus::DOING) {
        task_->SetCompleted(!task_->isFirstInit_);
    }
    if (task_->GetStatus() == CacheProcessStatus::DONE) {
        task_->Reset();
        task_->isFirstInit_ = false;
        task_->SwapTexture();
        task_->SetStatus(CacheProcessStatus::WAITING);
    }
    if (task_->isTaskRelease_.load()) {
        return;
    }
    if (task_->cachedFirstFilter_ != nullptr && task_->isFirstInit_ &&
        task_->GetStatus() == CacheProcessStatus::DOING) {
        cachedFilteredSnapshot_ = task_->cachedFirstFilter_;
    } else {
        task_->isFirstInit_ = true;
        task_->cachedFirstFilter_ = nullptr;
    }
    if (RSFilterCacheTask::FilterPartialRenderEnabled &&
        (cachedSnapshot_ != nullptr && cachedSnapshot_->cachedImage_ != nullptr) &&
        (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) &&
#ifndef USE_ROSEN_DRAWING
        IsNearlyFullScreen(cachedSnapshot_->cachedRect_.size(), canvasWidth, canvasHeight)) {
#else
        IsNearlyFullScreen(cachedSnapshot_->cachedRect_, canvasWidth, canvasHeight)) {
#endif
        PostPartialFilterRenderTask(filter, dstRect);
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

bool RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(const RSDirtyRegionManager& dirtyManager)
{
    if (!IsCacheValid()) {
        return false;
    }
    RS_OPTIONAL_TRACE_FUNC();
    auto& cachedImageRect = GetCachedImageRegion();
    if (dirtyManager.currentFrameDirtyRegion_.Intersect(cachedImageRect) ||
        std::any_of(dirtyManager.visitedDirtyRegions_.begin(), dirtyManager.visitedDirtyRegions_.end(),
            [&cachedImageRect](const RectI& rect) { return rect.Intersect(cachedImageRect); })) {
        // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache
        // age. [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
        if (cacheUpdateInterval_ > 0) {
            ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
                       "invalidation for %{public}d frames.",
                cacheUpdateInterval_);
            pendingPurge_ = true;
        } else {
            InvalidateCache();
        }
        return false;
    }
    if (pendingPurge_) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion MergeDirtyRect at %{public}d frames",
            cacheUpdateInterval_);
        InvalidateCache();
        return true;
    } else {
        return false;
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSFilterCacheManager::RSFilterCacheTask::InitSurface(GrRecordingContext* grContext)
#else
bool RSFilterCacheManager::RSFilterCacheTask::InitSurface(Drawing::GPUContext* grContext)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    if (!needClearSurface_) {
        return true;
    }
    cacheSurface_ = nullptr;

    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::MakeN32Premul(dstRect_.width(), dstRect_.height());
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);
#else
    Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(dstRect_.GetWidth(), dstRect_.GetHeight());
    cacheSurface_ = Drawing::Surface::MakeRenderTarget(grContext, true, info);
#endif
    return cacheSurface_ != nullptr;
}

bool RSFilterCacheManager::RSFilterCacheTask::Render()
{
    RS_TRACE_NAME_FMT("RSFilterCacheManager::RSFilterCacheTask::Render:%p", this);
    ROSEN_LOGD("RSFilterCacheManager::RSFilterCacheTask::Render:%{public}p", this);
    if (cacheSurface_ == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSFilterCacheManager::Render: cacheSurface_ is null");
        return false;
    }
    CHECK_CACHE_PROCESS_STATUS;
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSFilterCacheManager::Render: cacheCanvas is null");
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    GrSurfaceOrigin surfaceOrigin = kTopLeft_GrSurfaceOrigin;
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        surfaceOrigin = kTopLeft_GrSurfaceOrigin;
    } else {
        surfaceOrigin = kBottomLeft_GrSurfaceOrigin;
    }
#else
    surfaceOrigin = kBottomLeft_GrSurfaceOrigin;
#endif // RS_ENABLE_VK
    auto threadImage = SkImage::MakeFromTexture(cacheCanvas->recordingContext(), cacheBackendTexture_, surfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
    if (!threadImage) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSFilterCacheManager::Render: threadImage is null");
        return false;
    }
    auto src = SkRect::MakeSize(SkSize::Make(snapshotSize_));
    auto dst = SkRect::MakeSize(SkSize::Make(dstRect_.size()));
#else // USE_ROSEN_DRAWING
    Drawing::TextureOrigin surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
    } else {
        surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
    }
#else
    surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif // RS_ENABLE_VK
    Drawing::BitmapFormat bitmapFormat = { cacheBackendTextureColorType_.load(),
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto threadImage = std::make_shared<Drawing::Image>();
    CHECK_CACHE_PROCESS_STATUS;
    {
        std::unique_lock<std::mutex> lock(grBackendTextureMutex_);
        if (cachedSnapshotInTask_ == nullptr ||
            !threadImage->BuildFromTexture(*cacheCanvas->GetGPUContext(), cacheBackendTexture_.GetTextureInfo(),
                surfaceOrigin, bitmapFormat, nullptr)) {
            SetStatus(CacheProcessStatus::WAITING);
            ROSEN_LOGE("RSFilterCacheManager::Render: BuildFromTexture is null");
            return false;
        }
    }
    auto src = Drawing::Rect(0, 0, snapshotSize_.GetWidth(), snapshotSize_.GetHeight());
    auto dst = Drawing::Rect(0, 0, dstRect_.GetWidth(), dstRect_.GetHeight());
#endif // USE_ROSEN_DRAWING
    CHECK_CACHE_PROCESS_STATUS;
    filter_->DrawImageRect(*cacheCanvas, threadImage, src, dst);
    filter_->PostProcess(*cacheCanvas);
    CHECK_CACHE_PROCESS_STATUS;
    return true;
}

bool RSFilterCacheManager::RSFilterCacheTask::SaveFilteredImage()
{
    CHECK_CACHE_PROCESS_STATUS;
#ifndef USE_ROSEN_DRAWING
    resultBackendTexture_ =
        cacheSurface_->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
#else
    if (cacheSurface_) {
        resultBackendTexture_ = cacheSurface_->GetBackendTexture();
    } else {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSFilterCacheManager::SaveFilteredImage: cacheSurface_ is null");
        return false;
    }
#endif
    CHECK_CACHE_PROCESS_STATUS;
    return true;
}

void RSFilterCacheManager::RSFilterCacheTask::SwapInit()
{
    RS_OPTIONAL_TRACE_FUNC();
    std::unique_lock<std::mutex> lock(grBackendTextureMutex_);
    std::swap(filter_, filterBefore_);
    std::swap(cachedSnapshotInTask_, cachedSnapshotBefore_);
    std::swap(snapshotSize_, snapshotSizeBefore_);
    std::swap(dstRect_, dstRectBefore_);
    if (cachedSnapshotInTask_ != nullptr) {
#ifndef USE_ROSEN_DRAWING
        cacheBackendTexture_ = cachedSnapshotInTask_->cachedImage_->getBackendTexture(false);
#else
        cacheBackendTexture_ = cachedSnapshotInTask_->cachedImage_->GetBackendTexture(false, nullptr);
#endif
        cacheBackendTextureColorType_ = cachedSnapshotInTask_->cachedImage_->GetColorType();
    }
}

bool RSFilterCacheManager::RSFilterCacheTask::SetDone()
{
    RS_OPTIONAL_TRACE_FUNC();
    CHECK_CACHE_PROCESS_STATUS;
    if (isTaskRelease_.load()) {
        SetStatus(CacheProcessStatus::WAITING);
    } else {
        SetStatus(CacheProcessStatus::DONE);
    }
    return true;
}
#ifndef USE_ROSEN_DRAWING
bool RSFilterCacheManager::IsNearlyFullScreen(SkISize imageSize, int32_t canvasWidth, int32_t canvasHeight)
#else
bool RSFilterCacheManager::IsNearlyFullScreen(Drawing::RectI imageSize, int32_t canvasWidth, int32_t canvasHeight)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    auto widthThreshold = static_cast<int32_t>(canvasWidth * PARALLEL_FILTER_RATIO_THRESHOLD);
    auto heightThreshold = static_cast<int32_t>(canvasHeight * PARALLEL_FILTER_RATIO_THRESHOLD);
#ifndef USE_ROSEN_DRAWING
    return imageSize.width() >= widthThreshold && imageSize.height() >= heightThreshold;
#else
    return imageSize.GetWidth() >= widthThreshold && imageSize.GetHeight() >= heightThreshold;
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::PostPartialFilterRenderTask(
    const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect)
#else
void RSFilterCacheManager::PostPartialFilterRenderTask(
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
    // Prepare a backup of common resources for threads
    if (RSFilter::postTask != nullptr && (task_->GetStatus() == CacheProcessStatus::WAITING)) {
        // Because the screenshot is zoomed out, here you need to zoom in
        task_->InitTask(filter, cachedSnapshot_, dstRect);
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
void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const bool needSnapshotOutset, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
#ifndef USE_ROSEN_DRAWING
    if (canvas.getDeviceClipBounds().isEmpty()) {
#else
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
#endif
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
#ifndef USE_ROSEN_DRAWING
    if (src.isEmpty() || dst.isEmpty()) {
#else
    if (src.IsEmpty() || dst.IsEmpty()) {
#endif
        return;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::DrawFilter status: %s", GetCacheState());
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src, needSnapshotOutset);
    } else {
        --cacheUpdateInterval_;
    }
#ifndef USE_ROSEN_DRAWING
    auto surface = canvas.getSurface();
    auto width = surface->width();
    auto height = surface->height();
    if (filter->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR) {
        SkMatrix mat = canvas.getTotalMatrix();
        filter->SetCanvasChange(mat, width, height);
    }
#else
    auto surface = canvas.GetSurface();
    auto width = surface->Width();
    auto height = surface->Height();
    if (filter->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR) {
        Drawing::Matrix mat = canvas.GetTotalMatrix();
        filter->SetCanvasChange(mat, width, height);
    }
#endif
    PostPartialFilterRenderInit(filter, dst, width, height);
    bool shouldClearFilteredCache = false;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        auto previousFilterHash = cachedFilterHash_;
        if (RSFilterCacheTask::FilterPartialRenderEnabled && task_->IsCompleted()) {
            FilterPartialRender(canvas, filter, dst);
        } else {
            GenerateFilteredSnapshot(canvas, filter, dst);
        }
        newCache_ = true;
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
    } else if (RSFilterCacheTask::FilterPartialRenderEnabled && task_->IsCompleted()) {
        FilterPartialRender(canvas, filter, dst);
    } else {
        newCache_ = false;
    }
    if (task_->GetStatus() == CacheProcessStatus::DOING && task_->isFirstInit_ &&
        task_->cachedFirstFilter_ == nullptr) {
        task_->cachedFirstFilter_ = cachedFilteredSnapshot_;
    }
    DrawCachedFilteredSnapshot(canvas, dst);
    if (filter->GetFilterType() == RSFilter::AIBAR) {
        shouldClearFilteredCache = false;
    }
    task_->SetCompleted(false);
    // To reduce the memory consumption, we only keep either the cached snapshot or the filtered image.
    CompactCache(shouldClearFilteredCache);
}

#ifndef USE_ROSEN_DRAWING
const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect,
    const std::optional<SkIRect>& dstRect, const std::tuple<bool, bool>& forceCacheFlags)
#else
const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect,
    const std::tuple<bool, bool>& forceCacheFlags)
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
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect, forceCacheFlags);
#ifndef USE_ROSEN_DRAWING
    if (src.isEmpty() || dst.isEmpty()) {
#else
    if (src.IsEmpty() || dst.IsEmpty()) {
#endif
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::GeneratedCachedEffectData status: %s", GetCacheState());
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
    if (std::get<0>(forceCacheFlags) || std::get<1>(forceCacheFlags)) {
        shouldClearFilteredCache = false;
    }
    CompactCache(shouldClearFilteredCache);
    return cachedFilteredSnapshot;
}

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
    const SkIRect& srcRect, const bool needSnapshotOutset)
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
        ROSEN_LOGD("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
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
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    if (filter->GetFilterType() == RSFilter::AIBAR) {
        // when dirty region change, delay 5 frames to update
        cacheUpdateInterval_ = AIBAR_CACHE_UPDATE_INTERVAL;
    } else {
        bool isLargeArea = IsLargeArea(srcRect.width(), srcRect.height());
        cacheUpdateInterval_ =
            isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
    }
    cachedFilterHash_ = 0;
    pendingPurge_ = false;
}
#else
void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& srcRect, const bool needSnapshotOutset)
{
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    Drawing::RectI snapshotIBounds;
    snapshotIBounds = srcRect;
    if (needSnapshotOutset) {
        snapshotIBounds.MakeOutset(-1, -1);
    }

    // Take a screenshot.
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds);
    if (snapshot == nullptr) {
        ROSEN_LOGD("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(snapshot->GetWidth(), snapshot->GetHeight())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).", snapshot->GetWidth(),
            snapshot->GetHeight());
        as_IB(snapshot->ExportSkImage().get())->hintCacheGpuResource();
    }
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight());
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    if (filter->GetFilterType() == RSFilter::AIBAR) {
        // when dirty region change, delay 5 frames to update
        cacheUpdateInterval_ = AIBAR_CACHE_UPDATE_INTERVAL;
    } else {
        bool isLargeArea = IsLargeArea(srcRect.GetWidth(), srcRect.GetHeight());
        cacheUpdateInterval_ =
            isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
    }
    cachedFilterHash_ = 0;
    pendingPurge_ = false;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSFilterCacheManager::FilterPartialRender(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect)
#else
void RSFilterCacheManager::FilterPartialRender(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
#endif
{
    RS_OPTIONAL_TRACE_FUNC();
#ifndef USE_ROSEN_DRAWING
    auto filteredSnapshot = SkImage::MakeFromTexture(canvas.recordingContext(), task_->GetResultTexture(),
        kBottomLeft_GrSurfaceOrigin, COLORTYPE_N32, kPremul_SkAlphaType, nullptr);
#else
    auto filteredSnapshot = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL };
    filteredSnapshot->BuildFromTexture(*canvas.GetGPUContext(), task_->GetResultTexture().GetTextureInfo(),
        Drawing::TextureOrigin::BOTTOM_LEFT, bitmapFormat, nullptr);
#endif
    if (filteredSnapshot == nullptr) {
        GenerateFilteredSnapshot(canvas, filter, dstRect);
    } else {
#ifndef USE_ROSEN_DRAWING
        auto filteredRect = SkIRect::MakeWH(filteredSnapshot->width(), filteredSnapshot->height());
        if (RSSystemProperties::GetImageGpuResourceCacheEnable(filteredSnapshot->width(), filteredSnapshot->height())) {
            ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
                filteredSnapshot->width(), filteredSnapshot->height());
            as_IB(filteredSnapshot)->hintCacheGpuResource();
        }
#else
        auto filteredRect = Drawing::RectI(0, 0, filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
        if (RSSystemProperties::GetImageGpuResourceCacheEnable(
                filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight())) {
            ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
                filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
            as_IB(filteredSnapshot->ExportSkImage().get())->hintCacheGpuResource();
        }
#endif
        cachedFilteredSnapshot_.reset();
        cachedFilteredSnapshot_ =
            std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), task_->GetDstRect());
        cachedFilterHash_ = filter->Hash();
    }
}

#ifndef USE_ROSEN_DRAWING
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
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (filteredSnapshot->isTextureBacked()) {
            RS_LOGI("RSFilterCacheManager::GenerateFilteredSnapshot cachedImage from texture to raster image");
            filteredSnapshot = filteredSnapshot->makeRasterImage();
        }
    }
    cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), offscreenRect);
    cachedFilterHash_ = filter->Hash();
}
#else
void RSFilterCacheManager::GenerateFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
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
    auto offscreenSurface = surface->MakeSurface(offscreenRect.GetWidth(), offscreenRect.GetHeight());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Src rect and dst rect, with origin at (0, 0).
    auto src = Drawing::Rect(0, 0, cachedSnapshot_->cachedRect_.GetWidth(), cachedSnapshot_->cachedRect_.GetHeight());
    auto dst = Drawing::Rect(0, 0, offscreenRect.GetWidth(), offscreenRect.GetHeight());

    // Draw the cached snapshot on the offscreen canvas, apply the filter, and post-process.
    filter->DrawImageRect(offscreenCanvas, cachedSnapshot_->cachedImage_, src, dst);
    filter->PostProcess(offscreenCanvas);

    // Update the cache state with the filtered snapshot.
    auto filteredSnapshot = offscreenSurface->GetImageSnapshot();
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(
            filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight())) {
        ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
        as_IB(filteredSnapshot->ExportSkImage().get())->hintCacheGpuResource();
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (filteredSnapshot->IsTextureBacked()) {
            RS_LOGI("RSFilterCacheManager::GenerateFilteredSnapshot cachedImage from texture to raster image");
            filteredSnapshot = filteredSnapshot->MakeRasterImage();
        }
    }
    cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), offscreenRect);
    cachedFilterHash_ = filter->Hash();
}
#endif

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
    SkRect dst = SkRect::Make(dstRect);
    SkRect src;
    if (newCache_) {
        src = SkRect::MakeSize(SkSize::Make(cachedFilteredSnapshot_->cachedImage_->dimensions()));
    } else {
        src = SkRect::Make(dstRect.makeOffset(-cachedFilteredSnapshot_->cachedRect_.topLeft()));
        src.intersect(SkRect::Make(cachedFilteredSnapshot_->cachedImage_->bounds()));
    }

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
    Drawing::AutoCanvasRestore autoRestore(canvas, true);
    canvas.ResetMatrix();

    // Only draw within the visible rect.
    ClipVisibleRect(canvas);

    // The cache type and parameters has been validated, dstRect must be subset of cachedFilteredSnapshot_->cachedRect_.
    Drawing::Rect dst(dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom());
    Drawing::Rect src;
    if (newCache_) {
        src = { 0, 0, cachedFilteredSnapshot_->cachedImage_->GetWidth(),
            cachedFilteredSnapshot_->cachedImage_->GetHeight() };
    } else {
        src = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom() };
        src.Offset(-cachedFilteredSnapshot_->cachedRect_.GetLeft(), -cachedFilteredSnapshot_->cachedRect_.GetTop());
        src.Intersect(Drawing::Rect(0, 0, cachedFilteredSnapshot_->cachedImage_->GetWidth(),
            cachedFilteredSnapshot_->cachedImage_->GetHeight()));
    }

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
}

void RSFilterCacheManager::ReleaseCacheOffTree()
{
    RS_OPTIONAL_TRACE_FUNC();
    ROSEN_LOGD("RSFilterCacheManager::ReleaseCacheOffTree task_:%{public}p", task_.get());
    std::unique_lock<std::mutex> lock(task_->grBackendTextureMutex_);
    cachedSnapshot_.reset();
    cachedFilteredSnapshot_.reset();
    newCache_ = false;
    task_->isTaskRelease_.store(true);
    task_->cachedFirstFilter_ = nullptr;
    task_->ResetInTask();
    task_->SetCompleted(false);
    task_->isFirstInit_ = true;
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
    Drawing::RectI visibleIRect = { (int)visibleRectF.GetLeft(), (int)visibleRectF.GetTop(),
        (int)visibleRectF.GetRight(), (int)visibleRectF.GetBottom() };
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
        ROSEN_LOGE("RSFilterCacheManager::CheckCachedImages cachedSnapshot_ is invalid");
        cachedSnapshot_.reset();
    }
    if (cachedFilteredSnapshot_ != nullptr && IsCacheInvalid(*cachedFilteredSnapshot_, canvas)) {
        ROSEN_LOGE("RSFilterCacheManager::CheckCachedImages cachedFilteredSnapshot_ is invalid");
        cachedFilteredSnapshot_.reset();
    }
}

#ifndef USE_ROSEN_DRAWING
std::tuple<SkIRect, SkIRect> RSFilterCacheManager::ValidateParams(
    RSPaintFilterCanvas& canvas, const std::optional<SkIRect>& srcRect,
    const std::optional<SkIRect>& dstRect, const std::tuple<bool, bool>& forceCacheFlags)
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
        ROSEN_LOGE("RSFilterCacheManager::ValidateParams Cache expired. Reason: dst region is out of snapshot region.");
    }
    return { src, dst };
}
#else
std::tuple<Drawing::RectI, Drawing::RectI> RSFilterCacheManager::ValidateParams(RSPaintFilterCanvas& canvas,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect,
    const std::tuple<bool, bool>& forceCacheFlags)
{
    Drawing::RectI src;
    Drawing::RectI dst;
    auto deviceRect = Drawing::RectI(0, 0, canvas.GetImageInfo().GetWidth(), canvas.GetImageInfo().GetHeight());
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
        if (!std::get<0>(forceCacheFlags) || !std::get<1>(forceCacheFlags)) {
            InvalidateCache();
        }
    }
    return { src, dst };
}
#endif

inline void RSFilterCacheManager::CompactCache(bool shouldClearFilteredCache)
{
    if (pendingPurge_ && cacheUpdateInterval_ == 0) {
        InvalidateCache();
        return;
    }
    InvalidateCache(
        shouldClearFilteredCache ? CacheType::CACHE_TYPE_FILTERED_SNAPSHOT : CacheType::CACHE_TYPE_SNAPSHOT);
}
} // namespace Rosen
} // namespace OHOS
#endif
