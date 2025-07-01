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
#ifndef UIEFFECT_FILTER_CONTENT_LIGHT_PARA_H
#define UIEFFECT_FILTER_CONTENT_LIGHT_PARA_H
#include <iostream>
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "filter_para.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT ContentLightPara : public FilterPara {
public:
    ContentLightPara()
    {
        this->type_ = FilterPara::ParaType::CONTENT_LIGHT;
    }
    ~ContentLightPara() override = default;

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

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;

private:
    Vector3f lightPosition_;
    Vector4f lightColor_;
    float lightIntensity_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_CONTENT_LIGHT_PARA_H
