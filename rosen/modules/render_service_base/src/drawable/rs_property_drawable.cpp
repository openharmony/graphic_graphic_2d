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
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_filter_cache_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"

namespace OHOS::Rosen {
constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 5;
constexpr int ROTATION_CACHE_UPDATE_INTERVAL = 1;
namespace DrawableV2 {
constexpr int TRACE_LEVEL_TWO = 2;
void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    std::swap(drawCmdList_, stagingDrawCmdList_);
    propertyDescription_ = stagingPropertyDescription_;
    stagingPropertyDescription_.clear();
    needSync_ = false;
}

void RSPropertyDrawable::OnPurge()
{
    if (drawCmdList_) {
        drawCmdList_->Purge();
    }
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        ptr->drawCmdList_->Playback(*canvas);
        if (!ptr->propertyDescription_.empty()) {
            RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "RSPropertyDrawable:: %s, bounds:%s",
                ptr->propertyDescription_.c_str(), rect->ToString().c_str());
        }
    };
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
    } else if (node.GetType() == RSRenderNodeType::SURFACE_NODE && RSSystemProperties::GetCacheEnabledForRotation() &&
        node.ReinterpretCastTo<RSSurfaceRenderNode>()->IsAppWindow()) {
        Drawing::Rect rect = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
        Drawing::RectI iRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        canvas.ClipIRect(iRect, Drawing::ClipOp::INTERSECT);
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

    renderFilterHashChanged_ = stagingFilterHashChanged_;
    renderForceClearCacheForLastFrame_ = stagingForceClearCacheForLastFrame_;
    renderIsEffectNode_ = stagingIsEffectNode_;
    renderIsSkipFrame_ = stagingIsSkipFrame_;
    renderNodeId_ = stagingNodeId_;
    renderNodeName_ = stagingNodeName_;
    renderClearType_ = stagingClearType_;
    renderIntersectWithDRM_ = stagingIntersectWithDRM_;
    renderIsDarkColorMode_ = stagingIsDarkColorMode_;

    ClearFilterCache();

    stagingFilterHashChanged_ = false;
    stagingFilterRegionChanged_ = false;
    stagingFilterInteractWithDirty_ = false;
    stagingRotationChanged_ = false;
    stagingForceClearCache_ = false;
    stagingForceUseCache_ = false;
    stagingIsOccluded_ = false;
    stagingForceClearCacheForLastFrame_ = false;
    stagingIntersectWithDRM_ = false;
    stagingIsDarkColorMode_ = false;

    stagingClearType_ = FilterCacheType::BOTH;
    stagingIsLargeArea_ = false;
    isFilterCacheValid_ = false;
    stagingIsEffectNode_ = false;
    stagingIsSkipFrame_ = false;
    needSync_ = false;
    stagingUpdateInterval_ = cacheUpdateInterval_;
    stagingLastCacheType_ = lastCacheType_;
}

bool RSFilterDrawable::WouldDrawLargeAreaBlur()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlur stagingIsLargeArea:%d canSkipFrame:%d"
        " stagingUpdateInterval:%d stagingFilterInteractWithDirty:%d stagingNodeId[%llu]",
        stagingIsLargeArea_, canSkipFrame_, stagingUpdateInterval_, stagingFilterInteractWithDirty_, stagingNodeId_);
    if (stagingIsLargeArea_) {
        if (!canSkipFrame_) {
            return true;
        }
        return stagingUpdateInterval_ == 1 && stagingFilterInteractWithDirty_;
    }
    return false;
}

bool RSFilterDrawable::WouldDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlurPrecisely stagingIsLargeArea:%d stagingForceClearCache:%d"
        " canSkipFrame:%d stagingFilterHashChanged:%d stagingFilterInteractWithDirty:%d stagingFilterRegionChanged:%d"
        " stagingUpdateInterval:%d stagingLastCacheType:%d stagingNodeId[%llu]", stagingIsLargeArea_,
        stagingForceClearCache_, canSkipFrame_, stagingFilterHashChanged_, stagingFilterInteractWithDirty_,
        stagingFilterRegionChanged_, stagingUpdateInterval_, stagingLastCacheType_, stagingNodeId_);
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
    if (stagingUpdateInterval_ == 0) {
        return true;
    }
    if (stagingLastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && stagingFilterHashChanged_) {
        return true;
    }
    return false;
}

