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

static RectF CalcEffectBound(const Drawing::Rect* rect)
{
    return rect != nullptr ? RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight()) : RectF();
}

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
    ApplyFilterOnUpdate(node, filter);
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    stagingScreenNodeId_ = node.GetScreenNodeId();
    stagingFilter_ = filter;
    if (stagingCacheManager_ != nullptr) {
        stagingCacheManager_->RecordFilterInfos(stagingFilter_);
    }
    needSync_ = true;
    return true;
}

void RSFilterDrawable::CheckAndEnableEDR(const std::shared_ptr<RSFilter>& filter)
{
    if (filter == nullptr || !filter->IsDrawingFilter()) {
        return;
    }
    auto renderFilter = std::static_pointer_cast<RSDrawingFilter>(filter)->GetNGRenderFilter();
    enableEDREffect_ = RSNGRenderFilterHelper::CheckEnableEDR(renderFilter);
}

void RSFilterDrawable::OnSync()
{
    if (needSync_) {
        renderFilter_ = std::move(stagingFilter_);
        if (renderFilter_) {
            renderFilter_->OnSync();
        }
        needSync_ = false;
    }

    renderNodeId_ = stagingNodeId_;
    renderNodeName_ = stagingNodeName_;
    renderScreenNodeId_ = stagingScreenNodeId_;
    renderIntersectWithDRM_ = stagingIntersectWithDRM_;
    stagingIntersectWithDRM_ = false;
    renderIsDarkColorMode_ = stagingIsDarkColorMode_;
    stagingIsDarkColorMode_ = false;
    renderRelativeRectInfo_ =
        stagingRelativeRectInfo_ != nullptr ? std::make_unique<FilterRectInfo>(*stagingRelativeRectInfo_) : nullptr;
    lastStagingVisibleRectInfo_ = std::move(stagingVisibleRectInfo_);

    ApplyFilterOnSync();

    if (!RSFilterCacheManager::IsFilterCacheEnabled() || stagingCacheManager_ == nullptr || cacheManager_ == nullptr ||
        renderFilter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache failed or no need to clear cache, filterCacheEnabled:%{public}d,"
                   "cacheManager:%{public}d, filter:%{public}d",
            RSFilterCacheManager::IsFilterCacheEnabled(), stagingCacheManager_ != nullptr, renderFilter_ == nullptr);
        return;
    }
    stagingCacheManager_->SwapDataAndInitStagingFlags(cacheManager_);
}

void RSFilterDrawable::DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(canvas,
        std::static_pointer_cast<RSDrawingFilter>(renderFilter_));
    RectF bound = CalcEffectBound(rect);
    Drawing::RectI snapshotRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound);
    Drawing::RectI drawRect = GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, bound);
    if (rect != nullptr) {
        RectF snapshotRelativeRect = GetRenderRelativeRect(EffectRectType::SNAPSHOT, bound);
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        filter->SetGeometry(canvas->GetTotalMatrix(), Drawing::Rect(snapshotRect), Drawing::Rect(drawRect),
            snapshotRelativeRect.GetWidth(), snapshotRelativeRect.GetHeight());
    }
    RS_TRACE_NAME_FMT("RSFilterDrawable::DrawFilter node[%llu] ", renderNodeId_);
    int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
    RSPropertyDrawableUtils::DrawFilter(
        canvas, renderFilter_, cacheManager_, renderNodeId_, IsForeground(), snapshotRect, drawRect);
    ReportBlur(rect, startBlurTime);
}

void RSFilterDrawable::DrawFilterWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RS_TRACE_NAME_FMT("RSFilterDrawable::DrawFilterWithDRM IntersectWithDRM isDarkColorMode[%d]",
        renderIsDarkColorMode_);
    RSEffectDrawableUtils::DrawFilterWithDRM(canvas, rect, renderIsDarkColorMode_, renderFilter_);
}

void RSFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas && renderIntersectWithDRM_) {
        DrawFilterWithDRM(canvas, rect);
        return;
    }
    if (canvas && renderFilter_) {
        DrawFilter(canvas, rect);
    }
}

void RSFilterDrawable::ReportBlur(const Drawing::Rect* rect, int64_t startBlurTime) const
{
    int64_t blurDuration = Drawing::PerfmonitorReporter::GetCurrentTime() - startBlurTime;
    auto filterType = renderFilter_->GetFilterType();
    RSPerfMonitorReporter::GetInstance().RecordBlurNode(
        renderNodeName_, blurDuration, RSPropertyDrawableUtils::IsBlurFilterType(filterType));
    if (rect != nullptr) {
        RSPerfMonitorReporter::GetInstance().RecordBlurPerfEvent(renderNodeId_, renderNodeName_,
            static_cast<uint16_t>(filterType), RSPropertyDrawableUtils::GetBlurFilterRadius(renderFilter_),
            rect->GetWidth(), rect->GetHeight(), blurDuration, RSPropertyDrawableUtils::IsBlurFilterType(filterType));
    }
}

// should be called in rs main thread
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

Drawing::RectI RSFilterDrawable::GetAbsRenderEffectRect(
    const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const
{
    RectF rect = GetRenderRelativeRect(type, bound);
    auto drawingRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, drawingRect);
    Drawing::RectI absRectI = absRect.RoundOut();
    Drawing::RectI clipBounds = canvas.GetDeviceClipBounds();
    // if absRectI.Intersect(clipBounds) is true,
    // it means that absRectI intersects with clipBounds, and absRectI has been set to their intersection.
    return absRectI.Intersect(clipBounds) ? absRectI : Drawing::RectI();
}

// ============================================================================
// RSBackgroundFilterDrawable
RSDrawable::Ptr RSBackgroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr && !node.NeedDrawBehindWindow()) {
        return nullptr;
    }

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
    // A non-null filter with an absent background filter can only come from the behind-window
    // fallback in GetFilter.
    stagingNeedDrawBehindWindow_ = node.GetRenderProperties().GetBackgroundFilter() == nullptr;
    if (stagingNeedDrawBehindWindow_) {
        stagingDrawBehindWindowRegion_ = node.GetBehindWindowRegion();
    }
    return RSFilterDrawable::OnUpdate(node);
}

void RSBackgroundFilterDrawable::OnSync()
{
    if (needSync_) {
        needDrawBehindWindow_ = stagingNeedDrawBehindWindow_;
    }
    drawBehindWindowRegion_ = stagingDrawBehindWindowRegion_;
    RSFilterDrawable::OnSync();
}

void RSBackgroundFilterDrawable::ApplyFilterOnSync()
{
    if (renderFilter_ && renderFilter_->IsDrawingFilter() && enableEDREffect_) {
        float displayHeadroom = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(renderScreenNodeId_);
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        filter->SetDisplayHeadroom(displayHeadroom);
    }
}

void RSBackgroundFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (needDrawBehindWindow_) {
        DrawBehindWindow(canvas, rect);
        return;
    }
    RSFilterDrawable::OnDraw(canvas, rect);
}

void RSBackgroundFilterDrawable::SetDrawBehindWindowRegion(RectI region)
{
    stagingDrawBehindWindowRegion_ = region;
}

void RSBackgroundFilterDrawable::RemovePixelStretch()
{
    RSPropertyDrawableUtils::RSFilterRemovePixelStretch(stagingFilter_);
}

bool RSBackgroundFilterDrawable::FuzePixelStretch(const RSRenderNode& node)
{
    return RSPropertyDrawableUtils::RSFilterSetPixelStretch(node.GetRenderProperties(), stagingFilter_);
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

void RSBackgroundFilterDrawable::ApplyFilterOnUpdate(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter)
{
    if (filter->IsDrawingFilter()) {
        RSPropertyDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
            std::static_pointer_cast<RSDrawingFilter>(filter), node.GetId());
    }
    CheckAndEnableEDR(filter);
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
        return RSPropertyDrawableUtils::GenerateBehindWindowFilter(radius, saturation, brightness, maskColor);
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

void RSBackgroundFilterDrawable::DrawBehindWindow(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
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
    RSPropertyDrawableUtils::DrawBackgroundEffect(
        paintFilterCanvas, renderFilter_, renderNodeId_, cacheManager_, bounds, true);
}

// ============================================================================
// RSBackgroundEffectDrawable
void RSBackgroundEffectDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    if (rect) {
        paintFilterCanvas->ClipRect(*rect);
    }
    DrawFilter(canvas, rect);
}

std::shared_ptr<RSFilter> RSBackgroundEffectDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetBackgroundFilter();
}

void RSBackgroundEffectDrawable::DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSBACKGROUNDEFFECTDRAWABLE);
#endif
    RectF bound = CalcEffectBound(rect);
    Drawing::RectI snapshotRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound);
    if (snapshotRect.IsEmpty()) {
        RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::DrawBackgroundEffect boundsRect is empty");
        return;
    }
    if (rect != nullptr) {
        Drawing::RectI drawRect = renderRelativeRectInfo_ != nullptr
                                       ? GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, bound)
                                       : Drawing::RectI();
        Drawing::Rect snapshotGeometry = renderRelativeRectInfo_ != nullptr ? Drawing::Rect(snapshotRect) : *rect;
        Drawing::Rect drawGeometry = renderRelativeRectInfo_ != nullptr ? Drawing::Rect(drawRect) : *rect;
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        if (filter) {
            filter->SetGeometry(canvas->GetTotalMatrix(), snapshotGeometry, drawGeometry, rect->GetWidth(),
                rect->GetHeight());
        }
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::DrawBackgroundEffect nodeId[%lld]", renderNodeId_);
    RSPropertyDrawableUtils::DrawBackgroundEffect(
        paintFilterCanvas, renderFilter_, renderNodeId_, cacheManager_, snapshotRect);
}

