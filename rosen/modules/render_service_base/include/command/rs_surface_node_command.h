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
struct SkRect;

namespace OHOS {
namespace Rosen {

enum RSSurfaceNodeCommandType : uint16_t {
    SURFACE_NODE_CREATE,
    SURFACE_NODE_SET_PROXY,
    SURFACE_NODE_SET_CONTEXT_MATRIX,
    SURFACE_NODE_SET_CONTEXT_ALPHA,
    SURFACE_NODE_SET_CONTEXT_CLIP_REGION,
    SURFACE_NODE_SET_SECURITY_LAYER,
    SURFACE_NODE_UPDATE_SURFACE_SIZE,
    SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE,
    SURFACE_NODE_SET_CALLBACK_FOR_RENDER_THREAD,
};

class SurfaceNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId nodeId);
    static void SetProxy(RSContext& context, NodeId nodeId);
    static void SetContextMatrix(RSContext& context, NodeId nodeId, SkMatrix matrix);
    static void SetContextAlpha(RSContext& context, NodeId nodeId, float alpha);
    static void SetContextClipRegion(RSContext& context, NodeId nodeId, SkRect clipRect);
    static void SetSecurityLayer(RSContext& context, NodeId nodeId, bool isSecurityLayer);
    static void UpdateSurfaceDefaultSize(RSContext& context, NodeId nodeId, float width, float height);
    static void ConnectToNodeInRenderService(RSContext& context, NodeId id);
    static void SetCallbackForRenderThreadRefresh(RSContext& context, NodeId id, std::function<void(void)> callback);
};

ADD_COMMAND(RSSurfaceNodeCreate, ARG(SURFACE_NODE, SURFACE_NODE_CREATE, SurfaceNodeCommandHelper::Create, NodeId))
ADD_COMMAND(
    RSSurfaceNodeSetProxy, ARG(SURFACE_NODE, SURFACE_NODE_SET_PROXY, SurfaceNodeCommandHelper::SetProxy, NodeId))
ADD_COMMAND(RSSurfaceNodeSetContextMatrix,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CONTEXT_MATRIX, SurfaceNodeCommandHelper::SetContextMatrix, NodeId, SkMatrix))
ADD_COMMAND(RSSurfaceNodeSetContextAlpha,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CONTEXT_ALPHA, SurfaceNodeCommandHelper::SetContextAlpha, NodeId, float))
ADD_COMMAND(RSSurfaceNodeSetContextClipRegion, ARG(SURFACE_NODE, SURFACE_NODE_SET_CONTEXT_CLIP_REGION,
    SurfaceNodeCommandHelper::SetContextClipRegion, NodeId, SkRect))
ADD_COMMAND(RSSurfaceNodeSetSecurityLayer,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_SECURITY_LAYER, SurfaceNodeCommandHelper::SetSecurityLayer, NodeId, bool))
ADD_COMMAND(RSSurfaceNodeUpdateSurfaceDefaultSize, ARG(SURFACE_NODE, SURFACE_NODE_UPDATE_SURFACE_SIZE,
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize, NodeId, float, float))
ADD_COMMAND(RSSurfaceNodeConnectToNodeInRenderService,
    ARG(SURFACE_NODE, SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE,
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService, NodeId))
ADD_COMMAND(RSSurfaceNodeSetCallbackForRenderThreadRefresh,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CALLBACK_FOR_RENDER_THREAD,
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh, NodeId, std::function<void(void)>))

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H
