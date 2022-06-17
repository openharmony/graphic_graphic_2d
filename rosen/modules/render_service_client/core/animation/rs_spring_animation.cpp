/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
template<typename T>
template<typename P>
void RSSpringAnimation<T>::StartAnimationImpl()
{
    RSPropertyAnimation<T>::OnStart();
    auto target = RSPropertyAnimation<T>::GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start spring animation, target is null!");
        return;
    }
    auto animation = std::make_shared<RSRenderSpringAnimation<T>>(RSPropertyAnimation<T>::GetId(),
        RSPropertyAnimation<T>::GetProperty(), RSPropertyAnimation<T>::originValue_,
        RSPropertyAnimation<T>::startValue_, RSPropertyAnimation<T>::endValue_);
    animation->SetDuration(RSPropertyAnimation<T>::GetDuration());
    animation->SetStartDelay(RSPropertyAnimation<T>::GetStartDelay());
    animation->SetRepeatCount(RSPropertyAnimation<T>::GetRepeatCount());
    animation->SetAutoReverse(RSPropertyAnimation<T>::GetAutoReverse());
    animation->SetSpeed(RSPropertyAnimation<T>::GetSpeed());
    animation->SetDirection(RSPropertyAnimation<T>::GetDirection());
    animation->SetFillMode(RSPropertyAnimation<T>::GetFillMode());
    animation->SetSpringParameters(timingCurve_.response_, timingCurve_.dampingRatio_);
    std::unique_ptr<RSCommand> command = std::make_unique<P>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(
            command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<P>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
    }
}

template<>
void RSSpringAnimation<float>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringFloat>();
}

template<>
void RSSpringAnimation<Color>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringColor>();
}

template<>
void RSSpringAnimation<Matrix3f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringMatrix3f>();
}

template<>
void RSSpringAnimation<Vector2f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringVec2f>();
}

template<>
void RSSpringAnimation<Vector4f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringVec4f>();
}

template<>
void RSSpringAnimation<Quaternion>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringQuaternion>();
}

template<>
void RSSpringAnimation<std::shared_ptr<RSFilter>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringFilter>();
}

template<>
void RSSpringAnimation<Vector4<Color>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateSpringVec4Color>();
}
} // namespace Rosen
} // namespace OHOS
