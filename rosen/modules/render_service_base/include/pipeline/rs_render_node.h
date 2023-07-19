/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H

#include <memory>
#include <mutex>
#include <unordered_set>
#include <stdint.h>
#include <functional>
#include "common/rs_common_def.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#else
#include "draw/surface.h"
#endif

#include "animation/rs_animation_manager.h"
#include "animation/rs_frame_rate_range.h"
#include "common/rs_macros.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"

#ifndef USE_ROSEN_DRAWING
class SkCanvas;
#endif
namespace OHOS {
namespace Rosen {
class DrawCmdList;

class RSB_EXPORT RSRenderNode : public RSBaseRenderNode {
public:
#ifndef USE_ROSEN_DRAWING
    using ClearCacheSurfaceFunc = std::function<void(sk_sp<SkSurface>, sk_sp<SkSurface>, uint32_t)>;
#else
    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>, std::shared_ptr<Drawing::Surface>, uint32_t)>;
#endif
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::RS_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    ~RSRenderNode() override;
    bool IsDirty() const override;
    bool IsContentDirty() const override;
    void AddDirtyType(RSModifierType type)
    {
        dirtyTypes_.emplace(type);
    }

    std::pair<bool, bool> Animate(int64_t timestamp) override;

    // clipRect has value in UniRender when calling PrepareCanvasRenderNode, else it is nullopt
    bool Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty,
        std::optional<RectI> clipRect = std::nullopt);
#ifndef USE_ROSEN_DRAWING
    virtual std::optional<SkRect> GetContextClipRegion() const { return std::nullopt; }
#else
    virtual std::optional<Drawing::Rect> GetContextClipRegion() const { return std::nullopt; }
#endif

    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;
    void UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled);
    inline bool IsRenderUpdateIgnored() const
    {
        return isRenderUpdateIgnored_;
    }

    // used for animation test
    RSAnimationManager& GetAnimationManager()
    {
        return animationManager_;
    }

    virtual void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);

    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}

    virtual void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void RenderTraceDebug() const;
    bool HasDisappearingTransition(bool recursive) const override
    {
        return (disappearingTransitionCount_ > 0) || RSBaseRenderNode::HasDisappearingTransition(recursive);
    }
    bool ShouldPaint() const;

    inline RectI GetOldDirty() const
    {
        return oldDirty_;
    }
    inline RectI GetOldDirtyInSurface() const
    {
        return oldDirtyInSurface_;
    }

    inline bool IsDirtyRegionUpdated() const
    {
        return isDirtyRegionUpdated_;
    }
    void AddModifier(const std::shared_ptr<RSRenderModifier> modifier);
    void RemoveModifier(const PropertyId& id);

    void ApplyModifiers();
    virtual void OnApplyModifiers() {}
    std::shared_ptr<RSRenderModifier> GetModifier(const PropertyId& id);

    bool IsShadowValidLastFrame() const
    {
        return isShadowValidLastFrame_;
    }
    void SetShadowValidLastFrame(bool isShadowValidLastFrame)
    {
        isShadowValidLastFrame_ = isShadowValidLastFrame;
    }

    // update parent's children rect including childRect and itself
    void UpdateParentChildrenRect(std::shared_ptr<RSBaseRenderNode> parentNode) const;
    void UpdateFilterCacheManagerWithCacheRegion(const std::optional<RectI>& clipRect = std::nullopt) const;

    void SetStaticCached(bool isStaticCached)
    {
        isStaticCached_ = isStaticCached;
    }

    bool IsStaticCached() const
    {
        return isStaticCached_;
    }

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    void InitCacheSurface(GrRecordingContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#else
    void InitCacheSurface(GrContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#endif
#else
    void InitCacheSurface(Drawing::GPUContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#endif

    Vector2f GetOptionalBufferSize() const;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCacheSurface() const
#else
    std::shared_ptr<Drawing::Surface> GetCacheSurface() const
#endif
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        return cacheSurface_;
    }

    void UpdateCompletedCacheSurface()
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        std::swap(cacheSurface_, cacheCompletedSurface_);
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
        std::swap(cacheBackendTexture_, cacheCompletedBackendTexture_);
        SetTextureValidFlag(true);
#endif
#endif
    }

    void SetTextureValidFlag(bool isValid)
    {
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        isTextureValid_ = isValid;
#endif
#endif
    }

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCompletedCacheSurface(uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool isUIFirst = false);
#else
    std::shared_ptr<Drawing::Surface> GetCompletedCacheSurface(uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool isUIFirst = false);
#endif

    void ClearCacheSurface()
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheSurface_ = nullptr;
        cacheCompletedSurface_ = nullptr;
    }