Drawing::RecordingCanvas::DrawFunc RSFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (ptr->needDrawBehindWindow_) {
            auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
            if (!paintFilterCanvas || !canvas->GetSurface()) {
                RS_LOGE("RSFilterDrawable::CreateDrawFunc DrawBehindWindow canvas:[%{public}d], surface:[%{public}d]",
                    paintFilterCanvas != nullptr, canvas->GetSurface() != nullptr);
                return;
            }
            RS_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc DrawBehindWindow node[%llu], windowFreezeCapture[%d]",
                ptr->renderNodeId_, paintFilterCanvas->GetIsWindowFreezeCapture());
            if (paintFilterCanvas->GetIsWindowFreezeCapture()) {
                RS_LOGD("RSFilterDrawable::CreateDrawFunc DrawBehindWindow capture freeze surface, "
                    "no need to drawBehindWindow");
                return;
            }
            Drawing::AutoCanvasRestore acr(*canvas, true);
            paintFilterCanvas->ClipRect(*rect);
            Drawing::Rect absRect(0.0, 0.0, 0.0, 0.0);
            Drawing::Rect relativeBounds(ptr->drawBehindWindowRegion_.GetLeft(), ptr->drawBehindWindowRegion_.GetTop(),
                ptr->drawBehindWindowRegion_.GetRight(), ptr->drawBehindWindowRegion_.GetBottom());
            canvas->GetTotalMatrix().MapRect(absRect, relativeBounds);
            Drawing::RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()),
                std::ceil(absRect.GetRight()), std::ceil(absRect.GetBottom()));
            auto deviceRect = Drawing::RectI(0, 0, canvas->GetSurface()->Width(), canvas->GetSurface()->Height());
            bounds.Intersect(deviceRect);
            RSPropertyDrawableUtils::DrawBackgroundEffect(paintFilterCanvas, ptr->filter_, ptr->cacheManager_,
                ptr->renderClearFilteredCacheAfterDrawing_, bounds, true);
            return;
        }
        if (canvas && ptr && ptr->renderIntersectWithDRM_) {
            RS_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc IntersectWithDRM node[%lld] isDarkColorMode[%d]",
                ptr->renderNodeId_, ptr->renderIsDarkColorMode_);
            RSPropertyDrawableUtils::DrawFilterWithDRM(canvas, ptr->renderIsDarkColorMode_);
            return;
        }
        if (canvas && ptr && ptr->filter_) {
            RS_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc node[%llu] ", ptr->renderNodeId_);
            if (ptr->filter_->GetFilterType() == RSFilter::LINEAR_GRADIENT_BLUR && rect != nullptr) {
                auto filter = std::static_pointer_cast<RSDrawingFilter>(ptr->filter_);
                std::shared_ptr<RSShaderFilter> rsShaderFilter =
                    filter->GetShaderFilterWithType(RSShaderFilter::LINEAR_GRADIENT_BLUR);
                if (rsShaderFilter != nullptr) {
                    auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                    tmpFilter->SetGeometry(*canvas, rect->GetWidth(), rect->GetHeight());
                }
            }
            int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_,
                ptr->cacheManager_, ptr->IsForeground(), ptr->renderClearFilteredCacheAfterDrawing_);
            int64_t blurDuration = Drawing::PerfmonitorReporter::GetCurrentTime() - startBlurTime;
            auto filterType = ptr->filter_->GetFilterType();
            RSPerfMonitorReporter::GetInstance().RecordBlurNode(ptr->renderNodeName_, blurDuration,
                RSPropertyDrawableUtils::IsBlurFilterType(filterType));
            if (rect != nullptr) {
                RSPerfMonitorReporter::GetInstance().RecordBlurPerfEvent(ptr->renderNodeId_, ptr->renderNodeName_,
                    static_cast<uint16_t>(filterType), RSPropertyDrawableUtils::GetBlurFilterRadius(ptr->filter_),
                    rect->GetWidth(), rect->GetHeight(), blurDuration,
                    RSPropertyDrawableUtils::IsBlurFilterType(filterType));
            }
        }
    };
}

