/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable.h"

#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_filter_cache_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"

namespace OHOS::Rosen {
constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 5;
constexpr int ROTATION_CACHE_UPDATE_INTERVAL = 1;
namespace DrawableV2 {
void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    std::swap(drawCmdList_, stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) { ptr->drawCmdList_->Playback(*canvas); };
}

// ============================================================================
// Updater
RSPropertyDrawCmdListUpdater::RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target)
    : target_(target)
{
    // PLANNING: use RSRenderNode to determine the correct recording canvas size
    recordingCanvas_ = ExtendRecordingCanvas::Obtain(10, 10, false); // width 10, height 10
}

RSPropertyDrawCmdListUpdater::~RSPropertyDrawCmdListUpdater()
{
    if (recordingCanvas_ && target_) {
        target_->stagingDrawCmdList_ = recordingCanvas_->GetDrawCmdList();
        target_->needSync_ = true;
        ExtendRecordingCanvas::Recycle(recordingCanvas_);
        recordingCanvas_.reset();
        target_ = nullptr;
    } else {
        ROSEN_LOGE("Update failed, recording canvas is null!");
    }
}

const std::unique_ptr<ExtendRecordingCanvas>& RSPropertyDrawCmdListUpdater::GetRecordingCanvas() const
{
    return recordingCanvas_;
}

// ============================================================================
RSDrawable::Ptr RSFrameOffsetDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSFrameOffsetDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSFrameOffsetDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->Translate(frameOffsetX, frameOffsetY);
    return true;
}

// ============================================================================
RSDrawable::Ptr RSClipToBoundsDrawable::OnGenerate(const RSRenderNode& node)
{
    auto ret = std::make_shared<RSClipToBoundsDrawable>();
    ret->OnUpdate(node);
    ret->OnSync();
    return std::move(ret);
};

bool RSClipToBoundsDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    auto& canvas = *updater.GetRecordingCanvas();
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, true);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, false);
    }
    return true;
}

RSDrawable::Ptr RSClipToFrameDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSClipToFrameDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.GetClipToFrame()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->ClipRect(
        RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetFrameRect()), Drawing::ClipOp::INTERSECT, false);
    return true;
}

RSFilterDrawable::RSFilterDrawable()
{
    if (RSProperties::FilterCacheEnabled) {
        cacheManager_ = std::make_unique<RSFilterCacheManager>();
    }
}

void RSFilterDrawable::OnSync()
{
    if (needSync_) {
        filter_ = std::move(stagingFilter_);
        needSync_ = false;
    }

    ClearFilterCache();

    forceUseCache_ = stagingForceUseCache_;
    clearFilteredCacheAfterDrawing_ = stagingClearFilteredCacheAfterDrawing_;

    filterHashChanged_ = false;
    filterRegionChanged_ = false;
    filterInteractWithDirty_ = false;
    rotationChanged_ = false;
    forceClearCache_ = false;
    stagingForceUseCache_ = false;
    stagingClearFilteredCacheAfterDrawing_ = false;
    isOccluded_ = false;
    forceClearCacheWithLastFrame_ = false;

    clearType_ = FilterCacheType::BOTH;
    isLargeArea_ = false;
    isFilterCacheValid_ = false;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr && ptr->filter_) {
            RS_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc node[%llu] ", ptr->nodeId_);
            if (ptr->filter_->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR && rect != nullptr) {
                auto filter = std::static_pointer_cast<RSDrawingFilter>(ptr->filter_);
                std::shared_ptr<RSShaderFilter> rsShaderFilter =
                    filter->GetShaderFilterWithType(RSShaderFilter::LINEAR_GRADIENT_BLUR);
                if (rsShaderFilter != nullptr) {
                    auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                    tmpFilter->SetGeometry(*canvas, rect->GetWidth(), rect->GetHeight());
                }
            }
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_,
                ptr->cacheManager_, ptr->IsForeground(), ptr->clearFilteredCacheAfterDrawing_);
        }
    };
}

const RectI RSFilterDrawable::GetFilterCachedRegion() const
{
    return cacheManager_ == nullptr ? RectI() : cacheManager_->GetCachedImageRegion();
}

void RSFilterDrawable::MarkFilterRegionChanged()
{
    filterRegionChanged_ = true;
}

void RSFilterDrawable::MarkFilterRegionInteractWithDirty()
{
    filterInteractWithDirty_ = true;
}

void RSFilterDrawable::MarkFilterRegionIsLargeArea()
{
    isLargeArea_ = true;
}

void RSFilterDrawable::MarkFilterForceUseCache()
{
    stagingForceUseCache_ = true;
}

void RSFilterDrawable::MarkFilterForceClearCache()
{
    forceClearCache_ = true;
}

void RSFilterDrawable::MarkRotationChanged()
{
    rotationChanged_ = true;
}

void RSFilterDrawable::MarkHasEffectChildren()
{
    stagingHasEffectChildren_ = true;
}

void RSFilterDrawable::MarkNodeIsOccluded(bool isOccluded)
{
    isOccluded_ = isOccluded;
}

void RSFilterDrawable::ForceClearCacheWithLastFrame()
{
    forceClearCacheWithLastFrame_ = true;
}

