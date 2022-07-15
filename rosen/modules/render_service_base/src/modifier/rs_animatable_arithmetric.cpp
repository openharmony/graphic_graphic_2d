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

#include "modifier/rs_animatable_arithmetic.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSAnimatableBase> RSAnimatableFloat::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFloat>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableFloat>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableFloat>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableFloat::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFloat>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableFloat>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableFloat>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableFloat::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableFloat>(this->value_ * scale);
}

bool RSAnimatableFloat::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFloat>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

float RSAnimatableFloat::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableInt::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableInt>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableInt>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableInt>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableInt::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableInt>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableInt>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableInt>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableInt::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableInt>(this->value_ * scale);
}

bool RSAnimatableInt::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableInt>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

int RSAnimatableInt::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableColor::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableColor>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableColor>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableColor>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableColor::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableColor>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableColor>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableColor>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableColor::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableColor>(this->value_ * scale);
}

bool RSAnimatableColor::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableColor>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Color RSAnimatableColor::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableMatrix3f::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableMatrix3f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableMatrix3f>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableMatrix3f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableMatrix3f::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableMatrix3f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableMatrix3f>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableMatrix3f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableMatrix3f::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableMatrix3f>(this->value_ * scale);
}

bool RSAnimatableMatrix3f::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableMatrix3f>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Matrix3f RSAnimatableMatrix3f::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableQuaternion::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableQuaternion>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableQuaternion>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableQuaternion>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableQuaternion::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableQuaternion>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableQuaternion>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableQuaternion>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableQuaternion::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableQuaternion>(this->value_ * scale);
}

bool RSAnimatableQuaternion::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableQuaternion>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Quaternion RSAnimatableQuaternion::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableFilter::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFilter>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableFilter>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableFilter>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableFilter::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFilter>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableFilter>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableFilter>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableFilter::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableFilter>(this->value_ * scale);
}

bool RSAnimatableFilter::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableFilter>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

std::shared_ptr<RSFilter> RSAnimatableFilter::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector2f::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector2f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector2f>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector2f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector2f::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector2f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector2f>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector2f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector2f::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableVector2f>(this->value_ * scale);
}

bool RSAnimatableVector2f::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector2f>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Vector2f RSAnimatableVector2f::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4f::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector4f>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector4f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4f::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4f>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector4f>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector4f>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4f::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableVector4f>(this->value_ * scale);
}

bool RSAnimatableVector4f::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4f>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Vector4f RSAnimatableVector4f::GetValue() const
{
    return value_;
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4Color::Add(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4Color>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector4Color>(this->value_ + newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector4Color>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4Color::Minus(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4Color>(value);
    if (newValue != nullptr) {
        return std::make_shared<RSAnimatableVector4Color>(this->value_ - newValue->GetValue());
    }
    return std::make_shared<RSAnimatableVector4Color>(this->value_);
}

std::shared_ptr<RSAnimatableBase> RSAnimatableVector4Color::Multiply(const float scale) const
{
    return std::make_shared<RSAnimatableVector4Color>(this->value_ * scale);
}

bool RSAnimatableVector4Color::IsEqual(const std::shared_ptr<RSAnimatableBase>& value) const
{
    auto newValue = std::static_pointer_cast<RSAnimatableVector4Color>(value);
    if (newValue != nullptr) {
        return this->value_ == newValue->GetValue();
    }
    return false;
}

Vector4<Color> RSAnimatableVector4Color::GetValue() const
{
    return value_;
}
} // namespace Rosen
} // namespace OHOS
