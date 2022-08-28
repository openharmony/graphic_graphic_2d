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
#include <memory>
#include <surface.h>

#include "display_type.h"
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

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    virtual ~RSSurfaceRenderNode();

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);
    void ResetParent() override;

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

    void SetProxy()
    {
        isProxy_ = true;
    }
    bool IsProxy() const
    {
        return isProxy_;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
                        std::vector<RSBaseRenderNode::SharedPtr>& vec,
                        bool isUniRender) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::SURFACE_NODE;
    }

    void SetContextBounds(const Vector4f bounds);

    // pass render context (matrix/alpha/clip) from RT to RS
    void SetContextMatrix(const SkMatrix& transform, bool sendMsg = true);
    const SkMatrix& GetContextMatrix() const;

    void SetTotalMatrix(const SkMatrix& totalMatrix)
    {
        totalMatrix_ = totalMatrix;
    }
    const SkMatrix& GetTotalMatrix() const
    {
        return totalMatrix_;
    }

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
        abilityBgAlphaChanged_ = true;
    }

    uint8_t GetAbilityBgAlpha() const
    {
        return abilityBgAlpha_;
    }

    void SetVisibleRegionRecursive(const Occlusion::Region& region, VisibleData& visibleVec)
    {
        visibleRegion_ = region;
        bool vis = region.GetSize() > 0;
        if (vis) {
            visibleVec.emplace_back(GetId());
        }
        SetOcclusionVisible(vis);
        for (auto& child : GetSortedChildren()) {
            if (child->GetType() == RSRenderNodeType::SURFACE_NODE) {
                auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
                if (surface == nullptr) {
                    continue;
                }
                surface->SetVisibleRegionRecursive(region, visibleVec);
            }
        }
    }

    bool GetDstRectChanged() const
    {
        return dstRectChanged_;
    }

    void CleanDstRectChanged()
    {
        dstRectChanged_ = false;
    }

    bool GetAbilityBgAlphaChanged() const
    {
        return abilityBgAlphaChanged_;
    }

    void CleanAbilityBgAlphaChanged()
    {
        abilityBgAlphaChanged_ = false;
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

private:
    void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);
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

    std::string name_;
    bool isProxy_ = false;
    BlendType blendType_ = BlendType::BLEND_SRCOVER;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    RectI clipRegionFromParent_;
    Occlusion::Region visibleRegion_;
    bool isOcclusionVisible_ = true;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    RectI dstRect_;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool abilityBgAlphaChanged_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
