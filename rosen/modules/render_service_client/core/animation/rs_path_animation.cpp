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
#include "animation/rs_motion_path_option.h"
#include "animation/rs_render_path_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
static constexpr int HALF_NUM = 2;

RSPathAnimation::RSPathAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPath>& animationPath) : RSPropertyAnimation(property), animationPath_(animationPath)
{}

RSPathAnimation::RSPathAnimation(std::shared_ptr<RSPropertyBase> property, const std::string& path,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSPathAnimation(property, PreProcessPath(path, startValue, endValue))
{
    startValue_ = startValue;
    endValue_ = endValue;
    InitNeedPath(startValue_, endValue_);
}

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
    if (RSAnimation::IsStarted()) {
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
        ROSEN_LOGE("Failed to set begin fraction, invalid value:%{public}f", fraction);
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
        ROSEN_LOGE("Failed to set end fraction, invalid value:%{public}f", fraction);
        return;
    }

    endFraction_ = fraction;
}

float RSPathAnimation::GetEndFraction() const
{
    return endFraction_;
}

void RSPathAnimation::SetPathNeedAddOrigin(bool needAddOrigin)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set need Add Origin, path animation has started!");
        return;
    }

    needAddOrigin_ = needAddOrigin;
}

bool RSPathAnimation::GetPathNeedAddOrigin() const
{
    return needAddOrigin_;
}

void RSPathAnimation::OnStart()
{
    if (!animationPath_) {
        ROSEN_LOGE("Failed to start path animation, path is null!");
        return;
    }

    RSPropertyAnimation::OnStart();
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start curve animation, target is null!");
        return;
    }

    InitRotationId(target);
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    auto animation = std::make_shared<RSRenderPathAnimation>(GetId(), GetPropertyId(),
        originValue_->GetRenderProperty(), startValue_->GetRenderProperty(), endValue_->GetRenderProperty(),
        target->GetStagingProperties().GetRotation(), animationPath_);
    UpdateParamToRenderAnimation(animation);
    animation->SetInterpolator(interpolator);
    animation->SetRotationMode(GetRotationMode());
    animation->SetBeginFraction(GetBeginFraction());
    animation->SetEndFraction(GetEndFraction());
    animation->SetIsNeedPath(isNeedPath_);
    animation->SetPathNeedAddOrigin(GetPathNeedAddOrigin());
    animation->SetAdditive(GetAdditive());
    animation->SetRotationId(rotationId_);
    if (isNeedPath_) {
        property_->AddPathAnimation();
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreatePath>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(),
            target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote =
                std::make_unique<RSAnimationCreatePath>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
    }
}

void RSPathAnimation::InitInterpolationValue()
{
    if (!animationPath_) {
        ROSEN_LOGE("Failed to update interpolation value, path is null!");
        return;
    }

    if (isNeedPath_) {
        if (startValue_->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR2F &&
            InitInterpolationVector2f(startValue_, endValue_)) {
            return;
        }
        if (startValue_->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR4F &&
            InitInterpolationVector4f(startValue_, endValue_)) {
            return;
        }
    }

    byValue_ = endValue_ - startValue_;
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
        if (GetAutoReverse() && GetRepeatCount() % HALF_NUM == 0) {
            targetRotation = startTangent;
        } else {
            targetRotation = endTangent;
        }
    } else {
        float currentRotation = target->GetStagingProperties().GetRotation();
        if (GetAutoReverse() && GetRepeatCount() % HALF_NUM == 0) {
            targetRotation = IsReversed() ? currentRotation + byRotation
                : currentRotation - byRotation;
        } else {
            targetRotation = IsReversed() ? currentRotation - byRotation
                : currentRotation + byRotation;
        }
    }

    SetRotation(target, targetRotation);
}

void RSPathAnimation::InitRotationId(const std::shared_ptr<RSNode>& node)
{
    if (GetRotationPropertyId(node) == 0) {
        node->SetRotation(0.f);
    }
    rotationId_ = GetRotationPropertyId(node);
}

PropertyId RSPathAnimation::GetRotationPropertyId(const std::shared_ptr<RSNode>& node)
{
    std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());
    auto iter = node->propertyModifiers_.find(RSModifierType::ROTATION);
    if (iter != node->propertyModifiers_.end()) {
        return iter->second->GetPropertyId();
    }

    for (const auto& [id, modifier] : node->modifiers_) {
        if (modifier->GetModifierType() == RSModifierType::ROTATION) {
            return modifier->GetPropertyId();
        }
    }
    return 0;
}

void RSPathAnimation::SetRotation(const std::shared_ptr<RSNode>& node, const float rotation)
{
    std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());
    auto iter = node->modifiers_.find(rotationId_);
    if (iter != node->modifiers_.end()) {
        auto modifier = iter->second;
        if (modifier != nullptr) {
            std::static_pointer_cast<RSProperty<float>>(modifier->GetProperty())->stagingValue_ = rotation;
        }
        return;
    }

    for (const auto& [type, modifier] : node->propertyModifiers_) {
        if (modifier != nullptr && modifier->GetPropertyId() == rotationId_) {
            std::static_pointer_cast<RSProperty<float>>(modifier->GetProperty())->stagingValue_ = rotation;
        }
        return;
    }
}

