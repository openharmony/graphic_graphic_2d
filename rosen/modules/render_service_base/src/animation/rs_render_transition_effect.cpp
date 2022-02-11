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

#include "animation/rs_render_transition_effect.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"
#include "property/rs_transition_properties.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
namespace {
enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};
}

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
           parcel.WriteFloat(dz_) && parcel.WriteFloat(angle_);
}

RSRenderTransitionEffect* RSTransitionRotate::Unmarshalling(Parcel& parcel)
{
    Quaternion quaternion;
    float dx;
    float dy;
    float dz;
    float angle;
    if (!parcel.ReadFloat(dx) || !parcel.ReadFloat(dy) || !parcel.ReadFloat(dz) || !parcel.ReadFloat(angle)) {
        ROSEN_LOGE("RSTransitionRotate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionRotate(dx, dy, dz, angle);
}
#endif

void RSTransitionFade::OnTransition(const std::unique_ptr<RSTransitionProperties>& transitionProperties, float fraction)
{
#ifdef ROSEN_OHOS
    transitionProperties->DoAlphaTransition(1.0f - fraction);
#endif
}

void RSTransitionScale::OnTransition(
    const std::unique_ptr<RSTransitionProperties>& transitionProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector3f startValue(1.0f, 1.0f, 1.0f);
    Vector3f endValue(scaleX_, scaleY_, scaleZ_);
    auto value = RSValueEstimator::Estimate(fraction, startValue, endValue);

    transitionProperties->DoScaleTransition(value);
#endif
}

void RSTransitionTranslate::OnTransition(
    const std::unique_ptr<RSTransitionProperties>& transitionProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector3f startValue(0.0f, 0.0f, 0.0f);
    Vector3f endValue(translateX_, translateY_, translateZ_);
    auto value = RSValueEstimator::Estimate(fraction, startValue, endValue);
    transitionProperties->DoTranslateTransition(value);
#endif
}

void RSTransitionRotate::OnTransition(
    const std::unique_ptr<RSTransitionProperties>& transitionProperties, float fraction)
{
#ifdef ROSEN_OHOS
    auto angle = angle_ * fraction;
    auto rotateMatrix = SkMatrix44::I();
    rotateMatrix.setRotateDegreesAbout(dx_, dy_, dz_, angle);

    transitionProperties->DoRotateTransition(rotateMatrix);
#endif
}
} // namespace Rosen
} // namespace OHOS
