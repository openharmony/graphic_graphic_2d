/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_uni_render_visitor.h"

#ifdef RS_ENABLE_VK
#include <vulkan_window.h>
#endif

#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
constexpr uint32_t PHONE_MAX_APP_WINDOW_NUM = 1;
#endif

bool IsFirstFrameReadyToDraw(RSSurfaceRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        if (child != nullptr && child->IsInstanceOf<RSRootRenderNode>()) {
            auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
            const auto& property = rootNode->GetRenderProperties();
            if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0 && rootNode->GetEnableRender()) {
                return true;
            }
        }
    }
    return false;
}
}

#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
constexpr uint32_t PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER = 50;
#endif

RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenNum = screenManager->GetAllScreenIds().size();
    isPartialRenderEnabled_ = (screenNum <= 1) && (partialRenderType_ != PartialRenderType::DISABLED);
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_);
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isOpaqueRegionDfxEnabled_ = RSSystemProperties::GetOpaqueRegionDfxEnabled();
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isOpDropped_ = isPartialRenderEnabled_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE)
        && (!isDirtyRegionDfxEnabled_ && !isTargetDirtyRegionDfxEnabled_ && !isOpaqueRegionDfxEnabled_);
    isQuickSkipPreparationEnabled_ = RSSystemProperties::GetQuickSkipPrepareEnabled();
    isHardwareComposerEnabled_ = RSSystemProperties::GetHardwareComposerEnabled();
    RSTagTracker::UpdateReleaseGpuResourceEnable(RSSystemProperties::GetReleaseGpuResourceEnabled());
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled()) {
        drivenInfo_ = std::make_unique<DrivenInfo>();
    }
#endif
    surfaceNodePrepareMutex_ = std::make_shared<std::mutex>();
    parallelRenderType_ = RSSystemProperties::GetParallelRenderingEnabled();
#if defined(RS_ENABLE_PARALLEL_RENDER)
    isCalcCostEnable_ = RSSystemParameters::GetCalcCostEnabled();
#endif
}

RSUniRenderVisitor::RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex)
    : RSUniRenderVisitor()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    parallelRenderVisitorIndex_ = surfaceIndex;
#if defined(RS_ENABLE_GL)
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
#endif
#endif
}

RSUniRenderVisitor::RSUniRenderVisitor(const RSUniRenderVisitor& visitor) : RSUniRenderVisitor()
{
    currentVisitDisplay_ = visitor.currentVisitDisplay_;
    screenInfo_ = visitor.screenInfo_;
    displayHasSecSurface_ = visitor.displayHasSecSurface_;
    parentSurfaceNodeMatrix_ = visitor.parentSurfaceNodeMatrix_;
    curAlpha_ = visitor.curAlpha_;
    dirtyFlag_ = visitor.dirtyFlag_;
    curDisplayNode_ = visitor.curDisplayNode_;
    currentFocusedPid_ = visitor.currentFocusedPid_;
    surfaceNodePrepareMutex_ = visitor.surfaceNodePrepareMutex_;
    prepareClipRect_ = visitor.prepareClipRect_;
    isOpDropped_ = visitor.isOpDropped_;
    isPartialRenderEnabled_ = visitor.isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor.isHardwareForcedDisabled_;
    doAnimate_ = visitor.doAnimate_;
    isDirty_ = visitor.isDirty_;
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::CopyVisitorInfos(std::shared_ptr<RSUniRenderVisitor> visitor)
{
    std::unique_lock<std::mutex> lock(copyVisitorInfosMutex_);
    currentVisitDisplay_ = visitor->currentVisitDisplay_;
    screenInfo_ = visitor->screenInfo_;
    displayHasSecSurface_ = visitor->displayHasSecSurface_;
    parentSurfaceNodeMatrix_ = visitor->parentSurfaceNodeMatrix_;
    curAlpha_ = visitor->curAlpha_;
    dirtyFlag_ = visitor->dirtyFlag_;
    curDisplayNode_ = visitor->curDisplayNode_;
    currentFocusedPid_ = visitor->currentFocusedPid_;
    surfaceNodePrepareMutex_ = visitor->surfaceNodePrepareMutex_;
    prepareClipRect_ = visitor->prepareClipRect_;
    isOpDropped_ = visitor->isOpDropped_;
    isPartialRenderEnabled_ = visitor->isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor->isHardwareForcedDisabled_;
    doAnimate_ = visitor->doAnimate_;
    isDirty_ = visitor->isDirty_;
}

void RSUniRenderVisitor::CopyPropertyForParallelVisitor(RSUniRenderVisitor *mainVisitor)
{
    if (!mainVisitor) {
        RS_LOGE("main thread visitor is nullptr");
        return;
    }
    doAnimate_ = mainVisitor->doAnimate_;
    isParallel_ = mainVisitor->isParallel_;
    isFreeze_ = mainVisitor->isFreeze_;
    isHardwareForcedDisabled_ = mainVisitor->isHardwareForcedDisabled_;
    isOpDropped_ = mainVisitor->isOpDropped_;
    isPartialRenderEnabled_ = mainVisitor->isPartialRenderEnabled_;
}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    node.ResetSortedChildren();
    for (auto& child : node.GetChildren()) {
        if (auto renderChild = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(child.lock())) {
            renderChild->ApplyModifiers();
        }
    }
    const auto& children = node.GetSortedChildren();

    // GetSortedChildren() may remove disappearingChildren_ when transition animation end.
    // So the judgement whether node has removed child should be executed after this.
    // merge last childRect as dirty if any child has been removed
    if (curSurfaceDirtyManager_ && node.HasRemovedChild()) {
        RectI dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
        curSurfaceDirtyManager_->MergeDirtyRect(dirtyRect);
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            // since childRect includes multiple rects, defaultly marked as canvas_node
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::REMOVE_CHILD_RECT, dirtyRect);
        }
        node.ResetHasRemovedChild();
    }
    // collect extensionAbility children nodeIds since they belong to another process, managing different view
    auto surfaceNode = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode != nullptr && surfaceNode->IsExtensionAbility() && curSurfaceNode_) {
        std::string childInfo = "ExtensionAbility has " + std::to_string(children.size()) + " children:[";
        // check child in GetSortedChildren considering disappearingChildren_
        for (auto& child : children) {
            curSurfaceNode_->UpdateAbilityNodeIds(child->GetId());
            childInfo += std::to_string(child->GetId()) + ",";
        }
        RS_TRACE_NAME(childInfo + "]");
    }

    // reset childRect before prepare children
    node.ResetChildrenRect();
    for (auto& child : children) {
        child->Prepare(shared_from_this());
    }
}

void RSUniRenderVisitor::CheckColorSpace(RSSurfaceRenderNode& node)
{
    if (node.IsAppWindow()) {
        auto surfaceNodeColorSpace = node.GetColorSpace();
        if (surfaceNodeColorSpace != ColorGamut::COLOR_GAMUT_SRGB) {
            ROSEN_LOGD("RSUniRenderVisitor::CheckColorSpace: node (%s) set new colorspace %d",
                node.GetName().c_str(), surfaceNodeColorSpace);
            if (std::find(colorGamutModes_.begin(), colorGamutModes_.end(),
                static_cast<ScreenColorGamut>(surfaceNodeColorSpace)) != colorGamutModes_.end()) {
                newColorSpace_ = surfaceNodeColorSpace;
            } else {
                RS_LOGD("RSUniRenderVisitor::CheckColorSpace: colorSpace is not supported on current screen");
            }
        }
    } else {
        if (node.GetSortedChildren().size() > 0) {
            auto surfaceNodePtr = node.GetSortedChildren().front()->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceNodePtr) {
                return;
            }
            CheckColorSpace(*surfaceNodePtr);
        }
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    dirtySurfaceNodeMap_.clear();

    RS_TRACE_NAME("RSUniRender:PrepareDisplay " + std::to_string(currentVisitDisplay_));
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayDirtyManager_->Clear();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();

    dirtyFlag_ = isDirty_;

    node.ApplyModifiers();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::PrepareDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    prepareClipRect_.SetAll(0, 0, screenInfo_.width, screenInfo_.height);
    screenManager->GetScreenSupportedColorGamuts(node.GetScreenId(), colorGamutModes_);
    for (auto& child : node.GetSortedChildren()) {
        auto surfaceNodePtr = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceNodePtr) {
            RS_LOGE("RSUniRenderVisitor::PrepareDisplayRenderNode ReinterpretCastTo fail");
            return;
        }
        CheckColorSpace(*surfaceNodePtr);
    }
    parentSurfaceNodeMatrix_ = SkMatrix::I();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        geoPtr->UpdateByMatrixFromSelf();
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }
    dirtyFlag_ = dirtyFlag_ || node.IsRotationChanged();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
    }
#endif
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence partial-render quick-reject should be disabled.
    if(node.IsRotationChanged()) {
        isOpDropped_ = false;
    }
    node.UpdateRotation();
    curAlpha_ = node.GetRenderProperties().GetAlpha();
    isParallel_ = false;
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    ParallelPrepareDisplayRenderNodeChildrens(node);
#else
    PrepareBaseRenderNode(node);
