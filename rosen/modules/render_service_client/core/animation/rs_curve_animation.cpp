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

#include "animation/rs_curve_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_curve_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
template<typename T>
template<typename P>
void RSCurveAnimation<T>::StartAnimationImpl()
{
    RSPropertyAnimation<T>::OnStart();
    auto target = RSPropertyAnimation<T>::GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start curve animation, target is null!");
        return;
    }
    auto interpolator = timingCurve_.GetInterpolator(RSPropertyAnimation<T>::GetDuration());
    auto animation = std::make_shared<RSRenderCurveAnimation<T>>(RSPropertyAnimation<T>::GetId(),
        RSPropertyAnimation<T>::GetPropertyId(), RSPropertyAnimation<T>::originValue_,
        RSPropertyAnimation<T>::startValue_, RSPropertyAnimation<T>::endValue_);
    animation->SetInterpolator(interpolator);
    animation->SetAdditive(RSPropertyAnimation<T>::GetAdditive());
    RSAnimation::UpdateParamToRenderAnimation(animation);
    std::unique_ptr<RSCommand> command = std::make_unique<P>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    }
}

template<>
void RSCurveAnimation<float>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveFloat>();
}

template<>
void RSCurveAnimation<Color>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveColor>();
}

template<>
void RSCurveAnimation<Matrix3f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveMatrix3f>();
}

template<>
void RSCurveAnimation<Vector2f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveVec2f>();
}

template<>
void RSCurveAnimation<Vector4f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveVec4f>();
}

template<>
void RSCurveAnimation<Quaternion>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveQuaternion>();
}

template<>
void RSCurveAnimation<std::shared_ptr<RSFilter>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveFilter>();
}

template<>
void RSCurveAnimation<Vector4<Color>>::OnStart()
{
    StartAnimationImpl<RSAnimationCreateCurveVec4Color>();
}

template<>
void RSCurveAnimation<std::shared_ptr<RSAnimatableBase>>::OnStart()
{
    RSPropertyAnimation::OnStart();
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    auto animation = std::make_shared<RSRenderCurveAnimation<std::shared_ptr<RSAnimatableBase>>>(GetId(),
        GetPropertyId(), originValue_, startValue_, endValue_);
    animation->SetInterpolator(interpolator);
    StartCustomPropertyAnimation(animation);
}
} // namespace Rosen
} // namespace OHOS
