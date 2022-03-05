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

#include "animation/rs_path_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_path_animation.h"
#include "platform/common/rs_log.h"
#include "command/rs_animation_command.h"
#include "transaction/rs_transaction_proxy.h"
#include "render/rs_path.h"

namespace OHOS {
namespace Rosen {
RSPathAnimation::RSPathAnimation(const RSAnimatableProperty& property, const std::shared_ptr<RSPath>& animationPath)
    : RSPropertyAnimation(property), animationPath_(animationPath)
{
    SetAdditive(false);
}

RSPathAnimation::RSPathAnimation(
    const RSAnimatableProperty& property, const std::string& path, const Vector2f& startValue, const Vector2f& endValue)
    : RSPathAnimation(property, PreProcessPath(path, startValue, endValue))
{}

const std::vector<RSAnimatableProperty> RSPathAnimation::PROP_FOR_PATH_ANIM = {
    RSAnimatableProperty::BOUNDS_POSITION,
    RSAnimatableProperty::FRAME_POSITION,
    RSAnimatableProperty::TRANSLATE
};

void RSPathAnimation::SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
{
    timingCurve_ = timingCurve;
}

const RSAnimationTimingCurve& RSPathAnimation::GetTimingCurve() const
{
    return timingCurve_;
}

void RSPathAnimation::SetRotationMode(const RotationMode& rotationMode)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to enable rotate, path animation has started!");
        return;
    }

    rotationMode_ = rotationMode;
}

RotationMode RSPathAnimation::GetRotationMode() const
{
    return rotationMode_;
}

void RSPathAnimation::SetBeginFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set begin fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction > endFraction_) {
        ROSEN_LOGE("Failed to set begin fraction, invalid value:%f", fraction);
        return;
    }

    beginFraction_ = fraction;
}

float RSPathAnimation::GetBeginFraction() const
{
    return beginFraction_;
}

void RSPathAnimation::SetEndFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set end fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction < beginFraction_) {
        ROSEN_LOGE("Failed to set end fraction, invalid value:%f", fraction);
        return;
    }

    endFraction_ = fraction;
}

float RSPathAnimation::GetEndFraction() const
{
    return endFraction_;
}

void RSPathAnimation::OnStart()
{
    if (!IsAnimatablePathProperty(GetProperty())) {
        ROSEN_LOGE("Failed to start path animation, property[%d] is not supported!", static_cast<int>(GetProperty()));
        return;
    }

    if (!animationPath_) {
        ROSEN_LOGE("Failed to start path animation, path is null!");
        return;
    }

    RSPropertyAnimation::OnStart();

    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start path animation, target is null!");
        return;
    }

    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    auto animation = std::make_shared<RSRenderPathAnimation>(
        GetId(), GetProperty(), originValue_, target->stagingProperties_.GetRotation(), animationPath_);
    animation->SetDuration(GetDuration());
    animation->SetStartDelay(GetStartDelay());
    animation->SetRepeatCount(GetRepeatCount());
    animation->SetAutoReverse(GetAutoReverse());
    animation->SetSpeed(GetSpeed());
    animation->SetFillMode(GetFillMode());
    animation->SetDirection(GetDirection());
    animation->SetInterpolator(interpolator);
    animation->SetRotationMode(GetRotationMode());
    animation->SetBeginFraction(GetBeginFraction());
    animation->SetEndFraction(GetEndFraction());
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreatePath>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote =
                std::make_unique<RSAnimationCreatePath>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true);
        }
    }
}

void RSPathAnimation::InitInterpolationValue()
{
    if (!animationPath_) {
        ROSEN_LOGE("Failed to update interpolation value, path is null!");
        return;
    }

#ifdef ROSEN_OHOS
    animationPath_->GetPosTan(0.0f, startValue_, startTangent_);
    animationPath_->GetPosTan(animationPath_->GetDistance(), endValue_, endTangent_);
    byValue_ = endValue_ - startValue_;
#endif
}

void RSPathAnimation::OnUpdateStagingValue(bool isFirstStart)
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to update staging value, target is null!");
        return;
    }

    RSPropertyAnimation::OnUpdateStagingValue(isFirstStart);

    float startTangent = 0.0f;
    float endTangent = 0.0f;
    switch (rotationMode_) {
        case RotationMode::ROTATE_NONE:
            return;
        case RotationMode::ROTATE_AUTO:
            startTangent = startTangent_;
            endTangent = endTangent_;
            break;
        case RotationMode::ROTATE_AUTO_REVERSE:
            startTangent = startTangent_ + 180.0f;
            endTangent = endTangent_ + 180.0f;
            break;
        default:
            ROSEN_LOGE("Unknow rotation mode!");
            return;
    }
    if (!GetDirection()) {
        std::swap(startTangent, endTangent);
    }

    float targetRotation = 0.0f;
    float byRotation = endTangent - startTangent;
    if (isFirstStart) {
        if (GetAutoReverse() && GetRepeatCount() % 2 == 0) {
            targetRotation = startTangent;
        } else {
            targetRotation = endTangent;
        }
    } else {
        float currentRotation = target->stagingProperties_.GetRotation();
        if (GetAutoReverse() && GetRepeatCount() % 2 == 0) {
            targetRotation = IsReversed() ? currentRotation + byRotation : currentRotation - byRotation;
        } else {
            targetRotation = IsReversed() ? currentRotation - byRotation : currentRotation + byRotation;
        }
    }

    target->stagingProperties_.SetRotation(targetRotation);
}

void RSPathAnimation::ReplaceSubString(
    std::string& sourceStr, const std::string& subStr, const std::string& newStr) const
{
    std::string::size_type position = 0;
    while ((position = sourceStr.find(subStr)) != std::string::npos) {
        sourceStr.replace(position, subStr.length(), newStr);
    }

    ROSEN_LOGD("SVG path:%s", sourceStr.c_str());
}

const std::shared_ptr<RSPath> RSPathAnimation::PreProcessPath(
    const std::string& path, const Vector2f& startValue, const Vector2f& endValue) const
{
    std::string animationPath = path;
    ReplaceSubString(animationPath, "start.x", std::to_string(startValue[0]));
    ReplaceSubString(animationPath, "start.y", std::to_string(startValue[1]));
    ReplaceSubString(animationPath, "end.x", std::to_string(endValue[0]));
    ReplaceSubString(animationPath, "end.y", std::to_string(endValue[1]));
#ifdef ROSEN_OHOS
    return RSPath::CreateRSPath(animationPath);
#else
    return nullptr;
#endif
}

bool RSPathAnimation::IsAnimatablePathProperty(const RSAnimatableProperty& property)
{
    if (find(PROP_FOR_PATH_ANIM.begin(), PROP_FOR_PATH_ANIM.end(), property) == PROP_FOR_PATH_ANIM.end()) {
        return false;
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS
