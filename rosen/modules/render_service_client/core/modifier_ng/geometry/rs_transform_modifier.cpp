/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/geometry/rs_transform_modifier.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen::ModifierNG {
void RSTransformModifier::SetSandBox(std::optional<Vector2f> parentPosition)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::SANDBOX, parentPosition);
}

std::optional<Vector2f> RSTransformModifier::GetSandBox() const
{
    return GetterOptional<Vector2f>(RSPropertyType::SANDBOX);
}

void RSTransformModifier::SetPositionZ(float positionZ)
{
    Setter(RSPropertyType::POSITION_Z, positionZ);
}

float RSTransformModifier::GetPositionZ() const
{
    return Getter(RSPropertyType::POSITION_Z, 0.f);
}

void RSTransformModifier::SetPositionZApplicableCamera3D(bool isApplicable)
{
    Setter<RSProperty>(RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D, isApplicable);
}

bool RSTransformModifier::GetPositionZApplicableCamera3D() const
{
    return Getter(RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D, true);
}

void RSTransformModifier::SetPivot(const Vector2f& pivot, bool animatable)
{
    if (animatable) {
        Setter(RSPropertyType::PIVOT, pivot);
    } else {
        Setter<RSProperty>(RSPropertyType::PIVOT, pivot);
    }
}

Vector2f RSTransformModifier::GetPivot() const
{
    return Getter(RSPropertyType::PIVOT, Vector2f { 0.5f, 0.5f });
}

void RSTransformModifier::SetPivotZ(float pivotZ)
{
    Setter(RSPropertyType::PIVOT_Z, pivotZ);
}

float RSTransformModifier::GetPivotZ() const
{
    return Getter(RSPropertyType::PIVOT_Z, 0.f);
}

void RSTransformModifier::SetQuaternion(const Quaternion& quaternion)
{
    Setter(RSPropertyType::QUATERNION, quaternion);
}

Quaternion RSTransformModifier::GetQuaternion() const
{
    return Getter(RSPropertyType::QUATERNION, Quaternion());
}

void RSTransformModifier::SetRotation(float degree)
{
    Setter(RSPropertyType::ROTATION, degree);
}

float RSTransformModifier::GetRotation() const
{
    return Getter(RSPropertyType::ROTATION, 0.f);
}

void RSTransformModifier::SetRotationX(float degree)
{
    Setter(RSPropertyType::ROTATION_X, degree);
}

float RSTransformModifier::GetRotationX() const
{
    return Getter(RSPropertyType::ROTATION_X, 0.f);
}

void RSTransformModifier::SetRotationY(float degree)
{
    Setter(RSPropertyType::ROTATION_Y, degree);
}

float RSTransformModifier::GetRotationY() const
{
    return Getter(RSPropertyType::ROTATION_Y, 0.f);
}

void RSTransformModifier::SetCameraDistance(float cameraDistance)
{
    Setter(RSPropertyType::CAMERA_DISTANCE, cameraDistance);
}

float RSTransformModifier::GetCameraDistance() const
{
    return Getter(RSPropertyType::CAMERA_DISTANCE, 0.f);
}

void RSTransformModifier::SetTranslate(const Vector2f& translate)
{
    Setter(RSPropertyType::TRANSLATE, translate);
}

Vector2f RSTransformModifier::GetTranslate() const
{
    return Getter(RSPropertyType::TRANSLATE, Vector2f());
}

void RSTransformModifier::SetTranslateZ(float translate)
{
    Setter(RSPropertyType::TRANSLATE_Z, translate);
}

float RSTransformModifier::GetTranslateZ() const
{
    return Getter(RSPropertyType::TRANSLATE_Z, 0.f);
}

void RSTransformModifier::SetScale(const Vector2f& scale)
{
    Setter(RSPropertyType::SCALE, scale);
}

Vector2f RSTransformModifier::GetScale() const
{
    return Getter(RSPropertyType::SCALE, Vector2f());
}

void RSTransformModifier::SetScaleZ(float scaleZ)
{
    Setter(RSPropertyType::SCALE_Z, scaleZ);
}

float RSTransformModifier::GetScaleZ() const
{
    return Getter(RSPropertyType::SCALE_Z, 1.f);
}

void RSTransformModifier::SetSkew(const Vector3f& skew)
{
    Setter(RSPropertyType::SKEW, skew);
}

Vector3f RSTransformModifier::GetSkew() const
{
    return Getter(RSPropertyType::SKEW, Vector3f());
}

void RSTransformModifier::SetPersp(const Vector4f& persp)
{
    Setter(RSPropertyType::PERSP, persp);
}

Vector4f RSTransformModifier::GetPersp() const
{
    return Getter(RSPropertyType::PERSP, Vector4f());
}

void RSTransformModifier::ApplyGeometry(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    if (geometry == nullptr) {
        RS_LOGE("RSTransformModifier::ApplyGeometry, geometry null");
        return;
    }
    if (auto pivotPtr = GetProperty(RSPropertyType::PIVOT)) {
        auto pivot = GetterWithoutCheck<Vector2f>(pivotPtr);
        geometry->SetPivot(pivot.x_, pivot.y_);
    }
    if (auto pivotZPtr = GetProperty(RSPropertyType::PIVOT_Z)) {
        geometry->SetPivotZ(GetterWithoutCheck<float>(pivotZPtr));
    }
    if (auto quaternionPtr = GetProperty(RSPropertyType::QUATERNION)) {
        geometry->SetQuaternion(GetterWithoutCheck<Quaternion>(quaternionPtr));
    }
    if (auto rotationPtr = GetProperty(RSPropertyType::ROTATION)) {
        geometry->SetRotation(GetterWithoutCheck<float>(rotationPtr) + geometry->GetRotation());
    }
    if (auto rotationXPtr = GetProperty(RSPropertyType::ROTATION_X)) {
        geometry->SetRotationX(GetterWithoutCheck<float>(rotationXPtr) + geometry->GetRotationX());
    }
    if (auto rotationYPtr = GetProperty(RSPropertyType::ROTATION_Y)) {
        geometry->SetRotationY(GetterWithoutCheck<float>(rotationYPtr) + geometry->GetRotationY());
    }
    if (auto cameraDistancePtr = GetProperty(RSPropertyType::CAMERA_DISTANCE)) {
        geometry->SetCameraDistance(GetterWithoutCheck<float>(cameraDistancePtr));
    }
    if (auto scalePtr = GetProperty(RSPropertyType::SCALE)) {
        auto scale = GetterWithoutCheck<Vector2f>(scalePtr);
        scale *= Vector2f(geometry->GetScaleX(), geometry->GetScaleY());
        geometry->SetScale(scale.x_, scale.y_);
    }
    if (auto scaleZPtr = GetProperty(RSPropertyType::SCALE_Z)) {
        geometry->SetScaleZ(GetterWithoutCheck<float>(scaleZPtr) * geometry->GetScaleZ());
    }
    if (auto skewPtr = GetProperty(RSPropertyType::SKEW)) {
        auto skew = GetterWithoutCheck<Vector3f>(skewPtr);
        skew += Vector3f(geometry->GetSkewX(), geometry->GetSkewY(), geometry->GetSkewZ());
        geometry->SetSkew(skew.x_, skew.y_, skew.z_);
    }
    if (auto perspPtr = GetProperty(RSPropertyType::PERSP)) {
        auto persp = GetterWithoutCheck<Vector4f>(perspPtr);
        geometry->SetPersp(persp.x_, persp.y_, persp.z_, persp.w_);
    }
    if (auto translatePtr = GetProperty(RSPropertyType::TRANSLATE)) {
        auto translate = GetterWithoutCheck<Vector2f>(translatePtr);
        translate += Vector2f(geometry->GetTranslateX(), geometry->GetTranslateY());
        geometry->SetTranslateX(translate.x_);
        geometry->SetTranslateY(translate.y_);
    }
    if (auto translateZPtr = GetProperty(RSPropertyType::TRANSLATE_Z)) {
        geometry->SetTranslateZ(GetterWithoutCheck<float>(translateZPtr) + geometry->GetTranslateZ());
    }
}
} // namespace OHOS::Rosen::ModifierNG
