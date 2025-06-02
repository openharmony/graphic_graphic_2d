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
#ifndef UIEFFECT_FILTER_DISPLACENMENT_DISTORT_PARA_H
#define UIEFFECT_FILTER_DISPLACENMENT_DISTORT_PARA_H
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
// limits for ripple mask center parameters
constexpr std::pair<float, float> DISPLACEMENT_DISTORT_FACTOR_LIMITS { 0.0f, 10.0f };

class DisplacementDistortPara : public FilterPara {
public:
    DisplacementDistortPara()
    {
        this->type_ = FilterPara::ParaType::DISPLACEMENT_DISTORT;
    }
    ~DisplacementDistortPara() override = default;

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetFactor(Vector2f& factor)
    {
        factor_ = UIEffect::GetLimitedPara(factor, DISPLACEMENT_DISTORT_FACTOR_LIMITS);
    }

    const Vector2f& GetFactor() const
    {
        return factor_;
    }

private:
    std::shared_ptr<MaskPara> maskPara_;
    Vector2f factor_ = { 1.0f, 1.0f };
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_DISPLACENMENT_DISTORT_PARA_H
 
