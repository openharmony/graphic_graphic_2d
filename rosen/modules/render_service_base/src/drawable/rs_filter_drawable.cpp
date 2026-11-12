/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_filter_drawable.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_effect_drawable_utils.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shape_base.h"
#include "memory/rs_tag_tracker.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_effect_luminance_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
constexpr int TRACE_LEVEL_TWO = 2;

RSFilterDrawable::RSFilterDrawable()
{
    if (RSFilterCacheManager::IsFilterCacheEnabled()) {
        stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
        cacheManager_ = std::make_unique<RSFilterCacheManager>();
    }
}

bool RSFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto filter = GetFilter(node);
    if (filter == nullptr) {
        return false;
    }
    OnFilterUpdated(node, filter);
    stagingScreenNodeId_ = node.GetScreenNodeId();
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    stagingFilter_ = filter;
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->RecordFilterInfos(stagingFilter_);
    }
    needSync_ = true;
    return true;
}

void RSFilterDrawable::OnSync()
{
    renderScreenNodeId_ = stagingScreenNodeId_;
    renderNodeId_ = stagingNodeId_;
    renderNodeName_ = stagingNodeName_;

    if (needSync_) {
        renderFilter_ = std::move(stagingFilter_);
        if (renderFilter_) {
            renderFilter_->OnSync();
        }
        needSync_ = false;
    }
    if (renderFilter_ && renderFilter_->IsDrawingFilter() && enableEDREffect_) {
        float displayHeadroom = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(renderScreenNodeId_);
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        filter->SetDisplayHeadroom(displayHeadroom);
    }

    renderIntersectWithDRM_ = stagingIntersectWithDRM_;
    stagingIntersectWithDRM_ = false;
    renderIsDarkColorMode_ = stagingIsDarkColorMode_;
    stagingIsDarkColorMode_ = false;

    renderRelativeRectInfo_ =
        stagingRelativeRectInfo_ != nullptr ? std::make_unique<FilterRectInfo>(*stagingRelativeRectInfo_) : nullptr;
    lastStagingVisibleRectInfo_ = std::move(stagingVisibleRectInfo_);

    if (stagingCacheManager_ == nullptr || cacheManager_ == nullptr || renderFilter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache failed or no need to clear cache, "
                   "cacheManager:%{public}d, filter:%{public}d",
            stagingCacheManager_ != nullptr, renderFilter_ == nullptr);
        return;
    }
    stagingCacheManager_->SwapDataAndInitStagingFlags(cacheManager_);
}

void RSFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (renderIntersectWithDRM_ && RenderWithDRM(canvas, rect)) {
        return;
    }
    if (renderFilter_) {
        Drawing::RectI snapshotRect;
        Drawing::RectI drawRect;
        CalcFilterRects(canvas, rect, snapshotRect, drawRect);
        SetFilterGeometry(canvas, rect, snapshotRect, drawRect);
        RenderFilter(canvas, rect, snapshotRect, drawRect);
    }
}

void RSFilterDrawable::CalcFilterRects(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    Drawing::RectI& snapshotRect, Drawing::RectI& drawRect) const
{
    if (canvas == nullptr) {
        return;
    }
    RectF bound = rect != nullptr
                        ? RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight())
                        : RectF();
    Drawing::RectI clipBound = GetClipBound(*canvas);
    snapshotRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, GetSnapshotBound(bound), clipBound);
    drawRect = GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, GetDrawBound(bound), clipBound);
}

RectF RSFilterDrawable::GetSnapshotBound(const RectF& bound) const
{
    return bound;
}

RectF RSFilterDrawable::GetDrawBound(const RectF& bound) const
{
    return bound;
}

