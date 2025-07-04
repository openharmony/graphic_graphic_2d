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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_EDGE_LIGHT_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_EDGE_LIGHT_FILTER_H
#include <memory>
#include <vector>

#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"

#include "common/rs_color.h"
#include "common/rs_vector4.h"
#include "modifier/rs_property.h"
#include "render/rs_render_edge_light_filter.h"
#include "render/rs_render_ripple_mask.h"

namespace OHOS {
namespace Rosen {

class RSUIEdgeLightFilterPara : public RSUIFilterParaBase {
public:
    RSUIEdgeLightFilterPara() : RSUIFilterParaBase(RSUIFilterType::EDGE_LIGHT) {}
    virtual ~RSUIEdgeLightFilterPara() = default;

    bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void Dump(std::string& out) const override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetEdgeLight(const std::shared_ptr<EdgeLightPara>& edgeLight);

    void SetAlpha(float alpha);

    void SetBloom(bool bloom);

    void SetColor(const Vector4f& color);

    void SetRawColor(bool useRawColor);

    void SetMask(const std::shared_ptr<RSUIMaskPara>& mask);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

    virtual bool CheckEnableHdrEffect() override;

private:
    static std::shared_ptr<RSUIMaskPara> CreateMask(RSUIFilterType type);

    template<typename PropertyType>
    std::shared_ptr<PropertyType> GetPropertyWithFilterType(RSUIFilterType type) const
    {
        auto property = GetRSProperty(type);
        if (property == nullptr) {
            return nullptr;
        }

        return std::static_pointer_cast<PropertyType>(property);
    }

    bool CreateRSRenderFilterAlpha(const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty);
    bool CreateRSRenderFilterBloom(const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty);
    bool CreateRSRenderFilterColor(const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty);
    bool CreateRSRenderFilterUseRawColor(const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty);
    bool CreateRSRenderFilterMask(const std::shared_ptr<RSRenderEdgeLightFilterPara>& frProperty);
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_EDGE_LIGHT_FILTER_H