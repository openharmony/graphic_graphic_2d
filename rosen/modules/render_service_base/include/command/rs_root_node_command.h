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

#ifndef ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSRootNodeCommandType : uint16_t {
    ROOT_NODE_CREATE = 0,
    ROOT_NODE_ATTACH = 1,
    ATTACH_TO_UNI_SURFACENODE = 2,
    SET_ENABLE_RENDER = 3,
    UPDATE_SUGGESTED_BUFFER_SIZE = 4,
    SET_OCCLUSION_SCENE_ENABLE = 5,
};

class RSB_EXPORT RootNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isTextureExportNode = false);
    static void AttachRSSurfaceNode(RSContext& context, NodeId id, NodeId surfaceNodeId, uint64_t token = 0);
    static void AttachToUniSurfaceNode(RSContext& context, NodeId id, NodeId surfaceNodeId);
    static void SetEnableRender(RSContext& context, NodeId id, bool flag);
    static void UpdateSuggestedBufferSize(RSContext& context, NodeId id, float width, float height);
    static void UpdateOcclusionCullingStatus(RSContext& context, NodeId nodeId, bool enable, NodeId keyOcclusionNodeId);
};

ADD_COMMAND(RSRootNodeCreate,
    ARG(PERMISSION_APP, ROOT_NODE, ROOT_NODE_CREATE,
        RootNodeCommandHelper::Create, NodeId, bool))
ADD_COMMAND(RSRootNodeAttachRSSurfaceNode,
    ARG(PERMISSION_APP, ROOT_NODE, ROOT_NODE_ATTACH,
        RootNodeCommandHelper::AttachRSSurfaceNode, NodeId, NodeId, uint64_t))
ADD_COMMAND(RSRootNodeSetEnableRender,
    ARG(PERMISSION_APP, ROOT_NODE, SET_ENABLE_RENDER,
        RootNodeCommandHelper::SetEnableRender, NodeId, bool))
// unirender
ADD_COMMAND(RSRootNodeAttachToUniSurfaceNode,
    ARG(PERMISSION_APP, ROOT_NODE, ATTACH_TO_UNI_SURFACENODE,
        RootNodeCommandHelper::AttachToUniSurfaceNode, NodeId, NodeId))
ADD_COMMAND(RSRootNodeUpdateSuggestedBufferSize,
    ARG(PERMISSION_APP, ROOT_NODE, UPDATE_SUGGESTED_BUFFER_SIZE,
        RootNodeCommandHelper::UpdateSuggestedBufferSize, NodeId, float, float))
ADD_COMMAND(RSUpdateOcclusionCullingStatus,
    ARG(PERMISSION_APP, ROOT_NODE, SET_OCCLUSION_SCENE_ENABLE,
        RootNodeCommandHelper::UpdateOcclusionCullingStatus, NodeId, bool, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_ROOT_COMMAND_RS_ROOT_NODE_COMMAND_H
