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

#include "pipeline/main_thread/rs_render_service_visitor.h"

#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderServiceVisitor"

namespace OHOS {
namespace Rosen {

RSRenderServiceVisitor::RSRenderServiceVisitor(bool parallel) : mParallelEnable(parallel) {}

RSRenderServiceVisitor::~RSRenderServiceVisitor() {}

void RSRenderServiceVisitor::PrepareChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderServiceVisitor::ProcessChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSRenderServiceVisitor::PrepareScreenRenderNode(RSScreenRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("PrepareScreenRenderNode ScreenManager is nullptr");
        return;
    }
    node.SetScreenInfo(screenManager->QueryScreenInfo(node.GetScreenId()));
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    offsetX_ = curScreenInfo.offsetX;
    offsetY_ = curScreenInfo.offsetY;
    UpdateScreenNodeCompositeType(node, curScreenInfo);

    ResetSurfaceNodeAttrsInScreenNode(node);

    curScreenNode_ = node.shared_from_this()->ReinterpretCastTo<RSScreenRenderNode>();

    int32_t logicalScreenWidth = static_cast<int32_t>(node.GetRenderProperties().GetFrameWidth());
    int32_t logicalScreenHeight = static_cast<int32_t>(node.GetRenderProperties().GetFrameHeight());
    if (logicalScreenWidth <= 0 || logicalScreenHeight <= 0) {
        logicalScreenWidth = static_cast<int32_t>(curScreenInfo.width);
        logicalScreenHeight = static_cast<int32_t>(curScreenInfo.height);
    }

    if (node.IsMirrorScreen()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("PrepareScreenRenderNode mirrorSource haven't existed");
            return;
        }
        if (mParallelEnable) {
            CreateCanvas(logicalScreenWidth, logicalScreenHeight, true);
        }
        PrepareChildren(*existingSource);
    } else {
        auto& boundsGeoPtr = (node.GetRenderProperties().GetBoundsGeometry());
        RSBaseRenderUtil::SetNeedClient(boundsGeoPtr && boundsGeoPtr->IsNeedClientCompose());
        CreateCanvas(logicalScreenWidth, logicalScreenHeight);
        PrepareChildren(node);
    }
    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), false, false);
}

void RSRenderServiceVisitor::PrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node)
{
    PrepareChildren(node);
}

void RSRenderServiceVisitor::ProcessScreenRenderNode(RSScreenRenderNode& node)
{
    // need reset isSecurityDisplay_ on ProcessScreenRenderNode
    RS_LOGD("RsDebug ProcessScreenRenderNode: nodeid:[%{public}" PRIu64 "]"
        " screenid:[%{public}" PRIu64 "] isSecurityDisplay:[%{public}s] child size:[%{public}d]",
        node.GetId(), node.GetScreenId(), isSecurityDisplay_ ? "true" : "false", node.GetChildrenCount());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("ProcessScreenRenderNode ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    RS_TRACE_NAME("ProcessScreenRenderNode[" + std::to_string(node.GetScreenId()) + "]");
    RSScreenModeInfo modeInfo = {};
    screenManager->GetDefaultScreenActiveMode(modeInfo);
    uint32_t refreshRate = modeInfo.GetScreenRefreshRate();
    screenManager->RemoveForceRefreshTask();
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (node.SkipFrame(refreshRate, curScreenInfo.skipFrameInterval)) {
        RS_TRACE_NAME("SkipFrame, screenId:" + std::to_string(node.GetScreenId()));
        screenManager->PostForceRefreshTask();
        return;
    }

    curScreenNode_ = node.shared_from_this()->ReinterpretCastTo<RSScreenRenderNode>();

    if (!CreateProcessor(node)) {
        return;
    }

    if (node.IsMirrorScreen()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("ProcessScreenRenderNode mirrorSource haven't existed");
            return;
        }
        if (isSecurityDisplay_ && displayHasSecSurface_[node.GetScreenId()]) {
            processor_->SetSecurityDisplay(isSecurityDisplay_);
            processor_->SetDisplayHasSecSurface(true);
            processor_->PostProcess();
            return;
        }
        ProcessChildren(*existingSource);
    } else {
        ProcessChildren(node);
    }
    processor_->PostProcess();
}

void RSRenderServiceVisitor::ProcessLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node)
{
    ProcessChildren(node);
    for (auto& [_, funcs] : foregroundSurfaces_) {
        for (const auto& func : funcs) {
            func();
        }
    }
    foregroundSurfaces_.clear();
}

void RSRenderServiceVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (RSInnovation::GetParallelCompositionEnabled(false)) {
        typedef bool (*CheckForSerialForcedFunc)(std::string&);
        CheckForSerialForcedFunc CheckForSerialForced =
            reinterpret_cast<CheckForSerialForcedFunc>(RSInnovation::_s_checkForSerialForced);
        auto name = node.GetName();
        mForceSerial |= CheckForSerialForced(name);
    }

    if (isSecurityDisplay_ && node.GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY)) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
        RS_LOGI("PrepareSurfaceRenderNode node: [%{public}" PRIu64 "] prepare paused "
            "because of security SurfaceNode.", node.GetId());
        return;
    }
    
    if (isSecurityDisplay_ && node.GetSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
        RS_LOGD("PrepareSurfaceRenderNode node : [%{public}" PRIu64 "] prepare paused "
            "because of skip SurfaceNode.", node.GetId());
        return;
    }
    if (!canvas_) {
        RS_LOGD("PrepareSurfaceRenderNode node : %{public}" PRIu64 " canvas is nullptr",
            node.GetId());
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("PrepareSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }

    node.SetVisibleRegion(Occlusion::Region());
    node.SetOffset(offsetX_, offsetY_);
    node.PrepareRenderBeforeChildren(*canvas_);
    PrepareChildren(node);
    node.PrepareRenderAfterChildren(*canvas_);
    
    if (curScreenNode_) {
        StoreSurfaceNodeAttrsToScreenNode(*curScreenNode_, node);
    }
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!processor_) {
        RS_LOGE("ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
   
    if (curScreenNode_) {
        RestoreSurfaceNodeAttrsFromScreenNode(*curScreenNode_, node);
        node.SetOffset(curScreenNode_->GetScreenOffsetX(), curScreenNode_->GetScreenOffsetY());
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("ProcessSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        return;
    }
    if (isSecurityDisplay_ && node.GetMultableSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
        RS_LOGD("ProcessSurfaceRenderNode node[%{public}" PRIu64 "] process paused "
            "because of skip SurfaceNode.", node.GetId());
        return;
    }
    if (mParallelEnable) {
        node.ParallelVisitLock();
    }
    ProcessChildren(node);
    auto func = [nodePtr = node.ReinterpretCastTo<RSSurfaceRenderNode>(), this]() {
        nodePtr->GetMutableRSSurfaceHandler()->SetGlobalZOrder(globalZOrder_);
        globalZOrder_ = globalZOrder_ + 1;
        processor_->ProcessSurface(*nodePtr);
    };
    if (node.GetIsForeground()) {
        auto parent = node.GetParent().lock();
        foregroundSurfaces_[parent ? parent->GetId() : 0].push_back(func);
    } else {
        func();
    }
    auto it = foregroundSurfaces_.find(node.GetId());
    if (it != foregroundSurfaces_.end()) {
        for (auto f : foregroundSurfaces_[node.GetId()]) {
            f();
        }
        foregroundSurfaces_.erase(it);
    }
    RSBaseRenderUtil::WriteSurfaceRenderNodeToPng(node);
    if (mParallelEnable) {
        node.ParallelVisitUnlock();
    }
}

void RSRenderServiceVisitor::CreateCanvas(int32_t width, int32_t height, bool isMirrored)
{
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(width, height);
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    Drawing::Rect tmpRect(0, 0, width, height);
    canvas_->ClipRect(tmpRect, Drawing::ClipOp::INTERSECT, false);
    if (!isMirrored) {
        Drawing::Matrix matrix;
        matrix.Translate(offsetX_ * -1, offsetY_ * -1);
        canvas_->SetMatrix(matrix);
    }
}

bool RSRenderServiceVisitor::CreateProcessor(RSScreenRenderNode& node)
{
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("CreateProcessor: processor_ is null!");
        return false;
    }

    auto mirrorNode = node.GetMirrorSource().lock();

    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        processorRenderEngine_ = mainThread->GetRenderEngine();
    }

    if (!processor_->Init(node, node.GetScreenOffsetX(), node.GetScreenOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, processorRenderEngine_)) {
        RS_LOGE("ProcessDisplayRenderNode: processor init failed!");
        return false;
    }

    return true;
}

void RSRenderServiceVisitor::UpdateScreenNodeCompositeType(RSScreenRenderNode& node, const ScreenInfo& screenInfo)
{
    ScreenState state = screenInfo.state;
    switch (state) {
        case ScreenState::SOFTWARE_OUTPUT_ENABLE:
            node.SetCompositeType(CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                CompositeType::SOFTWARE_COMPOSITE:
                CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            RS_LOGE("PrepareDisplayRenderNode State is unusual");
            return;
    }
}

void RSRenderServiceVisitor::StoreSurfaceNodeAttrsToScreenNode(
    RSScreenRenderNode& screenNode, const RSSurfaceRenderNode& surfaceNode)
{
    screenNode.SetSurfaceSrcRect(surfaceNode.GetId(), surfaceNode.GetSrcRect());
    screenNode.SetSurfaceDstRect(surfaceNode.GetId(), surfaceNode.GetDstRect());
    screenNode.SetSurfaceTotalMatrix(surfaceNode.GetId(), surfaceNode.GetTotalMatrix());
}

void RSRenderServiceVisitor::RestoreSurfaceNodeAttrsFromScreenNode(
    const RSScreenRenderNode& screenNode, RSSurfaceRenderNode& surfaceNode)
{
    surfaceNode.SetSrcRect(screenNode.GetSurfaceSrcRect(surfaceNode.GetId()));
    surfaceNode.SetDstRect(screenNode.GetSurfaceDstRect(surfaceNode.GetId()));
    surfaceNode.SetTotalMatrix(screenNode.GetSurfaceTotalMatrix(surfaceNode.GetId()));
}

void RSRenderServiceVisitor::ResetSurfaceNodeAttrsInScreenNode(RSScreenRenderNode& screenNode)
{
    screenNode.ClearSurfaceSrcRect();
    screenNode.ClearSurfaceDstRect();
    screenNode.ClearSurfaceTotalMatrix();
}

} // namespace Rosen
} // namespace OHOS
