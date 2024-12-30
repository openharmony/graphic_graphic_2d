/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <utility>

#include "offscreen_render/rs_offscreen_render_thread.h"
#include "rs_profiler.h"
#include "rs_trace.h"

#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "modifier/rs_modifier_type.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_render_filter.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"

#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSurface.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS {
namespace Rosen {

std::unordered_map<pid_t, size_t> RSRenderNode::blurEffectCounter_ = {};
void RSRenderNode::UpdateBlurEffectCounter(int deltaCount)
{
    if (LIKELY(deltaCount == 0)) {
        return;
    }

    auto pid = ExtractPid(GetId());
    // Try to insert pid with value 0 and we got an iterator to the inserted element or to the existing element.
    auto it = blurEffectCounter_.emplace(std::make_pair(pid, 0)).first;
    if (deltaCount > 0 || (it->second > -deltaCount)) {
        it->second += deltaCount;
    } else {
        blurEffectCounter_.erase(it);
    }
}

void RSRenderNode::OnRegister(const std::weak_ptr<RSContext>& context)
{
    context_ = context;
    renderProperties_.backref_ = weak_from_this();
    SetDirty(true);
    InitRenderParams();
}

bool RSRenderNode::IsPureContainer() const
{
    return (drawCmdModifiers_.empty() && !GetRenderProperties().isDrawn_ && !GetRenderProperties().alphaNeedApply_);
}

bool RSRenderNode::IsPureBackgroundColor() const
{
    static const std::unordered_set<RSDrawableSlot> pureBackgroundColorSlots = {
        RSDrawableSlot::BG_SAVE_BOUNDS,
        RSDrawableSlot::CLIP_TO_BOUNDS,
        RSDrawableSlot::BACKGROUND_COLOR,
        RSDrawableSlot::BG_RESTORE_BOUNDS,
        RSDrawableSlot::SAVE_FRAME,
        RSDrawableSlot::FRAME_OFFSET,
        RSDrawableSlot::CLIP_TO_FRAME,
        RSDrawableSlot::CHILDREN,
        RSDrawableSlot::RESTORE_FRAME
    };
    for (int8_t i = 0; i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        if (drawableVec_[i] &&
            !pureBackgroundColorSlots.count(static_cast<RSDrawableSlot>(i))) {
            return false;
        }
    }
    return true;
}

void RSRenderNode::SetDrawNodeType(DrawNodeType nodeType)
{
    drawNodeType_ = nodeType;
}

DrawNodeType RSRenderNode::GetDrawNodeType() const
{
    return drawNodeType_;
}

std::string DrawNodeTypeToString(DrawNodeType nodeType)
{
    // Corresponding to DrawNodeType
    const std::string typeMap[] = {
        "PureContainerType",
        "MergeableType",
        "DrawPropertyType",
        "GeometryPropertyType"
    };
    return typeMap[nodeType];
}

bool RSRenderNode::IsContentNode() const
{
    return ((drawCmdModifiers_.size() == 1 &&
        (drawCmdModifiers_.find(RSModifierType::CONTENT_STYLE) != drawCmdModifiers_.end())) ||
        drawCmdModifiers_.empty()) &&
        !GetRenderProperties().isDrawn_;
}

namespace {
const std::set<RSModifierType> GROUPABLE_ANIMATION_TYPE = {
    RSModifierType::ALPHA,
    RSModifierType::ROTATION,
    RSModifierType::SCALE,
};
const std::set<RSModifierType> CACHEABLE_ANIMATION_TYPE = {
    RSModifierType::BOUNDS,
    RSModifierType::FRAME,
};
const std::set<RSModifierType> BASIC_GEOTRANSFORM_ANIMATION_TYPE = {
    RSModifierType::TRANSLATE,
    RSModifierType::SCALE,
    RSModifierType::ALPHA,
};
}

static inline bool IsPurgeAble()
{
    return RSSystemProperties::GetRenderNodePurgeEnabled() && RSUniRenderJudgement::IsUniRender();
}

RSRenderNode::RSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : isTextureExportNode_(isTextureExportNode), isPurgeable_(IsPurgeAble()), id_(id), context_(context)
{}

RSRenderNode::RSRenderNode(
    NodeId id, bool isOnTheTree, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : isOnTheTree_(isOnTheTree), isTextureExportNode_(isTextureExportNode), isPurgeable_(IsPurgeAble()),
      id_(id), context_(context)
{}

void RSRenderNode::AddUIExtensionChild(SharedPtr child)
{
    auto realParent = shared_from_this();
    while (realParent) {
        auto surfaceNode = realParent->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode && surfaceNode->IsAppWindow()) {
            break;
        }
        realParent = realParent->GetParent().lock();
    }
    if (!realParent) {
        return;
    }
    realParent->AddChild(child, -1);
    RS_LOGI("RSRenderNode::AddUIExtensionChild parent:%{public}" PRIu64 ",child:%{public}" PRIu64 ".",
        realParent->GetId(), child->GetId());
    AddToPendingSyncList();
}

// when child is UnobscuredUIExtension and parent is not main window, Mark Need, Rout to main window.
bool RSRenderNode::NeedRoutedBasedOnUIExtension(SharedPtr child)
{
    if (!child) {
        return false;
    }
    auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
    bool isUnobscuredUIExtension = surfaceNode && surfaceNode->IsUnobscuredUIExtensionNode();
    auto parent = ReinterpretCastTo<RSSurfaceRenderNode>();
    return isUnobscuredUIExtension && !(parent && parent->IsMainWindowType());
}

void RSRenderNode::AddChild(SharedPtr child, int index)
{
    if (NeedRoutedBasedOnUIExtension(child)) {
        stagingUECChildren_->insert(child);
        unobscuredUECChildrenNeedSync_ = true;
        return AddUIExtensionChild(child);
    }
    // sanity check, avoid loop
    if (child == nullptr || child->GetId() == GetId()) {
        return;
    }

    if (RS_PROFILER_PROCESS_ADD_CHILD(this, child, index)) {
        RS_LOGI("Add child: blocked during replay");
        return;
    }

    // if child already has a parent, remove it from its previous parent
    if (auto prevParent = child->GetParent().lock()) {
        prevParent->RemoveChild(child, true);
        child->InternalRemoveSelfFromDisappearingChildren();
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
        child->SetIsOnTheTree(true, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_,
            uifirstRootNodeId_, displayNodeId_);
    } else {
        if (child->GetType() == RSRenderNodeType::SURFACE_NODE) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            ROSEN_LOGI("RSRenderNode:: add child surfaceNode[id:%{public}" PRIu64 " name:%{public}s]"
            " parent'S isOnTheTree_:%{public}d", surfaceNode->GetId(), surfaceNode->GetNodeName().c_str(),
            isOnTheTree_);
        }
    }
    (RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && child->GetType() == RSRenderNodeType::SURFACE_NODE) ?
        SetParentSubTreeDirty() : SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::SetContainBootAnimation(bool isContainBootAnimation)
{
    isContainBootAnimation_ = isContainBootAnimation;
    isFullChildrenListValid_ = false;
}

void RSRenderNode::MoveUIExtensionChild(SharedPtr child)
{
    if (!child) {
        return;
    }
    auto parent = child->GetParent().lock();
    if (!parent) {
        return;
    }
    parent->MoveChild(child, -1);
}

void RSRenderNode::MoveChild(SharedPtr child, int index)
{
    if (NeedRoutedBasedOnUIExtension(child)) {
        return MoveUIExtensionChild(child);
    }
    if (child == nullptr || child->GetParent().lock().get() != this) {
        return;
    }
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
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
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::RemoveUIExtensionChild(SharedPtr child)
{
    if (!child) {
        return;
    }
    auto parent = child->GetParent().lock();
    if (!parent) {
        return;
    }
    parent->RemoveChild(child);
    RS_LOGI("RSRenderNode::RemoveUIExtensionChild parent:%{public}" PRIu64 ",child:%{public}" PRIu64 ".",
        parent->GetId(), child->GetId());
    AddToPendingSyncList();
}

void RSRenderNode::RemoveChild(SharedPtr child, bool skipTransition)
{
    if (NeedRoutedBasedOnUIExtension(child)) {
        stagingUECChildren_->erase(child);
        unobscuredUECChildrenNeedSync_ = true;
        return RemoveUIExtensionChild(child);
    }
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }
    // avoid duplicate entry in disappearingChildren_ (this should not happen)
    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // if child has disappearing transition, add it to disappearingChildren_
    if (skipTransition == false && child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSRenderNode::RemoveChild %{public}" PRIu64 " move child(id %{public}" PRIu64 ") into"
            " disappearingChildren", GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->ResetParent();
    }
    children_.erase(it);
    if (child->GetBootAnimation()) {
        SetContainBootAnimation(false);
    }
    (RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && child->GetType() == RSRenderNodeType::SURFACE_NODE) ?
        SetParentSubTreeDirty() : SetContentDirty();
    isFullChildrenListValid_ = false;
}

bool RSRenderNode::HasUnobscuredUEC() const
{
    return stagingRenderParams_->HasUnobscuredUEC();
}

void RSRenderNode::SetHasUnobscuredUEC()
{
    bool hasUnobscuredUEC = stagingUECChildren_ && !stagingUECChildren_->empty();
    if (hasUnobscuredUEC) {
        return stagingRenderParams_->SetHasUnobscuredUEC(hasUnobscuredUEC);
    }
    for (auto childWeakPtr : children_) {
        if (auto child = childWeakPtr.lock()) {
            hasUnobscuredUEC |= child->HasUnobscuredUEC();
            if (hasUnobscuredUEC) {
                break;
            }
        }
    }
    stagingRenderParams_->SetHasUnobscuredUEC(hasUnobscuredUEC);
}

void RSRenderNode::SetHdrNum(bool flag, NodeId instanceRootNodeId, HDRComponentType hdrType)
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSRenderNode::SetHdrNum: Invalid context");
        return;
    }
    auto parentInstance = context->GetNodeMap().GetRenderNode(instanceRootNodeId);
    if (!parentInstance) {
        ROSEN_LOGE("RSRenderNode::SetHdrNum get parent instance root node info failed.");
        return;
    }
    if (auto parentSurface = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        if (flag) {
            parentSurface->IncreaseHDRNum(hdrType);
        } else {
            parentSurface->ReduceHDRNum(hdrType);
        }
    }
}

void RSRenderNode::SetEnableHdrEffect(bool enableHdrEffect)
{
    if (enableHdrEffect_ == enableHdrEffect) {
        return;
    }
    if (IsOnTheTree()) {
        SetHdrNum(enableHdrEffect, GetInstanceRootNodeId(), HDRComponentType::EFFECT);
    }
    enableHdrEffect_ = enableHdrEffect;
}

void RSRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId, NodeId displayNodeId)
{
#ifdef RS_ENABLE_GPU
    // We do not need to label a child when the child is removed from a parent that is not on the tree
    if (flag == isOnTheTree_) {
        return;
    }

    if (autoClearCloneNode_ && !flag) {
        ClearCloneCrossNode();
    }

    // Need to count upeer or lower trees of HDR nodes
    if (GetType() == RSRenderNodeType::CANVAS_NODE) {
        auto canvasNode = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(shared_from_this());
        if (canvasNode != nullptr && (canvasNode->GetHDRPresent() ||
            canvasNode->GetRenderProperties().IsHDRUIBrightnessValid())) {
            NodeId parentNodeId = flag ? instanceRootNodeId : instanceRootNodeId_;
            ROSEN_LOGD("RSRenderNode::SetIsOnTheTree HDRClient canvasNode[id:%{public}" PRIu64 " name:%{public}s]"
                " parent'S id:%{public}" PRIu64 " ", canvasNode->GetId(), canvasNode->GetNodeName().c_str(),
                parentNodeId);
            if (canvasNode->GetHDRPresent()) {
                SetHdrNum(flag, parentNodeId, HDRComponentType::IMAGE);
                canvasNode->UpdateDisplayHDRNodeList(flag, displayNodeId);
            }
            if (canvasNode->GetRenderProperties().IsHDRUIBrightnessValid()) {
                SetHdrNum(flag, parentNodeId, HDRComponentType::UICOMPONENT);
            }
        }
    }

    if (enableHdrEffect_) {
        NodeId parentNodeId = flag ? instanceRootNodeId : instanceRootNodeId_;
        ROSEN_LOGD("RSRenderNode::SetIsOnTheTree HDREffect Node[id:%{public}" PRIu64 " name:%{public}s]"
            " parent's id:%{public}" PRIu64 " ", GetId(), GetNodeName().c_str(),
            parentNodeId);
        SetHdrNum(flag, parentNodeId, HDRComponentType::EFFECT);
    }

    isNewOnTree_ = flag && !isOnTheTree_;
    isOnTheTree_ = flag;
    displayNodeId_ = displayNodeId;
    if (isOnTheTree_) {
        instanceRootNodeId_ = instanceRootNodeId;
        firstLevelNodeId_ = firstLevelNodeId;
        OnTreeStateChanged();
    } else {
        OnTreeStateChanged();
        instanceRootNodeId_ = instanceRootNodeId;
        if (firstLevelNodeId_ != INVALID_NODEID) {
            preFirstLevelNodeIdSet_.insert(firstLevelNodeId_);
        }
        firstLevelNodeId_ = firstLevelNodeId;
    }
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    if (cacheNodeId != INVALID_NODEID) {
        drawingCacheRootId_ = cacheNodeId;
    }
    if (uifirstRootNodeId != INVALID_NODEID) {
        uifirstRootNodeId_ = uifirstRootNodeId;
    }

    if (stagingRenderParams_) {
        bool ret = stagingRenderParams_->SetFirstLevelNode(firstLevelNodeId_);
        ret |= stagingRenderParams_->SetUiFirstRootNode(uifirstRootNodeId_);
        if (ret) {
            AddToPendingSyncList();
        }
        stagingRenderParams_->SetIsOnTheTree(isOnTheTree_);
    }

    for (auto& weakChild : children_) {
        auto child = weakChild.lock();
        if (child == nullptr) {
            continue;
        }
        if (isOnTheTree_) {
            AddPreFirstLevelNodeIdSet(child->GetPreFirstLevelNodeIdSet());
        }
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId,
            uifirstRootNodeId, displayNodeId);
    }

    for (auto& [child, _] : disappearingChildren_) {
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId,
            uifirstRootNodeId, displayNodeId);
    }
#endif
}

void RSRenderNode::ResetChildRelevantFlags()
{
    childHasVisibleFilter_ = false;
    childHasVisibleEffect_ = false;
    childHasSharedTransition_ = false;
    visibleFilterChild_.clear();
    visibleEffectChild_.clear();
    childrenRect_.Clear();
    hasChildrenOutOfRect_ = false;
}

void RSRenderNode::UpdateChildrenRect(const RectI& subRect)
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

void RSRenderNode::UpdateSubTreeInfo(const RectI& clipRect)
{
    auto& geoPtr = GetRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    lastFrameHasChildrenOutOfRect_ = HasChildrenOutOfRect();
    oldChildrenRect_ = childrenRect_;
    oldClipRect_ = clipRect;
    oldAbsMatrix_ = geoPtr->GetAbsMatrix();
}

void RSRenderNode::ClearCloneCrossNode()
{
    if (cloneCrossNodeVec_.size() == 0) {
        return;
    }

    for (auto it = cloneCrossNodeVec_.begin(); it != cloneCrossNodeVec_.end(); ++it) {
        if (auto parent = (*it)->GetParent().lock()) {
            parent->RemoveChild(*it, true);
        }
    }
    cloneCrossNodeVec_.clear();
}

void RSRenderNode::SetIsCrossNode(bool isCrossNode)
{
    if (!isCrossNode) {
        ClearCloneCrossNode();
    }
    isCrossNode_ = isCrossNode;
}

bool RSRenderNode::IsCrossNode() const
{
    return isCrossNode_ || isCloneCrossNode_;
}

bool RSRenderNode::HasVisitedCrossNode() const
{
    return hasVisitedCrossNode_;
}

void RSRenderNode::SetCrossNodeVisitedStatus(bool hasVisited)
{
    if (isCrossNode_) {
        hasVisitedCrossNode_ = hasVisited;
        RS_LOGD("%{public}s NodeId[%{public}" PRIu64 "] hasVisited:%{public}d", __func__, GetId(), hasVisited);
        for (auto cloneNode : cloneCrossNodeVec_) {
            if (!cloneNode) {
                RS_LOGE("%{public}s cloneNode is nullptr sourceNodeId[%{public}" PRIu64 "] hasVisited:%{public}d",
                        __func__, GetId(), hasVisited);
                continue;
            }
            cloneNode->hasVisitedCrossNode_ = hasVisited;
            RS_LOGD("%{public}s cloneNodeId[%{public}" PRIu64 "] hasVisited:%{public}d",
                    __func__, cloneNode->GetId(), hasVisited);
        }
    } else if (isCloneCrossNode_) {
        auto sourceNode = GetSourceCrossNode().lock();
        if (!sourceNode) {
            RS_LOGE("%{public}s sourceNode is nullptr cloneNodeId[%{public}" PRIu64 "] hasVisited:%{public}d",
                    __func__, GetId(), hasVisited);
            return;
        }
        sourceNode->SetCrossNodeVisitedStatus(hasVisited);
    }
}

void RSRenderNode::AddCrossParentChild(const SharedPtr& child, int32_t index)
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
        child->SetIsOnTheTree(true, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_, uifirstRootNodeId_);
    }
    if (child->IsCrossNode()) {
        child->SetDirty();
    }
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }
    // avoid duplicate entry in disappearingChildren_ (this should not happen)
    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // if child has disappearing transition, add it to disappearingChildren_
    if (child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSRenderNode::RemoveChild %{public}" PRIu64 " move child(id %{public}" PRIu64 ")"
            " into disappearingChildren", GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->SetParent(newParent);
        // attention: set new parent means 'old' parent has removed this child
        hasRemovedChild_ = true;
    }
    children_.erase(it);
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::AddCrossScreenChild(const SharedPtr& child, NodeId cloneNodeId, int32_t index,
    bool autoClearCloneNode)
{
    auto context = GetContext().lock();
    if (child == nullptr || context == nullptr) {
        ROSEN_LOGE("RSRenderNode::AddCrossScreenChild child is null? %{public}d context is null? %{public}d",
            child == nullptr, context == nullptr);
        return;
    }

    child->autoClearCloneNode_ = autoClearCloneNode;

    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode::AddCrossScreenChild cloneNodeId=%" PRIu64 "", cloneNodeId);
    RSSurfaceRenderNodeConfig config = {
        .id = cloneNodeId,
        .name = child->GetNodeName() + "_cloneNode",
        .nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE,
        .surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW
    };
    auto cloneNode = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(config,
        context->weak_from_this()), RSRenderNodeGC::NodeDestructor);
    auto res = context->GetMutableNodeMap().RegisterRenderNode(cloneNode);
    if (!res) {
        ROSEN_LOGE("RSRenderNode::AddCrossScreenChild register clone node failed! id=%{public}"
            "" PRIu64 "", cloneNode->GetId());
        return;
    }
    auto& cloneNodeParams = cloneNode->GetStagingRenderParams();
    if (cloneNodeParams == nullptr) {
        ROSEN_LOGE("RSRenderNode::AddCrossScreenChild failed! clone node params is null. id=%{public}"
            "" PRIu64 "", GetId());
        return;
    }
    child->ApplyPositionZModifier();
    cloneNode->GetMutableRenderProperties().SetPositionZ(child->GetRenderProperties().GetPositionZ());
    cloneNode->isCloneCrossNode_ = true;
    cloneNode->sourceCrossNode_ = child;

    cloneNodeParams->SetCloneSourceDrawable(child->GetRenderDrawable());
    cloneNodeParams->SetShouldPaint(true);
    cloneNodeParams->SetNeedSync(true);
    cloneNode->AddToPendingSyncList();

    child->RecordCloneCrossNode(cloneNode);
    AddChild(cloneNode, index);
}

void RSRenderNode::RecordCloneCrossNode(SharedPtr node)
{
    cloneCrossNodeVec_.emplace_back(node);
}

void RSRenderNode::RemoveCrossScreenChild(const SharedPtr& child)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSRenderNode::RemoveCrossScreenChild child is null");
        return;
    }

    auto cloneIt = std::find_if(child->cloneCrossNodeVec_.begin(), child->cloneCrossNodeVec_.end(),
        [this](auto cloneNode) -> bool {
            if (cloneNode) {
                auto parent = cloneNode->GetParent().lock();
                return parent && parent->GetId() == id_;
            } else {
                return false;
            }
        });
    if (cloneIt == child->cloneCrossNodeVec_.end()) {
        ROSEN_LOGE("RSRenderNode::RemoveCrossScreenChild can not find clone node %{public}" PRIu64 " in source node"
            "(id %{public}" PRIu64 ")", GetId(), child->GetId());
        return;
    }
    RemoveChild(*cloneIt, true);
    child->cloneCrossNodeVec_.erase(cloneIt);
}

void RSRenderNode::RemoveFromTree(bool skipTransition)
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
    parentPtr->isFullChildrenListValid_ = false;
    child->ResetParent();
}

void RSRenderNode::ClearChildren()
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
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::SetParent(WeakPtr parent)
{
    AddSubSurfaceUpdateInfo(parent.lock(), parent_.lock());
    parent_ = parent;
}

void RSRenderNode::ResetParent()
{
    if (auto parentNode = parent_.lock()) {
        parentNode->hasRemovedChild_ = true;
        auto geoPtr = GetRenderProperties().GetBoundsGeometry();
        if (geoPtr != nullptr) {
            parentNode->removedChildrenRect_ = parentNode->removedChildrenRect_.JoinRect(
                geoPtr->MapRect(selfDrawRect_.JoinRect(childrenRect_.ConvertTo<float>()), geoPtr->GetMatrix()));
        }
        (RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && GetType() == RSRenderNodeType::SURFACE_NODE)
            ? SetParentSubTreeDirty()
            : parentNode->SetContentDirty();
        AddSubSurfaceUpdateInfo(nullptr, parentNode);
    }
    SetIsOnTheTree(false);
    parent_.reset();
    OnResetParent();
}

bool RSRenderNode::IsFirstLevelNode()
{
    return id_ == firstLevelNodeId_;
}

