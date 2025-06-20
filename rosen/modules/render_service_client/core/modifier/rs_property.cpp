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

#include "sandbox_utils.h"
#include "ui_effect/property/include/rs_ui_filter.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"

#include "command/rs_node_command.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PID_SHIFT = 32;

namespace {
constexpr float DEFAULT_NEAR_ZERO_THRESHOLD = 1.0f / 256.0f;
constexpr float FLOAT_NEAR_ZERO_COARSE_THRESHOLD = 1.0f / 256.0f;
constexpr float FLOAT_NEAR_ZERO_MEDIUM_THRESHOLD = 1.0f / 1000.0f;
constexpr float FLOAT_NEAR_ZERO_FINE_THRESHOLD = 1.0f / 3072.0f;
constexpr float COLOR_NEAR_ZERO_THRESHOLD = 0.0f;
constexpr float LAYOUT_NEAR_ZERO_THRESHOLD = 0.5f;
constexpr float ZERO = 0.0f;
} // namespace

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
#if defined(MODIFIER_NG)
    if (auto modifier = modifierNG_.lock()) {
#else
    if (auto modifier = modifier_.lock()) {
#endif
        auto node = target_.lock();
        if (node && node->GetRSUIContext()) {
            modifier->SetDirty(true, node->GetRSUIContext()->GetRSModifierManager());
        } else {
            modifier->SetDirty(true, RSModifierManagerMap::Instance()->GetModifierManager(gettid()));
        }
    }
}

void RSPropertyBase::MarkNodeDirty()
{
    if (auto modifier = modifierNG_.lock()) {
        modifier->MarkNodeDirty();
    } else if (auto modifier = modifier_.lock()) {
        modifier->MarkNodeDirty();
    }
}

void RSPropertyBase::UpdateExtendModifierForGeometry(const std::shared_ptr<RSNode>& node)
{
    if (auto modifier = modifierNG_.lock()) {
        if (modifier->GetType() == ModifierNG::RSModifierType::BOUNDS ||
            modifier->GetType() == ModifierNG::RSModifierType::FRAME) {
            node->MarkAllExtendModifierDirty();
            return;
        }
    }

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

    const auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Add(b);
}

std::shared_ptr<RSPropertyBase> operator-(const std::shared_ptr<const RSPropertyBase>& a,
    const std::shared_ptr<const RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    const auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Minus(b);
}

std::shared_ptr<RSPropertyBase> operator*(const std::shared_ptr<const RSPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    const auto clone = value->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Multiply(scale);
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

template<>
void RSProperty<std::shared_ptr<RSNGFilterBase>>::OnAttach(const std::shared_ptr<RSNode>& node)
{
    if (stagingValue_) {
        stagingValue_->Attach(node);
    }
}

template<>
void RSProperty<std::shared_ptr<RSNGFilterBase>>::OnDetach(const std::shared_ptr<RSNode>& node)
{
    if (stagingValue_) {
        stagingValue_->Detach();
    }
}

template<>
void RSProperty<std::shared_ptr<RSNGFilterBase>>::Set(const std::shared_ptr<RSNGFilterBase>& value)
{
    if (stagingValue_ && stagingValue_->SetValue(value, target_.lock())) {
        return;
    }

    // failed to update all properties in filter, fall back to replace filter
    if (stagingValue_) {
        stagingValue_->Detach();
    }

    stagingValue_ = value;
    auto node = target_.lock();
    if (node == nullptr) {
        return;
    }
    if (stagingValue_) {
        stagingValue_->Attach(node);
    }

    MarkNodeDirty();
    UpdateToRender(stagingValue_, UPDATE_TYPE_OVERWRITE);
}

template<>
RSC_EXPORT std::shared_ptr<RSRenderPropertyBase> RSProperty<std::shared_ptr<RSNGFilterBase>>::GetRenderProperty()
{
    return std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(
        stagingValue_->GetRenderEffect(), id_);
}

#define UPDATE_TO_RENDER(Command, value, type)                                                                       \
    auto node = target_.lock();                                                                                      \
    if (node != nullptr) {                                                                                           \
        auto transaction = node->GetRSTransaction();                                                                 \
        if (!transaction) {                                                                                          \
            do {                                                                                                     \
                auto transactionProxy = RSTransactionProxy::GetInstance();                                           \
                if (transactionProxy) {                                                                              \
                    std::unique_ptr<RSCommand> command = std::make_unique<Command>(node->GetId(), value, id_, type); \
                    transactionProxy->AddCommand(                                                                    \
                        command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());                 \
                    if (node->NeedForcedSendToRemote()) {                                                            \
                        std::unique_ptr<RSCommand> commandForRemote =                                                \
                            std::make_unique<Command>(node->GetId(), value, id_, type);                              \
                        transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());  \
                    }                                                                                                \
                }                                                                                                    \
            } while (0);                                                                                             \
        } else {                                                                                                     \
            do {                                                                                                     \
                std::unique_ptr<RSCommand> command = std::make_unique<Command>(node->GetId(), value, id_, type);     \
                transaction->AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId()); \
                if (node->NeedForcedSendToRemote()) {                                                                \
                    std::unique_ptr<RSCommand> commandForRemote =                                                    \
                        std::make_unique<Command>(node->GetId(), value, id_, type);                                  \
                    transaction->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());           \
                }                                                                                                    \
            } while (0);                                                                                             \
        }                                                                                                            \
    }

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
void RSProperty<std::vector<float>>::UpdateToRender(const std::vector<float>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyComplexShaderParam, value, type);
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
void RSProperty<RSWaterRipplePara>::UpdateToRender(
    const RSWaterRipplePara& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyWaterRipple, value, type);
}
template<>
void RSProperty<RSFlyOutPara>::UpdateToRender(
    const RSFlyOutPara& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyFlyOut, value, type);
}
template<>
void RSProperty<std::shared_ptr<MotionBlurParam>>::UpdateToRender(
    const std::shared_ptr<MotionBlurParam>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyMotionBlurPara, value, type);
}
template<>
void RSProperty<std::shared_ptr<RSMagnifierParams>>::UpdateToRender(
    const std::shared_ptr<RSMagnifierParams>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyMagnifierPara, value, type);
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
void RSProperty<Vector3f>::UpdateToRender(const Vector3f& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyVector3f, value, type);
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
void RSProperty<std::shared_ptr<RSUIFilter>>::UpdateToRender(
    const std::shared_ptr<RSUIFilter>& value, PropertyUpdateType type) const
{
    auto rsRenderFilter = value->GetRSRenderFilter();
    UPDATE_TO_RENDER(RSUpdatePropertyUIFilter, rsRenderFilter, type);
}

template<>
void RSProperty<std::shared_ptr<RSNGFilterBase>>::UpdateToRender(
    const std::shared_ptr<RSNGFilterBase>& value, PropertyUpdateType type) const
{
    UPDATE_TO_RENDER(RSUpdatePropertyNGFilterBase, value->GetRenderEffect(), type);
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

#define DECLARE_PROPERTY(T, TYPE_ENUM) template class RSProperty<T>
#define DECLARE_ANIMATABLE_PROPERTY(T, TYPE_ENUM) \
    template class RSAnimatableProperty<T>;       \
    template class RSProperty<T>

#include "modifier/rs_property_def.in"
#undef DECLARE_PROPERTY
#undef DECLARE_ANIMATABLE_PROPERTY
} // namespace Rosen
} // namespace OHOS
