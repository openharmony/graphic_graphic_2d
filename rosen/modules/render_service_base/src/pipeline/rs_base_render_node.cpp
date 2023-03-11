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
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
void RSBaseRenderNode::AddChild(SharedPtr child, int index)
{
    // sanity check, avoid loop
    if (child == nullptr || child->GetId() == GetId()) {
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
    // A child is not on the tree until its parent is on the tree
    if (isOnTheTree_) {
        child->SetIsOnTheTree(true);
    }
    SetDirty();
}

void RSBaseRenderNode::MoveChild(SharedPtr child, int index)
{
    if (child == nullptr || child->GetParent().lock().get() != this) {
        return;
    }
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSBaseRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }

    // Reset parent-child relationship
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }
    children_.erase(it);
    SetDirty();
}

void RSBaseRenderNode::RemoveChild(SharedPtr child, bool skipTransition)
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
    // if child has disappearing transition, add it to disappearingChildren_
    if (skipTransition == false && child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSBaseRenderNode::RemoveChild %" PRIu64 " move child(id %" PRIu64 ") into disappearingChildren",
            GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->ResetParent();
    }
    children_.erase(it);
    SetDirty();
}

void RSBaseRenderNode::SetIsOnTheTree(bool flag)
{
    // We do not need to label a child when the child is removed from a parent that is not on the tree
    if (!flag && !isOnTheTree_) {
        return;
    }

    isOnTheTree_ = flag;
    for (auto& childWeakPtr : children_) {
        auto child = childWeakPtr.lock();
        if (child == nullptr) {
            continue;
        }
        child->SetIsOnTheTree(flag);
    }

    if (flag) {
        return;
    }

    for (auto& childPtr : disappearingChildren_) {
        auto child = childPtr.first;
        if (child == nullptr) {
            continue;
        }
        child->SetIsOnTheTree(flag);
    }
}

void RSBaseRenderNode::UpdateChildrenRect(const RectI& subRect)
{
    if (!subRect.IsEmpty()) {
        if (childrenRect_.IsEmpty()) {
            // init as not empty subRect in case join RectI enlarging area
            childrenRect_ = subRect;
        } else {
            childrenRect_ = childrenRect_.JoinRect(subRect);
        }
    }
}

void RSBaseRenderNode::AddCrossParentChild(const SharedPtr& child, int32_t index)
{
    // AddCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // so this child will not remove from the old parent.
    if (child == nullptr) {
        return;
    }

    // Set parent-child relationship
    child->SetParent(weak_from_this());
    if (index < 0 || index >= static_cast<int32_t>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }

    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // A child is not on the tree until its parent is on the tree
    if (isOnTheTree_) {
        child->SetIsOnTheTree(true);
    }
    SetDirty();
}

void RSBaseRenderNode::RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
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
    // if child has disappearing transition, add it to disappearingChildren_
    if (child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSBaseRenderNode::RemoveChild %" PRIu64 " move child(id %" PRIu64 ") into disappearingChildren",
            GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->SetParent(newParent);
        // attention: set new parent means 'old' parent has removed this child
        hasRemovedChild_ = true;
    }
    children_.erase(it);
    SetDirty();
}

void RSBaseRenderNode::RemoveFromTree(bool skipTransition)
{
    auto parentPtr = parent_.lock();
    if (parentPtr == nullptr) {
        return;
    }
    auto child = shared_from_this();
    parentPtr->RemoveChild(child, skipTransition);
    if (skipTransition == false) {
        return;
    }
    // force remove child from disappearingChildren_ and clean sortChildren_ cache
    parentPtr->disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    parentPtr->sortedChildren_.clear();
    child->ResetParent();
}