const RectI RSFilterDrawable::GetFilterCachedRegion() const
{
    return cacheManager_ == nullptr ? RectI() : cacheManager_->GetCachedImageRegion();
}

void RSFilterDrawable::MarkFilterRegionChanged()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_REGION_CHANGED, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingFilterRegionChanged_ = true;
}

void RSFilterDrawable::MarkFilterRegionInteractWithDirty()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_CONTENT_CHANGED, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingFilterInteractWithDirty_ = true;
}

void RSFilterDrawable::MarkFilterRegionIsLargeArea()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::DIRTY_OVER_SIZE, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingIsLargeArea_ = true;
}

void RSFilterDrawable::MarkFilterForceUseCache(bool forceUseCache)
{
    stagingForceUseCache_ = forceUseCache;
}

void RSFilterDrawable::MarkFilterForceClearCache()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::FORCE_CLEAR_CACHE, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingForceClearCache_ = true;
}

void RSFilterDrawable::MarkRotationChanged()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::ROTATION_CHANGED, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingRotationChanged_ = true;
}

void RSFilterDrawable::MarkNodeIsOccluded(bool isOccluded)
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::NODE_IS_OCCLUDED, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingIsOccluded_ = isOccluded;
}

void RSFilterDrawable::MarkForceClearCacheWithLastFrame()
{
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::SKIP_FRAME_NO_VSYNC, RSPropertyDrawableUtils::IsBlurFilterType(filterType_));
    stagingForceClearCacheForLastFrame_ = true;
}

