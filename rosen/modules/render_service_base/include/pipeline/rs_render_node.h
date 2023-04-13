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

#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"

#include "animation/rs_animation_manager.h"
#include "common/rs_macros.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"

class SkCanvas;
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

    enum CacheType {
        NONE = 0,
        FREEZE,
        SPHERIZE,
    };
    ~RSRenderNode() override;
    bool IsDirty() const override;

    std::pair<bool, bool> Animate(int64_t timestamp) override;
    // PrepareCanvasRenderNode in UniRender
    bool Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, RectI clipRect);
    // Other situation
    bool Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty);

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

    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) {}
    void CheckCacheType();
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

    void SetFreeze(bool isFreeze)
    {
        isFreeze_ = isFreeze;
    }

    bool IsFreeze() const
    {
        return isFreeze_;
    }

    void SetCacheSurface(sk_sp<SkSurface> cacheSurface)
    {
        cacheSurface_ = std::move(cacheSurface);
    }

    sk_sp<SkSurface> GetCacheSurface() const
    {
        return cacheSurface_;
    }

    void ClearCacheSurface()
    {
        cacheSurface_ = nullptr;
    }

    void SetCacheType(CacheType cacheType)
    {
        cacheType_ = cacheType;
    }

    CacheType GetCacheType() const
    {
        return cacheType_;
    }

    void SetCacheTypeChanged(bool cacheTypeChanged)
    {
        cacheTypeChanged_ = cacheTypeChanged;
    }

    bool GetCacheTypeChanged() const
    {
        return cacheTypeChanged_;
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
        return isContentChanged_ || !animationManager_.animations_.empty();
    }

    void SetDrawRegion(std::shared_ptr<RectF> rect)
    {
        drawRegion_ = rect;
    }

    std::shared_ptr<RectF> GetDrawRegion() const
    {
        return drawRegion_;
    }
    /////////////////////////////////////////////

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
    void UpdateOverlayBounds();
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

    std::atomic<bool> isFreeze_ = false;
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    CacheType cacheType_ = CacheType::NONE;
    bool cacheTypeChanged_ = false;

    // driven render
    int itemIndex_ = -1;
    bool isMarkDriven_ = false;
    bool isMarkDrivenRender_ = false;
    bool paintState_ = false;
    bool isContentChanged_ = false;

    std::shared_ptr<RectF> drawRegion_ = nullptr;
    friend class RSRenderTransition;
    friend class RSRenderNodeMap;
    friend class RSProxyRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
