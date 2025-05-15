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
#include <unordered_set>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSSurfaceRenderNode;
class RSDisplayRenderNode;
class RSCanvasDrawingRenderNode;
class RSB_EXPORT RSRenderNodeMap final {
public:
    bool RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr);
    bool RegisterDisplayRenderNode(const std::shared_ptr<RSDisplayRenderNode>& nodePtr);
    void UnregisterRenderNode(NodeId id);

    void RegisterUnTreeNode(NodeId id);
    bool UnRegisterUnTreeNode(NodeId id);

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

    const std::string GetSelfDrawSurfaceNameByPid(pid_t nodePid) const;

    bool ContainPid(pid_t pid) const;
    // On remote died, the instance root node will be directly removed from the tree,
    // and the remaining subtree will be released in batches (this feature is enabled by default).
    void FilterNodeByPid(pid_t pid);
    void MoveRenderNodeMap(
        std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap, pid_t pid);
    void TraversalNodes(std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const;
    void TraversalNodesByPid(int pid, std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const;
    void TraverseSurfaceNodes(std::function<void (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const;
    void TraverseSurfaceNodesBreakOnCondition(
        std::function<bool (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const;
    void TraverseDisplayNodes(std::function<void (const std::shared_ptr<RSDisplayRenderNode>&)> func) const;
    void TraverseCanvasDrawingNodes(std::function<void (const std::shared_ptr<RSCanvasDrawingRenderNode>&)> func) const;
    const std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& GetResidentSurfaceNodeMap() const;
    bool IsResidentProcessNode(NodeId id) const;
    bool IsUIExtensionSurfaceNode(NodeId id) const;

    NodeId GetEntryViewNodeId() const;
    NodeId GetWallPaperViewNodeId() const;
    NodeId GetScreenLockWindowNodeId() const;
    NodeId GetNegativeScreenNodeId() const;
    void ObtainScreenLockWindowNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void ObtainLauncherNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);

    uint32_t GetVisibleLeashWindowCount() const;
    uint64_t GetSize() const;

    // call from main thread
    std::vector<NodeId> GetSelfDrawingNodeInProcess(pid_t pid);
private:
    explicit RSRenderNodeMap();
    ~RSRenderNodeMap() = default;
    RSRenderNodeMap(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap(const RSRenderNodeMap&&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&&) = delete;

private:
    std::weak_ptr<RSContext> context_;
    NodeId entryViewNodeId_ = 0;
    NodeId negativeScreenNodeId_ = 0;
    NodeId wallpaperViewNodeId_ = 0;
    NodeId screenLockWindowNodeId_ = 0;

    std::unordered_map<pid_t, std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> renderNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> residentSurfaceNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSDisplayRenderNode>> displayNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSCanvasDrawingRenderNode>> canvasDrawingNodeMap_;
    std::unordered_map<NodeId, bool> purgeableNodeMap_;
    std::unordered_map<pid_t, std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>>
        selfDrawingNodeInProcess_;
    std::unordered_set<NodeId> unInTreeNodeSet_;

    void Initialize(const std::weak_ptr<RSContext>& context);

    void AddUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void RemoveUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void CollectSelfDrawingNodeOfSubTree(std::vector<NodeId>& vec, const std::shared_ptr<RSBaseRenderNode> rootNode);
    std::unordered_set<NodeId> uiExtensionSurfaceNodes_;
    mutable std::mutex uiExtensionSurfaceNodesMutex_;

    friend class RSContext;
    friend class RSMainThread;
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_MAP_H
