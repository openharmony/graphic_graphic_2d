/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui/rs_base_node.h"

#include <algorithm>
#include <sstream>

#include "sandbox_utils.h"

#include "command/rs_base_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
static bool gIsUniRenderEnabled = false;
}

NodeId RSBaseNode::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Node Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid_ << 32) | currentId;
}

void RSBaseNode::InitUniRenderEnabled()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        gIsUniRenderEnabled = RSSystemProperties::GetUniRenderEnabled();
        ROSEN_LOGD("RSBaseNode::InitUniRenderEnabled:%d", gIsUniRenderEnabled);
    }
}

RSBaseNode::RSBaseNode(bool isRenderServiceNode) : RSBaseNode(isRenderServiceNode, GenerateId()) {}

RSBaseNode::RSBaseNode(bool isRenderServiceNode, NodeId id) : isRenderServiceNode_(isRenderServiceNode), id_(id)
{
    InitUniRenderEnabled();
}

RSBaseNode::~RSBaseNode()
{
    // break current (ui) parent-child relationship.
    // render nodes will check if its child is expired and remove it, no need to manually remove it here.
    if (auto parentPtr = RSNodeMap::Instance().GetNode(parent_)) {
        parentPtr->RemoveChildById(id_);
    }
    // unregister node from node map
    RSNodeMap::MutableInstance().UnregisterNode(id_);

    // tell RT/RS to destroy related render node
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr || skipDestroyCommandInDestructor_) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeDestroy>(id_);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode());
    }
}

bool RSBaseNode::IsUniRenderEnabled() const
{
    return gIsUniRenderEnabled;
}

bool RSBaseNode::IsRenderServiceNode() const
{
    return gIsUniRenderEnabled || isRenderServiceNode_;
}

bool RSBaseNode::NeedSendExtraCommand() const
{
    return gIsUniRenderEnabled && !isRenderServiceNode_;
}

void RSBaseNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::AddChild, child is nullptr");
        return;
    }
    if (child->parent_ == id_) {
        ROSEN_LOGI("RSBaseNode::AddChild, child already exist");
        return;
    }
    NodeId childId = child->GetId();
    if (child->parent_ != 0) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    child->OnAddChildren();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
    bool disallowSendToRemote = gIsUniRenderEnabled && !RSSystemProperties::IsUniRenderMode() && // dynamic-Non Uni
        !isRenderServiceNode_ && !IsInstanceOf(RSUINodeType::SURFACE_NODE) && // canvas/root node
        child->IsInstanceOf(RSUINodeType::SURFACE_NODE);
    if (disallowSendToRemote) {
        transactionProxy->AddCommand(command, false, GetFollowType(), id_);
        return;
    }
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::MoveChild(SharedPtr child, int index)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGD("RSBaseNode::MoveChild, not valid child");
        return;
    }
    NodeId childId = child->GetId();
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr == children_.end()) {
        ROSEN_LOGD("RSBaseNode::MoveChild, not child");
        return;
    }
    children_.erase(itr);
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeMoveChild>(id_, childId, index);
    bool disallowSendToRemote = gIsUniRenderEnabled && !RSSystemProperties::IsUniRenderMode() && // dynamic-Non Uni
                                !isRenderServiceNode_ &&
                                !IsInstanceOf(RSUINodeType::SURFACE_NODE) && // canvas/root node
                                child->IsInstanceOf(RSUINodeType::SURFACE_NODE);
    if (disallowSendToRemote) {
        transactionProxy->AddCommand(command, false, GetFollowType(), id_);
        return;
    }
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeMoveChild>(id_, childId, index);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGI("RSBaseNode::RemoveChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(0);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::AddCrossParentChild(SharedPtr child, int index)
{
    // AddCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // so this child will not remove from the old parent.
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::AddCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSBaseNode::AddCrossScreenChild, only displayNode support AddCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    child->OnAddChildren();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossParentChild>(id_, childId, index);
    bool disallowSendToRemote = gIsUniRenderEnabled && !RSSystemProperties::IsUniRenderMode() && // dynamic-Non Uni
                                !isRenderServiceNode_ &&
                                !IsInstanceOf(RSUINodeType::SURFACE_NODE) && // canvas/root node
                                child->IsInstanceOf(RSUINodeType::SURFACE_NODE);
    if (disallowSendToRemote) {
        transactionProxy->AddCommand(command, false, GetFollowType(), id_);
        return;
    }
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeAddCrossParentChild>(id_, childId, index);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::RemoveCrossParentChild(SharedPtr child, NodeId newParentId)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
    if (child == nullptr) {
        ROSEN_LOGI("RSBaseNode::RemoveCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSBaseNode::RemoveCrossScreenChild, only displayNode support RemoveCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(newParentId);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveCrossParentChild>(id_, childId, newParentId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand =
            std::make_unique<RSBaseNodeRemoveCrossParentChild>(id_, childId, newParentId);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::RemoveChildById(NodeId childId)
{
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr != children_.end()) {
        children_.erase(itr);
    }
}

void RSBaseNode::RemoveFromTree()
{
    if (auto parentPtr = RSNodeMap::Instance().GetNode(parent_)) {
        parentPtr->RemoveChildById(GetId());
        OnRemoveChildren();
        SetParent(0);
    }
    // always send Remove-From-Tree command
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(nodeId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeRemoveFromTree>(nodeId);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), nodeId);
    }
}

void RSBaseNode::ClearChildren()
{
    for (auto child : children_) {
        if (auto childPtr = RSNodeMap::Instance().GetNode(child)) {
            childPtr->SetParent(0);
        }
    }
    children_.clear();

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(nodeId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeClearChild>(nodeId);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), nodeId);
    }
}

void RSBaseNode::SetParent(NodeId parentId)
{
    parent_ = parentId;
}

RSBaseNode::SharedPtr RSBaseNode::GetParent()
{
    return RSNodeMap::Instance().GetNode(parent_);
}

std::string RSBaseNode::DumpNode(int depth) const
{
    std::stringstream ss;
    auto it = RSUINodeTypeStrs.find(GetType());
    if (it == RSUINodeTypeStrs.end()) {
        return "";
    }
    ss << it->second << "[" << std::to_string(id_) << "] child[";
    for (auto child : children_) {
        ss << std::to_string(child) << " ";
    }
    ss << "]";
    return ss.str();
}

bool RSBaseNode::IsInstanceOf(RSUINodeType type) const
{
    auto targetType = static_cast<uint32_t>(type);
    auto instanceType = static_cast<uint32_t>(GetType());
    // use bitmask to check whether the instance is a subclass of the target type
    return (instanceType & targetType) == targetType;
}

template<typename T>
bool RSBaseNode::IsInstanceOf() const
{
    return IsInstanceOf(T::Type);
}

// explicit instantiation with all render node types
template bool RSBaseNode::IsInstanceOf<RSBaseNode>() const;
template bool RSBaseNode::IsInstanceOf<RSDisplayNode>() const;
template bool RSBaseNode::IsInstanceOf<RSNode>() const;
template bool RSBaseNode::IsInstanceOf<RSSurfaceNode>() const;
template bool RSBaseNode::IsInstanceOf<RSProxyNode>() const;
template bool RSBaseNode::IsInstanceOf<RSCanvasNode>() const;
template bool RSBaseNode::IsInstanceOf<RSRootNode>() const;

} // namespace Rosen
} // namespace OHOS
