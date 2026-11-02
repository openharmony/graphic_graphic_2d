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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_FILTER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_FILTER_DRAWABLE_H

#include <memory>
#include <optional>
#include <string>

#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "recording/draw_cmd_list.h"

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "drawable/rs_drawable.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "property/rs_properties_def.h"
#include "render/rs_filter.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSFilter;
namespace DrawableV2 {
class RSFilterDrawable : public RSDrawable {
public:
    RSFilterDrawable();
    ~RSFilterDrawable() override = default;

    virtual bool IsForeground() const
    {
        return false;
    }
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

    // -------------------- special adapter -------------------- //
    // all of them are called from the main thread.
    void SetBlurIntersectWithDRM(bool IsIntersectWithDRM, bool isDark);
    bool GetEnableEDR() const override
    {
        return enableEDREffect_;
    }

    // -------------------- render rect -------------------- //
    void UpdateFilterRectInfo(const RectF& bound, const std::shared_ptr<RSFilter>& filter);
    RectF GetStagingRelativeRect(EffectRectType type, const RectF& defaultValue) const; // called from the main thread.
    RectF GetRenderRelativeRect(EffectRectType type, const RectF& defaultValue) const; // called from the render thread.

    // -------------------- dirty rect -------------------- //
    virtual void CalVisibleRect(
        const Drawing::Matrix& absMatrix, const std::optional<RectI>& clipRect, const RectF& defaultRelativeRect);
    RectI GetVisibleTotalRegion(const RectI& defaultValue) const;
    RectI GetVisibleSnapshotRegion(const RectI& defaultValue) const;
    RectI GetVisibleDrawRegion(const RectI& defaultValue) const;
    RectI GetLastVisibleSnapshotRegion(const RectI& defaultValue) const;
    RectI GetLastVisibleDrawRegion(const RectI& defaultValue) const;

    // -------------------- filter cache  -------------------- //
    // all of them are called from the main thread.
    void MarkFilterRegionChanged();
    void MarkFilterRegionInteractWithDirty();
    void MarkFilterRegionIsLargeArea();
    void MarkFilterForceUseCache(bool forceUseCache = true);
    void MarkFilterForceClearCache();
    void MarkEffectNode();
    void MarkRotationChanged();
    void MarkNodeIsOccluded(bool isOccluded);
    void MarkForceClearCacheWithLastFrame();
    void MarkNeedClearFilterCache();
    void MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId);
    void MarkDebugEnabled();
    bool MarkAIBarCacheStatus(bool intersectHwcDamage);
    // Return true if the cache interval of aibar has been successfully reduced; otherwise, return false.
    bool MarkForceReduceAIBarCacheInterval(bool intersectHwcDamage);
    bool IsFilterCacheValid() const;
    bool IsForceClearFilterCache() const;
    bool IsForceUseFilterCache() const;
    bool IsLastHpaeClearCache() const;
    bool IsPendingPurge() const;
    bool IsSkippingFrame() const;
    bool IsAIBarFilter() const;
    bool IsDrawLargeAreaBlur();
    bool IsDrawLargeAreaBlurPrecisely();

    // all of them are called from the render thread.
    bool IsFilterCacheValidForOcclusion();
    bool IsFilterCacheValidForPartialRender() const;
    const RectI GetFilterCachedRegion() const;
protected:
    virtual std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const = 0;
    // Called on the main thread from OnUpdate; lets each derived class apply its own updates to
    // the filter (e.g. the SDF shape and the EDR check) before the filter is staged.
    virtual void ApplyFilterOnUpdate(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter) {}
    // Called on the render thread from OnSync after the filter commits and the render state is
    // published; lets each derived class apply its own render-side updates to the filter
    // (e.g. the EDR display headroom).
    virtual void ApplyFilterOnSync() {}
    // Called on the render thread from OnDraw; applies the adaptive frosted glass params and the
    // geometry to the filter, resolves the effect rects and draws the filter.
    virtual void DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const;
    // DRM bypass on the render thread from OnDraw: draws the flat mask color, or the nearly
    // opaque SDF shape color if the filter carries the frosted glass effect.
    virtual void DrawFilterWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const;
    // Shared tail for drawing filters: checks the filter chain and stages enableEDREffect_.
    void CheckAndEnableEDR(const std::shared_ptr<RSFilter>& filter);
    void ReportBlur(const Drawing::Rect* rect, int64_t startBlurTime) const;
    bool needSync_ = false;

    NodeId stagingNodeId_ = INVALID_NODEID;
    std::string stagingNodeName_ = "invalid0";
    NodeId stagingScreenNodeId_ = INVALID_NODEID;
    std::shared_ptr<RSFilter> stagingFilter_;

