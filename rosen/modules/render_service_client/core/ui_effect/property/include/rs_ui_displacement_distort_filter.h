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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_DISPLACEMENT_DISTORT_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_DISPLACEMENT_DISTORT_FILTER_H
#include <memory>

#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"

#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "render/rs_render_ripple_mask.h"
namespace OHOS {
namespace Rosen {

class RSUIDispDistortFilterPara : public RSUIFilterParaBase {
public:
    RSUIDispDistortFilterPara() : RSUIFilterParaBase(RSUIFilterType::DISPLACEMENT_DISTORT) {}
    virtual ~RSUIDispDistortFilterPara() = default;

    bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void Dump(std::string& out) const override;

    void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetDisplacementDistort(const std::shared_ptr<DisplacementDistortPara>& displacementDistort);

    void SetFactor(Vector2f factor);

    void SetMask(std::shared_ptr<RSUIMaskPara> mask);

    std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

private:
    static std::shared_ptr<RSUIMaskPara> CreateMask(RSUIFilterType type);
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_DISPLACEMENT_DISTORT_FILTER_H