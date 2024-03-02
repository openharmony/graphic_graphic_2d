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

#include "animation/rs_property_animation.h"

#include "animation/rs_render_animation.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
static constexpr int HALF_NUM = 2;

RSPropertyAnimation::RSPropertyAnimation(std::shared_ptr<RSPropertyBase> property) : property_(property)
{
    InitAdditiveMode();
}

void RSPropertyAnimation::SetIsCustom(const bool isCustom)
{
    isCustom_ = isCustom;
}

void RSPropertyAnimation::SetAdditive(bool isAdditive)
{
    isAdditive_ = isAdditive;
}

bool RSPropertyAnimation::GetAdditive() const
{
    return isAdditive_;
}

const std::shared_ptr<RSPropertyBase> RSPropertyAnimation::GetOriginValue() const
{
    return originValue_;
}

void RSPropertyAnimation::SetPropertyValue(const std::shared_ptr<RSPropertyBase>& value)
{
    if (property_ != nullptr) {
        property_->SetValue(value);
    }
}

const std::shared_ptr<RSPropertyBase> RSPropertyAnimation::GetPropertyValue() const
{
    if (property_ != nullptr) {
        return property_->Clone();
    }

    return nullptr;
}

PropertyId RSPropertyAnimation::GetPropertyId() const
{
    if (property_ != nullptr) {
        return property_->GetId();
    }

    return {};
}

void RSPropertyAnimation::OnStart()
{
    if (!hasOriginValue_) {
        originValue_ = GetPropertyValue();
        hasOriginValue_ = true;
    }
    InitInterpolationValue();
}

void RSPropertyAnimation::SetOriginValue(const std::shared_ptr<RSPropertyBase>& originValue)
{
    if (!hasOriginValue_) {
        originValue_ = originValue->Clone();
        hasOriginValue_ = true;
    }
}

void RSPropertyAnimation::InitInterpolationValue()
{
    if (isDelta_) {
        startValue_ = originValue_->Clone();
        endValue_ = originValue_ + byValue_;
    } else {
        byValue_ = endValue_ - startValue_;
    }
}

void RSPropertyAnimation::OnUpdateStagingValue(bool isFirstStart)
{
    auto startValue = startValue_;
    auto endValue = endValue_;
    if (!GetDirection()) {
        std::swap(startValue, endValue);
    }
    auto byValue = endValue - startValue;
    auto targetValue = endValue;
    if (isFirstStart) {
        if (GetAutoReverse() && GetRepeatCount() % HALF_NUM == 0) {
            targetValue = startValue;
        } else {
            targetValue = endValue;
        }
    } else {
        auto currentValue = GetPropertyValue();
        if (GetAutoReverse() && GetRepeatCount() % HALF_NUM == 0) {
            targetValue = IsReversed() ? currentValue + byValue : currentValue - byValue;
        } else {
            targetValue = IsReversed() ? currentValue - byValue : currentValue + byValue;
        }
    }

    SetPropertyValue(targetValue);
}

void RSPropertyAnimation::SetPropertyOnAllAnimationFinish()
{
    if (property_ != nullptr) {
        property_->UpdateOnAllAnimationFinish();
    }
}

void RSPropertyAnimation::InitAdditiveMode()
{
    if (property_ == nullptr) {
        return;
    }

    switch (property_->type_) {
        case RSModifierType::QUATERNION:
            SetAdditive(false);
            break;
        default:
            break;
    }
}
} // namespace Rosen
} // namespace OHOS