void RSRenderNode::DumpTree(int32_t depth, std::string& out) const
{
    // Exceed max depth for dumping render node tree, refuse to continue and add a warning.
    // Possible reason: loop in the render node tree
    constexpr int32_t MAX_DUMP_DEPTH = 256;
    if (depth >= MAX_DUMP_DEPTH) {
        ROSEN_LOGW("RSRenderNode::DumpTree depth exceed max depth, stop dumping. current depth = %d, "
            "nodeId = %" PRIu64, depth, id_);
        out += "===== WARNING: exceed max depth for dumping render node tree =====\n";
        return;
    }

    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    out += "| ";
    DumpNodeType(GetType(), out);
    out += "[" + std::to_string(GetId()) + "], instanceRootNodeId" + "[" +
        std::to_string(GetInstanceRootNodeId()) + "]";
    if (auto displayNode = ReinterpretCastTo<RSDisplayRenderNode>()) {
        out += ", screenId[" + std::to_string(displayNode->GetScreenId()) + "]";
    }
    if (auto surfaceNode = ReinterpretCastTo<RSSurfaceRenderNode>()) {
#if defined(ROSEN_OHOS)
        if (surfaceNode->GetRSSurfaceHandler() && surfaceNode->GetRSSurfaceHandler()->GetConsumer()) {
            out +=
                ", uniqueId[" + std::to_string(surfaceNode->GetRSSurfaceHandler()->GetConsumer()->GetUniqueId()) + "]";
        }
#endif
        if (surfaceNode->HasSubSurfaceNodes()) {
            out += surfaceNode->SubSurfaceNodesDump();
        }
        out += ", abilityState: " +
            std::string(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::FOREGROUND ?
            "foreground" : "background");

#if defined(ROSEN_OHOS)
        out += ", FrameGravity: " + std::to_string((static_cast<int>(
            surfaceNode->GetRenderProperties().GetFrameGravity())));
        if (surfaceNode->GetRSSurfaceHandler() && surfaceNode->GetRSSurfaceHandler()->GetBuffer()) {
            out += ", ScalingMode: " + std::to_string(
                surfaceNode->GetRSSurfaceHandler()->GetBuffer()->GetSurfaceBufferScalingMode());
            if (surfaceNode->GetRSSurfaceHandler()->GetConsumer()) {
                auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
                surfaceNode->GetRSSurfaceHandler()->GetConsumer()->GetSurfaceBufferTransformType(
                    surfaceNode->GetRSSurfaceHandler()->GetBuffer(), &transformType);
                out += ", TransformType: " + std::to_string(transformType);
            }
        }
        out += ", AbsRotation: " + std::to_string(surfaceNode->GetAbsRotation());
#endif
    }
    if (sharedTransitionParam_) {
        out += sharedTransitionParam_->Dump();
    }
    if (IsSuggestedDrawInGroup()) {
        out += ", [nodeGroup" + std::to_string(nodeGroupType_) + "]"; // adapt for SmartPerf Editor tree tool
    }
    if (HasChildrenOutOfRect()) {
        out += ", [ChildrenOutOfParent: true]";
    }
    if (uifirstRootNodeId_ != INVALID_NODEID) {
        out += ", uifirstRootNodeId_: " + std::to_string(uifirstRootNodeId_);
    }
    if (HasSubSurface()) {
        out += ", subSurfaceCnt: " + std::to_string(subSurfaceCnt_);
    }

#if defined(ROSEN_OHOS)
    if (RSSystemProperties::GetDumpRsTreeDetailEnabled()) {
        out += ", PrepareSeq: " + std::to_string(curFrameInfoDetail_.curFramePrepareSeqNum);
        out += ", PostPrepareSeq: " + std::to_string(curFrameInfoDetail_.curFramePostPrepareSeqNum);
        out += ", VsyncId: " + std::to_string(curFrameInfoDetail_.curFrameVsyncId);
        out += ", IsSubTreeSkipped: " + std::to_string(curFrameInfoDetail_.curFrameSubTreeSkipped);
        out += ", ReverseChildren: " + std::to_string(curFrameInfoDetail_.curFrameReverseChildren);
        out += ", zOrder: " + std::to_string(hwcRecorder_.GetZOrderForHwcEnableByFilter());
    }
#endif

    DumpSubClassNode(out);
    out += ", Properties: " + GetRenderProperties().Dump();
    if (uiContextToken_ > 0) {
        out += ", RSUIContextToken: " + std::to_string(uiContextToken_);
    } else {
        out += ", RSUIContextToken: NO_RSUIContext";
    }
    if (GetBootAnimation()) {
        out += ", GetBootAnimation: true";
    }
    if (isContainBootAnimation_) {
        out += ", isContainBootAnimation: true";
    }
    if (dirtyStatus_ != NodeDirty::CLEAN) {
        out += ", isNodeDirty: " + std::to_string(static_cast<int>(dirtyStatus_));
    }
    if (GetRenderProperties().IsDirty()) {
        out += ", isPropertyDirty: true";
    }
    if (isSubTreeDirty_) {
        out += ", isSubTreeDirty: true";
    }
    if (IsPureContainer()) {
        out += ", IsPureContainer: true";
    }
    if (RSSystemProperties::ViewDrawNodeType() && GetType() == RSRenderNodeType::CANVAS_NODE) {
        out += "DrawNodeType: " + DrawNodeTypeToString(GetDrawNodeType());
    }
    if (!oldDirty_.IsEmpty()) {
        out += ", oldDirty: " + oldDirty_.ToString();
    }
    if (!innerAbsDrawRect_.IsEmpty()) {
        out += ", innerAbsDrawRect: " + innerAbsDrawRect_.ToString();
    }
    if (!localShadowRect_.IsEmpty()) {
        out += ", localShadowRect: " + localShadowRect_.ToString();
    }
    if (!localOutlineRect_.IsEmpty()) {
        out += ", localOutlineRect: " + localOutlineRect_.ToString();
    }
    if (!localPixelStretchRect_.IsEmpty()) {
        out += ", localPixelStretchRect: " + localPixelStretchRect_.ToString();
    }
    if (!localForegroundEffectRect_.IsEmpty()) {
        out += ", localForegroundEffectRect: " + localForegroundEffectRect_.ToString();
    }
    if (auto drawRegion = GetRenderProperties().GetDrawRegion()) {
        if (!drawRegion->IsEmpty()) {
            out += ", drawRegion: " + drawRegion->ToString();
        }
    }
    if (drawableVecStatus_ != 0) {
        out += ", drawableVecStatus: " + std::to_string(drawableVecStatus_);
    }
    if (isRepaintBoundary_) {
        out += ", RB: true";
    }
    DumpDrawCmdModifiers(out);
    DumpModifiers(out);
    animationManager_.DumpAnimations(out);
    ChildrenListDump(out);

    for (auto& child : children_) {
        if (auto c = child.lock()) {
            c->DumpTree(depth + 1, out);
        }
    }
    for (auto& [child, pos] : disappearingChildren_) {
        child->DumpTree(depth + 1, out);
    }
}

void RSRenderNode::ChildrenListDump(std::string& out) const
{
    auto sortedChildren = GetSortedChildren();
    const int childrenCntLimit = 10;
    if (!isFullChildrenListValid_) {
        out += ", Children list needs update, current count: " + std::to_string(fullChildrenList_->size());
        if (!fullChildrenList_->empty()) {
            int cnt = 0;
            out += "(";
            for (auto child = fullChildrenList_->begin(); child != fullChildrenList_->end(); child++) {
                if (cnt > childrenCntLimit) {
                    break;
                }
                if ((*child) == nullptr) {
                    continue;
                }
                out += std::to_string((*child)->GetId()) + " ";
                cnt++;
            }
            out += ")";
        }
        out +=" expected count: " + std::to_string(sortedChildren->size());
        if (!sortedChildren->empty()) {
            int cnt = 0;
            out += "(";
            for (auto child = sortedChildren->begin(); child != sortedChildren->end(); child++) {
                if (cnt > childrenCntLimit) {
                    break;
                }
                if ((*child) == nullptr) {
                    continue;
                }
                out += std::to_string((*child)->GetId()) + " ";
                cnt++;
            }
            out += ")";
        }
    } else if (!sortedChildren->empty()) {
        out += ", sortedChildren: " + std::to_string(sortedChildren->size());
        int cnt = 0;
        out += "(";
        for (auto child = sortedChildren->begin(); child != sortedChildren->end(); child++) {
            if (cnt > childrenCntLimit) {
                break;
            }
            if ((*child) == nullptr) {
                continue;
            }
            out += std::to_string((*child)->GetId()) + " ";
            cnt++;
        }
        out += ")";
    }
    if (!disappearingChildren_.empty()) {
        out += ", disappearingChildren: " + std::to_string(disappearingChildren_.size());
        int cnt = 0;
        out += "(";
        for (auto& [child, _] : disappearingChildren_) {
            if (cnt > childrenCntLimit) {
                break;
            }
            if (child == nullptr) {
                continue;
            }
            out += std::to_string(child->GetId()) + " ";
            cnt++;
        }
        out += ")";
    }
    out += "\n";
}

void RSRenderNode::DumpNodeType(RSRenderNodeType nodeType, std::string& out)
{
    switch (nodeType) {
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
        case RSRenderNodeType::CANVAS_DRAWING_NODE: {
            out += "CANVAS_DRAWING_NODE";
            break;
        }
        case RSRenderNodeType::EFFECT_NODE: {
            out += "EFFECT_NODE";
            break;
        }
        default: {
            out += "UNKNOWN_NODE";
            break;
        }
    }
}

void RSRenderNode::DumpSubClassNode(std::string& out) const
{
    if (GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = (static_cast<const RSSurfaceRenderNode*>(this));
        auto p = parent_.lock();
        out += ", Parent [" + (p != nullptr ? std::to_string(p->GetId()) : "null") + "]";
        out += ", Name [" + surfaceNode->GetName() + "]";
        out += ", hasConsumer: " + std::to_string(surfaceNode->GetRSSurfaceHandler()->HasConsumer());
        std::string propertyAlpha = std::to_string(surfaceNode->GetRenderProperties().GetAlpha());
        out += ", Alpha: " + propertyAlpha;
        if (surfaceNode->contextAlpha_ < 1.0f) {
            std::string contextAlpha = std::to_string(surfaceNode->contextAlpha_);
            out += " (ContextAlpha: " + contextAlpha + ")";
        }
        out += ", Visible: " + std::to_string(surfaceNode->GetRenderProperties().GetVisible());
        out += ", Visible" + surfaceNode->GetVisibleRegion().GetRegionInfo();
        out += ", Opaque" + surfaceNode->GetOpaqueRegion().GetRegionInfo();
        out += ", OcclusionBg: " + std::to_string(surfaceNode->GetAbilityBgAlpha());
        const auto specialLayerManager = surfaceNode->GetSpecialLayerMgr();
        out += ", SpecialLayer: " + std::to_string(specialLayerManager.Get());
        out += ", surfaceType: " + std::to_string((int)surfaceNode->GetSurfaceNodeType());
        out += ", ContainerConfig: " + surfaceNode->GetContainerConfigDump();
        out += ", colorSpace: " + std::to_string(surfaceNode->GetColorSpace());
        out += ", uifirstColorGamut: " + std::to_string(surfaceNode->GetFirstLevelNodeColorGamut());
    } else if (GetType() == RSRenderNodeType::ROOT_NODE) {
        auto rootNode = static_cast<const RSRootRenderNode*>(this);
        out += ", Visible: " + std::to_string(rootNode->GetRenderProperties().GetVisible());
        out += ", Size: [" + std::to_string(rootNode->GetRenderProperties().GetFrameWidth()) + ", " +
            std::to_string(rootNode->GetRenderProperties().GetFrameHeight()) + "]";
        out += ", EnableRender: " + std::to_string(rootNode->GetEnableRender());
    } else if (GetType() == RSRenderNodeType::DISPLAY_NODE) {
        auto displayNode = static_cast<const RSDisplayRenderNode*>(this);
        out += ", skipLayer: " + std::to_string(displayNode->GetSecurityDisplay());
        out += ", securityExemption: " + std::to_string(displayNode->GetSecurityExemption());
        out += ", virtualScreenMuteStatus: " + std::to_string(displayNode->GetVirtualScreenMuteStatus());
        out += ", colorSpace: " + std::to_string(displayNode->GetColorSpace());
    } else if (GetType() == RSRenderNodeType::CANVAS_NODE) {
        auto canvasNode = static_cast<const RSCanvasRenderNode*>(this);
        NodeId linkedRootNodeId = canvasNode->GetLinkedRootNodeId();
        if (linkedRootNodeId != INVALID_NODEID) {
            out += ", linkedRootNodeId: " + std::to_string(linkedRootNodeId);
        }
    }
}

void RSRenderNode::DumpDrawCmdModifiers(std::string& out) const
{
    if (drawCmdModifiers_.empty()) {
        return;
    }
    std::string splitStr = ", ";
    std::string modifierDesc = ", DrawCmdModifiers:[";
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        auto modifierTypeString = std::make_shared<RSModifierTypeString>();
        std::string typeName = modifierTypeString->GetModifierTypeString(type);
        modifierDesc += typeName + ":[";
        std::string propertyDesc = "";
        bool found = false;
        for (auto& modifier : modifiers) {
            if (modifier != nullptr) {
                found = true;
                modifier->Dump(propertyDesc);
                propertyDesc += splitStr;
            }
        }
        if (found) {
            modifierDesc += propertyDesc.substr(0, propertyDesc.length() - splitStr.length());
        }
        modifierDesc += "]" + splitStr;
    }
    out += modifierDesc.substr(0, modifierDesc.length() - splitStr.length()) + "]";
}

void RSRenderNode::DumpModifiers(std::string& out) const
{
    if (modifiers_.empty()) {
        return;
    }
    std::string splitStr = ", ";
    out += ", OtherModifiers:[";
    std::string propertyDesc = "";
    for (auto& [type, modifier] : modifiers_) {
        auto pid = ExtractPid(modifier->GetPropertyId());
        propertyDesc = propertyDesc + "pid:" + std::to_string(pid) + "->";
        propertyDesc += modifier->GetModifierTypeString();
        modifier->Dump(propertyDesc);
        propertyDesc += splitStr;
    }
    out += propertyDesc.substr(0, propertyDesc.length() - splitStr.length()) + "]";
}

void RSRenderNode::ResetIsOnlyBasicGeoTransform()
{
    isOnlyBasicGeoTransform_ = true;
}

bool RSRenderNode::IsOnlyBasicGeoTransform() const
{
    return isOnlyBasicGeoTransform_;
}

void RSRenderNode::ForceMergeSubTreeDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect)
{
    // prepare skip -> quick prepare, old dirty do not update
    if (geoUpdateDelay_) {
        auto absChildrenRect = RSObjAbsGeometry::MapRect(oldChildrenRect_.ConvertTo<float>(), oldAbsMatrix_);
        subTreeDirtyRegion_ = absChildrenRect.IntersectRect(oldClipRect_);
        dirtyManager.MergeDirtyRect(subTreeDirtyRegion_);
    }
    lastFrameSubTreeSkipped_ = false;
}

void RSRenderNode::SubTreeSkipPrepare(
    RSDirtyRegionManager& dirtyManager, bool isDirty, bool accumGeoDirty, const RectI& clipRect)
{
    // [planning] Prev and current dirty rect need to be joined only when accumGeoDirty is true.
    if ((isDirty || accumGeoDirty) && (HasChildrenOutOfRect() || lastFrameHasChildrenOutOfRect_)) {
        auto& geoPtr = GetRenderProperties().GetBoundsGeometry();
        if (geoPtr == nullptr) {
            return;
        }
        auto oldDirtyRect = geoPtr->MapRect(oldChildrenRect_.ConvertTo<float>(), oldAbsMatrix_);
        auto oldDirtyRectClip = oldDirtyRect.IntersectRect(oldClipRect_);
        auto dirtyRect = geoPtr->MapAbsRect(childrenRect_.ConvertTo<float>());
        auto dirtyRectClip = dirtyRect.IntersectRect(clipRect);
        dirtyRectClip = dirtyRectClip.JoinRect(oldDirtyRectClip);
        IsFirstLevelCrossNode() ?
            dirtyManager.MergeDirtyRect(dirtyRect.JoinRect(oldDirtyRect)) : dirtyManager.MergeDirtyRect(dirtyRectClip);
        UpdateSubTreeSkipDirtyForDFX(dirtyManager, dirtyRectClip);
    }
    if (isDirty && GetChildrenCount() == 0) {
        ResetChildRelevantFlags();
    }
    SetGeoUpdateDelay(accumGeoDirty);
    UpdateSubTreeInfo(clipRect);
    lastFrameSubTreeSkipped_ = true;
}

// attention: current all base node's dirty ops causing content dirty
void RSRenderNode::SetContentDirty()
{
    isContentDirty_ = true;
    isOnlyBasicGeoTransform_ = false;
    SetDirty();
}

void RSRenderNode::SetDirty(bool forceAddToActiveList)
{
    bool dirtyEmpty = dirtyTypes_.none() && dirtyTypesNG_.none();
    // TO avoid redundant add, only add if both: 1. on-tree node 2. newly dirty node (or forceAddToActiveList = true)
    if (dirtyStatus_ == NodeDirty::CLEAN || dirtyEmpty || forceAddToActiveList) {
        if (auto context = GetContext().lock()) {
            context->AddActiveNode(shared_from_this());
        }
    }
    SetParentSubTreeDirty();
    dirtyStatus_ = NodeDirty::DIRTY;
}

void RSRenderNode::CollectSurface(
    const std::shared_ptr<RSRenderNode>& node, std::vector<RSRenderNode::SharedPtr>& vec, bool isUniRender,
    bool onlyFirstLevel)
{
    if (node == nullptr) {
        return;
    }

    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
    }
}

void RSRenderNode::CollectSelfDrawingChild(const std::shared_ptr<RSRenderNode>& node, std::vector<NodeId>& vec)
{
    if (node == nullptr) {
        return;
    }

    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSelfDrawingChild(child, vec);
    }
}

void RSRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareChildren(*this);
}

void RSRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareChildren(*this);

    // fallback for global root node
    UpdateRenderParams();
    AddToPendingSyncList();
}

bool RSRenderNode::IsSubTreeNeedPrepare(bool filterInGlobal, bool isOccluded)
{
    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    if (checkType == SubTreePrepareCheckType::DISABLED) {
        return true;
    }
    // stop visit invisible or clean without filter subtree
    // Exception: If cross-display node is fully invisible under current visited display, its subtree can't be skipped,
    // since it may be visible on other displays, and it is only prepared once.
    if (!shouldPaint_) {
        // when subTreeOccluded, need to applyModifiers to node's children
        RS_OPTIONAL_TRACE_NAME_FMT("IsSubTreeNeedPrepare node[%llu] skip subtree ShouldPaint [%d], isOccluded [%d], "
            "CrossDisplay: %d", GetId(), shouldPaint_, isOccluded, IsFirstLevelCrossNode());
        return false;
    }
    if (checkType == SubTreePrepareCheckType::DISABLE_SUBTREE_DIRTY_CHECK) {
        return true;
    }
    if (IsSubTreeDirty()) {
        // reset iff traverses dirty subtree
        SetSubTreeDirty(false);
        UpdateChildrenOutOfRectFlag(false); // collect again
        return true;
    }
    if (childHasSharedTransition_ || isAccumulatedClipFlagChanged_ || subSurfaceCnt_ > 0) {
        return true;
    }
    if (ChildHasVisibleFilter()) {
        RS_OPTIONAL_TRACE_NAME_FMT("IsSubTreeNeedPrepare node[%d] filterInGlobal_[%d]",
            GetId(), filterInGlobal);
    }
    // if clean without filter skip subtree
    return ChildHasVisibleFilter() ? filterInGlobal : false;
}

void RSRenderNode::PrepareChildrenForApplyModifiers()
{
    auto children = GetSortedChildren();
    std::for_each((*children).begin(), (*children).end(),
        [this](const std::shared_ptr<RSRenderNode>& node) {
        node->PrepareSelfNodeForApplyModifiers();
    });
}

void RSRenderNode::PrepareSelfNodeForApplyModifiers()
{
#ifdef RS_ENABLE_GPU
    ApplyModifiers();
    PrepareChildrenForApplyModifiers();

    stagingRenderParams_->SetAlpha(GetRenderProperties().GetAlpha());

    UpdateRenderParams();
    AddToPendingSyncList();
#endif
}

bool RSRenderNode::IsUifirstArkTsCardNode()
{
    if (nodeGroupType_ == NodeGroupType::NONE) {
        return false;
    }
    for (auto& child : *GetChildren()) {
        if (!child) {
            continue;
        }
        auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceChild) {
            continue;
        }
        if (surfaceChild->GetLastFrameUifirstFlag() == MultiThreadCacheType::ARKTS_CARD) {
            return true;
        }
    }
    return false;
}

void RSRenderNode::UpdateDrawingCacheInfoBeforeChildren(bool isScreenRotation)
{
    auto foregroundFilterCache = GetRenderProperties().GetForegroundFilterCache();
    bool rotateOptimize  = RSSystemProperties::GetCacheOptimizeRotateEnable() ? false : isScreenRotation;
    if (!ShouldPaint() || (rotateOptimize && !foregroundFilterCache)) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        return;
    }
    CheckDrawingCacheType();
    if (GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        RS_LOGD("RSRenderNode::UpdateDrawingCacheInfoBC drawingCacheType is %{public}d", GetDrawingCacheType());
        return;
    }
    SetDrawingCacheChanged((IsContentDirty() || IsSubTreeDirty() || IsAccessibilityConfigChanged()));
    RS_OPTIONAL_TRACE_NAME_FMT(
        "SetDrawingCacheChanged id:%llu nodeGroupType:%d contentDirty:%d propertyDirty:%d subTreeDirty:%d "
        "AccessibilityConfigChanged:%d",
        GetId(), nodeGroupType_, isContentDirty_, GetRenderProperties().IsContentDirty(), IsSubTreeDirty(),
        IsAccessibilityConfigChanged());
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetDrawingCacheIncludeProperty(nodeGroupIncludeProperty_);
#endif
}

void RSRenderNode::UpdateDrawingCacheInfoAfterChildren(bool isInBlackList)
{
    RS_LOGI_IF(DEBUG_NODE, "RSRenderNode::UpdateDrawingCacheInfoAC uifirstArkTsCardNode:%{public}d"
        " startingWindowFlag_:%{public}d HasChildrenOutOfRect:%{public}d drawingCacheType:%{public}d",
        IsUifirstArkTsCardNode(), startingWindowFlag_, HasChildrenOutOfRect(), GetDrawingCacheType());
    if (IsUifirstArkTsCardNode() || startingWindowFlag_) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    } else if (isInBlackList) {
        stagingRenderParams_->SetNodeGroupHasChildInBlackList(true);
    }
    if (HasChildrenOutOfRect() && GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawingCacheInfoAfter ChildrenOutOfRect id:%llu", GetId());
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    }
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetDrawingCacheType(GetDrawingCacheType());
#endif
    if (GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawingCacheInfoAfter id:%llu cacheType:%d childHasVisibleFilter:%d " \
            "childHasVisibleEffect:%d",
            GetId(), GetDrawingCacheType(), childHasVisibleFilter_, childHasVisibleEffect_);
    }
    AddToPendingSyncList();
}

void RSRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessChildren(*this);
}

void RSRenderNode::SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId)
{
    auto transactionProxy = RSTransactionProxy::GetInstance(); // planing
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command, nodeId);
    }
}

void RSRenderNode::InternalRemoveSelfFromDisappearingChildren()
{
    // internal use only, force remove self from parent's disappearingChildren_
    auto parent = parent_.lock();
    if (parent == nullptr) {
        return;
    }
    auto it = std::find_if(parent->disappearingChildren_.begin(), parent->disappearingChildren_.end(),
        [childPtr = shared_from_this()](const auto& pair) -> bool { return pair.first == childPtr; });
    if (it == parent->disappearingChildren_.end()) {
        return;
    }
    parent->disappearingChildren_.erase(it);
    parent->isFullChildrenListValid_ = false;
    ResetParent();
}

RSRenderNode::~RSRenderNode()
{
    if (appPid_ != 0) {
        RSSingleFrameComposer::AddOrRemoveAppPidToMap(false, appPid_);
    }
    FallbackAnimationsToRoot();
    if (clearCacheSurfaceFunc_ && (cacheSurface_ || cacheCompletedSurface_)) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_), cacheSurfaceThreadIndex_,
            completedSurfaceThreadIndex_);
    }
