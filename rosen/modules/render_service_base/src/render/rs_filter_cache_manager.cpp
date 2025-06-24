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

#include "render/rs_filter_cache_manager.h"
#include "rs_trace.h"
#include "render/rs_filter.h"

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSurface.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_high_performance_visual_engine.h"
#include "render/rs_render_magnifier_filter.h"
#include "render/rs_skia_filter.h"
#include "drawable/rs_property_drawable_utils.h"

namespace OHOS {
namespace Rosen {

constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 5;
constexpr int ROTATION_CACHE_UPDATE_INTERVAL = 1;

bool RSFilterCacheManager::isCCMFilterCacheEnable_ = true;
bool RSFilterCacheManager::isCCMEffectMergeEnable_ = true;

RSFilterCacheManager::RSFilterCacheManager()
{
    hpaeCacheManager_ = std::make_shared<RSHpaeFilterCacheManager>();
}
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

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateFilterCache();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
                    "invalidation for %{public}d frames.",
            cacheUpdateInterval_);
        pendingPurge_ = true;
    } else {
        InvalidateFilterCache();
    }
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
            InvalidateFilterCache();
        }
        return false;
    }
    if (pendingPurge_) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion MergeDirtyRect at %{public}d frames",
            cacheUpdateInterval_);
        InvalidateFilterCache();
        return true;
    } else {
        return false;
    }
}

bool RSFilterCacheManager::CanDiscardCanvas(RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect) const
{
    /* Check that drawing will be in full canvas and no issues with clip */
    return (RSSystemProperties::GetDiscardCanvasBeforeFilterEnabled() && canvas.IsClipRect() &&
        canvas.GetDeviceClipBounds() == dstRect && canvas.GetWidth() == dstRect.GetWidth() &&
        canvas.GetHeight() == dstRect.GetHeight() && dstRect.GetLeft() == 0 && dstRect.GetTop() == 0);
}

bool RSFilterCacheManager::DrawFilterWithoutSnapshot(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& src, const Drawing::RectI& dst,
    bool shouldClearFilteredCache)
{
    if (!RSSystemProperties::GetDrawFilterWithoutSnapshotEnabled() || !shouldClearFilteredCache ||
        cachedSnapshot_ == nullptr || cachedSnapshot_->cachedImage_ == nullptr) {
        return false;
    }
    RS_OPTIONAL_TRACE_FUNC();

    /* Reuse code from RSPropertiesPainter::DrawFilter() when cache manager is not available */
    auto clipIBounds = dst;
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ResetMatrix();
    // Only draw within the visible rect.
    ClipVisibleRect(canvas);
    Drawing::Rect srcRect = Drawing::Rect(0, 0, cachedSnapshot_->cachedImage_->GetWidth(),
            cachedSnapshot_->cachedImage_->GetHeight());
    Drawing::Rect dstRect = clipIBounds;
    RS_OPTIONAL_TRACE_NAME_FMT("DrawFilterWithoutSnapshot srcRect:%s, dstRect:%s",
        srcRect.ToString().c_str(), dstRect.ToString().c_str());
    ROSEN_LOGD("DrawFilterWithoutSnapshot srcRect:%{public}s, dstRect:%{public}s",
        srcRect.ToString().c_str(), dstRect.ToString().c_str());
    bool discardCanvas = CanDiscardCanvas(canvas, dst);
    filter->DrawImageRect(canvas, cachedSnapshot_->cachedImage_, srcRect, dstRect, { discardCanvas, false });
    filter->PostProcess(canvas);
    cachedFilterHash_ = filter->Hash();
    return true;
}

