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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_TRANSITION_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_TRANSITION_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSTransformModifier : public RSModifier {
public:
    RSTransformModifier() = default;
    ~RSTransformModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::TRANSFORM;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetSandBox(std::optional<Vector2f> parentPosition);
    void SetPositionZ(float positionZ);
    void SetPositionZApplicableCamera3D(bool isApplicable);
    void SetPivot(const Vector2f& pivot);
    void SetPivotZ(float pivotZ);
    std::optional<Vector2f> GetSandBox() const;
    float GetPositionZ() const;
    bool GetPositionZApplicableCamera3D() const;
    Vector2f GetPivot() const;
    float GetPivotZ() const;

    void SetCornerRadius(const Vector4f& cornerRadius);
    Vector4f GetCornerRadius() const;

    void SetQuaternion(const Quaternion& quaternion);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);
    Quaternion GetQuaternion() const;
    float GetRotation() const;
    float GetRotationX() const;
    float GetRotationY() const;

    void SetCameraDistance(float cameraDistance);
    void SetTranslate(const Vector2f& translate);
    void SetTranslateZ(float translate);
    float GetCameraDistance() const;
    Vector2f GetTranslate() const;
    float GetTranslateZ() const;

    void SetScale(const Vector2f& scale);
    void SetScaleZ(float scaleZ);
    void SetSkew(const Vector3f& skew);
    void SetPersp(const Vector4f& persp);
    Vector2f GetScale() const;
    float GetScaleZ() const;
    Vector3f GetSkew() const;
    Vector4f GetPersp() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_TRANSITION_MODIFIER_H
