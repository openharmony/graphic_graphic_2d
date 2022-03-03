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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H

#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
class RSSurface;
class RSRootRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::ROOT_NODE;
    explicit RSRootRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    ~RSRootRenderNode() override;

    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void AttachRSSurfaceNode(NodeId SurfaceNodeId);

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::ROOT_NODE;
    }

    std::shared_ptr<RSSurface> GetSurface();
    NodeId GetRSSurfaceNodeId();
    int32_t GetSurfaceWidth() const;
    int32_t GetSurfaceHeight() const;

    void AddSurfaceRenderNode(NodeId id);
    void ClearSurfaceNodeInRS();

private:
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
    NodeId surfaceNodeId_ = 0;
    std::vector<NodeId> childSurfaceNodeId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H