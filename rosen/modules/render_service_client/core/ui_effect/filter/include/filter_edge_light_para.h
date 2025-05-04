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
#ifndef UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
#define UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
#include "common/rs_vector4.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"

namespace OHOS {
namespace Rosen {
class EdgeLightPara : public FilterPara {
public:
    EdgeLightPara()
    {
        this->type_ = FilterPara::ParaType::EDGE_LIGHT;
    }
    ~EdgeLightPara() override = default;

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetAlpha(float alpha)
    {
        alpha_ = alpha;
    }

    float GetAlpha() const
    {
        return alpha_;
    }

    // color: red, green, blue, alpha
    void SetColor(const std::optional<Vector4f>& color)
    {
        color_ = color;
    }

    // return: red, green, blue, alpha
    const std::optional<Vector4f>& GetColor() const
    {
        return color_;
    }

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    std::optional<Vector4f> color_ = std::nullopt;
    float alpha_ = 0.f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
