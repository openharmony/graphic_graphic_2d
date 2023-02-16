/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_transition_effect.h"

#include <climits>

#include "animation/rs_animation_common.h"
#include "animation/rs_value_estimator.h"
#include "modifier/rs_render_modifier.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};
constexpr int PID_SHIFT = 32;

PropertyId GenerateTransitionPropertyId()
{
    // manually set pid to INT_MAX to avoid conflict with other process (note: valid pid is smaller than 2^22)
    static pid_t pid_ = INT_MAX;
    static std::atomic<uint32_t> currentId_ = 1;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Property Id overflow");
    }

    return ((PropertyId)pid_ << PID_SHIFT) | currentId;
}
} // namespace

RSRenderTransitionEffect* RSRenderTransitionEffect::Unmarshalling(Parcel& parcel)
{
    uint16_t transitionType = 0;
    if (!parcel.ReadUint16(transitionType)) {
        ROSEN_LOGE("RSRenderTransitionEffect::Unmarshalling, ParseParam Failed");
        return nullptr;
    }
    switch (transitionType) {
        case RSTransitionEffectType::FADE:
            return RSTransitionFade::Unmarshalling(parcel);
        case RSTransitionEffectType::SCALE:
            return RSTransitionScale::Unmarshalling(parcel);
        case RSTransitionEffectType::TRANSLATE:
            return RSTransitionTranslate::Unmarshalling(parcel);
        case RSTransitionEffectType::ROTATE:
            return RSTransitionRotate::Unmarshalling(parcel);
        default:
            return nullptr;
    }
}

const std::shared_ptr<RSRenderModifier>& RSRenderTransitionEffect::GetModifier()
{
    if (modifier_ == nullptr) {
        modifier_ = CreateModifier();
    }
    return modifier_;
}

bool RSTransitionFade::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::FADE) && parcel.WriteFloat(alpha_);
}

RSRenderTransitionEffect* RSTransitionFade::Unmarshalling(Parcel& parcel)
{
    float alpha;
    if (!RSMarshallingHelper::Unmarshalling(parcel, alpha)) {
        ROSEN_LOGE("RSTransitionFade::Unmarshalling, unmarshalling alpha failed");
        return nullptr;
    }
    return new RSTransitionFade(alpha);
}

bool RSTransitionScale::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::SCALE) && parcel.WriteFloat(scaleX_) &&
           parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_);
}

RSRenderTransitionEffect* RSTransitionScale::Unmarshalling(Parcel& parcel)
{
    float scaleX;
    float scaleY;
    float scaleZ;
    if (!parcel.ReadFloat(scaleX) || !parcel.ReadFloat(scaleY) || !parcel.ReadFloat(scaleZ)) {
        ROSEN_LOGE("RSTransitionScale::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionScale(scaleX, scaleY, scaleZ);
}

bool RSTransitionTranslate::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::TRANSLATE) && parcel.WriteFloat(translateX_) &&
           parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
}

RSRenderTransitionEffect* RSTransitionTranslate::Unmarshalling(Parcel& parcel)
{
    float translateX;
    float translateY;
    float translateZ;
    if (!parcel.ReadFloat(translateX) || !parcel.ReadFloat(translateY) || !parcel.ReadFloat(translateZ)) {
        ROSEN_LOGE("RSTransitionTranslate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionTranslate(translateX, translateY, translateZ);
}

bool RSTransitionRotate::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::ROTATE) && parcel.WriteFloat(dx_) && parcel.WriteFloat(dy_) &&
           parcel.WriteFloat(dz_) && parcel.WriteFloat(radian_);
}

RSRenderTransitionEffect* RSTransitionRotate::Unmarshalling(Parcel& parcel)
{
    Quaternion quaternion;
    float dx;
    float dy;
    float dz;
    float radian;
    if (!parcel.ReadFloat(dx) || !parcel.ReadFloat(dy) || !parcel.ReadFloat(dz) || !parcel.ReadFloat(radian)) {
        ROSEN_LOGE("RSTransitionRotate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionRotate(dx, dy, dz, radian);
}

const std::shared_ptr<RSRenderModifier> RSTransitionFade::CreateModifier()
{
    property_ = std::make_shared<RSRenderAnimatableProperty<float>>(0, GenerateTransitionPropertyId());
    return std::make_shared<RSAlphaRenderModifier>(property_);
}

void RSTransitionFade::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    float startValue(1.0f);
    float endValue(alpha_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionScale::CreateModifier()
{
    property_ =
        std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f { 0, 0 }, GenerateTransitionPropertyId());
    return std::make_shared<RSScaleRenderModifier>(property_);
}

void RSTransitionScale::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(1.0f, 1.0f);
    Vector2f endValue(scaleX_, scaleY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionTranslate::CreateModifier()
{
    property_ =
        std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f { 0, 0 }, GenerateTransitionPropertyId());
    return std::make_shared<RSTranslateRenderModifier>(property_);
}

void RSTransitionTranslate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(0.0f, 0.0f);
    Vector2f endValue(translateX_, translateY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionRotate::CreateModifier()
{
    property_ = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion {}, GenerateTransitionPropertyId());
    return std::make_shared<RSQuaternionRenderModifier>(property_);
}

void RSTransitionRotate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    auto radian = radian_ * fraction;
    float factor = std::sin(radian / 2);
    float qx = dx_ * factor;
    float qy = dy_ * factor;
    float qz = dz_ * factor;
    float qw = std::cos(radian / 2);
    Quaternion value(qx, qy, qz, qw);
    property_->Set(value);
}
} // namespace Rosen
} // namespace OHOS
