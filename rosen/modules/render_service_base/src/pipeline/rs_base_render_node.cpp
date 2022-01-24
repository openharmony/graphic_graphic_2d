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
void RSBaseRenderNode::AddChild(const SharedPtr& child, int index)
{
    if (child == nullptr) {
        return;
    }
    // if child already has a parent, remove it from its parent
    if (auto prevParent = child->GetParent().lock()) {
        prevParent->RemoveChild(child);
    }

    // Set parent-child relationship
    child->SetParent(weak_from_this());
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }

    OnAddChild(child);
}

void RSBaseRenderNode::RemoveChild(const SharedPtr& child)
{
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto itr = std::find_if(children_.begin(), children_.end(), [&](WeakPtr& ptr) {
        return ROSEN_EQ<RSBaseRenderNode>(ptr, child);
    });
    if (itr == children_.end()) {
        return;
    }
    children_.erase(itr);
    SetDirty();

    OnRemoveChild(child);
}

void RSBaseRenderNode::OnAddChild(const SharedPtr& child)
{
    RemoveDisappearingChild(child);
}

void RSBaseRenderNode::OnRemoveChild(const SharedPtr& child)
{
    if (child->HasTransition()) {
        AddDisappearingChild(child);
    } else {
        child->ResetParent();
    }
}

void RSBaseRenderNode::RemoveFromTree()
{
    if (auto parentPtr = parent_.lock()) {
        parentPtr->RemoveChild(shared_from_this());
    }
}

void RSBaseRenderNode::ClearChildren()
{
    for (auto& child : children_) {
        if (auto c = child.lock()) {
            OnRemoveChild(c);
            c->ResetParent();
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

RSBaseRenderNode::WeakPtr RSBaseRenderNode::GetParent() const
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

void RSBaseRenderNode::AddDisappearingChild(const SharedPtr& child)
{
    disappearingChildren_.emplace_back(child);
}

void RSBaseRenderNode::RemoveDisappearingChild(const SharedPtr& child)
{
    disappearingChildren_.remove(child);
}

bool RSBaseRenderNode::Animate(int64_t timestamp)
{
    bool hasRunningAnimation = false;
    // process disappearing children
    for (auto& node : disappearingChildren_) {
        hasRunningAnimation = node->Animate(timestamp) || hasRunningAnimation;
    }
    return hasRunningAnimation;
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
template bool RSBaseRenderNode::IsInstanceOf<RSRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSSurfaceRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSCanvasRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSRootRenderNode>();
template bool RSBaseRenderNode::IsInstanceOf<RSTextureRenderNode>();

} // namespace Rosen
} // namespace OHOS
