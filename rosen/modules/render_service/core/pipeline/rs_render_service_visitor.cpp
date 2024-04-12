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

#include "pipeline/rs_render_service_visitor.h"

#include "rs_divided_render_util.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

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

void RSRenderServiceVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSRenderServiceVisitor::PrepareDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    ScreenState state = curScreenInfo.state;
    switch (state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE:
                RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            RS_LOGE("RSRenderServiceVisitor::PrepareDisplayRenderNode State is unusual");
            return;
    }

    ScreenRotation rotation = node.GetRotation();
    int32_t logicalScreenWidth = static_cast<int32_t>(node.GetRenderProperties().GetFrameWidth());
    int32_t logicalScreenHeight = static_cast<int32_t>(node.GetRenderProperties().GetFrameHeight());
    if (logicalScreenWidth <= 0 || logicalScreenHeight <= 0) {
        logicalScreenWidth = static_cast<int32_t>(curScreenInfo.width);
        logicalScreenHeight = static_cast<int32_t>(curScreenInfo.height);
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(logicalScreenWidth, logicalScreenHeight);
        }
    }

    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::PrepareDisplayRenderNode mirrorSource haven't existed");
            return;
        }
        if (mParallelEnable) {
            drawingCanvas_ = std::make_unique<Drawing::Canvas>(logicalScreenWidth, logicalScreenHeight);
            canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
            Drawing::Rect tmpRect(0, 0, logicalScreenWidth, logicalScreenHeight);
            canvas_->ClipRect(tmpRect, Drawing::ClipOp::INTERSECT, false);
        }
        PrepareChildren(*existingSource);
    } else {
        auto boundsGeoPtr = (node.GetRenderProperties().GetBoundsGeometry());
        RSBaseRenderUtil::SetNeedClient(boundsGeoPtr && boundsGeoPtr->IsNeedClientCompose());
        drawingCanvas_ = std::make_unique<Drawing::Canvas>(logicalScreenWidth, logicalScreenHeight);
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
        Drawing::Rect tmpRect(0, 0, logicalScreenWidth, logicalScreenHeight);
        canvas_->ClipRect(tmpRect, Drawing::ClipOp::INTERSECT, false);
        PrepareChildren(node);
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), false, false);
}

void RSRenderServiceVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    /* need reset isSecurityDisplay_ on ProcessDisplayRenderNode */
    isSecurityDisplay_ = node.GetSecurityDisplay();
    RS_LOGD("RsDebug RSRenderServiceVisitor::ProcessDisplayRenderNode: nodeid:[%{public}" PRIu64 "]"
        " screenid:[%{public}" PRIu64 "] \
        isSecurityDisplay:[%{public}s] child size:[%{public}d]",
        node.GetId(), node.GetScreenId(), isSecurityDisplay_ ? "true" : "false", node.GetChildrenCount());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]");
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (node.SkipFrame(curScreenInfo.skipFrameInterval)) {
        RS_TRACE_NAME("SkipFrame, screenId:" + std::to_string(node.GetScreenId()));
        screenManager->ForceRefreshOneFrameIfNoRNV();
        return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();

    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        processorRenderEngine_ = mainThread->GetRenderEngine();
    }
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, processorRenderEngine_)) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }

    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::ProcessDisplayRenderNode mirrorSource haven't existed");
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
    for (auto& [_, funcs] : foregroundSurfaces_) {
        for (const auto& func : funcs) {
            func();
        }
    }
    foregroundSurfaces_.clear();
    processor_->PostProcess();
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

    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : [%{public}" PRIu64 "] prepare paused \
            because of security SurfaceNode.", node.GetId());
        return;
    }
    
    if (isSecurityDisplay_ && node.GetSkipLayer()) {
        RS_LOGD("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : [%{public}" PRIu64 "] prepare paused \
            because of skip SurfaceNode.", node.GetId());
        return;
    }
    if (!canvas_) {
        RS_LOGD("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %{public}" PRIu64 " canvas is nullptr",
            node.GetId());
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }
    node.SetOffset(offsetX_, offsetY_);
    node.PrepareRenderBeforeChildren(*canvas_);
    PrepareChildren(node);
    node.PrepareRenderAfterChildren(*canvas_);
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!processor_) {
        RS_LOGE("RSRenderServiceVisitor::ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
   
    if (!node.ShouldPaint()) {
        RS_LOGD("RSRenderServiceVisitor::ProcessSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        return;
    }
    if (isSecurityDisplay_ && node.GetSkipLayer()) {
        RS_LOGD("RSRenderServiceVisitor::ProcessSurfaceRenderNode node[%{public}" PRIu64 "] process paused \
            because of skip SurfaceNode.", node.GetId());
        return;
    }
    if (mParallelEnable) {
        node.ParallelVisitLock();
    }
    ProcessChildren(node);
    auto func = [nodePtr = node.ReinterpretCastTo<RSSurfaceRenderNode>(), this]() {
        nodePtr->SetGlobalZOrder(globalZOrder_);
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
} // namespace Rosen
} // namespace OHOS
