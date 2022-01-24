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

#include <memory>
#include <surface.h>

#include "pipeline/rs_render_node.h"

class SkCanvas;
namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;

    explicit RSSurfaceRenderNode(NodeId id);
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config);
    virtual ~RSSurfaceRenderNode();

    void SetConsumer(const sptr<Surface>& consumer);
    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    void SetFence(const int32_t fence);
    void SetDamageRegion(const Rect& damage);
    void IncreaseAvailableBuffer();
    int32_t ReduceAvailableBuffer();

    sptr<SurfaceBuffer>& GetBuffer()
    {
        return buffer_;
    }

    int32_t GetFence() const
    {
        return fence_;
    }

    sptr<SurfaceBuffer>& GetPreBuffer()
    {
        return preBuffer_;
    }

    int32_t GetPreFence() const
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

    // Only use in Render Service
    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;

    void SetParentId(NodeId parentId, bool sendMsg = true);
    NodeId GetParentId() const;

    void UpdateSurfaceDefaultSize(float width, float height);

    static void SendPropertyCommand(std::unique_ptr<RSCommand>& command);

private:
    friend class RSRenderTransition;
    sptr<Surface> consumer_;

    std::atomic<int> bufferAvailableCount_ = 0;
    SkMatrix matrix_;
    float alpha_ = 0.0f;
    float globalZOrder_ = 0.0f;
    NodeId parentId_ = 0;
    sptr<SurfaceBuffer> buffer_;
    sptr<SurfaceBuffer> preBuffer_;
    int32_t fence_ = -1;
    int32_t preFence_ = -1;
    Rect damageRect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
