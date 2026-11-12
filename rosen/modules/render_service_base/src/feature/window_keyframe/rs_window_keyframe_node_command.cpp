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

#include "feature/window_keyframe/rs_window_keyframe_node_command.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "pipeline/rs_render_node_gc.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void RSWindowKeyFrameNodeCommandHelper::Create(
    RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = RSWindowKeyFrameRenderNode::SharedPtr(new (std::nothrow) RSWindowKeyFrameRenderNode(id,
        context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    if (node == nullptr) {
        RS_LOGE("RSWindowKeyFrameNodeCommand Alloc render node object failed");
        return;
    }

    context.GetMutableNodeMap().RegisterRenderNode(node);
    RS_LOGI("RSWindowKeyFrameNodeCommand node[%{public}" PRIu64 "] created", id);
}

void RSWindowKeyFrameNodeCommandHelper::LinkNode(RSContext& context, NodeId nodeId, NodeId linkNodeId)
{
    auto node = context.GetNodeMap().GetRenderNode<RSWindowKeyFrameRenderNode>(nodeId);
    if (node != nullptr) {
        node->SetLinkedNodeId(linkNodeId);
        RS_LOGI("RSWindowKeyFrameNodeCommand node[%{public}" PRIu64 "] link to %{public}" PRIu64 "",
            nodeId, linkNodeId);
    } else {
        RS_LOGE("RSWindowKeyFrameNodeCommand can not find node[%{public}" PRIu64 "] for linkNode", nodeId);
    }
}

} // namespace Rosen
} // namespace OHOS
