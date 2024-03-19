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
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
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

inline static bool isEqualRect(const Drawing::RectI& src, const RectI& dst)
{
    return src.GetLeft() == dst.GetLeft() && src.GetTop() == dst.GetTop() && src.GetWidth() == dst.GetWidth() &&
           src.GetHeight() == dst.GetHeight();
}

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
void RSFilterCacheManager::PostPartialFilterRenderInit(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect, bool& shouldClearFilteredCache)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto surface = canvas.GetSurface();
    auto width = surface->Width();
    auto height = surface->Height();
    Drawing::Matrix mat = canvas.GetTotalMatrix();
    if (filter->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR) {
        filter->SetCanvasChange(mat, width, height);
    }
    auto directionBias = CalcDirectionBias(mat);
    auto previousSurfaceFlag = task_->surfaceFlag;
    task_->surfaceFlag = directionBias;
    if (previousSurfaceFlag != -1 && previousSurfaceFlag != task_->surfaceFlag) {
        StopFilterPartialRender();
    }
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
        shouldClearFilteredCache = IsClearFilteredCache(filter, dstRect);
    } else {
        task_->cachedFirstFilter_ = nullptr;
    }
    if (RSFilterCacheTask::FilterPartialRenderEnabled &&
        (cachedSnapshot_ != nullptr && cachedSnapshot_->cachedImage_ != nullptr) &&
        (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) &&
        IsNearlyFullScreen(cachedSnapshot_->cachedRect_, width, height)) {
        task_->isLastRender_ = false;
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

bool RSFilterCacheManager::RSFilterCacheTask::InitSurface(Drawing::GPUContext* grContext)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (!needClearSurface_) {
        return true;
    }
    cacheSurface_ = nullptr;

    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(dstRect_.GetWidth(), dstRect_.GetHeight());
    cacheSurface_ = Drawing::Surface::MakeRenderTarget(grContext, true, info);
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
    Drawing::TextureOrigin surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
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
    CHECK_CACHE_PROCESS_STATUS;
    filter_->DrawImageRect(*cacheCanvas, threadImage, src, dst);
    filter_->PostProcess(*cacheCanvas);
    CHECK_CACHE_PROCESS_STATUS;
    return true;
}

bool RSFilterCacheManager::RSFilterCacheTask::SaveFilteredImage()
{
    CHECK_CACHE_PROCESS_STATUS;
    if (cacheSurface_) {
        resultBackendTexture_ = cacheSurface_->GetBackendTexture();
    } else {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGE("RSFilterCacheManager::SaveFilteredImage: cacheSurface_ is null");
        return false;
    }
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
        cacheBackendTexture_ = cachedSnapshotInTask_->cachedImage_->GetBackendTexture(false, nullptr);
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
bool RSFilterCacheManager::IsNearlyFullScreen(Drawing::RectI imageSize, int32_t canvasWidth, int32_t canvasHeight)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto widthThreshold = static_cast<int32_t>(canvasWidth * PARALLEL_FILTER_RATIO_THRESHOLD);
    auto heightThreshold = static_cast<int32_t>(canvasHeight * PARALLEL_FILTER_RATIO_THRESHOLD);
    return imageSize.GetWidth() >= widthThreshold && imageSize.GetHeight() >= heightThreshold;
}