bool RSFilterCacheManager::DrawFilterUsingHpae(RSPaintFilterCanvas& paintFilterCanvas,
    const std::shared_ptr<RSFilter>& filter,
    const std::shared_ptr<RSHpaeFilterCacheManager>& hpaeCacheManager, NodeId nodeId)
{
    if (!hpaeCacheManager) {
        return false;
    }
    if (nodeId != RSHpaeBaseData::GetInstance().GetBlurNodeId()) {
        return false;
    }
    hpaeCacheManager->ResetFilterCache(GetCachedSnapshot(),
        GetCachedFilteredSnapshot(), GetSnapshotRegion());
    if (0 == hpaeCacheManager->DrawFilter(paintFilterCanvas, filter, ClearCacheAfterDrawing())) {
        ResetFilterCache(hpaeCacheManager->GetCachedSnapshot(),
            hpaeCacheManager->GetCachedFilteredSnapshot(), hpaeCacheManager->GetSnapshotRegion(), true);
            RSHpaeBaseData::GetInstance().SetBlurContentChanged(hpaeCacheManager->BlurContentChanged());
            return true;
    } else {
            hpaeCacheManager->InvalidateFilterCache(FilterCacheType::BOTH);
    }

    return false;
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    NodeId nodeId, bool manuallyHandleFilterCache, bool shouldClearFilteredCache,
    const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (nodeId !=0 && DrawFilterUsingHpae(canvas,filter,hpaeCacheManager_, nodeId)) {
        return;
    }

    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.IsEmpty() || dst.IsEmpty()) {
        return;
    }

    RS_TRACE_NAME_FMT("RSFilterCacheManager::DrawFilter status: %s", GetCacheState());
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    }
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        if (manuallyHandleFilterCache ? DrawFilterWithoutSnapshot(canvas, filter, src, dst, shouldClearFilteredCache)
            :DrawFilterWithoutSnapshot(canvas, filter, src, dst, renderClearFilteredCacheAfterDrawing_)) {
            return;
        } else {
            GenerateFilteredSnapshot(canvas, filter, dst);
        }
    }
    DrawCachedFilteredSnapshot(canvas, dst, filter);
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.IsEmpty() || dst.IsEmpty()) {
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::GeneratedCachedEffectData status: %s", GetCacheState());
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    } else if (snapshotNeedUpdate_) {
        if (canvas.GetSurface()) {
            RS_TRACE_NAME_FMT("ForceTakeSnapshot: %s", src.ToString().c_str());
            auto snapshot = canvas.GetSurface()->GetImageSnapshot(src, false);
            filter->PreProcess(snapshot);
            cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), src);
            cachedFilteredSnapshot_.reset();
        }
    }
    snapshotNeedUpdate_ = RSHpaeBaseData::GetInstance().GetBlurContentChanged() &&
        (!forceUseCache_)&&(filterType_ != RSFilter::AIBAR);

    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        GenerateFilteredSnapshot(canvas, filter, dst);
    }
    // Keep a reference to the cached image, since CompactCache may invalidate it.
    auto cachedFilteredSnapshot = cachedFilteredSnapshot_;
    return cachedFilteredSnapshot;
}

void RSFilterCacheManager::TakeSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& srcRect)
{
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    Drawing::RectI snapshotIBounds = srcRect;

    auto magnifierShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::MAGNIFIER);
    if (magnifierShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSMagnifierShaderFilter>(magnifierShaderFilter);
        snapshotIBounds.Offset(tmpFilter->GetMagnifierOffsetX(), tmpFilter->GetMagnifierOffsetY());
    }
    std::shared_ptr<Drawing::Image> snapshot;
    auto aibarShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::AIBAR);
    if ((aibarShaderFilter != nullptr) && (HveFilter::GetHveFilter().GetSurfaceNodeSize() > 0)) {
        snapshot = HveFilter::GetHveFilter().SampleLayer(canvas, srcRect);
    } else {
        // Take a screenshot
        snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    }
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
    cachedFilterHash_ = 0;
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
    ROSEN_LOGD("RSFilterCacheManager::GenerateFilteredSnapshot");
    // Create an offscreen canvas with the same size as the filter region.
    auto offscreenRect = dstRect;
    auto offscreenSurface = surface->MakeSurface(offscreenRect.GetWidth(), offscreenRect.GetHeight());
    if (offscreenSurface == nullptr) {
        RS_LOGD("RSFilterCacheManager::GenerateFilteredSnapshot offscreenSurface is nullptr");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Src rect and dst rect, with origin at (0, 0).
    auto src = Drawing::Rect(0, 0, cachedSnapshot_->cachedRect_.GetWidth(), cachedSnapshot_->cachedRect_.GetHeight());
    auto dst = Drawing::Rect(0, 0, offscreenRect.GetWidth(), offscreenRect.GetHeight());

    // Draw the cached snapshot on the offscreen canvas, apply the filter, and post-process.
    filter->DrawImageRect(offscreenCanvas, cachedSnapshot_->cachedImage_, src, dst, { false, true });
    filter->PostProcess(offscreenCanvas);

    // Update the cache state with the filtered snapshot.
    auto filteredSnapshot = offscreenSurface->GetImageSnapshot();
    if (filteredSnapshot == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::GenerateFilteredSnapshot failed to get filteredSnapshot.");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(filteredSnapshot->GetWidth(),
        filteredSnapshot->GetHeight())) {
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
    isHpaeCachedFilteredSnapshot_ = false;
}