void RSPathAnimation::OnCallFinishCallback()
{
    if (property_ != nullptr) {
        property_->RemovePathAnimation();
    }
}

void RSPathAnimation::ReplaceSubString(std::string& sourceStr, const std::string& subStr,
    const std::string& newStr) const
{
    std::string::size_type position = 0;
    while ((position = sourceStr.find(subStr)) != std::string::npos) {
        sourceStr.replace(position, subStr.length(), newStr);
    }

    ROSEN_LOGD("SVG path:%{public}s", sourceStr.c_str());
}

const std::shared_ptr<RSPath> RSPathAnimation::ProcessPath(const std::string& path, const float startX,
    const float startY, const float endX, const float endY) const
{
    std::string animationPath = path;
    ReplaceSubString(animationPath, "start.x", std::to_string(startX));
    ReplaceSubString(animationPath, "start.y", std::to_string(startY));
    ReplaceSubString(animationPath, "end.x", std::to_string(endX));
    ReplaceSubString(animationPath, "end.y", std::to_string(endY));
    return RSPath::CreateRSPath(animationPath);
}

const std::shared_ptr<RSPath> RSPathAnimation::PreProcessPath(const std::string& path,
    const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto startVector2f = std::static_pointer_cast<RSProperty<Vector2f>>(startValue);
    auto endVector2f = std::static_pointer_cast<RSProperty<Vector2f>>(endValue);
    if (startValue->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR2F && startVector2f != nullptr &&
        endVector2f != nullptr) {
        return ProcessPath(path, startVector2f->Get()[0], startVector2f->Get()[1], endVector2f->Get()[0],
            endVector2f->Get()[1]);
    }

    auto startVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(startValue);
    auto endVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(endValue);
    if (startValue->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR4F && startVector4f != nullptr &&
        endVector4f != nullptr) {
        return ProcessPath(path, startVector4f->Get()[0], startVector4f->Get()[1], endVector4f->Get()[0],
            endVector4f->Get()[1]);
    }

    return {};
}

void RSPathAnimation::InitNeedPath(const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue)
{
    auto startVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(startValue);
    auto endVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(endValue);
    if (startValue->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR4F &&
        startVector4f != nullptr && endVector4f != nullptr) {
        Vector2f start(startVector4f->Get()[0], startVector4f->Get()[1]);
        Vector2f end(endVector4f->Get()[0], endVector4f->Get()[1]);
        isNeedPath_ = (start != end);
        if (isNeedPath_) {
            SetAdditive(false);
        }
        return;
    }

    SetAdditive(false);
}

bool RSPathAnimation::InitInterpolationVector2f(const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue)
{
    auto startVector2f = std::static_pointer_cast<RSProperty<Vector2f>>(startValue);
    auto endVector2f = std::static_pointer_cast<RSProperty<Vector2f>>(endValue);
    if (startVector2f != nullptr && endVector2f != nullptr) {
        animationPath_->GetPosTan(0.0f * beginFraction_, startVector2f->stagingValue_, startTangent_);
        animationPath_->GetPosTan(animationPath_->GetDistance() * endFraction_,
            endVector2f->stagingValue_, endTangent_);
        auto originVector2f = std::static_pointer_cast<RSProperty<Vector2f>>(GetOriginValue());
        if (originVector2f != nullptr && needAddOrigin_) {
            UpdateVector2fValueAddOrigin(startVector2f->stagingValue_, endVector2f->stagingValue_,
                originVector2f->stagingValue_);
        }
        byValue_ = endValue - startValue;
        return true;
    }

    return false;
}

bool RSPathAnimation::InitInterpolationVector4f(const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue)
{
    auto startVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(startValue);
    auto endVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(endValue);
    if (startVector4f != nullptr && endVector4f != nullptr) {
        animationPath_->GetPosTan(0.0f * beginFraction_, startVector4f->stagingValue_, startTangent_);
        animationPath_->GetPosTan(animationPath_->GetDistance() * endFraction_,
            endVector4f->stagingValue_, endTangent_);
        auto originVector4f = std::static_pointer_cast<RSProperty<Vector4f>>(GetOriginValue());
        if (originVector4f != nullptr && needAddOrigin_) {
            UpdateVector4fValueAddOrigin(startVector4f->stagingValue_, endVector4f->stagingValue_,
                originVector4f->stagingValue_);
        }
        byValue_ = endValue - startValue;
        return true;
    }

    return false;
}

void RSPathAnimation::UpdateVector2fValueAddOrigin(Vector2f& startValue, Vector2f& endValue, Vector2f& deltaValue)
{
    startValue += deltaValue;
    endValue += deltaValue;
}

void RSPathAnimation::UpdateVector4fValueAddOrigin(Vector4f& startValue, Vector4f& endValue, Vector4f& deltaValue)
{
    startValue[0] += deltaValue[0];
    startValue[1] += deltaValue[1];
    endValue[0] += deltaValue[0];
    endValue[1] += deltaValue[1];
}
} // namespace Rosen
} // namespace OHOS