Drawing::RectI RSBackgroundEffectDrawable::GetAbsRenderEffectRect(
    const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const
{
    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGE("RSBackgroundEffectDrawable::GetAbsRenderEffectRect surface is nullptr.");
        return Drawing::RectI();
    }
    RectF rect = GetRenderRelativeRect(type, bound);
    auto drawingRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, drawingRect);
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = RSPropertyDrawableUtils::DrawingRectI2RectI(paintFilterCanvas->GetFilterClipBounds());
    RectI clipBound = renderRelativeRectInfo_ == nullptr || filterClipBounds.IsEmpty()
                          ? RectI(0, 0, surface->Width(), surface->Height())
                          : filterClipBounds;
    RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()), std::ceil(absRect.GetWidth()),
        std::ceil(absRect.GetHeight()));
    bounds = bounds.IntersectRect(clipBound);
    Drawing::RectI boundsRect(bounds.GetLeft(), bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    // When the drawing area of a useEffect node is empty in the current frame,
    // it won't be collected by the effect node, effect data is null(RSRenderNode::UpdateVisibleEffectChild).
    // if useEffect is within a node group, it will draw with fallback branch,
    // causing an increase in RSPropertyDrawableUtils::DrawBackgroundEffect g_blurCnt and
    // triggering frequency boosting. so add check for boundsRect: skip DrawBackgroundEffect to reduce blur count.
    return boundsRect;
}

// ============================================================================
// RSMaterialFilterDrawable
RSDrawable::Ptr RSMaterialFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetMaterialFilter();
    if (!rsFilter) {
        return nullptr;
    }

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

void RSMaterialFilterDrawable::OnSync()
{
    if (needSync_) {
        emptyShape_ = stagingEmptyShape_;
        clipPath_ = std::move(stagingClipPath_);
    }
    RSFilterDrawable::OnSync();
}

void RSMaterialFilterDrawable::ApplyFilterOnSync()
{
    if (renderFilter_ && renderFilter_->IsDrawingFilter() && enableEDREffect_) {
        float displayHeadroom = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(renderScreenNodeId_);
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        filter->SetDisplayHeadroom(displayHeadroom);
    }
}

void RSMaterialFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (emptyShape_) {
        return;
    }
    Drawing::AutoCanvasRestore acr(*canvas, clipPath_ != nullptr);
    if (clipPath_) {
        canvas->ClipPath(*clipPath_, Drawing::ClipOp::INTERSECT, true);
    }
    if (renderIntersectWithDRM_) {
        DrawFilterWithDRM(canvas, rect);
        return;
    }
    DrawFilter(canvas, rect);
}

void RSMaterialFilterDrawable::DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(), RSTagTracker::SOURCETYPE::SOURCE_RSFILTERDRAWABLE);
#endif
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(canvas,
        std::static_pointer_cast<RSDrawingFilter>(renderFilter_));
    RectF bound = CalcEffectBound(rect);
    Drawing::RectI snapshotRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound);
    Drawing::RectI drawRect = GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, bound);
    if (rect != nullptr) {
        auto filter = std::static_pointer_cast<RSDrawingFilter>(renderFilter_);
        filter->SetGeometry(canvas->GetTotalMatrix(), Drawing::Rect(snapshotRect), Drawing::Rect(drawRect),
            rect->GetWidth(), rect->GetHeight());
    }
    RS_TRACE_NAME_FMT("RSMaterialFilterDrawable::DrawFilter node[%llu] ", renderNodeId_);
    int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
    RSPropertyDrawableUtils::DrawFilter(
        canvas, renderFilter_, cacheManager_, renderNodeId_, IsForeground(), snapshotRect, drawRect);
    ReportBlur(rect, startBlurTime);
}

Drawing::RectI RSMaterialFilterDrawable::GetAbsRenderEffectRect(
    const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const
{
    RectF rect = GetRenderRelativeRect(type, bound);
    auto drawingRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, drawingRect);
    auto surface = canvas.GetSurface();
    if (!surface) {
        return Drawing::RectI();
    }

    Drawing::RectI absRectI = absRect.RoundOut();
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = paintFilterCanvas->GetFilterClipBounds();
    Drawing::RectI clipBound =
        filterClipBounds.IsEmpty() ? Drawing::RectI(0, 0, surface->Width(), surface->Height()) : filterClipBounds;
    // if absRectI.Intersect(deviceRect) is true,
    // it means that absRectI intersects with deviceRect, and absRectI has been set to their intersection.
    return absRectI.Intersect(clipBound) ? absRectI : Drawing::RectI();
}

std::shared_ptr<RSFilter> RSMaterialFilterDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetMaterialFilter();
}

void RSMaterialFilterDrawable::ApplyFilterOnUpdate(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter)
{
    if (filter->IsDrawingFilter()) {
        RSPropertyDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
            std::static_pointer_cast<RSDrawingFilter>(filter), node.GetId());
    }
    CheckAndEnableEDR(filter);
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

std::shared_ptr<RSFilter> RSCompositingFilterDrawable::GetFilter(const RSRenderNode& node) const
{
    return node.GetRenderProperties().GetCompositingFilter();
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
