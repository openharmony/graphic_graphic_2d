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

#include "animation/rs_spring_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_spring_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

#define START_SPRING_ANIMATION(RSRenderCommand, Type)                                                   \
    RSPropertyAnimation<Type>::OnStart();                                                               \
    auto target = GetTarget().lock();                                                                   \
    if (target == nullptr) {                                                                            \
        ROSEN_LOGE("Failed to start spring animation, target is null!");                                \
        return;                                                                                         \
    }                                                                                                   \
    auto animation = std::make_shared<RSRenderSpringAnimation<Type>>(GetId(), GetProperty(),            \
        RSPropertyAnimation<Type>::originValue_, RSPropertyAnimation<Type>::startValue_,                \
        RSPropertyAnimation<Type>::endValue_);                                                          \
    animation->SetDuration(GetDuration());                                                              \
    animation->SetStartDelay(GetStartDelay());                                                          \
    animation->SetRepeatCount(GetRepeatCount());                                                        \
    animation->SetAutoReverse(GetAutoReverse());                                                        \
    animation->SetSpeed(GetSpeed());                                                                    \
    animation->SetDirection(GetDirection());                                                            \
    animation->SetFillMode(GetFillMode());                                                              \
    animation->SetSpringParameters(timingCurve_.response_, timingCurve_.dampingRatio_);                 \
    std::unique_ptr<RSCommand> command = std::make_unique<RSRenderCommand>(target->GetId(), animation); \
    auto transactionProxy = RSTransactionProxy::GetInstance();                                          \
    if (transactionProxy != nullptr) {                                                                  \
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());                           \
        if (target->NeedForcedSendToRemote()) {                                                         \
            std::unique_ptr<RSCommand> commandForRemote =                                               \
                std::make_unique<RSRenderCommand>(target->GetId(), animation);                          \
            transactionProxy->AddCommand(commandForRemote, true);                                       \
        }                                                                                               \
    }

template<>
void RSSpringAnimation<float>::OnStart()
{
    START_SPRING_ANIMATION(RSAnimationCreateSpringFloat, float);
}

template<>
void RSSpringAnimation<Color>::OnStart()
{
    START_SPRING_ANIMATION(RSAnimationCreateSpringColor, Color);
}

template<>
void RSSpringAnimation<Vector2f>::OnStart()
{
    START_SPRING_ANIMATION(RSAnimationCreateSpringVec2f, Vector2f);
}

template<>
void RSSpringAnimation<Vector4f>::OnStart()
{
    START_SPRING_ANIMATION(RSAnimationCreateSpringVec4f, Vector4f);
}

} // namespace Rosen
} // namespace OHOS
