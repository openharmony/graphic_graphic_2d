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
#include "rs_trace.h"

#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "common/rs_common_tools.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "feature/hdr/rs_colorspace_util.h"
#ifdef RS_MEMORY_INFO_MANAGER
#include "feature/memory_info_manager/rs_memory_info_manager.h"
#endif
#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_union_render_node.h"
#include "pipeline/sk_resource_manager.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"
#include "render/rs_foreground_effect_filter.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"
#include "rs_profiler.h"

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

namespace {
const std::unordered_set<RSDrawableSlot> edrDrawableSlots = {
    RSDrawableSlot::FOREGROUND_FILTER,
    RSDrawableSlot::BACKGROUND_FILTER,
    RSDrawableSlot::BACKGROUND_NG_SHADER,
    RSDrawableSlot::COMPOSITING_FILTER,
    RSDrawableSlot::BLENDER,
    RSDrawableSlot::POINT_LIGHT,
};

// ensure the corresponding drawable type inherits from RSFilterDrawable.
static const std::unordered_set<RSDrawableSlot> filterDrawableSlotsSupportGetRect = {
    RSDrawableSlot::MATERIAL_FILTER,
    RSDrawableSlot::BACKGROUND_FILTER,
    RSDrawableSlot::COMPOSITING_FILTER,
};

constexpr uint32_t SET_IS_ON_THE_TREE_THRESHOLD = 50;
static uint32_t g_setIsOntheTreeCnt = 0;
constexpr size_t CACHE_FILTER_DRAWABLE_SIZE = 3;
} // namespace

using RSCacheFilterPara = std::pair<bool, RSDrawableSlot>; // first: update condition, second: slot
using RSCacheFilterParaArray = std::array<RSCacheFilterPara, CACHE_FILTER_DRAWABLE_SIZE>;

using namespace TemplateUtils;

void RSRenderNode::OnRegister(const std::weak_ptr<RSContext>& context)
{
    context_ = context;
    renderProperties_.backref_ = weak_from_this();
    SetDirty(true);
    InitRenderParams();
}

bool RSRenderNode::IsPureContainer() const
{
    return (!GetRenderProperties().isDrawn_ && !GetRenderProperties().alphaNeedApply_ && !HasDrawCmdModifiers());
}

bool RSRenderNode::IsPureBackgroundColor() const
{
    static const std::unordered_set<RSDrawableSlot> pureBackgroundColorSlots = {
        RSDrawableSlot::SAVE_ALL,
        RSDrawableSlot::BG_SAVE_BOUNDS,
        RSDrawableSlot::CLIP_TO_BOUNDS,
        RSDrawableSlot::BACKGROUND_COLOR,
        RSDrawableSlot::BG_RESTORE_BOUNDS,
        RSDrawableSlot::SAVE_FRAME,
        RSDrawableSlot::FRAME_OFFSET,
        RSDrawableSlot::CLIP_TO_FRAME,
        RSDrawableSlot::CHILDREN,
        RSDrawableSlot::RESTORE_FRAME,
        RSDrawableSlot::RESTORE_ALL
    };

#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (!drawableVec_) {
        RS_LOGE("drawableVec_ is nullptr");
        return false;
    }
    auto& drawableVec = *drawableVec_;
#else
    auto& drawableVec = drawableVec_;
#endif

    // Defines the valid RSDrawableSlot configuration of the pure background color node:
    // - if the node includes a "CHILDREN" slot, slots associated with the "CHILDREN" group are permitted.
    // - if the node includes a "BACKGROUND_COLOR" slot, slots associated with "BACKGROUND_COLOR" group are permitted.
    // - if both "CHILDREN" and  "BACKGROUND_COLOR" slots are present, slots valid in either group are permitted.
    for (int8_t i = 0; i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        if (drawableVec[i] &&
            !pureBackgroundColorSlots.count(static_cast<RSDrawableSlot>(i))) {
            const auto& property = GetRenderProperties();
            if (i == static_cast<int8_t>(RSDrawableSlot::BLENDER) &&
                property.GetColorBlendMode() == static_cast<int>(RSColorBlendMode::SRC_OVER) &&
                property.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
                continue;
            }
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
    return !GetRenderProperties().isDrawn_ &&
        ((HasContentStyleModifierOnly() && !GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE).empty()) ||
        !HasDrawCmdModifiers());
}

static inline bool IsPurgeAble()
{
    return RSSystemProperties::GetRenderNodePurgeEnabled() && RSUniRenderJudgement::IsUniRender();
}

RSRenderNode::RSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : isTextureExportNode_(isTextureExportNode), isPurgeable_(IsPurgeAble()), id_(id), context_(context)
#ifdef RS_ENABLE_MEMORY_DOWNTREE
        , drawableVec_(std::make_unique<RSDrawable::Vec>())
#endif
{
    RS_PROFILER_RENDERNODE_INC(isOnTheTree_);
}

RSRenderNode::RSRenderNode(
    NodeId id, bool isOnTheTree, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
        : isOnTheTree_(isOnTheTree), isTextureExportNode_(isTextureExportNode), isPurgeable_(IsPurgeAble()),
        id_(id), context_(context)
#ifdef RS_ENABLE_MEMORY_DOWNTREE
        , drawableVec_(std::make_unique<RSDrawable::Vec>())
#endif
{
    RS_PROFILER_RENDERNODE_INC(isOnTheTree_);
}

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
            uifirstRootNodeId_, screenNodeId_, logicalDisplayNodeId_);
        ShowSetIsOnetheTreeCntIfNeed(__func__, GetId(), GetNodeName());
    } else {
        if (child->GetType() == RSRenderNodeType::SURFACE_NODE) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            HILOG_COMM_INFO("RSRenderNode:: add child surfaceNode[id:%{public}" PRIu64 " name:%{public}s]"
            " parent'S isOnTheTree_:%{public}d", surfaceNode->GetId(), surfaceNode->GetNodeName().c_str(),
            isOnTheTree_);
        }
    }
    ((RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && child->GetType() == RSRenderNodeType::SURFACE_NODE) ||
        child->GetNeedUseCmdlistDrawRegion())
        ? child->SetParentSubTreeDirty()
        : SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::SetContainBootAnimation(bool isContainBootAnimation)
{
    isContainBootAnimation_ = isContainBootAnimation;
    isFullChildrenListValid_ = false;
    if (GetType() == RSRenderNodeType::SCREEN_NODE) {
        if (auto parentPtr = GetParent().lock()) {
            parentPtr->SetContainBootAnimation(isContainBootAnimation);
        }
    }
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
    ((RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && child->GetType() == RSRenderNodeType::SURFACE_NODE) ||
        child->GetNeedUseCmdlistDrawRegion())
        ? child->SetParentSubTreeDirty()
        : SetContentDirty();
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
    for (auto &childWeakPtr : children_) {
        if (auto child = childWeakPtr.lock()) {
            hasUnobscuredUEC |= child->HasUnobscuredUEC();
            if (hasUnobscuredUEC) {
                break;
            }
        }
    }
    stagingRenderParams_->SetHasUnobscuredUEC(hasUnobscuredUEC);
}

#ifdef SUBTREE_PARALLEL_ENABLE
void RSRenderNode::ClearSubtreeParallelNodes()
{
    subtreeParallelNodes_.clear();
}

void RSRenderNode::ResetRepaintBoundaryInfo()
{
    isAllChildRepaintBoundary_ = true;
    hasForceSubmit_ = false;
    repaintBoundaryWeight_ = 0;
}

void RSRenderNode::UpdateRepaintBoundaryInfo(RSRenderNode& node)
{
    isAllChildRepaintBoundary_ = isAllChildRepaintBoundary_ && node.IsRepaintBoundary();
    hasForceSubmit_ = hasForceSubmit_ || node.HasForceSubmit() || node.GetRenderProperties().GetNeedForceSubmit();
    repaintBoundaryWeight_ += node.GetRepaintBoundaryWeight() + 1;
}

uint32_t RSRenderNode::GetRepaintBoundaryWeight()
{
    return repaintBoundaryWeight_;
}

void RSRenderNode::UpdateSubTreeParallelNodes()
{
    // static constexpr size_t RB_POLICY_CHILDREN_NUMBER = SubtreeParallelParam::GetRBChildrenWeight();
    static constexpr size_t RB_POLICY_CHILDREN_NUMBER = 4;
    if (!isRepaintBoundary_ || GetChildrenCount() <= RB_POLICY_CHILDREN_NUMBER || !isAllChildRepaintBoundary_
        || ChildHasVisibleEffect() || GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        return;
    }
    subtreeParallelNodes_.emplace(id_);
}

void RSRenderNode::MergeSubtreeParallelNodes(RSRenderNode& childNode)
{
    auto& childSubtreeParallelNodes = childNode.GetSubtreeParallelNodes();
    subtreeParallelNodes_.insert(childSubtreeParallelNodes.begin(), childSubtreeParallelNodes.end());
}

const std::unordered_set<NodeId>& RSRenderNode::GetSubtreeParallelNodes()
{
    return subtreeParallelNodes_;
}
#endif

void RSRenderNode::SetHdrNum(bool flag, NodeId instanceRootNodeId, NodeId screenNodeId, HDRComponentType hdrType)
{
#ifndef ROSEN_CROSS_PLATFORM
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

        auto screenNode = context->GetNodeMap().GetRenderNode<RSScreenRenderNode>(screenNodeId);
        if (!screenNode) {
            ROSEN_LOGE("RSRenderNode::SetHdrNum: failed to get RSScreenRenderNode");
            return;
        }

        HdrStatus fromHdrType = HdrStatus::NO_HDR;
        uint32_t headroom = 0U;
        switch (hdrType) {
            case HDRComponentType::IMAGE:
                fromHdrType = HdrStatus::HDR_PHOTO;
                headroom = RSRenderNode::GetHdrPhotoHeadroom();
                break;
            case HDRComponentType::UICOMPONENT:
                fromHdrType = HdrStatus::HDR_UICOMPONENT;
                headroom = RSRenderNode::GetHdrUIComponentHeadroom();
                break;
            case HDRComponentType::EFFECT:
                fromHdrType = HdrStatus::HDR_EFFECT;
                headroom = RSRenderNode::GetHdrEffectHeadroom();
                break;
            default:
                fromHdrType = HdrStatus::NO_HDR;
                headroom = 0U;
                break;
        }
        if (flag) {
            screenNode->UpdateHeadroomMapIncrease(fromHdrType, headroom);
        } else {
            screenNode->UpdateHeadroomMapDecrease(fromHdrType, headroom);
        }
    }
#endif
}

void RSRenderNode::ResetNodeColorSpace()
{
    stagingRenderParams_->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

void RSRenderNode::SetNodeColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace = RSColorSpaceUtil::SelectBigGamut(colorSpace, stagingRenderParams_->GetNodeColorSpace());
    stagingRenderParams_->SetNodeColorSpace(colorSpace);
}

GraphicColorGamut RSRenderNode::GetNodeColorSpace() const
{
    return stagingRenderParams_->GetNodeColorSpace();
}

void RSRenderNode::SetEnableHdrEffect(bool enableHdrEffect)
{
    if (enableHdrEffect_ == enableHdrEffect) {
        return;
    }
    if (IsOnTheTree()) {
        float hdrEffectBrightness = 2.0F;
        auto& rsLuminance = RSLuminanceControl::Get();
        RSRenderNode::SetHdrEffectHeadroom(rsLuminance.ConvertScalerFromFloatToLevel(hdrEffectBrightness));
        SetHdrNum(enableHdrEffect, GetInstanceRootNodeId(), GetScreenNodeId(), HDRComponentType::EFFECT);
    }
    UpdateHDRStatus(HdrStatus::HDR_EFFECT, enableHdrEffect);
    enableHdrEffect_ = enableHdrEffect;
}

void RSRenderNode::UpdateHDRStatus(HdrStatus hdrStatus, bool isAdd)
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->UpdateHDRStatus(hdrStatus, isAdd);
#endif
}

void RSRenderNode::ClearHDRVideoStatus()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->ClearHDRVideoStatus();
#endif
}

HdrStatus RSRenderNode::GetHDRStatus() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->GetHDRStatus();
#endif
    return HdrStatus::NO_HDR;
}

void RSRenderNode::SetChildHasVisibleHDRContent(bool val)
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetChildHasVisibleHDRContent(val);
#endif
}

bool RSRenderNode::ChildHasVisibleHDRContent() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->ChildHasVisibleHDRContent();
#endif
    return false;
}

void RSRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId, NodeId screenNodeId, NodeId logicalDisplayNodeId)
{
#ifdef RS_ENABLE_GPU
    // We do not need to label a child when the child is removed from a parent that is not on the tree
    if (flag == isOnTheTree_) {
        return;
    }
    g_setIsOntheTreeCnt++;

#ifdef RS_MEMORY_INFO_MANAGER
    RSMemoryInfoManager::RecordNodeOnTreeStatus(flag, GetId(), instanceRootNodeId);
#endif
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (flag && IsNodeMemClearEnable()) {
        RS_LOGD("RSRenderNode::SetIsOnTheTree on tree: node[id:%{public}" PRIu64 "]", GetId());
        InitRenderDrawableAndDrawableVec();
    }
#endif
    if (autoClearCloneNode_ && !flag) {
        ClearCloneCrossNode();
    }

    if (enableHdrEffect_) {
        NodeId parentNodeId = flag ? instanceRootNodeId : instanceRootNodeId_;
        ROSEN_LOGD("RSRenderNode::SetIsOnTheTree HDREffect Node[id:%{public}" PRIu64 " name:%{public}s]"
                   " parent's id:%{public}" PRIu64 " ",
            GetId(), GetNodeName().c_str(), parentNodeId);
        NodeId actualScreenNodeId = flag ? screenNodeId : screenNodeId_;
        SetHdrNum(flag, parentNodeId, actualScreenNodeId, HDRComponentType::EFFECT);
    }

    if (isOnTheTree_ != flag) {
        RS_PROFILER_RENDERNODE_CHANGE(flag);
    }

    isNewOnTree_ = flag && !isOnTheTree_;
    isOnTheTree_ = flag;
    screenNodeId_ = screenNodeId;
    logicalDisplayNodeId_ = logicalDisplayNodeId;
    if (isOnTheTree_) {
        instanceRootNodeId_ = instanceRootNodeId;
        firstLevelNodeId_ = firstLevelNodeId;
        OnTreeStateChanged();
        auto instanceRootNode = GetInstanceRootNode();
        if (instanceRootNode) {
            auto appWindow = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (appWindow && appWindow->IsAppWindow()) {
                stagingRenderParams_->SetInstanceRootNodeId(appWindow->GetId());
                stagingRenderParams_->SetInstanceRootNodeName(appWindow->GetName());
            }
        }
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
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId, uifirstRootNodeId, screenNodeId,
            logicalDisplayNodeId);
    }

    for (auto& [child, _] : disappearingChildren_) {
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId, uifirstRootNodeId, screenNodeId,
            logicalDisplayNodeId);
    }

#ifdef RS_MEMORY_INFO_MANAGER
    RSMemoryInfoManager::ResetRootNodeStatusChangeFlag(GetId(), instanceRootNodeId);
#endif
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (IsNodeMemClearEnable()) {
        RSRenderNodeGC::Instance().SetIsOnTheTree(GetId(), weak_from_this(), isOnTheTree_);
    }
#endif
#endif
    auto context = GetContext().lock();
    auto task = [weakThis = weak_from_this()] () {
        auto node = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(weakThis.lock());
        if (node == nullptr) {
            return;
        }
        node->AfterTreeStatueChanged();
    };
    if (context) {
        context->PostTask(task, false);
    }
}

void RSRenderNode::ReleaseNodeMem()
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (isOnTheTree_) {
        return;
    }
    released_ = true;
    ROSEN_LOGD("RSRenderNode::ReleaseNodeMem, node[id:%{public}" PRIu64 "]", GetId());
    if (renderDrawable_) {
        renderDrawable_.reset();
        DrawableV2::RSRenderNodeDrawableAdapter::RemoveDrawableFromCache(GetId());
    }
    if (drawableVec_) {
        drawableVec_.reset();
        drawableVecStatus_ = 0;
#endif
}

bool RSRenderNode::IsNodeMemClearEnable()
{
#ifdef NOT_BUILD_FOR_OHOS_SDK
        return RSSystemProperties::GetNodeMemClearEnabled() && GetType() == RSRenderNodeType::CANVAS_NODE
        && RSProperties::IS_UNI_RENDER && !isTextureExportNode_;
#else
        return false;
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
    SetHasChildExcludedFromNodeGroup(false);
    SetChildHasVisibleHDRContent(false);
    ResetNodeColorSpace();
    SetForceDisableNodeGroup(false);
    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(shared_from_this(), false);
}

void RSRenderNode::ResetPixelStretchSlot()
{
    RSDrawable::ResetPixelStretchSlot(*this, GetDrawableVec(__func__));
}

bool RSRenderNode::CanFuzePixelStretch()
{
    return RSDrawable::CanFusePixelStretch(GetDrawableVec(__func__));
}

