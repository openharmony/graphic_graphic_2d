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

#include "pipeline/rs_base_render_node.h"

#include <algorithm>

#include "pipeline/rs_context.h"
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
    // if child already has a parent, remove it from its previous parent
    if (auto prevParent = child->GetParent().lock()) {
        prevParent->RemoveChild(child);
    }

    // Set parent-child relationship
    child->SetParent(weak_from_this());
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }

    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
}

void RSBaseRenderNode::RemoveChild(const SharedPtr& child)
{
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSBaseRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }
    // avoid duplicate entry in disappearingChildren_ (this should not happen)
    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    if (child->HasTransition(true)) {
        // keep shared_ptr alive for transition
        uint32_t origPos = std::distance(children_.begin(), it);
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->ResetParent();
    }
    children_.erase(it);
    SetDirty();
}

void RSBaseRenderNode::RemoveFromTree()
{
    if (auto parentPtr = parent_.lock()) {
        parentPtr->RemoveChild(shared_from_this());
    }
}

void RSBaseRenderNode::ClearChildren()
{
    if (children_.empty()) {
        return;
    }
    // Cache the parent's transition state to avoid redundant recursively check
    bool parentHasTransition = HasTransition(true);
    uint32_t pos = 0;
    for (auto& childWeakPtr : children_) {
        auto child = childWeakPtr.lock();
        if (child == nullptr) {
            ++pos;
            continue;
        }
        // avoid duplicate entry in disappearingChildren_ (this should not happen)
        disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
        if (parentHasTransition || child->HasTransition(false)) {
            // keep shared_ptr alive for transition
            disappearingChildren_.emplace_back(child, pos);
        } else {
            child->ResetParent();
        }
        ++pos;
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

    uint32_t i = 0;
    for (auto child : children_) {
        auto c = child.lock();
        if (c != nullptr) {
            out += "child[" + std::to_string(i) + "]: " + std::to_string(c->GetId()) + "\n";
        } else {
            out += "child[" + std::to_string(i) + "]: null\n";
        }
        ++i;
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

const std::list<RSBaseRenderNode::SharedPtr>& RSBaseRenderNode::GetSortedChildren()
{
    // generate sorted children list if it's empty
    if (sortedChildren_.empty() && (!children_.empty() || !disappearingChildren_.empty())) {
        GenerateSortedChildren();
    }
    return sortedChildren_;
}

void RSBaseRenderNode::GenerateSortedChildren()
{
    sortedChildren_.clear();

    // Step 1: copy all existing children to sortedChildren (skip and clean expired children)
    children_.remove_if([this](const auto& child) -> bool {
        auto existingChild = child.lock();
        if (existingChild == nullptr) {
            ROSEN_LOGI("RSBaseRenderNode::GenerateSortedChildren removing expired child");
            return true;
        }
        sortedChildren_.emplace_back(std::move(existingChild));
        return false;
    });

    // Step 2: insert disappearing children into sortedChildren, at it's original position, remove if it's transition
    // finished
    // If exist disappearing Children, cache the parent's transition state to avoid redundant recursively check
    bool parentHasTransition = disappearingChildren_.empty() ? false : HasTransition(true);
    disappearingChildren_.remove_if([this, parentHasTransition](const auto& pair) -> bool {
        auto& disappearingChild = pair.first;
        auto& origPos = pair.second;
        // if neither parent node or child node has transition, we can safely remove it
        if (!parentHasTransition && !disappearingChild->HasTransition(false)) {
            ROSEN_LOGD("RSBaseRenderNode::GenerateSortedChildren removing finished transition child");
            if (ROSEN_EQ<RSBaseRenderNode>(disappearingChild->GetParent(), weak_from_this())) {
                disappearingChild->ResetParent();
            }
            return true;
        }
        if (origPos < sortedChildren_.size()) {
            sortedChildren_.emplace(std::next(sortedChildren_.begin(), origPos), disappearingChild);
        } else {
            sortedChildren_.emplace_back(disappearingChild);
        }
        return false;
    });

    // Step 3: sort all children by z-order (std::list::sort is stable)
    sortedChildren_.sort([](const auto& first, const auto& second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(first);
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(second);
        if (node1 == nullptr || node2 == nullptr) {
            return false;
        }
        return node1->GetRenderProperties().GetPositionZ() < node2->GetRenderProperties().GetPositionZ();
    });
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
