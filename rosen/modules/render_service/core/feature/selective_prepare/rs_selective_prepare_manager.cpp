/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_selective_prepare_manager.h"

#include <set>

#include "feature_cfg/feature_param/performance_feature/dirtyregion_param.h"
#include "rs_trace.h"

#include "animation/rs_animation_manager.h"
#include "animation/rs_render_animation.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_point_light_manager.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
namespace {
// whitelist: surface name of the animating node must contain this substring (aweme0, aweme1, ...)
constexpr const char* SURFACE_NAME_WHITELIST = "aweme";
// subtree of the animating node: max depth (node itself is depth 0) and no branching
constexpr uint32_t MAX_SUBTREE_DEPTH = 2;
// repeatCount of infinite animations
constexpr int INFINITE_REPEAT_COUNT = -1;
// rotation property types accepted by the fast path
const std::vector<ModifierNG::RSPropertyType> ROTATION_PROPERTY_TYPES = {
    ModifierNG::RSPropertyType::ROTATION,
    ModifierNG::RSPropertyType::ROTATION_X,
    ModifierNG::RSPropertyType::ROTATION_Y,
    ModifierNG::RSPropertyType::QUATERNION,
};

AdvancedDirtyRegionType GetAdvancedDirtyRegionType()
{
    return DirtyRegionParam::IsAdvancedDirtyRegionEnable() ? RSSystemProperties::GetAdvancedDirtyRegionEnabled()
                                                           : AdvancedDirtyRegionType::DISABLED;
}
} // namespace

RSSelectivePrepareManager::RSSelectivePrepareManager(const std::weak_ptr<RSContext>& context) : context_(context) {}

void RSSelectivePrepareManager::ResetState()
{
    selectivePrepareOptActive_ = false;
    selectivePrepareOptNodes_.clear();
    pendingActivation_ = false;
}

bool RSSelectivePrepareManager::CollectAndCheckNodes(
    std::shared_ptr<RSRenderNode>& optNode, uint32_t& onTreeAnimatingCount, uint32_t& activeNodeCount)
{
    auto context = context_.lock();
    if (!context) {
        return false;
    }
    onTreeAnimatingCount = 0;
    for (auto& [id, weakNode] : context->GetAnimatingNodeList()) {
        auto node = weakNode.lock();
        if (!node || !node->IsOnTheTree()) {
            continue;
        }
        onTreeAnimatingCount++;
        optNode = node;
        if (onTreeAnimatingCount > 1) {
            break;
        }
    }
    if (onTreeAnimatingCount != 1) {
        return false;
    }
    // all active (dirty) nodes of this frame must be exactly the animating node
    activeNodeCount = 0;
    for (auto& [rootId, nodes] : context->GetActiveNodes()) {
        activeNodeCount += static_cast<uint32_t>(nodes.size());
        if (activeNodeCount > onTreeAnimatingCount) {
            return false;
        }
    }
    return optNode != nullptr;
}

