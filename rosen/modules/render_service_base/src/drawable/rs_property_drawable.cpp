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
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_filter_cache_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"

namespace OHOS::Rosen {
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
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSPROPERTYDRAWABLE);
#endif
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
    if (RSProperties::filterCacheEnabled_) {
        stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
        cacheManager_ = std::make_unique<RSFilterCacheManager>();
    }
}

void RSFilterDrawable::OnSync()
{
    if (needSync_) {
        filter_ = std::move(stagingFilter_);
        needSync_ = false;
    }
    renderNodeId_ = stagingNodeId_;
    renderNodeName_ = stagingNodeName_;
    renderIntersectWithDRM_ = stagingIntersectWithDRM_;
    renderIsDarkColorMode_ = stagingIsDarkColorMode_;

    stagingIntersectWithDRM_ = false;
    stagingIsDarkColorMode_ = false;

    needSync_ = false;

    if (!RSProperties::filterCacheEnabled_ || stagingCacheManager_ == nullptr ||
        cacheManager_ == nullptr || filter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache failed or no need to clear cache, filterCacheEnabled:%{public}d,"
            "cacheManager:%{public}d, filter:%{public}d", RSProperties::filterCacheEnabled_,
            stagingCacheManager_ != nullptr, filter_ == nullptr);
        return;
    }
    stagingCacheManager_->SwapDataAndInitStagingFlags(cacheManager_);
}

bool RSFilterDrawable::WouldDrawLargeAreaBlur()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlur stagingNodeId[%llu]", stagingNodeId_);
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->WouldDrawLargeAreaBlur();
}

bool RSFilterDrawable::WouldDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlurPrecisely stagingNodeId[%llu]", stagingNodeId_);
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->WouldDrawLargeAreaBlurPrecisely();
}

Drawing::RecordingCanvas::DrawFunc RSFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
        if (ptr->needDrawBehindWindow_) {
            if (!canvas->GetSurface()) {
                RS_LOGE("RSFilterDrawable::CreateDrawFunc DrawBehindWindow surface is nullptr");
                return;
            }
            auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
            RS_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc DrawBehindWindow node[%" PRIu64 "], "
                "windowFreezeCapture[%d], DrawingCache[%d]", ptr->renderNodeId_,
                paintFilterCanvas->GetIsWindowFreezeCapture(), paintFilterCanvas->GetIsDrawingCache());
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
            if (paintFilterCanvas->GetIsDrawingCache()) {
                RS_OPTIONAL_TRACE_NAME_FMT("RSFilterDrawable::CreateDrawFunc DrawBehindWindow DrawingCache bounds:%s",
                    relativeBounds.ToString().c_str());
                auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>(ptr->filter_, relativeBounds);
                paintFilterCanvas->SetCacheBehindWindowData(std::move(data));
                return;
            }
            canvas->GetTotalMatrix().MapRect(absRect, relativeBounds);
            Drawing::RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()),
                std::ceil(absRect.GetRight()), std::ceil(absRect.GetBottom()));
            auto deviceRect = Drawing::RectI(0, 0, canvas->GetSurface()->Width(), canvas->GetSurface()->Height());
            bounds.Intersect(deviceRect);
            RSPropertyDrawableUtils::DrawBackgroundEffect(paintFilterCanvas, ptr->filter_, ptr->cacheManager_,
                bounds, true);
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
                std::shared_ptr<RSRenderFilterParaBase> rsShaderFilter =
                    filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
                if (rsShaderFilter != nullptr) {
                    auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                    tmpFilter->SetGeometry(*canvas, rect->GetWidth(), rect->GetHeight());
                }
            }
            int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_,
                ptr->cacheManager_, ptr->IsForeground());
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
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_REGION_CHANGED,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkFilterRegionChanged();
}

void RSFilterDrawable::MarkFilterRegionInteractWithDirty()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_CONTENT_CHANGED,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkFilterRegionInteractWithDirty();
}

void RSFilterDrawable::MarkFilterRegionIsLargeArea()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::DIRTY_OVER_SIZE,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkFilterRegionIsLargeArea();
}

void RSFilterDrawable::MarkFilterForceUseCache(bool forceUseCache)
{
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->MarkFilterForceUseCache(forceUseCache);
    }
}

void RSFilterDrawable::MarkFilterForceClearCache()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::FORCE_CLEAR_CACHE,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));
    stagingCacheManager_->MarkFilterForceClearCache();
}

void RSFilterDrawable::MarkRotationChanged()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::ROTATION_CHANGED,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkRotationChanged();
}

void RSFilterDrawable::MarkNodeIsOccluded(bool isOccluded)
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::NODE_IS_OCCLUDED,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkNodeIsOccluded(isOccluded);
}

void RSFilterDrawable::MarkForceClearCacheWithLastFrame()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::SKIP_FRAME_NO_VSYNC,
        RSPropertyDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkForceClearCacheWithLastFrame();
}

void RSFilterDrawable::MarkNeedClearFilterCache()
{
    if (stagingCacheManager_  == nullptr) {
        return;
    }
    stagingCacheManager_->MarkNeedClearFilterCache(stagingNodeId_);
    ROSEN_LOGD("RSFilterDrawable::MarkNeedClearFilterCache nodeId[%{public}lld]",
        static_cast<long long>(stagingNodeId_));
}

//should be called in rs main thread
void RSFilterDrawable::MarkBlurIntersectWithDRM(bool intersectWithDRM, bool isDark)
{
    stagingIntersectWithDRM_ = intersectWithDRM;
    stagingIsDarkColorMode_ = isDark;
}

void RSFilterDrawable::MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    stagingCacheManager_->MarkInForegroundFilterAndCheckNeedForceClearCache(offscreenCanvasNodeId);
}

bool RSFilterDrawable::IsFilterCacheValid() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsFilterCacheValid();
}

bool RSFilterDrawable::IsSkippingFrame() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsSkippingFrame();
}

bool RSFilterDrawable::IsForceClearFilterCache() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsForceClearFilterCache();
}

bool RSFilterDrawable::IsForceUseFilterCache() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsForceUseFilterCache();
}

bool RSFilterDrawable::NeedPendingPurge() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->NeedPendingPurge();
}

void RSFilterDrawable::MarkEffectNode()
{
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->MarkEffectNode();
    }
}

void RSFilterDrawable::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
{
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->RecordFilterInfos(rsFilter);
    }
}

// called after OnSync()
bool RSFilterDrawable::IsFilterCacheValidForOcclusion()
{
    if (cacheManager_ == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::IsFilterCacheValidForOcclusion cacheManager not available");
        return false;
    }
    return cacheManager_->IsFilterCacheValidForOcclusion();
}

bool RSFilterDrawable::IsAIBarFilter() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->GetFilterType() == RSFilter::AIBAR;
}

bool RSFilterDrawable::IsAIBarCacheValid()
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsAIBarCacheValid();
}

void RSFilterDrawable::SetDrawBehindWindowRegion(RectI region)
{
    stagingDrawBehindWindowRegion_ = region;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
