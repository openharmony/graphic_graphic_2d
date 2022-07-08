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
template<typename T>
template<typename P>
void RSKeyframeAnimation<T>::StartAnimationImpl()
{
    RSPropertyAnimation<T>::OnStart();
    auto target = RSPropertyAnimation<T>::GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start keyframe animation, target is null!");
        return;
    }
    if (keyframes_.empty()) {
        ROSEN_LOGE("Failed to start keyframe animation, keyframes is null!");
        return;
    }
    auto animation = std::make_shared<RSRenderKeyframeAnimation<T>>(RSPropertyAnimation<T>::GetId(),
        RSPropertyAnimation<T>::GetPropertyId(), RSPropertyAnimation<T>::originValue_);
    for (const auto& [fraction, value, curve] : keyframes_) {
        animation->AddKeyframe(fraction, value, curve.GetInterpolator(RSPropertyAnimation<T>::GetDuration()));
    }
    animation->SetAdditive(RSPropertyAnimation<T>::GetAdditive());
    RSAnimation::UpdateParamToRenderAnimation(animation);
    std::unique_ptr<RSCommand> command = std::make_unique<P>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<P>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
    }
}

template<>
void RSKeyframeAnimation<float>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeFloat>();
}

template<>
void RSKeyframeAnimation<Color>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeColor>();
}

template<>
void RSKeyframeAnimation<Matrix3f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeMatrix3f>();
}

template<>
void RSKeyframeAnimation<Vector2f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeVec2f>();
}

template<>
void RSKeyframeAnimation<Vector4f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeVec4f>();
}

template<>
void RSKeyframeAnimation<Quaternion>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeQuaternion>();
}

template<>
void RSKeyframeAnimation<std::shared_ptr<RSFilter>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeFilter>();
}

template<>
void RSKeyframeAnimation<Vector4<Color>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateKeyframeVec4Color>();
}

template<>
void RSKeyframeAnimation<std::shared_ptr<RSAnimatableBase>>::OnStart()
{
    RSPropertyAnimation::OnStart();
    if (keyframes_.empty()) {
        ROSEN_LOGE("Failed to start keyframe animation, keyframes is null!");
        return;
    }
    auto animation = std::make_shared<RSRenderKeyframeAnimation<std::shared_ptr<RSAnimatableBase>>>(
        GetId(), GetPropertyId(), originValue_);
    for (const auto& [fraction, value, curve] : keyframes_) {
        animation->AddKeyframe(fraction, value, curve.GetInterpolator(GetDuration()));
    }
    StartCustomPropertyAnimation(animation);
}
} // namespace Rosen
} // namespace OHOS
