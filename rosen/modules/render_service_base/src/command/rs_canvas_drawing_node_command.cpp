/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "command/rs_canvas_drawing_node_command.h"

#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node_gc.h"

namespace OHOS {
namespace Rosen {

void RSCanvasDrawingNodeCommandHelper::Create(RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = std::shared_ptr<RSCanvasDrawingRenderNode>(new RSCanvasDrawingRenderNode(id,
        context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void RSCanvasDrawingNodeCommandHelper::ResetSurface(RSContext& context, NodeId id, int width, int height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id)) {
        node->ResetSurface(width, height);
    }
}

} // namespace Rosen
} // namespace OHOS