void RSFilterCacheManager::DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect,
    const std::shared_ptr<RSDrawingFilter>& filter) const
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
    Drawing::Rect dst = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    Drawing::Rect src = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    if (isHpaeCachedFilteredSnapshot_) {
        src = cachedFilteredSnapshot_->cachedRect_;
    }
    src.Offset(-cachedFilteredSnapshot_->cachedRect_.GetLeft(), -cachedFilteredSnapshot_->cachedRect_.GetTop());
    RS_OPTIONAL_TRACE_NAME_FMT("DrawCachedFilteredSnapshot cachedRect_:%s, src:%s, dst:%s",
        cachedFilteredSnapshot_->cachedRect_.ToString().c_str(), src.ToString().c_str(), dst.ToString().c_str());
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    if (CanDiscardCanvas(canvas, dstRect) && brush.GetColor().GetAlphaF() == 1.0 && filter &&
        filter->GetFilterType() == RSFilter::MATERIAL) {
        canvas.Discard();
    }
    canvas.DrawImageRect(*cachedFilteredSnapshot_->cachedImage_, src, dst, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

void RSFilterCacheManager::InvalidateFilterCache(FilterCacheType clearType)
{
    if (clearType == FilterCacheType::BOTH) {
        cachedSnapshot_.reset();
        cachedFilteredSnapshot_.reset();
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::InvalidateFilterCache BOTH");
        return;
    }
    if (clearType == FilterCacheType::SNAPSHOT) {
        cachedSnapshot_.reset();
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::InvalidateFilterCache SNAPSHOT");
        return;
    }
    if (clearType == FilterCacheType::FILTERED_SNAPSHOT) {
        cachedFilteredSnapshot_.reset();
    }
}

bool RSFilterCacheManager::GetFilterInvalid()
{
    return filterInvalid_;
}

void RSFilterCacheManager::SetFilterInvalid(bool invalidFilter)
{
    filterInvalid_ = invalidFilter;
}

bool RSFilterCacheManager::IsForceUseFilterCache() const
{
    return stagingForceUseCache_;
}

void RSFilterCacheManager::MarkFilterForceUseCache(bool forceUseCache)
{
    stagingForceUseCache_ = forceUseCache;
}

bool RSFilterCacheManager::IsForceClearFilterCache() const
{
    return stagingForceClearCache_;
}

void RSFilterCacheManager::MarkFilterForceClearCache()
{
    stagingForceClearCache_ = true;
}

void RSFilterCacheManager::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
{
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    if (filter == nullptr) {
        return;
    }
    stagingFilterHashChanged_ = stagingCachedFilterHash_ != filter->Hash();
    if (stagingFilterHashChanged_) {
        stagingCachedFilterHash_ = filter->Hash();
    }
    filterType_ = filter->GetFilterType();
    canSkipFrame_ = filter->CanSkipFrame();
}

void RSFilterCacheManager::MarkFilterRegionChanged()
{
    stagingFilterRegionChanged_ = true;
}

void RSFilterCacheManager::MarkFilterRegionInteractWithDirty()
{
    stagingFilterInteractWithDirty_ = true;
}

void RSFilterCacheManager::MarkForceClearCacheWithLastFrame()
{
    stagingForceClearCacheForLastFrame_ = true;
}

void RSFilterCacheManager::MarkFilterRegionIsLargeArea()
{
    stagingIsLargeArea_ = true;
}

void RSFilterCacheManager::MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
    stagingInForegroundFilter_ = offscreenCanvasNodeId;
    if (stagingInForegroundFilter_ != lastInForegroundFilter_ && lastCacheType_ != FilterCacheType::NONE) {
        MarkFilterForceClearCache();
    }
}