void RSFilterDrawable::SetFilterGeometry(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    const Drawing::RectI& snapshotRect, const Drawing::RectI& drawRect) const
{
    if (canvas == nullptr || rect == nullptr) {
        return;
    }
    RectF bound = RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight());
    RectF snapshotRelativeRect = GetRenderRelativeRect(EffectRectType::SNAPSHOT, bound);
    auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
    if (filter) {
        filter->SetGeometry(canvas->GetTotalMatrix(), Drawing::Rect(snapshotRect), Drawing::Rect(drawRect),
            snapshotRelativeRect.GetWidth(), snapshotRelativeRect.GetHeight());
    }
}

void RSFilterDrawable::RenderFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    const Drawing::RectI& snapshotRect, const Drawing::RectI& drawRect) const
{
    if (canvas == nullptr || rect == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RSEffectDrawableUtils::ApplyAdaptiveFrostedGlassParams(
        canvas, std::static_pointer_cast<RSDrawingFilter>(renderFilter_));
    RS_TRACE_NAME_FMT("RSFilterDrawable::RenderFilter node[%llu] ", renderNodeId_);
    int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
    RSEffectDrawableUtils::DrawFilter(
        canvas, renderFilter_, cacheManager_, renderNodeId_, IsForeground(), snapshotRect, drawRect);
    ReportBlur(rect, startBlurTime);
}

void RSFilterDrawable::CheckAndEnableEDR(const std::shared_ptr<RSFilter>& filter)
{
    if (filter == nullptr || !filter->IsDrawingFilter()) {
        return;
    }
    auto renderFilter = std::static_pointer_cast<RSDrawingFilter>(filter)->GetNGRenderFilter();
    enableEDREffect_ = RSNGRenderFilterHelper::CheckEnableEDR(renderFilter);
}

void RSFilterDrawable::ReportBlur(const Drawing::Rect* rect, int64_t startBlurTime) const
{
    int64_t blurDuration = Drawing::PerfmonitorReporter::GetCurrentTime() - startBlurTime;
    auto filterType = renderFilter_->GetFilterType();
    bool isBlurType = RSEffectDrawableUtils::IsBlurFilterType(filterType);
    float blurRadius = RSEffectDrawableUtils::GetBlurFilterRadius(renderFilter_);
    RSPerfMonitorReporter::GetInstance().RecordBlurNode(renderNodeName_, blurDuration, isBlurType);
    if (rect != nullptr) {
        RSPerfMonitorReporter::GetInstance().RecordBlurPerfEvent(
            renderNodeId_, renderNodeName_, static_cast<uint16_t>(filterType), blurRadius,
            rect->GetWidth(), rect->GetHeight(), blurDuration, isBlurType);
    }
}

void RSFilterDrawable::SetBlurIntersectWithDRM(bool intersectWithDRM, bool isDark)
{
    stagingIntersectWithDRM_ = intersectWithDRM;
    stagingIsDarkColorMode_ = isDark;
}

void RSFilterDrawable::UpdateFilterRectInfo(const RectF& bound, const std::shared_ptr<RSFilter>& filter)
{
    stagingRelativeRectInfo_ = nullptr;
    if (filter == nullptr) {
        return;
    }

    auto snapshotRect = filter->GetRect(bound, EffectRectType::SNAPSHOT);
    auto drawRect = filter->GetRect(bound, EffectRectType::DRAW);
    bool needGenerateRectInfo = !bound.IsEmpty() && (!snapshotRect.IsNearEqual(bound) || !drawRect.IsNearEqual(bound));
    if (!needGenerateRectInfo) {
        return;
    }

    stagingRelativeRectInfo_ = std::make_unique<FilterRectInfo>();
    stagingRelativeRectInfo_->snapshotRect_ = snapshotRect;
    stagingRelativeRectInfo_->drawRect_ = drawRect;
}

RectF RSFilterDrawable::GetStagingRelativeRect(EffectRectType type, const RectF& defaultValue) const
{
    return RSFilterDrawable::GetRelativeRect(stagingRelativeRectInfo_, type, defaultValue);
}

RectF RSFilterDrawable::GetRenderRelativeRect(EffectRectType type, const RectF& defaultValue) const
{
    return RSFilterDrawable::GetRelativeRect(renderRelativeRectInfo_, type, defaultValue);
}

void RSFilterDrawable::CalVisibleRect(
    const Drawing::Matrix& absMatrix, const std::optional<RectI>& clipRect, const RectF& defaultRelativeRect)
{
    if (stagingRelativeRectInfo_ == nullptr) {
        return;
    }

    stagingVisibleRectInfo_ = std::make_unique<FilterVisibleRectInfo>();
    stagingVisibleRectInfo_->snapshotRect_ =
        RSObjAbsGeometry::MapRect(GetStagingRelativeRect(EffectRectType::SNAPSHOT, defaultRelativeRect), absMatrix);
    stagingVisibleRectInfo_->drawRect_ =
        RSObjAbsGeometry::MapRect(GetStagingRelativeRect(EffectRectType::DRAW, defaultRelativeRect), absMatrix);
    if (clipRect.has_value()) {
        stagingVisibleRectInfo_->snapshotRect_ = stagingVisibleRectInfo_->snapshotRect_.IntersectRect(*clipRect);
        stagingVisibleRectInfo_->drawRect_ = stagingVisibleRectInfo_->drawRect_.IntersectRect(*clipRect);
    }
    stagingVisibleRectInfo_->totalRect_ =
        stagingVisibleRectInfo_->snapshotRect_.JoinRect(stagingVisibleRectInfo_->drawRect_);
}

RectI RSFilterDrawable::GetVisibleTotalRegion(const RectI& defaultValue) const
{
    return stagingVisibleRectInfo_ != nullptr ? stagingVisibleRectInfo_->totalRect_ : defaultValue;
}

RectI RSFilterDrawable::GetVisibleSnapshotRegion(const RectI& defaultValue) const
{
    return stagingVisibleRectInfo_ != nullptr ? stagingVisibleRectInfo_->snapshotRect_ : defaultValue;
}

RectI RSFilterDrawable::GetVisibleDrawRegion(const RectI& defaultValue) const
{
    return stagingVisibleRectInfo_ != nullptr ? stagingVisibleRectInfo_->drawRect_ : defaultValue;
}

RectI RSFilterDrawable::GetLastVisibleSnapshotRegion(const RectI& defaultValue) const
{
    return lastStagingVisibleRectInfo_ != nullptr ? lastStagingVisibleRectInfo_->snapshotRect_ : defaultValue;
}

RectI RSFilterDrawable::GetLastVisibleDrawRegion(const RectI& defaultValue) const
{
    return lastStagingVisibleRectInfo_ != nullptr ? lastStagingVisibleRectInfo_->drawRect_ : defaultValue;
}

void RSFilterDrawable::MarkFilterRegionChanged()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_REGION_CHANGED,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkFilterRegionChanged();
}

