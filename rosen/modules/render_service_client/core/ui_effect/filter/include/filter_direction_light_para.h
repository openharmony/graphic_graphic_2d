/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_DIRECTION_LIGHT_PARA_H
#define UIEFFECT_FILTER_DIRECTION_LIGHT_PARA_H
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT DirectionLightPara : public FilterPara {
public:
    DirectionLightPara()
    {
        this->type_ = FilterPara::ParaType::DIRECTION_LIGHT;
    }
    DirectionLightPara(const DirectionLightPara& other);
    ~DirectionLightPara() override = default;

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetMaskFactor(const float factor)
    {
        maskFactor_ = factor;
    }

    const float& GetMaskFactor() const
    {
        return maskFactor_;
    }

    void SetLightDirection(const Vector3f& lightDirection)
    {
        lightDirection_ = lightDirection;
    }

    const Vector3f& GetLightDirection() const
    {
        return lightDirection_;
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

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    float maskFactor_ = 1.0f;
    Vector3f lightDirection_ = Vector3f(0.0f, 0.0f, 1.0f);
    Vector4f lightColor_ = Vector4f(2.0f, 2.0f, 2.0f, 1.0f);
    float lightIntensity_ = 0.5f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_DIRECTION_LIGHT_PARA_H