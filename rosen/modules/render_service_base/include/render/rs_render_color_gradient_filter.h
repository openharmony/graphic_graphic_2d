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

#ifndef RENDER_COLOR_GRADIENT_FILTER_RENDER_PROPERTY_H
#define RENDER_COLOR_GRADIENT_FILTER_RENDER_PROPERTY_H

#include <vector>

#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {
class RSShaderMask;

class RSB_EXPORT RSRenderColorGradientFilterPara : public RSRenderFilterParaBase {

public:
    RSRenderColorGradientFilterPara(PropertyId id, RSUIFilterType maskType = RSUIFilterType::NONE) :
        RSRenderFilterParaBase(RSUIFilterType::COLOR_GRADIENT), maskType_(maskType) {
        id_ = id;
    }

    virtual ~RSRenderColorGradientFilterPara() = default;

    void GetDescription(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel) override;

    virtual bool ReadFromParcel(Parcel& parcel) override;

    static std::shared_ptr<RSRenderMaskPara> CreateMaskRenderProperty(RSUIFilterType type);

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;

    std::shared_ptr<RSRenderMaskPara> GetRenderMask();

    bool ParseFilterValues() override;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

    const std::vector<float> GetColors() const;
    const std::vector<float> GetPositions() const;
    const std::vector<float> GetStrengths() const;
    const std::shared_ptr<RSShaderMask>& GetMask() const;

private:
    std::vector<float> colors_;
    std::vector<float> positions_;
    std::vector<float> strengths_;
    std::shared_ptr<RSShaderMask> mask_ = nullptr;
    RSUIFilterType maskType_ = RSUIFilterType::NONE;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_COLOR_GRADIENT_FILTER_RENDER_PROPERTY_H