void RSFilterDrawable::MarkFilterRegionInteractWithDirty()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::BLUR_CONTENT_CHANGED,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkFilterRegionInteractWithDirty();
}

void RSFilterDrawable::MarkFilterRegionIsLargeArea()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::DIRTY_OVER_SIZE,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

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
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));
    stagingCacheManager_->MarkFilterForceClearCache();
}

void RSFilterDrawable::MarkEffectNode()
{
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->MarkEffectNode();
    }
}

void RSFilterDrawable::MarkRotationChanged()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::ROTATION_CHANGED,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkRotationChanged();
}

void RSFilterDrawable::MarkNodeIsOccluded(bool isOccluded)
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::NODE_IS_OCCLUDED,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkNodeIsOccluded(isOccluded);
}

void RSFilterDrawable::MarkForceClearCacheWithLastFrame()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    RSPerfMonitorReporter::GetInstance().RecordBlurCacheReason(this->renderNodeName_,
        BLUR_CLEAR_CACHE_REASON::SKIP_FRAME_NO_VSYNC,
        RSEffectDrawableUtils::IsBlurFilterType(stagingCacheManager_->GetFilterType()));

    stagingCacheManager_->MarkForceClearCacheWithLastFrame();
}

void RSFilterDrawable::MarkNeedClearFilterCache()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    stagingCacheManager_->MarkNeedClearFilterCache(stagingNodeId_);
    ROSEN_LOGD(
        "RSFilterDrawable::MarkNeedClearFilterCache nodeId[%{public}lld]", static_cast<long long>(stagingNodeId_));
}

