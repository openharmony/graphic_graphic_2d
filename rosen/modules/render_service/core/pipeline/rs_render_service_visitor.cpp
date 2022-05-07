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

#include <surface.h>
#include <window.h>
#include <window_manager.h>
#include <window_option.h>

#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

RSRenderServiceVisitor::RSRenderServiceVisitor() {}

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
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSRenderServiceVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::PrepareDisplayRenderNode mirrorSource haven't existed");
            return;
        }
        UpdateGeometry(*existingSource);
        PrepareBaseRenderNode(*existingSource);
    } else {
        UpdateGeometry(node);
        PrepareBaseRenderNode(node);
    }
}

void RSRenderServiceVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    RS_LOGD("RsDebug RSRenderServiceVisitor::ProcessDisplayRenderNode: nodeid:[%llu] screenid:[%llu] \
        isSecurityDisplay:[%s] child size:[%d] total size:[%d]", node.GetId(), node.GetScreenId(),
        isSecurityDisplay_ ? "true" : "false", node.GetChildrenCount(), node.GetSortedChildren().size());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    ScreenInfo currScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    ScreenState state = currScreenInfo.state;
    switch (state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ? RSDisplayRenderNode::CompositeType::COMPATIBLE_COMPOSITE
                                                              : RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode State is unusual");
            return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    processor_->Init(node.GetScreenId(), node.GetDisplayOffsetX(), node.GetDisplayOffsetY());

    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::ProcessDisplayRenderNode mirrorSource haven't existed");
            return;
        }
        ProcessBaseRenderNode(*existingSource);
    } else {
        ScreenRotation rotation = screenManager->GetRotation(node.GetScreenId());
        uint32_t boundWidth = currScreenInfo.width;
        uint32_t boundHeight = currScreenInfo.height;
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(boundWidth, boundHeight);
        }
        canvas_ = std::make_shared<RSPaintFilterCanvas>(new SkCanvas(boundWidth, boundHeight));
        ProcessBaseRenderNode(node);
    }
    processor_->PostProcess();
}

void RSRenderServiceVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node[%llu] prepare paused because of \
            security DisplayNode.", node.GetId());
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %llu is unvisible", node.GetId());
        return;
    }
    auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (currentGeoPtr != nullptr) {
        currentGeoPtr->UpdateByMatrixFromRenderThread(node.GetMatrix());
        currentGeoPtr->UpdateByMatrixFromSelf();
    }

    const auto updateGeometryFunc = [&](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (!node) {
            RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode this child haven't existed");
            return;
        }
        auto surfaceChild = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceChild) {
            RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode this child is not SurfaceNode");
            return;
        }
        auto childGeoPtr =
            std::static_pointer_cast<RSObjAbsGeometry>(surfaceChild->GetRenderProperties().GetBoundsGeometry());
        if (childGeoPtr != nullptr) {
            childGeoPtr->UpdateByMatrixFromParent(currentGeoPtr);
        }
    };

    for (auto& child : node.GetSortedChildren()) {
        updateGeometryFunc(child);
    }
    PrepareBaseRenderNode(node);
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!processor_) {
        RS_LOGE("RSRenderServiceVisitor::ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::ProcessSurfaceRenderNode node[%llu] process paused because of \
            security DisplayNode.", node.GetId());
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGI("RSRenderServiceVisitor::ProcessSurfaceRenderNode node : %llu is unvisible", node.GetId());
        return;
    }
    canvas_->save();
    node.SetOffset(offsetX_, offsetY_);
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.SetGlobalZOrder(globalZOrder_);
    globalZOrder_ = globalZOrder_ + 1;
    processor_->ProcessSurface(node);
    node.ProcessRenderAfterChildren(*canvas_);
    canvas_->restore();
}

void RSRenderServiceVisitor::UpdateGeometry(RSBaseRenderNode& displayNode)
{
    static const auto updateGeometryFunc = [&](const std::shared_ptr<RSBaseRenderNode>& child) {
        if (!child) {
            RS_LOGI("RSRenderServiceVisitor::PrepareDisplayRenderNode this child haven't existed");
            return;
        }
        auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceChild) {
            RS_LOGI("RSRenderServiceVisitor::PrepareDisplayRenderNode this child is not SurfaceNode");
            return;
        }
        auto childGeoPtr =
            std::static_pointer_cast<RSObjAbsGeometry>(surfaceChild->GetRenderProperties().GetBoundsGeometry());
        if (childGeoPtr != nullptr) {
            childGeoPtr->UpdateByMatrixFromParent(nullptr);
        }
    };
    for (auto& child : displayNode.GetSortedChildren()) {
        updateGeometryFunc(child);
    }
}
} // namespace Rosen
} // namespace OHOS
