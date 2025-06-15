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
#ifndef RENDER_CONTENT_LIGHT_FILTER_RENDER_PROPERTY_H
#define RENDER_CONTENT_LIGHT_FILTER_RENDER_PROPERTY_H

#include "render/rs_render_filter_base.h"
#include "render/rs_render_property_tag.h"
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderContentLightFilterPara : public RSRenderFilterParaBase {
public:
    RSRenderContentLightFilterPara(PropertyId id) : RSRenderFilterParaBase(RSUIFilterType::CONTENT_LIGHT)
    {
        id_ = id;
    }

    virtual ~RSRenderContentLightFilterPara() = default;

    void GetDescription(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel) override;

    virtual bool ReadFromParcel(Parcel& parcel) override;

    bool ParseFilterValues() override;

    static std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(RSUIFilterType type);

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    
    const Vector3f& GetLightPosition() const;

    const Vector4f& GetLightColor() const;

    float GetLightIntensity() const;

    const Vector3f& GetRotationAngle() const;

    void SetRotationAngle(const Vector3f& rotationAngle)
    {
        rotationAngle_ = rotationAngle;
    }

private:
    Vector3f lightPosition_;
    Vector4f lightColor_;
    float lightIntensity_;
    Vector3f rotationAngle_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_CONTENT_LIGHT_FILTER_RENDER_PROPERTY_H