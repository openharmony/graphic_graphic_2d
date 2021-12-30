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

#include "pipeline/rs_base_render_node.h"

#include <algorithm>

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_texture_render_node.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSBaseRenderNode::RSBaseRenderNode(NodeId id) : id_(id) {}

RSBaseRenderNode::~RSBaseRenderNode()
{
}

void RSBaseRenderNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        return;
    }
    if (child->GetParent().lock()) {
        child->RemoveFromTree();
    }
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }
    OnAddChild(child);
    child->SetParent(shared_from_this());
}

void RSBaseRenderNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr) {
        return;
    }
    auto itr = std::find_if(children_.begin(), children_.end(), [&](WeakPtr& ptr) {
        auto existingChild = ptr.lock();
        return existingChild && child->GetId() == existingChild->GetId();
    });
    SetDirty();
    if (itr == children_.end()) {
        return;
    }
    children_.erase(itr);
    OnRemoveChild(child);
}

void RSBaseRenderNode::RemoveFromTree()
{
    auto parentPtr = parent_.lock();
    auto thisPtr = shared_from_this();
    if (parentPtr != nullptr) {
        parentPtr->RemoveChild(thisPtr);
    }
}

void RSBaseRenderNode::ClearChildren()
{
    for (auto child : children_) {
        if (auto c = child.lock()) {
            OnRemoveChild(c);
        }
    }
    children_.clear();
    SetDirty();
}

void RSBaseRenderNode::SetParent(WeakPtr parent)
{
    parent_ = parent;
}

void RSBaseRenderNode::ResetParent()
{
    parent_.reset();
}

RSBaseRenderNode::WeakPtr RSBaseRenderNode::GetParent()
{
    return parent_;
}

void RSBaseRenderNode::DumpTree(std::string& out) const
{
    out += "id: " + std::to_string(GetId()) + "\n";
    auto p = parent_.lock();
    if (p != nullptr) {
        out += "parent: " + std::to_string(p->GetId()) + "\n";
    } else {
        out += "parent: null\n";
    }

    for (unsigned i = 0; i < children_.size(); ++i) {
        auto c = children_[i].lock();
        if (c != nullptr) {
            out += "child[" + std::to_string(i) + "]: " + std::to_string(c->GetId()) + "\n";
        } else {
            out += "child[" + std::to_string(i) + "]: null\n";
        }
    }

    for (auto child : children_) {
        if (auto c = child.lock()) {
            c->DumpTree(out);
        }
    }
}

bool RSBaseRenderNode::IsDirty() const
{
    return dirtyStatus_ == NodeDirty::DIRTY;
}

void RSBaseRenderNode::SetDirty()
{
    dirtyStatus_ = NodeDirty::DIRTY;
}

void RSBaseRenderNode::SetClean()
{
    dirtyStatus_ = NodeDirty::CLEAN;
}

void RSBaseRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareBaseRenderNode(*this);
}

void RSBaseRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessBaseRenderNode(*this);
}

template<typename T>
bool RSBaseRenderNode::IsInstanceOf()
{
    constexpr uint32_t targetType = static_cast<uint32_t>(T::Type);
    return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
}

// explicit instantiation with all rendernode types
template bool RSBaseRenderNode::IsInstanceOf<RSBaseRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSDisplayRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSPropertyRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSSurfaceRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSRootRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSTextureRenderNode>();

} // namespace Rosen
} // namespace OHOS