void RSFilterDrawable::MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    stagingCacheManager_->MarkInForegroundFilterAndCheckNeedForceClearCache(offscreenCanvasNodeId);
}

void RSFilterDrawable::MarkDebugEnabled()
{
    if (stagingCacheManager_ == nullptr) {
        return;
    }
    stagingCacheManager_->MarkDebugEnabled();
}

bool RSFilterDrawable::MarkAIBarCacheStatus(bool intersectHwcDamage)
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->CheckAndUpdateAIBarCacheStatus(intersectHwcDamage);
}

bool RSFilterDrawable::MarkForceReduceAIBarCacheInterval(bool intersectHwcDamage)
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }

    if (!IsAIBarFilter()) {
        return false;
    }

    if (intersectHwcDamage) {
        return stagingCacheManager_->ReduceCacheUpdateInterval();
    }

    return stagingCacheManager_->ReduceCacheUpdateIntervalIfPendingPurge();
}

bool RSFilterDrawable::IsFilterCacheValid() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsFilterCacheValid();
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

bool RSFilterDrawable::IsLastHpaeClearCache() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->LastHpaeClearCache();
}

bool RSFilterDrawable::IsPendingPurge() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->NeedPendingPurge();
}

bool RSFilterDrawable::IsSkippingFrame() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->IsSkippingFrame();
}

bool RSFilterDrawable::IsAIBarFilter() const
{
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->GetFilterType() == RSFilter::AIBAR;
}

bool RSFilterDrawable::IsDrawLargeAreaBlur()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlur stagingNodeId[%llu]", stagingNodeId_);
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->WouldDrawLargeAreaBlur();
}

bool RSFilterDrawable::IsDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlurPrecisely stagingNodeId[%llu]", stagingNodeId_);
    if (stagingCacheManager_ == nullptr) {
        return false;
    }
    return stagingCacheManager_->WouldDrawLargeAreaBlurPrecisely();
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

bool RSFilterDrawable::IsFilterCacheValidForPartialRender() const
{
    if (cacheManager_ == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::IsFilterCacheValidForPartialRender cacheManager not available");
        return false;
    }
    return cacheManager_->IsFilterCacheValidForPartialRender();
}

const RectI RSFilterDrawable::GetFilterCachedRegion() const
{
    return cacheManager_ == nullptr ? RectI() : cacheManager_->GetCachedImageRegion();
}

RectF RSFilterDrawable::GetRelativeRect(
    const std::unique_ptr<FilterRectInfo>& rectInfo, EffectRectType type, const RectF& defaultValue)
{
    if (rectInfo == nullptr) {
        return defaultValue;
    }

    switch (type) {
        case EffectRectType::SNAPSHOT: {
            return rectInfo->snapshotRect_;
        }
        case EffectRectType::DRAW: {
            return rectInfo->drawRect_;
        }
        case EffectRectType::TOTAL: {
            return rectInfo->drawRect_.JoinRect(rectInfo->snapshotRect_);
        }
        default: {
            return defaultValue;
        }
    }
}

Drawing::RectI RSFilterDrawable::GetClipBound(const Drawing::Canvas& canvas) const
{
    return canvas.GetDeviceClipBounds();
}