#endif
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), true);

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderPrepare");
        drivenInfo_->prepareInfo.hasInvalidScene = drivenInfo_->prepareInfo.hasInvalidScene ||
            node.GetChildrenCount() >= PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER ||
            isHardwareForcedDisabled_ || node.GetRotation() != ScreenRotation::ROTATION_0;
        drivenInfo_->prepareInfo.screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height),
        // prepare driven render tree
        RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(drivenInfo_->prepareInfo);
    }
#endif
}

void RSUniRenderVisitor::ParallelPrepareDisplayRenderNodeChildrens(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    doParallelRender_ = (node.GetChildrenCount() >= PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER) &&
                        (!doParallelComposition_);
    isParallel_ = AdaptiveSubRenderThreadMode(doParallelRender_) && parallelRenderManager->GetParallelMode();
    isDirtyRegionAlignedEnable_ = isParallel_;
    // we will open prepare parallel after check all properties.
    if (isParallel_ &&
        RSSystemProperties::GetPrepareParallelRenderingEnabled() != ParallelRenderingType::DISABLE) {
        parallelRenderManager->CopyPrepareVisitorAndPackTask(*this, node);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::PREPARE_TASK);
        parallelRenderManager->WaitPrepareEnd(*this);
    } else {
        PrepareBaseRenderNode(node);
    }
#endif
}

bool RSUniRenderVisitor::CheckIfSurfaceRenderNodeStatic(RSSurfaceRenderNode& node)
{
    // dirtyFlag_ includes leashWindow dirty
    // window layout change(e.g. move or zooming) | proxyRenderNode's cmd
    if (dirtyFlag_ || node.IsDirty() || node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        return false;
    }
    // if node has to be prepared, it's not static
    if (RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(node.GetId())) {
        return false;
    }
    if (node.IsAppWindow()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        // [Attention] node's ability pid could be different
        auto abilityNodeIds = node.GetAbilityNodeIds();
        if (std::any_of(abilityNodeIds.begin(), abilityNodeIds.end(),
                [&](uint64_t nodeId) { return RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(nodeId); })) {
            return false;
        }
    }
    RS_TRACE_NAME("Skip static surface " + node.GetName() + " nodeid - pid: " +
        std::to_string(node.GetId()) + " - " + std::to_string(ExtractPid(node.GetId())));
    ROSEN_LOGD("Skip static surface nodeid - pid - name: %" PRIu64 " - %d - %s", node.GetId(),
        ExtractPid(node.GetId()), node.GetName().c_str());
    // static node's dirty region is empty
    curSurfaceDirtyManager_ = node.GetDirtyManager();
    if (curSurfaceDirtyManager_) {
        curSurfaceDirtyManager_->Clear();
    }
    // static surface keeps same position
    curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), curDisplayNode_->GetLastFrameSurfacePos(node.GetId()));
    return true;
}

bool RSUniRenderVisitor::IsHardwareComposerEnabled()
{
    return !isHardwareForcedDisabled_ && !doAnimate_ && isHardwareComposerEnabled_;
}

void RSUniRenderVisitor::ClearTransparentBeforeSaveLayer()
{
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    for (auto& node : hardwareEnabledNodes_) {
        if (!node->ShouldPaint()) {
            continue;
        }
        auto dstRect = node->GetDstRect();
        if (dstRect.IsEmpty()) {
            continue;
        }
        canvas_->save();
        canvas_->clipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                            static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) });
        canvas_->clear(SK_ColorTRANSPARENT);
        canvas_->restore();
    }
}

void RSUniRenderVisitor::MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& surfaceNode)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }

    // hardware enabled type case: mark self
    if (surfaceNode.IsHardwareEnabledType()) {
        surfaceNode.SetHardwareForcedDisabledState(true);
        return;
    }

    if (!surfaceNode.IsAppWindow() && !surfaceNode.IsAbilityComponent()) {
        return;
    }

    // ability component type case: check pid
    if (surfaceNode.IsAbilityComponent()) {
        pid_t pid = ExtractPid(surfaceNode.GetId());
        for (auto& childNode : hardwareEnabledNodes_) {
            pid_t childPid = ExtractPid(childNode->GetId());
            if (pid == childPid) {
                childNode->SetHardwareForcedDisabledState(true);
            }
        }
        return;
    }

    // app window type case: mark all child hardware enabled nodes
    for (auto& node : surfaceNode.GetChildHardwareEnabledNodes()) {
        auto childNode = node.lock();
        if (childNode) {
            childNode->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::AdjustLocalZOrder(std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (!IsHardwareComposerEnabled() || !surfaceNode || !surfaceNode->IsAppWindow()) {
        return;
    }

    auto hardwareEnabledNodes = surfaceNode->GetChildHardwareEnabledNodes();
    if (hardwareEnabledNodes.empty()) {
        return;
    }
    localZOrder_ = static_cast<float>(hardwareEnabledNodes.size());
    if (isParallel_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        RSParallelRenderManager::Instance()->AddAppWindowNode(parallelRenderVisitorIndex_, surfaceNode);
#endif
    } else {
        appWindowNodesInZOrder_.emplace_back(surfaceNode);
    }
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "] pid: " + std::to_string(ExtractPid(node.GetId())) +
        ", nodeType " + std::to_string(static_cast<uint>(node.GetSurfaceNodeType())));
    if (node.GetSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    // avoid mouse error
    if (node.GetName() == "pointer window") {
        isOpDropped_ = false;
        isPartialRenderEnabled_ = false;
    }
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && (node.GetName() == "imeWindow" || node.GetName() == "RecentView")) {
        drivenInfo_->prepareInfo.hasInvalidScene = true;
    }
#endif
    // stop traversal if node keeps static
    if (isQuickSkipPreparationEnabled_ && CheckIfSurfaceRenderNodeStatic(node)) {
        return;
    }
    node.CleanDstRectChanged();
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;
    bool isQuickSkipPreparationEnabled = isQuickSkipPreparationEnabled_;

    // update geoptr with ContextMatrix
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto& property = node.GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    float alpha = curAlpha_;
    curAlpha_ *= (property.GetAlpha() * node.GetContextAlpha());
    node.SetGlobalAlpha(curAlpha_);

    // if current surfacenode is a main window type, reset the curSurfaceDirtyManager
    // reset leash window's dirtyManager pointer to avoid curSurfaceDirtyManager mis-pointing
    if (node.IsMainWindowType() || node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        curSurfaceDirtyManager_->Clear();
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        if (isTargetDirtyRegionDfxEnabled_ && CheckIfSurfaceTargetedForDFX(node.GetName())) {
            curSurfaceDirtyManager_->MarkAsTargetForDfx();
        }
    }
    // collect ability nodeId info within same app since it belongs to another process
    if (node.IsAbilityComponent() && curSurfaceNode_) {
        curSurfaceNode_->UpdateAbilityNodeIds(node.GetId());
    }

    // collect app window node's child hardware enabled node
    if (node.IsHardwareEnabledType() && node.GetBuffer() != nullptr && curSurfaceNode_) {
        curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }

    // Update node properties, including position (dstrect), OldDirty()
    if (auto parentNode = node.GetParent().lock()) {
        auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(parentNode);
        dirtyFlag_ = node.Update(
            *curSurfaceDirtyManager_, &(rsParent->GetRenderProperties()), dirtyFlag_, prepareClipRect_);
    } else {
        dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, nullptr, dirtyFlag_, prepareClipRect_);
    }
    geoPtr->ConcatMatrix(node.GetContextMatrix());
    // if expand screen, start from screen width
    node.SetDstRect(geoPtr->GetAbsRect().IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(),
        curDisplayNode_->GetDisplayOffsetY(), screenInfo_.width, screenInfo_.height)));

    node.SetDstRect(RectI(node.GetDstRect().left_ - curDisplayNode_->GetDisplayOffsetX(),
        node.GetDstRect().top_ - curDisplayNode_->GetDisplayOffsetY(),
        node.GetDstRect().GetWidth(), node.GetDstRect().GetHeight()));
    if (node.IsHardwareEnabledType()) {
        node.SetDstRect(node.GetDstRect().IntersectRect(prepareClipRect_));
    }

    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        // record node position for display render node dirtyManager
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetDstRect());

        if (node.IsAppWindow()) {
            curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
            // if update appwindow, its children should not skip
            isQuickSkipPreparationEnabled_ = false;
            node.ResetAbilityNodeIds();
            node.ResetChildHardwareEnabledNodes();
            boundsRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
            frameGravity_ = property.GetFrameGravity();
        }
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    // reset childrenFilterRects
    node.ResetChildrenFilterRects();

    dirtyFlag_ = dirtyFlag_ || node.GetDstRectChanged();
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    auto screenRotation = curDisplayNode_->GetRotation();
    node.ResetSurfaceOpaqueRegion(RectI(0, 0, screenInfo_.width, screenInfo_.height), geoPtr->GetAbsRect(),
        screenRotation, node.IsFocusedWindow(currentFocusedPid_));

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    bool isLeashWindowNode = false;
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }

        if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
            isLeashWindowNode = true;
            drivenInfo_->isPrepareLeashWinSubTree = true;
        }
        if (node.IsSelfDrawingType()) {
            drivenInfo_->prepareInfo.hasInvalidScene = true;
        }
    }