void RSFilterCacheManager::PostPartialFilterRenderTask(
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
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

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const bool needSnapshotOutset, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.IsEmpty() || dst.IsEmpty()) {
        return;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::DrawFilter status: %s", GetCacheState());
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src, needSnapshotOutset);
    } else {
        --cacheUpdateInterval_;
    }
    bool shouldClearFilteredCache = false;
    PostPartialFilterRenderInit(canvas, filter, dst, shouldClearFilteredCache);
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        if (RSFilterCacheTask::FilterPartialRenderEnabled && task_->IsCompleted()) {
            FilterPartialRender(canvas, filter, dst);
        } else {
            GenerateFilteredSnapshot(canvas, filter, dst);
        }
        newCache_ = true;
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = IsClearFilteredCache(filter, dst);
    } else if (RSFilterCacheTask::FilterPartialRenderEnabled && task_->IsCompleted()) {
        FilterPartialRender(canvas, filter, dst);
        if (!task_->isLastRender_) {
            cachedSnapshot_ = task_->cachedSnapshotInTask_;
            PostPartialFilterRenderTask(filter, dst);
            cachedSnapshot_ = nullptr;
            task_->isLastRender_ = true;
        }
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

uint8_t RSFilterCacheManager::CalcDirectionBias(const Drawing::Matrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
        // 0 and 4 represents rotate matrix's index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
        // 1 and 3 represents rotate matrix's index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect,
    const std::tuple<bool, bool>& forceCacheFlags)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect, forceCacheFlags);
    if (src.IsEmpty() || dst.IsEmpty()) {
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
        snapshot->HintCacheGpuResource();
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

void RSFilterCacheManager::FilterPartialRender(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto filteredSnapshot = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL };
    filteredSnapshot->BuildFromTexture(*canvas.GetGPUContext(), task_->GetResultTexture().GetTextureInfo(),
        Drawing::TextureOrigin::BOTTOM_LEFT, bitmapFormat, nullptr);
    if (filteredSnapshot == nullptr) {
        GenerateFilteredSnapshot(canvas, filter, dstRect);
    } else {
        auto filteredRect = Drawing::RectI(0, 0, filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
        if (RSSystemProperties::GetImageGpuResourceCacheEnable(
                filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight())) {
            ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
                filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
            filteredSnapshot->HintCacheGpuResource();
        }
        cachedFilteredSnapshot_.reset();
        cachedFilteredSnapshot_ =
            std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), task_->GetDstRect());
    }
}

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
        filteredSnapshot->HintCacheGpuResource();
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (filteredSnapshot->IsTextureBacked()) {
            RS_LOGI("RSFilterCacheManager::GenerateFilteredSnapshot cachedImage from texture to raster image");
            filteredSnapshot = filteredSnapshot->MakeRasterImage();
        }
    }
    cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), offscreenRect);
}

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
    cachedSnapshot_.reset();
    cachedFilteredSnapshot_.reset();
    StopFilterPartialRender();
}

void RSFilterCacheManager::StopFilterPartialRender()
{
    RS_OPTIONAL_TRACE_FUNC();
    std::unique_lock<std::mutex> lock(task_->grBackendTextureMutex_);
    newCache_ = false;
    task_->isTaskRelease_.store(true);
    task_->cachedFirstFilter_ = nullptr;
    task_->ResetInTask();
    task_->SetCompleted(false);
    task_->isFirstInit_ = true;
    task_->needClearSurface_ = false;
    task_->isLastRender_ = false;
    task_->surfaceFlag = -1;
    task_->SetStatus(CacheProcessStatus::WAITING);
    task_->Reset();
    if (task_->GetHandler() != nullptr) {
        auto task_tmp = task_;
        task_->GetHandler()->PostTask(
            [task_tmp]() { task_tmp->ResetGrContext(); }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

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

const RectI& RSFilterCacheManager::GetCachedImageRegion() const
{
    static const auto emptyRect = RectI();
    return IsCacheValid() ? snapshotRegion_ : emptyRect;
}

inline static bool IsCacheInvalid(const RSPaintFilterCanvas::CachedEffectData& cache, RSPaintFilterCanvas& canvas)
{
    return cache.cachedImage_ == nullptr || !cache.cachedImage_->IsValid(canvas.GetGPUContext().get());
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
        if (!std::get<0>(forceCacheFlags) && !std::get<1>(forceCacheFlags)) {
            InvalidateCache();
        }
    }
    return { src, dst };
}

inline bool RSFilterCacheManager::IsClearFilteredCache(
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dst)
{
    RS_TRACE_NAME_FMT("RSFilterCacheManager::ShouldClearFilteredCache::");
    auto previousFilterHash = cachedFilterHash_;
    cachedFilterHash_ = filter->Hash();
    return previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
}

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