Drawing::RectI RSFilterDrawable::GetAbsRenderEffectRect(const Drawing::Canvas& canvas, EffectRectType type,
    const RectF& bound, const Drawing::RectI& clipBound) const
{
    RectF rect = GetRenderRelativeRect(type, bound);
    auto relativeRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, relativeRect);
    Drawing::RectI absRectI = absRect.RoundOut();

    return absRectI.Intersect(clipBound) ? absRectI : Drawing::RectI();
}

// ============================================================================
// RSBackgroundFilterDrawable
RSDrawable::Ptr RSBackgroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    RSDrawable::Ptr filterDrawable = nullptr;
    if (node.IsInstanceOf<RSEffectRenderNode>()) {
        filterDrawable = std::make_shared<RSBackgroundEffectDrawable>();
    } else {
        filterDrawable = std::make_shared<RSBackgroundFilterDrawable>();
    }
    if (filterDrawable->OnUpdate(node)) {
        return filterDrawable;
    }
    return nullptr;
}

bool RSBackgroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNeedDrawBehindWindow_ =
        node.GetRenderProperties().GetBackgroundFilter() == nullptr && node.NeedDrawBehindWindow();
    if (stagingNeedDrawBehindWindow_) {
        stagingDrawBehindWindowRegion_ = node.GetBehindWindowRegion();
    }
    return RSFilterDrawable::OnUpdate(node);
}

void RSBackgroundFilterDrawable::OnFilterUpdated(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter)
{
    if (filter && filter->IsDrawingFilter()) {
        RSEffectDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
            std::static_pointer_cast<RSDrawingFilter>(filter), node.GetId());
    }
    CheckAndEnableEDR(filter);
}

void RSBackgroundFilterDrawable::OnSync()
{
    needDrawBehindWindow_ = stagingNeedDrawBehindWindow_;
    drawBehindWindowRegion_ = stagingDrawBehindWindowRegion_;
    RSFilterDrawable::OnSync();
}

void RSBackgroundFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (needDrawBehindWindow_) {
        DrawBehindWindow(canvas, rect);
        return;
    }
    RSFilterDrawable::OnDraw(canvas, rect);
}

std::shared_ptr<RSFilter> RSBackgroundFilterDrawable::GetFilter(const RSRenderNode& node) const
{
    const auto& filter = node.GetRenderProperties().GetBackgroundFilter();
    if (filter != nullptr) {
        return filter;
    }
    if (!node.NeedDrawBehindWindow()) {
        return nullptr;
    }
    return GetBehindWindowFilter(node);
}

bool RSBackgroundFilterDrawable::RenderWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas == nullptr) {
        return false;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RS_TRACE_NAME_FMT("RSBackgroundFilterDrawable::RenderWithDRM IntersectWithDRM isDarkColorMode[%d]",
        renderIsDarkColorMode_);
    RSEffectDrawableUtils::DrawFilterWithDRM(canvas, renderIsDarkColorMode_, rect, nullptr);
    return true;
}

std::shared_ptr<RSFilter> RSBackgroundFilterDrawable::GetBehindWindowFilter(const RSRenderNode& node)
{
    float radius = 0.f;
    float saturation = 1.f;
    float brightness = 1.f;
    RSColor maskColor = {};
    if (GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, radius) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, saturation) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, brightness) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, maskColor)) {
        return RSEffectDrawableUtils::GenerateBehindWindowFilter(radius, saturation, brightness, maskColor);
    }
    return nullptr;
}

template<typename T>
bool RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
    const RSRenderNode& node, ModifierNG::RSPropertyType type, T& property)
{
    const auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER);
    if (!modifier) {
        return false;
    }
    property = modifier->Getter<T>(type, T());
    return true;
}

template bool RSBackgroundFilterDrawable::GetBehindWindowFilterProperty<float>(
    const RSRenderNode&, ModifierNG::RSPropertyType, float&);
template bool RSBackgroundFilterDrawable::GetBehindWindowFilterProperty<RSColor>(
    const RSRenderNode&, ModifierNG::RSPropertyType, RSColor&);