#endif

    node.UpdateChildrenOutOfRectFlag(false);
    if (node.ShouldPrepareSubnodes()) {
        PrepareBaseRenderNode(node);
    }
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    auto parentNode = node.GetParent().lock();
    auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(parentNode);
    if (rsParent == curDisplayNode_) {
        std::unique_lock<std::mutex> lock(*surfaceNodePrepareMutex_);
        node.UpdateParentChildrenRect(parentNode);
    } else {
        node.UpdateParentChildrenRect(parentNode);
    }
#else
    node.UpdateParentChildrenRect(node.GetParent().lock());
#endif
    // restore flags
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    curAlpha_ = alpha;
    dirtyFlag_ = dirtyFlag;
    isQuickSkipPreparationEnabled_ = isQuickSkipPreparationEnabled;
    prepareClipRect_ = prepareClipRect;
    if (node.GetDstRectChanged() || (node.GetDirtyManager() && node.GetDirtyManager()->IsDirty())) {
        dirtySurfaceNodeMap_.emplace(node.GetId(), node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    if (node.IsAppWindow()) {
        RS_TRACE_NAME(node.GetName() + " PreparedNodes: " + std::to_string(preparedCanvasNodeInCurrentSurface_));
        preparedCanvasNodeInCurrentSurface_ = 0;
    }
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && isLeashWindowNode) {
        drivenInfo_->isPrepareLeashWinSubTree = false;
    }
#endif
}

void RSUniRenderVisitor::PrepareProxyRenderNode(RSProxyRenderNode& node)
{
    auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(node.GetParent().lock());
    if (rsParent == nullptr) {
        return;
    }
    auto& property = rsParent->GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = geoPtr->GetAbsMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(curAlpha_);

    PrepareBaseRenderNode(node);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    float alpha = curAlpha_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    RectI prepareClipRect = prepareClipRect_;

    auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());

    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent ? &(rsParent->GetRenderProperties()) : nullptr,
        dirtyFlag_);
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        drivenInfo_->currentRootNode = node.shared_from_this();
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty = dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }
    }
#endif
    curAlpha_ *= property.GetAlpha();
    if (rsParent == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
        SkMatrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.width(), boundsRect_.height() }, rootWidth, rootHeight, gravityMatrix);
        // Only Apply gravityMatrix when rootNode is dirty
        if (geoPtr != nullptr && (dirtyFlag || geoDirty)) {
            geoPtr->ConcatMatrix(gravityMatrix);
        }
    }

    if (geoPtr != nullptr) {
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareBaseRenderNode(node);
    node.UpdateParentChildrenRect(node.GetParent().lock());

    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    curAlpha_ = alpha;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    preparedCanvasNodeInCurrentSurface_++;
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;

    auto nodeParent = node.GetParent().lock();
    while (nodeParent && nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>() &&
        nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>()->GetSurfaceNodeType() ==
        RSSurfaceNodeType::SELF_DRAWING_NODE) {
        nodeParent = nodeParent->GetParent().lock();
    }
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (nodeParent != nullptr) {
        rsParent = nodeParent->ReinterpretCastTo<RSRenderNode>();
    }

    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent ? &(rsParent->GetRenderProperties()) : nullptr,
        dirtyFlag_, prepareClipRect_);

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // driven render
    bool isContentCanvasNode = false;
    bool isBeforeContentNodeDirty = false;
    if (drivenInfo_ && currentVisitDisplay_ == 0 && drivenInfo_->isPrepareLeashWinSubTree && node.IsMarkDriven()) {
        auto drivenCanvasNode = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->GetDrivenCanvasNode();
        if (node.IsMarkDrivenRender() ||
            (!drivenInfo_->hasDrivenNodeMarkRender &&
            drivenCanvasNode != nullptr && node.GetId() == drivenCanvasNode->GetId())) {
            drivenInfo_->prepareInfo.backgroundNode = drivenInfo_->currentRootNode;
            drivenInfo_->prepareInfo.contentNode = node.shared_from_this();
            drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE;
            drivenInfo_->prepareInfo.dirtyInfo.contentDirty = false;
            isContentCanvasNode = true;
            isBeforeContentNodeDirty = drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty;
            if (node.IsMarkDrivenRender()) {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN_RENDER;
            } else {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN;
            }
        }
    }
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        } else {
            if (node.IsContentChanged()) {
                drivenInfo_->prepareInfo.dirtyInfo.contentDirty = true;
            }
        }
        if (node.IsContentChanged()) {
            node.SetIsContentChanged(false);
        }
    }
#endif

    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (property.GetClipToBounds()) {
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
    }
    // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
    if (property.GetClipToFrame()) {
        RectF frameRect{property.GetFrameOffsetX(), property.GetFrameOffsetY(),
            property.GetFrameWidth(), property.GetFrameHeight()};
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }

    float alpha = curAlpha_;
    curAlpha_ *= property.GetAlpha();
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareBaseRenderNode(node);
    // attention: accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(node.GetParent().lock());
    if (property.NeedFilter() && curSurfaceNode_) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyRegionForFilterNode
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        curSurfaceNode_->UpdateChildrenFilterRects(node.GetOldDirtyInSurface());
        if (curSurfaceDirtyManager_ && curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
    }
    curAlpha_ = alpha;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // skip content node and its children, calculate dirty contain background and foreground
    if (drivenInfo_ && isContentCanvasNode) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty = isBeforeContentNodeDirty;
        drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
    }
#endif
}


void RSUniRenderVisitor::CopyForParallelPrepare(std::shared_ptr<RSUniRenderVisitor> visitor)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    isPartialRenderEnabled_ = isPartialRenderEnabled_ && visitor->isPartialRenderEnabled_;
    isOpDropped_ = isOpDropped_ && visitor->isOpDropped_;
    needFilter_ = needFilter_ || visitor->needFilter_;
    for (auto &u : visitor->displayHasSecSurface_) {
        displayHasSecSurface_[u.first] = displayHasSecSurface_[u.first] || u.second;
    }

    for (auto &u : visitor->dirtySurfaceNodeMap_) {
        dirtySurfaceNodeMap_[u.first] = u.second;
    }
#endif
}

void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int edgeWidth = 6)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %s", dirtyRect.ToString().c_str());
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_);
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the skRect
    SkPaint rectPaint;
    // font size: 24
    sk_sp<SkTextBlob> SkTextBlob = SkTextBlob::MakeFromString(position.c_str(), SkFont(nullptr, 24.0f, 1.0f, 0.0f));
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(edgeWidth);
    if (fillType == SkPaint::kFill_Style) {
        rectPaint.setStrokeJoin(SkPaint::kRound_Join);
    }
    canvas_->drawRect(skRect, rectPaint);
    canvas_->drawTextBlob(SkTextBlob, dirtyRect.left_ + defaultTextOffsetX,
        dirtyRect.top_ + defaultTextOffsetY, SkPaint());
}

void RSUniRenderVisitor::DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : dirtyRects) {
        DrawDirtyRectForDFX(subRect, SK_ColorBLUE, SkPaint::kStroke_Style, fillAlpha);
    }
}

void RSUniRenderVisitor::DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region)
{
    std::vector<Occlusion::Rect> visibleDirtyRects = region.GetRegionRects();
    std::vector<RectI> rects;
    for (auto rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    DrawDirtyRegionForDFX(rects);

    // draw display dirtyregion with red color
    RectI dirtySurfaceRect = node.GetDirtyManager()->GetDirtyRegion();
    const float fillAlpha = 0.2;
    DrawDirtyRectForDFX(dirtySurfaceRect, SK_ColorRED, SkPaint::kStroke_Style, fillAlpha);
}

void RSUniRenderVisitor::DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto& it : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode->IsMainWindowType()) {
            DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
        }
    }
}

