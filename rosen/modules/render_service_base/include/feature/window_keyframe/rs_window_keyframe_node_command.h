/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_KEYFRAME_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_KEYFRAME_NODE_COMMAND_H


#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSWindowKeyFrameNodeCommandType : uint16_t {
    WINDOW_KEYFRAME_NODE_CREATE,
    WINDOW_KEYFRAME_NODE_LINK_NODE,
};

class RSB_EXPORT RSWindowKeyFrameNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isTextureExportNode);
    static void LinkNode(RSContext& context, NodeId nodeId, NodeId linkNodeId);
};

ADD_COMMAND(RSWindowKeyFrameNodeCreate,
    ARG(PERMISSION_APP, WINDOW_KEYFRAME_NODE, WINDOW_KEYFRAME_NODE_CREATE,
        RSWindowKeyFrameNodeCommandHelper::Create, NodeId, bool))
ADD_COMMAND(RSWindowKeyFrameNodeLinkNode,
    ARG(PERMISSION_APP, WINDOW_KEYFRAME_NODE, WINDOW_KEYFRAME_NODE_LINK_NODE,
        RSWindowKeyFrameNodeCommandHelper::LinkNode, NodeId, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_KEYFRAME_NODE_COMMAND_H