#ifdef RS_ENABLE_GPU
    DrawableV2::RSRenderNodeDrawableAdapter::RemoveDrawableFromCache(GetId());
#endif
    ClearCacheSurface();
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGD("RSRenderNode::~RSRenderNode: Invalid context");
        return;
    }
}

void RSRenderNode::FallbackAnimationsToRoot()
{
    if (animationManager_.animations_.empty()) {
        return;
    }

    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSRenderNode::FallbackAnimationsToRoot: Invalid context");
        return;
    }
    auto target = context->GetNodeMap().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }
    context->RegisterAnimatingRenderNode(target);

    for (auto& [unused, animation] : animationManager_.animations_) {
        if (animation->IsPaused()) {
            animation->Resume();
        }

        animation->Detach(true);
        // avoid infinite loop for fallback animation
        animation->SetRepeatCount(1);
        target->animationManager_.AddAnimation(std::move(animation));
    }
    animationManager_.animations_.clear();
    // Check the next frame's VSync has been requested. If there is no request, add another VSync request
    if (!context->IsRequestedNextVsyncAnimate()) {
        context->RequestVsync();
        context->SetRequestedNextVsyncAnimate(true);
    }
}

void RSRenderNode::ActivateDisplaySync()
{
    if (!displaySync_) {
        displaySync_ = std::make_shared<RSRenderDisplaySync>(GetId());
    }
}

void RSRenderNode::UpdateDisplaySyncRange()
{
    if (!displaySync_) {
        return;
    }
    auto animationRange = animationManager_.GetFrameRateRange();
    if (animationRange.IsValid()) {
        displaySync_->SetExpectedFrameRateRange(animationRange);
    }
}

std::tuple<bool, bool, bool> RSRenderNode::Animate(
    int64_t timestamp, int64_t& minLeftDelayTime, int64_t period, bool isDisplaySyncEnabled)
{
    if (displaySync_ && displaySync_->OnFrameSkip(timestamp, period, isDisplaySyncEnabled)) {
        minLeftDelayTime = 0;
        return displaySync_->GetAnimateResult();
    }
    RSSurfaceNodeAbilityState abilityState = RSSurfaceNodeAbilityState::FOREGROUND;

    // Animation on surfaceNode is always on foreground ability state.
    // If instanceRootNode is surfaceNode, get its ability state. If not, regard it as foreground ability state.
    if (GetType() != RSRenderNodeType::SURFACE_NODE) {
        if (auto instanceRootNode = GetInstanceRootNode()) {
            abilityState = instanceRootNode->GetAbilityState();
        }
    }

    auto animateResult = animationManager_.Animate(timestamp, minLeftDelayTime, IsOnTheTree(), abilityState);
    if (displaySync_) {
        displaySync_->SetAnimateResult(animateResult);
    }
    return animateResult;
}

bool RSRenderNode::IsClipBound() const
{
    return GetRenderProperties().GetClipToBounds() || GetRenderProperties().GetClipToFrame();
}

bool RSRenderNode::SetAccumulatedClipFlag(bool clipChange)
{
    isAccumulatedClipFlagChanged_ = (hasAccumulatedClipFlag_ != IsClipBound()) || clipChange;
    if (isAccumulatedClipFlagChanged_) {
        hasAccumulatedClipFlag_ = IsClipBound();
    }
    return isAccumulatedClipFlagChanged_;
}

#ifdef RS_ENABLE_GPU
std::unique_ptr<RSRenderParams>& RSRenderNode::GetStagingRenderParams()
{
    return stagingRenderParams_;
}

// Deprecated! Do not use this interface.
// This interface has crash risks and will be deleted in later versions.
const std::unique_ptr<RSRenderParams>& RSRenderNode::GetRenderParams() const
{
    if (renderDrawable_ == nullptr) {
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    }
    return renderDrawable_->renderParams_;
}
#endif
void RSRenderNode::CollectAndUpdateLocalShadowRect()
{
    // update shadow if shadow changes
    if (dirtySlots_.find(RSDrawableSlot::SHADOW) != dirtySlots_.end()) {
        auto& properties = GetRenderProperties();
        if (properties.IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                RRect absClipRRect = RRect(properties.GetBoundsRect(), properties.GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(localShadowRect_, properties, &absClipRRect, false, true);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(localShadowRect_, properties, nullptr, false, true);
            }
        }
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localShadowRect_.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalOutlineRect()
{
    // update outline if oueline changes
    if (dirtySlots_.find(RSDrawableSlot::OUTLINE) != dirtySlots_.end()) {
        RSPropertiesPainter::GetOutlineDirtyRect(localOutlineRect_, GetRenderProperties(), false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localOutlineRect_.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalPixelStretchRect()
{
    // update outline if oueline changes
    if (dirtySlots_.find(RSDrawableSlot::PIXEL_STRETCH) != dirtySlots_.end()) {
        RSPropertiesPainter::GetPixelStretchDirtyRect(localPixelStretchRect_, GetRenderProperties(), false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localPixelStretchRect_.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalForegroundEffectRect()
{
    // update foreground effect's dirty region if it changes
    if (GetRenderProperties().GetForegroundEffectDirty()) {
        RSPropertiesPainter::GetForegroundEffectDirtyRect(localForegroundEffectRect_, GetRenderProperties(), false);
        GetMutableRenderProperties().SetForegroundEffectDirty(false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localForegroundEffectRect_.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalDistortionEffectRect()
{
    // update distortion effect's dirty region if it changes
    if (GetRenderProperties().GetDistortionDirty()) {
        RSPropertiesPainter::GetDistortionEffectDirtyRect(localDistortionEffectRect_, GetRenderProperties());
        GetMutableRenderProperties().SetDistortionDirty(false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localDistortionEffectRect_.ConvertTo<float>());
}

void RSRenderNode::UpdateBufferDirtyRegion()
{
#ifndef ROSEN_CROSS_PLATFORM
    isSelfDrawingNode_ = false;
    if (GetType() != RSRenderNodeType::SURFACE_NODE) {
        return;
    }
    auto surfaceNode = ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode == nullptr) {
        return;
    }
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    if (buffer != nullptr) {
        isSelfDrawingNode_ = true;
        // Use the matrix from buffer to relative coordinate and the absolute matrix
        // to calculate the buffer damageRegion's absolute rect
        auto rect = surfaceNode->GetRSSurfaceHandler()->GetDamageRegion();
        auto matrix = surfaceNode->GetBufferRelMatrix();
        auto bufferDirtyRect = GetRenderProperties().GetBoundsGeometry()->MapRect(
            RectF(rect.x, rect.y, rect.w, rect.h), matrix).ConvertTo<float>();
        // The buffer's dirtyRect should not be out of the scope of the node's dirtyRect
        selfDrawingNodeDirtyRect_ = bufferDirtyRect.IntersectRect(selfDrawRect_);
        RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode id: %" PRIu64 ", buffer size [%d,%d], "
            "buffer damageRegion [%d,%d,%d,%d], dirtyRect %s", GetId(),
            buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
            rect.x, rect.y, rect.w, rect.h, selfDrawingNodeDirtyRect_.ToString().c_str());
    }
#endif
}

bool RSRenderNode::UpdateSelfDrawRect()
{
    auto prevSelfDrawRect = selfDrawRect_;
    // empty rect would not join and doesn't need to check
    auto& properties = GetRenderProperties();
    selfDrawRect_ = properties.GetLocalBoundsAndFramesRect();
    UpdateBufferDirtyRegion();
    if (auto drawRegion = properties.GetDrawRegion()) {
        selfDrawRect_ = selfDrawRect_.JoinRect(*drawRegion);
    }
    CollectAndUpdateLocalShadowRect();
    CollectAndUpdateLocalOutlineRect();
    CollectAndUpdateLocalPixelStretchRect();
    CollectAndUpdateLocalDistortionEffectRect();
    return !selfDrawRect_.IsNearEqual(prevSelfDrawRect);
}

const RectF& RSRenderNode::GetSelfDrawRect() const
{
    return selfDrawRect_;
}

const RectI& RSRenderNode::GetAbsDrawRect() const
{
    return absDrawRect_;
}

bool RSRenderNode::CheckAndUpdateGeoTrans(std::shared_ptr<RSObjAbsGeometry>& geoPtr)
{
    if (drawCmdModifiers_.find(RSModifierType::GEOMETRYTRANS) == drawCmdModifiers_.end()) {
        return false;
    }
    RSModifierContext context = { GetMutableRenderProperties() };
    for (auto& modifier : drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
        // todo Concat matrix directly
        modifier->Apply(context);
    }
    return true;
}

void RSRenderNode::UpdateAbsDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect)
{
    // merge old children draw rect if node's sub tree is all dirty
    const auto& renderProperties = GetRenderProperties();
    if (renderProperties.IsSubTreeAllDirty()) {
        auto oldChildrenDirtyRect = renderProperties.GetBoundsGeometry()->MapRect(
            oldChildrenRect_.ConvertTo<float>(), oldAbsMatrix_);
        dirtyManager.MergeDirtyRect(IsFirstLevelCrossNode() ? oldChildrenDirtyRect :
            oldChildrenDirtyRect.IntersectRect(oldClipRect_));
    }
    // it is necessary to ensure that last frame dirty rect is merged
    auto oldDirtyRect = oldDirty_;
    if (absDrawRect_ != oldAbsDrawRect_) {
        if (isSelfDrawingNode_) {
            // merge self drawing node last frame size and join current frame size to absDrawRect_ when changed
            dirtyManager.MergeDirtyRect(oldAbsDrawRect_.IntersectRect(clipRect));
            selfDrawingNodeAbsDirtyRect_.JoinRect(absDrawRect_);
        }
        oldAbsDrawRect_ = absDrawRect_;
    }
    // easily merge oldDirty if switch to invisible
    if (!shouldPaint_ && isLastVisible_) {
        dirtyManager.MergeDirtyRect(oldDirtyRect);
        return;
    }
    auto dirtyRect = isSelfDrawingNode_ ? selfDrawingNodeAbsDirtyRect_ : absDrawRect_;
    dirtyRect = IsFirstLevelCrossNode() ? dirtyRect : dirtyRect.IntersectRect(clipRect);
    oldDirty_ = dirtyRect;
    oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
    dirtyManager.MergeDirtyRect(dirtyRect.JoinRect(oldDirtyRect));
    // compute inward-rounding abs draw rect, used for opaque region calculations
    auto dirtyRectF = isSelfDrawingNode_ ? selfDrawingNodeAbsDirtyRectF_ : absDrawRectF_;
    innerAbsDrawRect_ = RSObjAbsGeometry::DeflateToRectI(dirtyRectF);
    if (!IsFirstLevelCrossNode()) {
        innerAbsDrawRect_ = innerAbsDrawRect_.IntersectRect(clipRect);
    }
}

bool RSRenderNode::UpdateDrawRectAndDirtyRegion(RSDirtyRegionManager& dirtyManager, bool accumGeoDirty,
    const RectI& clipRect, const Drawing::Matrix& parentSurfaceMatrix)
{
    auto& properties = GetMutableRenderProperties();
#ifdef RS_ENABLE_PREFETCH
    // The 2 is the cache level.
    __builtin_prefetch(&(properties.boundsGeo_), 0, 2);
    __builtin_prefetch(&(properties.frameGeo_), 0, 2);
#endif
    // 1. update self drawrect if dirty
    bool selfDrawRectChanged = IsDirty() ? UpdateSelfDrawRect() : false;
    if (selfDrawRectChanged) {
        UpdateChildrenOutOfRectFlag(!childrenRect_.ConvertTo<float>().IsInsideOf(selfDrawRect_));
    }
    // 2. update geoMatrix by parent for dirty collection
    // update geoMatrix and accumGeoDirty if needed
    auto parent = curCloneNodeParent_.lock();
    if (parent == nullptr) {
        parent = GetParent().lock();
    }
    if (parent && parent->GetGeoUpdateDelay()) {
        accumGeoDirty = true;
        // Set geometry update delay flag recursively to update node's old dirty in subTree
        SetGeoUpdateDelay(true);
        oldAbsMatrix_ = parent->oldAbsMatrix_;
        oldAbsMatrix_.PostConcat(oldMatrix_);
    }
    if (accumGeoDirty || properties.NeedClip() || properties.geoDirty_ || (dirtyStatus_ != NodeDirty::CLEAN)) {
        UpdateDrawRect(accumGeoDirty, clipRect, parentSurfaceMatrix);
        // planning: double check if it would be covered by updateself without geo update
        // currently CheckAndUpdateGeoTrans without dirty check
        auto& geoPtr = properties.boundsGeo_;
        // selfdrawing node's geo may not dirty when its dirty region changes
        if (geoPtr && (CheckAndUpdateGeoTrans(geoPtr) || accumGeoDirty || properties.geoDirty_ ||
            isSelfDrawingNode_ || selfDrawRectChanged)) {
            absDrawRectF_ = geoPtr->MapRectWithoutRounding(selfDrawRect_, geoPtr->GetAbsMatrix());
            absDrawRect_ = geoPtr->InflateToRectI(absDrawRectF_);
            if (isSelfDrawingNode_) {
                selfDrawingNodeAbsDirtyRectF_ = geoPtr->MapRectWithoutRounding(
                    selfDrawingNodeDirtyRect_, geoPtr->GetAbsMatrix());
                selfDrawingNodeAbsDirtyRect_ = geoPtr->InflateToRectI(selfDrawingNodeAbsDirtyRectF_);
            }
            UpdateSrcOrClipedAbsDrawRectChangeState(clipRect);
        }
    }
    // 3. update dirtyRegion if needed
    if (properties.GetBackgroundFilter()) {
        UpdateFilterCacheWithBelowDirty(Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()));
    }
    ValidateLightResources();
    isDirtyRegionUpdated_ = false; // todo make sure why windowDirty use it
    // Only when satisfy following conditions, absDirtyRegion should update:
    // 1.The node is dirty; 2.The clip absDrawRect change; 3.Parent clip property change or has GeoUpdateDelay dirty;
    // When the subtree is all dirty and the node should not paint, it also needs to add dirty region
    if ((IsDirty() || srcOrClipedAbsDrawRectChangeFlag_ || (parent && (parent->GetAccumulatedClipFlagChange() ||
        parent->GetGeoUpdateDelay()))) && (shouldPaint_ || isLastVisible_ || properties.IsSubTreeAllDirty())) {
        // update ForegroundFilterCache
        UpdateAbsDirtyRegion(dirtyManager, clipRect);
        UpdateDirtyRegionInfoForDFX(dirtyManager);
    }
    // 4. reset dirty status
    ResetDirtyStatus();
    return accumGeoDirty;
}

void RSRenderNode::UpdateDrawRect(
    bool& accumGeoDirty, const RectI& clipRect, const Drawing::Matrix& parentSurfaceMatrix)
{
    auto parent = curCloneNodeParent_.lock();
    if (parent == nullptr) {
        parent = GetParent().lock();
    }
    auto& properties = GetMutableRenderProperties();
    if (auto sandbox = properties.GetSandBox(); sandbox.has_value() && sharedTransitionParam_) {
        // case a. use parent sur_face matrix with sandbox
        auto translateMatrix = Drawing::Matrix();
        translateMatrix.Translate(sandbox->x_, sandbox->y_);
        properties.GetBoundsGeometry()->SetContextMatrix(translateMatrix);
        accumGeoDirty = properties.UpdateGeometryByParent(&parentSurfaceMatrix, std::nullopt) || accumGeoDirty;
        properties.GetBoundsGeometry()->SetContextMatrix(std::nullopt);
    } else if (parent != nullptr) {
        // case b. use parent matrix
        auto parentMatrix = &(parent->GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix());
        bool isSurfaceRenderNode = IsInstanceOf<RSSurfaceRenderNode>();
        auto offset = !isSurfaceRenderNode
                          ? std::make_optional<Drawing::Point>(parent->GetRenderProperties().GetFrameOffsetX(),
                                parent->GetRenderProperties().GetFrameOffsetY())
                          : std::nullopt;
        if (isSurfaceRenderNode && GetGlobalPositionEnabled()) {
            offset = std::make_optional<Drawing::Point>(-GetPreparedDisplayOffsetX(), -GetPreparedDisplayOffsetY());
        }
        accumGeoDirty = properties.UpdateGeometryByParent(parentMatrix, offset) || accumGeoDirty;
    } else {
        // case c. no parent
        accumGeoDirty = properties.UpdateGeometryByParent(nullptr, std::nullopt) || accumGeoDirty;
    }
}

void RSRenderNode::UpdateDirtyRegionInfoForDFX(RSDirtyRegionManager& dirtyManager)
{
    if (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebugType::DISABLED) {
        return;
    }
    // update dirty region information that depends on geoPtr.
    auto& properties = GetRenderProperties();
    if (auto& geoPtr = properties.GetBoundsGeometry()) {
        // drawRegion can be nullptr if not set.
        if (auto drawRegion = properties.GetDrawRegion()) {
            dirtyManager.UpdateDirtyRegionInfoForDfx(
                GetId(), GetType(), DirtyRegionType::OVERLAY_RECT, geoPtr->MapAbsRect(*drawRegion));
        }
        dirtyManager.UpdateDirtyRegionInfoForDfx(
            GetId(), GetType(), DirtyRegionType::SHADOW_RECT, geoPtr->MapAbsRect(localShadowRect_.ConvertTo<float>()));
        dirtyManager.UpdateDirtyRegionInfoForDfx(GetId(),
            GetType(), DirtyRegionType::OUTLINE_RECT, geoPtr->MapAbsRect(localOutlineRect_.ConvertTo<float>()));
    }

    // update dirty region information in abs Coords.
    dirtyManager.UpdateDirtyRegionInfoForDfx(
        GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
    dirtyManager.UpdateDirtyRegionInfoForDfx(
        GetId(), GetType(), DirtyRegionType::FILTER_RECT, filterRegion_);
    if (LastFrameSubTreeSkipped()) {
        dirtyManager.UpdateDirtyRegionInfoForDfx(
            GetId(), GetType(), DirtyRegionType::SUBTREE_SKIP_RECT, subTreeDirtyRegion_);
    }
    if (properties.GetClipToBounds() || properties.GetClipToFrame()) {
        dirtyManager.UpdateDirtyRegionInfoForDfx(
            GetId(), GetType(), DirtyRegionType::PREPARE_CLIP_RECT, GetAbsDrawRect());
    }
}

void RSRenderNode::UpdateSubTreeSkipDirtyForDFX(RSDirtyRegionManager& dirtyManager, const RectI& rect)
{
    if (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebugType::DISABLED) {
        return;
    }
    dirtyManager.UpdateDirtyRegionInfoForDfx(
        GetId(), GetType(), DirtyRegionType::SUBTREE_SKIP_OUT_OF_PARENT_RECT, rect);
}

bool RSRenderNode::Update(RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent,
    bool parentDirty, std::optional<RectI> clipRect)
{
    // no need to update invisible nodes
    if (!ShouldPaint() && !isLastVisible_) {
        SetClean();
        GetMutableRenderProperties().ResetDirty();
        return false;
    }
    // [planning] surfaceNode use frame instead
    std::optional<Drawing::Point> offset;
    if (parent != nullptr && !IsInstanceOf<RSSurfaceRenderNode>()) {
        auto& properties = parent->GetRenderProperties();
        offset = Drawing::Point { properties.GetFrameOffsetX(), properties.GetFrameOffsetY() };
    }
    // in some case geodirty_ is not marked in drawCmdModifiers_, we should update node geometry
    // [planing] using drawcmdModifierDirty from dirtyType_
    parentDirty = parentDirty || (dirtyStatus_ != NodeDirty::CLEAN);
    auto parentProperties = parent ? &parent->GetRenderProperties() : nullptr;
    bool dirty = GetMutableRenderProperties().UpdateGeometry(parentProperties, parentDirty, offset);
    if ((IsDirty() || dirty) && (drawCmdModifiers_.find(RSModifierType::GEOMETRYTRANS) != drawCmdModifiers_.end())) {
        RSModifierContext context = { GetMutableRenderProperties() };
        for (auto& modifier : drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
            modifier->Apply(context);
        }
    }
    isDirtyRegionUpdated_ = false;
    isLastVisible_ = ShouldPaint();
    GetMutableRenderProperties().ResetDirty();

    // Note:
    // 1. cache manager will use dirty region to update cache validity, background filter cache manager should use
    // 'dirty region of all the nodes drawn before this node', and foreground filter cache manager should use 'dirty
    // region of all the nodes drawn before this node, this node, and the children of this node'
    // 2. Filter must be valid when filter cache manager is valid, we make sure that in RSRenderNode::ApplyModifiers().
    if (GetRenderProperties().GetBackgroundFilter()) {
        UpdateFilterCacheWithBelowDirty(Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()));
    }
    UpdateDirtyRegion(dirtyManager, dirty, clipRect);
    return dirty;
}

bool RSRenderNode::UpdateBufferDirtyRegion(RectI& dirtyRect, const RectI& drawRegion)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (GetType() != RSRenderNodeType::SURFACE_NODE) {
        return false;
    }
    auto surfaceNode = ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode == nullptr) {
        return false;
    }
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto buffer = surfaceHandler->GetBuffer();
    if (buffer != nullptr) {
        // Use the matrix from buffer to relative coordinate and the absolute matrix
        // to calculate the buffer damageRegion's absolute rect
        auto rect = surfaceHandler->GetDamageRegion();
        auto matrix = surfaceNode->GetBufferRelMatrix();
        matrix.PostConcat(GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix());
        auto bufferDirtyRect = GetRenderProperties().GetBoundsGeometry()->MapRect(
            RectF(rect.x, rect.y, rect.w, rect.h), matrix);
        bufferDirtyRect.JoinRect(drawRegion);
        // The buffer's dirtyRect should not be out of the scope of the node's dirtyRect
        dirtyRect = bufferDirtyRect.IntersectRect(dirtyRect);
        RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode id: %" PRIu64 ", buffer size [%d,%d], "
            "buffer damageRegion [%d,%d,%d,%d], dirtyRect %s", GetId(),
            buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
            rect.x, rect.y, rect.w, rect.h, dirtyRect.ToString().c_str());
        return true;
    }
#endif
    return false;
}

void RSRenderNode::UpdateDirtyRegion(
    RSDirtyRegionManager& dirtyManager, bool geoDirty, const std::optional<RectI>& clipRect)
{
    if (!IsDirty() && !geoDirty) {
        return;
    }
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        // while node absRect not change and other content not change, return directly for not generate dirty region
        if (!IsSelfDrawingNode() && !geometryChangeNotPerceived_ && !geoDirty) {
            return;
        }
        geometryChangeNotPerceived_ = false;
    }
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    // merge old dirty if switch to invisible
    if (!ShouldPaint() && isLastVisible_) {
        ROSEN_LOGD("RSRenderNode:: id %{public}" PRIu64 " UpdateDirtyRegion visible->invisible", GetId());
    } else {
        RectI drawRegion;
        RectI shadowRect;
        auto& properties = GetRenderProperties();
        auto dirtyRect = properties.GetDirtyRect(drawRegion);
        auto rectFromRenderProperties = dirtyRect;
        // When surface node with buffer has damageRegion, use this instead of the node size
        if (UpdateBufferDirtyRegion(dirtyRect, drawRegion)) {
            // Add node's last and current frame absRect when the node size change
            if (rectFromRenderProperties != oldRectFromRenderProperties_) {
                dirtyManager.MergeDirtyRect(oldRectFromRenderProperties_);
                dirtyRect.JoinRect(rectFromRenderProperties);
                oldRectFromRenderProperties_ = rectFromRenderProperties;
            }
        }

        // Add node's shadow region to dirtyRect
        if (properties.IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                const RectF absBounds = {0, 0, properties.GetBoundsWidth(),
                    properties.GetBoundsHeight()};
                RRect absClipRRect = RRect(absBounds, properties.GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, properties, &absClipRRect);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, properties);
            }
            if (!shadowRect.IsEmpty()) {
                // Avoid deviation caused by converting float to int
                shadowRect = shadowRect.MakeOutset({1, 1, 1, 1});
                dirtyRect = dirtyRect.JoinRect(shadowRect);
            }
        }

        // Add node's outline region to dirtyRect
        auto& outline = properties.GetOutline();
        RectI outlineRect;
        if (outline && outline->HasBorder()) {
            RSPropertiesPainter::GetOutlineDirtyRect(outlineRect, properties);
            if (!outlineRect.IsEmpty()) {
                dirtyRect = dirtyRect.JoinRect(outlineRect);
            }
        }

        // Add node's pixelStretch region to dirtyRect
        if (properties.pixelStretch_) {
            auto stretchDirtyRect = properties.GetPixelStretchDirtyRect();
            dirtyRect = dirtyRect.JoinRect(stretchDirtyRect);
        }

        // Add node's foregroundEffect region to dirtyRect
        std::shared_ptr<RSFilter> foregroundFilter = nullptr;
        if (RSProperties::IS_UNI_RENDER) {
            foregroundFilter = properties.GetForegroundFilterCache();
        } else {
            foregroundFilter = properties.GetForegroundFilter();
        }
        if (foregroundFilter && foregroundFilter->GetFilterType() == RSFilter::FOREGROUND_EFFECT) {
            float dirtyExtension =
                std::static_pointer_cast<RSForegroundEffectFilter>(foregroundFilter)->GetDirtyExtension();
            dirtyRect = dirtyRect.MakeOutset(Vector4<int>(dirtyExtension));
        }

        if (clipRect.has_value()) {
            dirtyRect = dirtyRect.IntersectRect(*clipRect);
        }
        oldDirty_ = dirtyRect;
        oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
        // filter invalid dirtyrect
        if (!dirtyRect.IsEmpty()) {
            dirtyManager.MergeDirtyRect(dirtyRect);
            isDirtyRegionUpdated_ = true;
            // save types of dirty region of target dirty manager for dfx
            if (dirtyManager.IsTargetForDfx() &&
                (GetType() == RSRenderNodeType::CANVAS_NODE || GetType() == RSRenderNodeType::SURFACE_NODE)) {
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OVERLAY_RECT, drawRegion);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::SHADOW_RECT, shadowRect);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::PREPARE_CLIP_RECT, clipRect.value_or(RectI()));
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::RENDER_PROPERTIES_RECT, rectFromRenderProperties);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OUTLINE_RECT, outlineRect);
            }
        }
    }

    SetClean();
}

