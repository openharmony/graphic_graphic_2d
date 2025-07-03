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

#ifndef SAFUZZ_RS_NODE_COMMAND_UTILS_H
#define SAFUZZ_RS_NODE_COMMAND_UTILS_H

#include "command/rs_node_command.h"

#include "command/rs_command_utils.h"

namespace OHOS {
namespace Rosen {
class RSNodeCommandUtils {
public:
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAddModifier, Uint64, RSRenderModifierSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSRemoveModifier, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyBool, Uint64, Bool, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyFloat, Uint64, Float, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyInt, Uint64, Int, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyColor, Uint64, Color, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyGravity, Uint64, Gravity, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyMatrix3f, Uint64, Matrix3f, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyQuaternion, Uint64, Quaternion, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyImage, Uint64, RSImageSharedPtr, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyMask, Uint64, RSMaskSharedPtr, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyPath, Uint64, RSPathSharedPtr, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyDynamicBrightness, Uint64, RSDynamicBrightnessPara, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyWaterRipple, Uint64, RSWaterRipplePara, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyFlyOut, Uint64, RSFlyOutPara, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyLinearGradientBlurPara, Uint64, RSLinearGradientBlurParaSharedPtr,
                                    Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyMotionBlurPara, Uint64, MotionBlurParamSharedPtr, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyMagnifierPara, Uint64, RSMagnifierParamsSharedPtr, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyEmitterUpdater, Uint64, EmitterUpdaterSharedPtrVector, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyParticleNoiseFields, Uint64, ParticleNoiseFieldsSharedPtr, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyShader, Uint64, RSShaderSharedPtr, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyVector2f, Uint64, Vector2f, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyVector3f, Uint64, Vector3f, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyBorderStyle, Uint64, Uint32Vector4, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyVector4Color, Uint64, ColorVector4, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyVector4f, Uint64, Vector4f, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyRRect, Uint64, RRect, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyDrawCmdList, Uint64, DrawingDrawCmdListPtr, Uint64,
                                    PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSUpdatePropertyDrawingMatrix, Uint64, DrawingMatrix, Uint64, PropertyUpdateType);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetFreeze, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetNodeName, Uint64, String);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSMarkNodeGroup, Uint64, Bool, Bool, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSMarkNodeSingleFrameComposer, Uint64, Bool, Pid);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSMarkSuggestOpincNode, Uint64, Bool, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSMarkUifirstNode, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSForceUifirstNode, Uint64, Bool, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetUIFirstSwitch, Uint64, RSUIFirstSwitch);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetDrawRegion, Uint64, RectFSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetOutOfParent, Uint64, OutOfParentType);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSetNeedUseCmdlistDrawRegion, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSRegisterGeometryTransitionNodePair, Uint64, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSUnregisterGeometryTransitionNodePair, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSRemoveAllModifiers, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSDumpClientNodeTree, Uint64, Pid, Uint32);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSCommitDumpClientNodeTree, Uint64, Pid, Uint32, String);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_NODE_COMMAND_UTILS_H