void RSFilterCacheManager::UpdateFlags(FilterCacheType type, bool cacheValid)
{
    stagingClearType_ = type;
    isFilterCacheValid_ = cacheValid;
    if (!cacheValid) {
        cacheUpdateInterval_ = stagingRotationChanged_ ? ROTATION_CACHE_UPDATE_INTERVAL :
            (filterType_ == RSFilter::AIBAR ? AIBAR_CACHE_UPDATE_INTERVAL :
            (stagingIsLargeArea_ && canSkipFrame_ ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0));
        pendingPurge_ = false;
        return;
    }
    if (stagingIsAIBarInteractWithHWC_) {
        if (cacheUpdateInterval_ > 0) {
            cacheUpdateInterval_--;
            pendingPurge_ = true;
        }
    } else {
        if ((stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ > 0) {
            cacheUpdateInterval_--;
            pendingPurge_ = true;
        }
    }
    stagingIsAIBarInteractWithHWC_ = false;
}

bool RSFilterCacheManager::IsAIBarCacheValid()
{
    if (filterType_ != RSFilter::AIBAR) {
        return false;
    }
    stagingIsAIBarInteractWithHWC_ = true;
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::IsAIBarCacheValid \
        cacheUpdateInterval_:%d forceClearCacheForLastFrame_:%d",
        cacheUpdateInterval_, stagingForceClearCacheForLastFrame_);
    if (cacheUpdateInterval_ == 0 || stagingForceClearCacheForLastFrame_) {
        return false;
    } else {
        MarkFilterForceUseCache(true);
        return true;
    }
}

void RSFilterCacheManager::MarkEffectNode()
{
    stagingIsEffectNode_  = true;
}

void RSFilterCacheManager::SwapDataAndInitStagingFlags(std::unique_ptr<RSFilterCacheManager>& cacheManager)
{
    if (cacheManager == nullptr) {
        return;
    }
    // stagingParams to renderParams
    cacheManager->renderFilterHashChanged_ = stagingFilterHashChanged_;
    cacheManager->renderForceClearCacheForLastFrame_ = stagingForceClearCacheForLastFrame_;
    cacheManager->renderIsEffectNode_ = stagingIsEffectNode_;
    cacheManager->renderIsSkipFrame_ = stagingIsSkipFrame_;
    cacheManager->renderClearType_ = stagingClearType_;

    cacheManager->stagingFilterRegionChanged_ = stagingFilterRegionChanged_;
    cacheManager->filterType_ = filterType_;
    cacheManager->stagingIsOccluded_ = stagingIsOccluded_;
    cacheManager->ClearFilterCache();
    cacheManager->forceUseCache_ = stagingForceUseCache_;
    cacheManager->belowDirty_ = stagingFilterInteractWithDirty_;

    // renderParams to stagingParams
    lastCacheType_ = cacheManager->lastCacheType_;

    // save staging param value
    lastInForegroundFilter_ = stagingInForegroundFilter_;

    // stagingParams init
    stagingFilterHashChanged_ = false;
    stagingForceClearCacheForLastFrame_ = false;
    stagingIsEffectNode_ = false;
    stagingIsSkipFrame_ = false;
    stagingClearType_ = FilterCacheType::BOTH;

    stagingFilterRegionChanged_ = false;
    stagingFilterInteractWithDirty_ = false;
    stagingRotationChanged_ = false;
    stagingForceClearCache_ = false;
    stagingForceUseCache_ = false;
    stagingIsOccluded_ = false;

    stagingIsLargeArea_ = false;
    isFilterCacheValid_ = false;

    stagingInForegroundFilter_ = false;
}

