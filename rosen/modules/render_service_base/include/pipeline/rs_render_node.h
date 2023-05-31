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
#include <unordered_set>

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#else
#include "draw/surface.h"
#endif

#include "animation/rs_animation_manager.h"
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

    std::pair<bool, bool> Animate(int64_t timestamp) override;
    // PrepareCanvasRenderNode in UniRender
    bool Update(
        RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, RectI clipRect);
    // Other situation
    bool Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty);
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

    void SetStaticCached(bool isStaticCached)
    {
        isStaticCached_ = isStaticCached;
    }

    bool IsStaticCached() const
    {
        return isStaticCached_;
    }

#ifdef NEW_SKIA
    void InitCacheSurface(GrRecordingContext* grContext);
#else
    void InitCacheSurface(GrContext* grContext);
#endif

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCacheSurface() const
#else
    std::shared_ptr<Drawing::Surface> GetCacheSurface() const
#endif
    {
        return cacheSurface_;
    }

    void UpdateCompletedCacheSurface()
    {
        std::swap(cacheSurface_, cacheCompletedSurface_);
    }

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCompletedCacheSurface() const
#else
    std::shared_ptr<Drawing::Surface> GetCompletedCacheSurface() const
#endif
    {
        return cacheCompletedSurface_;
    }

    void ClearCacheSurface()
    {
        cacheSurface_ = nullptr;
        cacheCompletedSurface_ = nullptr;
    }

    void DrawCacheSurface(RSPaintFilterCanvas& canvas, bool isSubThreadNode = false) const;

    void SetCacheType(CacheType cacheType)
    {
        cacheType_ = cacheType;
    }

    CacheType GetCacheType() const
    {
        return cacheType_;
    }

    float GetShadowRectOffsetX() const
    {
        return shadowRectOffsetX_;
    }

    float GetShadowRectOffsetY() const
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
        return cacheCompletedSurface_ != nullptr;
    }

    void SetCacheTexture(sk_sp<SkImage> texture)
    {
        cacheTexture_ = texture;
    }

    sk_sp<SkImage> GetCacheTexture() const
    {
        return cacheTexture_;
    }

    void SetDrawRegion(std::shared_ptr<RectF> rect)
    {
        drawRegion_ = rect;
    }

    void UpdateDrawRegion();

    bool HasGroupableAnimations() const;
    bool isForcedDrawInGroup() const;
    bool isSuggestedDrawInGroup() const;

    enum NodeGroupType {
        NONE,
        GROUPED_BY_ANIM,
        GROUPED_BY_UI,
        GROUPED_BY_USER,
    };
    void MarkNodeGroup(NodeGroupType type);
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

protected:
    explicit RSRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    void AddGeometryModifier(const std::shared_ptr<RSRenderModifier> modifier);
    RSPaintFilterCanvas::SaveStatus renderNodeSaveCount_;
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> drawCmdModifiers_;
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;

private:
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);

    // clipRect only used in UniRener when calling PrepareCanvasRenderNode
    // PrepareCanvasRenderNode in UniRender: needClip = true and clipRect is meaningful
    // Other situation: needClip = false and clipRect is meaningless
    bool Update(
        RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, bool needClip, RectI clipRect);
    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, bool needClip, RectI clipRect);

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
    std::atomic<bool> isStaticCached_ = false;
    CacheType cacheType_ = CacheType::NONE;
    // drawing group cache
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    bool isDrawingCacheChanged_ = false;

    sk_sp<SkImage> cacheTexture_;
    bool isMainThreadNode_ = true;
    bool isScale_ = false;
    bool hasFilter_ = false;
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

    RectI shadowRect_;
    // shadowRectOffset means offset between shadowRect and absRect of node
    float shadowRectOffsetX_ = 0.0f;
    float shadowRectOffsetY_ = 0.0f;
    // Only use in RSRenderNode::DrawCacheSurface to calculate scale factor
    float boundsWidth_ = 0.0f;
    float boundsHeight_ = 0.0f;

    friend class RSRenderTransition;
    friend class RSRenderNodeMap;
    friend class RSProxyRenderNode;
    friend class RSBaseRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
