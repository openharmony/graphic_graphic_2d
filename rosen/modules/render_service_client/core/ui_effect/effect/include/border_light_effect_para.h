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
#ifndef UIEFFECT_EFFECT_BORDER_LIGHT_EFFECT_PARA_H
#define UIEFFECT_EFFECT_BORDER_LIGHT_EFFECT_PARA_H

#include "visual_effect_para.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class BorderLightEffectPara : public VisualEffectPara {
public:
    BorderLightEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::BORDER_LIGHT_EFFECT;
    }
    ~BorderLightEffectPara() override = default;

    void SetLightPosition(const Vector3f& lightPosition)
    {
        lightPosition_ = lightPosition;
    }

    const Vector3f& GetLightPosition() const
    {
        return lightPosition_;
    }

    void SetLightColor(const Vector4f& lightColor)
    {
        lightColor_ = lightColor;
    }

    const Vector4f& GetLightColor() const
    {
        return lightColor_;
    }

    void SetLightIntensity(const float lightIntensity)
    {
        lightIntensity_ = lightIntensity;
    }

    const float& GetLightIntensity() const
    {
        return lightIntensity_;
    }

    void SetLightWidth(const float lightWidth)
    {
        lightWidth_ = lightWidth;
    }

    const float& GetLightWidth() const
    {
        return lightWidth_;
    }

private:
    Vector3f lightPosition_;
    Vector4f lightColor_;
    float lightIntensity_ = 0.0f;
    float lightWidth_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_BORDER_LIGHT_EFFECT_PARA_H
