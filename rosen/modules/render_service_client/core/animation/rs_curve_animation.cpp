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

#include "animation/rs_curve_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_curve_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

#define START_CURVE_ANIMATION(RSRenderCommand, Type)                                                               \
    RSPropertyAnimation<Type>::OnStart();                                                                          \
    auto target = GetTarget().lock();                                                                              \
    if (target == nullptr) {                                                                                       \
        ROSEN_LOGE("Failed to start curve animation, target is null!");                                            \
        return;                                                                                                    \
    }                                                                                                              \
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());                                               \
    auto animation = std::make_shared<RSRenderCurveAnimation<Type>>(GetId(), GetProperty(),                        \
        RSPropertyAnimation<Type>::originValue_, RSPropertyAnimation<Type>::startValue_,                           \
        RSPropertyAnimation<Type>::endValue_);                                                                     \
    animation->SetDuration(GetDuration());                                                                         \
    animation->SetStartDelay(GetStartDelay());                                                                     \
    animation->SetRepeatCount(GetRepeatCount());                                                                   \
    animation->SetAutoReverse(GetAutoReverse());                                                                   \
    animation->SetSpeed(GetSpeed());                                                                               \
    animation->SetDirection(GetDirection());                                                                       \
    animation->SetFillMode(GetFillMode());                                                                         \
    animation->SetInterpolator(interpolator);                                                                      \
    std::unique_ptr<RSCommand> command = std::make_unique<RSRenderCommand>(target->GetId(), std::move(animation)); \
    RSTransactionProxy::GetInstance().AddCommand(command);

template<>
void RSCurveAnimation<int>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveInt, int);
}

template<>
void RSCurveAnimation<float>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveFloat, float);
}

template<>
void RSCurveAnimation<Color>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveColor, Color);
}

template<>
void RSCurveAnimation<Matrix3f>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveMatrix3f, Matrix3f);
}

template<>
void RSCurveAnimation<Vector2f>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveVec2f, Vector2f);
}

template<>
void RSCurveAnimation<Vector4f>::OnStart()
{
    START_CURVE_ANIMATION(RSAnimationCreateCurveVec4f, Vector4f);
}

template<>
void RSCurveAnimation<std::shared_ptr<RSFilter>>::OnStart()
{
    // START_CURVE_ANIMATION(RSAnimationCreateCurveFilter, std::shared_ptr<RSFilter>);
}
} // namespace Rosen
} // namespace OHOS
