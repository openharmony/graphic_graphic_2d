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

#include "feature/occlusion_culling/rs_occlusion_handler.h"

#include "common/rs_optional_trace.h"
#include "feature/occlusion_culling/rs_occlusion_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_render_node.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
void RSOcclusionHandler::UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId)
{
    RS_TRACE_NAME_FMT("RSOcclusionHandler::UpdateOcclusionCullingStatus enable:%d", enable);
    if (!enable) {
        TerminateOcclusionProcessing();
        return;
    }
    isOcclusionActive_ = OcclusionCullingParam::IsIntraAppControlsLevelOcclusionCullingEnable();
    keyOcclusionNodeId_ = keyOcclusionNodeId;
}

void RSOcclusionHandler::TerminateOcclusionProcessing()
{
    if (!isOcclusionActive_) {
        return;
    }
    RS_TRACE_NAME_FMT("RSOcclusionHandler::TerminateOcclusionProcessing");
    isOcclusionActive_ = false;
    keyOcclusionNodeId_ = INVALID_NODEID;
    subTreeSkipPrepareNodes_.clear();
    occlusionNodes_ = std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>>();
    rootOcclusionNode_ = nullptr;
    rootNode_.reset();
}

void RSOcclusionHandler::ProcessOffTreeNodes(const std::unordered_set<NodeId>& offTreeNodeIds)
{
    if (!isOcclusionActive_ || occlusionNodes_.size() == 0) {
        return;
    }
    RS_TRACE_NAME_FMT("RSOcclusionHandler::ProcessOffTreeNodes");
    for (auto &id : offTreeNodeIds) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSOcclusionHandler off id %lu", id);
        if (keyOcclusionNodeId_ == id) {
            TerminateOcclusionProcessing();
        }
        auto it = occlusionNodes_.find(id);
        if (it == occlusionNodes_.end() || it->second == nullptr) {
            continue;
        }
        it->second->RemoveSubTree(occlusionNodes_);
        if (occlusionNodes_.empty()) {
            return;
        }
    }
}

void RSOcclusionHandler::CollectNode(const RSRenderNode& node)
{
    if (!isOcclusionActive_) {
        return;
    }
    if (node.GetId() == rootNodeId_) {
        CollectRootNode(node);
    } else {
        CollectNodeInner(node);
    }
}

void RSOcclusionHandler::CollectRootNode(const RSRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSOcclusionHandler::CollectRootNode node id = %lu", node.GetId());
    auto itNode = occlusionNodes_.find(node.GetId());
    auto ocNode = itNode != occlusionNodes_.end() ?
        itNode->second : std::make_shared<OcclusionNode>(node.GetId(), node.GetType());
    rootOcclusionNode_ = ocNode;
    rootNode_ = node.weak_from_this();
    ocNode->MarkAsRootOcclusionNode();
    ocNode->UpdateClipRect(node);
    occlusionNodes_[ocNode->GetId()] = ocNode;
}

void RSOcclusionHandler::CollectNodeInner(const RSRenderNode& node)
{
    auto nodeId = node.GetId();
    auto parent = node.GetParent().lock();
    if (!parent) {
        return;
    }
    auto itParent = occlusionNodes_.find(parent->GetId());
    if (itParent == occlusionNodes_.end() || itParent->second == nullptr || itParent->second->IsSubTreeIgnored()) {
        return;
    }
    auto parentOcNode = itParent->second;
    auto itNode = occlusionNodes_.find(node.GetId());
    auto ocNode = (itNode != occlusionNodes_.end() && itNode->second != nullptr) ?
        itNode->second : std::make_shared<OcclusionNode>(nodeId, node.GetType());

    parentOcNode->ForwardOrderInsert(ocNode);
    ocNode->CollectNodeProperties(node);
    ocNode->CalculateNodeAllBounds();
    occlusionNodes_[nodeId] = ocNode;
}

void RSOcclusionHandler::CollectSubTree(const RSRenderNode& node,
    bool isSubTreeNeedPrepare)
{
    if (isSubTreeNeedPrepare || !isOcclusionActive_) {
        return;
    }
    auto itNode = occlusionNodes_.find(node.GetId());
    if (itNode == occlusionNodes_.end() || itNode->second == nullptr) {
        CollectSubTreeInner(node);
        return;
    }
    // If the node is the root of a static subtree and the node can participate in occlusion culling,
    // recollect the node's attributes and update the subtree.
    if (!itNode->second->IsSubTreeIgnored()) {
        CollectNodeInner(node);
        subTreeSkipPrepareNodes_.insert(node.GetId());
        return;
    }
    RS_TRACE_NAME_FMT("RSOcclusionHandler::CollectSubTree node id = %llu", node.GetId());
    CollectSubTreeInner(node);
}

