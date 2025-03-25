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

#ifndef SAFUZZ_RS_ANIMATION_COMMAND_UTILS_H
#define SAFUZZ_RS_ANIMATION_COMMAND_UTILS_H

#include "command/rs_animation_command.h"

#include "command/rs_command_utils.h"

namespace OHOS {
namespace Rosen {
class RSAnimationCommandUtils {
public:
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationStart, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationPause, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationResume, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationFinish, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSAnimationReverse, Uint64, Uint64, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSAnimationSetFraction, Uint64, Uint64, Float);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCancel, Uint64, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_4(RSAnimationCallback, Uint64, Uint64, Uint64, AnimationCallbackEvent);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateCurve, Uint64, RSRenderCurveAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateParticle, Uint64, RSRenderParticleAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateKeyframe, Uint64, RSRenderKeyframeAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreatePath, Uint64, RSRenderPathAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateTransition, Uint64, RSRenderTransitionSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateSpring, Uint64, RSRenderSpringAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSAnimationCreateInterpolatingSpring, Uint64,
                                    RSRenderInterpolatingSpringAnimationSharedPtr);
    ADD_RANDOM_COMMAND_WITH_PARAM_3(RSInteractiveAnimatorCreate, Uint64, Uint64AndUint64PairVector, Bool);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSInteractiveAnimatorDestory, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSInteractiveAnimatorPause, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSInteractiveAnimatorContinue, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSInteractiveAnimatorFinish, Uint64, RSInteractiveAnimationPosition);
    ADD_RANDOM_COMMAND_WITH_PARAM_1(RSInteractiveAnimatorReverse, Uint64);
    ADD_RANDOM_COMMAND_WITH_PARAM_2(RSInteractiveAnimatorSetFraction, Uint64, Float);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_ANIMATION_COMMAND_UTILS_H