bool RSRenderNode::IsSelfDrawingNode() const
{
    return GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE || GetType() == RSRenderNodeType::SURFACE_NODE;
}

bool RSRenderNode::IsDirty() const
{
    return dirtyStatus_ != NodeDirty::CLEAN || GetRenderProperties().IsDirty();
}

bool RSRenderNode::IsSubTreeDirty() const
{
    return isSubTreeDirty_;
}

void RSRenderNode::SetSubTreeDirty(bool val)
{
    isSubTreeDirty_ = val;
}

void RSRenderNode::SetParentSubTreeDirty()
{
    auto parentNode = parent_.lock();
    if (parentNode && !parentNode->IsSubTreeDirty()) {
        parentNode->SetSubTreeDirty(true);
        parentNode->SetParentSubTreeDirty();
    }
}

bool RSRenderNode::IsContentDirty() const
{
    // Considering renderNode, it should consider both basenode's case and its properties
    return isContentDirty_ || GetRenderProperties().IsContentDirty();
}

void RSRenderNode::UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled)
{
    auto dirtyRect = GetRenderProperties().GetDirtyRect();
    // should judge if there's any child out of parent
    if (!isPartialRenderEnabled || HasChildrenOutOfRect()) {
        isRenderUpdateIgnored_ = false;
    } else if (dirtyRegion.IsEmpty() || dirtyRect.IsEmpty()) {
        isRenderUpdateIgnored_ = true;
    } else {
        RectI intersectRect = dirtyRegion.IntersectRect(dirtyRect);
        isRenderUpdateIgnored_ = intersectRect.IsEmpty();
    }
}

void RSRenderNode::MapAndUpdateChildrenRect()
{
    auto geoPtr = GetRenderProperties().GetBoundsGeometry();
    if (!shouldPaint_ || geoPtr == nullptr) {
        return;
    }
    auto childRect = selfDrawRect_;
    // all child must map to its direct parent
    if (!childrenRect_.IsEmpty()) {
        // clean subtree means childrenRect maps to parent already
        childRect = childRect.JoinRect(childrenRect_.ConvertTo<float>());
    }
    // map before update parent, if parent has clip property, use clipped children rect instead.
    // node with sharedTransitionParam should recalculate childRelativeToParentMatrix from absMatrix due to sandbox.
    if (auto parentNode = parent_.lock()) {
        const auto& parentProperties = parentNode->GetRenderProperties();
        const auto& sandbox = GetRenderProperties().GetSandBox();
        RectI childRectMapped;
        if (LIKELY(!sandbox.has_value())) {
            childRectMapped = geoPtr->MapRect(childRect, geoPtr->GetMatrix());
        } else {
            Drawing::Matrix invertAbsParentMatrix;
            const auto& parentGeoPtr = parentProperties.GetBoundsGeometry();
            if (parentGeoPtr && parentGeoPtr->GetAbsMatrix().Invert(invertAbsParentMatrix)) {
                auto childRelativeToParentMatrix = geoPtr->GetAbsMatrix();
                childRelativeToParentMatrix.PostConcat(invertAbsParentMatrix);
                childRectMapped = geoPtr->MapRect(childRect, childRelativeToParentMatrix);
            } else {
                childRectMapped = geoPtr->MapRect(childRect, geoPtr->GetMatrix());
            }
        }
        if (parentProperties.GetClipToBounds() || parentProperties.GetClipToFrame()) {
            childRectMapped = parentNode->GetSelfDrawRect().ConvertTo<int>().IntersectRect(childRectMapped);
        }
        parentNode->UpdateChildrenRect(childRectMapped);
        // check each child is inside of parent
        childRect = childRectMapped.ConvertTo<float>();
        if (!childRect.IsInsideOf(parentNode->GetSelfDrawRect())) {
            parentNode->UpdateChildrenOutOfRectFlag(true);
        }
    }
}

void RSRenderNode::UpdateParentChildrenRect(std::shared_ptr<RSRenderNode> parentNode) const
{
    if (!shouldPaint_ || (oldDirty_.IsEmpty() && GetChildrenRect().IsEmpty())) {
        return;
    }
    if (parentNode) {
        // accumulate current node's all children region(including itself)
        // apply oldDirty_ as node's real region(including overlay and shadow)
        RectI accumulatedRect = GetChildrenRect().JoinRect(oldDirty_);
        parentNode->UpdateChildrenRect(accumulatedRect);
        // check each child is inside of parent
        if (!accumulatedRect.IsInsideOf(parentNode->GetOldDirty())) {
            parentNode->UpdateChildrenOutOfRectFlag(true);
        }
    }
}

bool RSRenderNode::IsFilterCacheValid() const
{
#ifdef RS_ENABLE_GPU
    if (!RSSystemProperties::GetBlurEnabled() || !RSProperties::filterCacheEnabled_) {
        ROSEN_LOGD("IsBackgroundFilterCacheValid::blur is disabled or filter cache is disabled.");
        return false;
    }
    auto filterDrawable = GetRenderProperties().GetFilter() != nullptr ?
        GetFilterDrawable(true) : GetFilterDrawable(false);
    if (filterDrawable == nullptr) {
        return false;
    }
    return filterDrawable->IsFilterCacheValid();
#endif
    return false;
}

bool RSRenderNode::IsAIBarFilter() const
{
    if (!RSSystemProperties::GetBlurEnabled() || !RSProperties::filterCacheEnabled_) {
        ROSEN_LOGD("blur is disabled or filter cache is disabled.");
        return false;
    }
    auto filterDrawable = GetRenderProperties().GetFilter() != nullptr ?
        GetFilterDrawable(true) : GetFilterDrawable(false);
    if (filterDrawable == nullptr) {
        return false;
    }
    return filterDrawable->IsAIBarFilter();
}

bool RSRenderNode::IsAIBarFilterCacheValid() const
{
#ifdef RS_ENABLE_GPU
    if (!RSSystemProperties::GetBlurEnabled() || !RSProperties::filterCacheEnabled_) {
        ROSEN_LOGD("IsBackgroundFilterCacheValid::blur is disabled or filter cache is disabled.");
        return false;
    }
    auto filterDrawable = GetRenderProperties().GetFilter() != nullptr ?
        GetFilterDrawable(true) : GetFilterDrawable(false);
    if (filterDrawable == nullptr) {
        return false;
    }
    return filterDrawable->IsAIBarCacheValid();
#endif
return false;
}

const RectI RSRenderNode::GetFilterCachedRegion() const
{
    return lastFilterRegion_;
}

bool RSRenderNode::HasBlurFilter() const
{
    return GetRenderProperties().GetBackgroundFilter() || GetRenderProperties().GetFilter();
}

void RSRenderNode::UpdateLastFilterCacheRegion()
{
    lastFilterRegion_ = filterRegion_;
}

bool RSRenderNode::GetAbsMatrixReverse(const RSRenderNode& rootNode, Drawing::Matrix& absMatrix)
{
    auto& rootProperties = rootNode.GetRenderProperties();
    auto rootGeo = rootProperties.GetBoundsGeometry();
    auto selfGeo = GetRenderProperties().GetBoundsGeometry();
    if (!rootGeo || !selfGeo) {
        return false;
    }
    Drawing::Matrix selfMatrix = selfGeo->GetMatrix();
    auto directParent = GetParent().lock();
    while (directParent && directParent->GetId() != rootNode.GetId()) {
        if (auto parentGeo = directParent->GetRenderProperties().GetBoundsGeometry()) {
            selfMatrix.PostConcat(parentGeo->GetMatrix());
        }
        directParent = directParent->GetParent().lock();
    }
    if (!directParent) {
        return false;
    }
    selfMatrix.PostConcat(rootGeo->GetAbsMatrix());
    absMatrix = selfMatrix;
    return true;
}

void RSRenderNode::UpdateFilterRegionInSkippedSubTree(RSDirtyRegionManager& dirtyManager,
    const RSRenderNode& subTreeRoot, RectI& filterRect, const RectI& clipRect)
{
    Drawing::Matrix absMatrix;
    if (!GetAbsMatrixReverse(subTreeRoot, absMatrix)) {
        return;
    }
    absDrawRect_ = RSObjAbsGeometry::MapRect(selfDrawRect_, absMatrix);
    oldDirtyInSurface_ = absDrawRect_.IntersectRect(clipRect);
    filterRect = RSObjAbsGeometry::MapRect(GetRenderProperties().GetBoundsRect(), absMatrix);
    filterRect = filterRect.IntersectRect(clipRect);
    filterRegion_ = filterRect;
    if (filterRect == lastFilterRegion_) {
        return;
    }
    dirtyManager.MergeDirtyRect(filterRect);
    isDirtyRegionUpdated_ = true;
}

void RSRenderNode::CheckBlurFilterCacheNeedForceClearOrSave(bool rotationChanged, bool rotationStatusChanged)
{
#ifdef RS_ENABLE_GPU
    bool rotationClear = false;
    if (!IsInstanceOf<RSEffectRenderNode>() && rotationChanged) {
        rotationClear = true;
    }
    const auto& properties = GetRenderProperties();
    if (properties.GetBackgroundFilter()) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            auto bgDirty = dirtySlots_.count(RSDrawableSlot::BACKGROUND_COLOR) ||
                dirtySlots_.count(RSDrawableSlot::BACKGROUND_SHADER) ||
                dirtySlots_.count(RSDrawableSlot::BACKGROUND_IMAGE);
            if (!(filterDrawable->IsForceClearFilterCache()) && (rotationClear || bgDirty)) {
                RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode[%llu] background color or shader or image is dirty", GetId());
                filterDrawable->MarkFilterForceClearCache();
            }
        }
    }
    if (IsInstanceOf<RSEffectRenderNode>()) {
        rotationStatusChanged = false;
    }
    if (properties.GetFilter()) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            if (!(filterDrawable->IsForceClearFilterCache()) && (rotationStatusChanged || !dirtySlots_.empty())) {
                RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode[%llu] foreground is dirty", GetId());
                filterDrawable->MarkFilterForceClearCache();
            }
        }
    }
#endif
}

void RSRenderNode::MarkFilterInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
#ifdef RS_ENABLE_GPU
    const auto& properties = GetRenderProperties();
    if (properties.GetBackgroundFilter()) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkInForegroundFilterAndCheckNeedForceClearCache(offscreenCanvasNodeId);
        }
    }

    if (properties.GetFilter()) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkInForegroundFilterAndCheckNeedForceClearCache(offscreenCanvasNodeId);
        }
    }
#endif
}

#ifdef RS_ENABLE_GPU
bool RSRenderNode::IsForceClearOrUseFilterCache(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable)
{
    return filterDrawable->IsForceUseFilterCache() || filterDrawable->IsForceClearFilterCache();
}
#endif
void RSRenderNode::MarkFilterStatusChanged(bool isForeground, bool isFilterRegionChanged)
{
#ifdef RS_ENABLE_GPU
    auto filterDrawable = GetFilterDrawable(isForeground);
    if (filterDrawable == nullptr) {
        return;
    }
    auto& flag = isForeground ?
        (isFilterRegionChanged ? foregroundFilterRegionChanged_ : foregroundFilterInteractWithDirty_) :
        (isFilterRegionChanged ? backgroundFilterRegionChanged_ : backgroundFilterInteractWithDirty_);
    flag = true;
    isFilterRegionChanged ?
        filterDrawable->MarkFilterRegionChanged() : filterDrawable->MarkFilterRegionInteractWithDirty();
#endif
}
#ifdef RS_ENABLE_GPU
std::shared_ptr<DrawableV2::RSFilterDrawable> RSRenderNode::GetFilterDrawable(bool isForeground) const
{
    auto slot = isForeground ? RSDrawableSlot::COMPOSITING_FILTER : RSDrawableSlot::BACKGROUND_FILTER;
    if (auto& drawable = drawableVec_[static_cast<uint32_t>(slot)]) {
        if (auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable)) {
            return filterDrawable;
        }
    }
    return nullptr;
}
#endif
void RSRenderNode::UpdateFilterCacheWithBackgroundDirty()
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSProperties::filterCacheEnabled_) {
        return;
    }
    auto filterDrawable = GetFilterDrawable(false);
    if (filterDrawable == nullptr || IsForceClearOrUseFilterCache(filterDrawable)) {
        return;
    }
    auto hasBackground = drawableVec_[static_cast<int32_t>(RSDrawableSlot::BACKGROUND_COLOR)] ||
                         drawableVec_[static_cast<int32_t>(RSDrawableSlot::BACKGROUND_SHADER)] ||
                         drawableVec_[static_cast<int32_t>(RSDrawableSlot::BACKGROUND_IMAGE)];
    auto alphaDirty = dirtyTypes_.test(static_cast<size_t>(RSModifierType::ALPHA));
    if (alphaDirty && hasBackground) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSRenderNode[%llu] background color or shader or image is dirty due to changes in alpha", GetId());
        filterDrawable->MarkFilterForceClearCache();
    }
#endif
}

bool RSRenderNode::UpdateFilterCacheWithBelowDirty(const Occlusion::Region& belowDirty, bool isForeground)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSProperties::filterCacheEnabled_) {
        ROSEN_LOGE("RSRenderNode::UpdateFilterCacheWithBelowDirty filter cache is disabled.");
        return false;
    }
    auto filterDrawable = GetFilterDrawable(isForeground);
    if (filterDrawable == nullptr || IsForceClearOrUseFilterCache(filterDrawable)) {
        return false;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateFilterCacheWithBelowDirty:node[%llu] foreground:%d, lastRect:%s, dirtyRegion:%s",
        GetId(), isForeground, lastFilterRegion_.ToString().c_str(), belowDirty.GetRegionInfo().c_str());
    if (!belowDirty.IsIntersectWith(lastFilterRegion_)) {
        return false;
    }
    MarkFilterStatusChanged(isForeground, false);
    return true;
#else
    return false;
#endif
}

void RSRenderNode::UpdateFilterCacheWithSelfDirty()
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSProperties::filterCacheEnabled_) {
        ROSEN_LOGE("RSRenderNode::UpdateFilterCacheWithSelfDirty filter cache is disabled.");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] UpdateFilterCacheWithSelfDirty lastRect:%s, currRegion:%s",
        GetId(), lastFilterRegion_.ToString().c_str(), filterRegion_.ToString().c_str());
    const auto& properties = GetRenderProperties();
    if ((properties.GetBackgroundFilter() && !filterRegion_.IsInsideOf(lastFilterRegion_)) ||
        (properties.GetNeedDrawBehindWindow() && filterRegion_ != lastFilterRegion_)) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            if (!IsForceClearOrUseFilterCache(filterDrawable)) {
                MarkFilterStatusChanged(false, true);
            }
        }
    }
    if (properties.GetFilter() && filterRegion_ != lastFilterRegion_) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            if (!IsForceClearOrUseFilterCache(filterDrawable)) {
                MarkFilterStatusChanged(true, true);
            }
        }
    }
#endif
}

bool RSRenderNode::IsBackgroundInAppOrNodeSelfDirty() const
{
    return backgroundFilterInteractWithDirty_ || backgroundFilterRegionChanged_;
}

void RSRenderNode::UpdateDirtySlotsAndPendingNodes(RSDrawableSlot slot)
{
    dirtySlots_.emplace(slot);
    AddToPendingSyncList();
}
#ifdef RS_ENABLE_GPU
inline static bool IsLargeArea(int width, int height)
{
    static const auto threshold = RSSystemProperties::GetFilterCacheSizeThreshold();
    return width > threshold && height > threshold;
}
#endif
void RSRenderNode::PostPrepareForBlurFilterNode(RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync)
{
#ifdef RS_ENABLE_GPU
    MarkFilterHasEffectChildren();
    if (!RSProperties::filterCacheEnabled_) {
        ROSEN_LOGE("RSRenderNode::PostPrepareForBlurFilterNode filter cache is disabled.");
        return;
    }
    const auto& properties = GetRenderProperties();
    if (properties.GetBackgroundFilter() || properties.GetNeedDrawBehindWindow()) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            MarkFilterCacheFlags(filterDrawable, dirtyManager, needRequestNextVsync);
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, RSDrawableSlot::BACKGROUND_FILTER);
        }
    }
    if (properties.GetFilter()) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            MarkFilterCacheFlags(filterDrawable, dirtyManager, needRequestNextVsync);
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, RSDrawableSlot::COMPOSITING_FILTER);
        }
    }
    OnFilterCacheStateChanged();
    UpdateLastFilterCacheRegion();
#endif
}
#ifdef RS_ENABLE_GPU
void RSRenderNode::MarkFilterCacheFlags(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
    RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync)
{
    if (IsForceClearOrUseFilterCache(filterDrawable)) {
        return;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("MarkFilterCacheFlags:node[%llu], NeedPendingPurge:%d,"
        " forceClearWithoutNextVsync:%d, filterRegion: %s",
        GetId(), filterDrawable->NeedPendingPurge(), (!needRequestNextVsync && filterDrawable->IsSkippingFrame()),
        filterRegion_.ToString().c_str());
    ROSEN_LOGD("MarkFilterCacheFlags:node[%{public}" PRIu64 "], NeedPendingPurge:%{public}d,"
        " forceClearWithoutNextVsync:%{public}d, filterRegion: %{public}s",
        GetId(), filterDrawable->NeedPendingPurge(), (!needRequestNextVsync && filterDrawable->IsSkippingFrame()),
        filterRegion_.ToString().c_str());
    // force update if last frame use cache because skip-frame and current frame background is not dirty
    if (filterDrawable->NeedPendingPurge()) {
        dirtyManager.MergeDirtyRect(filterRegion_);
        isDirtyRegionUpdated_ = true;
        return;
    }
    // force update if no next vsync when skip-frame enabled
    if (!needRequestNextVsync && filterDrawable->IsSkippingFrame()) {
        filterDrawable->MarkForceClearCacheWithLastFrame();
        return;
    }

    // when background changed, skip-frame will enabled if filter region > 400 and blur radius > 25
    if (IsLargeArea(filterRegion_.GetWidth(), filterRegion_.GetHeight())) {
        filterDrawable->MarkFilterRegionIsLargeArea();
    }
}

void RSRenderNode::CheckFilterCacheAndUpdateDirtySlots(
    std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable, RSDrawableSlot slot)
{
    if (filterDrawable == nullptr) {
        return;
    }
    filterDrawable->MarkNeedClearFilterCache();
    UpdateDirtySlotsAndPendingNodes(slot);
}
#endif
void RSRenderNode::MarkForceClearFilterCacheWithInvisible()
{
#ifdef RS_ENABLE_GPU
    if (GetRenderProperties().GetBackgroundFilter()) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkFilterForceClearCache();
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, RSDrawableSlot::BACKGROUND_FILTER);
        }
    }
    if (GetRenderProperties().GetFilter()) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkFilterForceClearCache();
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, RSDrawableSlot::COMPOSITING_FILTER);
        }
    }
#endif
}

void RSRenderNode::SetOccludedStatus(bool occluded)
{
#ifdef RS_ENABLE_GPU
    if (GetRenderProperties().GetBackgroundFilter()) {
        auto filterDrawable = GetFilterDrawable(false);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkNodeIsOccluded(occluded);
        }
    }
    if (GetRenderProperties().GetFilter()) {
        auto filterDrawable = GetFilterDrawable(true);
        if (filterDrawable != nullptr) {
            filterDrawable->MarkNodeIsOccluded(occluded);
        }
    }
    isOccluded_ = occluded;
#endif
}

void RSRenderNode::RenderTraceDebug() const
{
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(GetId(),
            std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry()));
        RSPropertyTrace::GetInstance().TracePropertiesByNodeName(GetId(), GetNodeName(), GetRenderProperties());
    }
}

void RSRenderNode::DrawPropertyDrawable(RSDrawableSlot slot, RSPaintFilterCanvas& canvas)
{
    auto& drawablePtr = drawableVec_[static_cast<size_t>(slot)];
    if (!drawablePtr) {
        return;
    }

    auto recordingCanvas = canvas.GetRecordingCanvas();
    if (recordingCanvas == nullptr || !canvas.GetRecordDrawable()) {
        // non-recording canvas, draw directly
        Drawing::Rect rect = { 0, 0, GetRenderProperties().GetFrameWidth(), GetRenderProperties().GetFrameHeight() };
        drawablePtr->OnSync();
        drawablePtr->CreateDrawFunc()(&canvas, &rect);
        return;
    }

    auto castRecordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas.GetRecordingCanvas());
    auto drawFunc = [sharedPtr = shared_from_this(), slot](Drawing::Canvas* canvas, const Drawing::Rect* rect) -> void {
        if (auto canvasPtr = static_cast<RSPaintFilterCanvas*>(canvas)) {
            sharedPtr->DrawPropertyDrawable(slot, *canvasPtr);
        }
    };
    // recording canvas, record lambda that draws the drawable
    castRecordingCanvas->DrawDrawFunc(std::move(drawFunc));
}

