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
#include <memory>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "pipeline/rs_base_render_node.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSSurfaceRenderNode;
class RSScreenRenderNode;
class RSCanvasRenderNode;
class RSCanvasDrawingRenderNode;
class RSLogicalDisplayRenderNode;
class RSSurfaceHandler;
class RSUIRenderDirector;

class RSB_EXPORT RSRenderNodeMap final {
public:
    bool RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr);
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


    std::string GetSelfDrawSurfaceNameByPidAndUniqueId(pid_t nodePid, uint64_t uniqueId);

    bool ContainPid(pid_t pid) const;
    // On remote died, the instance root node will be directly removed from the tree,
    // and the remaining subtree will be released in batches (this feature is enabled by default).
    void FilterNodeByPid(pid_t pid, bool immediate = false);
    void MoveRenderNodeMap(
        std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap, pid_t pid);
    void TraversalNodes(std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const;
    void TraversalNodesByPid(int pid, std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const;
    void TraverseSurfaceNodes(std::function<void (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const;
    void TraverseSurfaceNodesBreakOnCondition(
        std::function<bool (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const;
    void TraverseScreenNodes(std::function<void (const std::shared_ptr<RSScreenRenderNode>&)> func) const;
    void TraverseLogicalDisplayNodes(
        std::function<void (const std::shared_ptr<RSLogicalDisplayRenderNode>&)> func) const;
    void TraverseCanvasDrawingNodes(std::function<void (const std::shared_ptr<RSCanvasDrawingRenderNode>&)> func) const;
    const std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& GetResidentSurfaceNodeMap() const;
    bool IsResidentProcessNode(NodeId id) const;
    bool IsUIExtensionSurfaceNode(NodeId id) const;
    void DestroyTokenNode(pid_t pid, uint64_t token);

    NodeId GetEntryViewNodeId() const;
    NodeId GetWallPaperViewNodeId() const;
    NodeId GetScreenLockWindowNodeId() const;
    NodeId GetNegativeScreenNodeId() const;
    void ObtainScreenLockWindowNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void ObtainLauncherNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    size_t GetNodeCountByPid(pid_t pid) const;

    uint32_t GetVisibleLeashWindowCount() const;
    uint64_t GetSize() const;

    // call from main thread
    std::vector<NodeId> GetSelfDrawingNodeInProcess(pid_t pid);

    bool AttachToDisplay(
        std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode, ScreenId screenId, bool toContainer) const;
    void RegisterNeedAttachedNode(std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode);
    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetNeedAttachedNode() const
    {
        return needAttachedNode_;
    }

    void ClearNeedAttachedNode()
    {
        needAttachedNode_.clear();
    }

    void RegisterSurfaceHandler(NodeId nodeId, std::shared_ptr<RSSurfaceHandler> surfaceHandler);
 
    std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler(NodeId nodeId, bool unregister);
 
    void UnregisterSurfaceHandlerByPid(pid_t pid);
#ifndef ROSEN_CROSS_PLATFORM
    void SaveSurfaceHandlerInfo(
        NodeId nodeId, const std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> &info);
    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> GetSurfaceHandlerInfo(
        NodeId nodeId) const;
    void RemoveSurfaceHandlerInfo(NodeId nodeId);
#endif

private:
    explicit RSRenderNodeMap();
    ~RSRenderNodeMap() = default;
    RSRenderNodeMap(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap(const RSRenderNodeMap&&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&) = delete;
    RSRenderNodeMap& operator=(const RSRenderNodeMap&&) = delete;

    std::weak_ptr<RSContext> context_;
    NodeId entryViewNodeId_ = 0;
    NodeId negativeScreenNodeId_ = 0;
    NodeId wallpaperViewNodeId_ = 0;
    NodeId screenLockWindowNodeId_ = 0;

    std::mutex pSurfaceSizeBackUpSetMutex_;

    std::unordered_map<pid_t, std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> renderNodeMap_;
    std::unordered_map<pid_t, std::vector<std::shared_ptr<RSUIRenderDirector>>> rsUIRenderDirectorMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> residentSurfaceNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSScreenRenderNode>> screenNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSLogicalDisplayRenderNode>> logicalDisplayNodeMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSCanvasDrawingRenderNode>> canvasDrawingNodeMap_;
    std::unordered_map<NodeId, bool> purgeableNodeMap_;
    std::unordered_map<pid_t, std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>>
        selfDrawingNodeInProcess_;
    std::unordered_set<NodeId> unInTreeNodeSet_;

    std::mutex surfaceHandlerMutex_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceHandler>> surfaceHandlerMap_;

    void Initialize(const std::weak_ptr<RSContext>& context);

    void AddUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void RemoveUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void CollectSelfDrawingNodeOfSubTree(std::vector<NodeId>& vec, const std::shared_ptr<RSBaseRenderNode> rootNode);
    std::unordered_set<NodeId> uiExtensionSurfaceNodes_;
    mutable std::mutex uiExtensionSurfaceNodesMutex_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> needAttachedNode_;

#ifndef ROSEN_CROSS_PLATFORM
    std::unordered_map<NodeId, std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>>>
        surfaceHandlerInfoMap_;
    mutable std::mutex surfaceHandlerInfoMutex_;
#endif

    friend class RSContext;
    friend class RSMainThread;
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_MAP_H