bool RSRenderNode::IsPixelStretchValid() const
{
    if (!GetRenderProperties().GetPixelStretch().has_value()) {
        return false;
    }
    const auto drawablePtr = findMapValueRef(GetDrawableVec(__func__),
        static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH));
    const auto pixelStretchDrawable = std::static_pointer_cast<const DrawableV2::RSPixelStretchDrawable>(drawablePtr);
    if (pixelStretchDrawable == nullptr) {
        return false;
    }
    const auto& pixelStretch = pixelStretchDrawable->GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }
    return !std::isinf(pixelStretch->x_) &&
        !std::isinf(pixelStretch->y_) && !std::isinf(pixelStretch->z_) && !std::isinf(pixelStretch->w_);
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
        child->SetIsOnTheTree(true, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_, uifirstRootNodeId_,
            screenNodeId_, logicalDisplayNodeId_);
        ShowSetIsOnetheTreeCntIfNeed(__func__, GetId(), GetNodeName());
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
        ((RSSystemProperties::GetOptimizeParentNodeRegionEnabled() && GetType() == RSRenderNodeType::SURFACE_NODE) ||
            GetNeedUseCmdlistDrawRegion())
            ? SetParentSubTreeDirty()
            : parentNode->SetContentDirty();
        AddSubSurfaceUpdateInfo(nullptr, parentNode);
    }
    SetIsOnTheTree(false);
    ShowSetIsOnetheTreeCntIfNeed(__func__, GetId(), GetNodeName());
    parent_.reset();
    OnResetParent();
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
    if (auto displayNode = ReinterpretCastTo<RSScreenRenderNode>()) {
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
        if (surfaceNode->IsSubSurfaceNode()) {
            out += ", isSubSurfaceId[" + std::to_string(GetId()) + "]";
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
        out += ", persistId [" + std::to_string(surfaceNode->GetLeashPersistentId()) + "]";
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
    out += ", NodeColorSpace: " + std::to_string(GetNodeColorSpace());
    out += ", Properties: " + GetRenderProperties().Dump();
    if (!uiContextTokenList_.empty()) {
        out += ", RSUIContextToken: [";
        for (const auto& token : uiContextTokenList_) {
            out += " " + std::to_string(token);
        }
        out += " ]";
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
    if (auto drawRegion = GetRenderProperties().GetDrawRegion()) {
        if (!drawRegion->IsEmpty()) {
            out += ", drawRegion: " + drawRegion->ToString();
        }
    }
    if (drawableVecStatus_ != 0) {
        out += ", drawableVecStatus: " + std::to_string(drawableVecStatus_);
    }
#ifdef SUBTREE_PARALLEL_ENABLE
    if (isRepaintBoundary_) {
        out += ", RB: true";
    }
#endif
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
        case RSRenderNodeType::SCREEN_NODE: {
            out += "SCREEN_NODE";
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
        case RSRenderNodeType::LOGICAL_DISPLAY_NODE: {
            out += "LOGICAL_DISPLAY_NODE";
            break;
        }
        case RSRenderNodeType::UNION_NODE: {
            out += "UNION_NODE";
            break;
        }
        case RSRenderNodeType::WINDOW_KEYFRAME_NODE: {
            out += "WINDOW_KEYFRAME_NODE";
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
        out += ", Alpha: " + std::to_string(surfaceNode->GetRenderProperties().GetAlpha());
        if (surfaceNode->contextAlpha_ < 1.0f) {
            std::string contextAlpha = std::to_string(surfaceNode->contextAlpha_);
            out += " (ContextAlpha: " + contextAlpha + ")";
        }
        out += ", Visible: " + std::to_string(surfaceNode->GetRenderProperties().GetVisible());
        out += ", Visible" + surfaceNode->GetVisibleRegion().GetRegionInfo();
        out += ", Opaque" + surfaceNode->GetOpaqueRegion().GetRegionInfo();
        out += ", OcclusionBg: " + std::to_string(surfaceNode->GetAbilityBgAlpha());
        out += ", IsContainerTransparent: " + std::to_string(surfaceNode->IsContainerWindowTransparent());
        const auto specialLayerManager = surfaceNode->GetSpecialLayerMgr();
        out += ", SpecialLayer: " + std::to_string(specialLayerManager.Get());
        out += ", surfaceType: " + std::to_string((int)surfaceNode->GetSurfaceNodeType());
        out += ", ContainerConfig: " + surfaceNode->GetContainerConfigDump();
        out += ", colorSpace: " + std::to_string(surfaceNode->GetColorSpace());
        out += ", uifirstColorGamut: " + std::to_string(surfaceNode->GetFirstLevelNodeColorGamut());
        out += ", isSurfaceBufferOpaque: " + std::to_string(surfaceNode->GetSurfaceBufferOpaque());
    } else if (GetType() == RSRenderNodeType::ROOT_NODE) {
        auto rootNode = static_cast<const RSRootRenderNode*>(this);
        out += ", Visible: " + std::to_string(rootNode->GetRenderProperties().GetVisible());
        out += ", Size: [" + std::to_string(rootNode->GetRenderProperties().GetFrameWidth()) + ", " +
            std::to_string(rootNode->GetRenderProperties().GetFrameHeight()) + "]";
        out += ", EnableRender: " + std::to_string(rootNode->GetEnableRender());
    } else if (GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE) {
        auto displayNode = static_cast<const RSLogicalDisplayRenderNode*>(this);
        out += ", skipLayer: " + std::to_string(displayNode->GetSecurityDisplay());
        out += ", securityExemption: " + std::to_string(displayNode->GetSecurityExemption());
        out += ", virtualScreenMuteStatus: " + std::to_string(displayNode->GetVirtualScreenMuteStatus());
    } else if (GetType() == RSRenderNodeType::WINDOW_KEYFRAME_NODE) {
        auto wndKeyframeNode = static_cast<const RSWindowKeyFrameRenderNode*>(this);
        out += ", linkedNodeId: " + std::to_string(wndKeyframeNode->GetLinkedNodeId());
    }
}

void RSRenderNode::DumpDrawCmdModifiers(std::string& out) const
{
    const std::string splitStr = ", ";
    std::string modifierDesc = "";
    for (const auto& [_, slot] : modifiersNG_) {
        for (auto& modifier : slot) {
            if (!modifier->IsCustom()) {
                continue;
            }
            modifier->Dump(modifierDesc, splitStr);
        }
    }
    if (modifierDesc.empty()) {
        return;
    }
    modifierDesc = ", DrawCmdModifiers2:[" + modifierDesc;
    out += modifierDesc.substr(0, modifierDesc.length() - splitStr.length()) + "]";
}

void RSRenderNode::DumpModifiers(std::string& out) const
{
    const std::string splitStr = ", ";
    std::string propertyDesc = "";
    for (const auto& [_, slot] : modifiersNG_) {
        for (auto& modifier : slot) {
            if (modifier->IsCustom()) {
                continue;
            }
            propertyDesc = propertyDesc + "pid:" + std::to_string(ExtractPid(modifier->GetId())) + "->";
            modifier->Dump(propertyDesc, splitStr);
        }
    }
    if (propertyDesc.empty()) {
        return;
    }
    out += ", OtherModifiers:[" + propertyDesc.substr(0, propertyDesc.length() - splitStr.length()) + "]";
}

bool RSRenderNode::IsOnlyBasicGeoTransform() const
{
    return isOnlyBasicGeoTransform_;
}

void RSRenderNode::ForceMergeSubTreeDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect)
{
    // prepare skip -> quick prepare, old dirty do not update
    if (geoUpdateDelay_) {
        if (auto& geoPtr = GetRenderProperties().GetBoundsGeometry()) {
            auto absChildrenRect = geoPtr->MapRect(oldChildrenRect_.ConvertTo<float>(), oldAbsMatrix_);
            subTreeDirtyRegion_ = absChildrenRect.IntersectRect(oldClipRect_);
            dirtyManager.MergeDirtyRect(subTreeDirtyRegion_);
        }
    }
    lastFrameSubTreeSkipped_ = false;
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
    // TO avoid redundant add, only add if both: 1. on-tree node 2. newly dirty node (or forceAddToActiveList = true)
    if (dirtyStatus_ == NodeDirty::CLEAN || dirtyTypesNG_.none() || forceAddToActiveList) {
        if (auto context = GetContext().lock()) {
            context->AddActiveNode(shared_from_this());
        }
    }
    isParentTreeDirty_ = true;
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
    if (!shouldPaint_ || (isOccluded && !IsFirstLevelCrossNode() && !IsTreeStateChangeDirty())) {
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
        SetTreeStateChangeDirty(false);
        UpdateChildrenOutOfRectFlag(false); // collect again
        return true;
    }
    if (childHasSharedTransition_ || isAccumulatedClipFlagChanged_ || subSurfaceCnt_ > 0) {
        return true;
    }
    if (RSPointLightManager::Instance()->GetChildHasVisibleIlluminated(shared_from_this())) {
        return true;
    }
    if (ChildHasVisibleFilter()) {
        RS_OPTIONAL_TRACE_NAME_FMT("IsSubTreeNeedPrepare node[%lu] filterInGlobal_[%d]",
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
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (IsNodeMemClearEnable()) {
        ROSEN_LOGD("RSRenderNode::PrepareSelfNodeForApplyModifiers, node[id:%{public}" PRIu64 "]", GetId());
        InitRenderDrawableAndDrawableVec();
    }
#endif
    ApplyModifiers();
    PrepareChildrenForApplyModifiers();

    stagingRenderParams_->SetAlpha(GetRenderProperties().GetAlpha());

    UpdateRenderParams();
    AddToPendingSyncList();
#endif
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
        return;
    }
    SetDrawingCacheChanged((IsContentDirty() || IsSubTreeDirty() || IsAccessibilityConfigChanged()));
    RS_OPTIONAL_TRACE_NAME_FMT(
        "SetDrawingCacheChanged id:%llu nodeGroupType:%u contentDirty:%d propertyDirty:%d subTreeDirty:%d "
        "AccessibilityConfigChanged:%d",
        GetId(), nodeGroupType_, isContentDirty_, GetRenderProperties().IsContentDirty(), IsSubTreeDirty(),
        IsAccessibilityConfigChanged());
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetDrawingCacheIncludeProperty(nodeGroupIncludeProperty_);
#endif
    // renderGroup memory tagTracer
    auto instanceRootNode = GetInstanceRootNode();
    if (instanceRootNode) {
        auto appWindow = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (appWindow && appWindow->IsAppWindow()) {
            stagingRenderParams_->SetInstanceRootNodeId(appWindow->GetId());
            stagingRenderParams_->SetInstanceRootNodeName(appWindow->GetName());
        }
    }
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
    RS_PROFILER_RENDERNODE_DEC(isOnTheTree_);
    if (appPid_ != 0) {
        RSSingleFrameComposer::AddOrRemoveAppPidToMap(false, appPid_);
    }
    FallbackAnimationsToRoot();
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
        displaySync_ = std::make_unique<RSRenderDisplaySync>(GetId());
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
    RS_PROFILER_ANIMATION_NODE(GetType(), selfDrawRect_.GetWidth() * selfDrawRect_.GetWidth());
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
    auto& renderProperties = GetRenderProperties();
    return renderProperties.GetClipToBounds() || renderProperties.GetClipToFrame() ||
        renderProperties.GetClipToRRect() || renderProperties.GetClipBounds() != nullptr;
}

bool RSRenderNode::SetAccumulatedClipFlag(bool clipChange)
{
    isAccumulatedClipFlagChanged_ = (hasAccumulatedClipFlag_ != IsClipBound()) || clipChange;
    if (isAccumulatedClipFlagChanged_) {
        hasAccumulatedClipFlag_ = IsClipBound();
    }
    return isAccumulatedClipFlagChanged_;
}

void RSRenderNode::ShowSetIsOnetheTreeCntIfNeed(const std::string& funcName, NodeId nodeId, const std::string& nodeName)
{
    if (g_setIsOntheTreeCnt > SET_IS_ON_THE_TREE_THRESHOLD) {
        RS_TRACE_NAME_FMT("SetIsOnetheTreeCnt too many funcName is %s count is %d nodeId is %" PRIu64 " name is %s",
            funcName.c_str(),
            g_setIsOntheTreeCnt,
            nodeId,
            nodeName.c_str());
    }
    g_setIsOntheTreeCnt = 0;
}

#ifdef RS_ENABLE_GPU
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

RectI RSRenderNode::GetDrawCmdListRect() const
{
    int32_t width = 0;
    int32_t height = 0;
    RSRenderNode::ModifierNGContainer customModifiers[] = {
        GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE),
        GetModifiersNG(ModifierNG::RSModifierType::TRANSITION_STYLE),
        GetModifiersNG(ModifierNG::RSModifierType::BACKGROUND_STYLE),
        GetModifiersNG(ModifierNG::RSModifierType::FOREGROUND_STYLE),
        GetModifiersNG(ModifierNG::RSModifierType::OVERLAY_STYLE),
        GetModifiersNG(ModifierNG::RSModifierType::NODE_MODIFIER),
    };

    for (auto& slot : customModifiers) {
        for (auto& modifier : slot) {
            auto propertyType = ModifierNG::ModifierTypeConvertor::GetPropertyType(modifier->GetType());
            auto propertyPtr = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(
                modifier->GetProperty(propertyType));
            auto drawCmdListPtr = propertyPtr ? propertyPtr->Get() : nullptr;
            if (drawCmdListPtr == nullptr) {
                continue;
            }
            width = width > drawCmdListPtr->GetWidth() ? width : drawCmdListPtr->GetWidth();
            height = height > drawCmdListPtr->GetHeight() ? height : drawCmdListPtr->GetHeight();
        }
    }
    return RectI(0, 0, width, height);
}

void RSRenderNode::CollectAndUpdateRenderFitRect()
{
    const auto& properties = GetRenderProperties();
    if (properties.GetFrameGravity() == Gravity::TOP_LEFT) {
        return;
    }
    auto drawRegion = properties.GetDrawRegion();
    if (drawRegion == nullptr) {
        return;
    }
    auto drawCmdListRect = GetDrawCmdListRect();
    if (drawCmdListRect.IsEmpty()) {
        return;
    }
    Drawing::Matrix mat;
    RSPropertiesPainter::GetGravityMatrix(properties.GetFrameGravity(),
        properties.GetFrameRect(), drawCmdListRect.GetWidth(), drawCmdListRect.GetHeight(), mat);
    Drawing::Rect srcRegion(
        drawRegion->GetLeft(), drawRegion->GetTop(), drawRegion->GetRight(), drawRegion->GetBottom());
    Drawing::Rect dstRegion;
    if (mat.MapRect(dstRegion, srcRegion)) {
        selfDrawRect_ = selfDrawRect_.JoinRect(
            RectF(dstRegion.GetLeft(), dstRegion.GetTop(), dstRegion.GetWidth(), dstRegion.GetHeight()));
    }
}

void RSRenderNode::CollectAndUpdateLocalShadowRect()
{
    // update shadow if shadow changes
    RectI localShadowRect;
    if (dirtySlots_.find(RSDrawableSlot::SHADOW) != dirtySlots_.end()) {
        auto& properties = GetRenderProperties();
        if (properties.IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                RRect absClipRRect = RRect(properties.GetBoundsRect(), properties.GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(localShadowRect, properties, &absClipRRect, false, true);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(localShadowRect, properties, nullptr, false, true);
            }
        }
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localShadowRect.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalOutlineRect()
{
    // update outline if oueline changes
    RectI localOutlineRect;
    if (dirtySlots_.find(RSDrawableSlot::OUTLINE) != dirtySlots_.end()) {
        RSPropertiesPainter::GetOutlineDirtyRect(localOutlineRect, GetRenderProperties(), false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localOutlineRect.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalPixelStretchRect()
{
    // update outline if oueline changes
    RectI localPixelStretchRect;
    if (dirtySlots_.find(RSDrawableSlot::PIXEL_STRETCH) != dirtySlots_.end()) {
        RSPropertiesPainter::GetPixelStretchDirtyRect(localPixelStretchRect, GetRenderProperties(), false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localPixelStretchRect.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalForegroundEffectRect()
{
    // update foreground effect's dirty region if it changes
    RectI localForegroundEffectRect;
    if (GetRenderProperties().GetForegroundEffectDirty()) {
        RSPropertiesPainter::GetForegroundEffectDirtyRect(localForegroundEffectRect, GetRenderProperties(), false);
        GetMutableRenderProperties().SetForegroundEffectDirty(false);
    }
    if (GetRenderProperties().GetForegroundNGFilter()) {
        RSPropertiesPainter::GetForegroundNGFilterDirtyRect(localForegroundEffectRect, GetRenderProperties(), false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localForegroundEffectRect.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalDistortionEffectRect()
{
    // update distortion effect's dirty region if it changes
    RectI localDistortionEffectRect;
    if (GetRenderProperties().GetDistortionDirty()) {
        RSPropertiesPainter::GetDistortionEffectDirtyRect(localDistortionEffectRect, GetRenderProperties());
        GetMutableRenderProperties().SetDistortionDirty(false);
    }
    selfDrawRect_ = selfDrawRect_.JoinRect(localDistortionEffectRect.ConvertTo<float>());
}

void RSRenderNode::CollectAndUpdateLocalMagnifierEffectRect()
{
    // update magnifier effect's dirty region if it changes
    if (GetRenderProperties().GetMagnifierDirty()) {
        RectI localMagnifierEffectRect;
        RSPropertiesPainter::GetMagnifierEffectDirtyRect(localMagnifierEffectRect, GetRenderProperties());
        selfDrawRect_ = selfDrawRect_.JoinRect(localMagnifierEffectRect.ConvertTo<float>());
    }
}

void RSRenderNode::CollectAndUpdateLocalEffectRect()
{
    auto boundsRect = GetRenderProperties().GetBoundsRect();
    for (auto slot : filterDrawableSlotsSupportGetRect) {
        if (dirtySlots_.find(slot) == dirtySlots_.end()) {
            continue;
        }
        std::shared_ptr<RSFilter> filter = GetRSFilterWithSlot(slot);
        if (filter == nullptr) {
            continue;
        }
        selfDrawRect_ = selfDrawRect_.JoinRect(filter->GetRect(boundsRect, EffectRectType::TOTAL));
    }
}

void RSRenderNode::UpdateBufferDirtyRegion(RectF& selfDrawingNodeDirtyRect)
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
        // if the buffer size changed, use the node size as dirty rect
        if (surfaceNode->GetRSSurfaceHandler()->GetBufferSizeChanged()) {
            selfDrawingNodeDirtyRect = selfDrawRect_;
            RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode id: %" PRIu64 ", buffer size changed.", GetId());
            return;
        }
        // Use the matrix from buffer to relative coordinate and the absolute matrix
        // to calculate the buffer damageRegion's absolute rect
        auto rect = surfaceNode->GetRSSurfaceHandler()->GetDamageRegion();
        bool isUseSelfDrawingDirtyRegion = buffer->GetSurfaceBufferWidth() == rect.w &&
            buffer->GetSurfaceBufferHeight() == rect.h && rect.x == 0 && rect.y == 0;
        if (isUseSelfDrawingDirtyRegion) {
            Rect selfDrawingDirtyRect;
            bool isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, selfDrawingDirtyRect);
            if (isDirtyRectValid) {
                rect = { selfDrawingDirtyRect.x, selfDrawingDirtyRect.y,
                    selfDrawingDirtyRect.w, selfDrawingDirtyRect.h };
                RS_OPTIONAL_TRACE_NAME_FMT("selfDrawingDirtyRect:[%d, %d, %d, %d]", rect.x, rect.y, rect.w, rect.h);
            }
        }
        auto matrix = surfaceNode->GetBufferRelMatrix();
        auto bufferDirtyRect = GetRenderProperties().GetBoundsGeometry()->MapRect(
            RectF(rect.x, rect.y, rect.w, rect.h), matrix).ConvertTo<float>();
        // The buffer's dirtyRect should not be out of the scope of the node's dirtyRect
        selfDrawingNodeDirtyRect = bufferDirtyRect.IntersectRect(selfDrawRect_);
        RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode id: %" PRIu64 ", buffer size [%d,%d], "
            "buffer damageRegion [%d,%d,%d,%d], dirtyRect %s", GetId(),
            buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
            rect.x, rect.y, rect.w, rect.h, selfDrawingNodeDirtyRect.ToString().c_str());
    }
#endif
}

bool RSRenderNode::UpdateSelfDrawRect(RectF& selfDrawingNodeDirtyRect)
{
    auto prevSelfDrawRect = selfDrawRect_;
    // empty rect would not join and doesn't need to check
    auto& properties = GetRenderProperties();
    selfDrawRect_ = properties.GetLocalBoundsAndFramesRect();
    UpdateBufferDirtyRegion(selfDrawingNodeDirtyRect);
    if (auto drawRegion = properties.GetDrawRegion()) {
        selfDrawRect_ = selfDrawRect_.JoinRect(*drawRegion);
    }
    CollectAndUpdateRenderFitRect();
    CollectAndUpdateLocalShadowRect();
    CollectAndUpdateLocalOutlineRect();
    CollectAndUpdateLocalPixelStretchRect();
    CollectAndUpdateLocalDistortionEffectRect();
    CollectAndUpdateLocalMagnifierEffectRect();
    CollectAndUpdateLocalEffectRect();
    return !selfDrawRect_.IsNearEqual(prevSelfDrawRect);
}

void RSRenderNode::UpdateAbsDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect,
    RectI& selfDrawingNodeAbsDirtyRect, RectI& absCmdlistDrawRect)
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
            selfDrawingNodeAbsDirtyRect.JoinRect(absDrawRect_);
        }
        oldAbsDrawRect_ = absDrawRect_;
    }
    // easily merge oldDirty if switch to invisible
    if (!shouldPaint_ && isLastVisible_) {
        dirtyManager.MergeDirtyRect(oldDirtyRect);
        return;
    }
    auto dirtyRect = isSelfDrawingNode_ ? selfDrawingNodeAbsDirtyRect :
        (!absCmdlistDrawRect.IsEmpty() ? absCmdlistDrawRect : absDrawRect_);
    dirtyRect = IsFirstLevelCrossNode() ? dirtyRect : dirtyRect.IntersectRect(clipRect);
    oldDirty_ = dirtyRect;
    oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
    dirtyManager.MergeDirtyRect(dirtyRect.JoinRect(oldDirtyRect));
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
    RectF selfDrawingNodeDirtyRect;
    UpdateFilterRectInfo();
    bool selfDrawRectChanged = IsDirty() ? UpdateSelfDrawRect(selfDrawingNodeDirtyRect) : false;
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
    RectI selfDrawingNodeAbsDirtyRect;
    RectI absCmdlistDrawRect;
    if (accumGeoDirty || properties.NeedClip() || properties.geoDirty_ || (dirtyStatus_ != NodeDirty::CLEAN)) {
        UpdateDrawRect(accumGeoDirty, clipRect, parentSurfaceMatrix);
        // planning: double check if it would be covered by updateself without geo update
        auto& geoPtr = properties.boundsGeo_;
        // selfdrawing node's geo may not dirty when its dirty region changes
        // updateDrawRect info when this node need to use cmdlistDrawRegion
        if (geoPtr && (accumGeoDirty || properties.geoDirty_ ||
            isSelfDrawingNode_ || selfDrawRectChanged || GetNeedUseCmdlistDrawRegion())) {
            absDrawRectF_ = geoPtr->MapRectWithoutRounding(selfDrawRect_, geoPtr->GetAbsMatrix());
            absDrawRect_ = geoPtr->InflateToRectI(absDrawRectF_);
            innerAbsDrawRect_ = geoPtr->DeflateToRectI(absDrawRectF_);
            absCmdlistDrawRect = GetNeedUseCmdlistDrawRegion() ?
                geoPtr->MapRect(cmdlistDrawRegion_, geoPtr->GetAbsMatrix()) : RectI(0, 0, 0, 0);
            if (isSelfDrawingNode_) {
                selfDrawingNodeAbsDirtyRect = geoPtr->MapAbsRect(selfDrawingNodeDirtyRect);
            }
            UpdateSrcOrClipedAbsDrawRectChangeState(clipRect);
        }
    }
    // 3. update dirtyRegion if needed
    if (properties.GetMaterialFilter()) {
        UpdateFilterCacheWithBelowDirty(Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()),
            RSDrawableSlot::MATERIAL_FILTER);
        UpdatePendingPurgeFilterDirtyRect(dirtyManager, RSDrawableSlot::MATERIAL_FILTER);
    }
    if (properties.GetBackgroundFilter()) {
        UpdateFilterCacheWithBelowDirty(Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()));
        UpdatePendingPurgeFilterDirtyRect(dirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    }
    ValidateLightResources();
    isDirtyRegionUpdated_ = false;
    // Only when satisfy following conditions, absDirtyRegion should update:
    // 1.The node is dirty; 2.The clip absDrawRect change; 3.Parent clip property change or has GeoUpdateDelay dirty;
    // When the subtree is all dirty and the node should not paint, it also needs to add dirty region
    if ((IsDirty() || srcOrClipedAbsDrawRectChangeFlag_ || (parent && (parent->GetAccumulatedClipFlagChange() ||
        parent->GetGeoUpdateDelay()))) && (shouldPaint_ || isLastVisible_ || properties.IsSubTreeAllDirty())) {
        // update ForegroundFilterCache
        UpdateAbsDirtyRegion(dirtyManager, clipRect, selfDrawingNodeAbsDirtyRect, absCmdlistDrawRect);
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
    }

    // update dirty region information in abs Coords.
    dirtyManager.UpdateDirtyRegionInfoForDfx(
        GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
    dirtyManager.UpdateDirtyRegionInfoForDfx(
        GetId(), GetType(), DirtyRegionType::FILTER_RECT, GetFilterRegion());
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
    isDirtyRegionUpdated_ = false;
    isLastVisible_ = ShouldPaint();
    GetMutableRenderProperties().ResetDirty();

    // Note:
    // 1. cache manager will use dirty region to update cache validity, background filter cache manager should use
    // 'dirty region of all the nodes drawn before this node', and foreground filter cache manager should use 'dirty
    // region of all the nodes drawn before this node, this node, and the children of this node'
    // 2. Filter must be valid when filter cache manager is valid, we make sure that in RSRenderNode::ApplyModifiers().
    if (GetRenderProperties().GetMaterialFilter()) {
        UpdateFilterCacheWithBelowDirty(Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()),
            RSDrawableSlot::MATERIAL_FILTER);
    }
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
        if (properties.GetPixelStretch()) {
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

void RSRenderNode::SetParentSubTreeDirty()
{
    auto parentNode = parent_.lock();
    if (parentNode && !parentNode->IsSubTreeDirty()) {
        parentNode->SetSubTreeDirty(true);
        // Only used in quick skip prepare phase
        parentNode->SetForcePrepare(true);
        parentNode->SetParentSubTreeDirty();
    }
}

void RSRenderNode::SetParentTreeStateChangeDirty(bool isUpdateAllParentNode)
{
    auto parentNode = parent_.lock();
    if (!parentNode) {
        return;
    }
    if (parentNode && (isUpdateAllParentNode || !parentNode->IsTreeStateChangeDirty())) {
        parentNode->SetTreeStateChangeDirty(true);
        parentNode->SetParentTreeStateChangeDirty(isUpdateAllParentNode);
    }
}

void RSRenderNode::SetChildrenTreeStateChangeDirty()
{
    auto sortedChildren = GetSortedChildren();
    for (auto& child : *sortedChildren) {
        child->SetTreeStateChangeDirty(true);
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
    std::vector<RSDrawableSlot> cacheDrawableSlots{RSDrawableSlot::COMPOSITING_FILTER,
        RSDrawableSlot::BACKGROUND_FILTER, RSDrawableSlot::MATERIAL_FILTER};
    for (auto& slot : cacheDrawableSlots) {
        auto filterDrawable = GetFilterDrawable(slot);
        if (filterDrawable != nullptr) {
            return filterDrawable->IsFilterCacheValid();
        }
    }
    return false;
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

bool RSRenderNode::CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage) const
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
    return filterDrawable->CheckAndUpdateAIBarCacheStatus(intersectHwcDamage);
#endif
return false;
}

bool RSRenderNode::ForceReduceAIBarCacheInterval(bool intersectHwcDamage)
{
#ifdef RS_ENABLE_GPU
    if (!RSSystemProperties::GetBlurEnabled() || !RSProperties::filterCacheEnabled_) {
        ROSEN_LOGD("ForceReduceAIBarCacheInterval: blur is disabled or filter cache is disabled.");
        return false;
    }

    auto filterDrawable = GetFilterDrawable(false); // AIBar filter is only background filter
    if (filterDrawable == nullptr) {
        return false;
    }
    return filterDrawable->ForceReduceAIBarCacheInterval(intersectHwcDamage);
#else
    return false;
#endif
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
    lastFilterRegion_ = GetFilterRegionInfo().filterRegion_;
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
    auto boundsRect = GetRenderProperties().GetBoundsRect();
    auto totalRect = RSObjAbsGeometry::MapRect(boundsRect, absMatrix);
    CalVisibleFilterRect(totalRect, absMatrix, clipRect);
    filterRect = GetFilterRegionInfo().filterRegion_;
}

void RSRenderNode::FilterRectMergeDirtyRectInSkippedSubtree(RSDirtyRegionManager& dirtyManager, const RectI& filterRect)
{
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
    if (IsInstanceOf<RSEffectRenderNode>()) {
        rotationStatusChanged = false;
    }
    auto bgDirty = dirtySlots_.count(RSDrawableSlot::BACKGROUND_COLOR) ||
            dirtySlots_.count(RSDrawableSlot::BACKGROUND_SHADER) ||
            dirtySlots_.count(RSDrawableSlot::BACKGROUND_IMAGE) ||
            dirtySlots_.count(RSDrawableSlot::MATERIAL_FILTER);
    const auto& properties = GetRenderProperties();

    RSCacheFilterParaArray filterCacheHandles {
        RSCacheFilterPara{properties.GetMaterialFilter() != nullptr && rotationClear,
            RSDrawableSlot::MATERIAL_FILTER},
        RSCacheFilterPara{properties.GetBackgroundFilter() != nullptr && (rotationClear || bgDirty),
            RSDrawableSlot::BACKGROUND_FILTER},
        RSCacheFilterPara{properties.GetFilter() != nullptr &&
            (rotationStatusChanged || !dirtySlots_.empty()), RSDrawableSlot::COMPOSITING_FILTER}
    };

    for (auto& p : filterCacheHandles) {
        auto filterDrawable = GetFilterDrawable(p.second);
        if (filterDrawable && !(filterDrawable->IsForceClearFilterCache()) && p.first) {
            RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode[%llu] background color or shader or"
                " image or meterial is dirty", GetId());
            filterDrawable->MarkFilterForceClearCache();
        }
    }
#endif
}

void RSRenderNode::MarkFilterInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
#ifdef RS_ENABLE_GPU
    const auto& properties = GetRenderProperties();
    auto invokeFunc = [offscreenCanvasNodeId] (
        std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable) {
        if (filterDrawable != nullptr) {
            filterDrawable->MarkInForegroundFilterAndCheckNeedForceClearCache(offscreenCanvasNodeId);
        }
    };
    if (properties.GetMaterialFilter()) {
        RSRenderNode::InvokeFilterDrawable(RSDrawableSlot::MATERIAL_FILTER, invokeFunc);
    }
    if (properties.GetBackgroundFilter()) {
        RSRenderNode::InvokeFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER, invokeFunc);
    }
    if (properties.GetFilter()) {
        auto filterDrawable = GetFilterDrawable(RSDrawableSlot::COMPOSITING_FILTER);
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
void RSRenderNode::MarkFilterStatusChanged(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
    bool isForeground, bool isFilterRegionChanged)
{
#ifdef RS_ENABLE_GPU
    if (filterDrawable == nullptr) {
        return;
    }
    if (!isForeground) {
        auto& flag = isFilterRegionChanged ? backgroundFilterRegionChanged_ : backgroundFilterInteractWithDirty_;
        flag = true;
    }
    isFilterRegionChanged ?
        filterDrawable->MarkFilterRegionChanged() : filterDrawable->MarkFilterRegionInteractWithDirty();
#endif
}
#ifdef RS_ENABLE_GPU
std::shared_ptr<DrawableV2::RSFilterDrawable> RSRenderNode::GetFilterDrawable(bool isForeground) const
{
    auto slot = isForeground ? RSDrawableSlot::COMPOSITING_FILTER : RSDrawableSlot::BACKGROUND_FILTER;
    if (auto& drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot))) {
        if (auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable)) {
            return filterDrawable;
        }
    }
    return nullptr;
}

std::shared_ptr<DrawableV2::RSFilterDrawable> RSRenderNode::GetFilterDrawable(RSDrawableSlot slot) const
{
    if (filterDrawableSlotsSupportGetRect.find(slot) == filterDrawableSlotsSupportGetRect.end()) {
        return nullptr;
    }
    if (auto& drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot))) {
        if (auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable)) {
            return filterDrawable;
        }
    }
    return nullptr;
}