void RSSelectivePrepareManager::CheckAndSetup()
{
    bool debugEnabled = RSSystemProperties::IsSelectivePrepareOptDebugEnabled();
    if (!RSSystemProperties::IsSelectivePrepareOptEnabled()) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: disabled by feature switch");
        }
        ResetState();
        return;
    }
    // O(1) exit conditions: GPU surface buffer update or transaction commands in this frame
    if (hasGpuSurfaceDirty_ || hasCommandInFrame_) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [gpuDirty=%d cmdInFrame=%d]", hasGpuSurfaceDirty_, hasCommandInFrame_);
        }
        ResetState();
        return;
    }
    std::shared_ptr<RSRenderNode> optNode = nullptr;
    uint32_t onTreeAnimatingCount = 0;
    uint32_t activeNodeCount = 0;
    if (!CollectAndCheckNodes(optNode, onTreeAnimatingCount, activeNodeCount)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [onTreeAnimatingCount=%u activeNodeCount=%u]",
                onTreeAnimatingCount, activeNodeCount);
        }
        ResetState();
        return;
    }
    auto instanceRoot = optNode->GetInstanceRootNode();
    auto surfaceNode = instanceRoot != nullptr ? instanceRoot->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    if (selectivePrepareOptActive_) {
        // already active: cached ancestor state stays valid while O(1) conditions hold, only the
        // surface alpha safety net is re-checked (whitelist/rotation cannot change without commands)
        HandleAlreadyActive(optNode, surfaceNode, debugEnabled);
        return;
    }
    // whitelist: surface name of the animating node, checked before animation type
    const std::string& surfaceName = surfaceNode ? surfaceNode->GetName() : "";
    if (surfaceName.find(SURFACE_NAME_WHITELIST) == std::string::npos) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [surface=%s not in aweme whitelist]", surfaceName.c_str());
        }
        ResetState();
        return;
    }
    if (!IsRotationOnlyAnimation(optNode)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [notRotationOnly] nodeId=%" PRIu64, optNode->GetId());
        }
        ResetState();
        return;
    }
    if (!pendingActivation_) {
        // first frame O(1) conditions pass: keep running QuickPrepare to fill aggregate caches
        pendingActivation_ = true;
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: pending activation [nodeId=%" PRIu64 " surface=%s]",
                optNode->GetId(), surfaceName.c_str());
        }
        return;
    }
    if (!CheckSurfaceEligibility(optNode, surfaceNode, debugEnabled)) {
        pendingActivation_ = false;
        return;
    }
    if (!IsSubtreeShallow(optNode)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [subtreeNotShallow] nodeId=%" PRIu64, optNode->GetId());
        }
        pendingActivation_ = false;
        return;
    }
    if (!IsCanvasOnlySubtree(optNode)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [subtreeNotCanvasOnly] nodeId=%" PRIu64, optNode->GetId());
        }
        pendingActivation_ = false;
        return;
    }
    selectivePrepareOptActive_ = true;
    pendingActivation_ = false;
    selectivePrepareOptNodes_.clear();
    selectivePrepareOptNodes_.emplace_back(optNode);
    if (debugEnabled) {
        RS_TRACE_NAME_FMT(
            "SelectivePrepareOpt: activated [nodeId=%" PRIu64 " surface=%s]", optNode->GetId(), surfaceName.c_str());
    }
}

bool RSSelectivePrepareManager::CheckSurfaceFilterAndLight(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled)
{
    // subtree filter/effect aggregate flags written by last QuickPrepare, O(1)
    if (surfaceNode->ChildHasVisibleFilter()) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceChildHasFilter] surface=%s", surfaceNode->GetName().c_str());
        }
        return false;
    }
    if (surfaceNode->ChildHasVisibleEffect()) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceChildHasEffect] surface=%s", surfaceNode->GetName().c_str());
        }
        return false;
    }
    // surface's own filters
    const auto& properties = surfaceNode->GetRenderProperties();
    if (properties.GetFilter() != nullptr) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceHasFilter] surface=%s", surfaceNode->GetName().c_str());
        }
        return false;
    }
    if (properties.GetBackgroundFilter() != nullptr) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceHasBgFilter] surface=%s", surfaceNode->GetName().c_str());
        }
        return false;
    }
    // PointLight aggregate flag
    const auto& pointLightManager = RSPointLightManager::Instance(surfaceNode->GetLogicalDisplayNodeId());
    if (pointLightManager && pointLightManager->GetChildHasVisibleIlluminated(surfaceNode)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceHasPointLight] surface=%s", surfaceNode->GetName().c_str());
        }
        return false;
    }
    // globalAlpha_ is the product of all alphas from root to surface written by last QuickPrepare
    if (!ROSEN_EQ(surfaceNode->GetGlobalAlpha(), 1.0f)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [surfaceAlphaNot1] surface=%s globalAlpha=%f",
                surfaceNode->GetName().c_str(), surfaceNode->GetGlobalAlpha());
        }
        return false;
    }
    return true;
}

