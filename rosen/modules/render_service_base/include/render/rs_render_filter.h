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

#ifndef ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_H
#define ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_H

#include "render/rs_render_filter_base.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderFilter {
public:
    RSRenderFilter() = default;
    virtual ~RSRenderFilter() = default;

    // add property
    inline void Insert(RSUIFilterType type, std::shared_ptr<RSRenderFilterParaBase> value)
    {
        auto it = properties_.find(type);
        if (it != properties_.end()) {
            properties_[type] = value;
        } else {
            properties_[type] = value;
            propertyTypes_.push_back(type);
        }
    }

    std::shared_ptr<RSRenderFilterParaBase> GetRenderFilterPara(RSUIFilterType type) const;

    static std::shared_ptr<RSRenderFilterParaBase> CreateRenderFilterPara(RSUIFilterType type);

    static bool MarshallingPropertyType(Parcel& parcel, const std::vector<RSUIFilterType>& val);

    static bool UnmarshallingPropertyType(Parcel& parcel, std::vector<RSUIFilterType>& val,
        int maxSize = RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE);

    void Dump(std::string& out);

    bool WriteToParcel(Parcel& parcel);

    bool ReadFromParcel(Parcel& parcel);

    inline const std::vector<RSUIFilterType>& GetUIFilterTypes()
    {
        return propertyTypes_;
    }

protected:
    std::vector<RSUIFilterType> propertyTypes_; // the order of properties_
    std::map<RSUIFilterType, std::shared_ptr<RSRenderFilterParaBase>> properties_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_H
