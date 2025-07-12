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
#ifndef UIEFFECT_FILTER_VARIABLE_RADIUS_BLUR_PARA_H
#define UIEFFECT_FILTER_VARIABLE_RADIUS_BLUR_PARA_H

#include "common/rs_macros.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT VariableRadiusBlurPara : public FilterPara {
public:
    VariableRadiusBlurPara()
    {
        this->type_ = FilterPara::ParaType::VARIABLE_RADIUS_BLUR;
    }
    ~VariableRadiusBlurPara() override = default;

    void SetBlurRadius(float blurRadius)
    {
        blurRadius_ = blurRadius;
    }

    float GetBlurRadius() const
    {
        return blurRadius_;
    }

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

private:
    float blurRadius_ = 0.0f;
    std::shared_ptr<MaskPara> maskPara_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_VARIABLE_RADIUS_BLUR_PARA_H
 