void RSRenderNode::DrawPropertyDrawableRange(RSDrawableSlot begin, RSDrawableSlot end, RSPaintFilterCanvas& canvas)
{
    auto recordingCanvas = canvas.GetRecordingCanvas();
    if (recordingCanvas == nullptr || !canvas.GetRecordDrawable()) {
        // non-recording canvas, draw directly
        Drawing::Rect rect = { 0, 0, GetRenderProperties().GetFrameWidth(), GetRenderProperties().GetFrameHeight() };
        std::for_each(drawableVec_.begin() + static_cast<size_t>(begin),
            drawableVec_.begin() + static_cast<size_t>(end) + 1, // +1 since we need to include end
            [&](auto& drawablePtr) {
                if (drawablePtr) {
                    drawablePtr->OnSync();
                    drawablePtr->CreateDrawFunc()(&canvas, &rect);
                }
            });
        return;
    }

    auto castRecordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas.GetRecordingCanvas());
    auto drawFunc = [sharedPtr = shared_from_this(), begin, end](
                        Drawing::Canvas* canvas, const Drawing::Rect* rect) -> void {
        if (auto canvasPtr = static_cast<RSPaintFilterCanvas*>(canvas)) {
            sharedPtr->DrawPropertyDrawableRange(begin, end, *canvasPtr);
        }
    };
    // recording canvas, record lambda that draws the drawable
    castRecordingCanvas->DrawDrawFunc(std::move(drawFunc));
}

void RSRenderNode::ApplyAlphaAndBoundsGeometry(RSPaintFilterCanvas& canvas)
{
    canvas.ConcatMatrix(GetRenderProperties().GetBoundsGeometry()->GetMatrix());
    auto alpha = GetRenderProperties().GetAlpha();
    if (alpha == 1) {
        return;
    }
    if (GetRenderProperties().GetAlphaOffscreen()) {
        auto rect = RSPropertiesPainter::Rect2DrawingRect(GetRenderProperties().GetBoundsRect());
        Drawing::Brush brush;
        brush.SetAlpha(std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
        Drawing::SaveLayerOps slr(&rect, &brush);
        canvas.SaveLayer(slr);
        return;
    }
    canvas.MultiplyAlpha(alpha);
}

void RSRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSDrawableSlot::SAVE_ALL, RSDrawableSlot::MASK, canvas);
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSDrawableSlot::RESTORE_ALL, canvas);
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSDrawableSlot::RESTORE_ALL, canvas);
}

void RSRenderNode::SetUifirstSyncFlag(bool needSync)
{
    uifirstNeedSync_ = needSync;
}

std::shared_ptr<RSRenderPropertyBase> RSRenderNode::GetProperty(PropertyId id)
{
    auto it = properties_.find(id);
    if (it == properties_.end()) {
        return nullptr;
    }
    return it->second;
}

void RSRenderNode::AddProperty(std::shared_ptr<RSRenderPropertyBase> property)
{
    properties_.emplace(property->GetId(), property);
}

void RSRenderNode::RemoveProperty(std::shared_ptr<RSRenderPropertyBase> property)
{
    properties_.erase(property->GetId());
}

void RSRenderNode::AddModifier(const std::shared_ptr<RSRenderModifier>& modifier, bool isSingleFrameComposer)
{
    if (!modifier) {
        ROSEN_LOGW("RSRenderNode: null modifier add failed.");
        return;
    }
    SetDirty();
    if (RSSystemProperties::GetSingleFrameComposerEnabled() &&
        GetNodeIsSingleFrameComposer() && isSingleFrameComposer) {
        if (singleFrameComposer_ == nullptr) {
            singleFrameComposer_ = std::make_shared<RSSingleFrameComposer>();
        }
        singleFrameComposer_->SingleFrameAddModifier(modifier);
        ROSEN_LOGI_IF(DEBUG_MODIFIER, "RSRenderNode:add modifier for single frame, node id: %{public}" PRIu64 ","
            "type: %{public}s, cmdList: %{public}s",
            GetId(), modifier->GetModifierTypeString().c_str(), std::to_string(modifier->GetDrawCmdListId()).c_str());
        return;
    }
    auto modifierType = modifier->GetType();
    if (modifierType == RSModifierType::BOUNDS || modifierType == RSModifierType::FRAME) {
        AddGeometryModifier(modifier);
    } else if (modifierType == RSModifierType::BACKGROUND_UI_FILTER ||
        modifierType == RSModifierType::FOREGROUND_UI_FILTER) {
        AddUIFilterModifier(modifier);
    } else if (modifierType < RSModifierType::CUSTOM) {
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
        if (modifierType == RSModifierType::COMPLEX_SHADER_PARAM) {
            auto property = modifier->GetProperty();
            properties_.emplace(modifier->GetPropertyId(), property);
        }
    } else {
        modifier->SetSingleFrameModifier(false);
        drawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
    }
    modifier->GetProperty()->Attach(shared_from_this());
    ROSEN_LOGI_IF(DEBUG_MODIFIER, "RSRenderNode:add modifier, node id: %{public}" PRIu64 ", type: %{public}s",
        GetId(), modifier->GetModifierTypeString().c_str());
}

void RSRenderNode::AddUIFilterModifier(const std::shared_ptr<RSRenderModifier>& modifier)
{
    if (!modifier) {
        ROSEN_LOGW("RSRenderNode::AddUIFilterModifier: null modifier add failed.");
        return;
    }
    modifiers_.emplace(modifier->GetPropertyId(), modifier);
    auto renderProperty =
        std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(modifier->GetProperty());
    if (!renderProperty) {
        ROSEN_LOGW("RSRenderNode::AddUIFilterModifier: null renderProperty.");
        return;
    }
    auto& renderFilter = renderProperty->GetRef();
    for (auto& type : renderFilter->GetUIFilterTypes()) {
        auto propGroup = renderFilter->GetRenderFilterPara(type);
        if (!propGroup) {
            continue;
        }
        for (auto& prop : propGroup->GetLeafRenderProperties()) {
            if (prop) {
                prop->Attach(shared_from_this());
                properties_.emplace(prop->GetId(), prop);
            }
        }
    }
}

void RSRenderNode::AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier)
{
    // bounds and frame modifiers must be unique
    if (modifier->GetType() == RSModifierType::BOUNDS) {
        if (boundsModifier_ == nullptr) {
            boundsModifier_ = modifier;
        } else {
            boundsModifier_->Update(modifier->GetProperty(), false);
        }
        modifiers_.emplace(modifier->GetPropertyId(), boundsModifier_);
    }

    if (modifier->GetType() == RSModifierType::FRAME) {
        if (frameModifier_ == nullptr) {
            frameModifier_ = modifier;
        } else {
            frameModifier_->Update(modifier->GetProperty(), false);
        }
        modifiers_.emplace(modifier->GetPropertyId(), frameModifier_);
    }
}

void RSRenderNode::RemoveModifier(const PropertyId& id)
{
    SetDirty();
    auto it = modifiers_.find(id);
    if (it != modifiers_.end()) {
        if (it->second) {
            AddDirtyType(it->second->GetType());
        }
        ROSEN_LOGI_IF(DEBUG_MODIFIER, "RSRenderNode::remove modifier, node id: %{public}" PRIu64 ", type: %{public}s",
            GetId(), (it->second) ? it->second->GetModifierTypeString().c_str() : "UNKNOWN");
        it->second->GetProperty()->Detach(shared_from_this());
        modifiers_.erase(it);
        return;
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        bool found = EraseIf(modifiers,
            [id](const auto& modifier) -> bool { return modifier == nullptr || modifier->GetPropertyId() == id; });
        if (found) {
            AddDirtyType(type);
        }
    }
}

void RSRenderNode::RemoveAllModifiers()
{
    modifiers_.clear();
    properties_.clear();
    drawCmdModifiers_.clear();
}

void RSRenderNode::DumpNodeInfo(DfxString& log)
{
    // Drawing is not supported
}

void RSRenderNode::AccumulateDirtyInOcclusion(bool isOccluded)
{
    if (isOccluded) {
        // Accumulate dirtytypes for modifiers
        AccumulateDirtyTypes();
        // Accumulate dirtystatus in rendernode
        AccumulateDirtyStatus();
        // Accumulate dirtystatus in render properties(isDirty, geoDirty, contentDirty)
        GetMutableRenderProperties().AccumulateDirtyStatus();
        return;
    }
    ResetAccumulateDirtyTypes();
    ResetAccumulateDirtyStatus();
}

void RSRenderNode::RecordCurDirtyStatus()
{
    curDirtyStatus_ = dirtyStatus_;
    GetMutableRenderProperties().RecordCurDirtyStatus();
}

void RSRenderNode::AccumulateDirtyStatus()
{
    GetMutableRenderProperties().AccumulateDirtyStatus();
    if (curDirtyStatus_ == NodeDirty::CLEAN) {
        return;
    }
    SetDirty();
}

void RSRenderNode::ResetAccumulateDirtyStatus()
{
    dirtyStatus_ = NodeDirty::CLEAN;
    GetMutableRenderProperties().ResetDirty();
}

void RSRenderNode::RecordCurDirtyTypes()
{
    curDirtyTypes_ |= dirtyTypes_;
    curDirtyTypesNG_ |= dirtyTypesNG_;
}

void RSRenderNode::AccumulateDirtyTypes()
{
    dirtyTypes_ |= curDirtyTypes_;
    dirtyTypesNG_ |= curDirtyTypesNG_;
}

void RSRenderNode::ResetAccumulateDirtyTypes()
{
    dirtyTypes_.reset();
    dirtyTypesNG_.reset();
}

void RSRenderNode::ApplyPositionZModifier()
{
    constexpr auto positionZModifierType = static_cast<size_t>(RSModifierType::POSITION_Z);
    // TODO: modifier NG adaption
    if (!dirtyTypes_.test(positionZModifierType)) {
        return;
    }

    GetMutableRenderProperties().SetPositionZ(0.0f);
    RSModifierContext context = { GetMutableRenderProperties() };
    for (auto& [id, modifier] : modifiers_) {
        if (modifier->GetType() == RSModifierType::POSITION_Z) {
            modifier->Apply(context);
        }
    }

    dirtyTypes_.reset(positionZModifierType);
}

void RSRenderNode::SetChildHasSharedTransition(bool val)
{
    childHasSharedTransition_ = val;
}

bool RSRenderNode::ChildHasSharedTransition() const
{
    return childHasSharedTransition_;
}

void RSRenderNode::MarkForegroundFilterCache()
{
    if (GetRenderProperties().GetForegroundFilterCache() != nullptr) {
        MarkNodeGroup(NodeGroupType::GROUPED_BY_FOREGROUND_FILTER, true, true);
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_FOREGROUND_FILTER) { // clear foreground filter cache
        MarkNodeGroup(NodeGroupType::GROUPED_BY_FOREGROUND_FILTER, false, false);
    }
}

void RSRenderNode::ApplyModifier(RSModifierContext& context, std::shared_ptr<RSRenderModifier> modifier)
{
    auto modifierType = modifier->GetType();
    // TODO: modifier NG adaption
    if (!dirtyTypes_.test(static_cast<size_t>(modifierType))) {
        return;
    }
    modifier->Apply(context);
    isOnlyBasicGeoTransform_ = isOnlyBasicGeoTransform_ && BASIC_GEOTRANSFORM_ANIMATION_TYPE.count(modifierType);
}

CM_INLINE void RSRenderNode::ApplyModifiers()
{
    RS_LOGI_IF(DEBUG_NODE, "RSRenderNode::apply modifiers isFullChildrenListValid_:%{public}d"
        " isChildrenSorted_:%{public}d childrenHasSharedTransition_:%{public}d",
        isFullChildrenListValid_, isChildrenSorted_, childrenHasSharedTransition_);
    if (const auto& sharedTransitionParam = GetSharedTransitionParam()) {
        sharedTransitionParam->UpdateHierarchy(GetId());
        SharedTransitionParam::UpdateUnpairedSharedTransitionMap(sharedTransitionParam);
    }
    if (UNLIKELY(!isFullChildrenListValid_)) {
        GenerateFullChildrenList();
        AddDirtyType(RSModifierType::CHILDREN);
    } else if (UNLIKELY(!isChildrenSorted_)) {
        ResortChildren();
        AddDirtyType(RSModifierType::CHILDREN);
    } else if (UNLIKELY(childrenHasSharedTransition_)) {
        // if children has shared transition, force regenerate RSChildrenDrawable
        AddDirtyType(RSModifierType::CHILDREN);
    } else if (!RSRenderNode::IsDirty() || dirtyTypes_.none()) {
        // TODO: modifier NG adaption
        RS_LOGD("RSRenderNode::apply modifiers RSRenderNode's dirty is false or dirtyTypes_ is none");
        // clean node, skip apply
        return;
    }
    RecordCurDirtyTypes();
    // Reset and re-apply all modifiers
    RSModifierContext context = { GetMutableRenderProperties() };

    // Reset before apply modifiers
    // TODO: modifier NG adaption
    GetMutableRenderProperties().ResetProperty(dirtyTypes_);

    // Apply modifiers
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(shared_from_this());
    if (displayNode && displayNode->GetCurrentScbPid() != -1) {
        RS_LOGD("RSRenderNode::apply modifiers displayNode's currentScbPid:%{public}d",
            displayNode->GetCurrentScbPid());
        for (auto& [id, modifier] : modifiers_) {
            if (ExtractPid(id) == displayNode->GetCurrentScbPid()) {
                ApplyModifier(context, modifier);
            }
        }
    } else {
        RS_LOGD("RSRenderNode::apply modifiers displayNode is nullptr or displayNode's currentScbPid is -1");
        for (auto& [id, modifier] : modifiers_) {
            ApplyModifier(context, modifier);
        }
    }

    // execute hooks
    GetMutableRenderProperties().OnApplyModifiers();
    OnApplyModifiers();
    MarkForegroundFilterCache();
    UpdateShouldPaint();

    // TODO: modifier NG adaption
    if (dirtyTypes_.test(static_cast<size_t>(RSModifierType::USE_EFFECT)) ||
        dirtyTypes_.test(static_cast<size_t>(RSModifierType::USE_EFFECT_TYPE))) {
        ProcessBehindWindowAfterApplyModifiers();
    }

    RS_LOGI_IF(DEBUG_NODE,
        "RSRenderNode::apply modifiers RenderProperties's sandBox's hasValue is %{public}d"
        " isTextureExportNode_:%{public}d", GetRenderProperties().GetSandBox().has_value(),
        isTextureExportNode_);
    if (dirtyTypes_.test(static_cast<size_t>(RSModifierType::SANDBOX)) &&
        !GetRenderProperties().GetSandBox().has_value() && sharedTransitionParam_) {
        auto paramCopy = sharedTransitionParam_;
        paramCopy->InternalUnregisterSelf();
    }
    if (dirtyTypes_.test(static_cast<size_t>(RSModifierType::FOREGROUND_EFFECT_RADIUS)) ||
        dirtyTypes_.test(static_cast<size_t>(RSModifierType::BOUNDS))) {
        std::shared_ptr<RSFilter> foregroundFilter = nullptr;
        if (RSProperties::IS_UNI_RENDER) {
            foregroundFilter = GetRenderProperties().GetForegroundFilterCache();
        } else {
            foregroundFilter = GetRenderProperties().GetForegroundFilter();
        }
        if (foregroundFilter) {
            GetMutableRenderProperties().SetForegroundEffectDirty(true);
        }
    }

    // Temporary code, copy matrix into render params
    if (GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasdrawingnode = ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        canvasdrawingnode->CheckCanvasDrawingPostPlaybacked();
    }
    UpdateDrawableVecV2();

    UpdateFilterCacheWithBackgroundDirty();

    // Clear node some resource
    ClearResource();
    // update state
    dirtyTypes_.reset();
    dirtyTypesNG_.reset();
    AddToPendingSyncList();

    // update rate decider scale reference size and scale.
    animationManager_.SetRateDeciderSize(GetRenderProperties().GetBoundsWidth(),
        GetRenderProperties().GetBoundsHeight());
    animationManager_.SetRateDeciderScale(GetRenderProperties().GetScaleX(), GetRenderProperties().GetScaleY());
    auto& rect = GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    animationManager_.SetRateDeciderAbsRect(rect.GetWidth(), rect.GetHeight());
}

void RSRenderNode::MarkParentNeedRegenerateChildren() const
{
    auto parent = GetParent().lock();
    if (parent == nullptr) {
        return;
    }
    parent->isChildrenSorted_ = false;
}

int RSRenderNode::GetBlurEffectDrawbleCount()
{
    bool fgFilterValid = drawableVec_[static_cast<int32_t>(RSDrawableSlot::FOREGROUND_FILTER)] != nullptr;
    bool bgFilterValid = drawableVec_[static_cast<int32_t>(RSDrawableSlot::BACKGROUND_FILTER)] != nullptr;
    bool cpFilterValid = drawableVec_[static_cast<int32_t>(RSDrawableSlot::COMPOSITING_FILTER)] != nullptr;
    return static_cast<int>(fgFilterValid) + static_cast<int>(bgFilterValid) + static_cast<int>(cpFilterValid);
}

void RSRenderNode::UpdateDrawableVecV2()
{
#ifdef RS_ENABLE_GPU
    // Step 1: Collect dirty slots
    auto dirtySlots = RSDrawable::CalculateDirtySlots(dirtyTypes_, drawableVec_);
    if (dirtySlots.empty()) {
        RS_LOGD("RSRenderNode::update drawable VecV2 dirtySlots is empty");
        return;
    }
    auto preBlurDrawableCnt = GetBlurEffectDrawbleCount();
    // Step 2: Update or regenerate drawable if needed
    bool drawableChanged = RSDrawable::UpdateDirtySlots(*this, drawableVec_, dirtySlots);
    // Step 2.1 (optional): fuze some drawables
    RSDrawable::FuzeDrawableSlots(*this, drawableVec_);
    // If any drawable has changed, or the CLIP_TO_BOUNDS slot has changed, then we need to recalculate
    // save/clip/restore.
    RS_LOGI_IF(DEBUG_NODE,
        "RSRenderNode::update drawable VecV2 drawableChanged:%{public}d", drawableChanged);
    if (GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasdrawingnode = ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        drawableChanged |= canvasdrawingnode->GetIsPostPlaybacked();
    }
    if (drawableChanged || dirtySlots.count(RSDrawableSlot::CLIP_TO_BOUNDS)) {
        // Step 3: Recalculate save/clip/restore on demands
        RSDrawable::UpdateSaveRestore(*this, drawableVec_, drawableVecStatus_);
        // if shadow changed, update shadow rect
        UpdateShadowRect();
        UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::SHADOW);
        std::unordered_set<RSDrawableSlot> dirtySlotShadow;
        dirtySlotShadow.emplace(RSDrawableSlot::SHADOW);
        RSDrawable::UpdateDirtySlots(*this, drawableVec_, dirtySlotShadow);
        // Step 4: Generate drawCmdList from drawables
        UpdateDisplayList();
        UpdateBlurEffectCounter(GetBlurEffectDrawbleCount() - preBlurDrawableCnt);
    }
    // Merge dirty slots
    if (dirtySlots_.empty()) {
        dirtySlots_ = std::move(dirtySlots);
    } else {
        dirtySlots_.insert(dirtySlots.begin(), dirtySlots.end());
    }

    waitSync_ = true;
#endif
}

void RSRenderNode::UpdateShadowRect()
{
#ifdef RS_ENABLE_GPU
    if (drawableVec_[static_cast<int8_t>(RSDrawableSlot::SHADOW)] != nullptr &&
        GetRenderProperties().GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        RectI shadowRect;
        auto rRect = GetRenderProperties().GetRRect();
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &rRect, false, false);
        stagingRenderParams_->SetShadowRect(Drawing::Rect(
            static_cast<float>(shadowRect.GetLeft()),
            static_cast<float>(shadowRect.GetTop()),
            static_cast<float>(shadowRect.GetRight()),
            static_cast<float>(shadowRect.GetBottom())));
        RS_OPTIONAL_TRACE_NAME_FMT("UpdateShadowRect id:%llu shadowRect:%s",
            GetId(), shadowRect.ToString().c_str());
    } else {
        stagingRenderParams_->SetShadowRect(Drawing::Rect());
    }
#endif
}

void RSRenderNode::UpdateDisplayList()
{
#ifndef ROSEN_ARKUI_X
    // Planning: use the mask from DrawableVecStatus in rs_drawable.cpp
    constexpr uint8_t FRAME_NOT_EMPTY = 1 << 4;
#ifdef RS_ENABLE_GPU
    constexpr uint8_t NODE_NOT_EMPTY = 1 << 5;
#endif
    stagingDrawCmdList_.clear();
    drawCmdListNeedSync_ = true;
#ifdef RS_ENABLE_GPU
    if (UNLIKELY((drawableVecStatus_ & NODE_NOT_EMPTY) == 0)) {
        // NODE_NOT_EMPTY is not set, so nothing to draw, just skip
        stagingRenderParams_->SetContentEmpty(IsInstanceOf<RSCanvasRenderNode>());
        return;
    }
#endif

    int8_t index = 0;
    // Process all drawables in [index, end], end is included.
    // Note: After this loop, index will be end+1
    auto AppendDrawFunc = [&](RSDrawableSlot end) -> int8_t {
        auto endIndex = static_cast<int8_t>(end);
        for (; index <= endIndex; ++index) {
            if (const auto& drawable = drawableVec_[index]) {
                stagingDrawCmdList_.emplace_back(drawable->CreateDrawFunc());
            }
        }
        // If the end drawable exist, return its index, otherwise return -1
        return drawableVec_[endIndex] != nullptr ? stagingDrawCmdList_.size() - 1 : -1;
    };
    // Update index of ENV_FOREGROUND_COLOR
    stagingDrawCmdIndex_.envForeGroundColorIndex_ = AppendDrawFunc(RSDrawableSlot::ENV_FOREGROUND_COLOR);

    // Update index of SHADOW
    stagingDrawCmdIndex_.shadowIndex_ = AppendDrawFunc(RSDrawableSlot::SHADOW);

    AppendDrawFunc(RSDrawableSlot::OUTLINE);
    stagingDrawCmdIndex_.renderGroupBeginIndex_ = stagingDrawCmdList_.size();
    stagingDrawCmdIndex_.foregroundFilterBeginIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());

    // Update index of BACKGROUND_COLOR
    stagingDrawCmdIndex_.backgroundColorIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_COLOR);

    // Update index of BACKGROUND_IMAGE
    stagingDrawCmdIndex_.backgroundImageIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_IMAGE);

    // Update index of BACKGROUND_FILTER
    stagingDrawCmdIndex_.backgroundFilterIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_FILTER);

    // Update index of USE_EFFECT
    stagingDrawCmdIndex_.useEffectIndex_ = AppendDrawFunc(RSDrawableSlot::USE_EFFECT);

    AppendDrawFunc(RSDrawableSlot::BG_RESTORE_BOUNDS);

    if (drawableVecStatus_ & FRAME_NOT_EMPTY) {
        // Update index of CONTENT_STYLE
        stagingDrawCmdIndex_.contentIndex_ = AppendDrawFunc(RSDrawableSlot::CONTENT_STYLE);

        // Update index of BACKGROUND_END
        stagingDrawCmdIndex_.backgroundEndIndex_ = stagingDrawCmdIndex_.contentIndex_ == -1
            ? static_cast<int8_t>(stagingDrawCmdList_.size()) : stagingDrawCmdIndex_.contentIndex_;

        // Update index of CHILDREN
        stagingDrawCmdIndex_.childrenIndex_ = AppendDrawFunc(RSDrawableSlot::CHILDREN);
        stagingDrawCmdIndex_.foregroundBeginIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());

        AppendDrawFunc(RSDrawableSlot::RESTORE_FRAME);
    } else {
        // Nothing inside frame, skip useless slots and update indexes
        stagingDrawCmdIndex_.contentIndex_ = -1;
        stagingDrawCmdIndex_.childrenIndex_ = -1;
        stagingDrawCmdIndex_.backgroundEndIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());
        stagingDrawCmdIndex_.foregroundBeginIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());
        index = static_cast<int8_t>(RSDrawableSlot::FG_SAVE_BOUNDS);
    }
    stagingDrawCmdIndex_.renderGroupEndIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());

    AppendDrawFunc(RSDrawableSlot::RESTORE_BLENDER);
    stagingDrawCmdIndex_.foregroundFilterEndIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());
    AppendDrawFunc(RSDrawableSlot::RESTORE_ALL);
    stagingDrawCmdIndex_.endIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetContentEmpty(false);
