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
#ifndef RENDER_EDGE_LIGHT_FILTER_H
#define RENDER_EDGE_LIGHT_FILTER_H
#include <memory>

#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"

#include "common/rs_color.h"
#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "render/rs_render_ripple_mask.h"
#include "render/rs_render_edge_light_filter.h"
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

    void SetDetectColor(RSColor detectColor);

    void SetColor(RSColor color);

    void SetEdgeThreshold(float edgeThreshold);

    void SetEdgeIntensity(float edgeIntensity);

    void SetEdgeSoftThreshold(float edgeSoftThreshold);

    void SetBloomLevel(int bloomLevel);

    void SetUseRawColor(bool useRawColor);

    void SetGradient(bool gradient);

    void SetAlphaProgress(float alphaProgress);

    void SetAddImage(bool addImage);

    void SetMask(std::shared_ptr<RSUIMaskPara> mask);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

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

    bool SetRSRenderPropertyWithUIFilterType(
        RSRenderEdgeLightFilterPara& renderPara, RSUIFilterType uiFilterType) const;

    RSUIFilterType maskType_ = RSUIFilterType::NONE;
    bool CreateRSRenderFilterDetectColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterThreshold(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterIntensity(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterSoftThreshold(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterBloomLevel(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterUseRawColor(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterGradient(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterProgress(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
    bool CreateRSRenderFilterAddImage(std::shared_ptr<RSRenderEdgeLightFilterPara> frProperty);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_EDGE_LIGHT_FILTER_H