bool RSRenderNode::InvokeFilterDrawable(RSDrawableSlot slot,
    std::function<void(std::shared_ptr<DrawableV2::RSFilterDrawable>)> checkMethodInvokeFunc)
{
    if (checkMethodInvokeFunc == nullptr) {
        return false;
    }
    auto drawable = GetFilterDrawable(slot);
    if (drawable == nullptr) {
        return false;
    }
    checkMethodInvokeFunc(drawable);
    return true;
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
    auto hasBackground =
        findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::BACKGROUND_COLOR)) ||
        findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::BACKGROUND_SHADER)) ||
        findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::BACKGROUND_IMAGE));
    auto alphaDirty = dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::ALPHA));
    if (alphaDirty && hasBackground) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSRenderNode[%llu] background color or shader or image is dirty due to changes in alpha", GetId());
        filterDrawable->MarkFilterForceClearCache();
    }
#endif
}

bool RSRenderNode::UpdateFilterCacheWithBelowDirty(const Occlusion::Region& belowDirty, RSDrawableSlot slot)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSProperties::filterCacheEnabled_) {
#ifndef ROSEN_ARKUI_X
        ROSEN_LOGE("RSRenderNode::UpdateFilterCacheWithBelowDirty filter cache is disabled.");
#endif
        return false;
    }
    auto filterDrawable = GetFilterDrawable(slot);
    if (filterDrawable == nullptr || IsForceClearOrUseFilterCache(filterDrawable)) {
        return false;
    }
    auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(lastFilterRegion_);
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateFilterCacheWithBelowDirty:node[%llu] drawable:%d, lastRect:%s, dirtyRegion:%s",
        GetId(), static_cast<int>(slot), lastSnapshotRegion.ToString().c_str(), belowDirty.GetRegionInfo().c_str());
    if (!belowDirty.IsIntersectWith(lastSnapshotRegion)) {
        return false;
    }
    MarkFilterStatusChanged(filterDrawable, RSDrawableSlot::COMPOSITING_FILTER == slot, false);
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

    const auto& properties = GetRenderProperties();
    auto invokeFunc = [&properties, this] (std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable) {
        if (filterDrawable != nullptr) {
            auto snapshotRegion = filterDrawable->GetVisibleSnapshotRegion(GetFilterRegionInfo().defaultFilterRegion_);
            auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(lastFilterRegion_);
            bool regionChanged = ((!snapshotRegion.IsInsideOf(lastSnapshotRegion)) ||
                (properties.GetNeedDrawBehindWindow() && snapshotRegion != lastSnapshotRegion)) &&
                !IsForceClearOrUseFilterCache(filterDrawable);
            RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] background UpdateFilterCacheWithSelfDirty"
                " lastRect:%s, currRegion:%s", GetId(), lastSnapshotRegion.ToString().c_str(),
                snapshotRegion.ToString().c_str());
            if (regionChanged) {
                MarkFilterStatusChanged(filterDrawable, false, true);
            }
        }
    };
    RSRenderNode::InvokeFilterDrawable(RSDrawableSlot::MATERIAL_FILTER, invokeFunc);
    RSRenderNode::InvokeFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER, invokeFunc);
    auto filterDrawable = GetFilterDrawable(RSDrawableSlot::COMPOSITING_FILTER);
    if (filterDrawable != nullptr) {
        auto snapshotRegion = filterDrawable->GetVisibleSnapshotRegion(GetFilterRegionInfo().defaultFilterRegion_);
        auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(lastFilterRegion_);
        bool regionChanged = (properties.GetFilter() && snapshotRegion != lastSnapshotRegion) &&
            !IsForceClearOrUseFilterCache(filterDrawable);
        RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] compositing UpdateFilterCacheWithSelfDirty lastRect:%s, currRegion:%s",
            GetId(), lastSnapshotRegion.ToString().c_str(), snapshotRegion.ToString().c_str());
        if (regionChanged) {
            MarkFilterStatusChanged(filterDrawable, true, true);
        }
    }
