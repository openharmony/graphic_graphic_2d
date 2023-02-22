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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "common/rs_vector4.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

class SkMatrix;
struct SkRect;

namespace OHOS {
namespace Rosen {

enum RSSurfaceNodeCommandType : uint16_t {
    SURFACE_NODE_CREATE,
    SURFACE_NODE_SET_CONTEXT_MATRIX,
    SURFACE_NODE_SET_CONTEXT_ALPHA,
    SURFACE_NODE_SET_CONTEXT_CLIP_REGION,
    SURFACE_NODE_SET_SECURITY_LAYER,
    SURFACE_NODE_SET_COLOR_SPACE,
    SURFACE_NODE_UPDATE_SURFACE_SIZE,
    SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE,
    SURFACE_NODE_SET_CALLBACK_FOR_RENDER_THREAD,
    SURFACE_NODE_SET_CONTEXT_BOUNDS,
    SURFACE_NODE_SET_ABILITY_BG_ALPHA,
    SURFACE_NODE_UPDATE_PARENT_WITHOUT_TRANSITION,
    SURFACE_NODE_SET_IS_NOTIFY_BUFFER_AVAILABLE,
    SURFACE_NODE_SET_APP_FREEZE,
    SURFACE_NODE_SET_SURFACE_NODE_TYPE,
    SURFACE_NODE_SET_CONTAINER_WINDOW,
    SURFACE_NODE_SET_ANIMATION_FINISHED
};

class RSB_EXPORT SurfaceNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId nodeId);
    static void SetContextMatrix(RSContext& context, NodeId nodeId, SkMatrix matrix);
    static void SetContextAlpha(RSContext& context, NodeId nodeId, float alpha);
    static void SetContextClipRegion(RSContext& context, NodeId nodeId, SkRect clipRect);
    static void SetSecurityLayer(RSContext& context, NodeId nodeId, bool isSecurityLayer);
#ifndef ROSEN_CROSS_PLATFORM
    static void SetColorSpace(RSContext& context, NodeId nodeId, ColorGamut colorSpace);
#endif
    static void UpdateSurfaceDefaultSize(RSContext& context, NodeId nodeId, float width, float height);
    static void ConnectToNodeInRenderService(RSContext& context, NodeId id);
    static void SetCallbackForRenderThreadRefresh(RSContext& context, NodeId id, std::function<void(void)> callback);
    static void SetContextBounds(RSContext& context, NodeId id, Vector4f bounds);
    static void SetAbilityBGAlpha(RSContext& context, NodeId id, uint8_t alpha);
    static void UpdateParentWithoutTransition(RSContext& context, NodeId nodeId, NodeId parentId);
    static void SetIsNotifyUIBufferAvailable(RSContext& context, NodeId nodeId, bool available);
    static void SetAppFreeze(RSContext& context, NodeId nodeId, bool isAppFreeze);
    static void SetSurfaceNodeType(RSContext& context, NodeId nodeId, RSSurfaceNodeType type);
    static void SetContainerWindow(RSContext& context, NodeId nodeId, bool hasContainerWindow, float density);
    static void SetAnimationFinished(RSContext& context, NodeId nodeId);
};

ADD_COMMAND(RSSurfaceNodeCreate, ARG(SURFACE_NODE, SURFACE_NODE_CREATE, SurfaceNodeCommandHelper::Create, NodeId))
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
ADD_COMMAND(RSSurfaceNodeSetBounds,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CONTEXT_BOUNDS, SurfaceNodeCommandHelper::SetContextBounds, NodeId, Vector4f))
ADD_COMMAND(RSSurfaceNodeSetAbilityBGAlpha,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_ABILITY_BG_ALPHA, SurfaceNodeCommandHelper::SetAbilityBGAlpha, NodeId, uint8_t))
ADD_COMMAND(RSSurfaceNodeUpdateParentWithoutTransition,
    ARG(SURFACE_NODE, SURFACE_NODE_UPDATE_PARENT_WITHOUT_TRANSITION,
        SurfaceNodeCommandHelper::UpdateParentWithoutTransition, NodeId, NodeId))
ADD_COMMAND(RSSurfaceNodeSetIsNotifyUIBufferAvailable,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_IS_NOTIFY_BUFFER_AVAILABLE,
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable, NodeId, bool))
ADD_COMMAND(RSSurfaceNodeSetAppFreeze,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_APP_FREEZE, SurfaceNodeCommandHelper::SetAppFreeze, NodeId, bool))
ADD_COMMAND(RSSurfaceNodeSetSurfaceNodeType,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_SURFACE_NODE_TYPE,
    SurfaceNodeCommandHelper::SetSurfaceNodeType, NodeId, RSSurfaceNodeType))
ADD_COMMAND(RSSurfaceNodeSetContainerWindow,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_CONTAINER_WINDOW, SurfaceNodeCommandHelper::SetContainerWindow,
    NodeId, bool, float))
ADD_COMMAND(RSSurfaceNodeSetAnimationFinished,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_ANIMATION_FINISHED, SurfaceNodeCommandHelper::SetAnimationFinished, NodeId))

#ifndef ROSEN_CROSS_PLATFORM
ADD_COMMAND(RSSurfaceNodeSetColorSpace,
    ARG(SURFACE_NODE, SURFACE_NODE_SET_COLOR_SPACE, SurfaceNodeCommandHelper::SetColorSpace, NodeId, ColorGamut))
#endif
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_SURFACE_NODE_COMMAND_H
