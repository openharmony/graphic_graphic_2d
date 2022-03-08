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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_vector4.h"

class SkMatrix;

namespace OHOS {
namespace Rosen {

enum RSSurfaceNodeCommandType : uint16_t {
    SURFACE_NODE_CREATE,
    SURFACE_NODE_SET_MATRIX,
    SURFACE_NODE_SET_ALPHA,
    SURFACE_NODE_SET_CLIP_REGION,
    SURFACE_NODE_SET_PARENT_SURFACE,
    SURFACE_NODE_REMOVE_SELF,
    SURFACE_NODE_UPDATE_SURFACE_SIZE,
    SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE,
};

class SurfaceNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId nodeId);
    static void SetMatrix(RSContext& context, NodeId nodeId, SkMatrix matrix);
    static void SetAlpha(RSContext& context, NodeId nodeId, float alpha);
    static void SetParentSurface(RSContext& context, NodeId nodeId, NodeId parentId);
    static void SetClipRegion(RSContext& context, NodeId nodeId, Vector4f clipRect);
    static void RemoveSelf(RSContext& context, NodeId nodeId);
    static void UpdateSurfaceDefaultSize(RSContext& context, NodeId nodeId, float width, float height);
    static void ConnectToNodeInRenderService(RSContext& context, NodeId id);
};

ADD_COMMAND(RSSurfaceNodeCreate, ARG(SURFACE_NODE, SURFACE_NODE_CREATE, SurfaceNodeCommandHelper::Create, NodeId))
ADD_COMMAND(
    RSSurfaceNodeSetMatrix, ARG(SURFACE_NODE, SURFACE_NODE_SET_MATRIX, SurfaceNodeCommandHelper::SetMatrix, NodeId, SkMatrix))
ADD_COMMAND(
    RSSurfaceNodeSetAlpha, ARG(SURFACE_NODE, SURFACE_NODE_SET_ALPHA, SurfaceNodeCommandHelper::SetAlpha, NodeId, float))
ADD_COMMAND(RSSurfaceNodeSetClipRegion,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CLIP_REGION, SurfaceNodeCommandHelper::SetClipRegion, NodeId, Vector4f))
ADD_COMMAND(RSSurfaceNodeSetParentSurface,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_PARENT_SURFACE, SurfaceNodeCommandHelper::SetParentSurface, NodeId, NodeId))
ADD_COMMAND(RSSurfaceNodeRemoveSelf,
    ARG(SURFACE_NODE, SURFACE_NODE_REMOVE_SELF, SurfaceNodeCommandHelper::RemoveSelf, NodeId))
ADD_COMMAND(RSSurfaceNodeUpdateSurfaceDefaultSize, ARG(SURFACE_NODE, SURFACE_NODE_UPDATE_SURFACE_SIZE,
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize, NodeId, float, float))
ADD_COMMAND(RSSurfaceNodeConnectToNodeInRenderService,
    ARG(SURFACE_NODE, SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE,
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H
