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

#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
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

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t USE_CACHE_SURFACE_NUM = 7;

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

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
constexpr uint32_t PARALLEL_RENDER_MINIMUN_RENDER_NODE_NUMBER = 50;
constexpr float PARALLEL_RENDER_LAYER_Z_ORDER_FACTOR = 100.0f;
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
    isDirtyRegionDfxEnabled_ = (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebugType::EGL_DAMAGE);
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_);
    isOpaqueRegionDfxEnabled_ = RSSystemProperties::GetOpaqueRegionDfxEnabled();
    if (isDirtyRegionDfxEnabled_ && isTargetDirtyRegionDfxEnabled_) {
        isDirtyRegionDfxEnabled_ = false;
    }
    isOpDropped_ = isPartialRenderEnabled_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE)
        && (!isDirtyRegionDfxEnabled_ && !isTargetDirtyRegionDfxEnabled_ && !isOpaqueRegionDfxEnabled_);
    // this config may downgrade the calcOcclusion performance when windows number become huge (i.e. > 30), keep it now
    containerWindowConfig_ = RSSystemProperties::GetContainerWindowConfig();
    isQuickSkipPreparationEnabled_ = RSSystemProperties::GetQuickSkipPrepareEnabled();
    isHardwareComposerEnabled_ = RSSystemProperties::GetHardwareComposerEnabled();
    surfaceNodePrepareMutex_ = std::make_shared<std::mutex>();
    parallelRenderType_ = RSSystemProperties::GetParallelRenderingEnabled();
}

RSUniRenderVisitor::RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex)
    : RSUniRenderVisitor()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    parallelRenderVisitorIndex_ = surfaceIndex;
    globalZOrder_ = parallelRenderVisitorIndex_ * PARALLEL_RENDER_LAYER_Z_ORDER_FACTOR;
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
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

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
}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    node.ResetSortedChildren();
    const auto& children = node.GetSortedChildren();

    // GetSortedChildren() may remove disappearingChildren_ when transition animation end.
    // So the judgement whether node has removed child should be executed after this.
    // merge last childRect as dirty if any child has been removed
    if (curSurfaceDirtyManager_ && node.HasRemovedChild()) {
        RectI dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
        curSurfaceDirtyManager_->MergeDirtyRect(dirtyRect);
        node.ResetHasRemovedChild();
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
            if (std::find(colorGamutmodes_.begin(), colorGamutmodes_.end(),
                static_cast<ScreenColorGamut>(surfaceNodeColorSpace)) != colorGamutmodes_.end()) {
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
    screenManager->GetScreenSupportedColorGamuts(node.GetScreenId(), colorGamutmodes_);
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
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence partialrender quickreject should be disabled.
    if(node.IsRotationChanged()) {
        isOpDropped_ = false;
    }
    node.UpdateRotation();
    curAlpha_ = node.GetRenderProperties().GetAlpha();
    isParallel_ = false;
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
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
}

void RSUniRenderVisitor::ParallelPrepareDisplayRenderNodeChildrens(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    isParallel_ = AdaptiveSubRenderThreadMode(node.GetChildrenCount()) &&
        parallelRenderManager->GetParallelMode();
    isDirtyRegionAlignedEnable_ = parallelRenderManager->GetParallelModeSafe();
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
    // dirtyFlag_ includes leashwindow dirty
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
        for (size_t i = 0; i < abilityNodeIds.size(); ++i) {
            if (RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(abilityNodeIds[i])) {
                return false;
            }
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
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    for (auto& node : hardwareEnabledNodes_) {
        if (!node->ShouldPaint()) {
            continue;
        }
        canvas_->save();
        auto dstRect = node->GetDstRect();
        canvas_->clipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                            static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) });
        canvas_->clear(SK_ColorTRANSPARENT);
        canvas_->restore();
    }
}

