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

#include "modifier_ng/rs_transition_render_modifier.h"

#include "property/rs_properties.h"
#include "utils/camera3d.h"

namespace OHOS::Rosen::ModifierNG {
// constexpr unsigned RECT_POINT_NUM = 4;
// constexpr unsigned LEFT_TOP_POINT = 0;
// constexpr unsigned RIGHT_TOP_POINT = 1;
// constexpr unsigned RIGHT_BOTTOM_POINT = 2;
// constexpr unsigned LEFT_BOTTOM_POINT = 3;
constexpr float INCH_TO_PIXEL = 72;
constexpr float EPSILON = 1e-6f;
constexpr float DEGREE_TO_RADIAN = M_PI / 180;

const RSTransformRenderModifier::LegacyPropertyApplierMap RSTransformRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::SANDBOX,
        RSRenderModifier::PropertyApplyHelper<std::optional<Vector2f>, &RSProperties::SetSandBox> },
    { RSPropertyType::POSITION_Z,
        RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetPositionZ, &RSProperties::GetPositionZ> },
    { RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D,
        RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetPositionZApplicableCamera3D> },
    { RSPropertyType::PIVOT, RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetPivot> },
    { RSPropertyType::PIVOT_Z, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetPivotZ> },
    { RSPropertyType::CORNER_RADIUS, RSRenderModifier::PropertyApplyHelperAdd<Vector4f, &RSProperties::SetCornerRadius,
                                         &RSProperties::GetCornerRadius> },
    { RSPropertyType::QUATERNION, RSRenderModifier::PropertyApplyHelper<Quaternion, &RSProperties::SetQuaternion> },
    { RSPropertyType::ROTATION,
        RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetRotation, &RSProperties::GetRotation> },
    { RSPropertyType::ROTATION_X,
        RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetRotationX, &RSProperties::GetRotationX> },
    { RSPropertyType::ROTATION_Y,
        RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetRotationY, &RSProperties::GetRotationY> },
    { RSPropertyType::CAMERA_DISTANCE, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetCameraDistance> },
    { RSPropertyType::TRANSLATE,
        PropertyApplyHelperAdd<Vector2f, &RSProperties::SetTranslate, &RSProperties::GetTranslate> },
    { RSPropertyType::TRANSLATE_Z,
        PropertyApplyHelperAdd<float, &RSProperties::SetTranslateZ, &RSProperties::GetTranslateZ> },
    { RSPropertyType::SCALE,
        RSRenderModifier::PropertyApplyHelperMultiply<Vector2f, &RSProperties::SetScale, &RSProperties::GetScale> },
    { RSPropertyType::SCALE_Z,
        RSRenderModifier::PropertyApplyHelperMultiply<float, &RSProperties::SetScaleZ, &RSProperties::GetScaleZ> },
    { RSPropertyType::SKEW,
        RSRenderModifier::PropertyApplyHelperAdd<Vector3f, &RSProperties::SetSkew, &RSProperties::GetSkew> },
    { RSPropertyType::PERSP, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetPersp> },
};

void RSTransformRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.ResetSandBox();
    properties.SetPositionZ(0.f);
    properties.SetPositionZApplicableCamera3D(true);
    properties.SetPivot(Vector2f(0.5f, 0.5f));
    properties.SetPivotZ(0.f);
    properties.SetCornerRadius(0.f);
    properties.SetQuaternion(Quaternion());
    properties.SetRotation(0.f);
    properties.SetRotationX(0.f);
    properties.SetRotationY(0.f);
    properties.SetCameraDistance(0.f);
    properties.SetTranslate(Vector2f(0.f, 0.f));
    properties.SetTranslateZ(0.f);
    properties.SetScale(Vector2f(1.f, 1.f));
    properties.SetScaleZ(1.f);
    properties.SetSkew({ 0.f, 0.f, 0.f });
    properties.SetPersp({ 0.f, 0.f, 0.f, 1.f });
}

bool RSTransformRenderModifier::OnApply(RSModifierContext& context)
{
    stagingMatrix_.Reset();
    if ((HasProperty(RSPropertyType::TRANSLATE_Z) && !ROSEN_EQ(Getter<float>(RSPropertyType::TRANSLATE_Z), 0.0f)) ||
        (HasProperty(RSPropertyType::ROTATION_X) && !ROSEN_EQ(Getter<float>(RSPropertyType::ROTATION_X), 0.0f)) ||
        (HasProperty(RSPropertyType::ROTATION_Y) && !ROSEN_EQ(Getter<float>(RSPropertyType::ROTATION_Y), 0.0f)) ||
        (HasProperty(RSPropertyType::QUATERNION) && !Getter<Quaternion>(RSPropertyType::QUATERNION).IsIdentity())) {
        UpdateMatrix3D(context);
    } else {
        UpdateMatrix2D(context);
    }
    return !stagingMatrix_.IsIdentity();
}

void RSTransformRenderModifier::OnSync()
{
    std::swap(stagingMatrix_, renderMatrix_);
}

void RSTransformRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    // canvas.SetMatrix(renderMatrix_);
    canvas.ConcatMatrix(renderMatrix_);
}

void RSTransformRenderModifier::UpdateMatrix2D(RSModifierContext& context)
{
    const auto& bounds = context.bounds_;
    auto pivot = Getter<Vector2f>(RSPropertyType::PIVOT, Vector2f(0.5f, 0.5f));
    auto mappedPivot = Vector2f(pivot.x_ * bounds.GetWidth(), pivot.y_ * bounds.GetHeight());

    stagingMatrix_.PreTranslate(bounds.GetLeft(), bounds.GetTop());
    if (HasProperty(RSPropertyType::TRANSLATE)) {
        auto translate = Getter<Vector2f>(RSPropertyType::TRANSLATE);
        stagingMatrix_.PreTranslate(translate.x_, translate.y_);
    }
    if (HasProperty(RSPropertyType::PERSP)) {
        auto persp = Getter<Vector3f>(RSPropertyType::PERSP);
        stagingMatrix_.PreTranslate(mappedPivot.x_, mappedPivot.y_);
        // ApplyPerspToMatrix
        stagingMatrix_.PreTranslate(-mappedPivot.x_, -mappedPivot.y_);
    }
    if (HasProperty(RSPropertyType::ROTATION)) {
        auto rotation = Getter<float>(RSPropertyType::ROTATION);
        stagingMatrix_.PreRotate(rotation, mappedPivot.x_, mappedPivot.y_);
    }
    if (HasProperty(RSPropertyType::SKEW)) {
        auto skew = Getter<Vector2f>(RSPropertyType::SKEW);
        stagingMatrix_.PreSkew(skew.x_, skew.y_, mappedPivot.x_, mappedPivot.y_);
    }
    if (HasProperty(RSPropertyType::SCALE)) {
        auto scale = Getter<Vector2f>(RSPropertyType::SCALE);
        stagingMatrix_.PreScale(scale.x_, scale.y_, mappedPivot.x_, mappedPivot.y_);
    }
}

void RSTransformRenderModifier::UpdateMatrix3D(RSModifierContext& context)
{
    const auto& bounds = context.bounds_;
    auto pivot = Getter<Vector2f>(RSPropertyType::PIVOT, Vector2f(0.5f, 0.5f));
    auto mappedPivot = Vector2f(pivot.x_ * bounds.GetWidth(), pivot.y_ * bounds.GetHeight());

    Drawing::Matrix44 matrix3D;
    auto quaternion = Getter<Quaternion>(RSPropertyType::QUATERNION);

    // Pivot
    matrix3D.Translate(mappedPivot.x_, mappedPivot.y_, 0.0f);

    // Persp
    // ApplyPerspToMatrix44

    // Translate
    auto isZApplicableCamera3D = Getter<bool>(RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D, true);
    auto positionZ = isZApplicableCamera3D ? Getter<float>(RSPropertyType::TRANSLATE_Z, 0.0f) : 0.0f;
    auto translate = Getter<Vector2f>(RSPropertyType::TRANSLATE, Vector2f(0.0f, 0.0f));
    auto translateZ = Getter<float>(RSPropertyType::TRANSLATE_Z, 0.0f);
    matrix3D.Translate(bounds.GetLeft() + translate.x_, bounds.GetRight() + translate.y_, positionZ + translateZ);

    // Rotate
    if (HasProperty(RSPropertyType::QUATERNION)) {
        matrix3D = matrix3D * RotateByQuaternion();
    } else if (HasProperty(RSPropertyType::ROTATION) || HasProperty(RSPropertyType::ROTATION_X) ||
               HasProperty(RSPropertyType::ROTATION_Y)) {
        // matrix3D = matrix3D * RotateByEulerAngle(context);
    }

    // Skew
    // ApplySkewToMatrix44(trans_.value(), matrix3D, false);

    // Scale
    if (HasProperty(RSPropertyType::SCALE)) {
        auto scale = Getter<Vector2f>(RSPropertyType::SCALE, Vector2f(1.0f, 1.0f));
        matrix3D.PreScale(scale.x_, scale.y_, 1.0f);
    }

    // Pivot
    matrix3D.Translate(-mappedPivot.x_, -mappedPivot.y_, 0.0f);

    // Concatenate the 3D matrix with the 2D matrix
    stagingMatrix_.PreConcat(matrix3D);
}

Drawing::Matrix RSTransformRenderModifier::RotateByEulerAngle(RSModifierContext& context) const
{
    auto rotationZ = Getter<float>(RSPropertyType::ROTATION, 0.0f);
    auto rotationX = Getter<float>(RSPropertyType::ROTATION_X, 0.0f);
    auto rotationY = Getter<float>(RSPropertyType::ROTATION_Y, 0.0f);
    if (ROSEN_EQ(rotationZ, 0.0f) && ROSEN_EQ(rotationX, 0.0f) && ROSEN_EQ(rotationY, 0.0f)) {
        return {};
    }

    Drawing::Camera3D camera;
    Drawing::Matrix matrix3D;

    // Set camera distance
    auto cameraDistance = Getter<float>(RSPropertyType::CAMERA_DISTANCE, 0.0f);
    if (ROSEN_EQ(cameraDistance, 0.0f)) {
        const auto& bounds = context.bounds_;
        float zOffSet = sqrt(bounds.GetHeight() * bounds.GetHeight() + bounds.GetWidth() * bounds.GetWidth()) / 2;
        camera.SetCameraPos(0, 0, camera.GetCameraPosZ() - zOffSet / INCH_TO_PIXEL);
    } else {
        camera.SetCameraPos(0, 0, cameraDistance);
    }

    auto pivotZ = Getter<float>(RSPropertyType::PIVOT_Z, 0.0f);
    // Rotate
    if (!ROSEN_EQ(pivotZ, 0.f, EPSILON)) {
        camera.Translate(-sin(rotationY * DEGREE_TO_RADIAN) * pivotZ, -sin(rotationX * DEGREE_TO_RADIAN) * pivotZ,
            (1 - cos(rotationX * DEGREE_TO_RADIAN) * cos(rotationY * DEGREE_TO_RADIAN)) * pivotZ);
    }

    camera.RotateXDegrees(rotationX);
    camera.RotateYDegrees(rotationY);
    camera.RotateZDegrees(rotationZ);
    camera.ApplyToMatrix(matrix3D);

    return matrix3D;
}

Drawing::Matrix44 RSTransformRenderModifier::RotateByQuaternion() const
{
    auto quaternion = Getter<Quaternion>(RSPropertyType::QUATERNION);
    if (quaternion.IsIdentity()) {
        return {};
    }
    // Rotate
    float x = quaternion[0];
    float y = quaternion[1];
    float z = quaternion[2];
    float w = quaternion[3];
    Drawing::Matrix44::Buffer buffer = { 1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w),
        0,                                                                            // m00 ~ m30
        2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0, // m01 ~ m31
        2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0, // m02 ~ m32
        0, 0, 0, 1 };                                                                 // m03 ~ m33
    Drawing::Matrix44 matrix4;
    matrix4.SetMatrix44ColMajor(buffer);

    return matrix4;
}
} // namespace OHOS::Rosen::ModifierNG
