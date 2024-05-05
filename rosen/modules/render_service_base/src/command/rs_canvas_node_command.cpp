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
#include "pipeline/rs_render_node_gc.h"

namespace OHOS {
namespace Rosen {

void RSCanvasNodeCommandHelper::Create(RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = std::shared_ptr<RSCanvasRenderNode>(new RSCanvasRenderNode(id,
        context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

bool RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(std::shared_ptr<RSCanvasRenderNode> node,
    std::shared_ptr<Drawing::DrawCmdList> drawCmds, RSModifierType type)
{
    if (node->GetNodeIsSingleFrameComposer()) {
        if (RSSingleFrameComposer::IsShouldSingleFrameComposer()) {
            node->UpdateRecording(drawCmds, type, true);
        } else {
            node->UpdateRecording(drawCmds, type);
        }
    } else {
        if (RSSingleFrameComposer::IsShouldSingleFrameComposer()) {
            return true;
        } else {
            node->UpdateRecording(drawCmds, type);
        }
    }
    return false;
}

void RSCanvasNodeCommandHelper::UpdateRecording(
    RSContext& context, NodeId id, std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType)
{
    auto type = static_cast<RSModifierType>(modifierType);
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id)) {
        if (RSSystemProperties::GetSingleFrameComposerEnabled()) {
            if (AddCmdToSingleFrameComposer(node, drawCmds, type)) {
                return;
            }
        } else {
            node->UpdateRecording(drawCmds, type);
        }
        if (!drawCmds) {
            return;
        }
        drawCmds->UpdateNodeIdToPicture(id);
    }
}

void RSCanvasNodeCommandHelper::ClearRecording(RSContext& context, NodeId id)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id)) {
        node->ClearRecording();
    }
}

} // namespace Rosen
} // namespace OHOS