RSSelectivePrepareManager::AncestorCheckResult RSSelectivePrepareManager::CheckAncestorStateToDisplay(
    const std::shared_ptr<RSRenderNode>& optNode, NodeId logicalDisplayNodeId)
{
    AncestorCheckResult result;
    auto current = optNode;
    while (current && current->GetId() != logicalDisplayNodeId) {
        // skip optNode's own state: its alpha animates and its NodeGroup is a normal state
        if (!result.hasNodeGroup && current != optNode &&
            current->GetNodeGroupType() != RSRenderNode::NodeGroupType::NONE) {
            result.hasNodeGroup = true;
        }
        if (!result.hasAlphaNotOne && current != optNode &&
            !ROSEN_EQ(current->GetRenderProperties().GetAlpha(), 1.0f)) {
            result.hasAlphaNotOne = true;
        }
        current = current->GetParent().lock();
        result.traverseCount++;
    }
    RS_TRACE_NAME_FMT("SelectivePrepareOpt ancestorCheck: traversed=%u nodeGroup=%d alphaNot1=%d", result.traverseCount,
        result.hasNodeGroup, result.hasAlphaNotOne);
    return result;
}

bool RSSelectivePrepareManager::CheckSurfaceEligibility(const std::shared_ptr<RSRenderNode>& optNode,
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled)
{
    if (!surfaceNode) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT(
                "SelectivePrepareOpt: reject [surfaceEligibility] surface is null, nodeId=%" PRIu64, optNode->GetId());
        }
        return false;
    }
    if (!CheckSurfaceFilterAndLight(surfaceNode, debugEnabled)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [surfaceEligibility] surface=%s"
                              " childFilter=%d childEffect=%d globalAlpha=%f",
                surfaceNode->GetName().c_str(), surfaceNode->ChildHasVisibleFilter(),
                surfaceNode->ChildHasVisibleEffect(), surfaceNode->GetGlobalAlpha());
        }
        return false;
    }
    // single-pass traversal covering container canvas nodes above the surface
    AncestorCheckResult result = CheckAncestorStateToDisplay(optNode, surfaceNode->GetLogicalDisplayNodeId());
    if (result.hasNodeGroup || result.hasAlphaNotOne) {
        if (debugEnabled) {
            if (result.hasNodeGroup) {
                RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [hasNodeGroup] nodeId=%" PRIu64, optNode->GetId());
            }
            if (result.hasAlphaNotOne) {
                RS_TRACE_NAME_FMT(
                    "SelectivePrepareOpt: reject [intermediateAlphaNot1] nodeId=%" PRIu64, optNode->GetId());
            }
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: reject [surfaceEligibility] nodeId=%" PRIu64
                              " hasNodeGroup=%d hasAlphaNotOne=%d traversed=%u",
                optNode->GetId(), result.hasNodeGroup, result.hasAlphaNotOne, result.traverseCount);
        }
        return false;
    }
    return true;
}

bool RSSelectivePrepareManager::HandleAlreadyActive(const std::shared_ptr<RSRenderNode>& optNode,
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled)
{
    // safety net: cached ancestor state is trusted while O(1) conditions hold, only surface
    // alpha is re-checked here
    if (!surfaceNode || !ROSEN_EQ(surfaceNode->GetGlobalAlpha(), 1.0f)) {
        if (debugEnabled) {
            RS_TRACE_NAME_FMT("SelectivePrepareOpt: deactivated [surfaceAlpha=%f]",
                surfaceNode ? surfaceNode->GetGlobalAlpha() : -1.0f);
        }
        ResetState();
        return false;
    }
    selectivePrepareOptNodes_.clear();
    selectivePrepareOptNodes_.emplace_back(optNode);
    return true;
}

bool RSSelectivePrepareManager::IsSubtreeShallow(const std::shared_ptr<RSRenderNode>& node)
{
    // no branching means each node of the subtree has at most one child, so the subtree is a
    // chain; walk down level by level, depth of the node itself is 0
    uint32_t depth = 0;
    auto current = node;
    while (current != nullptr) {
        if (depth > MAX_SUBTREE_DEPTH) {
            return false;
        }
        // infinite loop check for each node of the shallow subtree
        auto animationManager = current->GetAnimationManager();
        if (!animationManager) {
            return false;
        }
        for (auto& [id, animation] : animationManager->GetAnimations()) {
            if (animation && animation->IsRunning() && animation->GetRepeatCount() != INFINITE_REPEAT_COUNT) {
                return false;
            }
        }
        const auto& children = current->GetSortedChildren();
        if (children == nullptr || children->empty()) {
            return true;
        }
        if (children->size() > 1) {
            return false;
        }
        current = children->front();
        depth++;
    }
    return true;
}

