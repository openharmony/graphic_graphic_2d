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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_DISPERSION_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_DISPERSION_FILTER_H

#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/property/include/rs_ui_mask_para.h"

#include "common/rs_vector2.h"
#include "render/rs_render_dispersion_filter.h"

namespace OHOS {
namespace Rosen {

class RSUIDispersionFilterPara : public RSUIFilterParaBase {
public:
    RSUIDispersionFilterPara() : RSUIFilterParaBase(RSUIFilterType::DISPERSION) {}
    virtual ~RSUIDispersionFilterPara() = default;

    virtual bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void Dump(std::string& out) const override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetDispersion(const std::shared_ptr<DispersionPara>& dispersion);

    void SetMask(const std::shared_ptr<RSUIMaskPara>& mask);

    void SetOpacity(float opacity);
    void SetRedOffset(const Vector2f& redOffset);
    void SetGreenOffset(const Vector2f& greenOffset);
    void SetBlueOffset(const Vector2f& blueOffset);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

private:
    static std::shared_ptr<RSUIMaskPara> CreateMask(RSUIFilterType type);

    RSUIFilterType maskType_ = RSUIFilterType::NONE;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_EDGE_LIGHT_FILTER_H