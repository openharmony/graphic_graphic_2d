/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_SPATIAL_POINT_LIGHT_EFFECT_PARA_H
#define UIEFFECT_EFFECT_SPATIAL_POINT_LIGHT_EFFECT_PARA_H

#include "visual_effect_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class SpatialPointLightEffectPara : public VisualEffectPara {
public:
    SpatialPointLightEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::SPATIAL_POINT_LIGHT_EFFECT;
    }
    ~SpatialPointLightEffectPara() override = default;

    void SetLightIntensity(const float lightIntensity)
    {
        lightIntensity_ = lightIntensity;
    }

    const float& GetLightIntensity() const
    {
        return lightIntensity_;
    }

    void SetLightPosition(const Vector3f& lightPosition)
    {
        lightPosition_ = lightPosition;
    }

    const Vector3f& GetLightPosition() const
    {
        return lightPosition_;
    }

    void SetAttenuation(const float attenuation)
    {
        attenuation_ = attenuation;
    }

    const float& GetAttenuation() const
    {
        return attenuation_;
    }

    void SetLightColor(const Vector4f& lightColor)
    {
        lightColor_ = lightColor;
    }

    const Vector4f& GetLightColor() const
    {
        return lightColor_;
    }

    void SetMask(std::shared_ptr<MaskPara> mask)
    {
        maskPara_ = mask;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

private:
    float lightIntensity_ = 1.0f;
    Vector3f lightPosition_ = {0.0f, 0.0f, 1.0f};
    float attenuation_ = 0.5f;
    Vector4f lightColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_SPATIAL_POINT_LIGHT_EFFECT_PARA_H
