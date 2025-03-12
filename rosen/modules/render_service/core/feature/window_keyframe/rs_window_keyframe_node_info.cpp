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

#include "rs_window_keyframe_node_info.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

void RSWindowKeyframeNodeInfo::UpdateLinkedNodeId(NodeId srcNodeId, NodeId linkedNodeId)
{
    if (srcNodeId == INVALID_NODEID || linkedNodeId == INVALID_NODEID) {
        return;
    }

    linkedNodeMap_[linkedNodeId] = srcNodeId;
}

size_t RSWindowKeyframeNodeInfo::GetLinkedNodeCount() const
{
    return linkedNodeMap_.size();
}

void RSWindowKeyframeNodeInfo::ClearLinkedNodeInfo()
{
    linkedNodeMap_.clear();
}

bool RSWindowKeyframeNodeInfo::PrepareRootNodeOffscreen(RSSurfaceRenderNode& surfaceNode)
{
    if (LIKELY(linkedNodeMap_.empty())) {
        return false;
    }

    for (const auto& [linkedRootNodeId, sourceNodeId] : linkedNodeMap_) {
        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
        auto rootNode = nodeMap.GetRenderNode<RSRootRenderNode>(linkedRootNodeId);
        auto canvasNode = nodeMap.GetRenderNode<RSCanvasRenderNode>(sourceNodeId);
        if (rootNode == nullptr || canvasNode == nullptr) {
            RS_LOGE("RSWindowKeyframeNodeInfo::PrepareRootNodeOffscreen rootNode or canvasNode is nullptr");
            continue;
        }
        auto parentNode = rootNode->GetParent().lock();
        if (parentNode == nullptr || parentNode->GetId() != surfaceNode.GetId()) {
            continue;
        }
        auto rootNodeDrawable = rootNode->GetRenderDrawable();
        auto& rootNodeStagingRenderParams = rootNode->GetStagingRenderParams();
        auto& canvasNodeStagingRenderParams = canvasNode->GetStagingRenderParams();
        if (rootNodeDrawable == nullptr || rootNodeStagingRenderParams == nullptr ||
            canvasNodeStagingRenderParams == nullptr) {
            RS_LOGE("RSWindowKeyframeNodeInfo::PrepareRootNodeOffscreen stageRenderParams or drawable is nullptr");
            continue;
        }

        surfaceNode.SetHwcChildrenDisabledState();
        rootNode->SetNeedOffscreen(true);

        canvasNodeStagingRenderParams->SetLinkedRootNodeDrawable(rootNodeDrawable);
        canvasNodeStagingRenderParams->SetShouldPaint(true);
        canvasNodeStagingRenderParams->SetContentEmpty(false);
        canvasNode->AddToPendingSyncList();

        rootNodeStagingRenderParams->SetNeedSwapBuffer(!canvasNode->IsStaticCached());
        rootNodeStagingRenderParams->SetCacheNodeFrameRect({0, 0, canvasNode->GetRenderProperties().GetFrameWidth(),
            canvasNode->GetRenderProperties().GetFrameHeight()});
    }

    return true;
}

} // namespace OHOS::Rosen
