/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/filter/include/filter_content_light_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool ContentLightPara::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(static_cast<uint16_t>(type_)) ||
        !parcel.WriteUint16(static_cast<uint16_t>(type_))) {
        RS_LOGE("[ui_effect] ContentLightPara Marshalling write type failed");
        return false;
    }

    bool isInvalid = !parcel.WriteFloat(lightPosition_.x_) ||
        !parcel.WriteFloat(lightPosition_.y_) ||
        !parcel.WriteFloat(lightPosition_.z_);
    isInvalid = isInvalid || (
        !parcel.WriteFloat(lightColor_.x_) || !parcel.WriteFloat(lightColor_.y_) ||
        !parcel.WriteFloat(lightColor_.z_) || !parcel.WriteFloat(lightColor_.w_)) ||
        !parcel.WriteFloat(lightIntensity_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] ContentLightPara Marshalling write para failed");
        return false;
    }
    return true;
}

void ContentLightPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::CONTENT_LIGHT, OnUnmarshalling);
}

bool ContentLightPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::CONTENT_LIGHT) {
        RS_LOGE("[ui_effect] ContentLightPara OnUnmarshalling read type failed");
        return false;
    }

    auto contentLightPara = std::make_shared<ContentLightPara>();
    Vector3f lightPosition;
    Vector4f lightColor;
    float lightIntensity = 0.0f;
    bool isInvalid = !parcel.ReadFloat(lightPosition.x_) ||
        !parcel.ReadFloat(lightPosition.y_) ||
        !parcel.ReadFloat(lightPosition.z_);
    isInvalid = isInvalid || (
        !parcel.ReadFloat(lightColor.x_) || !parcel.ReadFloat(lightColor.y_) ||
        !parcel.ReadFloat(lightColor.z_) || !parcel.ReadFloat(lightColor.w_)) ||
        !parcel.ReadFloat(lightIntensity);
    if (isInvalid) {
        RS_LOGE("[ui_effect] ContentLightPara OnUnmarshalling read para failed");
        return false;
    }
    contentLightPara->SetLightPosition(lightPosition);
    contentLightPara->SetLightColor(lightColor);
    contentLightPara->SetLightIntensity(lightIntensity);
    val = std::move(contentLightPara);
    return true;
}

std::shared_ptr<FilterPara> ContentLightPara::Clone() const
{
    return std::make_shared<ContentLightPara>(*this);
}

} // namespace Rosen
} // namespace OHOS