bool RSSelectivePrepareManager::IsCanvasOnlySubtree(const std::shared_ptr<RSRenderNode>& node)
{
    if (!node) {
        return false;
    }
    auto nodeType = node->GetType();
    if (nodeType != RSRenderNodeType::CANVAS_NODE && nodeType != RSRenderNodeType::CANVAS_DRAWING_NODE) {
        return false;
    }
    const auto& children = node->GetSortedChildren();
    if (children == nullptr) {
        return true;
    }
    for (auto& child : *children) {
        if (child && !IsCanvasOnlySubtree(child)) {
            return false;
        }
    }
    return true;
}

bool RSSelectivePrepareManager::IsRotationProperty(const std::shared_ptr<RSRenderNode>& node, PropertyId propertyId)
{
    const auto& transformModifiers = node->GetModifiersNG(ModifierNG::RSModifierType::TRANSFORM);
    for (auto& modifier : transformModifiers) {
        if (!modifier) {
            continue;
        }
        for (auto propertyType : ROTATION_PROPERTY_TYPES) {
            auto property = modifier->GetProperty(propertyType);
            if (property && property->GetId() == propertyId) {
                return true;
            }
        }
    }
    return false;
}

bool RSSelectivePrepareManager::IsRotationOnlyAnimation(const std::shared_ptr<RSRenderNode>& node)
{
    auto animationManager = node->GetAnimationManager();
    if (!animationManager) {
        return false;
    }
    bool hasRunningAnimation = false;
    for (auto& [id, animation] : animationManager->GetAnimations()) {
        if (!animation || !animation->IsRunning()) {
            continue;
        }
        hasRunningAnimation = true;
        if (!IsRotationProperty(node, animation->GetPropertyId())) {
            return false;
        }
    }
    return hasRunningAnimation;
}

void RSSelectivePrepareManager::SelectivePrepareFastPath(
    const std::shared_ptr<RSRenderNode>& node, const std::shared_ptr<RSSurfaceRenderNode>& hostSurfaceNode)
{
    RS_TRACE_NAME_FMT("SelectivePrepareFastPath nodeId[%" PRIu64 "]", node->GetId());
    auto parent = node->GetParent().lock();
    // step 1: apply modifiers (animation values to staging properties)
    node->ApplyModifiers();
    // step 2~4: update geometry by parent, self draw rect, abs draw rect and dirty region
    auto& dirtyManager = hostSurfaceNode->GetDirtyManager();
    RectI clipRect = dirtyManager->GetSurfaceRect();
    // parentSurfaceMatrix is only consumed by sandbox geometry, match the visitor by passing
    // the host surface's abs matrix
    Drawing::Matrix parentSurfaceMatrix;
    if (auto& geoPtr = hostSurfaceNode->GetRenderProperties().GetBoundsGeometry(); geoPtr != nullptr) {
        parentSurfaceMatrix = geoPtr->GetAbsMatrix();
    }
    node->UpdateDrawRectAndDirtyRegion(*dirtyManager, false, clipRect, parentSurfaceMatrix);
    // step 5: subset of NodePostPrepare
    if (parent) {
        // prevent childrenRect_ of parent accumulating frame by frame
        parent->ResetChildRelevantFlags();
    }
    if (auto& stagingParams = node->GetStagingRenderParams()) {
        stagingParams->SetAlpha(node->GetRenderProperties().GetAlpha());
    }
    // propagate ancestor corner radius via cached value; globalCornerRect_ may be imprecise but
    // is not consumed by render phase
    Vector4f parentCornerRadius = parent ? parent->GetGlobalCornerRadius() : Vector4f(0.f);
    RectI curCornerRect;
    node->UpdateCurCornerInfo(parentCornerRadius, curCornerRect);
    node->MapAndUpdateChildrenRect();
    node->UpdateSubTreeInfo(clipRect);
    node->UpdateLocalDrawRect();
    node->UpdateAbsDrawRect();
    node->ResetChangeState();
    // step 6
    node->UpdateRenderParams();
    node->AddToPendingSyncList();
    if (RSSystemProperties::IsSelectivePrepareOptDebugEnabled()) {
        const auto& dirtyRect = dirtyManager->GetCurrentFrameDirtyRegion();
        RS_TRACE_NAME_FMT("SelectivePrepareFastPath nodeId=%" PRIu64 " surface=%s"
                          " dirty=[%d, %d, %d, %d]",
            node->GetId(), hostSurfaceNode->GetName().c_str(), dirtyRect.GetLeft(), dirtyRect.GetTop(),
            dirtyRect.GetWidth(), dirtyRect.GetHeight());
    }
}

