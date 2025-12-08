/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_KEYFRAME_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_KEYFRAME_RENDER_NODE_H

#include <memory>
#include <unordered_map>
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSWindowKeyFrameRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSWindowKeyFrameRenderNode>;
    using SharedPtr = std::shared_ptr<RSWindowKeyFrameRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::WINDOW_KEYFRAME_NODE;

    virtual ~RSWindowKeyFrameRenderNode();

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::WINDOW_KEYFRAME_NODE;
    }

    void OnTreeStateChanged() override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void SetLinkedNodeId(NodeId nodeId);
    NodeId GetLinkedNodeId() const;

    void CollectLinkedNodeInfo();

    static size_t GetLinkedNodeCount();
    static void ClearLinkedNodeInfo();
    static void ResetLinkedWindowKeyFrameInfo(RSRenderNode& node);
    static bool PrepareLinkedNodeOffscreen(RSSurfaceRenderNode& surfaceNode, RSContext& rsContext);
    static bool UpdateKeyFrameBehindWindowRegion(RSSurfaceRenderNode& surfaceNode,
        RSContext& rsContext, RectI &behindWndRect);

protected:
    explicit RSWindowKeyFrameRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);

private:
    friend class RSWindowKeyFrameNodeCommandHelper;

    NodeId linkedNodeId_ = INVALID_NODEID;

    static std::unordered_map<NodeId, NodeId> linkedNodeMap_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_KEYFRAME_RENDER_NODE_H
