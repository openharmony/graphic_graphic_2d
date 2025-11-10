/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "rs_window_keyframe_node.h"

#include <algorithm>
#include <string>

#include "command/rs_node_command.h"
#include "common/rs_optional_trace.h"
#include "feature/window_keyframe/rs_window_keyframe_node_command.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {

RSWindowKeyFrameNode::SharedPtr RSWindowKeyFrameNode::Create(bool isRenderServiceNode,
    bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
{
    if (!RSSystemProperties::GetWindowKeyFrameEnabled()) {
        RS_LOGW("RSWindowKeyFrameNode::Create WindowKeyFrame feature disabled");
        return nullptr;
    }

    if (!RSSystemProperties::GetUniRenderEnabled()) {
        RS_LOGW("RSWindowKeyFrameNode::Create WindowKeyFrame is not supported in separate render");
        return nullptr;
    }

    SharedPtr node(new (std::nothrow) RSWindowKeyFrameNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (node == nullptr) {
        RS_LOGE("RSWindowKeyFrameNode::Create Alloc object failed");
        return nullptr;
    }

    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    RS_LOGI("RSWindowKeyFrameNode created, id: %{public}" PRIu64 "", node->GetId());

    std::unique_ptr<RSCommand> command = std::make_unique<RSWindowKeyFrameNodeCreate>(
        node->GetId(), isTextureExportNode);
    node->AddCommand(command, node->IsRenderServiceNode());
    node->SetUIContextToken();
    return node;
}

RSWindowKeyFrameNode::RSWindowKeyFrameNode(bool isRenderServiceNode,
    bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
{
}

RSWindowKeyFrameNode::RSWindowKeyFrameNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode,
    std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, id, isTextureExportNode, rsUIContext)
{
}

RSWindowKeyFrameNode::~RSWindowKeyFrameNode()
{
}

void RSWindowKeyFrameNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

void RSWindowKeyFrameNode::SetFreeze(bool isFreeze)
{
    ROSEN_LOGI("RSWindowKeyFrameNode::SetFreeze NodeId=%{public}" PRIu64 " freeze=%{public}d", GetId(), isFreeze);
    RS_OPTIONAL_TRACE_NAME_FMT("RSWindowKeyFrameNode::SetFreeze id:%llu freeze:%d", GetId(), isFreeze);
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    AddCommand(command, true);
}

void RSWindowKeyFrameNode::SetLinkedNodeId(NodeId nodeId)
{
    ROSEN_LOGI("RSWindowKeyFrameNode::SetLinkedNodeId nodeId: %{public}" PRIu64
        ", linkedNode: %{public}" PRIu64 "", GetId(), nodeId);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSWindowKeyFrameNodeLinkNode>(GetId(), nodeId);
    AddCommand(command, true);
    linkedNodeId_ = nodeId;
}

NodeId RSWindowKeyFrameNode::GetLinkedNodeId() const
{
    return linkedNodeId_;
}

bool RSWindowKeyFrameNode::WriteToParcel(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) &&
        parcel.WriteBool(IsRenderServiceNode()) &&
        parcel.WriteUint64(linkedNodeId_);
}

RSWindowKeyFrameNode::SharedPtr RSWindowKeyFrameNode::ReadFromParcel(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    NodeId linkedNodeId = INVALID_NODEID;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadBool(isRenderServiceNode) && parcel.ReadUint64(linkedNodeId))) {
        ROSEN_LOGE("RSWindowKeyFrameNode::ReadFromParcel, read param failed");
        return nullptr;
    }

    auto prevNode = RSNodeMap::Instance().GetNode(id);
    if (prevNode != nullptr) {
        ROSEN_LOGW("RSWindowKeyFrameNode::ReadFromParcel, the node id is already in the map");
        // if the node id is already in the map, we should not create a new node
        return prevNode->ReinterpretCastTo<RSWindowKeyFrameNode>();
    }

    SharedPtr keyFrameNode(new (std::nothrow) RSWindowKeyFrameNode(isRenderServiceNode, id));
    if (keyFrameNode == nullptr) {
        ROSEN_LOGE("RSWindowKeyFrameNode::ReadFromParcel alloc object failed");
        return nullptr;
    }
    
    RSNodeMap::MutableInstance().RegisterNode(keyFrameNode);

    // for nodes constructed by ReadFromParcel, we should not destroy the corresponding render node on destruction
    keyFrameNode->skipDestroyCommandInDestructor_ = true;
    keyFrameNode->linkedNodeId_ = linkedNodeId;

    return keyFrameNode;
}

} // namespace Rosen
} // namespace OHOS
