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
#ifndef RENDER_EDGE_LIGHT_FILTER_RENDER_PROPERTY_H
#define RENDER_EDGE_LIGHT_FILTER_RENDER_PROPERTY_H

#include <vector>

#include "common/rs_vector2.h"
#include "render/rs_render_filter_base.h"
#include "render/rs_render_ripple_mask.h"
namespace OHOS {
namespace Rosen {
class RSShaderMask;

class RSB_EXPORT RSRenderEdgeLightFilterPara : public RSRenderFilterParaBase {

public:
    RSRenderEdgeLightFilterPara(PropertyId id, RSUIFilterType maskType = RSUIFilterType::NONE) :
        RSRenderFilterParaBase(RSUIFilterType::EDGE_LIGHT), maskType_(maskType) {
        id_ = id;
    }

    virtual ~RSRenderEdgeLightFilterPara() = default;

    std::shared_ptr<RSRenderFilterParaBase> DeepCopy() const override;

    void GetDescription(std::string& out) const override;

    bool WriteToParcel(Parcel& parcel) override;

    bool ReadFromParcel(Parcel& parcel) override;

    std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;

    RSUIFilterType GetMaskType() const
    {
        return maskType_;
    }
    bool ParseFilterValues() override;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
private:
    float alpha_{ 0.f };
    bool bloom_{ true };
    std::optional<Vector4f> color_{ std::nullopt };
    std::shared_ptr<RSShaderMask> mask_{ nullptr };
    static std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(RSUIFilterType type);
    void CalculateHash();

    RSUIFilterType maskType_ = RSUIFilterType::NONE;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_EDGE_LIGHT_FILTER_RENDER_PROPERTY_H
