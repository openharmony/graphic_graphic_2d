/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "animation/rs_keyframe_animation.h"

#include "animation/rs_render_keyframe_animation.h"
#include "command/rs_animation_command.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

#define START_KEYFRAME_ANIMATION(Command, Type)                                                            \
    RSPropertyAnimation::OnStart();                                                                        \
    auto target = GetTarget().lock();                                                                      \
    if (target == nullptr) {                                                                               \
        ROSEN_LOGE("Failed to start keyframe animation, target is null!");                                 \
        return;                                                                                            \
    }                                                                                                      \
    if (keyframes_.empty()) {                                                                              \
        ROSEN_LOGE("Failed to start keyframe animation, keyframes is null!");                              \
        return;                                                                                            \
    }                                                                                                      \
    auto animation = std::make_shared<RSRenderKeyframeAnimation<Type>>(                                    \
        GetId(), GetProperty(), RSPropertyAnimation<Type>::originValue_);                                  \
    for (const auto& [fraction, value, curve] : keyframes_) {                                              \
        animation->AddKeyframe(fraction, value, curve.GetInterpolator(GetDuration()));                     \
    }                                                                                                      \
    animation->SetDuration(GetDuration());                                                                 \
    animation->SetStartDelay(GetStartDelay());                                                             \
    animation->SetRepeatCount(GetRepeatCount());                                                           \
    animation->SetAutoReverse(GetAutoReverse());                                                           \
    animation->SetSpeed(GetSpeed());                                                                       \
    animation->SetFillMode(GetFillMode());                                                                 \
    animation->SetDirection(GetDirection());                                                               \
    std::unique_ptr<RSCommand> command = std::make_unique<Command>(target->GetId(), animation);            \
    auto transactionProxy = RSTransactionProxy::GetInstance();                                             \
    if (transactionProxy != nullptr) {                                                                     \
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());                              \
        if (target->NeedForcedSendToRemote()) {                                                            \
            std::unique_ptr<RSCommand> commandForRemote =                                                  \
                std::make_unique<Command>(target->GetId(), animation);                                     \
            transactionProxy->AddCommand(commandForRemote, true);                                          \
        }                                                                                                  \
    }

template<>
void RSKeyframeAnimation<int>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeInt, int);
}

template<>
void RSKeyframeAnimation<float>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeFloat, float);
}

template<>
void RSKeyframeAnimation<Color>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeColor, Color);
}

template<>
void RSKeyframeAnimation<Matrix3f>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeMatrix3f, Matrix3f);
}

template<>
void RSKeyframeAnimation<Vector2f>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeVec2f, Vector2f);
}

template<>
void RSKeyframeAnimation<Vector4f>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeVec4f, Vector4f);
}

template<>
void RSKeyframeAnimation<Quaternion>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeQuaternion, Quaternion);
}

template<>
void RSKeyframeAnimation<std::shared_ptr<RSFilter>>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeFilter, std::shared_ptr<RSFilter>);
}

template<>
void RSKeyframeAnimation<Vector4<Color>>::OnStart()
{
    START_KEYFRAME_ANIMATION(RSAnimationCreateKeyframeVec4Color, Vector4<Color>);
}
} // namespace Rosen
} // namespace OHOS