void RSFilterCacheManager::MarkNeedClearFilterCache(NodeId nodeId)
{
    RS_TRACE_NAME_FMT("RSFilterCacheManager::MarkNeedClearFilterCache nodeId[%llu] forceUseCache_:%d,"
        "forceClearCache_:%d, hashChanged:%d, regionChanged_:%d, belowDirty_:%d,"
        "lastCacheType:%d, cacheUpdateInterval_:%d, canSkip:%d, isLargeArea:%d, filterType_:%d, pendingPurge_:%d,"
        "forceClearCacheWithLastFrame:%d, rotationChanged:%d, offscreenCanvasNodeId:%llu", nodeId,
        stagingForceUseCache_, stagingForceClearCache_, stagingFilterHashChanged_,
        stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_,
        stagingInForegroundFilter_);

    ROSEN_LOGD("RSFilterDrawable::MarkNeedClearFilterCache, forceUseCache_:%{public}d,"
        "forceClearCache_:%{public}d, hashChanged:%{public}d, regionChanged_:%{public}d, belowDirty_:%{public}d,"
        "lastCacheType:%{public}hhu, cacheUpdateInterval_:%{public}d, canSkip:%{public}d, isLargeArea:%{public}d,"
        "filterType_:%{public}d, pendingPurge_:%{public}d,"
        "forceClearCacheWithLastFrame:%{public}d, rotationChanged:%{public}d,"
        "offscreenCanvasNodeId:%{public}" PRIu64,
        stagingForceUseCache_, stagingForceClearCache_,
        stagingFilterHashChanged_, stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_,
        stagingInForegroundFilter_);

    // if do not request NextVsync, close skip
    if (stagingForceClearCacheForLastFrame_) {
        cacheUpdateInterval_ = 0;
    }

    stagingIsSkipFrame_ = stagingIsLargeArea_ && canSkipFrame_ && !stagingFilterRegionChanged_;

    // no valid cache
    if (lastCacheType_ == FilterCacheType::NONE) {
        UpdateFlags(FilterCacheType::BOTH, false);
        return;
    }
    // No need to invalidate cache if background image is not null or freezed
    if (stagingForceUseCache_) {
        UpdateFlags(FilterCacheType::NONE, true);
        return;
    }

    // clear both two type cache: 1. force clear 2. filter region changed 3.skip-frame finished
    // 4. background changed and effectNode rotated will enable skip-frame, the last frame need to update.
    if (stagingForceClearCache_ || (stagingFilterRegionChanged_ && !stagingRotationChanged_) || NeedPendingPurge() ||
        ((stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ <= 0)) {
        UpdateFlags(FilterCacheType::BOTH, false);
        return;
    }

    // clear snapshot cache last frame and clear filtered cache current frame
    if (lastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && stagingFilterHashChanged_) {
        UpdateFlags(FilterCacheType::FILTERED_SNAPSHOT, false);
        return;
    }

    // when blur filter changes, we need to clear filtered cache if it valid.
    UpdateFlags(stagingFilterHashChanged_ ?
        FilterCacheType::FILTERED_SNAPSHOT : FilterCacheType::NONE, true);
}

bool RSFilterCacheManager::NeedPendingPurge() const
{
    return !stagingFilterInteractWithDirty_ && pendingPurge_;
}

void RSFilterCacheManager::ClearFilterCache()
{
    // 1. clear memory when region changed and is not the first time occured.
    bool needClearMemoryForGpu = stagingFilterRegionChanged_ && GetCachedType() != FilterCacheType::NONE;
    if (filterType_ == RSFilter::AIBAR && stagingIsOccluded_) {
        InvalidateFilterCache(FilterCacheType::BOTH);
    } else {
        InvalidateFilterCache(renderClearType_);
    }
    // 2. clear memory when region changed without skip frame.
    needClearMemoryForGpu = needClearMemoryForGpu && GetCachedType() == FilterCacheType::NONE;
    if (needClearMemoryForGpu) {
        SetFilterInvalid(true);
    }

    // whether to clear blur images. true: clear blur image, false: clear snapshot
    bool isSaveSnapshot = renderFilterHashChanged_ || GetCachedType() == FilterCacheType::NONE;
    bool isAIbarWithLastFrame = filterType_ == RSFilter::AIBAR && renderForceClearCacheForLastFrame_; // last vsync

    if ((filterType_ != RSFilter::AIBAR || isAIbarWithLastFrame) && isSaveSnapshot) {
        renderClearFilteredCacheAfterDrawing_ = true;      // hold snapshot
    } else {
        renderClearFilteredCacheAfterDrawing_ = false;     // hold blur image
    }
    if (renderIsEffectNode_ || renderIsSkipFrame_) { renderClearFilteredCacheAfterDrawing_ = renderFilterHashChanged_; }
    lastCacheType_ = stagingIsOccluded_ ? GetCachedType() : (renderClearFilteredCacheAfterDrawing_ ?
        FilterCacheType::SNAPSHOT : FilterCacheType::FILTERED_SNAPSHOT);
    RS_TRACE_NAME_FMT("RSFilterCacheManager::ClearFilterCache, clearType:%d,"
        " isOccluded_:%d, lastCacheType:%d needClearMemoryForGpu:%d ClearFilteredCacheAfterDrawing:%d",
        renderClearType_, stagingIsOccluded_, lastCacheType_, needClearMemoryForGpu,
        renderClearFilteredCacheAfterDrawing_);
    ROSEN_LOGD("RSFilterCacheManager::ClearFilterCache, clearType:%{public}d,"
        " isOccluded_:%{public}d, lastCacheType:%{public}d needClearMemoryForGpu:%{public}d"
        " ClearFilteredCacheAfterDrawing:%{public}d",
        static_cast<int>(renderClearType_), stagingIsOccluded_, static_cast<int>(lastCacheType_),
        needClearMemoryForGpu, renderClearFilteredCacheAfterDrawing_);
}

bool RSFilterCacheManager::IsSkippingFrame() const
{
    return (stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ > 0;
}

void RSFilterCacheManager::MarkRotationChanged()
{
    stagingRotationChanged_ = true;
}

// called after OnSync()
bool RSFilterCacheManager::IsFilterCacheValidForOcclusion()
{
    auto cacheType = GetCachedType();
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::IsFilterCacheValidForOcclusion cacheType:%d renderClearType_:%d",
        cacheType, renderClearType_);
    ROSEN_LOGD("RSFilterCacheManager::IsFilterCacheValidForOcclusion cacheType:%{public}d renderClearType_:%{public}d",
        static_cast<int>(cacheType), static_cast<int>(renderClearType_));

    return cacheType != FilterCacheType::NONE;
}

void RSFilterCacheManager::MarkNodeIsOccluded(bool isOccluded)
{
    stagingIsOccluded_ = isOccluded;
}

bool RSFilterCacheManager::IsFilterCacheValid() const
{
    return isFilterCacheValid_;
}

bool RSFilterCacheManager::WouldDrawLargeAreaBlur()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlur stagingIsLargeArea:%d canSkipFrame:%d"
        " stagingUpdateInterval:%d stagingFilterInteractWithDirty:%d",
        stagingIsLargeArea_, canSkipFrame_, cacheUpdateInterval_, stagingFilterInteractWithDirty_);
    ROSEN_LOGD("wouldDrawLargeAreaBlur stagingIsLargeArea:%{public}d canSkipFrame:%{public}d"
        " stagingUpdateInterval:%{public}d stagingFilterInteractWithDirty:%{public}d",
        stagingIsLargeArea_, canSkipFrame_, cacheUpdateInterval_, stagingFilterInteractWithDirty_);
    if (stagingIsLargeArea_) {
        if (!canSkipFrame_) {
            return true;
        }
        return cacheUpdateInterval_ == 1 && stagingFilterInteractWithDirty_;
    }
    return false;
}