void RSFilterDrawable::MarkNeedClearFilterCache()
{
    if (cacheManager_ == nullptr) {
        return;
    }

    RS_TRACE_NAME_FMT("RSFilterDrawable::MarkNeedClearFilterCache nodeId[%llu], forceUseCache_:%d,"
        "forceClearCache_:%d, hashChanged:%d, regionChanged_:%d, belowDirty_:%d,"
        "lastCacheType:%d, cacheUpdateInterval_:%d, canSkip:%d, isLargeArea:%d, filterType_:%d, pendingPurge_:%d,"
        "forceClearCacheWithLastFrame:%d, rotationChanged:%d",
        stagingNodeId_, stagingForceUseCache_, stagingForceClearCache_, stagingFilterHashChanged_,
        stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_);
    
    ROSEN_LOGD("RSFilterDrawable::MarkNeedClearFilterCache nodeId[%{public}lld], forceUseCache_:%{public}d,"
        "forceClearCache_:%{public}d, hashChanged:%{public}d, regionChanged_:%{public}d, belowDirty_:%{public}d,"
        "lastCacheType:%{public}hhu, cacheUpdateInterval_:%{public}d, canSkip:%{public}d, isLargeArea:%{public}d,"
        "filterType_:%{public}d, pendingPurge_:%{public}d,"
        "forceClearCacheWithLastFrame:%{public}d, rotationChanged:%{public}d",
        static_cast<long long>(stagingNodeId_), stagingForceUseCache_, stagingForceClearCache_,
        stagingFilterHashChanged_, stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_);

    // if do not request NextVsync, close skip
    if (stagingForceClearCacheForLastFrame_) {
        cacheUpdateInterval_ = 0;
    }

    stagingIsSkipFrame_ = stagingIsLargeArea_ && canSkipFrame_ && !stagingFilterRegionChanged_;

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

//should be called in rs main thread
void RSFilterDrawable::MarkBlurIntersectWithDRM(bool intersectWithDRM, bool isDark)
{
    stagingIntersectWithDRM_ = intersectWithDRM;
    stagingIsDarkColorMode_ = isDark;
}

bool RSFilterDrawable::IsFilterCacheValid() const
{
    return isFilterCacheValid_;
}

bool RSFilterDrawable::IsSkippingFrame() const
{
    return (stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ > 0;
}

bool RSFilterDrawable::IsForceClearFilterCache() const
{
    return stagingForceClearCache_;
}

bool RSFilterDrawable::IsForceUseFilterCache() const
{
    return stagingForceUseCache_;
}

bool RSFilterDrawable::NeedPendingPurge() const
{
    return !stagingFilterInteractWithDirty_ && pendingPurge_;
}

void RSFilterDrawable::MarkEffectNode()
{
    stagingIsEffectNode_  = true;
}

void RSFilterDrawable::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
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

void RSFilterDrawable::ClearFilterCache()
{
    if (!RSProperties::FilterCacheEnabled || cacheManager_ == nullptr || filter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache failed or no need to clear cache, filterCacheEnabled:%{public}d,"
            "cacheManager:%{public}d, filter:%{public}d", RSProperties::FilterCacheEnabled,
            cacheManager_ != nullptr, filter_ == nullptr);
        return;
    }
    // 1. clear memory when region changed and is not the first time occured.
    bool needClearMemoryForGpu = stagingFilterRegionChanged_ && cacheManager_->GetCachedType() != FilterCacheType::NONE;
    if (filterType_ == RSFilter::AIBAR && stagingIsOccluded_) {
        cacheManager_->InvalidateFilterCache(FilterCacheType::BOTH);
    } else {
        cacheManager_->InvalidateFilterCache(renderClearType_);
    }
    // 2. clear memory when region changed without skip frame.
    needClearMemoryForGpu = needClearMemoryForGpu && cacheManager_->GetCachedType() == FilterCacheType::NONE;
    if (needClearMemoryForGpu) {
        cacheManager_->SetFilterInvalid(true);
    }

    // whether to clear blur images. true: clear blur image, false: clear snapshot
    bool isSaveSnapshot = renderFilterHashChanged_ || cacheManager_->GetCachedType() == FilterCacheType::NONE;
    bool isAIbarWithLastFrame = filterType_ == RSFilter::AIBAR && renderForceClearCacheForLastFrame_; // last vsync

    if ((filterType_ != RSFilter::AIBAR || isAIbarWithLastFrame) && isSaveSnapshot) {
        renderClearFilteredCacheAfterDrawing_ = true;      // hold snapshot
    } else {
        renderClearFilteredCacheAfterDrawing_ = false;     // hold blur image
    }
    if (renderIsEffectNode_ || renderIsSkipFrame_) { renderClearFilteredCacheAfterDrawing_ = renderFilterHashChanged_; }
    lastCacheType_ = stagingIsOccluded_ ? cacheManager_->GetCachedType() : (renderClearFilteredCacheAfterDrawing_ ?
        FilterCacheType::SNAPSHOT : FilterCacheType::FILTERED_SNAPSHOT);
    RS_TRACE_NAME_FMT("RSFilterDrawable::ClearFilterCache nodeId[%llu], clearType:%d,"
        " isOccluded_:%d, lastCacheType:%d needClearMemoryForGpu:%d ClearFilteredCacheAfterDrawing:%d",
        renderNodeId_, renderClearType_, stagingIsOccluded_, lastCacheType_, needClearMemoryForGpu,
        renderClearFilteredCacheAfterDrawing_);
}

// called after OnSync()
bool RSFilterDrawable::IsFilterCacheValidForOcclusion()
{
    if (cacheManager_ == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::IsFilterCacheValidForOcclusion cacheManager not available");
        return false;
    }

    auto cacheType = cacheManager_->GetCachedType();
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterDrawable::IsFilterCacheValidForOcclusion cacheType:%d renderClearType_:%d",
        cacheType, renderClearType_);

    return cacheType != FilterCacheType::NONE;
}

void RSFilterDrawable::UpdateFlags(FilterCacheType type, bool cacheValid)
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

bool RSFilterDrawable::IsAIBarFilter() const
{
    return filterType_ == RSFilter::AIBAR;
}

bool RSFilterDrawable::IsAIBarCacheValid()
{
    if (filterType_ != RSFilter::AIBAR) {
        return false;
    }
    stagingIsAIBarInteractWithHWC_ = true;
    RS_OPTIONAL_TRACE_NAME_FMT("IsAIBarCacheValid cacheUpdateInterval_:%d forceClearCacheForLastFrame_:%d",
        cacheUpdateInterval_, stagingForceClearCacheForLastFrame_);
    if (cacheUpdateInterval_ == 0 || stagingForceClearCacheForLastFrame_) {
        return false;
    } else {
        MarkFilterForceUseCache(true);
        return true;
    }
}

void RSFilterDrawable::SetDrawBehindWindowRegion(RectI region)
{
    stagingDrawBehindWindowRegion_ = region;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
