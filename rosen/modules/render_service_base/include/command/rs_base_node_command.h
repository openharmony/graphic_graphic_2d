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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_BASE_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_BASE_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSBaseNodeCommandType : uint16_t {
    BASE_NODE_DESTROY,
    BASE_NODE_ADD_CHILD,
    BASE_NODE_MOVE_CHILD,
    BASE_NODE_ADD_CROSS_PARENT_CHILD,
    BASE_NODE_REMOVE_CHILD,
    BASE_NODE_REMOVE_CROSS_PARENT_CHILD,
    BASE_NODE_REMOVE_FROM_TREE,
    BASE_NODE_CLEAR_CHILDREN,
    BASE_NODE_CLEAR_SURFACE_NODE_CHILDREN,
};

class RSB_EXPORT BaseNodeCommandHelper {
public:
    static void Destroy(RSContext& context, NodeId nodeId);
    static void AddChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index);
    static void MoveChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index);
    static void RemoveChild(RSContext& context, NodeId nodeId, NodeId childNodeId);
    static void AddCrossParentChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index);
    static void RemoveCrossParentChild(RSContext& context, NodeId nodeId, NodeId childNodeId, NodeId newParentId);
    static void RemoveFromTree(RSContext& context, NodeId nodeId);
    static void ClearChildren(RSContext& context, NodeId nodeId);
    static void ClearSurfaceNodeChildren(RSContext& context, NodeId nodeId);
};

ADD_COMMAND(RSBaseNodeDestroy, ARG(BASE_NODE, BASE_NODE_DESTROY, BaseNodeCommandHelper::Destroy, NodeId))
ADD_COMMAND(
    RSBaseNodeAddChild, ARG(BASE_NODE, BASE_NODE_ADD_CHILD, BaseNodeCommandHelper::AddChild, NodeId, NodeId, int32_t))
ADD_COMMAND(RSBaseNodeMoveChild,
    ARG(BASE_NODE, BASE_NODE_MOVE_CHILD, BaseNodeCommandHelper::MoveChild, NodeId, NodeId, int32_t))
ADD_COMMAND(
    RSBaseNodeRemoveChild, ARG(BASE_NODE, BASE_NODE_REMOVE_CHILD, BaseNodeCommandHelper::RemoveChild, NodeId, NodeId))
ADD_COMMAND(
    RSBaseNodeAddCrossParentChild, ARG(BASE_NODE, BASE_NODE_ADD_CROSS_PARENT_CHILD,
    BaseNodeCommandHelper::AddCrossParentChild, NodeId, NodeId, int32_t))
ADD_COMMAND(
    RSBaseNodeRemoveCrossParentChild, ARG(BASE_NODE, BASE_NODE_REMOVE_CROSS_PARENT_CHILD,
    BaseNodeCommandHelper::RemoveCrossParentChild, NodeId, NodeId, NodeId))
ADD_COMMAND(
    RSBaseNodeRemoveFromTree, ARG(BASE_NODE, BASE_NODE_REMOVE_FROM_TREE, BaseNodeCommandHelper::RemoveFromTree, NodeId))
ADD_COMMAND(
    RSBaseNodeClearChild, ARG(BASE_NODE, BASE_NODE_CLEAR_CHILDREN, BaseNodeCommandHelper::ClearChildren, NodeId))
ADD_COMMAND(RSBaseNodeClearSurfaceNodeChild,
    ARG(BASE_NODE, BASE_NODE_CLEAR_SURFACE_NODE_CHILDREN, BaseNodeCommandHelper::ClearSurfaceNodeChildren, NodeId))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_BASE_NODE_COMMAND_H