void RSBaseRenderNode::ClearChildren()
{
    if (children_.empty()) {
        return;
    }
    // Cache the parent's transition state to avoid redundant recursively check
    bool parentHasDisappearingTransition = HasDisappearingTransition(true);
    uint32_t pos = 0;
    for (auto& childWeakPtr : children_) {
        auto child = childWeakPtr.lock();
        if (child == nullptr) {
            ++pos;
            continue;
        }
        // avoid duplicate entry in disappearingChildren_ (this should not happen)
        disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
        if (parentHasDisappearingTransition || child->HasDisappearingTransition(false)) {
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
    auto parentNode = parent_.lock();
    if (parentNode) {
        parentNode->hasRemovedChild_ = true;
    }
    parent_.reset();
    SetIsOnTheTree(false);
}

RSBaseRenderNode::WeakPtr RSBaseRenderNode::GetParent() const
{
    return parent_;
}

void RSBaseRenderNode::DumpTree(int32_t depth, std::string& out) const
{
    std::string space = "  ";
    for (int32_t i = 0; i < depth; ++i) {
        out += space;
    }
    out += "| ";
    DumpNodeType(out);
    out += "[" + std::to_string(GetId()) + "]";
    out += ", isOnTheTree: " + std::to_string(isOnTheTree_);
    if (GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = (static_cast<const RSSurfaceRenderNode*>(this));
        const RSSurfaceHandler& surfaceHandler = static_cast<const RSSurfaceHandler&>(*surfaceNode);
        out += ", hasConsumer: " + std::to_string(surfaceHandler.HasConsumer());
        out += ", Name [" + surfaceNode->GetName() + "]";
        auto p = parent_.lock();
        out += ", parent [" + (p != nullptr ? std::to_string(p->GetId()) : "null") + "]";
        out = out + ", " + surfaceNode->GetVisibleRegion().GetRegionInfo();
        out += ", SurfaceBgAlpha[ " + std::to_string(surfaceNode->GetAbilityBgAlpha()) + " ]";
    }
    out += ", children[";
    for (auto child : children_) {
        auto c = child.lock();
        if (c != nullptr) {
            out += std::to_string(c->GetId()) + " ";
        } else {
            out += ", null";
        }
    }
    if (!disappearingChildren_.empty()) {
        out += "], disappearing children[";
        int i = 0;
        for (auto& disappearingChild : disappearingChildren_) {
            out += "(" + std::to_string(i) + ": id:" + std::to_string(disappearingChild.first->GetId()) +
                   ", Transition:" + std::to_string(disappearingChild.first->HasDisappearingTransition(false)) + "),";
            ++i;
        }
    }
    out += "]\n";

    for (auto child : children_) {
        if (auto c = child.lock()) {
            c->DumpTree(depth + 1, out);
        }
    }
    for (auto& child : disappearingChildren_) {
        if (auto c = child.first) {
            c->DumpTree(depth + 1, out);
        }
    }
}

void RSBaseRenderNode::DumpNodeType(std::string& out) const
{
    switch (GetType()) {
        case RSRenderNodeType::BASE_NODE: {
            out += "BASE_NODE";
            break;
        }
        case RSRenderNodeType::DISPLAY_NODE: {
            out += "DISPLAY_NODE";
            break;
        }
        case RSRenderNodeType::RS_NODE: {
            out += "RS_NODE";
            break;
        }
        case RSRenderNodeType::SURFACE_NODE: {
            out += "SURFACE_NODE";
            break;
        }
        case RSRenderNodeType::CANVAS_NODE: {
            out += "CANVAS_NODE";
            break;
        }
        case RSRenderNodeType::ROOT_NODE: {
            out += "ROOT_NODE";
            break;
        }
        case RSRenderNodeType::PROXY_NODE: {
            out += "PROXY_NODE";
            break;
        }
        default: {
            out += "UNKNOWN_NODE";
            break;
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

void RSBaseRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender)
{
    for (auto& child : node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender);
    }
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
    bool parentHasDisappearingTransition = disappearingChildren_.empty() ? false : HasDisappearingTransition(true);
    disappearingChildren_.remove_if([this, parentHasDisappearingTransition](const auto& pair) -> bool {
        auto& disappearingChild = pair.first;
        const auto& origPos = pair.second;
        // if neither parent node or child node has transition, we can safely remove it
        if (!parentHasDisappearingTransition && !disappearingChild->HasDisappearingTransition(false)) {
            ROSEN_LOGD("RSBaseRenderNode::GenerateSortedChildren removing finished transition child(id %" PRIu64 ")",
                disappearingChild->GetId());
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

void RSBaseRenderNode::SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command, nodeId);
    }
}

void RSBaseRenderNode::InternalRemoveSelfFromDisappearingChildren()
{
    // internal use only, force remove self from parent's disappearingChildren_
    if (auto parent = parent_.lock()) {
        parent->disappearingChildren_.remove_if(
            [childPtr = shared_from_this()](const auto& pair) -> bool {
                if (pair.first == childPtr) {
                    childPtr ->ResetParent(); // when child been removed, notify dirty by ResetParent()
                    return true;
                } else {
                    return false;
                }
            }
        );
    }
}
} // namespace Rosen
} // namespace OHOS