#ifdef RS_ENABLE_GL
    void UpdateBackendTexture();
#endif

    void DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool isUIFirst = false);

    void SetCacheType(CacheType cacheType)
    {
        cacheType_ = cacheType;
    }

    CacheType GetCacheType() const
    {
        return cacheType_;
    }

    int GetShadowRectOffsetX() const
    {
        return shadowRectOffsetX_;
    }

    int GetShadowRectOffsetY() const
    {
        return shadowRectOffsetY_;
    }

    void SetDrawingCacheType(RSDrawingCacheType cacheType)
    {
        drawingCacheType_ = cacheType;
    }

    RSDrawingCacheType GetDrawingCacheType() const
    {
        return drawingCacheType_;
    }

    void SetDrawingCacheChanged(bool cacheChanged)
    {
        isDrawingCacheChanged_ = cacheChanged;
    }

    bool GetDrawingCacheChanged() const
    {
        return isDrawingCacheChanged_;
    }

    // driven render ///////////////////////////////////
    void SetIsMarkDriven(bool isMarkDriven)
    {
        isMarkDriven_ = isMarkDriven;
    }

    bool IsMarkDriven() const
    {
        return isMarkDriven_;
    }

    void SetIsMarkDrivenRender(bool isMarkDrivenRender)
    {
        isMarkDrivenRender_ = isMarkDrivenRender;
    }

    bool IsMarkDrivenRender() const
    {
        return isMarkDrivenRender_;
    }

    void SetItemIndex(int index)
    {
        itemIndex_ = index;
    }

    int GetItemIndex() const
    {
        return itemIndex_;
    }

    void SetPaintState(bool paintState)
    {
        paintState_ = paintState;
    }

    bool GetPaintState() const
    {
        return paintState_;
    }

    void SetIsContentChanged(bool isChanged)
    {
        isContentChanged_ = isChanged;
    }

    bool IsContentChanged() const
    {
        return isContentChanged_ || HasAnimation();
    }

    bool HasAnimation() const
    {
        return !animationManager_.animations_.empty();
    }

    bool HasFilter() const
    {
        return hasFilter_;
    }

    void SetHasFilter(bool hasFilter)
    {
        hasFilter_ = hasFilter;
    }

    std::recursive_mutex& GetSurfaceMutex() const
    {
        return surfaceMutex_;
    }

    bool HasHardwareNode() const
    {
        return hasHardwareNode_;
    }

    void SetHasHardwareNode(bool hasHardwareNode)
    {
        hasHardwareNode_ = hasHardwareNode;
    }

    bool HasAbilityComponent() const
    {
        return hasAbilityComponent_;
    }

    void SetHasAbilityComponent(bool hasAbilityComponent)
    {
        hasAbilityComponent_ = hasAbilityComponent;
    }

    uint32_t GetCacheSurfaceThreadIndex() const
    {
        return cacheSurfaceThreadIndex_;
    }

    bool IsMainThreadNode() const
    {
        return isMainThreadNode_;
    }

    void SetIsMainThreadNode(bool isMainThreadNode)
    {
        isMainThreadNode_ = isMainThreadNode;
    }

    bool IsScale() const
    {
        return isScale_;
    }

    void SetIsScale(bool isScale)
    {
        isScale_ = isScale;
    }

    void SetPriority(NodePriorityType priority)
    {
        priority_ = priority;
    }

    NodePriorityType GetPriority()
    {
        return priority_;
    }

    bool HasCachedTexture() const
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
        return isTextureValid_;
#else
        return true;
#endif
#else
        return false;
#endif
    }

    void SetDrawRegion(std::shared_ptr<RectF> rect)
    {
        drawRegion_ = rect;
    }

    void UpdateDrawRegion();
#ifndef USE_ROSEN_DRAWING
    void UpdateEffectRegion(std::optional<SkPath>& region) const;
#else
    void UpdateEffectRegion(std::optional<Drawing::Path>& region) const;