void RSFilterDrawable::CheckClearFilterCache()
{
    if (cacheManager_ == nullptr) {
        return;
    }

    stagingClearFilteredCacheAfterDrawing_ = filterType_ != RSFilter::AIBAR ? filterHashChanged_ : false;
    RS_TRACE_NAME_FMT("RSFilterDrawable::MarkNeedClearFilterCache nodeId[%llu], forceUseCache_:%d, "
        "forceClearCache_:%d, hashChanged:%d, regionChanged_:%d, belowDirty_:%d,  currentClearAfterDrawing:%d, "
        "lastCacheType:%d, cacheUpdateInterval_:%d, canSkip:%d, isLargeArea:%d, hasEffectChildren_:%d,"
        "filterType_:%d, pendingPurge_:%d", nodeId_, stagingForceUseCache_, forceClearCache_, filterHashChanged_,
        filterRegionChanged_, filterInteractWithDirty_, stagingClearFilteredCacheAfterDrawing_, lastCacheType_,
        cacheUpdateInterval_, canSkipFrame_, isLargeArea_, stagingHasEffectChildren_, filterType_, pendingPurge_);

    if (forceClearCacheWithLastFrame_) {
        cacheUpdateInterval_ = 0;
        pendingPurge_ = false;
        clearType_ = FilterCacheType::BOTH;
        isFilterCacheValid_ = false;
        return;
    }
    // no valid cache
    if (lastCacheType_ == FilterCacheType::NONE) {
        UpdateFlags(FilterCacheType::NONE, false);
        return;
    }
    // No need to invalidate cache if background image is not null or freezed
    if (stagingForceUseCache_) {
        UpdateFlags(FilterCacheType::NONE, true);
        return;
    }

    // clear both two type cache: 1. force clear 2. filter region changed 3.skip-frame finished
    // 4. background changed and effectNode rotated will enable skip-frame, the last frame need to update.
    if (forceClearCache_ || (filterRegionChanged_ && !rotationChanged_) || NeedPendingPurge() ||
        ((filterInteractWithDirty_ || rotationChanged_) && cacheUpdateInterval_ <= 0)) {
        UpdateFlags(FilterCacheType::BOTH, false);
        return;
    }

    // clear snapshot cache last frame and clear filtered cache current frame
    if (lastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && filterHashChanged_) {
        UpdateFlags(FilterCacheType::FILTERED_SNAPSHOT, false);
        return;
    }

    // when blur filter changes, we need to clear filtered cache if it valid.
    UpdateFlags(filterHashChanged_ ?
        FilterCacheType::FILTERED_SNAPSHOT : FilterCacheType::NONE, true);
}

bool RSFilterDrawable::IsFilterCacheValid() const
{
    return isFilterCacheValid_;
}

bool RSFilterDrawable::IsSkippingFrame() const
{
    return (filterInteractWithDirty_ || rotationChanged_) && cacheUpdateInterval_ > 0;
}

bool RSFilterDrawable::IsForceClearFilterCache() const
{
    return forceClearCache_;
}

bool RSFilterDrawable::IsForceUseFilterCache() const
{
    return forceUseCache_;
}

bool RSFilterDrawable::NeedPendingPurge() const
{
    return !filterInteractWithDirty_ && pendingPurge_;
}

void RSFilterDrawable::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
{
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    if (filter == nullptr) {
        return;
    }
    filterHashChanged_ = cachedFilterHash_ != filter->Hash();
    if (filterHashChanged_) {
        cachedFilterHash_ = filter->Hash();
    }
    filterType_ = filter->GetFilterType();
    canSkipFrame_ = filter->CanSkipFrame();
}

void RSFilterDrawable::ClearFilterCache()
{
    if (!RSProperties::FilterCacheEnabled || cacheManager_ == nullptr || filter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache failed or no need to clear cache, filterCacheEnabled:%{public}d,"
            "cacheManager:%{public}d, filter:%{public}d", RSProperties::FilterCacheEnabled,
            cacheManager_ != nullptr, filter_ == nullptr);
        return;
    }
    // 1. clear memory when region changed and is not the first time occured.
    bool needClearMemoryForGpu = filterRegionChanged_ && cacheManager_->GetCachedType() != FilterCacheType::NONE;
    cacheManager_->InvalidateFilterCache(clearType_);
    // 2. clear memory when region changed without skip frame.
    needClearMemoryForGpu = needClearMemoryForGpu && cacheManager_->GetCachedType() == FilterCacheType::NONE;
    if (needClearMemoryForGpu) {
        cacheManager_->SetFilterInvalid(true);
    }
    lastCacheType_ = isOccluded_ ? cacheManager_->GetCachedType() : (stagingClearFilteredCacheAfterDrawing_ ?
        FilterCacheType::SNAPSHOT : FilterCacheType::FILTERED_SNAPSHOT);
    RS_TRACE_NAME_FMT("RSFilterDrawable::ClearFilterCache "
        "nodeId[%llu], clearType:%d, isOccluded_:%d, lastCacheType:%d needClearMemoryForGpu:%d",
        nodeId_, clearType_, isOccluded_, lastCacheType_, needClearMemoryForGpu);
}

void RSFilterDrawable::UpdateFlags(FilterCacheType type, bool cacheValid)
{
    clearType_ = type;
    isFilterCacheValid_ = cacheValid;
    if (!cacheValid) {
        cacheUpdateInterval_ = rotationChanged_ ? ROTATION_CACHE_UPDATE_INTERVAL :
            (filterType_ == RSFilter::AIBAR ? AIBAR_CACHE_UPDATE_INTERVAL :
            (isLargeArea_ && canSkipFrame_ ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0));
        pendingPurge_ = false;
        return;
    }
    if ((filterInteractWithDirty_ || rotationChanged_) && cacheUpdateInterval_ > 0) {
        cacheUpdateInterval_--;
        pendingPurge_ = true;
    }
}

bool RSFilterDrawable::IsAIBarCacheValid() const
{
    return (filterType_ == RSFilter::AIBAR) && cacheUpdateInterval_ > 0;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
