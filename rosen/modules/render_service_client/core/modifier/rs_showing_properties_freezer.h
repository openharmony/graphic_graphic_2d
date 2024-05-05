/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H

#include "common/rs_common_def.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSNode;

class RSC_EXPORT RSShowingPropertiesFreezer {
public:
    RSShowingPropertiesFreezer(NodeId id);
    virtual ~RSShowingPropertiesFreezer() = default;

    std::optional<Vector4f> GetBounds() const;
    std::optional<Vector4f> GetFrame() const;
    std::optional<float> GetPositionZ() const;

    std::optional<Vector2f> GetPivot() const;
    std::optional<float> GetPivotZ() const;
    std::optional<Quaternion> GetQuaternion() const;
    std::optional<float> GetRotation() const;
    std::optional<float> GetRotationX() const;
    std::optional<float> GetRotationY() const;
    std::optional<float> GetCameraDistance() const;
    std::optional<Vector2f> GetTranslate() const;
    std::optional<float> GetTranslateZ() const;
    std::optional<Vector2f> GetScale() const;
    std::optional<Vector2f> GetSkew() const;
    std::optional<Vector2f> GetPersp() const;

    std::optional<float> GetAlpha() const;
    std::optional<Vector4f> GetCornerRadius() const;

    std::optional<Color> GetForegroundColor() const;
    std::optional<Color> GetBackgroundColor() const;
    std::optional<Color> GetSurfaceBgColor() const;
    std::optional<float> GetBgImageWidth() const;
    std::optional<float> GetBgImageHeight() const;
    std::optional<float> GetBgImagePositionX() const;
    std::optional<float> GetBgImagePositionY() const;

    std::optional<Vector4<Color>> GetBorderColor() const;
    std::optional<Vector4f> GetBorderWidth() const;

    std::optional<std::shared_ptr<RSFilter>> GetBackgroundFilter() const;
    std::optional<std::shared_ptr<RSFilter>> GetFilter() const;

    std::optional<Color> GetShadowColor() const;
    std::optional<float> GetShadowOffsetX() const;
    std::optional<float> GetShadowOffsetY() const;
    std::optional<float> GetShadowAlpha() const;
    std::optional<float> GetShadowElevation() const;
    std::optional<float> GetShadowRadius() const;

    std::optional<float> GetSpherizeDegree() const;
    std::optional<float> GetLightUpEffectDegree() const;
    std::optional<float> GetDynamicDimDegree() const;

private:
    NodeId id_;
    template<typename T, RSModifierType Type>
    std::optional<T> GetPropertyImpl() const;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H