#endif
#endif
}

void RSRenderNode::UpdateEffectRegion(std::optional<Drawing::RectI>& region, bool isForced)
{
    if (!region.has_value()) {
        return;
    }
    const auto& property = GetRenderProperties();
    if (!isForced && !property.GetUseEffect()) {
        return;
    }

    auto absRect = property.GetBoundsGeometry()->GetAbsRect();
    region->Join(Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()));
}

std::shared_ptr<RSRenderModifier> RSRenderNode::GetModifier(const PropertyId& id)
{
    if (modifiers_.count(id)) {
        return modifiers_[id];
    }
    for (const auto& [type, modifiers] : drawCmdModifiers_) {
        auto it = std::find_if(modifiers.begin(), modifiers.end(),
            [id](const auto& modifier) -> bool { return modifier->GetPropertyId() == id; });
        if (it != modifiers.end()) {
            return *it;
        }
    }
    return nullptr;
}

void RSRenderNode::FilterModifiersByPid(pid_t pid)
{
    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    EraseIf(modifiers_, [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });

    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if([pid](const auto& it) -> bool { return ExtractPid(it->GetPropertyId()) == pid; });
    }
}

void RSRenderNode::UpdateShouldPaint()
{
    // node should be painted if either it is visible or it has disappearing transition animation,
    // but only when its alpha is not zero.
    // Besides, if one node has sharedTransitionParam, it should be painted no matter what alpha it has.
    shouldPaint_ = ((ROSEN_GNE(GetRenderProperties().GetAlpha(), 0.0f)) &&
                   (GetRenderProperties().GetVisible() || HasDisappearingTransition(false))) ||
                   sharedTransitionParam_;
    if (!shouldPaint_ && HasBlurFilter()) { // force clear blur cache
        RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] is invisible", GetId());
        MarkForceClearFilterCacheWithInvisible();
    }
}

void RSRenderNode::SetSharedTransitionParam(const std::shared_ptr<SharedTransitionParam>& sharedTransitionParam)
{
    if (!sharedTransitionParam_ && !sharedTransitionParam) {
        // both are empty, do nothing
        return;
    }
    sharedTransitionParam_ = sharedTransitionParam;
    SetDirty();
    // tell parent to regenerate children drawable
    if (auto parent = parent_.lock()) {
        parent->AddDirtyType(RSModifierType::CHILDREN);
        parent->SetDirty();
    }
}

const std::shared_ptr<SharedTransitionParam>& RSRenderNode::GetSharedTransitionParam() const
{
    return sharedTransitionParam_;
}

void RSRenderNode::SetGlobalAlpha(float alpha)
{
    if (globalAlpha_ == alpha) {
        return;
    }
    if ((ROSEN_EQ(globalAlpha_, 1.0f) && !ROSEN_EQ(alpha, 1.0f)) ||
        (ROSEN_EQ(alpha, 1.0f) && !ROSEN_EQ(globalAlpha_, 1.0f))) {
        OnAlphaChanged();
    }
    globalAlpha_ = alpha;
#ifdef RS_ENABLE_GPU
    if (stagingRenderParams_) {
        stagingRenderParams_->SetGlobalAlpha(alpha);
    }
#endif
}

float RSRenderNode::GetGlobalAlpha() const
{
    return globalAlpha_;
}

void RSRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:%{public}" PRIu64 "isBootAnimation %{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;
}

bool RSRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

bool RSRenderNode::GetGlobalPositionEnabled() const
{
    return false;
}

bool RSRenderNode::NeedInitCacheSurface()
{
    auto cacheType = GetCacheType();
    int width = 0;
    int height = 0;
    if (cacheType == CacheType::ANIMATE_PROPERTY && GetRenderProperties().IsShadowValid() &&
        !GetRenderProperties().IsSpherizeValid() && !GetRenderProperties().IsAttractionValid()) {
        const RectF boundsRect = GetRenderProperties().GetBoundsRect();
        RRect rrect = RRect(boundsRect, {0, 0, 0, 0});
        RectI shadowRect;
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &rrect, false);
        width = shadowRect.GetWidth();
        height = shadowRect.GetHeight();
    } else {
        Vector2f size = GetOptionalBufferSize();
        width =  size.x_;
        height = size.y_;
    }
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

bool RSRenderNode::NeedInitCacheCompletedSurface()
{
    Vector2f size = GetOptionalBufferSize();
    int width = static_cast<int>(size.x_);
    int height = static_cast<int>(size.y_);
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheCompletedSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheCompletedSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

void RSRenderNode::InitCacheSurface(Drawing::GPUContext* gpuContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
{
    RS_TRACE_NAME_FMT("InitCacheSurface");
    if (func) {
        cacheSurfaceThreadIndex_ = threadIndex;
        if (!clearCacheSurfaceFunc_) {
            clearCacheSurfaceFunc_ = func;
        }
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (cacheSurface_) {
            func(std::move(cacheSurface_), nullptr,
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            cacheSurface_ = nullptr;
        }
    } else {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheSurface_ = nullptr;
    }
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        cacheCleanupHelper_ = nullptr;
    }
#endif
    auto cacheType = GetCacheType();
    float width = 0.0f, height = 0.0f;
    Vector2f size = GetOptionalBufferSize();
    boundsWidth_ = size.x_;
    boundsHeight_ = size.y_;
    if (cacheType == CacheType::ANIMATE_PROPERTY && GetRenderProperties().IsShadowValid() &&
        !GetRenderProperties().IsSpherizeValid() && !GetRenderProperties().IsAttractionValid()) {
        const RectF boundsRect = GetRenderProperties().GetBoundsRect();
        RRect rrect = RRect(boundsRect, {0, 0, 0, 0});
        RectI shadowRect;
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &rrect, false);
        width = shadowRect.GetWidth();
        height = shadowRect.GetHeight();
        shadowRectOffsetX_ = -shadowRect.GetLeft();
        shadowRectOffsetY_ = -shadowRect.GetTop();
    } else {
        width = std::ceil(boundsWidth_);
        height = std::ceil(boundsHeight_);
    }
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        if (func) {
            std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
            func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        return;
    }
#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(width, height);
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        auto initCacheBackendTexture = NativeBufferUtils::MakeBackendTexture(width, height, ExtractPid(GetId()));
        auto vkTextureInfo = initCacheBackendTexture.GetTextureInfo().GetVKTextureInfo();
        if (!initCacheBackendTexture.IsValid() || !vkTextureInfo) {
            if (func) {
                std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
                func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                    cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
                ClearCacheSurface();
            }
            return;
        }
        auto initCacheCleanupHelper = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory, vkTextureInfo->vkAlloc.statName);
        auto initCacheSurface = Drawing::Surface::MakeFromBackendTexture(
            gpuContext, initCacheBackendTexture.GetTextureInfo(), Drawing::TextureOrigin::BOTTOM_LEFT,
            1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
            NativeBufferUtils::DeleteVkImage, initCacheCleanupHelper);
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheBackendTexture_ = initCacheBackendTexture;
        cacheCleanupHelper_ = initCacheCleanupHelper;
        cacheSurface_ = initCacheSurface;
    }
#endif
#else
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = Drawing::Surface::MakeRasterN32Premul(width, height);
#endif
}

bool RSRenderNode::IsCacheSurfaceValid() const
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return  (cacheSurface_ != nullptr);
}

bool RSRenderNode::IsCacheCompletedSurfaceValid() const
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return  (cacheCompletedSurface_ != nullptr);
}

Vector2f RSRenderNode::GetOptionalBufferSize() const
{
    const auto& modifier = boundsModifier_ ? boundsModifier_ : frameModifier_;
    if (!modifier) {
        return {0.0f, 0.0f};
    }
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(modifier->GetProperty());
    auto vector4f = renderProperty->Get();
    // bounds vector4f: x y z w -> left top width height
    return { vector4f.z_, vector4f.w_ };
}

std::shared_ptr<Drawing::Image> RSRenderNode::GetCompletedImage(
    RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    if (isUIFirst) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (!cacheCompletedBackendTexture_.IsValid()) {
            RS_LOGE("invalid grBackendTexture_");
            return nullptr;
        }
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            if (!cacheCompletedSurface_ || !cacheCompletedCleanupHelper_) {
                return nullptr;
            }
        }
#endif
        if (canvas.GetGPUContext() == nullptr) {
            RS_LOGE("canvas GetGPUContext failed");
            return nullptr;
        }
        auto image = std::make_shared<Drawing::Image>();
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888,
            Drawing::ALPHATYPE_PREMUL };
#ifdef RS_ENABLE_GL
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr);
        }
#endif

#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr,
                NativeBufferUtils::DeleteVkImage, cacheCompletedCleanupHelper_->Ref());
        }
#endif
        return image;
#endif
    }

    if (!cacheCompletedSurface_) {
        RS_LOGE("DrawCacheSurface invalid cacheCompletedSurface");
        return nullptr;
    }
    auto completeImage = cacheCompletedSurface_->GetImageSnapshot();
    if (!completeImage) {
        RS_LOGE("Get complete image failed");
        return nullptr;
    }
    if (threadIndex == completedSurfaceThreadIndex_) {
        return completeImage;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = completeImage->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("get backendTexture failed");
        return nullptr;
    }
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ completeImage->GetColorType(), completeImage->GetAlphaType() };
    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("canvas GetGPUContext failed");
        return nullptr;
    }
    bool ret = cacheImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture.GetTextureInfo(),
        origin, info, nullptr);
    if (!ret) {
        RS_LOGE("RSRenderNode::GetCompletedImage image BuildFromTexture failed");
        return nullptr;
    }
    return cacheImage;
#else
    return completeImage;
#endif
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
void RSRenderNode::UpdateBackendTexture()
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return;
    }
    cacheBackendTexture_ = cacheSurface_->GetBackendTexture();
}
#endif

std::shared_ptr<Drawing::Surface> RSRenderNode::GetCompletedCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
#ifdef RS_ENABLE_VK
            if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
                OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
                cacheCompletedCleanupHelper_ = nullptr;
            }
#endif
            return cacheCompletedSurface_;
        }
        if (!needCheckThread || completedSurfaceThreadIndex_ == threadIndex || !cacheCompletedSurface_) {
            return cacheCompletedSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

void RSRenderNode::ClearCacheSurfaceInThread()
{
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_), cacheSurfaceThreadIndex_,
            completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

std::shared_ptr<Drawing::Surface> RSRenderNode::GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
#ifdef RS_ENABLE_VK
            if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
                OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
                cacheCleanupHelper_ = nullptr;
            }
#endif
            return std::move(cacheSurface_);
        }
        if (!needCheckThread || cacheSurfaceThreadIndex_ == threadIndex || !cacheSurface_) {
            return cacheSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

void RSRenderNode::CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd)
{
    if (id <= 0 || GetType() != RSRenderNodeType::CANVAS_NODE) {
        return;
    }
    auto context = GetContext().lock();
    if (!RSSystemProperties::GetAnimationCacheEnabled() ||
        !context || !context->GetNodeMap().IsResidentProcessNode(GetId())) {
        return;
    }
    auto target = modifiers_.find(id);
    if (target == modifiers_.end() || !target->second) {
        return;
    }
    if (isAnimAdd) {
        if (GROUPABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, true, false);
        } else if (CACHEABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            hasCacheableAnim_ = true;
        }
        return;
    }
    bool hasGroupableAnim = false;
    hasCacheableAnim_ = false;
    for (auto& [_, animation] : animationManager_.animations_) {
        if (!animation || id == animation->GetPropertyId()) {
            continue;
        }
        auto itr = modifiers_.find(animation->GetPropertyId());
        if (itr == modifiers_.end() || !itr->second) {
            continue;
        }
        hasGroupableAnim = (hasGroupableAnim || (GROUPABLE_ANIMATION_TYPE.count(itr->second->GetType()) != 0));
        hasCacheableAnim_ = (hasCacheableAnim_ || (CACHEABLE_ANIMATION_TYPE.count(itr->second->GetType()) != 0));
    }
    MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, hasGroupableAnim, false);
}

bool RSRenderNode::IsForcedDrawInGroup() const
{
    return nodeGroupType_ & NodeGroupType::GROUPED_BY_USER;
}

bool RSRenderNode::IsSuggestedDrawInGroup() const
{
    return nodeGroupType_ != NodeGroupType::NONE;
}

void RSRenderNode::MarkNodeGroup(NodeGroupType type, bool isNodeGroup, bool includeProperty)
{
    RS_OPTIONAL_TRACE_NAME_FMT("MarkNodeGroup type:%d isNodeGroup:%d id:%llu", type, isNodeGroup, GetId());
    RS_LOGI_IF(DEBUG_NODE, "RSRenderNode::MarkNodeGP type:%{public}d isNodeGroup:%{public}d id:%{public}" PRIu64,
        type, isNodeGroup, GetId());
    if (isNodeGroup && type == NodeGroupType::GROUPED_BY_UI) {
        auto context = GetContext().lock();
        if (context && context->GetNodeMap().IsResidentProcessNode(GetId())) {
            nodeGroupType_ |= type;
            SetDirty();
#ifdef RS_ENABLE_GPU
            if (stagingRenderParams_) {
                stagingRenderParams_->SetDirtyType(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);
            }
#endif
        }
    } else {
        if (isNodeGroup) {
            nodeGroupType_ |= type;
        } else {
            nodeGroupType_ &= ~type;
        }
        SetDirty();
#ifdef RS_ENABLE_GPU
        if (stagingRenderParams_) {
            stagingRenderParams_->SetDirtyType(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);
        }
#endif
    }
    if (nodeGroupType_ == static_cast<uint8_t>(NodeGroupType::NONE) && !isNodeGroup) {
        needClearSurface_ = true;
    }
    nodeGroupIncludeProperty_ = includeProperty;
#ifdef ROSEN_PREVIEW
    if (type == NodeGroupType::GROUPED_BY_USER) {
        dirtyTypes_.set(static_cast<int>(RSModifierType::ALPHA), true);
        GetMutableRenderProperties().SetAlphaOffscreen(isNodeGroup);
    }
#endif
    AddToPendingSyncList();
}

bool RSRenderNode::IsNodeGroupIncludeProperty() const
{
    return nodeGroupIncludeProperty_;
}

void RSRenderNode::MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid)
{
    isNodeSingleFrameComposer_ = isNodeSingleFrameComposer;
    appPid_ = pid;
}

bool RSRenderNode::GetNodeIsSingleFrameComposer() const
{
    return isNodeSingleFrameComposer_;
}

// arkui mark
void RSRenderNode::MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate)
{
    RS_TRACE_NAME_FMT("mark opinc %llx, isopinc:%d. isCal:%d", GetId(), isOpincNode, isNeedCalculate);
    opincCache_.MarkSuggestOpincNode(isOpincNode, isNeedCalculate);
    OpincSetNodeSupportFlag(true);
    SetDirty();
}

// mark support node
void RSRenderNode::OpincUpdateNodeSupportFlag(bool supportFlag)
{
    // supportFlag is obtained from isOpincNodeSupportFlag_ of the child node.
    // IsMarkedRenderGroup make sure current node is the bottom of the node marked by arkui.
    isOpincNodeSupportFlag_ = isOpincNodeSupportFlag_ && supportFlag &&
        (!opincCache_.IsMarkedRenderGroup(nodeGroupType_ > RSRenderNode::NodeGroupType::NONE));
}

std::string RSRenderNode::QuickGetNodeDebugInfo()
{
    std::string ret("");
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    AppendFormat(ret, "%llx, IsSTD:%d s:%d uc:%d suggest:%d support:%d rootF:%d",
        GetId(), IsSubTreeDirty(), opincCache_.GetNodeCacheState(), opincCache_.GetUnchangeCount(),
        opincCache_.IsSuggestOpincNode(), OpincGetNodeSupportFlag(), opincCache_.OpincGetRootFlag());
#endif
    return ret;
}

void RSRenderNode::UpdateOpincParam()
{
    if (stagingRenderParams_) {
        stagingRenderParams_->OpincSetCacheChangeFlag(opincCache_.GetCacheChangeFlag(), lastFrameSynced_);
        stagingRenderParams_->OpincUpdateRootFlag(opincCache_.OpincGetRootFlag());
        stagingRenderParams_->OpincSetIsSuggest(opincCache_.IsSuggestOpincNode());
        stagingRenderParams_->OpincUpdateSupportFlag(isOpincNodeSupportFlag_);
    }
}

void RSRenderNode::CheckDrawingCacheType()
{
    if (nodeGroupType_ == NodeGroupType::NONE) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_FOREGROUND_FILTER) {
        SetDrawingCacheType(RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_USER) {
        SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    } else {
        SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    }
}

#ifdef RS_ENABLE_STACK_CULLING
void RSRenderNode::SetFullSurfaceOpaqueMarks(const std::shared_ptr<RSRenderNode> curSurfaceNodeParam)
{
    if (!isFullSurfaceOpaquCanvasNode_) {
        int32_t tempValue = coldDownCounter_;
        coldDownCounter_ = (coldDownCounter_ + 1) % MAX_COLD_DOWN_NUM;
        if (tempValue != 0) {
            return;
        }
    } else {
        coldDownCounter_ = 0;
    }

    isFullSurfaceOpaquCanvasNode_ = false;
    if (!ROSEN_EQ(GetGlobalAlpha(), 1.0f) || HasFilter()) {
        return;
    }

    if (GetRenderProperties().GetBackgroundColor().GetAlpha() < 255) {
        return;
    }

    if (!curSurfaceNodeParam) {
        return;
    }

    auto curSurfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(curSurfaceNodeParam);
    auto surfaceNodeAbsRect = curSurfaceNode->GetOldDirty();
    auto absRect = GetFilterRect();
    if (surfaceNodeAbsRect.IsInsideOf(absRect)) {
        isFullSurfaceOpaquCanvasNode_ = true;

        auto rsParent = GetParent().lock();
        while (rsParent) {
            //skip whern another child has set its parent or reach rootnode
            if (rsParent->hasChildFullSurfaceOpaquCanvasNode_) {
                break;
            }

            rsParent->hasChildFullSurfaceOpaquCanvasNode_ = true;
            if (rsParent->IsInstanceOf<RSRootRenderNode>()) {
                break;
            }

            rsParent = rsParent->GetParent().lock();
        }
    }
}

void RSRenderNode::SetSubNodesCovered()
{
    if (hasChildFullSurfaceOpaquCanvasNode_) {
        auto sortedChildren_ = GetSortedChildren();
        if (sortedChildren_->size() <= 1) {
            return;
        }

        bool found = false;
        for (auto child = sortedChildren_->rbegin(); child != sortedChildren_->rend(); child++) {
            if (!found && ((*child)->isFullSurfaceOpaquCanvasNode_ || (*child)->hasChildFullSurfaceOpaquCanvasNode_)) {
                found = true;
                continue;
            }
            if (found) {
                (*child)->isCoveredByOtherNode_ = true;
            }
        }
    }
}
void RSRenderNode::ResetSubNodesCovered()
{
    hasChildFullSurfaceOpaquCanvasNode_ = false;
}
#endif

void RSRenderNode::ResetFilterRectsInCache(const std::unordered_set<NodeId>& curRects)
{
    curCacheFilterRects_ = curRects;
}

void RSRenderNode::GetFilterRectsInCache(std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects) const
{
    if (!curCacheFilterRects_.empty()) {
        allRects.emplace(GetId(), curCacheFilterRects_);
    }
}

bool RSRenderNode::IsFilterRectsInCache() const
{
    return !curCacheFilterRects_.empty();
}

RectI RSRenderNode::GetFilterRect() const
{
    auto& properties = GetRenderProperties();
    auto& geoPtr = (properties.GetBoundsGeometry());
    if (!geoPtr) {
        return {};
    }
    if (properties.GetClipBounds() != nullptr) {
        auto filterRect = properties.GetClipBounds()->GetDrawingPath().GetBounds();
        Drawing::Rect absRect;
        geoPtr->GetAbsMatrix().MapRect(absRect, filterRect);
        return {absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight()};
    } else {
        return geoPtr->GetAbsRect();
    }
}

void RSRenderNode::CalVisibleFilterRect(const std::optional<RectI>& clipRect)
{
    filterRegion_ = GetFilterRect();
    if (clipRect.has_value()) {
        filterRegion_ = filterRegion_.IntersectRect(*clipRect);
    }
}

void RSRenderNode::UpdateFullScreenFilterCacheRect(
    RSDirtyRegionManager& dirtyManager, bool isForeground) const
{
}

void RSRenderNode::OnTreeStateChanged()
{
    if (GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        ClearNeverOnTree();
    }

    auto curBlurDrawableCnt = GetBlurEffectDrawbleCount();
    if (!isOnTheTree_) {
        UpdateBlurEffectCounter(-curBlurDrawableCnt);
        startingWindowFlag_ = false;
        if (stagingUECChildren_ && !stagingUECChildren_->empty()) {
            for (auto uiExtension : *stagingUECChildren_) {
                uiExtension->RemoveFromTree();
            }
        }
    }
    if (isOnTheTree_) {
        UpdateBlurEffectCounter(curBlurDrawableCnt);
        // Set dirty and force add to active node list, re-generate children list if needed
        SetDirty(true);
        SetParentSubTreeDirty();
        if (stagingUECChildren_ && !stagingUECChildren_->empty()) {
            for (auto uiExtension : *stagingUECChildren_) {
                AddChild(uiExtension);
            }
        }
    } else if (sharedTransitionParam_) {
        // Mark shared transition unpaired, and mark paired node dirty
        sharedTransitionParam_->paired_ = false;
        if (auto pairedNode = sharedTransitionParam_->GetPairedNode(id_)) {
            pairedNode->SetDirty(true);
        }
    }
    drawableVecNeedClear_ = !isOnTheTree_;
    if (!isOnTheTree_ && HasBlurFilter()) { // force clear blur cache
        RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] off the tree", GetId());
        MarkForceClearFilterCacheWithInvisible();
    }
    // Clear fullChildrenList_ and RSChildrenDrawable of the parent node; otherwise, it may cause a memory leak.
    if (!isOnTheTree_) {
        isFullChildrenListValid_ = false;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
        drawableVec_[static_cast<int8_t>(RSDrawableSlot::CHILDREN)].reset();
        stagingDrawCmdList_.clear();
        RS_PROFILER_KEEP_DRAW_CMD(drawCmdListNeedSync_); // false only when used for debugging
        uifirstNeedSync_ = true;
        AddToPendingSyncList();
    }
    auto& properties = GetMutableRenderProperties();
    bool useEffect = properties.GetUseEffect();
    UseEffectType useEffectType = static_cast<UseEffectType>(properties.GetUseEffectType());
    if (useEffect && useEffectType == UseEffectType::BEHIND_WINDOW) {
        ProcessBehindWindowOnTreeStateChanged();
    }
}

