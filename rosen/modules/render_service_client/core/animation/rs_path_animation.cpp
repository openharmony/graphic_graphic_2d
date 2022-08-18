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

#include "animation/rs_path_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_path_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
template<typename T>
template<typename P>
void RSPathAnimation<T>::StartAnimationImpl()
{
    if (!animationPath_) {
        ROSEN_LOGE("Failed to start path animation, path is null!");
        return;
    }

    RSPropertyAnimation<T>::OnStart();
    auto target = RSPropertyAnimation<T>::GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start curve animation, target is null!");
        return;
    }
    InitRotationId(target);
    auto interpolator = timingCurve_.GetInterpolator(RSPropertyAnimation<T>::GetDuration());
    auto animation = std::make_shared<RSRenderPathAnimation<T>>(RSPropertyAnimation<T>::GetId(),
        RSPropertyAnimation<T>::GetPropertyId(), RSPropertyAnimation<T>::originValue_,
        RSPropertyAnimation<T>::startValue_, RSPropertyAnimation<T>::endValue_,
        target->GetStagingProperties().GetRotation(), animationPath_);
    RSAnimation::UpdateParamToRenderAnimation(animation);
    animation->SetInterpolator(interpolator);
    animation->SetRotationMode(GetRotationMode());
    animation->SetBeginFraction(GetBeginFraction());
    animation->SetEndFraction(GetEndFraction());
    animation->SetIsNeedPath(isNeedPath_);
    animation->SetPathNeedAddOrigin(GetPathNeedAddOrigin());
    animation->SetAdditive(RSPropertyAnimation<T>::GetAdditive());
    animation->SetRotationId(rotationId_);
    if (isNeedPath_) {
        RSPropertyAnimation<T>::property_.runningPathNum_ += 1;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<P>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<P>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
        if (target->NeedSendExtraCommand()) {
            std::unique_ptr<RSCommand> extraCommand = std::make_unique<P>(target->GetId(), animation);
            transactionProxy->AddCommand(extraCommand, !target->IsRenderServiceNode(), target->GetFollowType(),
                target->GetId());
        }
    }
}

template<>
void RSPathAnimation<Vector2f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreatePathVec2f>();
}

template<>
void RSPathAnimation<Vector4f>::OnStart()
{
    StartAnimationImpl<RSAnimationCreatePathVec4f>();
}

template<>
const std::shared_ptr<RSPath> RSPathAnimation<Vector2f>::PreProcessPath(
    const std::string& path, const Vector2f& startValue, const Vector2f& endValue) const
{
    return ProcessPath(path, startValue[0], startValue[1], endValue[0], endValue[1]);
}

template<>
const std::shared_ptr<RSPath> RSPathAnimation<Vector4f>::PreProcessPath(
    const std::string& path, const Vector4f& startValue, const Vector4f& endValue) const
{
    return ProcessPath(path, startValue[0], startValue[1], endValue[0], endValue[1]);
}

template<>
void RSPathAnimation<Vector4f>::InitNeedPath(const Vector4f& startValue, const Vector4f& endValue)
{
    Vector2f start(startValue[0], startValue[1]);
    Vector2f end(endValue[0], endValue[1]);
    isNeedPath_ = (start != end);
    if (isNeedPath_) {
        RSPropertyAnimation<Vector4f>::SetAdditive(false);
    }
}

template<>
void RSPathAnimation<Vector2f>::UpdateValueAddOrigin(Vector2f& value)
{
    value += GetOriginValue();
}

template<>
void RSPathAnimation<Vector4f>::UpdateValueAddOrigin(Vector4f& value)
{
    value[0] += GetOriginValue()[0];
    value[1] += GetOriginValue()[1];
}
} // namespace Rosen
} // namespace OHOS
