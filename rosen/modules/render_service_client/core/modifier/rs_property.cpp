/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "modifier/rs_property.h"

#include "command/rs_node_command.h"
#include "modifier/rs_modifier.h"
#include "sandbox_utils.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PID_SHIFT = 32;

PropertyId GeneratePropertyId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 1;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Property Id overflow");
    }

    return ((PropertyId)pid_ << PID_SHIFT) | currentId;
}
} // namespace

RSPropertyBase::RSPropertyBase() : id_(GeneratePropertyId())
{}

void RSPropertyBase::MarkModifierDirty()
{
    auto modifier = modifier_.lock();
    if (modifier != nullptr) {
        modifier->SetDirty(true);
    }
}

void RSPropertyBase::UpdateExtendModifierForGeometry(const std::shared_ptr<RSNode>& node)
{
    if (type_ == RSModifierType::BOUNDS || type_ == RSModifierType::FRAME) {
        node->MarkAllExtendModifierDirty();
    }
}

float RSPropertyBase::GetThresholdByThresholdType(ThresholdType thresholdType) const
{
    switch (thresholdType) {
        case ThresholdType::LAYOUT:
            return LAYOUT_NEAR_ZERO_THRESHOLD;
        case ThresholdType::COARSE:
            return FLOAT_NEAR_ZERO_COARSE_THRESHOLD;
        case ThresholdType::MEDIUM:
            return FLOAT_NEAR_ZERO_MEDIUM_THRESHOLD;
        case ThresholdType::FINE:
            return FLOAT_NEAR_ZERO_FINE_THRESHOLD;
        case ThresholdType::COLOR:
            return COLOR_NEAR_ZERO_THRESHOLD;
        case ThresholdType::ZERO:
            return ZERO;
        default:
            return DEFAULT_NEAR_ZERO_THRESHOLD;
    }
}

std::shared_ptr<RSPropertyBase> operator+=(const std::shared_ptr<RSPropertyBase>& a,
    const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Add(b);
}

std::shared_ptr<RSPropertyBase> operator-=(const std::shared_ptr<RSPropertyBase>& a,
    const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Minus(b);
}

std::shared_ptr<RSPropertyBase> operator*=(const std::shared_ptr<RSPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Multiply(scale);
}

std::shared_ptr<RSPropertyBase> operator+(const std::shared_ptr<const RSPropertyBase>& a,
    const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Clone()->Add(b);
}

std::shared_ptr<RSPropertyBase> operator-(const std::shared_ptr<const RSPropertyBase>& a,
    const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Clone()->Minus(b);
}

std::shared_ptr<RSPropertyBase> operator*(const std::shared_ptr<const RSPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Clone()->Multiply(scale);
}

bool operator==(const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }

    return a->IsEqual(b);
}

bool operator!=(const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }

    return !a->IsEqual(b);
}

#define UPDATE_TO_RENDER(Command, value, type)                                                                        \
    do {                                                                                                              \
        auto node = target_.lock();                                                                                   \
        auto transactionProxy = RSTransactionProxy::GetInstance();                                                    \
        if (transactionProxy && node) {                                                                               \
            std::unique_ptr<RSCommand> command = std::make_unique<Command>(node->GetId(), value, id_, type);          \
            transactionProxy->AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId()); \
            if (node->NeedForcedSendToRemote()) {                                                                     \
                std::unique_ptr<RSCommand> commandForRemote =                                                         \
                    std::make_unique<Command>(node->GetId(), value, id_, type);                                       \
                transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());           \
            }                                                                                                         \
        }                                                                                                             \
    } while (0)

template<>
void RSProperty<bool>::UpdateToRender(const bool& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyBool, value, type);
}
template<>
void RSProperty<float>::UpdateToRender(const float& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyFloat, value, type);
}
template<>
void RSProperty<int>::UpdateToRender(const int& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyInt, value, type);
}
template<>
void RSProperty<Color>::UpdateToRender(const Color& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyColor, value, type);
}
template<>
void RSProperty<Gravity>::UpdateToRender(const Gravity& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyGravity, value, type);
}
template<>
void RSProperty<Matrix3f>::UpdateToRender(const Matrix3f& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyMatrix3f, value, type);
}
template<>
void RSProperty<Quaternion>::UpdateToRender(const Quaternion& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyQuaternion, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSFilter>>::UpdateToRender(
    const std::shared_ptr<RSFilter>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyFilter, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSImage>>::UpdateToRender(
    const std::shared_ptr<RSImage>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyImage, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSMask>>::UpdateToRender(
    const std::shared_ptr<RSMask>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyMask, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSPath>>::UpdateToRender(
    const std::shared_ptr<RSPath>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyPath, value, type);
}
template<>
void RSProperty<RSDynamicBrightnessPara>::UpdateToRender(
    const RSDynamicBrightnessPara& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyDynamicBrightness, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>::UpdateToRender(
    const std::shared_ptr<RSLinearGradientBlurPara>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyLinearGradientBlurPara, value, type);
}
template<>
void RSProperty<std::shared_ptr<MotionBlurParam>>::UpdateToRender(
    const std::shared_ptr<MotionBlurParam>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyMotionBlurPara, value, type);
}
template<>
void RSProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::UpdateToRender(
    const std::vector<std::shared_ptr<EmitterUpdater>>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyEmitterUpdater, value, type);
}
template<>
void RSProperty<std::shared_ptr<ParticleNoiseFields>>::UpdateToRender(
    const std::shared_ptr<ParticleNoiseFields>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyParticleNoiseFields, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSShader>>::UpdateToRender(
    const std::shared_ptr<RSShader>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyShader, value, type);
}
template<>
void RSProperty<Vector2f>::UpdateToRender(const Vector2f& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyVector2f, value, type);
}
template<>
void RSProperty<Vector4<uint32_t>>::UpdateToRender(const Vector4<uint32_t>& value,
    PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyBorderStyle, value, type);
}
template<>
void RSProperty<Vector4<Color>>::UpdateToRender(const Vector4<Color>& value,
    PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyVector4Color, value, type);
}
template<>
void RSProperty<Vector4f>::UpdateToRender(const Vector4f& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyVector4f, value, type);
}

template<>
void RSProperty<RRect>::UpdateToRender(const RRect& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyRRect, value, type);
}

template<>
bool RSProperty<float>::IsValid(const float& value)
{
    return !isinf(value);
}
template<>
bool RSProperty<Vector2f>::IsValid(const Vector2f& value)
{
    return !value.IsInfinite();
}
template<>
bool RSProperty<Vector4f>::IsValid(const Vector4f& value)
{
    return !value.IsInfinite();
}

template<>
RSRenderPropertyType RSAnimatableProperty<float>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_FLOAT;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Color>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_COLOR;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Matrix3f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_MATRIX3F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector2f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR2F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector4f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR4F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Quaternion>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_QUATERNION;
}
template<>
RSRenderPropertyType RSAnimatableProperty<std::shared_ptr<RSFilter>>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_FILTER;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector4<Color>>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR4_COLOR;
}
template<>
RSRenderPropertyType RSAnimatableProperty<RRect>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_RRECT;
}
} // namespace Rosen
} // namespace OHOS