bool RSSelectivePrepareManager::CollectNodeSurfacePairs(
    std::shared_ptr<RSScreenRenderNode>& screenNode, OptNodeSurfacePairs& nodeSurfacePairs)
{
    auto advancedDirtyType = GetAdvancedDirtyRegionType();
    int maxDirtyRects = advancedDirtyType == AdvancedDirtyRegionType::DISABLED
                            ? RSAdvancedDirtyConfig::DISABLED_RECT_NUM_EACH_NODE
                            : RSAdvancedDirtyConfig::MAX_RECT_NUM_EACH_NODE;
    std::set<NodeId> processedSurfaces;
    for (auto& weakOptNode : selectivePrepareOptNodes_) {
        auto optNode = weakOptNode.lock();
        if (!optNode || !optNode->IsOnTheTree()) {
            return false;
        }
        // the instance root is the leash/app main window the subtree hangs from (id cached when
        // the node went on the tree); its dirty manager collects dirty of descendants in the
        // normal path
        auto instanceRoot = optNode->GetInstanceRootNode();
        auto hostSurfaceNode =
            instanceRoot != nullptr ? instanceRoot->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
        if (hostSurfaceNode == nullptr || (!hostSurfaceNode->IsMainWindowType() && !hostSurfaceNode->IsLeashWindow())) {
            return false;
        }
        if (screenNode == nullptr) {
            // the ancestor screen node is cached on the surface by the last traversal, same
            // lookup pattern as RSHdrUtil/RsDrmUtil
            screenNode =
                RSBaseRenderNode::ReinterpretCast<RSScreenRenderNode>(hostSurfaceNode->GetAncestorScreenNode().lock());
            if (screenNode == nullptr) {
                return false;
            }
        }
        nodeSurfacePairs.emplace_back(optNode, hostSurfaceNode);
        if (processedSurfaces.insert(hostSurfaceNode->GetId()).second) {
            auto& surfaceDirtyManager = hostSurfaceNode->GetDirtyManager();
            if (!surfaceDirtyManager) {
                return false;
            }
            // align with InitScreenInfo/BeforeUpdateSurfaceDirtyCalc
            const auto& screenProperty = screenNode->GetScreenProperty();
            surfaceDirtyManager->SetAdvancedDirtyRegionType(advancedDirtyType);
            surfaceDirtyManager->SetMaxNumOfDirtyRects(maxDirtyRects);
            surfaceDirtyManager->Clear();
            surfaceDirtyManager->SetSurfaceSize(screenProperty.GetWidth(), screenProperty.GetHeight());
            surfaceDirtyManager->SetActiveSurfaceRect(screenProperty.GetActiveRect());
        }
    }
    if (nodeSurfacePairs.empty()) {
        return false;
    }
    auto screenDirtyManager = screenNode->GetDirtyManager();
    if (!screenDirtyManager) {
        return false;
    }
    const auto& screenProperty = screenNode->GetScreenProperty();
    screenDirtyManager->SetAdvancedDirtyRegionType(advancedDirtyType);
    screenDirtyManager->SetMaxNumOfDirtyRects(maxDirtyRects);
    screenDirtyManager->Clear();
    screenDirtyManager->SetSurfaceSize(screenProperty.GetWidth(), screenProperty.GetHeight());
    screenDirtyManager->SetActiveSurfaceRect(screenProperty.GetActiveRect());
    return true;
}

