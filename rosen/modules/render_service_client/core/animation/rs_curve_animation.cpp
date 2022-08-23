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
#include "ui/rs_node.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
RSCurveAnimation::RSCurveAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& byValue) : RSPropertyAnimation(property)
{
    isDelta_ = true;
    byValue_ = byValue;
}

RSCurveAnimation::RSCurveAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSPropertyAnimation(property)
{
    isDelta_ = false;
    startValue_ = startValue;
    endValue_ = endValue;
}

void RSCurveAnimation::SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
{
    if (timingCurve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
        ROSEN_LOGE("RSCurveAnimation::SetTimingCurve: invalid timing curve type");
        return;
    }
    timingCurve_ = timingCurve;
}

const RSAnimationTimingCurve& RSCurveAnimation::GetTimingCurve() const
{
    return timingCurve_;
}

void RSCurveAnimation::StartRenderAnimation(const std::shared_ptr<RSRenderCurveAnimation>& animation)
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start curve animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateCurve>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedSendExtraCommand()) {
            std::unique_ptr<RSCommand> extraCommand =
                std::make_unique<RSAnimationCreateCurve>(target->GetId(), animation);
            transactionProxy->AddCommand(extraCommand,
                !target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        }
    }
}

void RSCurveAnimation::StartUIAnimation(const std::shared_ptr<RSRenderCurveAnimation>& animation)
{
    StartCustomPropertyAnimation(animation);
}

void RSCurveAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    auto animation = std::make_shared<RSRenderCurveAnimation>(GetId(), GetPropertyId(),
        originValue_->CreateRenderProperty(), startValue_->CreateRenderProperty(), endValue_->CreateRenderProperty());
    animation->SetInterpolator(interpolator);
    animation->SetAdditive(GetAdditive());
    UpdateParamToRenderAnimation(animation);
    if (isCustom_) {
        StartUIAnimation(animation);
    } else {
        StartRenderAnimation(animation);
    }
}
} // namespace Rosen
} // namespace OHOS
