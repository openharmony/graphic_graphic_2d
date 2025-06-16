/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H


#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSCanvasNodeCommandType : uint16_t {
    CANVAS_NODE_CREATE = 0,
    CANVAS_NODE_UPDATE_RECORDING = 1,
    CANVAS_NODE_CLEAR_RECORDING = 2,
    CANVAS_NODE_SET_HDR_PRESENT = 3,
    CANVAS_NODE_SET_LINKED_ROOTNODE = 4,
    CANVAS_NODE_SET_IS_WIDE_COLOR_GAMUT = 5,
};

namespace Drawing {
class DrawCmdList;
}

class RSB_EXPORT RSCanvasNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isTextureExportNode = false);
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType);
    static void ClearRecording(RSContext& context, NodeId id);
    static void SetHDRPresent(RSContext& context, NodeId nodeId, bool hdrPresent);
    static void SetIsWideColorGamut(RSContext& context, NodeId nodeId, bool isWideColorGamut);

    // [Attention] Only used in PC window resize scene now
    static void SetLinkedRootNodeId(RSContext& context, NodeId nodeId, NodeId rootNodeId);

private:
    static bool AddCmdToSingleFrameComposer(std::shared_ptr<RSCanvasRenderNode> node,
        std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType);
};

ADD_COMMAND(RSCanvasNodeCreate,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_CREATE,
        RSCanvasNodeCommandHelper::Create, NodeId, bool))
ADD_COMMAND(RSCanvasNodeUpdateRecording,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_UPDATE_RECORDING,
        RSCanvasNodeCommandHelper::UpdateRecording, NodeId, std::shared_ptr<Drawing::DrawCmdList>, uint16_t))
ADD_COMMAND(RSCanvasNodeClearRecording,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_CLEAR_RECORDING,
        RSCanvasNodeCommandHelper::ClearRecording, NodeId))
ADD_COMMAND(RSCanvasNodeSetHDRPresent,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_SET_HDR_PRESENT,
        RSCanvasNodeCommandHelper::SetHDRPresent, NodeId, bool))
ADD_COMMAND(RSCanvasNodeSetIsWideColorGamut,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_SET_IS_WIDE_COLOR_GAMUT,
        RSCanvasNodeCommandHelper::SetIsWideColorGamut, NodeId, bool))

// [Attention] Only used in PC window resize scene now
ADD_COMMAND(RSCanvasNodeSetLinkedRootNodeId,
    ARG(PERMISSION_APP, CANVAS_NODE, CANVAS_NODE_SET_LINKED_ROOTNODE,
        RSCanvasNodeCommandHelper::SetLinkedRootNodeId, NodeId, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H
