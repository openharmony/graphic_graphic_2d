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
#ifndef RENDER_CONTENT_LIGHT_FILTER_H
#define RENDER_CONTENT_LIGHT_FILTER_H
#include "ui_effect/property/include/rs_ui_filter_para_base.h"
#include "ui_effect/filter/include/filter_content_light_para.h"
#include "render/rs_render_content_light_filter.h"

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSUIContentLightFilterPara : public RSUIFilterParaBase {
public:
    RSUIContentLightFilterPara() : RSUIFilterParaBase(RSUIFilterType::CONTENT_LIGHT) {}
    virtual ~RSUIContentLightFilterPara() = default;

    bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetContentLight(const std::shared_ptr<ContentLightPara>& contentLight);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;

private:
    bool CreateRenderFilterLightPosition(const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty,
        const RSUIFilterType type);
    bool CreateRenderFilterLightColor(const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty,
        const RSUIFilterType type);
    bool CreateRenderFilterLightIntensity(const std::shared_ptr<RSRenderContentLightFilterPara>& frProperty,
        const RSUIFilterType type);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_CONTENT_LIGHT_FILTER_H