#endif
    // check node's rect if it has valid filter cache
    bool IsFilterCacheValid() const;
    void UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground=true) const;

    void CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd);
    bool IsForcedDrawInGroup() const;
    bool IsSuggestedDrawInGroup() const;
    void CheckDrawingCacheType();
    bool HasCacheableAnim() const { return hasCacheableAnim_; }

    enum NodeGroupType {
        NONE = 0,
        GROUPED_BY_ANIM,
        GROUPED_BY_UI,
        GROUPED_BY_USER,
    };
    void MarkNodeGroup(NodeGroupType type, bool isNodeGroup);
    NodeGroupType GetNodeGroupType()
    {
        return nodeGroupType_;
    }

    /////////////////////////////////////////////

    // shared transition params, in format <InNodeId, target weakPtr>, nullopt means no transition
    using SharedTransitionParam = std::pair<NodeId, std::weak_ptr<RSRenderNode>>;
    void SetSharedTransitionParam(const std::optional<SharedTransitionParam>&& sharedTransitionParam);
    const std::optional<SharedTransitionParam>& GetSharedTransitionParam() const;

    void SetGlobalAlpha(float alpha);
    float GetGlobalAlpha() const;
    virtual void OnAlphaChanged() {}

    void SetRSFrameRateRange(FrameRateRange range)
    {
        rsRange_ = range;
    }

    FrameRateRange GetRSFrameRateRange();

    void SetUIFrameRateRange(FrameRateRange range)
    {
        uiRange_ = range;
    }

    FrameRateRange GetUIFrameRateRange() const
    {
        return uiRange_;
    }

    void ResetRSFrameRateRange();
    void ResetUIFrameRateRange();

protected:
    explicit RSRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    void AddGeometryModifier(const std::shared_ptr<RSRenderModifier> modifier);
    RSPaintFilterCanvas::SaveStatus renderNodeSaveCount_;
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> drawCmdModifiers_;
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;

    virtual RectI GetFilterRect() const;
    void OnTreeStateChanged() override;

private:
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);

    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, std::optional<RectI> clipRect);

    bool isDirtyRegionUpdated_ = false;
    bool isLastVisible_ = false;
    bool fallbackAnimationOnDestroy_ = true;
    uint32_t disappearingTransitionCount_ = 0;
    RectI oldDirty_;
    RectI oldDirtyInSurface_;
    RSProperties renderProperties_;
    RSAnimationManager animationManager_;
    std::map<PropertyId, std::shared_ptr<RSRenderModifier>> modifiers_;
    // bounds and frame modifiers must be unique
    std::shared_ptr<RSRenderModifier> boundsModifier_;
    std::shared_ptr<RSRenderModifier> frameModifier_;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    sk_sp<SkSurface> cacheCompletedSurface_ = nullptr;
#else
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
#endif
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
    GrBackendTexture cacheBackendTexture_;
    GrBackendTexture cacheCompletedBackendTexture_;
    bool isTextureValid_ = false;
#endif
#endif
    std::atomic<bool> isStaticCached_ = false;
    CacheType cacheType_ = CacheType::NONE;
    // drawing group cache
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    bool isDrawingCacheChanged_ = false;

    mutable std::recursive_mutex surfaceMutex_;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    bool isMainThreadNode_ = true;
    bool isScale_ = false;
    bool hasFilter_ = false;
    bool hasHardwareNode_ = false;
    bool hasAbilityComponent_ = false;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;

    // driven render
    int itemIndex_ = -1;
    bool isMarkDriven_ = false;
    bool isMarkDrivenRender_ = false;
    bool paintState_ = false;
    bool isContentChanged_ = false;
    float globalAlpha_ = 1.0f;
    std::optional<SharedTransitionParam> sharedTransitionParam_;

    std::shared_ptr<RectF> drawRegion_ = nullptr;
    NodeGroupType nodeGroupType_ = NodeGroupType::NONE;

    // shadowRectOffset means offset between shadowRect and absRect of node
    int shadowRectOffsetX_ = 0;
    int shadowRectOffsetY_ = 0;
    // Only use in RSRenderNode::DrawCacheSurface to calculate scale factor
    float boundsWidth_ = 0.0f;
    float boundsHeight_ = 0.0f;
    std::unordered_set<RSModifierType> dirtyTypes_;
    bool hasCacheableAnim_ = false;

    FrameRateRange rsRange_ = {0, 0, 0};
    FrameRateRange uiRange_ = {0, 0, 0};

    friend class RSRenderTransition;
    friend class RSRenderNodeMap;
    friend class RSProxyRenderNode;
    friend class RSBaseRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
