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
#include "render/rs_render_edge_light_filter.h"

namespace OHOS {
namespace Rosen {
void RSRenderEdgeLightFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderEdgeLightFilterPara::[maskType_:" + std::to_string(static_cast<int>(maskType_)) + "]";
}

std::shared_ptr<RSRenderPropertyBase> RSRenderEdgeLightFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::EDGE_LIGHT_DETECT_COLOR :
        case RSUIFilterType::EDGE_LIGHT_COLOR : {
            return std::make_shared<RSRenderProperty<Color>>();
        }
        case RSUIFilterType::EDGE_LIGHT_BLOOM_LEVEL : {
            return std::make_shared<RSRenderProperty<int>>(0, 0);
        }
        case RSUIFilterType::EDGE_LIGHT_EDGE_THRESHOLD :
        case RSUIFilterType::EDGE_LIGHT_EDGE_SOFT_THRESHOLD : {
            return std::make_shared<RSRenderProperty<float>>(0.f, 0, RSRenderPropertyType::PROPERTY_FLOAT);
        }
        case RSUIFilterType::EDGE_LIGHT_USE_RAW_COLOR :
        case RSUIFilterType::EDGE_LIGHT_GRADIENT :
        case RSUIFilterType::EDGE_LIGHT_ADD_IMAGE : {
            return std::make_shared<RSRenderProperty<bool>>(false, 0);
        }
        case RSUIFilterType::EDGE_LIGHT_EDGE_INTENSITY :
        case RSUIFilterType::EDGE_LIGHT_ALPHA_PROGRESS : {
            return std::make_shared<RSRenderAnimatableProperty<float>>(
                0.f, 0, RSRenderPropertyType::PROPERTY_FLOAT);
        }
        default: {
            ROSEN_LOGD("RSRenderEdgeLightFilterPara::CreateRenderPropert nullptr");
            return nullptr;
        }
    }
}

bool RSRenderEdgeLightFilterPara::WriteToParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderDispDistortFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel type Error");
        return false;
    }

    if (!parcel.WriteUint32(properties_.size())) {
        ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel properties_.size = 0");
        return false;
    }

    for (const auto& [key, value] : properties_) {
        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSMarshallingHelper::Marshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel Marshalling failed. key: %{public}s",
                std::to_string(static_cast<int>(key)).c_str());
            return false;
        }
    }
    return true;
}

bool RSRenderEdgeLightFilterPara::ReadFromParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
        return false;
    }

    uint32_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        return false;
    }

    if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        return false;
    }

    properties_.clear();
    for (uint32_t i = 0; i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            return false;
        }

        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSRenderPropertyBase::Unmarshalling(parcel, value)) {
            return false;
        }
        Setter(key, value);
    }
    return true;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderEdgeLightFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS