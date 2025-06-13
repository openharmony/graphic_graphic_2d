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

#include "modifier_ng/geometry/rs_transform_render_modifier.h"

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSTransformRenderModifier::LegacyPropertyApplierMap RSTransformRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::SANDBOX, RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetSandBox> },
    { RSPropertyType::POSITION_Z,
        RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetPositionZ, &RSProperties::GetPositionZ> },
    { RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D,
        RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetPositionZApplicableCamera3D> },
    { RSPropertyType::PIVOT, RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetPivot> },
    { RSPropertyType::PIVOT_Z, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetPivotZ> },
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
    properties.SetPositionZApplicableCamera3D(false);
    properties.SetPivot(Vector2f(0.5f, 0.5f));
    properties.SetPivotZ(0.f);
    properties.SetQuaternion(Quaternion());
    properties.SetRotation(0.f);
    properties.SetRotationX(0.f);
    properties.SetRotationY(0.f);
    properties.SetCameraDistance(0.f);
    properties.SetTranslate(Vector2f());
    properties.SetTranslateZ(0.f);
    properties.SetScale(Vector2f(1.f, 1.f));
    properties.SetScaleZ(1.f);
    properties.SetSkew(Vector3f());
    properties.SetPersp(Vector4f(0.f, 0.f, 0.f, 1.f));
}

void RSTransformRenderModifier::OnSetDirty()
{
    if (!HasProperty(RSPropertyType::POSITION_Z)) {
        return;
    }
    if (auto node = target_.lock()) {
        node->MarkNonGeometryChanged();
        node->MarkParentNeedRegenerateChildren();
    }
}
} // namespace OHOS::Rosen::ModifierNG