#endif
}

void RSRenderNode::UpdatePendingPurgeFilterDirtyRect(RSDirtyRegionManager& dirtyManager, RSDrawableSlot slot)
{
    auto invokeFunc = [slot, &dirtyManager, this] (
        std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable) {
        if (filterDrawable == nullptr || IsForceClearOrUseFilterCache(filterDrawable)) {
            return;
        }
        auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(lastFilterRegion_);
        const auto& pendingPurgeFilterRegion = dirtyManager.GetFilterCollector().GetPendingPurgeFilterRegion();
        if (slot != RSDrawableSlot::COMPOSITING_FILTER &&
            pendingPurgeFilterRegion.IsIntersectWith(Occlusion::Rect(lastSnapshotRegion))) {
            backgroundFilterInteractWithDirty_ = true;
        }

        RS_OPTIONAL_TRACE_NAME_FMT("UpdatePendingPurgeFilterDirtyRect:node[%llu],"
            " needPendingPurge:%d, lastHpaeClearCache: %d, pendingPurgeFilterRegion: %s",
            GetId(), filterDrawable->NeedPendingPurge(), filterDrawable->LastHpaeClearCache(),
            pendingPurgeFilterRegion.GetRegionInfo().c_str());
        // mainly based on the pendingPurge_ within NeedPendingPurge().
        // because stagingFilterInteractWithDirty_ (whether filter intersects with the dirty region)
        // also AddPendingPurgeFilterRegion if HpaeBlur clear cache
        // may become true later in CheckMergeFilterDirtyWithPreDirty().
        if (filterDrawable->NeedPendingPurge() || filterDrawable->LastHpaeClearCache()) {
            dirtyManager.GetFilterCollector().AddPendingPurgeFilterRegion(
                Occlusion::Region(Occlusion::Rect(lastFilterRegion_)));
        }
    };
    RSRenderNode::InvokeFilterDrawable(slot, invokeFunc);
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
    RSCacheFilterParaArray filterDrawableVec {
        RSCacheFilterPara{properties.GetMaterialFilter(),
            RSDrawableSlot::MATERIAL_FILTER},
        RSCacheFilterPara{properties.GetBackgroundFilter() || properties.GetNeedDrawBehindWindow(),
            RSDrawableSlot::BACKGROUND_FILTER},
        RSCacheFilterPara{properties.GetFilter() != nullptr, RSDrawableSlot::COMPOSITING_FILTER}
    };
    for (auto& p : filterDrawableVec) {
        auto filterDrawable = GetFilterDrawable(p.second);
        if (p.first && filterDrawable != nullptr) {
            MarkFilterCacheFlags(filterDrawable, dirtyManager, needRequestNextVsync);
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, p.second);
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

    auto filterRegion = filterDrawable->GetVisibleTotalRegion(GetFilterRegion());
    auto snapshotRegion = filterDrawable->GetVisibleSnapshotRegion(GetFilterRegion());
    RS_OPTIONAL_TRACE_NAME_FMT("MarkFilterCacheFlags:node[%llu], NeedPendingPurge:%d,"
        " forceClearWithoutNextVsync:%d, filterRegion: %s, filterDrawable snapshotRegion: %s",
        GetId(), filterDrawable->NeedPendingPurge(), (!needRequestNextVsync && filterDrawable->IsSkippingFrame()),
        filterRegion.ToString().c_str(), snapshotRegion.ToString().c_str());
    // force update if last frame hpae-blur clear cache
    if (filterDrawable->LastHpaeClearCache()) {
        dirtyManager.MergeDirtyRect(filterRegion);
        isDirtyRegionUpdated_ = true;
    }

    // force update if last frame use cache because skip-frame and current frame background is not dirty
    if (filterDrawable->NeedPendingPurge()) {
        dirtyManager.MergeDirtyRect(filterRegion);
        isDirtyRegionUpdated_ = true;
        return;
    }

    // when background changed, skip-frame will enabled if filter region > 400 and blur radius > 25
    if (IsLargeArea(snapshotRegion.GetWidth(), snapshotRegion.GetHeight())) {
        filterDrawable->MarkFilterRegionIsLargeArea();
    }

    // force update if no next vsync when skip-frame enabled
    if (!needRequestNextVsync && filterDrawable->IsSkippingFrame()) {
        filterDrawable->MarkForceClearCacheWithLastFrame();
        return;
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
    RSCacheFilterParaArray filterDrawableVec {
        RSCacheFilterPara{GetRenderProperties().GetMaterialFilter() != nullptr,  RSDrawableSlot::MATERIAL_FILTER},
        RSCacheFilterPara{GetRenderProperties().GetBackgroundFilter() != nullptr, RSDrawableSlot::BACKGROUND_FILTER},
        RSCacheFilterPara{GetRenderProperties().GetFilter() != nullptr, RSDrawableSlot::COMPOSITING_FILTER}
    };
    for (auto& p : filterDrawableVec) {
        auto filterDrawable = GetFilterDrawable(p.second);
        if (p.first && filterDrawable != nullptr) {
            filterDrawable->MarkFilterForceClearCache();
            CheckFilterCacheAndUpdateDirtySlots(filterDrawable, p.second);
        }
    }
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
    auto& drawablePtr = findMapValueRef(GetDrawableVec(__func__),
        static_cast<int8_t>(slot));
    if (!drawablePtr) {
        return;
    }

    auto recordingCanvas = canvas.GetRecordingCanvas();
    if (recordingCanvas == nullptr || !canvas.GetRecordDrawable()) {
        // non-recording canvas, draw directly
        Drawing::Rect rect = { 0, 0, GetRenderProperties().GetFrameWidth(), GetRenderProperties().GetFrameHeight() };
        drawablePtr->OnSync();
        drawablePtr->OnDraw(&canvas, &rect);
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

        auto& drawVec = GetDrawableVec(__func__);
        for (auto i = static_cast<int8_t>(begin);
            i <= static_cast<int8_t>(end); ++i) {
            auto ptr = findMapValueRef(drawVec, i);
            if (ptr) {
                ptr->OnSync();
                ptr->OnDraw(&canvas, &rect);
            }
        };
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

void RSRenderNode::RegisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property)
{
    if (property) {
        properties_.emplace(property->GetId(), property);
    }
}

void RSRenderNode::UnregisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property)
{
    if (property) {
        properties_.erase(property->GetId());
    }
}

void RSRenderNode::UnregisterProperty(PropertyId id)
{
    properties_.erase(id);
}

void RSRenderNode::AccumulateDirtyInOcclusion(bool isOccluded)
{
    if (isOccluded) {
        // accumulate dirtytypes for modifiers
        AccumulateDirtyTypes();
        // accumulate dirtystatus in rendernode
        AccumulateDirtyStatus();
        // accumulate dirtystatus in render properties(isDirty, geoDirty, contentDirty)
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
    if (curDirtyStatus_ != NodeDirty::CLEAN) {
        SetDirty();
        return;
    }
    if (isSubTreeDirty_) {
        SetParentSubTreeDirty();
    }
}

void RSRenderNode::ResetAccumulateDirtyStatus()
{
    dirtyStatus_ = NodeDirty::CLEAN;
    GetMutableRenderProperties().ResetDirty();
}

void RSRenderNode::RecordCurDirtyTypes()
{
    curDirtyTypesNG_ |= ~dirtyTypesNG_;
}

void RSRenderNode::AccumulateDirtyTypes()
{
    dirtyTypesNG_ |= ~curDirtyTypesNG_;
}

void RSRenderNode::ResetAccumulateDirtyTypes()
{
    dirtyTypesNG_.reset();
}

void RSRenderNode::ApplyPositionZModifier()
{
    constexpr auto transformModifierTypeNG = static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM);
    if (!dirtyTypesNG_.test(transformModifierTypeNG)) {
        return;
    }
    auto transformModifiers = GetModifiersNG(ModifierNG::RSModifierType::TRANSFORM);
    if (transformModifiers.empty()) {
        return;
    }
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(shared_from_this());
    ModifierNG::RSTransformRenderModifier::ResetProperties(GetMutableRenderProperties());
    for (auto& modifier : transformModifiers) {
        modifier->ApplyLegacyProperty(GetMutableRenderProperties());
    }
    dirtyTypesNG_.reset(transformModifierTypeNG);
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

void RSRenderNode::ResetAndApplyModifiers()
{
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(shared_from_this());
    bool needUseCmdlistDrawRegion = GetNeedUseCmdlistDrawRegion();
    for (const auto& [modifierType, resetFunc] : ModifierNG::RSRenderModifier::GetResetFuncMap()) {
        if (dirtyTypesNG_.test(static_cast<size_t>(modifierType))) {
            resetFunc(GetMutableRenderProperties());
        }
    }
    for (uint16_t type = 0; type < ModifierNG::MODIFIER_TYPE_COUNT; type++) {
        auto slot = GetModifiersNG(static_cast<ModifierNG::RSModifierType>(type));
        if (slot.empty() || !dirtyTypesNG_.test(type)) {
            continue;
        }
        for (auto modifier : slot) {
            modifier->ApplyLegacyProperty(GetMutableRenderProperties());
            if (needUseCmdlistDrawRegion) {
                cmdlistDrawRegion_ =
                    RSOptimizeCanvasDirtyCollector::GetInstance().CalcCmdlistDrawRegionFromOpItem(modifier);
            }
        }
    }
    // execute hooks
    GetMutableRenderProperties().OnApplyModifiers();
    OnApplyModifiers();
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
        AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
    } else if (UNLIKELY(!isChildrenSorted_)) {
        ResortChildren();
        AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
    } else if (UNLIKELY(childrenHasSharedTransition_)) {
        // if children has shared transition, force regenerate RSChildrenDrawable
        AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
    } else if (!RSRenderNode::IsDirty() || dirtyTypesNG_.none()) {
        RS_LOGD("RSRenderNode::apply modifiers RSRenderNode's dirty is false or dirtyTypes_ is none");
        // clean node, skip apply
        return;
    }
    AccumulateLastDirtyTypes();
    RecordCurDirtyTypes();
    // Reset and re-apply all modifiers
    ResetAndApplyModifiers();
    ResetFilterInfo();
    MarkForegroundFilterCache();
    UpdateShouldPaint();

    if (dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::USE_EFFECT))) {
        ProcessBehindWindowAfterApplyModifiers();
    }
    if (dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS))) {
        RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers(shared_from_this());
    }

    RS_LOGI_IF(DEBUG_NODE,
        "RSRenderNode::apply modifiers RenderProperties's sandBox's hasValue is %{public}d"
        " isTextureExportNode_:%{public}d", GetRenderProperties().GetSandBox().has_value(),
        isTextureExportNode_);
    if ((dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::TRANSFORM))) &&
        !GetRenderProperties().GetSandBox().has_value() && sharedTransitionParam_) {
        auto paramCopy = sharedTransitionParam_;
        paramCopy->InternalUnregisterSelf();
    }
    if (dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::FOREGROUND_FILTER)) ||
        dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS))) {
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

    // update state
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

void RSRenderNode::UpdateDrawableVecV2()
{
#ifdef RS_ENABLE_GPU
    // Step 1: Collect dirty slots
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypesNG_, GetDrawableVec(__func__));
    if (dirtySlots.empty()) {
        RS_LOGD("RSRenderNode::update drawable VecV2 dirtySlots is empty");
        return;
    }
    // Step 2: Update or regenerate drawable if needed
    bool drawableChanged = RSDrawable::UpdateDirtySlots(*this, GetDrawableVec(__func__), dirtySlots);
    // Step 2.1 (optional): fuze some drawables
    RSDrawable::FuzeDrawableSlots(*this, GetDrawableVec(__func__));
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
        RSDrawable::UpdateSaveRestore(*this, GetDrawableVec(__func__), drawableVecStatus_);
        // if shadow changed, update shadow rect
        UpdateShadowRect();
        UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::SHADOW);
        std::unordered_set<RSDrawableSlot> dirtySlotShadow;
        dirtySlotShadow.emplace(RSDrawableSlot::SHADOW);
        RSDrawable::UpdateDirtySlots(*this, GetDrawableVec(__func__), dirtySlotShadow);
        // Step 4: Generate drawCmdList from drawables
        UpdateDisplayList();
    }
    // If any effect drawable become dirty, check all effect drawable and do edr update
    auto needUpdateEDR = std::any_of(edrDrawableSlots.begin(), edrDrawableSlots.end(), [&dirtySlots](auto slot) {
         return dirtySlots.count(slot);
    });
    if (needUpdateEDR) {
        UpdateDrawableEnableEDR();
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
    if (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::SHADOW)) != nullptr &&
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
        stagingRenderParams_->SetContentEmpty(GetType() == RSRenderNodeType::CANVAS_NODE);
        return;
    }
