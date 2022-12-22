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

namespace OHOS {
namespace Rosen {
RSParallelPackVisitor::RSParallelPackVisitor(RSUniRenderVisitor &visitor)
{
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenNum = screenManager->GetAllScreenIds().size();
    isPartialRenderEnabled_ = (screenNum <= 1) && (partialRenderType_ != PartialRenderType::DISABLED);
    isOpDropped_ = isPartialRenderEnabled_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE);
    doAnimate_ = visitor.GetAnimateState();
}

void RSParallelPackVisitor::PrepareBaseRenderNode(RSBaseRenderNode &node)
{
    node.ResetSortedChildren();
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSParallelPackVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode &node)
{
    PrepareBaseRenderNode(node);
}

void RSParallelPackVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RSParallelRenderManager::Instance()->PackRenderTask(node, TaskType::PREPARE_TASK);
}

void RSParallelPackVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto &child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSParallelPackVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    ProcessBaseRenderNode(node);
}

void RSParallelPackVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSUniRender::Process:[" + node.GetName() + "]" + node.GetDstRect().ToString());
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 ", child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());
    node.UpdatePositionZ();
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_TRACE_NAME("SecurityLayer Skip");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        RS_TRACE_NAME("Occlusion Skip");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
            RS_TRACE_NAME("QuickReject Skip");
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
            return;
        }
    }
#endif
    RSParallelRenderManager::Instance()->PackRenderTask(node, TaskType::PROCESS_TASK);
}
} // namespace Rosen
} // namespace OHOS
