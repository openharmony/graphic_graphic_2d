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
#include "platform/common/rs_log.h"
#include "render/rs_render_blur_filter.h"
#include "render/rs_render_displacement_distort_filter.h"

namespace OHOS {
namespace Rosen {

using RSUIFilterTypeUnderlying = std::underlying_type<RSUIFilterType>::type;

std::shared_ptr<RSRenderFilterParaBase> RSRenderFilter::GetRenderFilterPara(RSUIFilterType type) const
{
    auto it = properties_.find(type);
    if (it != properties_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<RSRenderFilterParaBase> RSRenderFilter::CreateRenderFilterPara(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::BLUR : {
            return std::make_shared<RSRenderBlurFilterPara>(0);
        }
        case RSUIFilterType::DISPLACEMENT_DISTORT : {
            return std::make_shared<RSRenderDispDistortFilterPara>(0);
        }
        default: {
            ROSEN_LOGE("RSRenderFilter::CreateRenderFilterPara null type %{public}d ",
                static_cast<int>(type));
            return nullptr;
        }
    }
    return nullptr;
}

bool RSRenderFilter::MarshallingPropertyType(Parcel& parcel, const std::vector<RSUIFilterType>& val)
{
    size_t size = val.size();
    RSMarshallingHelper::Marshalling(parcel, size);
    for (size_t i = 0; i < size; i++) {
        if (!RSMarshallingHelper::Marshalling(parcel, static_cast<RSUIFilterTypeUnderlying>(val[i]))) {
            return false;
        }
        ROSEN_LOGD("RSRenderFilter::MarshallingPropertyType type %{public}d ", static_cast<int>(val[i]));
    }
    return true;
}

bool RSRenderFilter::UnmarshallingPropertyType(Parcel& parcel, std::vector<RSUIFilterType>& val, int maxSize)
{
    if (maxSize < 0) {
        return false;
    }
    size_t size = 0;
    RSMarshallingHelper::Unmarshalling(parcel, size);
    if (size > static_cast<size_t>(maxSize)) {
        return false;
    }
    val.clear();
    val.reserve(size);
    for (size_t i = 0; i < size; i++) {
        RSUIFilterTypeUnderlying tmp = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, tmp)) {
            return false;
        }
        RSUIFilterType type = static_cast<RSUIFilterType>(tmp);
        val.push_back(type);
        ROSEN_LOGD("RSRenderFilter::UnmarshallingPropertyType type %{public}d ", static_cast<int>(type));
    }
    return true;
}

void RSRenderFilter::Dump(std::string& out)
{
    for (const auto& type : propertyTypes_) {
        auto frProperty = GetRenderFilterPara(type);
        if (frProperty == nullptr) {
            ROSEN_LOGE("RSRenderFilter::WriteToParcel null type %{public}d ", static_cast<int>(type));
            return;
        }
        frProperty->Dump(out);
    }
}

bool RSRenderFilter::WriteToParcel(Parcel& parcel)
{
    std::string dumpInfo;
    Dump(dumpInfo);
    ROSEN_LOGI("%{public}s", dumpInfo.c_str());
    if (propertyTypes_.empty() || !MarshallingPropertyType(parcel, propertyTypes_)) {
        ROSEN_LOGE("RSRenderFilter::WriteToParcel failed with size %{public}d ",
            static_cast<int>(propertyTypes_.size()));
        return false;
    }
    for (const auto& type : propertyTypes_) {
        auto frProperty = GetRenderFilterPara(type);
        if (frProperty == nullptr) {
            ROSEN_LOGE("RSRenderFilter::WriteToParcel null type %{public}d ", static_cast<int>(type));
            return false;
        }
        if (!frProperty->WriteToParcel(parcel)) {
            ROSEN_LOGE("RSRenderFilter::WriteToParcel error value type %{public}d ", static_cast<int>(type));
            return false;
        }
    }
    return true;
}

bool RSRenderFilter::ReadFromParcel(Parcel& parcel)
{
    propertyTypes_.clear();
    properties_.clear();
    std::vector<RSUIFilterType> propertyTypesTmp;
    if (!UnmarshallingPropertyType(parcel, propertyTypesTmp)) {
        ROSEN_LOGE("RSRenderFilter::ReadFromParcel failed with size %{public}d ",
            static_cast<int>(propertyTypesTmp.size()));
        return false;
    }
    if (propertyTypesTmp.empty()) {
        ROSEN_LOGE("RSRenderFilter::ReadFromParcel empty type");
        return false;
    }
    ROSEN_LOGD("RSRenderFilter::ReadFromParcel with size %{public}d ",
        static_cast<int>(propertyTypesTmp.size()));
    for (const auto& type : propertyTypesTmp) {
        ROSEN_LOGD("RSRenderFilter::ReadFromParcel type %{public}d ", static_cast<int>(type));
        auto frProperty = CreateRenderFilterPara(type);
        if (frProperty == nullptr) {
            ROSEN_LOGE("RSRenderFilter::ReadFromParcel null type %{public}d ", static_cast<int>(type));
            return false;
        }
        if (!frProperty->ReadFromParcel(parcel)) {
            ROSEN_LOGE("RSRenderFilter::ReadFromParcel null value %{public}d ", static_cast<int>(type));
            return false;
        }
        Insert(type, frProperty);
    }
    std::string dumpInfo;
    Dump(dumpInfo);
    ROSEN_LOGI("%{public}s", dumpInfo.c_str());
    return true;
}
} // namespace Rosen
} // namespace OHOS
