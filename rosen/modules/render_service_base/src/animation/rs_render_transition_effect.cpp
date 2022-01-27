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
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
RSRenderTransitionEffect* RSRenderTransitionEffect::Unmarshalling(Parcel& parcel)
{
    uint16_t transitionType;
    if (!parcel.ReadUint16(transitionType)) {
        ROSEN_LOGE("RSRenderTransitionEffect::Unmarshalling, ParseParam Failed");
        return nullptr;
    }
    RSRenderTransitionEffect* ret = nullptr;
    switch (transitionType) {
        case RSTransitionEffectType::FADE:
            ret = RSTransitionFade::Unmarshalling(parcel);
            break;
        case RSTransitionEffectType::SCALE:
            ret = RSTransitionScale::Unmarshalling(parcel);
            break;
        case RSTransitionEffectType::ROTATE:
            ret = RSTransitionRotate::Unmarshalling(parcel);
            break;
        case RSTransitionEffectType::TRANSLATE:
            ret = RSTransitionTranslate::Unmarshalling(parcel);
            break;
        default:
            return nullptr;
    }
    return ret;
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
           parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_) && parcel.WriteFloat(pivotX_) &&
           parcel.WriteFloat(pivotY_);
}

RSRenderTransitionEffect* RSTransitionScale::Unmarshalling(Parcel& parcel)
{
    float scaleX;
    float scaleY;
    float scaleZ;
    float pivotX;
    float pivotY;
    if (!parcel.ReadFloat(scaleX) || !parcel.ReadFloat(scaleY) || !parcel.ReadFloat(scaleZ) ||
        !parcel.ReadFloat(pivotX) || !parcel.ReadFloat(pivotY)) {
        ROSEN_LOGE("RSTransitionScale::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionScale(scaleX, scaleY, scaleZ, pivotX, pivotY);
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
           parcel.WriteFloat(dz_) && parcel.WriteFloat(angle_) && parcel.WriteFloat(pivotX_) &&
           parcel.WriteFloat(pivotY_);
}

RSRenderTransitionEffect* RSTransitionRotate::Unmarshalling(Parcel& parcel)
{
    Quaternion quaternion;
    float dx;
    float dy;
    float dz;
    float angle;
    float pivotX;
    float pivotY;
    if (!parcel.ReadFloat(dx) || !parcel.ReadFloat(dy) || !parcel.ReadFloat(dz) || !parcel.ReadFloat(angle) ||
        !parcel.ReadFloat(pivotX) || !parcel.ReadFloat(pivotY)) {
        ROSEN_LOGE("RSTransitionRotate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionRotate(dx, dy, dz, angle, pivotX, pivotY);
}
#endif

void RSTransitionFade::OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    canvas.MultiplyAlpha(1.0f - fraction);
#endif
}

void RSTransitionScale::OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(1.0f, 1.0f);
    Vector2f endValue(scaleX_, scaleY_);
    auto value = RSValueEstimator::Estimate(fraction, startValue, endValue);
    SkMatrix matrix;

    Vector2f pivot { renderProperties.GetBoundsWidth() * pivotX_, renderProperties.GetBoundsHeight() * pivotY_ };
    matrix.setScale(value.x_, value.y_, pivot.x_, pivot.y_);

    canvas.concat(matrix);
#endif
}

void RSTransitionTranslate::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(0.0f, 0.0f);
    Vector2f endValue(translateX_, translateY_);
    auto value = RSValueEstimator::Estimate(fraction, startValue, endValue);
    canvas.translate(value.x_, value.y_);
#endif
}

void RSTransitionRotate::OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    auto angle = angle_ * fraction;
    auto rotateMatrix = SkMatrix44::I();
    rotateMatrix.setRotateDegreesAbout(dx_, dy_, dz_, angle);

    Vector2f pivot { renderProperties.GetBoundsWidth() * pivotX_, renderProperties.GetBoundsHeight() * pivotY_ };

    canvas.translate(pivot.x_, pivot.y_);
    canvas.concat(rotateMatrix);
    canvas.translate(-pivot.x_, -pivot.y_);
#endif
}
} // namespace Rosen
} // namespace OHOS
