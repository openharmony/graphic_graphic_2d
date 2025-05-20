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
#include "render/rs_render_bezier_warp_filter.h"

namespace OHOS {
namespace Rosen {
void RSRenderBezierWarpFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderBezierWarpFilterPara";
}

bool RSRenderBezierWarpFilterPara::WriteToParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(type_)) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(modifierType_))) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::WriteToParcel type Error");
        return false;
    }
    if (properties_.size() > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size large Error");
        return false;
    }
    if (!parcel.WriteUint32(properties_.size())) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::WriteToParcel size Error");
        return false;
    }
    for (const auto& [key, value] : properties_) {
        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSRenderPropertyBase::Marshalling(parcel, value)) {
            return false;
        }
        ROSEN_LOGD("RSRenderBezierWarpFilterPara::WriteToParcel type %{public}d", static_cast<int>(key));
    }
    return true;
}

bool RSRenderBezierWarpFilterPara::ReadFromParcel(Parcel& parcel)
{
    int16_t type = 0;
    int16_t modifierType = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel type Error");
        return false;
    }
    type_ = static_cast<RSUIFilterType>(type);
    modifierType_ = static_cast<RSModifierType>(modifierType);

    uint32_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size Error");
        return false;
    }
    if (size > static_cast<uint32_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size large Error");
        return false;
    }
    properties_.clear();
    for (uint32_t i = 0; i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel type %{public}d", static_cast<int>(key));
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSRenderPropertyBase::Unmarshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel value %{public}d", static_cast<int>(key));
            return false;
        }
        Setter(key, value);
    }
    return true;
}

std::shared_ptr<RSRenderPropertyBase> RSRenderBezierWarpFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::BEZIER_CONTROL_POINT0 :
        case RSUIFilterType::BEZIER_CONTROL_POINT1 :
        case RSUIFilterType::BEZIER_CONTROL_POINT2 :
        case RSUIFilterType::BEZIER_CONTROL_POINT3 :
        case RSUIFilterType::BEZIER_CONTROL_POINT4 :
        case RSUIFilterType::BEZIER_CONTROL_POINT5 :
        case RSUIFilterType::BEZIER_CONTROL_POINT6 :
        case RSUIFilterType::BEZIER_CONTROL_POINT7 :
        case RSUIFilterType::BEZIER_CONTROL_POINT8 :
        case RSUIFilterType::BEZIER_CONTROL_POINT9 :
        case RSUIFilterType::BEZIER_CONTROL_POINT10 :
        case RSUIFilterType::BEZIER_CONTROL_POINT11 : {
            return std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
                Vector2f(0.f, 0.f), 0, RSRenderPropertyType::PROPERTY_VECTOR2F);
        }
        default: {
            ROSEN_LOGD("RSRenderBezierWarpFilterPara::CreateRenderProperty is nullptr");
            return std::shared_ptr<RSRenderPropertyBase>();
        }
    }
    return std::shared_ptr<RSRenderPropertyBase>();
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderBezierWarpFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS