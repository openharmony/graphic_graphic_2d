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

#include "animation/rs_transition.h"

#include "animation/rs_render_transition.h"
#include "command/rs_animation_command.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
RSTransition::RSTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
    : isTransitionIn_(isTransitionIn), effect_(effect)
{}

void RSTransition::OnStart()
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        return;
    }
    auto transition = std::make_shared<RSRenderTransition>(GetId(), effect_, isTransitionIn_);
    if (transition == nullptr) {
        return;
    }
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    transition->SetDuration(GetDuration());
    transition->SetStartDelay(GetStartDelay());
    transition->SetRepeatCount(GetRepeatCount());
    transition->SetAutoReverse(GetAutoReverse());
    transition->SetSpeed(GetSpeed());
    transition->SetDirection(GetDirection());
    transition->SetFillMode(GetFillMode());
    transition->SetInterpolator(interpolator);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCreateTransition>(target->GetId(), std::move(transition));
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}
} // namespace Rosen
} // namespace OHOS