bool RSRenderNode::HasDisappearingTransition(bool recursive) const
{
    if (!isOnTheTree_) {
        return false;
    }
    if (disappearingTransitionCount_ > 0) {
        return true;
    }
    if (recursive == false) {
        return false;
    }
    auto parent = GetParent().lock();
    if (parent == nullptr) {
        return false;
    }
    return parent->HasDisappearingTransition(true);
}

RSRenderNode::ChildrenListSharedPtr RSRenderNode::GetChildren() const
{
    return std::atomic_load_explicit(&fullChildrenList_, std::memory_order_acquire);
}

RSRenderNode::ChildrenListSharedPtr RSRenderNode::GetSortedChildren() const
{
    return std::atomic_load_explicit(&fullChildrenList_, std::memory_order_acquire);
}

void RSRenderNode::CollectAllChildren(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<NodeId>& vec)
{
    if (!node) {
        ROSEN_LOGD("RSRenderNode::CollectAllChildren node is nullptr");
        return;
    }
    vec.push_back(node->GetId());
    for (auto& child : *node->GetSortedChildren()) {
        child->CollectAllChildren(child, vec);
    }
}

std::shared_ptr<RSRenderNode> RSRenderNode::GetFirstChild() const
{
    return children_.empty() ? nullptr : children_.front().lock();
}

void RSRenderNode::GenerateFullChildrenList()
{
    // both children_ and disappearingChildren_ are empty, no need to generate fullChildrenList_
    if (children_.empty() && disappearingChildren_.empty()) {
        auto prevFullChildrenList = fullChildrenList_;
        isFullChildrenListValid_ = true;
        isChildrenSorted_ = true;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
        return;
    }

    // Step 0: Initialize
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();

    // Step 1: Copy all children into sortedChildren while checking and removing expired children.
    children_.remove_if([&](const auto& child) -> bool {
        auto existingChild = child.lock();
        if (existingChild == nullptr) {
            ROSEN_LOGI("RSRenderNode::GenerateSortedChildren removing expired child, this is rare but possible.");
            return true;
        }
        if (isContainBootAnimation_ && !existingChild->GetBootAnimation()) {
            ROSEN_LOGD("RSRenderNode::GenerateSortedChildren %{public}" PRIu64 " skip"
            " move not bootAnimation displaynode"
            "child(id %{public}" PRIu64 ")"" into children_", GetId(), existingChild->GetId());
            return false;
        }
        fullChildrenList->emplace_back(std::move(existingChild));
        return false;
    });

    // Step 2: Insert disappearing children into sortedChildren at their original position.
    // Note:
    //     1. We don't need to check if the disappearing transition is finished; it's already handled in
    //     RSRenderTransition::OnDetach.
    //     2. We don't need to check if the disappearing child is expired; it's already been checked when moving from
    //     children_ to disappearingChildren_. We hold ownership of the shared_ptr of the child after that.
    std::for_each(disappearingChildren_.begin(), disappearingChildren_.end(), [&](const auto& pair) -> void {
        auto& disappearingChild = pair.first;
        if (isContainBootAnimation_ && !disappearingChild->GetBootAnimation()) {
            ROSEN_LOGD("RSRenderNode::GenerateSortedChildren %{public}" PRIu64 " skip"
            " move not bootAnimation displaynode"
            "child(id %{public}" PRIu64 ")"" into disappearingChild", GetId(), disappearingChild->GetId());
            return;
        }
        fullChildrenList->emplace_back(disappearingChild);
    });

    // temporary fix for wrong z-order
    for (auto& child : *fullChildrenList) {
        child->ApplyPositionZModifier();
    }

    // Step 3: Sort all children by z-order
    std::stable_sort(
        fullChildrenList->begin(), fullChildrenList->end(), [](const auto& first, const auto& second) -> bool {
        return first->GetRenderProperties().GetPositionZ() < second->GetRenderProperties().GetPositionZ();
    });

    // Keep a reference to fullChildrenList_ to prevent its deletion when swapping it
    auto prevFullChildrenList = fullChildrenList_;

    // Update the flag to indicate that children are now valid and sorted
    isFullChildrenListValid_ = true;
    isChildrenSorted_ = true;

    // Move the fullChildrenList to fullChildrenList_ atomically
    ChildrenListSharedPtr constFullChildrenList = std::move(fullChildrenList);
    std::atomic_store_explicit(&fullChildrenList_, constFullChildrenList, std::memory_order_release);
}

void RSRenderNode::ResortChildren()
{
    // Make a copy of the fullChildrenList for sorting
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(*fullChildrenList_);

    // temporary fix for wrong z-order
    for (auto& child : *fullChildrenList) {
        child->ApplyPositionZModifier();
    }

    // Sort the children by their z-order
    std::stable_sort(
        fullChildrenList->begin(), fullChildrenList->end(), [](const auto& first, const auto& second) -> bool {
        return first->GetRenderProperties().GetPositionZ() < second->GetRenderProperties().GetPositionZ();
    });

    // Keep a reference to fullChildrenList_ to prevent its deletion when swapping it
    auto prevFullChildrenList = fullChildrenList_;

    // Update the flag to indicate that children are now sorted
    isChildrenSorted_ = true;

    // Move the fullChildrenList to fullChildrenList_ atomically
    ChildrenListSharedPtr constFullChildrenList = std::move(fullChildrenList);
    std::atomic_store_explicit(&fullChildrenList_, constFullChildrenList, std::memory_order_release);
}

uint32_t RSRenderNode::GetChildrenCount() const
{
    return children_.size();
}

std::list<RSRenderNode::WeakPtr> RSRenderNode::GetChildrenList() const
{
    return children_;
}

float RSRenderNode::GetHDRBrightness() const
{
    auto itr = drawCmdModifiers_.find(RSModifierType::HDR_BRIGHTNESS);
    if (itr == drawCmdModifiers_.end() || itr->second.empty()) {
        RS_LOGD("RSRenderNode::GetHDRBrightness drawCmdModifiers find failed");
        return 1.0f; // 1.0f make sure HDR video is still HDR state if RSNode::SetHDRBrightness not called
    }
    const auto& modifier = itr->second.back();
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(modifier->GetProperty());
    float hdrBrightness = renderProperty->Get();
    return hdrBrightness;
}

void RSRenderNode::SetTunnelHandleChange(bool change)
{
    isTunnelHandleChange_ = change;
}
bool RSRenderNode::GetTunnelHandleChange() const
{
    return isTunnelHandleChange_;
}
bool RSRenderNode::HasChildrenOutOfRect() const
{
    if (GetRenderProperties().GetClipToBounds() || GetRenderProperties().GetClipToFrame()) {
        return false;
    }
    return hasChildrenOutOfRect_;
}
void RSRenderNode::UpdateChildrenOutOfRectFlag(bool flag)
{
    hasChildrenOutOfRect_ = flag;
}
void RSRenderNode::ResetHasRemovedChild()
{
    hasRemovedChild_ = false;
}
bool RSRenderNode::HasRemovedChild() const
{
    return hasRemovedChild_;
}
RectI RSRenderNode::GetChildrenRect() const
{
    return childrenRect_;
}
RectI RSRenderNode::GetRemovedChildrenRect() const
{
    return removedChildrenRect_;
}
bool RSRenderNode::ChildHasVisibleFilter() const
{
    return childHasVisibleFilter_;
}
void RSRenderNode::SetChildHasVisibleFilter(bool val)
{
    childHasVisibleFilter_ = val;
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetChildHasVisibleFilter(val);
#endif
}
bool RSRenderNode::ChildHasVisibleEffect() const
{
    return childHasVisibleEffect_;
}
void RSRenderNode::SetChildHasVisibleEffect(bool val)
{
    childHasVisibleEffect_ = val;
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetChildHasVisibleEffect(val);
#endif
}
const std::vector<NodeId>& RSRenderNode::GetVisibleFilterChild() const
{
    return visibleFilterChild_;
}
void RSRenderNode::UpdateVisibleFilterChild(RSRenderNode& childNode)
{
    if (childNode.GetRenderProperties().NeedFilter() || childNode.GetHwcRecorder().IsBlendWithBackground() ||
        childNode.GetHwcRecorder().IsForegroundColorValid()) {
        visibleFilterChild_.emplace_back(childNode.GetId());
    }
    auto& childFilterNodes = childNode.GetVisibleFilterChild();
    visibleFilterChild_.insert(visibleFilterChild_.end(),
        childFilterNodes.begin(), childFilterNodes.end());
}
const std::unordered_set<NodeId>& RSRenderNode::GetVisibleEffectChild() const
{
    return visibleEffectChild_;
}
void RSRenderNode::UpdateVisibleEffectChild(RSRenderNode& childNode)
{
    if (childNode.GetRenderProperties().GetUseEffect()) {
        visibleEffectChild_.emplace(childNode.GetId());
    }
    auto& childEffectNodes = childNode.GetVisibleEffectChild();
    visibleEffectChild_.insert(childEffectNodes.begin(), childEffectNodes.end());
}

const std::shared_ptr<RSRenderNode> RSRenderNode::GetInstanceRootNode() const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGD("RSRenderNode::GetInstanceRootNode: Invalid context");
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode(instanceRootNodeId_);
}

void RSRenderNode::UpdateTreeUifirstRootNodeId(NodeId id)
{
#ifdef RS_ENABLE_GPU
    uifirstRootNodeId_ = id;
    if (stagingRenderParams_ && stagingRenderParams_->SetUiFirstRootNode(uifirstRootNodeId_)) {
        AddToPendingSyncList();
    }
    for (auto& child : *GetChildren()) {
        if (child) {
            child->UpdateTreeUifirstRootNodeId(id);
        }
    }
#endif
}

const std::shared_ptr<RSRenderNode> RSRenderNode::GetFirstLevelNode() const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSRenderNode::GetFirstLevelNode: Invalid context");
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode(firstLevelNodeId_);
}

const std::shared_ptr<RSRenderNode> RSRenderNode::GetUifirstRootNode() const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSRenderNode::GetUifirstRootNode: Invalid context");
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode(uifirstRootNodeId_);
}

void RSRenderNode::ResetDirtyStatus()
{
    RecordCurDirtyStatus();
    SetClean();
    auto& properties = GetMutableRenderProperties();
    properties.ResetDirty();
    isLastVisible_ = shouldPaint_;
    // The return of GetBoundsGeometry function must not be nullptr
    oldMatrix_ = properties.GetBoundsGeometry()->GetMatrix();
}

NodeId RSRenderNode::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0; // surfaceNode is seted correctly during boot when currentId is 1

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Node Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid_ << 32) | currentId;
}

bool RSRenderNode::IsRenderUpdateIgnored() const
{
    return isRenderUpdateIgnored_;
}
RSAnimationManager& RSRenderNode::GetAnimationManager()
{
    return animationManager_;
}
RectI RSRenderNode::GetOldDirty() const
{
    return oldDirty_;
}
RectI RSRenderNode::GetOldDirtyInSurface() const
{
    return oldDirtyInSurface_;
}
RectI RSRenderNode::GetOldClipRect() const
{
    return oldClipRect_;
}
void RSRenderNode::SetOldDirtyInSurface(RectI oldDirtyInSurface)
{
    oldDirtyInSurface_ = oldDirtyInSurface;
}

bool RSRenderNode::IsForegroundFilterEnable()
{
    return drawableVec_[static_cast<uint32_t>(RSDrawableSlot::FOREGROUND_FILTER)] != nullptr;
}

bool RSRenderNode::IsDirtyRegionUpdated() const
{
    return isDirtyRegionUpdated_;
}
void RSRenderNode::CleanDirtyRegionUpdated()
{
    isDirtyRegionUpdated_ = false;
}
bool RSRenderNode::IsShadowValidLastFrame() const
{
    return isShadowValidLastFrame_;
}
void RSRenderNode::SetStaticCached(bool isStaticCached)
{
    isStaticCached_ = isStaticCached;
    // ensure defrost subtree would be updated
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetRSFreezeFlag(isStaticCached);
#else
    isStaticCached = false;
#endif
    if (!isStaticCached_) {
        SetContentDirty();
    }
}
bool RSRenderNode::IsStaticCached() const
{
    return isStaticCached_;
}
void RSRenderNode::SetNodeName(const std::string& nodeName)
{
    nodeName_ = nodeName;
    auto context = GetContext().lock();
    if (!context || nodeName.empty()) {
        return;
    }
    // For LTPO: Record nodes that match the interested UI framework.
    auto& uiFrameworkTypeTable = context->GetUiFrameworkTypeTable();
    for (auto uiFwkType : uiFrameworkTypeTable) {
        if (nodeName.rfind(uiFwkType, 0) == 0) {
            context->UpdateUIFrameworkDirtyNodes(weak_from_this());
        }
    }
}
const std::string& RSRenderNode::GetNodeName() const
{
    return nodeName_;
}
void RSRenderNode::UpdateCompletedCacheSurface()
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    std::swap(cacheSurface_, cacheCompletedSurface_);
    std::swap(cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::swap(cacheBackendTexture_, cacheCompletedBackendTexture_);
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        std::swap(cacheCleanupHelper_, cacheCompletedCleanupHelper_);
    }
#endif
    SetTextureValidFlag(true);
#endif
}
void RSRenderNode::SetTextureValidFlag(bool isValid)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    isTextureValid_ = isValid;
#endif
}
void RSRenderNode::ClearCacheSurface(bool isClearCompletedCacheSurface)
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = nullptr;
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        cacheCleanupHelper_ = nullptr;
    }
#endif
    if (isClearCompletedCacheSurface) {
        cacheCompletedSurface_ = nullptr;
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            cacheCompletedCleanupHelper_ = nullptr;
        }
#endif
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        isTextureValid_ = false;
#endif
    }
}
void RSRenderNode::SetCacheType(CacheType cacheType)
{
    cacheType_ = cacheType;
}
CacheType RSRenderNode::GetCacheType() const
{
    return cacheType_;
}
int RSRenderNode::GetShadowRectOffsetX() const
{
    return shadowRectOffsetX_;
}
int RSRenderNode::GetShadowRectOffsetY() const
{
    return shadowRectOffsetY_;
}
void RSRenderNode::SetDrawingCacheType(RSDrawingCacheType cacheType)
{
    drawingCacheType_ = cacheType;
}
RSDrawingCacheType RSRenderNode::GetDrawingCacheType() const
{
    return drawingCacheType_;
}
void RSRenderNode::SetDrawingCacheChanged(bool cacheChanged)
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetDrawingCacheChanged(cacheChanged, lastFrameSynced_);
#endif
}
bool RSRenderNode::GetDrawingCacheChanged() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->GetDrawingCacheChanged();
#else
    return false;
#endif
}
void RSRenderNode::ResetDrawingCacheNeedUpdate()
{
    drawingCacheNeedUpdate_ = false;
}
void RSRenderNode::SetGeoUpdateDelay(bool val)
{
    geoUpdateDelay_ = geoUpdateDelay_ || val;
}
void RSRenderNode::ResetGeoUpdateDelay()
{
    geoUpdateDelay_ = false;
}
bool RSRenderNode::GetGeoUpdateDelay() const
{
    return geoUpdateDelay_;
}

void RSRenderNode::SetVisitedCacheRootIds(const std::unordered_set<NodeId>& visitedNodes)
{
    visitedCacheRoots_ = visitedNodes;
}
const std::unordered_set<NodeId>& RSRenderNode::GetVisitedCacheRootIds() const
{
    return visitedCacheRoots_;
}
void RSRenderNode::AddSubSurfaceUpdateInfo(SharedPtr curParent, SharedPtr preParent)
{
    if (!selfAddForSubSurfaceCnt_ && GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = ReinterpretCastTo<RSSurfaceRenderNode>();
        subSurfaceCnt_ = (surfaceNode && (surfaceNode->IsLeashWindow() || surfaceNode->IsAppWindow())) ?
            subSurfaceCnt_ + 1 : subSurfaceCnt_;
        selfAddForSubSurfaceCnt_ = true;
    }
    if (subSurfaceCnt_ == 0) {
        return;
    }
    if (auto context = context_.lock()) {
        context->AddSubSurfaceCntUpdateInfo({subSurfaceCnt_,
            preParent == nullptr ? INVALID_NODEID : preParent->GetId(),
            curParent == nullptr ? INVALID_NODEID : curParent->GetId()});
    }
}
void RSRenderNode::UpdateSubSurfaceCnt(int updateCnt)
{
    // avoid loop
    if (visitedForSubSurfaceCnt_) {
        RS_LOGE("RSRenderNode::UpdateSubSurfaceCnt: %{public}" PRIu64" has loop tree", GetId());
        return;
    }
    visitedForSubSurfaceCnt_ = true;
    if (updateCnt == 0) {
        visitedForSubSurfaceCnt_ = false;
        return;
    }
    int cnt = subSurfaceCnt_ + updateCnt;
    subSurfaceCnt_ = cnt < 0 ? 0 : cnt;
    if (auto parent = GetParent().lock()) {
        parent->UpdateSubSurfaceCnt(updateCnt);
    }
    visitedForSubSurfaceCnt_ = false;
}
bool RSRenderNode::HasSubSurface() const
{
    return subSurfaceCnt_ > 0;
}
void RSRenderNode::SetDrawingCacheRootId(NodeId id)
{
    drawingCacheRootId_ = id;
}
NodeId RSRenderNode::GetDrawingCacheRootId() const
{
    return drawingCacheRootId_;
}
bool RSRenderNode::HasAnimation() const
{
    return !animationManager_.animations_.empty();
}
bool RSRenderNode::HasFilter() const
{
    return hasFilter_;
}
void RSRenderNode::SetHasFilter(bool hasFilter)
{
    hasFilter_ = hasFilter;
}
std::recursive_mutex& RSRenderNode::GetSurfaceMutex() const
{
    return surfaceMutex_;
}

bool RSRenderNode::HasAbilityComponent() const
{
    return hasAbilityComponent_;
}
void RSRenderNode::SetHasAbilityComponent(bool hasAbilityComponent)
{
    hasAbilityComponent_ = hasAbilityComponent;
}
uint32_t RSRenderNode::GetCacheSurfaceThreadIndex() const
{
    return cacheSurfaceThreadIndex_;
}
uint32_t RSRenderNode::GetCompletedSurfaceThreadIndex() const
{
    return completedSurfaceThreadIndex_;
}

bool RSRenderNode::IsMainThreadNode() const
{
    return isMainThreadNode_;
}
void RSRenderNode::SetIsMainThreadNode(bool isMainThreadNode)
{
    isMainThreadNode_ = isMainThreadNode;
}
bool RSRenderNode::IsScale() const
{
    return isScale_;
}
void RSRenderNode::SetIsScale(bool isScale)
{
    isScale_ = isScale;
}
bool RSRenderNode::IsScaleInPreFrame() const
{
    return isScaleInPreFrame_;
}
void RSRenderNode::SetIsScaleInPreFrame(bool isScale)
{
    isScaleInPreFrame_ = isScale;
}
void RSRenderNode::SetPriority(NodePriorityType priority)
{
    priority_ = priority;
}
NodePriorityType RSRenderNode::GetPriority()
{
    return priority_;
}
bool RSRenderNode::IsAncestorDirty() const
{
    return isAncestorDirty_;
}
void RSRenderNode::SetIsAncestorDirty(bool isAncestorDirty)
{
    isAncestorDirty_ = isAncestorDirty;
}
bool RSRenderNode::IsParentLeashWindow() const
{
    return isParentLeashWindow_;
}
void RSRenderNode::SetParentLeashWindow()
{
    isParentLeashWindow_ = true;
}
bool RSRenderNode::IsParentScbScreen() const
{
    return isParentScbScreen_;
}
void RSRenderNode::SetParentScbScreen()
{
    isParentScbScreen_ = true;
}
bool RSRenderNode::HasCachedTexture() const
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return isTextureValid_;
#else
    return true;
#endif
}
void RSRenderNode::SetDrawRegion(const std::shared_ptr<RectF>& rect)
{
    if (rect && (rect->GetHeight() >= std::numeric_limits<uint16_t>::max() ||
        rect->GetWidth() >= std::numeric_limits<uint16_t>::max())) {
        RS_LOGW("node %{public}" PRIu64" set large draw region from arkui: %{public}s",
            GetId(), rect->ToString().c_str());
        RS_OPTIONAL_TRACE_NAME_FMT("node %" PRIu64" set large draw region from arkui: %s",
            GetId(), rect->ToString().c_str());
    }
    drawRegion_ = rect;
    GetMutableRenderProperties().SetDrawRegion(rect);
}
const std::shared_ptr<RectF>& RSRenderNode::GetDrawRegion() const
{
    return drawRegion_;
}
void RSRenderNode::SetOutOfParent(OutOfParentType outOfParent)
{
    outOfParent_ = outOfParent;
}
OutOfParentType RSRenderNode::GetOutOfParent() const
{
    return outOfParent_;
}
RSRenderNode::NodeGroupType RSRenderNode::GetNodeGroupType() const
{
    uint8_t type = NodeGroupType::GROUP_TYPE_BUTT;
    while (type != NodeGroupType::NONE) {
        if (nodeGroupType_ & type) {
            return static_cast<NodeGroupType>(type);
        } else {
            type = type >> 1;
        }
    }
    return NodeGroupType::NONE;
}

void RSRenderNode::MarkNonGeometryChanged()
{
    geometryChangeNotPerceived_ = true;
}

bool RSRenderNode::GetIsUsedBySubThread() const
{
    return isUsedBySubThread_.load();
}
void RSRenderNode::SetIsUsedBySubThread(bool isUsedBySubThread)
{
    isUsedBySubThread_.store(isUsedBySubThread);
}

bool RSRenderNode::GetLastIsNeedAssignToSubThread() const
{
    return lastIsNeedAssignToSubThread_;
}
void RSRenderNode::SetLastIsNeedAssignToSubThread(bool lastIsNeedAssignToSubThread)
{
    lastIsNeedAssignToSubThread_ = lastIsNeedAssignToSubThread;
}

void RSRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSRenderNode::InitRenderParams failed");
        return;
    }
#endif
}