void RSBackgroundFilterDrawable::DrawBehindWindow(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas == nullptr || rect == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    if (!canvas->GetSurface()) {
        RS_LOGE("RSFilterDrawable::OnDraw DrawBehindWindow surface is nullptr");
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    RS_TRACE_NAME_FMT("RSFilterDrawable::OnDraw DrawBehindWindow node[%" PRIu64 "], "
                      "windowFreezeCapture[%d], DrawingCache[%d]",
        renderNodeId_, paintFilterCanvas->GetIsWindowFreezeCapture(), paintFilterCanvas->GetIsDrawingCache());
    if (paintFilterCanvas->GetIsWindowFreezeCapture()) {
        RS_LOGD("RSFilterDrawable::OnDraw DrawBehindWindow capture freeze surface, "
                "no need to drawBehindWindow");
        return;
    }
    Drawing::AutoCanvasRestore acr(*canvas, true);
    paintFilterCanvas->ClipRect(*rect);
    Drawing::Rect absRect(0.0, 0.0, 0.0, 0.0);
    Drawing::Rect relativeBounds(drawBehindWindowRegion_.GetLeft(), drawBehindWindowRegion_.GetTop(),
        drawBehindWindowRegion_.GetRight(), drawBehindWindowRegion_.GetBottom());
    if (paintFilterCanvas->GetIsDrawingCache()) {
        RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
            "RSFilterDrawable::OnDraw DrawBehindWindow DrawingCache bounds:%s", relativeBounds.ToString().c_str());
        auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>(renderFilter_, relativeBounds);
        paintFilterCanvas->SetCacheBehindWindowData(std::move(data));
        return;
    }
    canvas->GetTotalMatrix().MapRect(absRect, relativeBounds);
    Drawing::RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()), std::ceil(absRect.GetRight()),
        std::ceil(absRect.GetBottom()));
    auto deviceRect = Drawing::RectI(0, 0, canvas->GetSurface()->Width(), canvas->GetSurface()->Height());
    bounds.Intersect(deviceRect);
    RSEffectDrawableUtils::DrawBackgroundEffect(
        paintFilterCanvas, renderFilter_, renderNodeId_, cacheManager_, bounds, true);
}

void RSBackgroundFilterDrawable::SetDrawBehindWindowRegion(RectI region)
{
    stagingDrawBehindWindowRegion_ = region;
}

void RSBackgroundFilterDrawable::RemovePixelStretch()
{
    RSEffectDrawableUtils::RSFilterRemovePixelStretch(stagingFilter_);
}

bool RSBackgroundFilterDrawable::FuzePixelStretch(const RSRenderNode& node)
{
    return RSEffectDrawableUtils::RSFilterSetPixelStretch(node.GetRenderProperties(), stagingFilter_);
}

// ============================================================================
// RSBackgroundEffectDrawable
std::shared_ptr<RSFilter> RSBackgroundEffectDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetBackgroundFilter();
}

bool RSBackgroundEffectDrawable::RenderWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    return false;
}