void RSUniRenderVisitor::DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().rbegin(); it != node.GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            if (DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNode)) {
                continue;
            }
            auto visibleDirtyRegions = surfaceNode->GetVisibleDirtyRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto rect : visibleDirtyRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            auto visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            auto displayDirtyRegion = node.GetDirtyManager()->GetDirtyRegion();
            for (auto rect : visibleRegions) {
                auto visibleRect = RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
                auto intersectRegion = displayDirtyRegion.IntersectRect(visibleRect);
                rects.emplace_back(intersectRegion);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSUniRenderVisitor::DrawAndTraceSingleDirtyRegionTypeForDFX(RSSurfaceRenderNode& node,
    DirtyRegionType dirtyType, bool isDrawn)
{
    auto dirtyManager = node.GetDirtyManager();
    auto matchType = DIRTY_REGION_TYPE_MAP.find(dirtyType);
    if (dirtyManager == nullptr ||  matchType == DIRTY_REGION_TYPE_MAP.end()) {
        return;
    }
    std::map<NodeId, RectI> dirtyInfo;
    float fillAlpha = 0.2;
    std::map<RSRenderNodeType, std::pair<std::string, SkColor>> nodeConfig = {
        {RSRenderNodeType::CANVAS_NODE, std::make_pair("canvas", SK_ColorRED)},
        {RSRenderNodeType::SURFACE_NODE, std::make_pair("surface", SK_ColorGREEN)},
    };

    std::string subInfo;
    for (const auto& [nodeType, info] : nodeConfig) {
        dirtyManager->GetDirtyRegionInfo(dirtyInfo, nodeType, dirtyType);
        subInfo += (" " + info.first + "node amount: " + std::to_string(dirtyInfo.size()));
        for (const auto& [nid, rect] : dirtyInfo) {
            if (isDrawn) {
                DrawDirtyRectForDFX(rect, info.second, SkPaint::kStroke_Style, fillAlpha);
            }
        }
    }
    RS_TRACE_NAME("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node " + node.GetName() + " - id[" +
        std::to_string(node.GetId()) + "] has dirtytype " + matchType->second + subInfo);
    ROSEN_LOGD("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node %s, id[%" PRIu64 "] has dirtytype %s%s",
        node.GetName().c_str(), node.GetId(), matchType->second.c_str(), subInfo.c_str());
}

bool RSUniRenderVisitor::DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node)
{
    if (dirtyRegionDebugType_ < DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        return false;
    }
    if (dirtyRegionDebugType_ == DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        auto i = DirtyRegionType::UPDATE_DIRTY_REGION;
        for (; i < DirtyRegionType::TYPE_AMOUNT; i = (DirtyRegionType)(i + 1)) {
            DrawAndTraceSingleDirtyRegionTypeForDFX(node, i, false);
        }
        return true;
    }
    const std::map<DirtyRegionDebugType, DirtyRegionType> DIRTY_REGION_DEBUG_TYPE_MAP {
        { DirtyRegionDebugType::UPDATE_DIRTY_REGION, DirtyRegionType::UPDATE_DIRTY_REGION },
        { DirtyRegionDebugType::OVERLAY_RECT, DirtyRegionType::OVERLAY_RECT },
        { DirtyRegionDebugType::FILTER_RECT, DirtyRegionType::FILTER_RECT },
        { DirtyRegionDebugType::SHADOW_RECT, DirtyRegionType::SHADOW_RECT },
        { DirtyRegionDebugType::PREPARE_CLIP_RECT, DirtyRegionType::PREPARE_CLIP_RECT },
        { DirtyRegionDebugType::REMOVE_CHILD_RECT, DirtyRegionType::REMOVE_CHILD_RECT },
    };
    auto matchType = DIRTY_REGION_DEBUG_TYPE_MAP.find(dirtyRegionDebugType_);
    if (matchType != DIRTY_REGION_DEBUG_TYPE_MAP.end()) {
        DrawAndTraceSingleDirtyRegionTypeForDFX(node, matchType->second);
    }
    return true;
}

void RSUniRenderVisitor::DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node)
{
    auto opaqueRegionRects = node.GetOpaqueRegion().GetRegionRects();
    for (const auto &subRect: opaqueRegionRects) {
        DrawDirtyRectForDFX(subRect.ToRectI(), SK_ColorGREEN, SkPaint::kFill_Style, 0.2f, 0);
    }
}

void RSUniRenderVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSUniRenderVisitor::ProcessParallelDisplayRenderNode(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    RS_TRACE_NAME("ProcessParallelDisplayRenderNode[" + std::to_string(node.GetId()));
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    switch (screenInfo_.state) {
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        case ScreenState::PRODUCER_SURFACE_ENABLE:
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenState only support HDI_OUTPUT_ENABLE");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: RSProcessor is null!");
        return;
    }

    // ParallelDisplayRenderNode cannot have mirror source.
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode renderFrame_ nullptr");
        return;
    }
    std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
    AddOverDrawListener(renderFrame_, overdrawListener);

    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: failed to create canvas");
        return;
    }
    canvas_->clear(SK_ColorTRANSPARENT);
    RSPropertiesPainter::SetBgAntiAlias(true);
    int saveCount = canvas_->save();
    canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        canvas_->concat(geoPtr->GetMatrix());
    }
    for (auto& child : node.GetChildren()) {
        auto childNode = child.lock();
        if (!childNode) {
            continue;
        }
        RSParallelRenderManager::Instance()->StartTiming(parallelRenderVisitorIndex_);
        childNode->Process(shared_from_this());
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            parallelRenderVisitorIndex_, childNode->GetId(), TaskType::PROCESS_TASK);
    }
    canvas_->restoreToCount(saveCount);

    if (overdrawListener != nullptr) {
        overdrawListener->Draw();
    }
    DrawWatermarkIfNeed();
    RS_TRACE_BEGIN("ProcessParallelDisplayRenderNode:FlushFrame");
    renderFrame_->Flush();
    RS_TRACE_END();
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode end");
#endif
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    if (node.IsParallelDisplayNode()) {
        ProcessParallelDisplayRenderNode(node);
        return;
    }
#endif
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" +
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    bool isNeedCalcCost = node.GetSurfaceChangedRects().size() > 0;
#endif
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    auto mirrorNode = node.GetMirrorSource().lock();
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(mirrorNode ?
                RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }

    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: renderEngine is null!");
        return;
    }
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (!node.IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodePtr);
        if (!node.CreateSurface(listener)) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
            return;
        }
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // [PLANNING]: processing of layers z-order to be implemented
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        drivenInfo_->currDrivenRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
        globalZOrder_ = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    }
#endif

    if (mirrorNode) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        if (displayHasSecSurface_[mirrorNode->GetScreenId()] && mirrorNode->GetSecurityDisplay() != isSecurityDisplay_
            && processor) {
            canvas_ = processor->GetCanvas();
            if (canvas_ == nullptr) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
                return;
            }
            int saveCount = canvas_->save();
            ProcessBaseRenderNode(*mirrorNode);
            DrawWatermarkIfNeed();
            canvas_->restoreToCount(saveCount);
        } else {
            processor_->ProcessDisplaySurface(*mirrorNode);
        }
    } else if (node.GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        canvas_ = processor->GetCanvas();
        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
            return;
        }
        ProcessBaseRenderNode(node);
        DrawWatermarkIfNeed();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    } else if (drivenInfo_ && drivenInfo_->currDrivenRenderMode == DrivenUniRenderMode::REUSE_WITH_CLIP_HOLE) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode DrivenUniRenderMode is REUSE_WITH_CLIP_HOLE");
        node.SetGlobalZOrder(globalZOrder_);
        processor_->ProcessDisplaySurface(node);
#endif
    } else {
#ifdef RS_ENABLE_EGLQUERYSURFACE
        curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        ClosePartialRenderWhenAnimatingWindows(displayNodePtr);
        if (isPartialRenderEnabled_) {
            CalcDirtyDisplayRegion(displayNodePtr);
            CalcDirtyRegionForFilterNode(displayNodePtr);
            displayNodePtr->ClearCurrentSurfacePos();
        }
        if (isOpDropped_ && dirtySurfaceNodeMap_.empty() && !curDisplayDirtyManager_->IsDirty()) {
            RS_LOGD("DisplayNode skip");
            RS_TRACE_NAME("DisplayNode skip");
            if (!IsHardwareComposerEnabled()) {
                return;
            }
            for (auto& node: hardwareEnabledNodes_) {
                if (!node->IsHardwareForcedDisabled()) {
                    node->MarkCurrentFrameHardwareEnabled();
                }
            }
            return;
        }
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
        if (isParallel_ &&!isPartialRenderEnabled_) {
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            vulkan::VulkanWindow::InitializeVulkan(
                parallelRenderManager->GetParallelThreadNumber());
            RS_TRACE_BEGIN("RSUniRender::VK::WaitFence");
            vulkan::VulkanWindow::WaitForSharedFence();
            vulkan::VulkanWindow::ResetSharedFence();
            RS_TRACE_END();
            parallelRenderManager->CopyVisitorAndPackTask(*this, node);
            parallelRenderManager->InitDisplayNodeAndRequestFrame(renderEngine_, screenInfo_);
            parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
            parallelRenderManager->WaitProcessEnd(*this);
            parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
            vulkan::VulkanWindow::PresentAll();

            RS_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
            RSMainThread::Instance()->WaitUtilUniRenderFinished();
            RS_TRACE_END();

            parallelRenderManager->ProcessParallelDisplaySurface(*this);
            processor_->PostProcess();

            parallelRenderManager->ReleaseBuffer();

            isParallel_ = false;
            return;
        }
#endif

        auto rsSurface = node.GetRSSurface();
        if (rsSurface == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        rsSurface->SetColorSpace(newColorSpace_);
        // we should request a framebuffer whose size is equals to the physical screen size.
        RS_TRACE_BEGIN("RSUniRender:RequestFrame");
        renderFrame_ = renderEngine_->RequestFrame(
            std::static_pointer_cast<RSSurfaceOhos>(rsSurface),
            RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true));
        RS_TRACE_BEGIN("RSUniRender::wait for bufferRequest cond");
        RSMainThread::Instance()->WaitUntilDisplayNodeBufferReleased(node);
        RS_TRACE_END();
        RS_TRACE_END();

        if (renderFrame_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
        AddOverDrawListener(renderFrame_, overdrawListener);

        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }

#ifdef RS_ENABLE_VK
        canvas_->clear(SK_ColorTRANSPARENT);
#endif

        int saveLayerCnt = 0;
        SkRegion region;
        Occlusion::Region dirtyRegionTest;
        std::vector<RectI> rects;
        bool clipPath = false;
#ifdef RS_ENABLE_EGLQUERYSURFACE
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (isPartialRenderEnabled_) {
            // Early history buffer Merging will have impact on Overdraw display, so we need to
            // set the full screen dirty to avoid this impact.
            if (RSOverdrawController::GetInstance().IsEnabled()) {
                node.GetDirtyManager()->ResetDirtyAsSurfaceSize();
            }
            RS_TRACE_BEGIN("RSUniRender::GetBufferAge");
            int bufferAge = renderFrame_->GetBufferAge();
            RS_TRACE_END();
            RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge, isDirtyRegionAlignedEnable_);
            Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
                displayNodePtr, isDirtyRegionAlignedEnable_);
            dirtyRegionTest = dirtyRegion;
            if (isDirtyRegionAlignedEnable_) {
                SetSurfaceGlobalAlignedDirtyRegion(displayNodePtr, dirtyRegion);
            } else {
                SetSurfaceGlobalDirtyRegion(displayNodePtr);
            }
            rects = GetDirtyRects(dirtyRegion);
            RectI rect = node.GetDirtyManager()->GetDirtyRegionFlipWithinSurface();
            if (!rect.IsEmpty()) {
                rects.emplace_back(rect);
            }
            if (!isDirtyRegionAlignedEnable_) {
                auto disH = screenInfo_.GetRotatedHeight();
                for (auto& r : rects) {
                    region.op(SkIRect::MakeXYWH(r.left_, disH - r.GetBottom(), r.width_, r.height_),
                        SkRegion::kUnion_Op);
                    RS_LOGD("SetDamageRegion %s", r.ToString().c_str());
                }
            }
            // SetDamageRegion and opDrop will be disabled for dirty region DFX visualization
            if (!isDirtyRegionDfxEnabled_ && !isTargetDirtyRegionDfxEnabled_ && !isOpaqueRegionDfxEnabled_) {
                renderFrame_->SetDamageRegion(rects);
            }
        }
        if (isOpDropped_ && !isDirtyRegionAlignedEnable_) {
            if (region.isEmpty()) {
                // [planning] Remove this after frame buffer can cancel
                canvas_->clipRect(SkRect::MakeEmpty());
            } else if (region.isRect()) {
                canvas_->clipRegion(region);
            } else {
                RS_TRACE_NAME("RSUniRenderVisitor: clipPath");
                clipPath = true;
                SkPath dirtyPath;
                region.getBoundaryPath(&dirtyPath);
                canvas_->clipPath(dirtyPath, true);
                // [planning] Remove this after skia is upgraded, the clipRegion is supported
                if (!needFilter_) {
                    ClearTransparentBeforeSaveLayer();
                    RSTagTracker tagTracker(canvas_->getGrContext(), RSTagTracker::TAGTYPE::TAG_SAVELAYER_DRAW_NODE);
                    saveLayerCnt = canvas_->saveLayer(SkRect::MakeWH(screenInfo_.width, screenInfo_.height), nullptr);
                }
            }
        }
