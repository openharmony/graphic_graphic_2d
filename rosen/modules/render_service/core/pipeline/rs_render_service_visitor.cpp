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

#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

RSRenderServiceVisitor::RSRenderServiceVisitor(bool parallel) : mParallelEnable(parallel) {}

RSRenderServiceVisitor::~RSRenderServiceVisitor() {}

void RSRenderServiceVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderServiceVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    if (!mParallelEnable) {
        // clear SortedChildren, it will be generated again in next frame
        node.ResetSortedChildren();
    }
}

void RSRenderServiceVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    node.ApplyModifiers();
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
            skCanvas_ = std::make_unique<SkCanvas>(logicalScreenWidth, logicalScreenHeight);
            canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_.get());
            canvas_->clipRect(SkRect::MakeWH(logicalScreenWidth, logicalScreenHeight));
        }
        PrepareBaseRenderNode(*existingSource);
    } else {
        auto boundsGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        RSDividedRenderUtil::SetNeedClient(boundsGeoPtr && boundsGeoPtr->IsNeedClientCompose());
        skCanvas_ = std::make_unique<SkCanvas>(logicalScreenWidth, logicalScreenHeight);
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_.get());
        canvas_->clipRect(SkRect::MakeWH(logicalScreenWidth, logicalScreenHeight));
        PrepareBaseRenderNode(node);
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), false);
}

void RSRenderServiceVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    RS_LOGD("RsDebug RSRenderServiceVisitor::ProcessDisplayRenderNode: nodeid:[%" PRIu64 "] screenid:[%" PRIu64 "] \
        isSecurityDisplay:[%s] child size:[%d] total size:[%d]",
        node.GetId(), node.GetScreenId(), isSecurityDisplay_ ? "true" : "false", node.GetChildrenCount(),
        node.GetSortedChildren().size());
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
        return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID)) {
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
        ProcessBaseRenderNode(*existingSource);
    } else {
        ProcessBaseRenderNode(node);
    }
    processor_->PostProcess();
}

void RSRenderServiceVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (RSInnovation::GetParallelCompositionEnabled()) {
        typedef bool (*CheckForSerialForcedFunc)(std::string&);
        CheckForSerialForcedFunc CheckForSerialForced =
            reinterpret_cast<CheckForSerialForcedFunc>(RSInnovation::_s_checkForSerialForced);
        auto name = node.GetName();
        mForceSerial |= CheckForSerialForced(name);
    }

    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node[%" PRIu64 "] prepare paused because of \
            security DisplayNode.",
            node.GetId());
        return;
    }
    if (!canvas_) {
        RS_LOGD("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %" PRIu64 " canvas is nullptr", node.GetId());
        return;
    }
    node.ApplyModifiers();
    if (!node.ShouldPaint()) {
        RS_LOGD("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %" PRIu64 " is invisible", node.GetId());
        return;
    }
    node.SetOffset(offsetX_, offsetY_);
    node.PrepareRenderBeforeChildren(*canvas_);
    PrepareBaseRenderNode(node);
    node.PrepareRenderAfterChildren(*canvas_);
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!processor_) {
        RS_LOGE("RSRenderServiceVisitor::ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::ProcessSurfaceRenderNode node[%" PRIu64 "] process paused because of \
            security DisplayNode.",
            node.GetId());
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSRenderServiceVisitor::ProcessSurfaceRenderNode node : %" PRIu64 " is invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        return;
    }
    if (mParallelEnable) {
        node.ParallelVisitLock();
    }
    ProcessBaseRenderNode(node);
    node.SetGlobalZOrder(globalZOrder_);
    globalZOrder_ = globalZOrder_ + 1;
    processor_->ProcessSurface(node);
    RSBaseRenderUtil::WriteSurfaceRenderNodeToPng(node);
    if (mParallelEnable) {
        node.ParallelVisitUnlock();
    }
}
} // namespace Rosen
} // namespace OHOS
