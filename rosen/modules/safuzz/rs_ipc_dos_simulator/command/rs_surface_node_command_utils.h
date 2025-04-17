/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_RS_SURFACE_NODE_COMMAND_UTILS_H
#define SAFUZZ_RS_SURFACE_NODE_COMMAND_UTILS_H

#include "command/rs_surface_node_command.h"

#include "command/rs_command_utils.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceNodeCommandUtils {
public:
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSSurfaceNodeCreate, Uint64, RSSurfaceNodeType, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSSurfaceNodeCreateWithConfig, Uint64, String, Uint8, SurfaceWindowType);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetContextMatrix, Uint64, OptionalDrawingMatrix);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetContextAlpha, Uint64, Float);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetContextClipRegion, Uint64, OptionalDrawingRect);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetHardwareAndFixRotation, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetBootAnimation, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetGlobalPositionEnabled, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetSecurityLayer, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetSkipLayer, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetSnapshotSkipLayer, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetFingerprint, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSSurfaceNodeUpdateSurfaceDefaultSize, Uint64, Float, Float);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSSurfaceNodeConnectToNodeInRenderService, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetCallbackForRenderThreadRefresh, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetBounds, Uint64, Vector4f);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetAbilityBGAlpha, Uint64, Uint8);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetIsNotifyUIBufferAvailable, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeMarkUIHidden, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetIsTextureExportNode, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetSurfaceNodeType, Uint64, Uint8);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSSurfaceNodeSetContainerWindow, Uint64, Bool, RRect);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSSurfaceNodeSetAnimationFinished, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeAttachToDisplay, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeDetachToDisplay, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSurfaceNodeSetSurfaceId, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSurfaceNodeSetLeashPersistentId, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetColorSpace, Uint64, GraphicColorGamut);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetForeground, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetForceUIFirst, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetAncoFlags, Uint64, Uint32);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetHDRPresent, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetSkipDraw, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSSurfaceNodeSetWatermarkEnabled, Uint64, String, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetAbilityState, Uint64, RSSurfaceNodeAbilityState);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetApiCompatibleVersion, Uint64, Uint32);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetHardwareEnableHint, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeAttachToWindowContainer, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeDetachFromWindowContainer, Uint64, Uint64);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_SURFACE_NODE_COMMAND_UTILS_H
