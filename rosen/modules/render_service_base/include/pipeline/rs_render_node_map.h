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
#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_MAP_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_MAP_H

#include <mutex>
#include <unordered_map>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSSurfaceRenderNode;
class RSB_EXPORT RSRenderNodeMap final {
public:
    bool RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr);
    void UnregisterRenderNode(NodeId id);

    // Get RenderNode with type T, return nullptr if not found or type mismatch
    template<typename T = RSBaseRenderNode>
    const std::shared_ptr<T> GetRenderNode(NodeId id) const
    {
        auto renderNode = GetRenderNode<RSBaseRenderNode>(id);
        return RSBaseRenderNode::ReinterpretCast<T>(renderNode);
    }
    template<>
    const std::shared_ptr<RSBaseRenderNode> GetRenderNode(NodeId id) const;

    const std::shared_ptr<RSRenderNode> GetAnimationFallbackNode() const;

    void FilterNodeByPid(pid_t pid);
    void TraversalNodes(std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const;
    void TraverseSurfaceNodes(std::function<void (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const;
private:
    explicit RSRenderNodeMap();
    ~RSRenderNodeMap() = default;
    RSRenderNodeMap(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap(const RSRenderNodeMap&&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&&) = delete;

private:
    std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>> renderNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeMap_;

    friend class RSContext;
    friend class RSMainThread;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_MAP_H