bool RSFilterCacheManager::WouldDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlurPrecisely stagingIsLargeArea:%d stagingForceClearCache:%d"
        " canSkipFrame:%d stagingFilterHashChanged:%d stagingFilterInteractWithDirty:%d stagingFilterRegionChanged:%d"
        " stagingUpdateInterval:%d stagingLastCacheType:%d", stagingIsLargeArea_,
        stagingForceClearCache_, canSkipFrame_, stagingFilterHashChanged_, stagingFilterInteractWithDirty_,
        stagingFilterRegionChanged_, cacheUpdateInterval_, lastCacheType_);
    ROSEN_LOGD("wouldDrawLargeAreaBlurPrecisely stagingIsLargeArea:%{public}d stagingForceClearCache:%{public}d"
        " canSkipFrame:%{public}d stagingFilterHashChanged:%{public}d stagingFilterInteractWithDirty:%{public}d"
        " stagingFilterRegionChanged:%{public}d stagingUpdateInterval:%{public}d stagingLastCacheType:%{public}d",
        stagingIsLargeArea_, stagingForceClearCache_, canSkipFrame_, stagingFilterHashChanged_,
        stagingFilterInteractWithDirty_, stagingFilterRegionChanged_, cacheUpdateInterval_,
        static_cast<int>(lastCacheType_));
    if (!stagingIsLargeArea_) {
        return false;
    }
    if (stagingForceClearCache_) {
        return true;
    }
    if (!canSkipFrame_ && !stagingFilterHashChanged_) {
        return true;
    }
    if (!stagingFilterInteractWithDirty_ && !stagingFilterHashChanged_ && !stagingFilterRegionChanged_) {
        return false;
    }
    if (cacheUpdateInterval_ == 0) {
        return true;
    }
    if (lastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && stagingFilterHashChanged_) {
        return true;
    }
    return false;
}