#endif

    int8_t index = 0;
    // Process all drawables in [index, end], end is included.
    // Note: After this loop, index will be end+1
    auto AppendDrawFunc = [&](RSDrawableSlot end) -> int8_t {
        auto endIndex = static_cast<int8_t>(end);
        for (; index <= endIndex; ++index) {
            if (const auto& drawable = findMapValueRef(GetDrawableVec(__func__), index)) {
                stagingDrawCmdList_.emplace_back(drawable);
            }
        }
        // If the end drawable exist, return its index, otherwise return -1
        return (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(endIndex))
            != nullptr ? stagingDrawCmdList_.size() - 1 : -1);
    };
    // Update index of TRANSITION
    stagingDrawCmdIndex_.transitionIndex_ = AppendDrawFunc(RSDrawableSlot::TRANSITION);

    // Update index of ENV_FOREGROUND_COLOR
    stagingDrawCmdIndex_.envForeGroundColorIndex_ = AppendDrawFunc(RSDrawableSlot::ENV_FOREGROUND_COLOR);
    // Update index of MATERIAL_FILTER
    stagingDrawCmdIndex_.materialFilterIndex_ = AppendDrawFunc(RSDrawableSlot::MATERIAL_FILTER);
    // Update index of SHADOW
    stagingDrawCmdIndex_.shadowIndex_ = AppendDrawFunc(RSDrawableSlot::SHADOW);

    AppendDrawFunc(RSDrawableSlot::OUTLINE);
    stagingDrawCmdIndex_.renderGroupBeginIndex_ = stagingDrawCmdList_.size();
    stagingDrawCmdIndex_.foregroundFilterBeginIndex_ = static_cast<int8_t>(stagingDrawCmdList_.size());

    stagingDrawCmdIndex_.bgSaveBoundsIndex_ = AppendDrawFunc(RSDrawableSlot::BG_SAVE_BOUNDS);
    // Update index of CLIP_TO_BOUNDS
    stagingDrawCmdIndex_.clipToBoundsIndex_ = AppendDrawFunc(RSDrawableSlot::CLIP_TO_BOUNDS);

    // Update index of BACKGROUND_COLOR
    stagingDrawCmdIndex_.backgroundColorIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_COLOR);

    stagingDrawCmdIndex_.backgroundNgShaderIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_NG_SHADER);

    // Update index of BACKGROUND_IMAGE
    stagingDrawCmdIndex_.backgroundImageIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_IMAGE);

    // Update index of BACKGROUND_FILTER
    stagingDrawCmdIndex_.backgroundFilterIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_FILTER);

    // Update index of USE_EFFECT
    stagingDrawCmdIndex_.useEffectIndex_ = AppendDrawFunc(RSDrawableSlot::USE_EFFECT);

    // Update index of BACKGROUND_STYLE
    stagingDrawCmdIndex_.backgroudStyleIndex_ = AppendDrawFunc(RSDrawableSlot::BACKGROUND_STYLE);

    // Update index of ENV_FOREGROUND_COLOR_STRATEGY
    stagingDrawCmdIndex_.envForegroundColorStrategyIndex_ = AppendDrawFunc(
        RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY);

    stagingDrawCmdIndex_.bgRestoreBoundsIndex_ = AppendDrawFunc(RSDrawableSlot::BG_RESTORE_BOUNDS);

    if (drawableVecStatus_ & FRAME_NOT_EMPTY) {
        // Update index of FRAME_OFFSET
        stagingDrawCmdIndex_.frameOffsetIndex_ = AppendDrawFunc(RSDrawableSlot::FRAME_OFFSET);

        // Update index of CLIP_TO_FRAME
        stagingDrawCmdIndex_.clipToFrameIndex_ = AppendDrawFunc(RSDrawableSlot::CLIP_TO_FRAME);

        // Update index of CUSTOM_CLIP_TO_FRAME
        stagingDrawCmdIndex_.customClipToFrameIndex_ = AppendDrawFunc(RSDrawableSlot::CUSTOM_CLIP_TO_FRAME);

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
    if (!isForced && !property.GetUseEffect() && !property.HasHarmonium()) {
        return;
    }

    auto absRect = property.GetBoundsGeometry()->GetAbsRect();
    region->Join(Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()));
}

void RSRenderNode::FilterModifiersByPid(pid_t pid)
{
    for (auto& [_, slot] : modifiersNG_) {
        auto it = std::find_if(slot.begin(), slot.end(),
            [pid](const auto& modifier) -> bool { return ExtractPid(modifier->GetId()) == pid; });
        if (it != slot.end()) {
            slot.erase(it);
        }
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
        parent->AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
        parent->SetDirty();
    }
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

void RSRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:%{public}" PRIu64 "isBootAnimation %{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;
    if (GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE) {
        if (auto parent = GetParent().lock()) {
            parent->SetBootAnimation(isBootAnimation);
        }
    } else if (GetType() == RSRenderNodeType::SCREEN_NODE) {
        if (!isBootAnimation) {
            return;
        }
        SetContainBootAnimation(true);
    }
}

bool RSRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

bool RSRenderNode::GetGlobalPositionEnabled() const
{
    return false;
}

Vector2f RSRenderNode::GetOptionalBufferSize() const
{
    auto& propeties = GetRenderProperties();
    if (isBoundsModifierAdded_) {
        return propeties.GetBoundsSize();
    }
    if (isFrameModifierAdded_) {
        return propeties.GetFrameSize();
    }
    return { 0.f, 0.f };
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
        dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::ALPHA), true);
        GetMutableRenderProperties().SetAlphaOffscreen(isNodeGroup);
    }
#endif
    AddToPendingSyncList();
}

void RSRenderNode::ExcludedFromNodeGroup(bool isExcluded)
{
#ifdef RS_ENABLE_GPU
    if (stagingRenderParams_->ExcludedFromNodeGroup(isExcluded)) {
        RS_TRACE_NAME_FMT("ExcludedFromNodeGroup id:%llu isExcluded:%d", GetId(), isExcluded);
        SetDirty();
    }
#endif
}

bool RSRenderNode::IsExcludedFromNodeGroup() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->IsExcludedFromNodeGroup();
#endif
    return false;
}

void RSRenderNode::SetHasChildExcludedFromNodeGroup(bool isExcluded)
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetHasChildExcludedFromNodeGroup(isExcluded);
#endif
}

bool RSRenderNode::HasChildExcludedFromNodeGroup() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->HasChildExcludedFromNodeGroup();
#endif
    return false;
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
    RS_TRACE_NAME_FMT("mark opinc %" PRIu64 ", isopinc:%d. isCal:%d", GetId(), isOpincNode, isNeedCalculate);
    opincCache_.MarkSuggestOpincNode(isOpincNode, isNeedCalculate);
    if (stagingRenderParams_) {
        stagingRenderParams_->OpincSetCacheChangeFlag(opincCache_.GetCacheChangeFlag(), GetLastFrameSync());
        stagingRenderParams_->OpincUpdateRootFlag(opincCache_.OpincGetRootFlag());
        stagingRenderParams_->OpincSetIsSuggest(opincCache_.IsSuggestOpincNode());
    }
    SetDirty();
}

void RSRenderNode::CheckDrawingCacheType()
{
    if (nodeGroupType_ == NodeGroupType::NONE || IsExcludedFromNodeGroup()) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_FOREGROUND_FILTER) {
        SetDrawingCacheType(RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_USER) {
        SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    } else {
        SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    }
}

RectI RSRenderNode::GetAbsRect() const
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

RectI RSRenderNode::GetFilterRect() const
{
    auto& properties = GetRenderProperties();
    auto& geoPtr = (properties.GetBoundsGeometry());
    if (!geoPtr) {
        return {};
    }
    RectI result = GetAbsRect();
    auto boundsRect = GetRenderProperties().GetBoundsRect();
    auto totalRect = boundsRect;
    for (auto slot : filterDrawableSlotsSupportGetRect) {
        auto drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot));
        if (drawable == nullptr) {
            continue;
        }
        auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable);
        totalRect = totalRect.JoinRect(filterDrawable->GetStagingRelativeRect(EffectRectType::TOTAL, boundsRect));
    }
    Drawing::Rect absRect;
    geoPtr->GetAbsMatrix().MapRect(absRect,
        Drawing::Rect(totalRect.left_, totalRect.top_, totalRect.GetRight(), totalRect.GetBottom()));
    return result.JoinRect({ absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight() });
}

void RSRenderNode::CalVisibleFilterRect(const std::optional<RectI>& clipRect)
{
    auto& geoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    CalVisibleFilterRect(GetAbsRect(), geoPtr->GetAbsMatrix(), clipRect);
}

void RSRenderNode::CalVisibleFilterRect(const RectI& absRect, const Drawing::Matrix& matrix,
    const std::optional<RectI>& clipRect)
{
    GetFilterRegionInfo().filterRegion_ = absRect;
    if (clipRect.has_value()) {
        GetFilterRegionInfo().filterRegion_ = GetFilterRegionInfo().filterRegion_.IntersectRect(*clipRect);
    }
    GetFilterRegionInfo().defaultFilterRegion_ = GetFilterRegionInfo().filterRegion_;
    auto boundsRect = GetRenderProperties().GetBoundsRect();

    for (RSDrawableSlot slot : filterDrawableSlotsSupportGetRect) {
        auto& drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot));
        if (drawable == nullptr) {
            continue;
        }

        auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable);
        filterDrawable->CalVisibleRect(matrix, clipRect, boundsRect);
        GetFilterRegionInfo().filterRegion_ = GetFilterRegionInfo().filterRegion_.
            JoinRect(filterDrawable->GetVisibleTotalRegion(GetFilterRegionInfo().defaultFilterRegion_));
    }
}

void RSRenderNode::UpdateFullScreenFilterCacheRect(
    RSDirtyRegionManager& dirtyManager, bool isForeground) const
{
}

