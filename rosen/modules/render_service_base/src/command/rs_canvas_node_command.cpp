/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_canvas_node_command.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"

#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

void RSCanvasNodeCommandHelper::Create(RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = RSRenderNodeAllocator::Instance().CreateRSCanvasRenderNode(id,
        context.weak_from_this(), isTextureExportNode);
    if (context.GetMutableNodeMap().UnRegisterUnTreeNode(id)) {
        HILOG_COMM_ERROR("RSCanvasNodeCommandHelper::Create after add, id:%{public}" PRIu64 " ", id);
        RS_TRACE_NAME_FMT("RSCanvasNodeCommandHelper::Create after add, id:%" PRIu64 " ", id);
    }
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

bool RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(
    std::shared_ptr<RSCanvasRenderNode> node, std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType)
{
    if (node->GetNodeIsSingleFrameComposer()) {
        node->UpdateRecordingNG(drawCmds, static_cast<ModifierNG::RSModifierType>(modifierType),
            RSSingleFrameComposer::IsShouldSingleFrameComposer());
    } else {
        if (RSSingleFrameComposer::IsShouldSingleFrameComposer()) {
            return true;
        }
        node->UpdateRecordingNG(drawCmds, static_cast<ModifierNG::RSModifierType>(modifierType));
    }
    return false;
}

void RSCanvasNodeCommandHelper::UpdateRecording(
    RSContext& context, NodeId id, std::shared_ptr<Drawing::DrawCmdList>& srcDrawCmds, uint16_t modifierType)
{
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    if (node == nullptr) {
        return;
    }
    auto drawCmds = std::move(srcDrawCmds);
    if (RSSystemProperties::GetSingleFrameComposerEnabled()) {
        if (AddCmdToSingleFrameComposer(node, drawCmds, modifierType)) {
            return;
        }
    } else {
        node->UpdateRecordingNG(drawCmds, static_cast<ModifierNG::RSModifierType>(modifierType));
    }
    if (!drawCmds) {
        return;
    }
    drawCmds->UpdateNodeIdToPicture(id);
}

void RSCanvasNodeCommandHelper::ClearRecording(RSContext& context, NodeId id)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id)) {
        node->ClearRecording();
    }
}

void RSCanvasNodeCommandHelper::SetHDRPresent(RSContext& context, NodeId nodeId, bool hdrPresent)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(nodeId)) {
        node->SetHDRPresent(hdrPresent);
    }
}

void RSCanvasNodeCommandHelper::SetColorGamut(RSContext& context, NodeId nodeId, uint32_t colorGamut)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(nodeId)) {
        node->SetColorGamut(colorGamut);
    }
}

} // namespace Rosen
} // namespace OHOS