#endif
        RSPropertiesPainter::SetBgAntiAlias(true);
        if (!isParallel_) {
            int saveCount = canvas_->save();
            canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
            auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
            if (geoPtr != nullptr) {
                canvas_->concat(geoPtr->GetMatrix());
                // enable cache if screen rotation is not times of 90 degree
                canvas_->SetCacheType(geoPtr->IsNeedClientCompose() ? RSPaintFilterCanvas::CacheType::ENABLED
                                                                    : RSPaintFilterCanvas::CacheType::DISABLED);
            }
            if (clipPath) {
                canvas_->SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
            }
            if (canvas_->GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
                // we are doing rotation animation, try offscreen render if capable
                ClearTransparentBeforeSaveLayer();
                PrepareOffscreenRender(node);
                ProcessBaseRenderNode(node);
                FinishOffscreenRender();
            } else {
                // render directly
                ProcessBaseRenderNode(node);
            }
            canvas_->restoreToCount(saveCount);
        }
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        if ((isParallel_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) && isCalcCostEnable_) {
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            parallelRenderManager->CopyCalcCostVisitorAndPackTask(*this, node, isNeedCalcCost,
                doAnimate_, isOpDropped_);
            if (parallelRenderManager->IsNeedCalcCost()) {
                parallelRenderManager->LoadBalanceAndNotify(TaskType::CALC_COST_TASK);
                parallelRenderManager->WaitCalcCostEnd();
                parallelRenderManager->UpdateNodeCost(node);
            }
        }
        if (isParallel_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) {
            ClearTransparentBeforeSaveLayer();
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            parallelRenderManager->SetFrameSize(screenInfo_.width, screenInfo_.height);
            parallelRenderManager->CopyVisitorAndPackTask(*this, node);
            parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
            parallelRenderManager->MergeRenderResult(*canvas_);
            parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
        }
#endif
        if (saveLayerCnt > 0) {
            RSTagTracker tagTracker(canvas_->getGrContext(), RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->restoreToCount(saveLayerCnt);
        }

        if (overdrawListener != nullptr) {
            overdrawListener->Draw();
        }
        DrawWatermarkIfNeed();
        // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property
        if (isPartialRenderEnabled_) {
            if (isDirtyRegionDfxEnabled_) {
                DrawAllSurfaceDirtyRegionForDFX(node, dirtyRegionTest);
            }
            if (isTargetDirtyRegionDfxEnabled_) {
                DrawTargetSurfaceDirtyRegionForDFX(node);
            }
            if (isOpaqueRegionDfxEnabled_) {
                DrawAllSurfaceOpaqueRegionForDFX(node);
            }
        }
        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        renderFrame_->Flush();
        RS_TRACE_END();
        RS_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_TRACE_END();
        AssignGlobalZOrderAndCreateLayer();
        node.SetGlobalZOrder(globalZOrder_);
        processor_->ProcessDisplaySurface(node);
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderProcess");
        // process driven render tree
        drivenInfo_->processInfo = { processor_, newColorSpace_, node.GetGlobalZOrder() };
        RSDrivenRenderManager::GetInstance().DoProcessRenderTask(drivenInfo_->processInfo);
    }
#endif
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::AssignGlobalZOrderAndCreateLayer()
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (hardwareEnabledNodes_.empty()) {
        return;
    }
    if (isParallel_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>().swap(appWindowNodesInZOrder_);
        auto subThreadNum = RSParallelRenderManager::Instance()->GetParallelThreadNumber();
        auto appWindowNodesMap = RSParallelRenderManager::Instance()->GetAppWindowNodes();
        std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodes;
        for (uint32_t i = 0; i < subThreadNum; i++) {
            appWindowNodes = appWindowNodesMap[i];
            appWindowNodesInZOrder_.insert(appWindowNodesInZOrder_.end(), appWindowNodes.begin(), appWindowNodes.end());
        }
#endif
    }
    globalZOrder_ = 0.0f;
    for (auto& appWindowNode : appWindowNodesInZOrder_) {
        // first, sort app window node's child surfaceView by local zOrder
        auto childHardwareEnabledNodes = appWindowNode->GetChildHardwareEnabledNodes();
        std::stable_sort(childHardwareEnabledNodes.begin(), childHardwareEnabledNodes.end(),
            [](const auto& first, const auto& second) {
            auto node1 = first.lock();
            auto node2 = second.lock();
            return node1 && node2 && node1->GetLocalZOrder() < node2->GetLocalZOrder();
        });
        for (auto& child : childHardwareEnabledNodes) {
            localZOrder_ = 0.0f;
            auto childNode = child.lock();
            if (childNode && childNode->GetBuffer() != nullptr && !childNode->IsHardwareForcedDisabled()) {
                // assign local zOrder here to ensure it range from 0 to childHardwareEnabledNodes.size()
                // for each app window node
                childNode->SetLocalZOrder(localZOrder_++);
                // SetGlobalZOrder here to ensure zOrder committed to composer is continuous
                childNode->SetGlobalZOrder(globalZOrder_++);
                RS_LOGD("createLayer: %" PRIu64 "", childNode->GetId());
                processor_->ProcessSurface(*childNode);
            }
        }
    }
}

