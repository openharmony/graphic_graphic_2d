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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H

#include "common/rs_common_def.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

namespace OHOS {
namespace Rosen {
class RSNode;

class RSC_EXPORT RSModifierExtractor {
public:
    RSModifierExtractor(NodeId id);
    virtual ~RSModifierExtractor() = default;

    Vector4f GetBounds() const;
    Vector4f GetFrame() const;
    float GetPositionZ() const;

    Vector2f GetPivot() const;
    float GetPivotZ() const;
    Quaternion GetQuaternion() const;
    float GetRotation() const;
    float GetRotationX() const;
    float GetRotationY() const;
    float GetCameraDistance() const;
    Vector2f GetTranslate() const;
    float GetTranslateZ() const;
    Vector2f GetScale() const;
    Vector2f GetSkew() const;

    float GetAlpha() const;
    bool GetAlphaOffscreen() const;

    Vector4f GetCornerRadius() const;

    Color GetForegroundColor() const;
    Color GetBackgroundColor() const;
    Color GetSurfaceBgColor() const;
    std::shared_ptr<RSShader> GetBackgroundShader() const;
    std::shared_ptr<RSImage> GetBgImage() const;
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;

    Vector4<Color> GetBorderColor() const;
    Vector4f GetBorderWidth() const;
    Vector4<uint32_t> GetBorderStyle() const;
    Vector4<Color> GetOutlineColor() const;
    Vector4f GetOutlineWidth() const;
    Vector4<uint32_t> GetOutlineStyle() const;
    Vector4f GetOutlineRadius() const;

    std::shared_ptr<RSFilter> GetBackgroundFilter() const;
    std::shared_ptr<RSFilter> GetFilter() const;

    Color GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    bool GetShadowMask() const;
    bool GetShadowIsFilled() const;
    int GetShadowColorStrategy() const;

    Gravity GetFrameGravity() const;

    std::shared_ptr<RSPath> GetClipBounds() const;
    bool GetClipToBounds() const;
    bool GetClipToFrame() const;
    bool GetVisible() const;

    std::shared_ptr<RSMask> GetMask() const;
    float GetSpherizeDegree() const;
    float GetLightUpEffectDegree() const;
    float GetDynamicDimDegree() const;

    float GetLightIntensity() const;
    Color GetLightColor() const;
    Vector4f GetLightPosition() const;
    float GetIlluminatedBorderWidth() const;
    int GetIlluminatedType() const;
    float GetBloom() const;

    std::string Dump() const;
private:
    NodeId id_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H