void RSUniRenderVisitor::MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& parentNode)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (!parentNode.IsMainWindowType() && !parentNode.IsAbilityComponent()) {
        if (parentNode.IsHardwareEnabledType()) {
            parentNode.SetHardwareForcedDisabledState(true);
        }
        RS_LOGD("RSUniRenderVisitor::MarkSubHardwareEnableNodeState skip name:%s", parentNode.GetName().c_str());
        return;
    }
    pid_t pid = ExtractPid(parentNode.GetId());
    std::vector<pid_t> abilityComponentPids;
    if (parentNode.IsMainWindowType()) {
        auto abilityNodeIds = parentNode.GetAbilityNodeIds();
        for (auto& nodeId : abilityNodeIds) {
            abilityComponentPids.emplace_back(ExtractPid(nodeId));
        }
    }
    for (auto& childNode : hardwareEnabledNodes_) {
        pid_t childPid = ExtractPid(childNode->GetId());
        if (pid == childPid || std::find(abilityComponentPids.begin(), abilityComponentPids.end(), childPid) !=
            abilityComponentPids.end()) {
            childNode->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "]" + " pid: " +
        std::to_string(ExtractPid(node.GetId())));
    if (node.GetSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    // avoid mouse error
    if (node.GetName() == "pointer window") {
        isOpDropped_ = false;
        isPartialRenderEnabled_ = false;
    }
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

    // if currentsurfacenode is a main window type, reset the curSurfaceDirtyManager
    // reset leash window's dirtyManager pointer to avoid curSurfaceDirtyManager mis-pointing
    if (node.IsMainWindowType() || node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        curSurfaceDirtyManager_->Clear();
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
    }
    // [planning] IsMainWindowType should contain ABILITY_COMPONENT_NODE
    // this branch should be included in other judgment
    if (node.IsAbilityComponent() && curSurfaceNode_) {
        curSurfaceNode_->UpdateAbilityNodeIds(node.GetId());
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

    if (node.IsMainWindowType()) {
        // record node position for display render node dirtyManager
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetDstRect());

        if (node.IsAppWindow()) {
            curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
            // if update appwindow, its children should not skip
            isQuickSkipPreparationEnabled_ = false;
            node.ResetAbilityNodeIds();
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

    node.UpdateChildrenOutOfRectFlag(false);
    if (node.ShouldPrepareSubnodes()) {
        PrepareBaseRenderNode(node);
    }
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
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
    if (node.GetDirtyManager() && node.GetDirtyManager()->IsDirty()) {
        dirtySurfaceNodeMap_.emplace(node.GetId(), node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    if (node.IsAppWindow()) {
        RS_TRACE_NAME(node.GetName() + " PreparedNodes: " + std::to_string(preparedCanvasNodeInCurrentSurface_));
        preparedCanvasNodeInCurrentSurface_ = 0;
    }
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
        // Partical rendering of node with filter would cause display problem.
        curSurfaceNode_->UpdateChildrenFilterRects(node.GetOldDirtyInSurface());
    }
    curAlpha_ = alpha;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}


void RSUniRenderVisitor::CopyForParallelPrepare(std::shared_ptr<RSUniRenderVisitor> visitor)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    isPartialRenderEnabled_ &= visitor->isPartialRenderEnabled_;
    isOpDropped_ &= visitor->isOpDropped_;
    needFilter_ |= visitor->needFilter_;
    for (auto &u : visitor->displayHasSecSurface_) {
        displayHasSecSurface_[u.first] |= u.second;
    }

    for (auto &u : visitor->dirtySurfaceNodeMap_) {
        dirtySurfaceNodeMap_[u.first] = u.second;
    }
#endif
}

void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int edgeWidth = 6)
{
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = [%d, %d, %d, %d]", dirtyRect.left_, dirtyRect.top_,
        dirtyRect.width_, dirtyRect.height_);
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
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
        rects.emplace_back(RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    DrawDirtyRegionForDFX(rects);

    // draw display dirtyregion with red color
    RectI dirtySurfaceRect = node.GetDirtyManager()->GetDirtyRegion();
    const float fillAlpha = 0.2;
    DrawDirtyRectForDFX(dirtySurfaceRect, SK_ColorRED, SkPaint::kStroke_Style, fillAlpha);
}

void RSUniRenderVisitor::DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().begin(); it != node.GetCurAllSurfaces().end(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
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
        if (std::find(dfxTargetSurfaceNames_.begin(), dfxTargetSurfaceNames_.end(),
            surfaceNode->GetName()) != dfxTargetSurfaceNames_.end()) {
            auto visibleDirtyRegions = surfaceNode->GetVisibleDirtyRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto rect : visibleDirtyRegions) {
                rects.emplace_back(RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_));
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
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" +
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
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
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID)) {
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

    if (mirrorNode) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        if (displayHasSecSurface_[mirrorNode->GetScreenId()] && mirrorNode->GetSecurityDisplay() != isSecurityDisplay_
            && processor) {
            canvas_ = processor->GetCanvas();
            ProcessBaseRenderNode(*mirrorNode);
        } else {
            processor_->ProcessDisplaySurface(*mirrorNode);
        }
    } else if (node.GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        ProcessBaseRenderNode(node);
    } else {
#ifdef RS_ENABLE_EGLQUERYSURFACE
        if (isPartialRenderEnabled_) {
            curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
            CalcDirtyDisplayRegion(displayNodePtr);
            CalcDirtyRegionForFilterNode(displayNodePtr);
            displayNodePtr->ClearCurrentSurfacePos();
        }
        if (isOpDropped_ && dirtySurfaceNodeMap_.empty() && !curDisplayDirtyManager_->IsDirty()) {
            RS_LOGD("DisplayNode skip");
            RS_TRACE_NAME("DisplayNode skip");
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
        auto renderFrame = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface),
            RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true));
        RS_TRACE_END();
        if (renderFrame == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
        AddOverDrawListener(renderFrame, overdrawListener);

        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }
        int saveLayerCnt = 0;
        SkRegion region;
        Occlusion::Region dirtyRegionTest;
        std::vector<RectI> rects;