void RSRenderNode::UpdateRenderParams()
{
#ifdef RS_ENABLE_GPU
    auto& boundGeo = GetRenderProperties().GetBoundsGeometry();
    if (!boundGeo) {
        return;
    }
    bool hasSandbox = sharedTransitionParam_ && GetRenderProperties().GetSandBox();
    stagingRenderParams_->SetHasSandBox(hasSandbox);
    stagingRenderParams_->SetMatrix(boundGeo->GetMatrix());
#ifdef RS_ENABLE_PREFETCH
    __builtin_prefetch(&boundsModifier_, 0, 1);
#endif
    stagingRenderParams_->SetFrameGravity(GetRenderProperties().GetFrameGravity());
    stagingRenderParams_->SetBoundsRect({ 0, 0, boundGeo->GetWidth(), boundGeo->GetHeight() });
    stagingRenderParams_->SetFrameRect({ 0, 0, GetRenderProperties().GetFrameWidth(),
        GetRenderProperties().GetFrameHeight() });
    stagingRenderParams_->SetShouldPaint(shouldPaint_);
    stagingRenderParams_->SetCacheSize(GetOptionalBufferSize());
    stagingRenderParams_->SetAlphaOffScreen(GetRenderProperties().GetAlphaOffscreen());
    stagingRenderParams_->SetForegroundFilterCache(GetRenderProperties().GetForegroundFilterCache());
    stagingRenderParams_->SetNeedFilter(GetRenderProperties().NeedFilter());
    stagingRenderParams_->SetHDRBrightness(GetHDRBrightness() *
        GetRenderProperties().GetCanvasNodeHDRBrightnessFactor());
    stagingRenderParams_->SetHasBlurFilter(HasBlurFilter());
    stagingRenderParams_->SetNodeType(GetType());
    stagingRenderParams_->SetEffectNodeShouldPaint(EffectNodeShouldPaint());
    stagingRenderParams_->SetHasGlobalCorner(!globalCornerRadius_.IsZero());
    stagingRenderParams_->SetFirstLevelCrossNode(isFirstLevelCrossNode_);
    stagingRenderParams_->SetAbsRotation(absRotation_);
    auto cloneSourceNode = GetSourceCrossNode().lock();
    if (cloneSourceNode) {
        stagingRenderParams_->SetCloneSourceDrawable(cloneSourceNode->GetRenderDrawable());
    }
    stagingRenderParams_->MarkRepaintBoundary(isRepaintBoundary_);
#endif
}

void RSRenderNode::SetCrossNodeOffScreenStatus(CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn_)
{
    stagingRenderParams_->SetCrossNodeOffScreenStatus(isCrossNodeOffscreenOn_);
}

bool RSRenderNode::UpdateLocalDrawRect()
{
#ifdef RS_ENABLE_GPU
    auto drawRect = selfDrawRect_.JoinRect(childrenRect_.ConvertTo<float>());
    return stagingRenderParams_->SetLocalDrawRect(drawRect);
#else
    return false;
#endif
}

void RSRenderNode::UpdateAbsDrawRect()
{
    auto absRect = GetAbsDrawRect();
    stagingRenderParams_->SetAbsDrawRect(absRect);
}

void RSRenderNode::UpdateCurCornerRadius(Vector4f& curCornerRadius)
{
    Vector4f::Max(GetRenderProperties().GetCornerRadius(), curCornerRadius, curCornerRadius);
    globalCornerRadius_ = curCornerRadius;
}

void RSRenderNode::ResetChangeState()
{
    srcOrClipedAbsDrawRectChangeFlag_ = false;
    geometryChangeNotPerceived_ = false;
    removedChildrenRect_.Clear();
}

void RSRenderNode::UpdateSrcOrClipedAbsDrawRectChangeState(const RectI& clipRect)
{
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        if (geometryChangeNotPerceived_) {
            srcOrClipedAbsDrawRectChangeFlag_ = false;
            return;
        }
    }
    auto clipedAbsDrawRect = absDrawRect_.IntersectRect(clipRect);
    // The old dirty In surface is equivalent to the old clipped absolute draw rectangle
    srcOrClipedAbsDrawRectChangeFlag_ = (absDrawRect_ != oldAbsDrawRect_ || clipedAbsDrawRect != oldDirtyInSurface_);
}

void RSRenderNode::NodeDrawLargeAreaBlur(std::pair<bool, bool>& nodeDrawLargeAreaBlur)
{
    auto backgroundFilterDrawable = GetFilterDrawable(false);
    auto compositingFilterDrawable = GetFilterDrawable(true);
    bool flagPredict = false;
    bool flagCurrent = false;
    if (backgroundFilterDrawable) {
        flagPredict = flagPredict || backgroundFilterDrawable->WouldDrawLargeAreaBlur();
        flagCurrent = flagCurrent || backgroundFilterDrawable->WouldDrawLargeAreaBlurPrecisely();
    }
    if (compositingFilterDrawable) {
        flagPredict = flagPredict || compositingFilterDrawable->WouldDrawLargeAreaBlur();
        flagCurrent = flagCurrent || compositingFilterDrawable->WouldDrawLargeAreaBlurPrecisely();
    }
    nodeDrawLargeAreaBlur.first = flagPredict;
    nodeDrawLargeAreaBlur.second = flagCurrent;
}

void RSRenderNode::OnSync()
{
    addedToPendingSyncList_ = false;
    bool isLeashWindowPartialSkip = false;

    if (renderDrawable_ == nullptr) {
        return;
    }
    // uifirstSkipPartialSync means don't need to trylock whether drawable is onDraw or not
    DrawableV2::RSRenderNodeSingleDrawableLocker
        singleLocker(uifirstSkipPartialSync_ ? nullptr : renderDrawable_.get());
    if (!uifirstSkipPartialSync_ && UNLIKELY(!singleLocker.IsLocked())) {
#ifdef RS_ENABLE_GPU
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
#endif
        RS_LOGE("Drawable try to Sync when node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            return;
        }
    }

    if (drawCmdListNeedSync_) {
        std::swap(stagingDrawCmdList_, renderDrawable_->drawCmdList_);
        stagingDrawCmdList_.clear();
        renderDrawable_->drawCmdIndex_ = stagingDrawCmdIndex_;
        drawCmdListNeedSync_ = false;
    }

    if (drawableVecNeedClear_) {
        ClearDrawableVec2();
    }

#ifdef RS_ENABLE_GPU
    renderDrawable_->backgroundFilterDrawable_ = GetFilterDrawable(false);
    renderDrawable_->compositingFilterDrawable_ = GetFilterDrawable(true);
    if (stagingRenderParams_->NeedSync()) {
        stagingRenderParams_->OnSync(renderDrawable_->renderParams_);
    }
#endif
    if (unobscuredUECChildrenNeedSync_) {
        renderDrawable_->UECChildrenIds_->clear();
        for (auto childUEC : *stagingUECChildren_) {
            renderDrawable_->UECChildrenIds_->insert(childUEC->GetId());
        }
        unobscuredUECChildrenNeedSync_ = false;
    }
    if (!uifirstSkipPartialSync_) {
        if (!dirtySlots_.empty()) {
            for (const auto& slot : dirtySlots_) {
                if (auto& drawable = drawableVec_[static_cast<uint32_t>(slot)]) {
                    drawable->OnSync();
                }
            }
            dirtySlots_.clear();
        }

        // copy newest for uifirst root node, now force sync done nodes
        if (uifirstNeedSync_) {
            RS_OPTIONAL_TRACE_NAME_FMT("uifirst_sync %lld", GetId());
            renderDrawable_->uifirstDrawCmdList_.assign(renderDrawable_->drawCmdList_.begin(),
                                                        renderDrawable_->drawCmdList_.end());
            renderDrawable_->uifirstDrawCmdIndex_ = renderDrawable_->drawCmdIndex_;
            renderDrawable_->renderParams_->OnSync(renderDrawable_->uifirstRenderParams_);
            uifirstNeedSync_ = false;
        }
    } else {
        RS_TRACE_NAME_FMT("partial_sync %lld", GetId());
        std::vector<RSDrawableSlot> todele;
        if (!dirtySlots_.empty()) {
            for (const auto& slot : dirtySlots_) {
                if (slot != RSDrawableSlot::CONTENT_STYLE && slot != RSDrawableSlot::CHILDREN) { // SAVE_FRAME
                    if (auto& drawable = drawableVec_[static_cast<uint32_t>(slot)]) {
                        drawable->OnSync();
                    }
                    todele.push_back(slot);
                }
            }
            for (const auto& slot : todele) {
                dirtySlots_.erase(slot);
            }
        }
        uifirstSkipPartialSync_ = false;
        isLeashWindowPartialSkip = true;
    }
#ifdef RS_ENABLE_GPU
    if (ShouldClearSurface()) {
        renderDrawable_->TryClearSurfaceOnSync();
        needClearSurface_ = false;
    }
#endif
    // Reset FilterCache Flags
    backgroundFilterRegionChanged_ = false;
    backgroundFilterInteractWithDirty_ = false;
    foregroundFilterRegionChanged_ = false;
    foregroundFilterInteractWithDirty_ = false;

    // Reset Sync Flag
    // only canvas drawing node use SetNeedDraw function
    if (GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE && waitSync_) {
        renderDrawable_->SetNeedDraw(true);
    }
    waitSync_ = false;

    lastFrameSynced_ = !isLeashWindowPartialSkip;
}

void RSRenderNode::OnSkipSync()
{
    lastFrameSynced_ = false;
    // clear flag: after skips sync, node not in RSMainThread::Instance()->GetContext.pendingSyncNodes_
    addedToPendingSyncList_ = false;
}

bool RSRenderNode::ShouldClearSurface()
{
#ifdef RS_ENABLE_GPU
    bool renderGroupFlag = GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE ||
        opincCache_.OpincGetRootFlag();
    bool freezeFlag = stagingRenderParams_->GetRSFreezeFlag();
    return (renderGroupFlag || freezeFlag || nodeGroupType_ == static_cast<uint8_t>(NodeGroupType::NONE)) &&
        needClearSurface_;
#else
    return (nodeGroupType_ == static_cast<uint8_t>(NodeGroupType::NONE)) && needClearSurface_;
#endif
}

void RSRenderNode::ValidateLightResources()
{
    auto& properties = GetMutableRenderProperties();
    if (properties.lightSourcePtr_ && properties.lightSourcePtr_->IsLightSourceValid()) {
        properties.CalculateAbsLightPosition();
        RSPointLightManager::Instance()->AddDirtyLightSource(weak_from_this());
    }
    if (properties.illuminatedPtr_ && properties.illuminatedPtr_->IsIlluminatedValid()) {
        RSPointLightManager::Instance()->AddDirtyIlluminated(weak_from_this());
    }
}

void RSRenderNode::MarkBlurIntersectWithDRM(bool intersectWithDRM, bool isDark)
{
#ifdef RS_ENABLE_GPU
    const auto& properties = GetRenderProperties();
    if (properties.GetBackgroundFilter()) {
        if (auto filterDrawable = GetFilterDrawable(false)) {
            filterDrawable->MarkBlurIntersectWithDRM(intersectWithDRM, isDark);
        }
    }
#endif
}

bool RSRenderNode::GetUifirstSupportFlag()
{
    if (sharedTransitionParam_ && !sharedTransitionParam_->IsInAppTranSition()) {
        return false;
    }
    return isChildSupportUifirst_ && isUifirstNode_;
}

void RSRenderNode::UpdatePointLightDirtySlot()
{
    UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::POINT_LIGHT);
}

void RSRenderNode::AddToPendingSyncList()
{
    if (addedToPendingSyncList_) {
        return;
    }

    if (auto context = GetContext().lock()) {
        context->AddPendingSyncNode(shared_from_this());
        addedToPendingSyncList_ = true;
    } else {
        ROSEN_LOGE("RSRenderNode::AddToPendingSyncList context is null");
        OnSync();
    }
}

void RSRenderNode::SetStartingWindowFlag(bool startingFlag)
{
    if (startingFlag) {
        UpdateDrawingCacheInfoAfterChildren();
    }
    if (startingWindowFlag_ == startingFlag) {
        return;
    }
    startingWindowFlag_ = startingFlag;
    auto stagingParams = stagingRenderParams_.get();
    if (stagingParams) {
        stagingParams->SetStartingWindowFlag(startingFlag);
        AddToPendingSyncList();
    }
}

void RSRenderNode::MarkUifirstNode(bool isUifirstNode)
{
    RS_OPTIONAL_TRACE_NAME_FMT("MarkUifirstNode id:%lld, isUifirstNode:%d", GetId(), isUifirstNode);
    isUifirstNode_ = isUifirstNode;
    isUifirstDelay_ = 0;
}


void RSRenderNode::MarkUifirstNode(bool isForceFlag, bool isUifirstEnable)
{
    RS_TRACE_NAME_FMT("MarkUifirstNode id:%lld, isForceFlag:%d, isUifirstEnable:%d",
        GetId(), isForceFlag, isUifirstEnable);
    ROSEN_LOGI("MarkUifirstNode id:%{public}" PRIu64 " isForceFlag:%{public}d, isUifirstEnable:%{public}d",
        GetId(), isForceFlag, isUifirstEnable);
    isForceFlag_ = isForceFlag;
    isUifirstEnable_ = isUifirstEnable;
}

bool RSRenderNode::GetUifirstNodeForceFlag() const
{
    return isForceFlag_;
}

void RSRenderNode::SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch)
{
    uiFirstSwitch_ = uiFirstSwitch;
    if (auto& firstNode = GetFirstLevelNode()) {
        firstNode->uiFirstSwitch_ = uiFirstSwitch;
    }
}

RSUIFirstSwitch RSRenderNode::GetUIFirstSwitch() const
{
    return uiFirstSwitch_;
}

void RSRenderNode::SetChildrenHasSharedTransition(bool hasSharedTransition)
{
    childrenHasSharedTransition_ = hasSharedTransition;
}

void RSRenderNode::RemoveChildFromFulllist(NodeId id)
{
    // Make a copy of the fullChildrenList
    if (!fullChildrenList_) {
        return;
    }
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(*fullChildrenList_);

    fullChildrenList->erase(std::remove_if(fullChildrenList->begin(),
        fullChildrenList->end(), [id](const auto& node) { return id == node->GetId(); }), fullChildrenList->end());

    // Move the fullChildrenList to fullChildrenList_ atomically
    ChildrenListSharedPtr constFullChildrenList = std::move(fullChildrenList);
    std::atomic_store_explicit(&fullChildrenList_, constFullChildrenList, std::memory_order_release);
}

std::map<NodeId, std::weak_ptr<SharedTransitionParam>> SharedTransitionParam::unpairedShareTransitions_;

SharedTransitionParam::SharedTransitionParam(RSRenderNode::SharedPtr inNode, RSRenderNode::SharedPtr outNode,
    bool isInSameWindow)
    : inNode_(inNode), outNode_(outNode), inNodeId_(inNode->GetId()), outNodeId_(outNode->GetId()),
      crossApplication_(!isInSameWindow)
{}

RSRenderNode::SharedPtr SharedTransitionParam::GetPairedNode(const NodeId nodeId) const
{
    if (inNodeId_ == nodeId) {
        return outNode_.lock();
    }
    if (outNodeId_ == nodeId) {
        return inNode_.lock();
    }
    return nullptr;
}

void RSRenderNode::SetChildrenHasUIExtension(bool childrenHasUIExtension)
{
    childrenHasUIExtension_ = childrenHasUIExtension;
    auto parent = GetParent().lock();
    if (parent && parent->ChildrenHasUIExtension() != childrenHasUIExtension) {
        parent->SetChildrenHasUIExtension(childrenHasUIExtension);
    }
}

bool SharedTransitionParam::HasRelation()
{
    return relation_ != NodeHierarchyRelation::UNKNOWN;
}

void SharedTransitionParam::SetNeedGenerateDrawable(const bool needGenerateDrawable)
{
    needGenerateDrawable_ = needGenerateDrawable;
}

void SharedTransitionParam::GenerateDrawable(const RSRenderNode& node)
{
    if (!needGenerateDrawable_ || !HasRelation() || IsLower(node.GetId())) {
        return;
    }
    if (auto parent = node.GetParent().lock()) {
        parent->ApplyModifiers();
    }
    SetNeedGenerateDrawable(false);
}

void SharedTransitionParam::UpdateUnpairedSharedTransitionMap(const std::shared_ptr<SharedTransitionParam>& param)
{
    if (auto it = unpairedShareTransitions_.find(param->inNodeId_);
        it != unpairedShareTransitions_.end()) {
        // remove successfully paired share transition
        unpairedShareTransitions_.erase(it);
        param->paired_ = true;
    } else {
        // add unpaired share transition
        unpairedShareTransitions_.emplace(param->inNodeId_, param);
    }
}

bool SharedTransitionParam::IsLower(const NodeId nodeId) const
{
    if (relation_ == NodeHierarchyRelation::UNKNOWN) {
        return false;
    }
    return relation_ == NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE ? inNodeId_ == nodeId : outNodeId_ == nodeId;
}

void SharedTransitionParam::UpdateHierarchy(const NodeId nodeId)
{
    // Skip if the hierarchy is already established
    if (relation_ != NodeHierarchyRelation::UNKNOWN) {
        return;
    }

    bool visitingInNode = (nodeId == inNodeId_);
    if (!visitingInNode && nodeId != outNodeId_) {
        return;
    }
    // Nodes in the same application will be traversed by order (first visited node has lower hierarchy), while
    // applications will be traversed by reverse order. If visitingInNode matches crossApplication_, inNode is above
    // outNode. Otherwise, inNode is below outNode.
    relation_ = (visitingInNode == crossApplication_) ? NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE
                                                      : NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;
}

std::string SharedTransitionParam::Dump() const
{
    return ", SharedTransitionParam: [" + std::to_string(inNodeId_) + " -> " + std::to_string(outNodeId_) + "]";
}

void SharedTransitionParam::ResetRelation()
{
    relation_ = NodeHierarchyRelation::UNKNOWN;
}

void SharedTransitionParam::InternalUnregisterSelf()
{
    auto inNode = inNode_.lock();
    if (inNode) {
        inNode->SetSharedTransitionParam(nullptr);
    }
    auto outNode = outNode_.lock();
    if (outNode) {
        outNode->SetSharedTransitionParam(nullptr);
    }
    if (inNode) {
        if (auto parent = inNode->GetParent().lock()) {
            parent->ApplyModifiers();
        }
    }
    if (outNode) {
        if (auto parent = outNode->GetParent().lock()) {
            parent->ApplyModifiers();
        }
    }
}

void RSRenderNode::ProcessBehindWindowOnTreeStateChanged()
{
    auto rootNode = GetInstanceRootNode();
    if (!rootNode) {
        return;
    }
    RS_LOGD("RSSurfaceRenderNode::ProcessBehindWindowOnTreeStateChanged nodeId = %{public}" PRIu64
        ", isOnTheTree_ = %{public}d", GetId(), isOnTheTree_);
    if (isOnTheTree_) {
        rootNode->AddChildBlurBehindWindow(GetId());
    } else {
        rootNode->RemoveChildBlurBehindWindow(GetId());
    }
}

void RSRenderNode::ProcessBehindWindowAfterApplyModifiers()
{
    auto rootNode = GetInstanceRootNode();
    if (!rootNode) {
        return;
    }
    auto& properties = GetMutableRenderProperties();
    bool useEffect = properties.GetUseEffect();
    UseEffectType useEffectType = static_cast<UseEffectType>(properties.GetUseEffectType());
    RS_LOGD("RSSurfaceRenderNode::ProcessBehindWindowAfterApplyModifiers nodeId = %{public}" PRIu64
        ", isOnTheTree_ = %{public}d, useEffect = %{public}d, useEffectType = %{public}hd",
        GetId(), isOnTheTree_, useEffect, useEffectType);
    if (useEffect && useEffectType == UseEffectType::BEHIND_WINDOW) {
        rootNode->AddChildBlurBehindWindow(GetId());
    } else {
        rootNode->RemoveChildBlurBehindWindow(GetId());
    }
}

void RSRenderNode::UpdateDrawableBehindWindow()
{
    AddDirtyType(RSModifierType::BACKGROUND_BLUR_RADIUS);
    SetContentDirty();
#ifdef RS_ENABLE_GPU
    // TODO: dirtyTypesNG
    auto dirtySlots = RSDrawable::CalculateDirtySlots(dirtyTypes_, drawableVec_);
    if (dirtySlots.empty()) {
        RS_LOGD("RSRenderNode::UpdateDrawableBehindWindow dirtySlots is empty");
        return;
    }
    bool drawableChanged = RSDrawable::UpdateDirtySlots(*this, drawableVec_, dirtySlots);
    RSDrawable::FuzeDrawableSlots(*this, drawableVec_);
    RS_LOGD("RSRenderNode::UpdateDrawableBehindWindow drawableChanged:%{public}d", drawableChanged);
    if (drawableChanged) {
        RSDrawable::UpdateSaveRestore(*this, drawableVec_, drawableVecStatus_);
        UpdateDisplayList();
    }
    if (dirtySlots_.empty()) {
        dirtySlots_ = std::move(dirtySlots);
    } else {
        dirtySlots_.insert(dirtySlots.begin(), dirtySlots.end());
    }
#endif
}

size_t RSRenderNode::GetAllModifierSize()
{
    size_t totalSize = 0;
    for (auto& [type, modifier] : modifiers_) {
        if (modifier) {
            totalSize += modifier->GetSize();
        }
    }

    for (auto& [type, modifiers] : drawCmdModifiers_) {
        for (auto& modifier : modifiers) {
            if (modifier != nullptr) {
                totalSize += modifier->GetSize();
            }
        }
    }

    return totalSize;
}

void RSRenderNode::AddModifier(const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier)
{
    if (modifier == nullptr) {
        return;
    }
    if (modifier->IsAttached()) {
        // other cleanup
        modifier->OnDetachModifier();
    }
    modifiersNG_[static_cast<uint8_t>(modifier->GetType())].emplace_back(modifier);
    modifier->OnAttachModifier(*this);
}

void RSRenderNode::RemoveModifier(ModifierNG::RSModifierType type, ModifierId id)
{
    auto& slot = modifiersNG_[static_cast<uint8_t>(type)];
    auto it = std::find_if (slot.begin(), slot.end(),
        [id](const auto& modifier) -> bool { return modifier->GetId() == id; });
    if (it == slot.end()) {
        return;
    }
    (*it)->OnDetachModifier();
    slot.erase(it);
}

void RSRenderNode::RemoveAllModifiersNG()
{
    for (auto& slot : modifiersNG_) {
        for (auto& modifier : slot) {
            modifier->OnDetachModifier();
        }
        slot.clear();
    }
}

std::shared_ptr<ModifierNG::RSRenderModifier> RSRenderNode::GetModifierNG(
    ModifierNG::RSModifierType type, ModifierId id)
{
    auto& slot = modifiersNG_[static_cast<uint8_t>(type)];
    auto it =
        std::find_if(slot.begin(), slot.end(), [id](const auto& modifier) -> bool { return modifier->GetId() == id; });
    if (it == slot.end()) {
        return nullptr;
    }
    return *it;
}

std::shared_ptr<ModifierNG::RSRenderModifier> RSRenderNode::GetModifierNG(PropertyId id)
{
    auto property = GetProperty(id);
    if (property == nullptr) {
        return nullptr;
    }
    // return property->GetTarget();
    return nullptr;
}

std::shared_ptr<RSRenderPropertyBase> RSRenderNode::GetProperty(PropertyId id)
{
    auto it = properties_.find(id);
    if (it == properties_.end()) {
        return nullptr;
    }
    return it->second;
}
} // namespace Rosen
} // namespace OHOS
