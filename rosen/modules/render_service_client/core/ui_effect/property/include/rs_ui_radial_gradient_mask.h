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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_RADIAL_GRADIENT_MASK_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_RADIAL_GRADIENT_MASK_H

#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/property/include/rs_ui_mask_para.h"

#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {

class RSUIRadialGradientMaskPara : public RSUIMaskPara {
public:
    RSUIRadialGradientMaskPara() : RSUIMaskPara(RSUIFilterType::RADIAL_GRADIENT_MASK) {}
    virtual ~RSUIRadialGradientMaskPara() = default;

    virtual bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetPara(const std::shared_ptr<MaskPara>& para) override;

    void SetRadius(const float radiusX, const float radiusY);

    void SetCenter(const Vector2f center);

    void SetValues(const std::vector<float> colors, const std::vector<float> positions);

    template<class T>
    std::shared_ptr<RSRenderAnimatableProperty<T>> GetAnimatRenderProperty(const RSUIFilterType type)
    {
        auto proX = std::static_pointer_cast<RSAnimatableProperty<T>>(GetRSProperty(type));
        if (proX == nullptr) {
            return nullptr;
        }
        return std::make_shared<RSRenderAnimatableProperty<T>>(proX->Get(), proX->GetId());
    }

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_RADIAL_GRADIENT_MASK_H
