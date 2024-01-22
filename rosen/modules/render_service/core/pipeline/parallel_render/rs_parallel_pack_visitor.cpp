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

#include "rs_parallel_pack_visitor.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
RSParallelPackVisitor::RSParallelPackVisitor(RSUniRenderVisitor &visitor)
    : partialRenderType_(RSSystemProperties::GetUniPartialRenderEnabled())
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    isPartialRenderEnabled_ = visitor.GetIsPartialRenderEnabled();
    isOpDropped_ = visitor.GetIsOpDropped();
    doAnimate_ = visitor.GetAnimateState();
}

void RSParallelPackVisitor::PrepareChildren(RSRenderNode &node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSParallelPackVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode &node)
{
    isFirstSurfaceNode_ = true;
    PrepareChildren(node);
}

void RSParallelPackVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    if (!isFirstSurfaceNode_) {
        RSParallelRenderManager::Instance()->PackRenderTask(node, TaskType::PREPARE_TASK);
    } else {
        isFirstSurfaceNode_ = false;
        RSParallelRenderManager::Instance()->WorkSerialTask(node);
    }
}

void RSParallelPackVisitor::ProcessChildren(RSRenderNode &node)
{
    for (auto &child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSParallelPackVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RSParallelRenderManager::Instance()->ClearFilterSurfaceRenderNode();
    isSecurityDisplay_ = node.GetSecurityDisplay();
    ProcessChildren(node);
}

void RSParallelPackVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    auto instance = RSParallelRenderManager::Instance();
    // DockView and windows whose z-order is larger than DockView are drawn in render service main thread.
    if (node.GetName() == "DockView" || instance->GetFilterSurfaceRenderNodeCount() > 0) {
        instance->SetFilterSurfaceRenderNode(node);
        return;
    }
    RS_TRACE_NAME("RSParallelPackVisitor::Process:[" + node.GetName() + "]" + node.GetDstRect().ToString());
    RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 ", child size:%{public}u"
        " %{public}s", node.GetId(), node.GetChildrenCount(), node.GetName().c_str());
    node.UpdatePositionZ();
    if (IsSkipProcessing(node)) {
        return;
    }
    instance->PackRenderTask(node, TaskType::PROCESS_TASK);
}

bool RSParallelPackVisitor::IsSkipProcessing(RSSurfaceRenderNode& node) const
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_OPTIONAL_TRACE_NAME("SecurityLayer Skip");
        return true;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 " invisible", node.GetId());
        return true;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_
        && RSSystemProperties::GetOcclusionEnabled() && !isSecurityDisplay_) {
        RS_OPTIONAL_TRACE_NAME("Occlusion Skip");
        return true;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
            RS_OPTIONAL_TRACE_NAME("QuickReject Skip");
            RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode skip: %{public}s", node.GetName().c_str());
            return true;
        }
    }
#endif
    return false;
}

void RSParallelPackVisitor::CalcSurfaceRenderNodeCost(RSSurfaceRenderNode& node) const
{
    if (IsSkipProcessing(node)) {
        return;
    }
    RSParallelRenderManager::Instance()->PackRenderTask(node, TaskType::CALC_COST_TASK);
}

void RSParallelPackVisitor::CalcDisplayRenderNodeCost(RSDisplayRenderNode& node) const
{
    for (auto& child : *node.GetChildren()) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (surface != nullptr) {
            CalcSurfaceRenderNodeCost(*surface);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