#ifdef RS_ENABLE_EGLQUERYSURFACE
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (isPartialRenderEnabled_) {
            // Early history buffer Merging will have impact on Overdraw display, so we need to
            // set the full screen dirty to avoid this impact.
            if (RSOverdrawController::GetInstance().IsEnabled()) {
                node.GetDirtyManager()->ResetDirtyAsSurfaceSize();
            }
            int bufferAge = renderFrame->GetBufferAge();
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
                renderFrame->SetDamageRegion(rects);
            }
        }
        if (isOpDropped_ && !isDirtyRegionAlignedEnable_) {
            if (region.isEmpty()) {
                // [planning] Remove this after frame buffer can cancel
                canvas_->clipRect(SkRect::MakeEmpty());
            } else if (region.isRect()) {
                canvas_->clipRegion(region);
            } else {
                SkPath dirtyPath;
                region.getBoundaryPath(&dirtyPath);
                canvas_->clipPath(dirtyPath, true);
                ClearTransparentBeforeSaveLayer();
                // [planning] Remove this after skia is upgraded, the clipRegion is supported
                if (!needFilter_) {
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
                canvas_->SetCacheEnabled(geoPtr->IsNeedClientCompose());
            }
            canvas_->SetCacheEnabled(canvas_->isCacheEnabled() ||
                node.GetCurAllSurfaces().size() > USE_CACHE_SURFACE_NUM);
            if (canvas_->isCacheEnabled()) {
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
        if (isParallel_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) {
            ClearTransparentBeforeSaveLayer();
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            parallelRenderManager->SetFrameSize(screenInfo_.width, screenInfo_.height);
            parallelRenderManager->CopyVisitorAndPackTask(*this, node);
            parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
            parallelRenderManager->MergeRenderResult(canvas_);
            parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
        }
#endif
        if (saveLayerCnt > 0) {
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->restoreToCount(saveLayerCnt);
        }

        if (overdrawListener != nullptr) {
            overdrawListener->Draw();
        }
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
        renderFrame->Flush();
        RS_TRACE_END();
        RS_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_TRACE_END();
        AdjustZOrderAndCreateLayer();
        node.SetGlobalZOrder(globalZOrder_);
        processor_->ProcessDisplaySurface(node);
    }
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::AdjustZOrderAndCreateLayer()
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    // create layer for hardwareEnabledNodes_
    globalZOrder_ = 0.0f;
    // sort the surfaceNodes by ZOrder
    std::stable_sort(hardwareEnabledNodes_.begin(), hardwareEnabledNodes_.end(),
        [](const auto& first, const auto& second) -> bool {
        return first->GetGlobalZOrder() < second->GetGlobalZOrder();
    });

    for (auto& surfaceNode : hardwareEnabledNodes_) {
        if (!surfaceNode->GetHardwareForcedDisabledState()) {
            RS_LOGD("createLayer: %" PRIu64 "", surfaceNode->GetId());
            // SetGlobalZOrder again to ensure ZOrder committed to composer is continuous
            surfaceNode->SetGlobalZOrder(globalZOrder_++);
            processor_->ProcessSurface(*surfaceNode);
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
    auto skSurface = renderFrame->GetFrame()->GetSurface();
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
            RS_LOGD("CalcDirtyDisplayRegion merge ShadowValid %s rect %s",
                surfaceNode->GetName().c_str(), surfaceNode->GetOldDirtyInSurface().ToString().c_str());
            // There are two situation here:
            // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
            // surfaceDirtyRect == surfaceNode->GetOldDirtyInSurface()
            // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
            // surfaceDirtyRect > surfaceNode->GetOldDirtyInSurface()
            // So we should always merge surfaceDirtyRect here.
            if (!shadowDirtyRect.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(surfaceDirtyRect);
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
                for (auto iter = node->GetCurAllSurfaces().begin(); iter != node->GetCurAllSurfaces().end(); ++iter) {
                    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
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
    for (auto it = node->GetCurAllSurfaces().begin(); it != node->GetCurAllSurfaces().end(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
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
    // calcultae extra dirty region after 32 bits alignedment
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
    for (auto it = node->GetCurAllSurfaces().begin(); it != node->GetCurAllSurfaces().end(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
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
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    node.SetCacheSurface(SkSurface::MakeRenderTarget(canvas_->getGrContext(), SkBudgeted::kYes, info));
#else
    node.SetCacheSurface(SkSurface::MakeRasterN32Premul(width, height));
#endif
}

void RSUniRenderVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    if (!node.IsFreeze()) {
        ProcessBaseRenderNode(node);
        node.ClearCacheSurface();
    } else if (node.GetCacheSurface()) {
        RSUniRenderUtil::DrawCachedSurface(node, *canvas_, node.GetCacheSurface());
    } else {
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
            ProcessBaseRenderNode(node);
            swap(cacheCanvas, canvas_);

            isOpDropped_ = isOpDropped;

            RSUniRenderUtil::DrawCachedSurface(node, *canvas_, node.GetCacheSurface());
            // To get all FreezeNode
            // execute: "set param rosen.dumpsurfacetype.enabled 2 && setenforce 0"
            // To get specific FreezeNode
            // execute: "set param rosen.dumpsurfacetype.enabled 1 && setenforce 0 && "
            // "set param rosen.dumpsurfaceid "NodeId" "
            // Png file could be found in /data
            RSBaseRenderUtil::WriteFreezeRenderNodeToPng(node);
        } else {
            RS_LOGE("RSUniRenderVisitor::DrawChildRenderNode %" PRIu64 " Create CacheSurface failed",
                node.GetId());
        }
        isFreeze_ = false;
    }
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME("RSUniRender::Process:[" + node.GetName() + "]" + " " + node.GetDstRect().ToString()
                    + " Alpha: " + std::to_string(node.GetGlobalAlpha()).substr(0, 4));
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 ", child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());
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
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        MarkSubHardwareEnableNodeState(node);
        RS_TRACE_NAME(node.GetName() + " Occlusion Skip");
        return;
    }
    if (node.IsAbilityComponent() && node.GetDstRect().IsEmpty()) {
        RS_TRACE_NAME(node.GetName() + " Empty AbilityComponent Skip");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
            RS_TRACE_NAME(node.GetName() + " QuickReject Skip");
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
            return;
        }
    }
    if (node.IsAppWindow()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    }
#endif

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
        needColdStartThread_ = RSSystemProperties::GetColdStartThreadEnabled() &&
                               !node.IsStartAnimationFinished() && doAnimate_;
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

    auto savedState = canvas_->SaveCanvasAndAlpha();
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
    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);

    // Fix bug that when AppWindow has shadow set by config.xml cannot be displayed for LEASH_WINDOW_NODE has clipped canvas.
    if (node.GetSurfaceNodeType() != RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        if (!property.GetCornerRadius().IsZero()) {
            canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
        } else {
            canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
        }
    }

    RSPropertiesPainter::DrawBackground(property, *canvas_);
    RSPropertiesPainter::DrawMask(property, *canvas_);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }

    node.SetTotalMatrix(canvas_->getTotalMatrix());

    if (node.GetBuffer() != nullptr) {
        if (node.IsHardwareEnabledType()) {
            node.SetHardwareForcedDisabledState(isFreeze_);
        }
        // if this window is in freeze state, disable hardware composer for its child surfaceView
        if (IsHardwareComposerEnabled() && !isFreeze_ && node.IsHardwareEnabledType() &&
            node.GetDstRect().GetWidth() > 1 && node.GetDstRect().GetHeight() > 1) { // avoid fallback by composer
            canvas_->clear(SK_ColorTRANSPARENT);
            node.SetGlobalAlpha(canvas_->GetAlpha());
            node.SetGlobalZOrder(globalZOrder_++);
            ParallelRenderEnableHardwareComposer(node);
            auto dstRect = node.GetDstRect();
            SkIRect dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
            node.UpdateSrcRect(*canvas_, dst);
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode src:%s, dst:%s name:%s id:%" PRIu64 "",
                node.GetSrcRect().ToString().c_str(), node.GetDstRect().ToString().c_str(),
                node.GetName().c_str(), node.GetId());
        } else {
            if (node.IsHardwareEnabledType()) {
                node.SetHardwareForcedDisabledState(true);
            }
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
        if (RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId())) {
            node.ClearCachedImage();
            RSColdStartManager::Instance().StopColdStartThread(node.GetId());
        }
        if (needCheckFirstFrame_ && IsFirstFrameReadyToDraw(node)) {
            node.NotifyUIBufferAvailable();
        }
        DrawChildRenderNode(node);
    } else if (node.IsAppWindow()) { // use skSurface drawn by cold start thread
        if (node.GetCachedImage() != nullptr) {
            RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
        }
        RecordAppWindowNodeAndPostTask(node, property.GetBoundsWidth(), property.GetBoundsHeight());
    } else {
        ProcessBaseRenderNode(node);
    }

    if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        // reset to default value
        needColdStartThread_ = false;
        needCheckFirstFrame_ = false;
    }

    filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }

    RSPropertiesPainter::SetBgAntiAlias(bgAntiAliasState);
    canvas_->RestoreCanvasAndAlpha(savedState);
    if (node.IsAppWindow()) {
        canvas_->SetVisibleRect(SkRect::MakeLTRB(0, 0, 0, 0));

        // count processed canvasnodes number
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
        RS_LOGD("RSUniRenderVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isOpDropped_ && curSurfaceNode_ &&
        !curSurfaceNode_->SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_) &&
        !node.HasChildrenOutOfRect()) {
        return;
    }
#endif
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    // in case preparation'update is skipped
    node.GetMutableRenderProperties().CheckEmptyBounds();
    node.ProcessRenderBeforeChildren(*canvas_);
    DrawChildRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
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

bool RSUniRenderVisitor::AdaptiveSubRenderThreadMode(uint32_t renderNodeNum)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    bool isParallel = (renderNodeNum >= PARALLEL_RENDER_MINIMUN_RENDER_NODE_NUMBER) &&
        (parallelRenderType_ != ParallelRenderingType::DISABLE);
    if (!isParallel) {
        return isParallel;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    switch (parallelRenderType_) {
        case ParallelRenderingType::AUTO:
            parallelRenderManager->SetParallelMode(isParallel);
            break;
        case ParallelRenderingType::DISABLE:
            parallelRenderManager->SetParallelMode(false);
            break;
        case ParallelRenderingType::ENABLE:
            parallelRenderManager->SetParallelMode(true);
            break;
    }
    return isParallel;
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
    if (!processor_->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID)) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: processor init failed!");
        return false;
    }
    processor_->ProcessDisplaySurface(*displayNode);
    for (auto& node: hardwareEnabledNodes_) {
        if (!node->GetHardwareForcedDisabledState()) {
            processor_->ProcessSurface(*node);
        }
    }
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::DoDirectComposition end");
    return true;
}
} // namespace Rosen
} // namespace OHOS