void RSFilterCacheManager::ReleaseCacheOffTree()
{
    RS_OPTIONAL_TRACE_FUNC();
    cachedSnapshot_.reset();
    cachedFilteredSnapshot_.reset();
}

FilterCacheType RSFilterCacheManager::GetCachedType() const
{
    if (cachedSnapshot_ == nullptr && cachedFilteredSnapshot_ == nullptr) {
        return FilterCacheType::NONE;
    }
    if (cachedSnapshot_ != nullptr) {
        return FilterCacheType::SNAPSHOT;
    }

    if (cachedFilteredSnapshot_ != nullptr) {
        return FilterCacheType::FILTERED_SNAPSHOT;
    }
    return FilterCacheType::BOTH;
}

inline void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleRectF = canvas.GetVisibleRect();
    visibleRectF.Round();
    Drawing::RectI visibleIRect = {(int)visibleRectF.GetLeft(), (int)visibleRectF.GetTop(),
                                   (int)visibleRectF.GetRight(), (int)visibleRectF.GetBottom()};
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

std::tuple<Drawing::RectI, Drawing::RectI> RSFilterCacheManager::ValidateParams(
    RSPaintFilterCanvas& canvas, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    Drawing::RectI src;
    Drawing::RectI dst;
    auto deviceRect = Drawing::RectI(0, 0, canvas.GetImageInfo().GetWidth(), canvas.GetImageInfo().GetHeight());
    if (!srcRect.has_value()) {
        src = canvas.GetRoundInDeviceClipBounds();
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::ValidateParams src:%s, dst:%s",
        src.ToString().c_str(), dst.ToString().c_str());
    ROSEN_LOGD("RSFilterCacheManager::ValidateParams src:%{public}s, dst:%{public}s",
        src.ToString().c_str(), dst.ToString().c_str());
    return { src, dst };
}

void RSFilterCacheManager::ResetFilterCache(std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedSnapshot,
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> cachedFilteredSnapshot, RectI snapshotRegion,
    bool isHpaeCachedFilteredSnapshot) {
        RS_TRACE_NAME_FMT("ResetFilterCache:[%p,%p]", cachedSnapshot.get(), cachedFilteredSnapshot.get());
        if (cachedSnapshot && cachedSnapshot->cachedImage_) {
            cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(cachedSnapshot->cachedImage_,
                cachedSnapshot->cachedRect_);
        } else {
            cachedSnapshot_.reset();
        }
        if (cachedFilteredSnapshot && cachedFilteredSnapshot->cachedImage_) {
            cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(
                cachedFilteredSnapshot->cachedImage_, cachedFilteredSnapshot->cachedRect_);
                isHpaeCachedFilteredSnapshot_ = isHpaeCachedFilteredSnapshot;
        } else {
            cachedFilteredSnapshot.reset();
            isHpaeCachedFilteredSnapshot_ =false;
        }
        snapshotRegion_ = snapshotRegion;
}
bool RSFilterCacheManager::ForceUpadateCacheByHpae()
{
    if (belowDirty_) {
        return false;
    }
    if (forceUseCache_ || filterType_ == RSFilter::AIBAR){
        return false;
    }
    return RSHpaeBaseData::GetInstance().GetBlurContentChanged();
}

void RSFilterCacheManager::CompactFilterCache()
{
    if (ForceUpadateCacheByHpae()) {
        RS_TRACE_NAME("blur content changed");
        renderClearFilteredCacheAfterDrawing_ = true;
    }
    InvalidateFilterCache(renderClearFilteredCacheAfterDrawing_ ?
        FilterCacheType::FILTERED_SNAPSHOT : FilterCacheType::SNAPSHOT);
}
} // namespace Rosen
} // namespace OHOS
#endif