void RSRenderNode::OnTreeStateChanged()
{
    if (isOnTheTree_ && GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        ClearNeverOnTree();
    }
    if (!isOnTheTree_) {
        startingWindowFlag_ = false;
        if (stagingUECChildren_ && !stagingUECChildren_->empty()) {
            for (auto uiExtension : *stagingUECChildren_) {
                uiExtension->RemoveFromTree();
            }
        }
    }
    if (isOnTheTree_) {
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
        assignOrEraseOnAccess(GetDrawableVec(__func__),
            static_cast<int8_t>(RSDrawableSlot::CHILDREN), nullptr);
        stagingDrawCmdList_.clear();
        RS_PROFILER_KEEP_DRAW_CMD(drawCmdListNeedSync_); // false only when used for debugging
        uifirstNeedSync_ = true;
        AddToPendingSyncList();
    }
    SetParentTreeStateChangeDirty();
    auto& properties = GetMutableRenderProperties();
    bool useEffect = properties.GetUseEffect();
    UseEffectType useEffectType = static_cast<UseEffectType>(properties.GetUseEffectType());
    if (useEffect && useEffectType == UseEffectType::BEHIND_WINDOW) {
        ProcessBehindWindowOnTreeStateChanged();
    }
    RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged(shared_from_this());
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

std::list<RSRenderNode::WeakPtr> RSRenderNode::GetChildrenList() const
{
    return children_;
}

float RSRenderNode::GetHDRBrightness() const
{
    auto modifiers = GetModifiersNG(ModifierNG::RSModifierType::HDR_BRIGHTNESS);
    if (modifiers.empty()) {
        return 1.0f; // 1.0f make sure HDR video is still HDR state if RSNode::SetHDRBrightness not called
    }
    auto modifier = modifiers.back();
    return modifier->Getter<float>(ModifierNG::RSPropertyType::HDR_BRIGHTNESS, 1.f); // 1.f defaule value
}

void RSRenderNode::SetHDRUIBrightness(float brightness)
{
    RSRenderNode::SetHdrUIComponentHeadroom(RSLuminanceControl::Get().ConvertScalerFromFloatToLevel(brightness));
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
RectI RSRenderNode::GetRemovedChildrenRect() const
{
    return removedChildrenRect_;
}
bool RSRenderNode::HasHpaeBackgroundFilter() const
{
    auto drawable = findMapValueRef(GetDrawableVec(__func__),
        static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER));
    return drawable != nullptr;
}
void RSRenderNode::SetChildHasVisibleFilter(bool val)
{
    childHasVisibleFilter_ = val;
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetChildHasVisibleFilter(val);
#endif
}
void RSRenderNode::SetChildHasVisibleEffect(bool val)
{
    childHasVisibleEffect_ = val;
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetChildHasVisibleEffect(val);
#endif
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
void RSRenderNode::UpdateVisibleEffectChild(RSRenderNode& childNode)
{
    if (childNode.GetRenderProperties().GetUseEffect() || childNode.GetRenderProperties().HasHarmonium()) {
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
    cmdlistDrawRegion_.Clear();
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

bool RSRenderNode::IsForegroundFilterEnable()
{
    return findMapValueRef(GetDrawableVec(__func__),
        static_cast<uint32_t>(RSDrawableSlot::FOREGROUND_FILTER)) != nullptr;
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

void RSRenderNode::SetCacheType(CacheType cacheType)
{
    cacheType_ = cacheType;
}
void RSRenderNode::SetDrawingCacheType(RSDrawingCacheType cacheType)
{
    drawingCacheType_ = cacheType;
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
void RSRenderNode::SetForceDisableNodeGroup(bool forceDisable)
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetForceDisableNodeGroup(forceDisable);
#endif
}
bool RSRenderNode::IsForceDisableNodeGroup() const
{
#ifdef RS_ENABLE_GPU
    return stagingRenderParams_->IsForceDisableNodeGroup();
#else
    return false;
#endif
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

void RSRenderNode::UpdateVirtualScreenWhiteListInfo()
{
    if (IsInstanceOf<RSSurfaceRenderNode>()) {
        return;
    }
    auto nodeParent = GetParent().lock();
    if (nodeParent == nullptr) {
        return;
    }
    for (const auto& [screenId, ret] : hasVirtualScreenWhiteList_) {
        nodeParent->SetHasWhiteListNode(screenId, ret);
    }
}

void RSRenderNode::MarkNonGeometryChanged()
{
    geometryChangeNotPerceived_ = true;
}

void RSRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
#ifndef ROSEN_ARKUI_X
        RS_LOGE("RSRenderNode::InitRenderParams failed");
#endif
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
    stagingRenderParams_->SetVirtualScreenWhiteListInfo(hasVirtualScreenWhiteList_);
    auto cloneSourceNode = GetSourceCrossNode().lock();
    if (cloneSourceNode) {
        stagingRenderParams_->SetCloneSourceDrawable(cloneSourceNode->GetRenderDrawable());
    }
    stagingRenderParams_->MarkRepaintBoundary(isRepaintBoundary_);
#endif
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
    const auto& absRect = GetAbsDrawRect();
    stagingRenderParams_->SetAbsDrawRect(absRect);
}

void RSRenderNode::UpdateCurCornerInfo(Vector4f& curCornerRadius, RectI& curCornerRect)
{
    const auto& selfCornerRadius = GetRenderProperties().GetCornerRadius();
    if (!selfCornerRadius.IsZero()) {
        const auto& selfCornerRect = GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
        curCornerRect = curCornerRadius.IsZero() ? selfCornerRect : curCornerRect.IntersectRect(selfCornerRect);
    }
    globalCornerRect_ = curCornerRect;

    Vector4f::Max(selfCornerRadius, curCornerRadius, curCornerRadius);
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
    bool flagPredict = false;
    bool flagCurrent = false;
    std::vector<RSDrawableSlot> drawableSlotTypes = {RSDrawableSlot::MATERIAL_FILTER, RSDrawableSlot::BACKGROUND_FILTER,
        RSDrawableSlot::COMPOSITING_FILTER};
    for (auto slotType : drawableSlotTypes) {
        auto drawable = GetFilterDrawable(slotType);
        if (drawable) {
            flagPredict = flagPredict || drawable->WouldDrawLargeAreaBlur();
            flagCurrent = flagCurrent || drawable->WouldDrawLargeAreaBlurPrecisely();
        }
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
        HILOG_COMM_ERROR("Drawable try to Sync when node %{public}" PRIu64 " onDraw!!!", GetId());
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
    renderDrawable_->filterDrawables_ = {
        GetFilterDrawable(RSDrawableSlot::MATERIAL_FILTER),
        GetFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER),
        GetFilterDrawable(RSDrawableSlot::COMPOSITING_FILTER)};
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
                if (auto& drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot))) {
                    drawable->OnSync();
                }
            }
            dirtySlots_.clear();
        }

        // copy newest for uifirst root node, now force sync done nodes
        if (uifirstNeedSync_) {
            RS_OPTIONAL_TRACE_NAME_FMT("uifirst_sync %lu", GetId());
            renderDrawable_->uifirstDrawCmdList_.assign(renderDrawable_->drawCmdList_.begin(),
                                                        renderDrawable_->drawCmdList_.end());
            renderDrawable_->uifirstDrawCmdIndex_ = renderDrawable_->drawCmdIndex_;
            renderDrawable_->renderParams_->OnSync(renderDrawable_->uifirstRenderParams_);
            uifirstNeedSync_ = false;
        }
    } else {
        RS_TRACE_NAME_FMT("partial_sync %lu", GetId());
        std::vector<RSDrawableSlot> todele;
        for (const auto& slot : dirtySlots_) {
            if (slot != RSDrawableSlot::CONTENT_STYLE && slot != RSDrawableSlot::CHILDREN) { // SAVE_FRAME
                if (auto& drawable = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(slot))) {
                    drawable->OnSync();
                }
                todele.push_back(slot);
            }
        }
        for (const auto& slot : todele) {
            dirtySlots_.erase(slot);
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
    if (properties.GetLightSource() && properties.GetLightSource()->IsLightSourceValid()) {
        properties.CalculateAbsLightPosition();
        RSPointLightManager::Instance()->AddDirtyLightSource(weak_from_this());
    }
    if (properties.GetIlluminated() && properties.GetIlluminated()->IsIlluminatedValid()) {
        RSPointLightManager::Instance()->AddDirtyIlluminated(weak_from_this());
    }
}

void RSRenderNode::MarkBlurIntersectWithDRM(bool intersectWithDRM, bool isDark)
{
#ifdef RS_ENABLE_GPU
    const auto& properties = GetRenderProperties();
    if (properties.GetMaterialFilter()) {
        if (auto filterDrawable = GetFilterDrawable(RSDrawableSlot::MATERIAL_FILTER)) {
            filterDrawable->MarkBlurIntersectWithDRM(intersectWithDRM, isDark);
        }
    }
    if (properties.GetBackgroundFilter()) {
        if (auto filterDrawable = GetFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER)) {
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

void RSRenderNode::UpdateDrawableEnableEDR()
{
    bool hasEDREffect = std::any_of(edrDrawableSlots.begin(), edrDrawableSlots.end(), [this](auto slot) {
        auto drawable = findMapValueRef(this->GetDrawableVec(__func__), static_cast<int8_t>(slot));
        return drawable && drawable->GetEnableEDR();
    });
    SetEnableHdrEffect(hasEDREffect);
}

void RSRenderNode::UpdatePointLightDirtySlot()
{
    auto& drawablePtr = findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::POINT_LIGHT));
    if (!drawablePtr) {
        return;
    }
    drawablePtr->OnUpdate(*shared_from_this());
    UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::POINT_LIGHT);
    // The illuminated node has no attribute changes, so it does not call applymodifier, and consequently does not
    // call SetEnableHdrEffect. Therefore, here we need call SetEnableHdrEffect.
    SetEnableHdrEffect(drawablePtr->GetEnableEDR());
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
    RS_OPTIONAL_TRACE_NAME_FMT("MarkUifirstNode id:%lu, isUifirstNode:%d", GetId(), isUifirstNode);
    isUifirstNode_ = isUifirstNode;
    isUifirstDelay_ = 0;
}

void RSRenderNode::MarkUifirstNode(bool isForceFlag, bool isUifirstEnable)
{
    RS_TRACE_NAME_FMT("MarkUifirstNode id:%lu, isForceFlag:%d, isUifirstEnable:%d",
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

void RSRenderNode::UpdateDrawableAfterPostPrepare(ModifierNG::RSModifierType type)
{
    AddDirtyType(type);
    SetContentDirty();
#ifdef RS_ENABLE_GPU
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypesNG_, GetDrawableVec(__func__));
    if (dirtySlots.empty()) {
        RS_LOGD("RSRenderNode::UpdateDrawableAfterPostPrepare dirtySlots is empty");
        return;
    }
    bool drawableChanged = RSDrawable::UpdateDirtySlots(*this, GetDrawableVec(__func__), dirtySlots);
    RSDrawable::FuzeDrawableSlots(*this, GetDrawableVec(__func__));
    RS_LOGD("RSRenderNode::UpdateDrawableAfterPostPrepare drawableChanged:%{public}d", drawableChanged);
    if (drawableChanged) {
        RSDrawable::UpdateSaveRestore(*this, GetDrawableVec(__func__), drawableVecStatus_);
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
    for (auto& [_, slot] : modifiersNG_) {
        for (auto& modifier : slot) {
            if (modifier) {
                totalSize += modifier->GetPropertySize();
            }
        }
    }

    return totalSize;
}

void RSRenderNode::ClearDrawableVec2()
{
    if (drawableVecNeedClear_) {
        if (GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE &&
            findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::CONTENT_STYLE))) {
            if (isPurgeable_) {
                findMapValueRef(GetDrawableVec(__func__),
                    static_cast<int8_t>(RSDrawableSlot::CONTENT_STYLE))->OnPurge();
            }
            GetDrawableVec(__func__).erase(static_cast<int8_t>(RSDrawableSlot::CONTENT_STYLE));
            dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
        }
        if (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::TRANSITION))) {
            GetDrawableVec(__func__).erase(static_cast<int8_t>(RSDrawableSlot::TRANSITION));
            dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::TRANSITION_STYLE), true);
        }
        if (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::BACKGROUND_STYLE))) {
            GetDrawableVec(__func__).erase(static_cast<int8_t>(RSDrawableSlot::BACKGROUND_STYLE));
            dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::BACKGROUND_STYLE), true);
        }
        if (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::FOREGROUND_STYLE))) {
            GetDrawableVec(__func__).erase(static_cast<int8_t>(RSDrawableSlot::FOREGROUND_STYLE));
            dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::FOREGROUND_STYLE), true);
        }
        if (findMapValueRef(GetDrawableVec(__func__), static_cast<int8_t>(RSDrawableSlot::OVERLAY))) {
            GetDrawableVec(__func__).erase(static_cast<int8_t>(RSDrawableSlot::OVERLAY));
            dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::OVERLAY_STYLE), true);
        }
        drawableVecNeedClear_ = false;
    }
}

