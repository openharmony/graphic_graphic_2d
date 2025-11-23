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

#include "feature/window_keyframe/rs_window_keyframe_render_node.h"

#include <algorithm>
#include "common/rs_common_def.h"
#include "memory/rs_memory_track.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"


namespace OHOS {
namespace Rosen {

std::unordered_map<NodeId, NodeId> RSWindowKeyFrameRenderNode::linkedNodeMap_;

RSWindowKeyFrameRenderNode::RSWindowKeyFrameRenderNode(
    NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSWindowKeyFrameRenderNode::~RSWindowKeyFrameRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSWindowKeyFrameRenderNode::OnTreeStateChanged()
{
    if (!IsOnTheTree()) {
        linkedNodeId_ = INVALID_NODEID;
    }

    RSRenderNode::OnTreeStateChanged();
}

void RSWindowKeyFrameRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }

    ApplyModifiers();
    visitor->QuickPrepareWindowKeyFrameRenderNode(*this);
}

void RSWindowKeyFrameRenderNode::SetLinkedNodeId(NodeId nodeId)
{
    linkedNodeId_ = nodeId;
}

NodeId RSWindowKeyFrameRenderNode::GetLinkedNodeId() const
{
    return linkedNodeId_;
}

size_t RSWindowKeyFrameRenderNode::GetLinkedNodeCount()
{
    return linkedNodeMap_.size();
}

void RSWindowKeyFrameRenderNode::ClearLinkedNodeInfo()
{
    linkedNodeMap_.clear();
}

void RSWindowKeyFrameRenderNode::CollectLinkedNodeInfo()
{
    linkedNodeMap_[GetId()] = GetLinkedNodeId();
}

void RSWindowKeyFrameRenderNode::ResetLinkedWindowKeyFrameInfo(RSRenderNode& node)
{
    auto& stgRenderParams = node.GetStagingRenderParams();
    if (stgRenderParams != nullptr) {
        stgRenderParams->SetWindowKeyFrameNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr());
        node.AddToPendingSyncList();
    }
}

bool RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(RSSurfaceRenderNode& surfaceNode, RSContext& rsContext)
{
    if (LIKELY(linkedNodeMap_.empty())) {
        return false;
    }

    int availableNodeCount = 0;
    const auto& nodeMap = rsContext.GetNodeMap();
    for (const auto& [keyFrameNodeId, linkedNodeId] : linkedNodeMap_) {
        auto rootNode = nodeMap.GetRenderNode<RSRootRenderNode>(linkedNodeId);
        auto keyFrameNode = nodeMap.GetRenderNode<RSWindowKeyFrameRenderNode>(keyFrameNodeId);
        if (UNLIKELY(rootNode == nullptr || keyFrameNode == nullptr)) {
            RS_LOGE("RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen rootNode or keyFrameNode is nullptr");
            continue;
        }
        
        auto parentNode = rootNode->GetParent().lock();
        if (parentNode == nullptr || parentNode->GetId() != surfaceNode.GetId()) {
            continue;
        }

        auto keyFrameNodeDrawable = keyFrameNode->GetRenderDrawable();
        auto& keyFrameNodeStagingRenderParams = keyFrameNode->GetStagingRenderParams();
        auto& rootNodeStagingRenderParams = rootNode->GetStagingRenderParams();
        if (UNLIKELY(keyFrameNodeDrawable == nullptr || keyFrameNodeStagingRenderParams == nullptr ||
            rootNodeStagingRenderParams == nullptr)) {
            RS_LOGE("RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen RenderParams or drawable is nullptr");
            continue;
        }

        keyFrameNodeStagingRenderParams->SetShouldPaint(true);
        keyFrameNodeStagingRenderParams->SetContentEmpty(false);
        keyFrameNode->AddToPendingSyncList();

        surfaceNode.SetHwcChildrenDisabledState();
        rootNodeStagingRenderParams->SetWindowKeyFrameNodeDrawable(keyFrameNodeDrawable);
        rootNode->AddToPendingSyncList();
        ++availableNodeCount;
    }

    return availableNodeCount > 0;
}

bool RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(
    RSSurfaceRenderNode& surfaceNode, RSContext& rsContext, RectI &behindWndRect)
{
    if (LIKELY(linkedNodeMap_.empty())) {
        return false;
    }
 
    bool updated = false;
    const auto& nodeMap = rsContext.GetNodeMap();
    for (const auto& [keyFrameNodeId, linkedNodeId] : linkedNodeMap_) {
        auto rootNode = nodeMap.GetRenderNode<RSRootRenderNode>(linkedNodeId);
        auto keyFrameNode = nodeMap.GetRenderNode<RSWindowKeyFrameRenderNode>(keyFrameNodeId);
        if (rootNode == nullptr || keyFrameNode == nullptr) {
            RS_LOGE("RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion get node failed");
            continue;
        }

        auto parentNode = rootNode->GetParent().lock();
        if (parentNode == nullptr || parentNode->GetId() != surfaceNode.GetId()) {
            continue;
        }
 
        const auto& rootProp = rootNode->GetRenderProperties();
        auto rootWidth = rootProp.GetBoundsWidth();
        auto rootHeight = rootProp.GetBoundsHeight();
        const auto& keyFrameProp = keyFrameNode->GetRenderProperties();
        auto keyFrameWidth = keyFrameProp.GetBoundsWidth();
        auto keyFrameHeight = keyFrameProp.GetBoundsHeight();
        if (rootWidth <= 0 || rootHeight <= 0 || keyFrameWidth <= 0 || keyFrameHeight <= 0) {
            continue;
        }
 
        RectI blurRect;
        auto ratio = rootWidth * keyFrameHeight / rootHeight >= keyFrameWidth ?
            keyFrameHeight / rootHeight : keyFrameWidth / rootWidth;
        blurRect.top_ = static_cast<int>(std::floor(std::min(behindWndRect.top_ * ratio, keyFrameHeight)));
        blurRect.left_ = static_cast<int>(std::floor(std::min(behindWndRect.left_ * ratio, keyFrameWidth)));
        blurRect.height_ = static_cast<int>(
            std::ceil(std::min(behindWndRect.height_ * ratio, keyFrameHeight - blurRect.top_)));
        blurRect.width_ = static_cast<int>(
            std::ceil(std::min(behindWndRect.width_ * ratio, keyFrameWidth - blurRect.left_)));
 
        behindWndRect = behindWndRect.JoinRect(blurRect);
        updated = true;
    }

    return updated;
}

} // namespace Rosen
} // namespace OHOS