void RSOcclusionHandler::CollectSubTreeInner(const RSRenderNode& node)
{
    CollectNodeInner(node);
    auto itNode = occlusionNodes_.find(node.GetId());
    // If the node is not collected or ignored, return.
    if (itNode == occlusionNodes_.end() || itNode->second == nullptr || itNode->second->IsSubTreeIgnored()) {
        return;
    }
    auto sortChildren = *(node.GetSortedChildren());
    std::for_each(sortChildren.begin(), sortChildren.end(), [&](std::shared_ptr<RSRenderNode> node) {
        CollectSubTreeInner(*node);
    });
}

void RSOcclusionHandler::UpdateChildrenOutOfRectInfo(const RSRenderNode& node)
{
    if (!isOcclusionActive_) {
        return;
    }
    auto it = occlusionNodes_.find(node.GetId());
    if (it == occlusionNodes_.end() || it->second == nullptr) {
        return;
    }
    it->second->UpdateChildrenOutOfRectInfo(node.HasChildrenOutOfRect());
}

void RSOcclusionHandler::UpdateSkippedSubTreeProp()
{
    if (!isOcclusionActive_ || subTreeSkipPrepareNodes_.empty()) {
        return;
    }
    RS_TRACE_NAME_FMT("RSOcclusionHandler::UpdateSkippedSubTreeProp");
    for (auto nodeId : subTreeSkipPrepareNodes_) {
        auto it = occlusionNodes_.find(nodeId);
        if (it == occlusionNodes_.end() || it->second == nullptr) {
            continue;
        }
        it->second->UpdateSubTreeProp();
    }
    subTreeSkipPrepareNodes_.clear();
}

void RSOcclusionHandler::CalculateFrameOcclusion()
{
    if (!isOcclusionActive_) {
        return;
    }
    {
        RS_TRACE_NAME_FMT("CalculateFrameOcclusion Started");
        culledNodes_.clear();
        culledNodes_.reserve(occlusionNodes_.size());
        std::unordered_set<NodeId> offTreeNodes_;
        if (rootOcclusionNode_) {
            rootOcclusionNode_->DetectOcclusion(culledNodes_, offTreeNodes_);
        }
        ProcessOffTreeNodes(offTreeNodes_);
        RS_TRACE_NAME_FMT(
            "CalculateFrameOcclusion Finished, occlusionNodes size is %d, culledNodes size is %d",
            occlusionNodes_.size(), culledNodes_.size());
    }
    DebugPostOcclusionProcessing();
}

std::unordered_set<NodeId> RSOcclusionHandler::AcquireCulledNodes()
{
    return std::move(culledNodes_);
}

void RSOcclusionHandler::DumpSubTreeOcclusionInfo(const RSRenderNode& node)
{
    if (!isOcclusionActive_) {
        return;
    }
    auto sortChildren = node.GetSortedChildren();
    auto it = occlusionNodes_.find(node.GetId());
    if (it == occlusionNodes_.end() || it->second == nullptr) {
        RS_TRACE_NAME_FMT("DumpSubTreeOcclusionInfo: error, node id: %lld not collect", node.GetId());
        for (const auto& child : *sortChildren) {
            DumpSubTreeOcclusionInfo(*child);
        }
        return;
    }
    auto ocNode = it->second;
    bool isNodeCulled = culledNodes_.count(ocNode->GetId()) > 0;
    RS_TRACE_NAME_FMT("%s isNodeCulled %d", ocNode->GetOcclusionNodeInfoString().c_str(), isNodeCulled);
    for (const auto& child : *sortChildren) {
        DumpSubTreeOcclusionInfo(*child);
    }
}

void RSOcclusionHandler::DebugPostOcclusionProcessing()
{
    if (debugLevel_ > 0 && rootOcclusionNode_) {
        std::vector<std::shared_ptr<OcclusionNode>> occlusionNodesVec;
        rootOcclusionNode_->PreorderTraversal(occlusionNodesVec);
        auto rootNodeShared = rootNode_.lock();
        if (rootNodeShared) {
            DumpSubTreeOcclusionInfo(*rootNodeShared);
        }
    }
}

} // namespace OHOS::Rosen
