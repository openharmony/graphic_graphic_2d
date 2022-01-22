/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <string>

#include "command/rs_base_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_texture_node.h"

namespace OHOS {
namespace Rosen {

NodeId RSBaseNode::GenerateId()
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> currentId_ = 0;

    ++currentId_;
    if (currentId_ == UINT32_MAX) {
        // TODO:process the overflow situations
        ROSEN_LOGE("Node Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid_ << 32) | currentId_;
}

RSBaseNode::RSBaseNode(bool isRenderServiceNode) : id_(GenerateId()), isRenderServiceNode_(isRenderServiceNode) {}

RSBaseNode::~RSBaseNode()
{
    RemoveFromTree();
    RSNodeMap::Instance().UnregisterNode(id_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSBaseNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::AddChild, child is nullptr");
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSBaseNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGE("RSBaseNode::RemoveChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(0);

    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
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
        parentPtr->RemoveChildById(id_);
        OnRemoveChildren();
        SetParent(0);
    }
    // always send Remove-From-Tree command
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
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

    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
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

void RSBaseNode::DumpTree(std::string& out)
{
    out += "id: " + std::to_string(GetId()) + "\n";
    auto p = RSNodeMap::Instance().GetNode(parent_);
    if (p != nullptr) {
        out += "parent: " + std::to_string(p->GetId()) + "\n";
    } else {
        out += "parent: null\n";
    }

    for (unsigned i = 0; i < children_.size(); ++i) {
        auto c = RSNodeMap::Instance().GetNode(children_[i]);
        if (c != nullptr) {
            out += "child[" + std::to_string(i) + "]: " + std::to_string(c->GetId()) + "\n";
        } else {
            out += "child[" + std::to_string(i) + "]: null\n";
        }
    }

    for (auto child : children_) {
        auto c = RSNodeMap::Instance().GetNode(child);
        if (c != nullptr) {
            c->DumpTree(out);
        }
    }
}

template<typename T>
bool RSBaseNode::IsInstanceOf()
{
    constexpr uint32_t targetType = static_cast<uint32_t>(T::Type);
    return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
}

// explicit instantiation with all rendernode types
template bool RSBaseNode::IsInstanceOf<RSBaseNode>();
template bool RSBaseNode::IsInstanceOf<RSDisplayNode>();
template bool RSBaseNode::IsInstanceOf<RSNode>();
template bool RSBaseNode::IsInstanceOf<RSSurfaceNode>();
template bool RSBaseNode::IsInstanceOf<RSCanvasNode>();
template bool RSBaseNode::IsInstanceOf<RSRootNode>();
template bool RSBaseNode::IsInstanceOf<RSTextureNode>();

} // namespace Rosen
} // namespace OHOS
