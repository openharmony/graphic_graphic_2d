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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_H

#include "ui_effect/property/include/rs_ui_filter_para_base.h"

#include "render/rs_render_filter.h"
namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSUIFilter {
public:
    RSUIFilter() = default;
    virtual ~RSUIFilter() = default;

    std::shared_ptr<RSRenderFilter> GetRSRenderFilter();

    void Dump(std::string& out)
    {
        for (const auto& type : propertyTypes_) {
            auto frProperty = GetUIFilterPara(type);
            if (frProperty) {
                frProperty->Dump(out);
            }
        }
    }

    std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(const PropertyId& id);

    void Insert(const std::shared_ptr<RSUIFilterParaBase>& value);

    bool IsStructureSame(const std::shared_ptr<RSUIFilter>& other) const;

    void SetValue(const std::shared_ptr<RSUIFilter>& other);

    inline const std::vector<RSUIFilterType>& GetAllTypes() const
    {
        return propertyTypes_;
    }

    std::shared_ptr<RSUIFilterParaBase> GetUIFilterPara(RSUIFilterType type) const;
    std::vector<RSUIFilterType>& GetUIFilterTypes();

protected:
    std::vector<RSUIFilterType> propertyTypes_; // the order of paras_
    std::map<RSUIFilterType, std::shared_ptr<RSUIFilterParaBase>> paras_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_H