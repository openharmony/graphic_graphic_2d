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

    std::tuple<Vector4f, bool> GetBounds() const;
    std::tuple<Vector4f, bool> GetFrame() const;
    std::tuple<float, bool> GetPositionZ() const;

    std::tuple<Vector2f, bool> GetPivot() const;
    std::tuple<float, bool> GetPivotZ() const;
    std::tuple<Quaternion, bool> GetQuaternion() const;
    std::tuple<float, bool> GetRotation() const;
    std::tuple<float, bool> GetRotationX() const;
    std::tuple<float, bool> GetRotationY() const;
    std::tuple<float, bool> GetCameraDistance() const;
    std::tuple<Vector2f, bool> GetTranslate() const;
    std::tuple<float, bool> GetTranslateZ() const;
    std::tuple<Vector2f, bool> GetScale() const;

    std::tuple<float, bool> GetAlpha() const;
    std::tuple<Vector4f, bool> GetCornerRadius() const;

    std::tuple<Color, bool> GetForegroundColor() const;
    std::tuple<Color, bool> GetBackgroundColor() const;
    std::tuple<Color, bool> GetSurfaceBgColor() const;
    std::tuple<float, bool> GetBgImageWidth() const;
    std::tuple<float, bool> GetBgImageHeight() const;
    std::tuple<float, bool> GetBgImagePositionX() const;
    std::tuple<float, bool> GetBgImagePositionY() const;

    std::tuple<Vector4<Color>, bool> GetBorderColor() const;
    std::tuple<Vector4f, bool> GetBorderWidth() const;

    std::tuple<std::shared_ptr<RSFilter>, bool> GetBackgroundFilter() const;
    std::tuple<std::shared_ptr<RSFilter>, bool> GetFilter() const;

    std::tuple<Color, bool> GetShadowColor() const;
    std::tuple<float, bool> GetShadowOffsetX() const;
    std::tuple<float, bool> GetShadowOffsetY() const;
    std::tuple<float, bool> GetShadowAlpha() const;
    std::tuple<float, bool> GetShadowElevation() const;
    std::tuple<float, bool> GetShadowRadius() const;

    std::tuple<float, bool> GetSpherizeDegree() const;
    std::tuple<float, bool> GetLightUpEffectDegree() const;

private:
    NodeId id_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H
