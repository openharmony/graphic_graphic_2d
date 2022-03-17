/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ipc_callbacks/buffer_available_callback.h"
#include "pipeline/rs_render_node.h"
#include "refbase.h"
#include "sync_fence.h"

class SkCanvas;
namespace OHOS {
namespace Rosen {
class RSCommand;
class RSSurfaceRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    virtual ~RSSurfaceRenderNode();

    void SetConsumer(const sptr<Surface>& consumer);
    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    void SetFence(sptr<SyncFence> fence);
    void SetDamageRegion(const Rect& damage);
    void IncreaseAvailableBuffer();
    int32_t ReduceAvailableBuffer();

    sptr<SurfaceBuffer>& GetBuffer()
    {
        return buffer_;
    }

    sptr<SyncFence> GetFence() const
    {
        return fence_;
    }

    sptr<SurfaceBuffer>& GetPreBuffer()
    {
        return preBuffer_;
    }

    sptr<SyncFence> GetPreFence() const
    {
        return preFence_;
    }

    const Rect& GetDamageRegion() const
    {
        return damageRect_;
    }

    const sptr<Surface>& GetConsumer() const
    {
        return consumer_;
    }

    int32_t GetAvailableBufferCount() const
    {
        return bufferAvailableCount_;
    }

    std::string GetName() const
    {
        return name_;
    }

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::SURFACE_NODE;
    }

    void SetMatrix(const SkMatrix& transform, bool sendMsg = true);
    const SkMatrix& GetMatrix() const;

    void SetAlpha(float alpha, bool sendMsg = true);
    float GetAlpha() const;

    void SetClipRegion(Vector4f clipRegion, bool sendMsg = true);

    const Vector4f& GetClipRegion() const
    {
        return clipRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    // Only use in Render Service
    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;

    void SetParentId(NodeId parentId, bool sendMsg = true);
    NodeId GetParentId() const;

    void UpdateSurfaceDefaultSize(float width, float height);

    static void SendPropertyCommand(std::unique_ptr<RSCommand>& command);

    BlendType GetBlendType();
    void SetBlendType(BlendType blendType);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener" to save callback method sent by RT
    void RegisterBufferAvailableListener(sptr<RSIBufferAvailableCallback> callback);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyBufferAvailable(bool isBufferAvailable);
    bool IsBufferAvailable() const;

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcely "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(std::function<void(void)> callback);
    bool NeedSetCallbackForRenderThreadRefresh();

private:
    friend class RSRenderTransition;
    sptr<Surface> consumer_;

    std::mutex mutex_;
    std::atomic<int> bufferAvailableCount_ = 0;
    SkMatrix matrix_;
    float alpha_ = 1.0f;
    float globalZOrder_ = 0.0f;
    NodeId parentId_ = 0;
    sptr<SurfaceBuffer> buffer_;
    sptr<SurfaceBuffer> preBuffer_;
    sptr<SyncFence> fence_;
    sptr<SyncFence> preFence_;
    Rect damageRect_ = {0, 0, 0, 0};
    RectI dstRect_;
    Vector4f clipRect_;
    std::string name_;
    BlendType blendType_ = BlendType::BLEND_SRCOVER;
    std::atomic<bool> isBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callback_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