void RSRenderNode::AddModifier(
    const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier, bool isSingleFrameComposer)
{
    if (modifier == nullptr) {
        ROSEN_LOGW("RSRenderNode::AddModifier: null modifier, add failed.");
        return;
    }
    auto type = modifier->GetType();
    if (RSSystemProperties::GetSingleFrameComposerEnabled() &&
        GetNodeIsSingleFrameComposer() && isSingleFrameComposer) {
        SetDirty();
        if (singleFrameComposer_ == nullptr) {
            singleFrameComposer_ = std::make_shared<RSSingleFrameComposer>();
        }
        singleFrameComposer_->SingleFrameAddModifierNG(modifier);
        ROSEN_LOGI_IF(DEBUG_MODIFIER, "RSRenderNode:add modifierNG for single frame, node id: %{public}" PRIu64 ","
            "type: %{public}d", GetId(), (int)type);
        return;
    }
    if (modifier->IsAttached()) {
        // other cleanup
        modifier->OnDetachModifier();
    }
    // bounds and frame modifiers must be unique
    if (type == ModifierNG::RSModifierType::BOUNDS) {
        isBoundsModifierAdded_ = true;
    }
    if (type == ModifierNG::RSModifierType::FRAME) {
        isFrameModifierAdded_ = true;
    }
    if (modifier->IsCustom()) {
        modifier->SetSingleFrameModifier(false);
    }
    const auto& modifiersIt = modifiersNG_.find(type);
    if (modifiersIt == modifiersNG_.end()) {
        ModifierNGContainer modifiers { modifier };
        modifiersNG_.emplace(type, modifiers);
    } else {
        modifiersIt->second.emplace_back(modifier);
    }
    modifier->OnAttachModifier(*this);
}

void RSRenderNode::RemoveModifier(ModifierNG::RSModifierType type, ModifierId id)
{
    const auto& modifiersIt = modifiersNG_.find(type);
    if (modifiersIt == modifiersNG_.end()) {
        RS_LOGE_LIMIT(__func__, __line__, "RSRenderNode::RemoveModifier modifierNG_ not find, ModifierId %{public}"
            PRIu64 ", ModifierType %{public}hu", id, type);
        return;
    }
    auto& slot = modifiersIt->second;
    auto it =
        std::find_if(slot.begin(), slot.end(), [id](const auto& modifier) -> bool { return modifier->GetId() == id; });
    if (it == slot.end()) {
        RS_LOGE_LIMIT(__func__, __line__, "RSRenderNode::RemoveModifier slot not find, ModifierId %{public}"
            PRIu64 ", ModifierType %{public}hu", id, type);
        return;
    }
    auto modifier = *it;
    slot.erase(it);
    modifier->OnDetachModifier();
}

void RSRenderNode::RemoveModifierNG(ModifierId id)
{
    SetDirty();
    for (auto& [_, slot] : modifiersNG_) {
        auto it = std::find_if(
            slot.begin(), slot.end(), [id](const auto& modifier) -> bool { return modifier->GetId() == id; });
        if (it == slot.end()) {
            continue;
        }
        auto modifier = *it;
        slot.erase(it);
        modifier->OnDetachModifier();
    }
}

void RSRenderNode::RemoveAllModifiersNG()
{
    for (auto& [_, slot] : modifiersNG_) {
        for (auto& modifier : slot) {
            modifier->OnDetachModifier();
        }
        slot.clear();
    }
}

std::shared_ptr<ModifierNG::RSRenderModifier> RSRenderNode::GetModifierNG(
    ModifierNG::RSModifierType type, ModifierId id) const
{
    auto slot = GetModifiersNG(type);
    if (id == 0) {
        return slot.empty() ? nullptr : slot.back();
    }
    auto it =
        std::find_if(slot.begin(), slot.end(), [id](const auto& modifier) -> bool { return modifier->GetId() == id; });
    if (it == slot.end()) {
        return nullptr;
    }
    return *it;
}

RSRenderNode::ModifierNGContainer RSRenderNode::GetModifiersNG(ModifierNG::RSModifierType type) const
{
    auto it = modifiersNG_.find(type);
    if (it != modifiersNG_.end()) {
        return it->second;
    }
    return {};
}

const RSRenderNode::ModifiersNGMap& RSRenderNode::GetAllModifiers() const
{
    return modifiersNG_;
}

bool RSRenderNode::HasDrawCmdModifiers() const
{
    bool ret = !GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::TRANSITION_STYLE).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::BACKGROUND_STYLE).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::FOREGROUND_STYLE).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::OVERLAY_STYLE).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::NODE_MODIFIER).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::HDR_BRIGHTNESS).empty() ||
               !GetModifiersNG(ModifierNG::RSModifierType::CHILDREN).empty();
    if (!ret) {
        const auto& clipToFrameModifiers = GetModifiersNG(ModifierNG::RSModifierType::CLIP_TO_FRAME);
        for (auto modifier : clipToFrameModifiers) {
            if (modifier->HasProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

bool RSRenderNode::HasContentStyleModifierOnly() const
{
    bool ret = !GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::TRANSITION_STYLE).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::BACKGROUND_STYLE).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::FOREGROUND_STYLE).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::OVERLAY_STYLE).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::NODE_MODIFIER).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::HDR_BRIGHTNESS).empty() &&
               GetModifiersNG(ModifierNG::RSModifierType::CHILDREN).empty();
    if (ret) {
        const auto& clipToFrameModifiers = GetModifiersNG(ModifierNG::RSModifierType::CLIP_TO_FRAME);
        for (auto modifier : clipToFrameModifiers) {
            if (modifier->HasProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
                ret = false;
                break;
            }
        }
    }
    return ret;
}

void RSRenderNode::SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion)
{
    needUseCmdlistDrawRegion_ = needUseCmdlistDrawRegion;
}

bool RSRenderNode::GetNeedUseCmdlistDrawRegion()
{
    // Only the target node of the target scene can usd the cmdlistDrawRegion when optimize open.
    return RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled() && needUseCmdlistDrawRegion_ &&
           RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(GetId());
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
#ifdef SUBTREE_PARALLEL_ENABLE
        ClearSubtreeParallelNodes();
#endif
        ResetChildRelevantFlags();
    }
    SetGeoUpdateDelay(accumGeoDirty);
    UpdateSubTreeInfo(clipRect);
    lastFrameSubTreeSkipped_ = true;
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

void RSRenderNode::UpdateDrawingCacheInfoAfterChildren(bool isInBlackList,
    const std::unordered_set<NodeId>& childHasProtectedNodeSet)
{
    RS_LOGI_IF(DEBUG_NODE, "RSRenderNode::UpdateDrawingCacheInfoAC uifirstArkTsCardNode:%{public}d"
        " startingWindowFlag_:%{public}d HasChildrenOutOfRect:%{public}d drawingCacheType:%{public}d",
        IsUifirstArkTsCardNode(), startingWindowFlag_, HasChildrenOutOfRect(), GetDrawingCacheType());
    if (IsForceDisableNodeGroup() || GetUIFirstSwitch() == RSUIFirstSwitch::FORCE_DISABLE_CARD) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawingCacheInfoAfter force disable nodeGroup id:%" PRIu64, GetId());
        auto parentNode = GetParent().lock();
        if (parentNode) {
            parentNode->SetForceDisableNodeGroup(true);
        }
    }
    if (IsUifirstArkTsCardNode() || IsForceDisableNodeGroup()) {
        // disable render group because cards will use uifirst cache.
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    } else if (isInBlackList) {
        stagingRenderParams_->SetNodeGroupHasChildInBlacklist(true);
    }
    if (HasChildrenOutOfRect() && GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawingCacheInfoAfter ChildrenOutOfRect id:%llu", GetId());
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    }
    if (childHasProtectedNodeSet.count(GetId()) &&
        GetDrawingCacheType() == RSDrawingCacheType::FOREGROUND_FILTER_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawingCacheInfoAfter disable nodeGroup by ChildHasProtectedNode id:%llu", GetId());
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

void RSRenderNode::NodePostPrepare(
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode, const RectI& clipRect)
{
    MapAndUpdateChildrenRect();
    UpdateSubTreeInfo(clipRect);
    UpdateLocalDrawRect();
    UpdateAbsDrawRect();
    ResetChangeState();
    SetHasUnobscuredUEC();
    if (curSurfaceNode == nullptr) {
        UpdateVirtualScreenWhiteListInfo();
    }
}

RSDrawable::Vec& RSRenderNode::GetDrawableVec(const char* func) const
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (!drawableVec_) {
        drawableVec_ = std::make_unique<RSDrawable::Vec>();
        ROSEN_LOGD("drawableVec_ is nullptr, %{public}s", func);
    }
    return *drawableVec_;
#else
    return drawableVec_;
#endif
}
void RSRenderNode::InitRenderDrawableAndDrawableVec()
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    if (renderDrawable_ == nullptr) {
        renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    }
#ifndef ROSEN_ARKUI_X
    if (renderDrawable_ == nullptr) {
        RS_LOGD("RSRenderNode::InitRenderDrawableAndDrawableVec init renderDrawable_ failed");
    }
#endif
    if (!drawableVec_) {
        drawableVec_ = std::make_unique<RSDrawable::Vec>();
    }
    if (!released_) {
        return;
    }
    SetDirty();
    AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
    for (auto& [_, slot] : modifiersNG_) {
        for (auto& modifier : slot) {
            AddDirtyType(modifier->GetType());
            break;
        }
    }
    auto parent = parent_.lock();
    if (parent != nullptr) {
        parent->AddDirtyType(ModifierNG::RSModifierType::CHILDREN);
    }
    if (stagingRenderParams_) {
        stagingRenderParams_->SetDirtyType(RSRenderParamsDirtyType::MATRIX_DIRTY);
        stagingRenderParams_->SetDirtyType(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);
    }
    released_ = false;
#endif
}

std::shared_ptr<RSFilter> RSRenderNode::GetRSFilterWithSlot(RSDrawableSlot slot) const
{
    switch (slot) {
        case RSDrawableSlot::MATERIAL_FILTER: {
            return GetRenderProperties().GetMaterialFilter();
        }
        case RSDrawableSlot::BACKGROUND_FILTER: {
            return GetRenderProperties().GetBackgroundFilter();
        }
        case RSDrawableSlot::COMPOSITING_FILTER: {
            return GetRenderProperties().GetFilter();
        }
        default: {
            return nullptr;
        }
    }
}

void RSRenderNode::UpdateFilterRectInfo()
{
    auto boundsRect = GetRenderProperties().GetBoundsRect();
    for (auto slot : filterDrawableSlotsSupportGetRect) {
        std::shared_ptr<RSFilter> filter = GetRSFilterWithSlot(slot);
        if (filter == nullptr) {
            continue;
        }

        auto drawable = GetDrawableVec(__func__)[static_cast<int8_t>(slot)];
        if (drawable == nullptr) {
            continue;
        }
        auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable);
        filterDrawable->UpdateFilterRectInfo(boundsRect, filter);
    }
}

RectI RSRenderNode::GetFilterDrawableSnapshotRegion() const
{
    RectI snapshotRegion;
    for (auto slot : filterDrawableSlotsSupportGetRect) {
        auto drawable = GetDrawableVec(__func__)[static_cast<int8_t>(slot)];
        if (drawable == nullptr) {
            continue;
        }

        auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable);
        snapshotRegion = snapshotRegion.JoinRect(filterDrawable->GetVisibleSnapshotRegion(GetDefaultFilterRegion()));
    }
    return snapshotRegion;
}

void RSRenderNode::ResetFilterInfo()
{
    // reset filterRegionInfo_ and will regenerate in postPrepare if need
    filterRegionInfo_ = nullptr;
}

void RSRenderNode::CheckHdrHeadroomInfoPointer()
{
    if (!headroomInfo_) {
        headroomInfo_ = std::make_unique<HeadroomInfo>();
    }
}

void RSRenderNode::SetHdrPhotoHeadroom(uint32_t headroom)
{
    RSRenderNode::CheckHdrHeadroomInfoPointer();
    if (headroomInfo_) {
        headroomInfo_->hdrPhotoHeadroom = headroom;
    }
}

void RSRenderNode::SetHdrEffectHeadroom(uint32_t headroom)
{
    RSRenderNode::CheckHdrHeadroomInfoPointer();
    if (headroomInfo_) {
        headroomInfo_->hdrEffectHeadroom = headroom;
    }
}

void RSRenderNode::SetHdrUIComponentHeadroom(uint32_t headroom)
{
    RSRenderNode::CheckHdrHeadroomInfoPointer();
    if (headroomInfo_) {
        headroomInfo_->hdrUIComponentHeadroom = headroom;
    }
}

uint32_t RSRenderNode::GetHdrPhotoHeadroom() const
{
    if (headroomInfo_) {
        return headroomInfo_->hdrPhotoHeadroom;
    }
    return RSRenderNode::DEFAULT_HEADROOM_VALUE;
}

uint32_t RSRenderNode::GetHdrEffectHeadroom() const
{
    if (headroomInfo_) {
        return headroomInfo_->hdrEffectHeadroom;
    }
    return RSRenderNode::DEFAULT_HEADROOM_VALUE;
}

uint32_t RSRenderNode::GetHdrUIComponentHeadroom() const
{
    if (headroomInfo_) {
        return headroomInfo_->hdrUIComponentHeadroom;
    }
    return RSRenderNode::DEFAULT_HEADROOM_VALUE;
}

} // namespace Rosen
} // namespace OHOS