void RSUniRenderVisitor::AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSCanvasListener>& overdrawListener)
{
    if (renderFrame->GetFrame() == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: RSSurfaceFrame is null");
        return;
    }
    RS_TRACE_BEGIN("RSUniRender::GetSurface");
    auto skSurface = renderFrame->GetFrame()->GetSurface();
    RS_TRACE_END();
    if (skSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: skSurface is null");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
    // if listenedCanvas is nullptr, that means disabled or listen failed
    std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;

    if (RSOverdrawController::GetInstance().IsEnabled()) {
        auto &oc = RSOverdrawController::GetInstance();
        listenedCanvas = std::make_shared<RSListenedCanvas>(skSurface.get());
        overdrawListener = oc.CreateListener<RSGPUOverdrawCanvasListener>(listenedCanvas.get());
        if (overdrawListener == nullptr) {
            overdrawListener = oc.CreateListener<RSCPUOverdrawCanvasListener>(listenedCanvas.get());
        }

        if (overdrawListener != nullptr) {
            listenedCanvas->SetListener(overdrawListener);
        } else {
            // create listener failed
            listenedCanvas = nullptr;
        }
    }

    if (listenedCanvas != nullptr) {
        canvas_ = listenedCanvas;
    } else {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
    }
}

void RSUniRenderVisitor::CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node) const
{
    RS_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        RectI surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        if (surfaceNode->IsTransparent()) {
            // Handles the case of transparent surface, merge transparent dirty rect
            RectI transparentDirtyRect = surfaceNode->GetDstRect().IntersectRect(surfaceDirtyRect);
            if (!transparentDirtyRect.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge transparent dirty rect %s rect %s",
                    surfaceNode->GetName().c_str(), transparentDirtyRect.ToString().c_str());
                displayDirtyManager->MergeDirtyRect(transparentDirtyRect);
            }
        }

        if (surfaceNode->GetZorderChanged()) {
            // Zorder changed case, merge surface dest Rect
            RS_LOGD("CalcDirtyDisplayRegion merge GetZorderChanged %s rect %s", surfaceNode->GetName().c_str(),
                surfaceNode->GetDstRect().ToString().c_str());
            displayDirtyManager->MergeDirtyRect(surfaceNode->GetDstRect());
        }

        RectI lastFrameSurfacePos = node->GetLastFrameSurfacePos(surfaceNode->GetId());
        RectI currentFrameSurfacePos = node->GetCurrentFrameSurfacePos(surfaceNode->GetId());
        if (lastFrameSurfacePos != currentFrameSurfacePos) {
            RS_LOGD("CalcDirtyDisplayRegion merge surface pos changed %s lastFrameRect %s currentFrameRect %s",
                surfaceNode->GetName().c_str(), lastFrameSurfacePos.ToString().c_str(),
                currentFrameSurfacePos.ToString().c_str());
            if (!lastFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(lastFrameSurfacePos);
            }
            if (!currentFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(currentFrameSurfacePos);
            }
        }

        bool isShadowDisappear = !surfaceNode->GetRenderProperties().IsShadowValid() && surfaceNode->IsShadowValidLastFrame();
        if (surfaceNode->GetRenderProperties().IsShadowValid() || isShadowDisappear) {
            RectI shadowDirtyRect = surfaceNode->GetOldDirtyInSurface().IntersectRect(surfaceDirtyRect);
            // There are two situation here:
            // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
            // surfaceDirtyRect == surfaceNode->GetOldDirtyInSurface()
            // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
            // surfaceDirtyRect > surfaceNode->GetOldDirtyInSurface()
            // So we should always merge surfaceDirtyRect here.
            if (!shadowDirtyRect.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(surfaceDirtyRect);
                RS_LOGD("CalcDirtyDisplayRegion merge ShadowValid %s rect %s",
                    surfaceNode->GetName().c_str(), surfaceDirtyRect.ToString().c_str());
            }
            if (isShadowDisappear) {
                surfaceNode->SetShadowValidLastFrame(false);
            }
        }
        auto transparentRegion = surfaceNode->GetTransparentRegion();
        Occlusion::Rect tmpRect = Occlusion::Rect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        Occlusion::Region surfaceDirtyRegion { tmpRect };
        Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
        if (!transparentDirtyRegion.IsEmpty()) {
            RS_LOGD("CalcDirtyDisplayRegion merge TransparentDirtyRegion %s region %s",
                surfaceNode->GetName().c_str(), transparentDirtyRegion.GetRegionInfo().c_str());
            std::vector<Occlusion::Rect> rects = transparentDirtyRegion.GetRegionRects();
            for (const auto& rect : rects) {
                displayDirtyManager->MergeDirtyRect(RectI
                    { rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
    std::vector<RectI> surfaceChangedRects = node->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CalcDirtyDisplayRegion merge Surface closed %s", surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            displayDirtyManager->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::CalcDirtyRegionForFilterNode(std::shared_ptr<RSDisplayRenderNode>& node)
{
    auto displayDirtyManager = node->GetDirtyManager();
    RectI displayDirtyRect = displayDirtyManager ? displayDirtyManager->GetDirtyRegion() : RectI{0, 0, 0, 0};
    for (auto it = node->GetCurAllSurfaces().begin(); it != node->GetCurAllSurfaces().end(); ++it) {
        auto currentSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (currentSurfaceNode == nullptr) {
            continue;
        }
        auto currentSurfaceDirtyManager = currentSurfaceNode->GetDirtyManager();
        RectI currentSurfaceDirtyRect = currentSurfaceDirtyManager->GetDirtyRegion();

        // child node (component) has filter
        auto filterRects = currentSurfaceNode->GetChildrenNeedFilterRects();
        if (currentSurfaceNode->IsAppWindow() && !filterRects.empty()) {
            needFilter_ = needFilter_ || !currentSurfaceNode->IsFreeze();
            for (auto subRect : filterRects) {
                if (!displayDirtyRect.IntersectRect(subRect).IsEmpty()) {
                    if (currentSurfaceNode->IsTransparent()) {
                        displayDirtyManager->MergeDirtyRect(subRect);
                    }
                    currentSurfaceDirtyManager->MergeDirtyRect(subRect);
                } else if (!currentSurfaceDirtyRect.IntersectRect(subRect).IsEmpty()) {
                    currentSurfaceDirtyManager->MergeDirtyRect(subRect);
                }
            }
        }

        // surfaceNode self has filter
        if (currentSurfaceNode->GetRenderProperties().NeedFilter()) {
            needFilter_ = needFilter_ || !currentSurfaceNode->IsFreeze();
            if (!displayDirtyRect.IntersectRect(currentSurfaceNode->GetOldDirtyInSurface()).IsEmpty() ||
                !currentSurfaceDirtyRect.IntersectRect(currentSurfaceNode->GetOldDirtyInSurface()).IsEmpty()) {
                currentSurfaceDirtyManager->MergeDirtyRect(currentSurfaceNode->GetOldDirtyInSurface());
            }

            if (currentSurfaceNode->IsTransparent()) {
                for (auto& iter : node->GetCurAllSurfaces()) {
                    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(iter);
                    if (surfaceNode == nullptr) {
                        continue;
                    }
                    if (!surfaceNode->GetDirtyManager()->GetDirtyRegion().IntersectRect(
                        currentSurfaceNode->GetOldDirtyInSurface()).IsEmpty()) {
                        currentSurfaceDirtyManager->MergeDirtyRect(currentSurfaceNode->GetOldDirtyInSurface());
                        displayDirtyManager->MergeDirtyRect(currentSurfaceNode->GetOldDirtyInSurface());
                        break;
                    }
                }
            }
        }
    }
    if (needFilter_) { // when need draw filter, disable hardware composer
        isHardwareForcedDisabled_ = true;
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_TRACE_FUNC();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        surfaceNode->SetDirtyRegionAlignedEnable(false);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = surfaceNode->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalAlignedDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    const Occlusion::Region alignedDirtyRegion)
{
    RS_TRACE_FUNC();
    if (!isDirtyRegionAlignedEnable_) {
        return;
    }
    // calculate extra dirty region after 32 bits alignment
    Occlusion::Region dirtyRegion = alignedDirtyRegion;
    auto globalRectI = node->GetDirtyManager()->GetDirtyRegion();
    Occlusion::Rect globalRect {globalRectI.left_, globalRectI.top_, globalRectI.GetRight(), globalRectI.GetBottom()};
    Occlusion::Region globalRegion{globalRect};
    dirtyRegion.SubSelf(globalRegion);
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        Occlusion::Region visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceAlignedDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        if (dirtyRegion.IsEmpty()) {
            surfaceNode->SetExtraDirtyRegionAfterAlignment(dirtyRegion);
        } else {
            auto extraDirtyRegion = (dirtyRegion.Sub(surfaceAlignedDirtyRegion)).And(visibleRegion);
            surfaceNode->SetExtraDirtyRegionAfterAlignment(extraDirtyRegion);
        }
        surfaceNode->SetDirtyRegionAlignedEnable(true);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto alignedVisibleDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        curVisibleDirtyRegion.OrSelf(alignedVisibleDirtyRegion);
    }
}

#ifdef RS_ENABLE_EGLQUERYSURFACE
std::vector<RectI> RSUniRenderVisitor::GetDirtyRects(const Occlusion::Region &region)
{
    std::vector<Occlusion::Rect> rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
        retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    RS_LOGD("GetDirtyRects size %d %s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}
#endif

void RSUniRenderVisitor::InitCacheSurface(RSRenderNode& node, int width, int height)
{
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    node.SetCacheSurface(SkSurface::MakeRenderTarget(canvas_->getGrContext(), SkBudgeted::kYes, info));
#else
    node.SetCacheSurface(SkSurface::MakeRasterN32Premul(width, height));
#endif
}

void RSUniRenderVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    if (node.GetCacheTypeChanged()) {
        node.ClearCacheSurface();
        node.SetCacheTypeChanged(false);
    }

    if (node.GetCacheType() == RSRenderNode::NONE) {
        ProcessBaseRenderNode(node);
    } else if (node.GetCacheSurface()) {
        RS_TRACE_BEGIN("RSUniRenderVisitor::DrawChildRenderNode Draw nodeId = " +
            std::to_string(node.GetId()));
        if (node.GetCacheType() == RSRenderNode::SPHERIZE) {
            RSPropertiesPainter::DrawCachedSpherizeSurface(node, *canvas_, node.GetCacheSurface());
        } else {
            RSUniRenderUtil::DrawCachedFreezeSurface(node, *canvas_, node.GetCacheSurface());
        }
        RS_TRACE_END();
    } else {
        RS_TRACE_BEGIN("RSUniRenderVisitor::DrawChildRenderNode Init Draw nodeId = " +
            std::to_string(node.GetId()));
        isFreeze_ = true;
        int width = std::ceil(node.GetRenderProperties().GetBoundsRect().GetWidth());
        int height = std::ceil(node.GetRenderProperties().GetBoundsRect().GetHeight());
        InitCacheSurface(node, width, height);

        if (node.GetCacheSurface()) {
            auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(node.GetCacheSurface().get());
            // When drawing CacheSurface, all child node should be drawn.
            // So set isOpDropped_ = false here.
            bool isOpDropped = isOpDropped_;
            isOpDropped_ = false;

            swap(cacheCanvas, canvas_);
            if (node.GetCacheType() == RSRenderNode::FREEZE) {
                node.ProcessRenderContents(*canvas_);
                ProcessBaseRenderNode(node);
            } else {
                node.ProcessAnimatePropertyBeforeChildren(*canvas_);
                node.ProcessRenderContents(*canvas_);
                ProcessBaseRenderNode(node);
                node.ProcessAnimatePropertyAfterChildren(*canvas_);
            }
            swap(cacheCanvas, canvas_);

            isOpDropped_ = isOpDropped;

            if (node.GetCacheType() == RSRenderNode::SPHERIZE) {
                RSPropertiesPainter::DrawCachedSpherizeSurface(node, *canvas_, node.GetCacheSurface());
            } else {
                RSUniRenderUtil::DrawCachedFreezeSurface(node, *canvas_, node.GetCacheSurface());
            }
            // To get all FreezeNode
            // execute: "set param rosen.dumpsurfacetype.enabled 2 && setenforce 0"
            // To get specific FreezeNode
            // execute: "set param rosen.dumpsurfacetype.enabled 1 && setenforce 0 && "
            // "set param rosen.dumpsurfaceid "NodeId" "
            // Png file could be found in /data
            RSBaseRenderUtil::WriteCacheRenderNodeToPng(node);
        } else {
            RS_LOGE("RSUniRenderVisitor::DrawChildRenderNode %" PRIu64 " Create CacheSurface failed",
                node.GetId());
        }
        isFreeze_ = false;
        RS_TRACE_END();
    }
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME("RSUniRender::Process:[" + node.GetName() + "]" + " " + node.GetDstRect().ToString()
                    + " Alpha: " + std::to_string(node.GetGlobalAlpha()).substr(0, 4));
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ",child size:%u,name:%s,OcclusionVisible:%d",
        node.GetId(), node.GetChildrenCount(), node.GetName().c_str(), node.GetOcclusionVisible());
    if (canvas_ != nullptr) {
        RSTagTracker tagTracker(canvas_->getGrContext(), node.GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);
    }
    node.UpdatePositionZ();
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_TRACE_NAME(node.GetName() + " SecurityLayer Skip");
        return;
    }
    const auto& property = node.GetRenderProperties();
    if (!node.ShouldPaint()) {
        MarkSubHardwareEnableNodeState(node);
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && isOcclusionEnabled_ && !isSecurityDisplay_) {
        MarkSubHardwareEnableNodeState(node);
        RS_TRACE_NAME(node.GetName() + " Occlusion Skip");
        return;
    }
    if (node.IsAbilityComponent() && node.GetDstRect().IsEmpty()) {
        RS_TRACE_NAME(node.GetName() + " Empty AbilityComponent Skip");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (node.IsAppWindow()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        localZOrder_ = 0.0f;
        AdjustLocalZOrder(curSurfaceNode_);
    }
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow() && !node.GetAnimateState() &&
        !node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
        RS_TRACE_NAME(node.GetName() + " QuickReject Skip");
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
        return;
    }
#endif
    node.ResetAnimateState();
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }

#ifdef RS_ENABLE_EGLQUERYSURFACE
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence visibleRegions cannot be used.
    if (isOpDropped_ && node.IsAppWindow()) {
        auto visibleRegions = node.GetVisibleRegion().GetRegionRects();
        if (visibleRegions.size() == 1) {
            canvas_->SetVisibleRect(SkRect::MakeLTRB(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_));
        }
    }
#endif

    if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        auto screenNum = screenManager->GetAllScreenIds().size();
        needColdStartThread_ = RSSystemProperties::GetColdStartThreadEnabled() &&
                               !node.IsStartAnimationFinished() && doAnimate_ && screenNum <= 1;
        needCheckFirstFrame_ = node.GetChildrenCount() > 1; // childCount > 1 means startingWindow and appWindow
    }

    if (node.IsAppWindow() && needColdStartThread_ && needCheckFirstFrame_ &&
        !RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId())) {
        if (!IsFirstFrameReadyToDraw(node)) {
            return;
        }
        auto nodePtr = node.shared_from_this();
        RSColdStartManager::Instance().StartColdStartThreadIfNeed(nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>());
        RecordAppWindowNodeAndPostTask(node, property.GetBoundsWidth(), property.GetBoundsHeight());
        return;
    }

    RSAutoCanvasRestore acr(canvas_);
    auto bgAntiAliasState = RSPropertiesPainter::GetBgAntiAlias();
    if (doAnimate_ && (!ROSEN_EQ(geoPtr->GetScaleX(), 1.f) || !ROSEN_EQ(geoPtr->GetScaleY(), 1.f))) {
        // disable background antialias when surfacenode has scale animation
        RSPropertiesPainter::SetBgAntiAlias(false);
    }

    canvas_->MultiplyAlpha(property.GetAlpha());
    canvas_->MultiplyAlpha(node.GetContextAlpha());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    // [planning] surfaceNode use frame instead
    // This is for SELF_DRAWING_NODE like RosenRenderTexture
    // BoundsRect of RosenRenderTexture is the size of video, not the size of the component.
    // The size of RosenRenderTexture is the paintRect (always be set to FrameRect) which is not passed to RenderNode
    // because RSSurfaceRenderNode is designed only affected by BoundsRect.
    // When RosenRenderTexture has child node, child node is layouted
    // according to paintRect of RosenRenderTexture, not the BoundsRect.
    // So when draw SELF_DRAWING_NODE, we should save canvas
    // to avoid child node being layout according to the BoundsRect of RosenRenderTexture.
    // Temporarily, we use parent of SELF_DRAWING_NODE which has the same paintRect with its child instead.
    // to draw child node of SELF_DRAWING_NODE
    node.CheckCacheType();
    if (isSelfDrawingSurface && node.GetCacheType() != RSRenderNode::SPHERIZE) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());
    if (node.GetCacheType() == RSRenderNode::SPHERIZE) {
        DrawChildRenderNode(node);
    } else {
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);

        if (node.GetBuffer() != nullptr) {
            if (node.IsHardwareEnabledType()) {
                node.SetHardwareForcedDisabledState(isFreeze_);
            }
            // if this window is in freeze state, disable hardware composer for its child surfaceView
            if (IsHardwareComposerEnabled() && !node.IsHardwareForcedDisabled() && node.IsHardwareEnabledType()) {
                canvas_->clear(SK_ColorTRANSPARENT);
                node.SetGlobalAlpha(canvas_->GetAlpha());
                node.SetLocalZOrder(localZOrder_++);
                ParallelRenderEnableHardwareComposer(node);
                auto dstRect = node.GetDstRect();
                SkIRect dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
                node.UpdateSrcRect(*canvas_, dst);
                RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode src:%s, dst:%s name:%s id:%" PRIu64 "",
                    node.GetSrcRect().ToString().c_str(), node.GetDstRect().ToString().c_str(),
                    node.GetName().c_str(), node.GetId());
            } else {
                node.SetGlobalAlpha(1.0f);
                auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
                renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
            }
        }

        if (isSelfDrawingSurface) {
            canvas_->restore();
        }

        if (node.IsAppWindow() &&
            (!needColdStartThread_ || !RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId()))) {
            bool needDrawCachedImage = false;
            if (RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId())) {
                if (RSColdStartManager::Instance().IsColdStartThreadIdle(node.GetId())) {
                    node.ClearCachedImage();
                    RSColdStartManager::Instance().StopColdStartThread(node.GetId());
                } else {
                    needDrawCachedImage = true;
                }
            }
            if (needCheckFirstFrame_ && IsFirstFrameReadyToDraw(node)) {
                node.NotifyUIBufferAvailable();
            }
            if (!needDrawCachedImage || node.GetCachedImage() == nullptr) {
                DrawChildRenderNode(node);
            } else {
                RS_LOGD("RSUniRenderVisitor cold start thread not idle, don't stop it, still use cached image");
                RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
            }
        } else if (node.IsAppWindow()) { // use skSurface drawn by cold start thread
            if (node.GetCachedImage() != nullptr) {
                RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
            }
            if (RSColdStartManager::Instance().IsColdStartThreadIdle(node.GetId())) {
                RecordAppWindowNodeAndPostTask(node, property.GetBoundsWidth(), property.GetBoundsHeight());
            } else {
                RS_LOGD("RSUniRenderVisitor cold start thread not idle, don't record this frame");
            }
        } else {
            ProcessBaseRenderNode(node);
        }

        if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
            // reset to default value
            needColdStartThread_ = false;
            needCheckFirstFrame_ = false;
        }

        node.ProcessAnimatePropertyAfterChildren(*canvas_);
    }

    RSPropertiesPainter::SetBgAntiAlias(bgAntiAliasState);
    if (node.IsAppWindow()) {
        canvas_->SetVisibleRect(SkRect::MakeLTRB(0, 0, 0, 0));

        // count processed canvas node
        RS_TRACE_NAME(node.GetName() + " ProcessedNodes: " + std::to_string(processedCanvasNodeInCurrentSurface_));
        processedCanvasNodeInCurrentSurface_ = 0; // reset
    }
}

void RSUniRenderVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    ProcessBaseRenderNode(node);
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    int saveCount;
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RsDebug RSBaseRenderEngine::SetColorFilterModeToPaint mode:%d", static_cast<int32_t>(colorFilterMode));
        SkPaint paint;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
        RSTagTracker tagTracker(canvas_->getGrContext(), RSTagTracker::TAGTYPE::TAG_SAVELAYER_COLOR_FILTER);
        saveCount = canvas_->saveLayer(nullptr, &paint);
    } else {
        saveCount = canvas_->save();
    }
    ProcessCanvasRenderNode(node);
    canvas_->restoreToCount(saveCount);
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    processedCanvasNodeInCurrentSurface_++;
    if (!node.ShouldPaint()) {
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isOpDropped_ && curSurfaceNode_ && !node.HasChildrenOutOfRect() &&
        !curSurfaceNode_->SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
        return;
    }
#endif
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // clip hole for driven render
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene &&
        drivenInfo_->currDrivenRenderMode != DrivenUniRenderMode::RENDER_WITH_NORMAL) {
        // skip render driven node sub tree
        if (RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(*canvas_, node)) {
            return;
        }
    }
#endif
    // in case preparation'update is skipped
    node.GetMutableRenderProperties().CheckEmptyBounds();
    node.CheckCacheType();
    auto cacheType = node.GetCacheType();
    if (cacheType == RSRenderNode::NONE) {
        node.ProcessRenderBeforeChildren(*canvas_);
        DrawChildRenderNode(node);
        node.ProcessRenderAfterChildren(*canvas_);
    } else if (cacheType == RSRenderNode::SPHERIZE) {
        node.ProcessTransitionBeforeChildren(*canvas_);
        DrawChildRenderNode(node);
        node.ProcessTransitionAfterChildren(*canvas_);
    } else {
        node.ProcessTransitionBeforeChildren(*canvas_);
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
        DrawChildRenderNode(node);
        node.ProcessRenderAfterChildren(*canvas_);
    }
}