void RSSelectivePrepareManager::PropagateDirtyRegions(
    const std::shared_ptr<RSScreenRenderNode>& screenNode, const OptNodeSurfacePairs& nodeSurfacePairs)
{
    auto screenDirtyManager = screenNode->GetDirtyManager();
    std::set<NodeId> processedSurfaces;
    for (auto& [optNode, hostSurfaceNode] : nodeSurfacePairs) {
        if (!processedSurfaces.insert(hostSurfaceNode->GetId()).second) {
            continue;
        }
        // surface dirty managers hold absolute(screen)-coordinate rects, merge into the screen
        // dirty manager directly without extra matrix mapping
        auto& surfaceDirtyManager = hostSurfaceNode->GetDirtyManager();
        surfaceDirtyManager->ClipDirtyRectWithinSurface();
        auto surfaceDirty = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        if (!surfaceDirty.IsEmpty()) {
            screenDirtyManager->MergeDirtyRect(surfaceDirty);
        }
        // surface params need sync when its dirty region changed
        hostSurfaceNode->AddToPendingSyncList();
    }
    screenNode->AddToPendingSyncList();
    if (RSSystemProperties::IsSelectivePrepareOptDebugEnabled()) {
        const auto& screenDirty = screenDirtyManager->GetCurrentFrameDirtyRegion();
        RS_TRACE_NAME_FMT("SelectivePrepareOpt: fast path hit, screenDirty=[%d, %d, %d, %d]", screenDirty.GetLeft(),
            screenDirty.GetTop(), screenDirty.GetWidth(), screenDirty.GetHeight());
    }
}

bool RSSelectivePrepareManager::PrepareOptNodes()
{
    if (!selectivePrepareOptActive_) {
        return false;
    }
    if (selectivePrepareOptNodes_.empty()) {
        ResetState();
        return false;
    }
    std::shared_ptr<RSScreenRenderNode> screenNode;
    OptNodeSurfacePairs nodeSurfacePairs;
    if (!CollectNodeSurfacePairs(screenNode, nodeSurfacePairs)) {
        ResetState();
        return false;
    }
    for (auto& [optNode, hostSurfaceNode] : nodeSurfacePairs) {
        SelectivePrepareFastPath(optNode, hostSurfaceNode);
    }
    selectivePrepareOptHitCount_++;
    PropagateDirtyRegions(screenNode, nodeSurfacePairs);
    return true;
}

void RSSelectivePrepareManager::LogCommandInfo(RSTransactionData& transactionData)
{
    if (!RSSystemProperties::IsSelectivePrepareOptDebugEnabled()) {
        return;
    }
    std::string nodeIds;
    for (auto& [nodeId, followType, command] : transactionData.GetPayload()) {
        nodeIds += std::to_string(nodeId) + ",";
    }
    RS_TRACE_NAME_FMT("SelectivePrepareOpt: command info pid=%d count=%lu nodes=[%s]", transactionData.GetSendingPid(),
        transactionData.GetCommandCount(), nodeIds.c_str());
}

void RSSelectivePrepareManager::LogHwcBufferUpdate(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool bufferConsumed)
{
    if (!RSSystemProperties::IsSelectivePrepareOptDebugEnabled() || !surfaceNode) {
        return;
    }
    RS_TRACE_NAME_FMT("SelectivePrepareOpt: hwc buffer update surface=%s id=%" PRIu64 " consumed=%d",
        surfaceNode->GetName().c_str(), surfaceNode->GetId(), bufferConsumed);
}

void RSSelectivePrepareManager::LogAnimatingNodes()
{
    if (!RSSystemProperties::IsSelectivePrepareOptDebugEnabled()) {
        return;
    }
    auto context = context_.lock();
    if (!context) {
        return;
    }
    for (auto& [id, weakNode] : context->GetAnimatingNodeList()) {
        auto node = weakNode.lock();
        if (!node) {
            continue;
        }
        RS_TRACE_NAME_FMT(
            "SelectivePrepareOpt: animating nodeId=%" PRIu64 " onTree=%d", node->GetId(), node->IsOnTheTree());
    }
}

void RSSelectivePrepareManager::ReportEnergyStats(HgmRPEnergy& energy)
{
    if (!RSSystemProperties::IsSelectivePrepareOptEnabled() || !selectivePrepareOptActive_) {
        return;
    }
    energy.AddEnergyCommonData(
        EnergyEvent::ANIMATION_EXEC_TIME,
        "SELECTIVE_PREPARE_OPT_HIT_COUNT", std::to_string(selectivePrepareOptHitCount_));
}
} // namespace OHOS::Rosen
