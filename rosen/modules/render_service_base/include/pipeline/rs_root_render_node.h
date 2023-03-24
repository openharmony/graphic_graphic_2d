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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H

#include "common/rs_macros.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
class RSSurface;
class RSDirtyRegionManager;
class RSB_EXPORT RSRootRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::ROOT_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSRootRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    ~RSRootRenderNode() override;

    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void AttachRSSurfaceNode(NodeId SurfaceNodeId);

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;
    std::shared_ptr<RSSurface> GetSurface();
    NodeId GetRSSurfaceNodeId();
    float GetSuggestedBufferWidth() const;
    float GetSuggestedBufferHeight() const;
    void UpdateSuggestedBufferSize(float width, float height);
    void SetEnableRender(bool enableRender)
    {
        if (enableRender_ != enableRender) {
            enableRender_ = enableRender;
            SetDirty();
        }
    }

    bool GetEnableRender() const
    {
        return enableRender_;
    }

    void SetNeedUpdateSurfaceNode(bool needUpdate)
    {
        needUpdateSurfaceNode_ = needUpdate;
    }

    bool GetNeedUpdateSurfaceNode() const
    {
        return needUpdateSurfaceNode_;
    }

private:
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
    NodeId surfaceNodeId_ = 0;
    bool enableRender_ = true;
    bool needUpdateSurfaceNode_ = false;
    float suggestedBufferWidth_ = 0.f;
    float suggestedBufferHeight_ = 0.f;

    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
