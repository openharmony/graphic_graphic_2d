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
#ifndef RENDER_DISPLACEMENT_DISTORT_FILTER_RENDER_PROPERTY_H
#define RENDER_DISPLACEMENT_DISTORT_FILTER_RENDER_PROPERTY_H

#include "common/rs_vector2.h"
#include "render/rs_render_filter_base.h"
#include "render/rs_render_ripple_mask.h"
namespace OHOS {
namespace Rosen {
class RSShaderMask;

class RSB_EXPORT RSRenderDispDistortFilterPara : public RSRenderFilterParaBase {

public:
    RSRenderDispDistortFilterPara(PropertyId id, RSUIFilterType maskType = RSUIFilterType::NONE) :
        RSRenderFilterParaBase(RSUIFilterType::DISPLACEMENT_DISTORT), maskType_(maskType) {
        id_ = id;
    }

    virtual ~RSRenderDispDistortFilterPara() = default;

    std::shared_ptr<RSRenderFilterParaBase> DeepCopy() const override;

    void GetDescription(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel) override;

    virtual bool ReadFromParcel(Parcel& parcel) override;

    static std::shared_ptr<RSRenderMaskPara> CreateRenderProperty(RSUIFilterType type);

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;

    std::shared_ptr<RSRenderMaskPara> GetRenderMask();

    bool ParseFilterValues() override;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

    const Vector2f& GetFactor() const;
    const std::shared_ptr<RSShaderMask>& GetMask() const;

private:
    void CalculateHash();

    static constexpr char GE_FILTER_DISPLACEMENT_DISTORT_FACTOR[] = "DISTORT_FACTOR";
    static constexpr char GE_FILTER_DISPLACEMENT_DISTORT_MASK[] = "DISTORT_MASK";
    std::shared_ptr<RSShaderMask> mask_ = nullptr;
    Vector2f factor_ = {1.f, 1.f};
    RSUIFilterType maskType_ = RSUIFilterType::NONE;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_DISPLACEMENT_DISTORT_FILTER_RENDER_PROPERTY_H