void RSUniRenderVisitor::RecordAppWindowNodeAndPostTask(RSSurfaceRenderNode& node, float width, float height)
{
    RSRecordingCanvas canvas(width, height);
#ifdef RS_ENABLE_GL
    canvas.SetGrContext(canvas_->getGrContext()); // SkImage::MakeFromCompressed need GrContext
#endif
    auto recordingCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    swap(canvas_, recordingCanvas);
    ProcessBaseRenderNode(node);
    swap(canvas_, recordingCanvas);
    RSColdStartManager::Instance().PostPlayBackTask(node.GetId(), canvas.GetDrawCmdList(), width, height);
}

void RSUniRenderVisitor::PrepareOffscreenRender(RSRenderNode& node)
{
    RS_TRACE_NAME("PrepareOffscreenRender");
    // cleanup
    canvasBackup_ = nullptr;
    offscreenSurface_ = nullptr;
    // check offscreen size and hardware renderer
    int32_t offscreenWidth = node.GetRenderProperties().GetFrameWidth();
    int32_t offscreenHeight = node.GetRenderProperties().GetFrameHeight();
    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (canvas_->GetSurface() == nullptr) {
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, current surface is nullptr (software renderer?)");
        return;
    }
    // create offscreen surface and canvas
    auto offscreenInfo = SkImageInfo::Make(offscreenWidth, offscreenHeight, kRGBA_8888_SkColorType, kPremul_SkAlphaType,
        canvas_->GetSurface()->imageInfo().refColorSpace());
    offscreenSurface_ = canvas_->GetSurface()->makeSurface(offscreenInfo);
    if (offscreenSurface_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenSurface is nullptr");
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfiguration(*canvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::move(canvas_);
    canvas_ = std::move(offscreenCanvas);
}

void RSUniRenderVisitor::FinishOffscreenRender()
{
    if (canvasBackup_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    RS_TRACE_NAME("RSUniRenderVisitor::OffscreenRender finish");
    // flush offscreen canvas, maybe unnecessary
    canvas_->flush();
    // draw offscreen surface to current canvas
    SkPaint paint;
    paint.setAntiAlias(true);
    canvasBackup_->drawImage(offscreenSurface_->makeImageSnapshot(), 0, 0, &paint);
    // restore current canvas and cleanup
    offscreenSurface_ = nullptr;
    canvas_ = std::move(canvasBackup_);
}

bool RSUniRenderVisitor::AdaptiveSubRenderThreadMode(bool doParallel)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    doParallel = doParallel && (parallelRenderType_ != ParallelRenderingType::DISABLE);
    if (!doParallel) {
        return doParallel;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    switch (parallelRenderType_) {
        case ParallelRenderingType::AUTO:
            parallelRenderManager->SetParallelMode(doParallel);
            break;
        case ParallelRenderingType::DISABLE:
            parallelRenderManager->SetParallelMode(false);
            break;
        case ParallelRenderingType::ENABLE:
            parallelRenderManager->SetParallelMode(true);
            break;
    }
    return doParallel;
#else
    return false;
#endif
}
void RSUniRenderVisitor::ParallelRenderEnableHardwareComposer(RSSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    if (isParallel_) {
        const auto& property = node.GetRenderProperties();
        auto dstRect = node.GetDstRect();
        RectF clipRect = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetWidth(), dstRect.GetHeight()};
        RSParallelRenderManager::Instance()->AddSelfDrawingSurface(parallelRenderVisitorIndex_,
            property.GetCornerRadius().IsZero(), clipRect, property.GetCornerRadius());
    }
#endif
}

void RSUniRenderVisitor::ClosePartialRenderWhenAnimatingWindows(std::shared_ptr<RSDisplayRenderNode>& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    if (!doAnimate_) {
        return;
    }
    if (appWindowNum_ > PHONE_MAX_APP_WINDOW_NUM) {
        node->GetDirtyManager()->MergeSurfaceRect();
    } else {
        isPartialRenderEnabled_ = false;
        isOpDropped_ = false;
    }
#endif
}

void RSUniRenderVisitor::SetHardwareEnabledNodes(
    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes)
{
    hardwareEnabledNodes_ = hardwareEnabledNodes;
}

bool RSUniRenderVisitor::DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    if (!IsHardwareComposerEnabled()) {
        RS_LOGD("RSUniRenderVisitor::DoDirectComposition HardwareComposer disabled");
        return false;
    }
    RS_TRACE_NAME("DoDirectComposition");
    auto child = rootNode->GetSortedChildren().front();
    if (child == nullptr || !child->IsInstanceOf<RSDisplayRenderNode>()) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition child type not match");
        return false;
    }
    auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNode ||
        displayNode->GetCompositeType() != RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition displayNode state error");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    screenInfo_ = screenManager->QueryScreenInfo(displayNode->GetScreenId());
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: ScreenState error!");
        return false;
    }
    processor_ = RSProcessorFactory::CreateProcessor(displayNode->GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: RSProcessor is null!");
        return false;
    }

    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: renderEngine is null!");
        return false;
    }
    if (!processor_->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: processor init failed!");
        return false;
    }
    processor_->ProcessDisplaySurface(*displayNode);
    for (auto& node: hardwareEnabledNodes_) {
        if (!node->IsHardwareForcedDisabled()) {
            processor_->ProcessSurface(*node);
        }
    }
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::DoDirectComposition end");
    return true;
}

void RSUniRenderVisitor::DrawWatermarkIfNeed()
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
        sk_sp<SkImage> skImage = RSMainThread::Instance()->GetWatermarkImg();
        sk_sp<SkShader> shader = skImage->makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat);
        SkPaint rectPaint;
        rectPaint.setShader(shader);
        canvas_->drawRect(SkRect::MakeWH(screenInfo_.width, screenInfo_.height), rectPaint);
    }
}


void RSUniRenderVisitor::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSUniRenderVisitor::ParallelComposition(const std::shared_ptr<RSBaseRenderNode> rootNode)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    doParallelComposition_ = true;
    doParallelComposition_ = AdaptiveSubRenderThreadMode(doParallelComposition_) &&
                             parallelRenderManager->GetParallelMode();
    if (doParallelComposition_) {
        parallelRenderManager->PackParallelCompositionTask(shared_from_this(), rootNode);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::COMPOSITION_TASK);
        parallelRenderManager->WaitCompositionEnd();
    } else {
        return false;
    }
    return true;
#else
    return false;
#endif
}

} // namespace Rosen
} // namespace OHOS