    NodeId renderNodeId_ = INVALID_NODEID;
    std::string renderNodeName_ = "invalid0";
    NodeId renderScreenNodeId_ = INVALID_NODEID;
    std::shared_ptr<RSFilter> renderFilter_;

    // -------------------- special adapter -------------------- //
    bool stagingIntersectWithDRM_ = false;
    bool stagingIsDarkColorMode_ = false;
    bool renderIntersectWithDRM_ = false;
    bool renderIsDarkColorMode_ = false;

    bool enableEDREffect_ = false;

    // -------------------- render rect -------------------- //
    struct FilterRectInfo {
        RectF snapshotRect_;
        RectF drawRect_;
    };
    static RectF GetRelativeRect(
        const std::unique_ptr<FilterRectInfo>& rectInfo, EffectRectType type, const RectF& defaultValue);
    virtual Drawing::RectI GetAbsRenderEffectRect(
        const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const;
    std::unique_ptr<FilterRectInfo> stagingRelativeRectInfo_;
    std::unique_ptr<FilterRectInfo> renderRelativeRectInfo_;

    // -------------------- dirty rect -------------------- //
    struct FilterVisibleRectInfo {
        RectI snapshotRect_;
        RectI totalRect_;
        RectI drawRect_;
    };
    std::unique_ptr<FilterVisibleRectInfo> stagingVisibleRectInfo_;
    std::unique_ptr<FilterVisibleRectInfo> lastStagingVisibleRectInfo_;

    // -------------------- filter cache  -------------------- //
    std::unique_ptr<RSFilterCacheManager> stagingCacheManager_;
    std::unique_ptr<RSFilterCacheManager> cacheManager_;
};

// BACKGROUND_FILTER slot (RSEffectRenderNode uses RSBackgroundEffectDrawable instead, see
// RSBackgroundFilterDrawable::OnGenerate). Also owns the behind-window blur path: when the
// surface node has no background filter but needs behind-window blur, OnDraw dispatches to
// DrawBehindWindow instead of the common filter drawing.
class RSBackgroundFilterDrawable : public RSFilterDrawable {
public:
    RSBackgroundFilterDrawable() = default;
    ~RSBackgroundFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;
    void SetDrawBehindWindowRegion(RectI region);
    void RemovePixelStretch();
    bool FuzePixelStretch(const RSRenderNode& node);

protected:
    std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const override;
    void ApplyFilterOnUpdate(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter) override;
    void ApplyFilterOnSync() override;

private:
    static std::shared_ptr<RSFilter> GetBehindWindowFilter(const RSRenderNode& node);
    template<typename T>
    static bool GetBehindWindowFilterProperty(const RSRenderNode& node, ModifierNG::RSPropertyType type, T& property);
    // Behind-window path of OnDraw: skip window freeze capture and drawing cache, clip to rect
    // and draw the background effect clipped to the surface device bounds.
    void DrawBehindWindow(Drawing::Canvas* canvas, const Drawing::Rect* rect) const;

    bool needDrawBehindWindow_ = false;
    bool stagingNeedDrawBehindWindow_ = false;
    RectI drawBehindWindowRegion_;
    RectI stagingDrawBehindWindowRegion_;
};

// BACKGROUND_FILTER slot variant for RSEffectRenderNode (effect merging path). Keeps the
// historical behavior: no SDF shape application and no EDR check (no ApplyFilterOnUpdate override).
class RSBackgroundEffectDrawable : public RSFilterDrawable {
public:
    RSBackgroundEffectDrawable() = default;
    ~RSBackgroundEffectDrawable() override = default;

    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

protected:
    std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const override;
    void DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

private:
    Drawing::RectI GetAbsRenderEffectRect(
        const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const override;
};

// MATERIAL_FILTER slot.
class RSMaterialFilterDrawable : public RSFilterDrawable {
public:
    RSMaterialFilterDrawable() = default;
    ~RSMaterialFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

    Drawing::RectI GetAbsRenderEffectRect(
        const Drawing::Canvas& canvas, EffectRectType type, const RectF& bound) const override;

protected:
    std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const override;
    void ApplyFilterOnUpdate(const RSRenderNode& node, const std::shared_ptr<RSFilter>& filter) override;
    void ApplyFilterOnSync() override;
    void DrawFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

private:
    bool stagingEmptyShape_ = false;
    bool emptyShape_ = false;
    std::shared_ptr<Drawing::Path> stagingClipPath_;
    std::shared_ptr<Drawing::Path> clipPath_;
};

// COMPOSITING_FILTER slot.
class RSCompositingFilterDrawable : public RSFilterDrawable {
public:
    RSCompositingFilterDrawable() = default;
    ~RSCompositingFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool IsForeground() const override
    {
        return true;
    }

protected:
    std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const override;
};

} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_FILTER_DRAWABLE_H
