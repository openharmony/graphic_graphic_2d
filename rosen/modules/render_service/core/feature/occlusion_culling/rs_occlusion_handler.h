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

#ifndef ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_HANDLER_H
#define ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_HANDLER_H
#include <unordered_map>

#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class OcclusionNode;
class RSB_EXPORT RSOcclusionHandler : public std::enable_shared_from_this<RSOcclusionHandler> {
public:
    RSOcclusionHandler(NodeId id, NodeId keyOcclusionNodeId) : rootNodeId_(id) {
        UpdateOcclusionCullingStatus(true, keyOcclusionNodeId);
    }
    RSOcclusionHandler() = delete;
    ~RSOcclusionHandler() {
        TerminateOcclusionProcessing();
    }
    // Status manage
    void UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId);
    void TerminateOcclusionProcessing();
    // Tree maintenance
    void ProcessOffTreeNodes(const std::unordered_set<NodeId>& ids);
    void CollectNode(const RSRenderNode& node);
    void CollectSubTree(const RSRenderNode& node, bool isSubTreeNeedPrepare);
    void UpdateFocusNode(const std::shared_ptr<RSRenderNode> rootNode);
    void UpdateChildrenOutOfRectInfo(const RSRenderNode& node);
    void UpdateSkippedSubTreeProp();
    // Occlusion detection
    void CalculateFrameOcclusion();
    std::unordered_set<NodeId> AcquireCulledNodes();

    NodeId GetRootNodeId() const {
        return rootNodeId_;
    }
    void UpdateKeyOcclusionNodeId(NodeId keyOcclusionNodeId) {
        keyOcclusionNodeId_ = keyOcclusionNodeId;
    }
    bool IsOcclusionActive() const {
        return isOcclusionActive_;
    }
private:
    RSOcclusionHandler(const RSOcclusionHandler&) = delete;
    RSOcclusionHandler(const RSOcclusionHandler&&) = delete;
    RSOcclusionHandler& operator=(const RSOcclusionHandler&) = delete;
    RSOcclusionHandler& operator=(const RSOcclusionHandler&&) = delete;

    void CollectRootNode(const RSRenderNode& node);
    void CollectNodeInner(const RSRenderNode& node);
    void CollectSubTreeInner(const RSRenderNode& node);
    void DumpSubTreeOcclusionInfo(const RSRenderNode& node);
    void DebugPostOcclusionProcessing();

    bool isOcclusionActive_ = false;
    int32_t debugLevel_ = 0;
    NodeId keyOcclusionNodeId_ = INVALID_NODEID;
    NodeId rootNodeId_ = INVALID_NODEID;
    std::weak_ptr<const RSRenderNode> rootNode_;
    std::shared_ptr<OcclusionNode> rootOcclusionNode_ = nullptr;
    std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>> occlusionNodes_;
    std::unordered_set<uint64_t> subTreeSkipPrepareNodes_;
    std::unordered_set<NodeId> culledNodes_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_HANDLER_H
