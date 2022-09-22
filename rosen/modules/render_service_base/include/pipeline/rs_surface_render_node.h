/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include <functional>
#include <limits>
#include <memory>
#include <surface.h>

#include "common/rs_vector4.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_surface_handler.h"
#include "refbase.h"
#include "sync_fence.h"
#include "common/rs_occlusion_region.h"
#include "transaction/rs_occlusion_data.h"

namespace OHOS {
namespace Rosen {
class RSCommand;
class RSDirtyRegionManager;
class RSSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    ~RSSurfaceRenderNode() override;

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);
    void ResetParent() override;

    bool IsAppWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE;
    }

    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return nodeType_;
    }

    void SetSurfaceNodeType(RSSurfaceNodeType nodeType)
    {
        nodeType_ = nodeType;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetOffSetX(int32_t offset)
    {
        offsetX_ = offset;
    }

    int32_t GetOffSetX()
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY()
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
                        std::vector<RSBaseRenderNode::SharedPtr>& vec,
                        bool isUniRender) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void SetContextBounds(const Vector4f bounds);

    void SetTotalMatrix(const SkMatrix& totalMatrix)
    {
        totalMatrix_ = totalMatrix;
    }
    const SkMatrix& GetTotalMatrix() const
    {
        return totalMatrix_;
    }

    // pass render context (matrix/alpha/clip) from RT to RS
    void SetContextMatrix(const SkMatrix& transform, bool sendMsg = true);
    const SkMatrix& GetContextMatrix() const;

    void SetContextAlpha(float alpha, bool sendMsg = true);
    float GetContextAlpha() const;

    void SetContextClipRegion(SkRect clipRegion, bool sendMsg = true);
    const SkRect& GetContextClipRegion() const;

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;
    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;

    void SetSrcRect(const RectI& rect)
    {
        srcRect_ = rect;
    }

    const RectI& GetSrcRect() const
    {
        return srcRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        if (dstRect_ != dstRect) {
            dstRectChanged_= true;
        }
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    void SetGlobalAlpha(float alpha)
    {
        if (globalAlpha_ == alpha) {
            return;
        }
        alphaChanged_ = true;
        globalAlpha_ = alpha;
    }

    float GetGlobalAlpha() const
    {
        return globalAlpha_;
    }

    void SetOcclusionVisible(bool visible)
    {
        isOcclusionVisible_ = visible;
    }

    bool GetOcclusionVisible() const
    {
        return isOcclusionVisible_;
    }

    const Occlusion::Region& GetVisibleRegion() const
    {
        return visibleRegion_;
    }

    void SetAbilityBGAlpha(uint8_t alpha)
    {
        abilityBgAlpha_ = alpha;
        alphaChanged_ = true;
    }

    uint8_t GetAbilityBgAlpha() const
    {
        return abilityBgAlpha_;
    }

    void setQosCal(bool qosPidCal)
    {
        qosPidCal_ = qosPidCal;
    }

    void SetVisibleRegionRecursive(const Occlusion::Region& region,
                                   VisibleData& visibleVec,
                                   std::map<uint32_t, bool>& pidVisMap);

    const Occlusion::Region& GetVisibleDirtyRegion() const
    {
        return visibleDirtyRegion_;
    }

    void SetVisibleDirtyRegion(const Occlusion::Region& region)
    {
        visibleDirtyRegion_ = region;
    }

    const Occlusion::Region& GetDirtyRegionBelowCurrentLayer() const
    {
        return dirtyRegionBelowCurrentLayer_;
    }

    void SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region)
    {
        Occlusion::Rect dstrect { dstRect_.left_, dstRect_.top_,
            dstRect_.GetRight(), dstRect_.GetBottom() };
        Occlusion::Region dstregion {dstrect};
        dirtyRegionBelowCurrentLayer_ = dstregion.And(region);
        dirtyRegionBelowCurrentLayerIsEmpty_ = dirtyRegionBelowCurrentLayer_.IsEmpty();
    }

    bool GetDstRectChanged() const
    {
        return dstRectChanged_;
    }

    void CleanDstRectChanged()
    {
        dstRectChanged_ = false;
    }

    bool GetAlphaChanged() const
    {
        return alphaChanged_;
    }

    void CleanAlphaChanged()
    {
        alphaChanged_ = false;
    }

    void SetGloblDirtyRegion(const RectI& rect)
    {
        globalDirtyRegion_ = GetDstRect().IntersectRect(rect);
        globalDirtyRegionIsEmpty_ = globalDirtyRegion_.IsEmpty();
    }

    RectI GetGlobalDirtyRegion() const
    {
        return globalDirtyRegion_;
    }

    void SetConsumer(const sptr<Surface>& consumer);

    void UpdateSurfaceDefaultSize(float width, float height);

    BlendType GetBlendType();
    void SetBlendType(BlendType blendType);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener"
    // to save callback method sent by RT or UI which depends on the value of "isFromRenderThread".
    void RegisterBufferAvailableListener(
        sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyRTBufferAvailable();
    bool IsNotifyRTBufferAvailable() const;
    bool IsNotifyRTBufferAvailablePre() const;

    void NotifyUIBufferAvailable();
    bool IsNotifyUIBufferAvailable() const;
    void SetIsNotifyUIBufferAvailable(bool available);

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcedly "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(std::function<void(void)> callback);
    bool NeedSetCallbackForRenderThreadRefresh();

    void ParallelVisitLock()
    {
        parallelVisitMutex_.lock();
    }

    void ParallelVisitUnlock()
    {
        parallelVisitMutex_.unlock();
    }

    bool IsIntersectWithDirty(const RectI& r) const
    {
        if (dirtyManager_ == nullptr) {
            return true;
        }
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };

        // if current node is in occluded region of the surface, it could be skipped in process step
        bool isVisible = visibleRegion_.IsIntersectWith(nodeRect);
        if (!isVisible) {
            return false;
        }

        // if current node rect r is in global dirtyregion, it CANNOT be skipped
        if (!globalDirtyRegionIsEmpty_) {
            auto globalRect = r.IntersectRect(globalDirtyRegion_);
            if (!globalRect.IsEmpty()) {
                return true;
            }
        }
        
        // if current node is in visible dirtyRegion, it CANNOT be skipped
        bool localIntersect = visibleDirtyRegion_.IsIntersectWith(nodeRect);
        if (localIntersect) {
            return true;
        }

        // if current node is transparent
        const uint8_t opacity = 255;
        if (!(GetAbilityBgAlpha() == opacity &&
                ROSEN_EQ(GetRenderProperties().GetAlpha(), 1.0f))) {
            return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
        }
        return false;
    }

    bool SubNodeVisible(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node is in occluded region of the surface, it could be skipped in process step
        return visibleRegion_.IsIntersectWith(nodeRect);
    }

    bool SubNodeIntersectWithDirty(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node rect r is in global dirtyregion, it CANNOT be skipped
        if (!globalDirtyRegionIsEmpty_) {
            auto globalRect = r.IntersectRect(globalDirtyRegion_);
            if (!globalRect.IsEmpty()) {
                return true;
            }
        }
        // if current node is in visible dirtyRegion, it CANNOT be skipped
        bool localIntersect = visibleDirtyRegion_.IsIntersectWith(nodeRect);
        if (localIntersect) {
            return true;
        }
        // if current node is transparent
        const uint8_t opacity = 255;
        if (!(GetAbilityBgAlpha() == opacity &&
                ROSEN_EQ(GetRenderProperties().GetAlpha(), 1.0f))) {
            return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
        }
        return false;
    }

    bool SubNodeNeedDraw(const RectI &r, PartialRenderType opDropType) const
    {
        if (dirtyManager_ == nullptr) {
            return true;
        }
        if (r.IsEmpty()) {
            return true;
        }
        switch (opDropType) {
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP:
                return SubNodeIntersectWithDirty(r);
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION:
                return SubNodeVisible(r);
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY:
                return SubNodeVisible(r) && SubNodeIntersectWithDirty(r);
            case PartialRenderType::DISABLED:
            case PartialRenderType::SET_DAMAGE:
            default:
                return true;
        }
        return true;
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

    void SetAppFreeze(bool isAppFreeze)
    {
        isAppFreeze_ = isAppFreeze;
    }

    bool IsAppFreeze() const
    {
        return isAppFreeze_;
    }

    bool GetZorderChanged() const
    {
        return (std::abs(GetRenderProperties().GetPositionZ() - positionZ_) > (std::numeric_limits<float>::epsilon()));
    }

    bool IsZOrderPromoted() const
    {
        return GetRenderProperties().GetPositionZ() > positionZ_;
    }

    void UpdatePositionZ()
    {
        positionZ_ = GetRenderProperties().GetPositionZ();
    }
private:
    void ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node);

    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutex_;

    std::mutex parallelVisitMutex_;

    SkMatrix contextMatrix_ = SkMatrix::I();
    float contextAlpha_ = 1.0f;
    SkRect contextClipRect_ = SkRect::MakeEmpty();

    bool isSecurityLayer_ = false;
    RectI srcRect_;
    SkMatrix totalMatrix_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float globalAlpha_ = 1.0f;
    float positionZ_ = 0.0f;
    bool qosPidCal_ = false;

    std::string name_;
    RSSurfaceNodeType nodeType_ = RSSurfaceNodeType::DEFAULT;
    BlendType blendType_ = BlendType::BLEND_SRCOVER;
    bool isNotifyRTBufferAvailablePre_ = false;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = false;
    std::atomic_bool isBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    RectI clipRegionFromParent_;
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleDirtyRegion_;
    bool isOcclusionVisible_ = true;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    RectI dstRect_;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool alphaChanged_ = false;
    RectI globalDirtyRegion_;

    std::atomic<bool> isAppFreeze_ = false;
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    bool globalDirtyRegionIsEmpty_ = false;
    // if a there a dirty layer under transparent clean layer, transparent layer should refreshed
    Occlusion::Region dirtyRegionBelowCurrentLayer_;
    bool dirtyRegionBelowCurrentLayerIsEmpty_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
