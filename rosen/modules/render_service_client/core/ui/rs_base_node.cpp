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

namespace OHOS {
namespace Rosen {

NodeId RSBaseNode::GenerateNodeId()
{
    ++currentId_;
    // TODO:process the overflow situations
    if (currentId_ == UINT32_MAX) {
        ROSEN_LOGE("Node Id overflow");
    }
    return ((NodeId)pid_ << 32) | currentId_;
}

RSBaseNode::RSBaseNode()
{
    id_ = GenerateNodeId();
}

RSBaseNode::~RSBaseNode()
{
    RemoveFromTree();
    RSNodeMap::Instance().UnregisterNode(id_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
    RSTransactionProxy::GetInstance().AddCommand(command);
}

void RSBaseNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        return;
    }
    NodeId childId = child->GetId();
    if (child->GetParent() != 0) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    auto ptr = RSNodeMap::Instance().GetNode(id_).lock();
    child->SetParent(ptr);
    ROSEN_LOGI("RSBaseNode::AddChild %llu ---> %llu", id_, childId);
    child->OnAddChildren();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
    RSTransactionProxy::GetInstance().AddCommand(command);
}

void RSBaseNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::RemoveChild: nullptr target");
        return;
    }
    NodeId childId = child->GetId();
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr != children_.end()) {
        child->OnRemoveChildren();
        children_.erase(itr);
        child->SetParent(nullptr);
        ROSEN_LOGI("RSBaseNode::RemoveChild %llu -/-> %llu", id_, childId);

        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
        RSTransactionProxy::GetInstance().AddCommand(command);
    }
}

void RSBaseNode::RemoveFromTree()
{
    auto parentPtr = RSNodeMap::Instance().GetNode(parent_).lock();
    if (parentPtr != nullptr) {
        auto ptr = RSNodeMap::Instance().GetNode(id_).lock();
        parentPtr->RemoveChild(ptr);
    }
}

void RSBaseNode::ClearChildren()
{
    for (auto child : children_) {
        auto c = RSNodeMap::Instance().GetNode(child).lock();
        if (c != nullptr) {
            c->SetParent(nullptr);
        }
    }
    children_.clear();

    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(id_);
    RSTransactionProxy::GetInstance().AddCommand(command);
}

void RSBaseNode::SetParent(SharedPtr parent)
{
    if (parent != nullptr) {
        parent_ = parent->GetId();
    } else {
        parent_ = 0;
    }
}

RSBaseNode::SharedPtr RSBaseNode::GetParent()
{
    return RSNodeMap::Instance().GetNode(parent_).lock();
}

void RSBaseNode::DumpTree(std::string& out)
{
    out += "id: " + std::to_string(GetId()) + "\n";
    auto p = RSNodeMap::Instance().GetNode(parent_).lock();
    if (p != nullptr) {
        out += "parent: " + std::to_string(p->GetId()) + "\n";
    } else {
        out += "parent: null\n";
    }

    for (unsigned i = 0; i < children_.size(); ++i) {
        auto c = RSNodeMap::Instance().GetNode(children_[i]).lock();
        if (c != nullptr) {
            out += "child[" + std::to_string(i) + "]: " + std::to_string(c->GetId()) + "\n";
        } else {
            out += "child[" + std::to_string(i) + "]: null\n";
        }
    }

    for (auto child : children_) {
        auto c = RSNodeMap::Instance().GetNode(child).lock();
        if (c != nullptr) {
            c->DumpTree(out);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