void RSBackgroundEffectDrawable::RenderFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    const Drawing::RectI& snapshotRect, const Drawing::RectI& drawRect) const
{
    if (canvas == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSBACKGROUNDEFFECTDRAWABLE);
#endif
    if (snapshotRect.IsEmpty()) {
        RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::RenderFilter snapshotRect is empty");
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::RenderFilter node[%" PRIu64 "]", renderNodeId_);
    RSEffectDrawableUtils::DrawBackgroundEffect(paintFilterCanvas, renderFilter_, renderNodeId_, cacheManager_,
        snapshotRect, false);
}

void RSBackgroundEffectDrawable::SetFilterGeometry(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    const Drawing::RectI& snapshotRect, const Drawing::RectI& drawRect) const
{
    if (canvas == nullptr || rect == nullptr) {
        return;
    }
    Drawing::Rect snapshotGeometry = renderRelativeRectInfo_ != nullptr ? Drawing::Rect(snapshotRect) : *rect;
    Drawing::Rect drawGeometry = renderRelativeRectInfo_ != nullptr ? Drawing::Rect(drawRect) : *rect;
    auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
    if (filter) {
        filter->SetGeometry(
            canvas->GetTotalMatrix(), snapshotGeometry, drawGeometry, rect->GetWidth(), rect->GetHeight());
    }
}

Drawing::RectI RSBackgroundEffectDrawable::GetClipBound(const Drawing::Canvas& canvas) const
{
    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGE("RSBackgroundEffectDrawable::GetClipBound surface is nullptr");
        return Drawing::RectI();
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = paintFilterCanvas->GetFilterClipBounds();
    return renderRelativeRectInfo_ == nullptr || filterClipBounds.IsEmpty()
                ? Drawing::RectI(0, 0, surface->Width(), surface->Height())
                : filterClipBounds;
}

// ============================================================================
// RSMaterialFilterDrawable
RSDrawable::Ptr RSMaterialFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaterialFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSMaterialFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto sdfShape = properties.GetSDFShape();
    stagingEmptyShape_ = sdfShape && sdfShape->GetType() == RSNGEffectType::SDF_EMPTY_SHAPE;
    auto clipBounds = properties.GetClipBounds();
    stagingClipPath_ = clipBounds ? std::make_shared<Drawing::Path>(clipBounds->GetDrawingPath()) : nullptr;
    return RSFilterDrawable::OnUpdate(node);
}

void RSMaterialFilterDrawable::OnFilterUpdated(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter)
{
    if (filter && filter->IsDrawingFilter()) {
        RSEffectDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
            std::static_pointer_cast<RSDrawingFilter>(filter), node.GetId());
    }
    CheckAndEnableEDR(filter);
}

void RSMaterialFilterDrawable::OnSync()
{
    emptyShape_ = stagingEmptyShape_;
    clipPath_ = std::move(stagingClipPath_);
    RSFilterDrawable::OnSync();
}

void RSMaterialFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (emptyShape_) {
        return;
    }

    if (canvas && clipPath_) {
        Drawing::AutoCanvasRestore acr(*canvas, true);
        canvas->ClipPath(*clipPath_, Drawing::ClipOp::INTERSECT, true);
        RSFilterDrawable::OnDraw(canvas, rect);
        return;
    }
    RSFilterDrawable::OnDraw(canvas, rect);
}

std::shared_ptr<RSFilter> RSMaterialFilterDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetMaterialFilter();
}

bool RSMaterialFilterDrawable::RenderWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas == nullptr) {
        return false;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RS_TRACE_NAME_FMT("RSMaterialFilterDrawable::RenderWithDRM IntersectWithDRM isDarkColorMode[%d]",
        renderIsDarkColorMode_);
    RSEffectDrawableUtils::DrawFilterWithDRM(canvas, renderIsDarkColorMode_, rect, renderFilter_);
    return true;
}

Drawing::RectI RSMaterialFilterDrawable::GetClipBound(const Drawing::Canvas& canvas) const
{
    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGE("RSMaterialFilterDrawable::GetClipBound surface is nullptr");
        return Drawing::RectI();
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = paintFilterCanvas->GetFilterClipBounds();
    return filterClipBounds.IsEmpty()
                ? Drawing::RectI(0, 0, surface->Width(), surface->Height())
                : filterClipBounds;
}

// ============================================================================
// RSCompositingFilterDrawable
RSDrawable::Ptr RSCompositingFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSCompositingFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

void RSCompositingFilterDrawable::OnFilterUpdated(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter)
{
    CheckAndEnableEDR(filter);
}

std::shared_ptr<RSFilter> RSCompositingFilterDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetCompositingFilter();
}

bool RSCompositingFilterDrawable::RenderWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    return false;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
