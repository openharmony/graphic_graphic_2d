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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H

#include <memory>
#include <surface.h>
#include <ibuffer_consumer_listener.h>

#include "platform/drawing/rs_surface.h"
#include "pipeline/rs_base_render_node.h"
#include "render_context/render_context.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSDisplayRenderNode : public RSBaseRenderNode {
public:
    enum CompositeType {
        COMPATIBLE_COMPOSITE = 0,
        HARDWARE_COMPOSITE,
        SOFTWARE_COMPOSITE
    };
    using WeakPtr = std::weak_ptr<RSDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSDisplayRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DISPLAY_NODE;

    explicit RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config, std::weak_ptr<RSContext> context = {});
    virtual ~RSDisplayRenderNode();

    void SetScreenId(uint64_t screenId)
    {
        screenId_ = screenId;
    }

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

    void SetDisplayOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    int32_t GetDisplayOffsetX() const
    {
        return offsetX_;
    }

    int32_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::DISPLAY_NODE;
    }

    bool IsMirrorDisplay() const;

    void SetCompositeType(CompositeType type);
    CompositeType GetCompositeType() const;
    void SetForceSoftComposite(bool flag);
    bool IsForceSoftComposite() const;
    void SetMirrorSource(SharedPtr node);
    void SetSecurityDisplay(bool isSecurityDisplay);
    bool GetSecurityDisplay() const;

    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    void SetDamageRegion(const Rect& damage);
    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;
    void SetConsumer(const sptr<Surface>& consumer);
    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    void SetFence(sptr<SyncFence> fence);
    void IncreaseAvailableBuffer();
    int32_t ReduceAvailableBuffer();

    WeakPtr GetMirrorSource() const
    {
        return mirrorSource_;
    }

    bool HasTransition(bool) const override
    {
        return false;
    }

    const sptr<Surface>& GetConsumer() const
    {
        return consumer_;
    }

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

    int32_t GetAvailableBufferCount() const
    {
        return bufferAvailableCount_;
    }

    std::shared_ptr<RSSurface> GetRSSurface() const
    {
        return surface_;
    }

    const Rect& GetDamageRegion() const
    {
        return damageRect_;
    }

    sptr<IBufferConsumerListener> GetConsumerListener() const
    {
        return consumerListener_;
    }

    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }

private:
    CompositeType compositeType_ { HARDWARE_COMPOSITE };
    uint64_t screenId_;
    int32_t offsetX_;
    int32_t offsetY_;
    bool forceSoftComposite_ { false };
    bool isMirroredDisplay_ = false;
    bool isSecurityDisplay_ = false;
    WeakPtr mirrorSource_;

    Rect damageRect_;
    float globalZOrder_ = 0.0f;
    std::shared_ptr<RSSurface> surface_;
    sptr<Surface> consumer_;
    std::atomic<int> bufferAvailableCount_ = 0;
    sptr<SurfaceBuffer> buffer_;
    sptr<SurfaceBuffer> preBuffer_;
    sptr<SyncFence> fence_;
    sptr<SyncFence> preFence_;
    bool surfaceCreated_ { false };
    sptr<IBufferConsumerListener> consumerListener_;
    RenderContext* renderContext_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
