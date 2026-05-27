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

#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

FrostedGlassBlurPara::FrostedGlassBlurPara(const FrostedGlassBlurPara& para)
{
    type_ = para.type_;
    radius_ = para.radius_;
    radiusScaleK_ = para.radiusScaleK_;
    refractOutPx_ = para.refractOutPx_;
    isSkipFrameEnable_ = para.isSkipFrameEnable_;
}

bool FrostedGlassBlurPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess =
        parcel.WriteUint16(static_cast<uint16_t>(type_)) && parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] FrostedGlassBlurPara Marshalling write type failed");
        return false;
    }

    isSuccess = parcel.WriteFloat(radius_) && parcel.WriteFloat(radiusScaleK_) && parcel.WriteFloat(refractOutPx_) &&
                parcel.WriteBool(isSkipFrameEnable_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] FrostedGlassBlurPara Marshalling write para failed");
        return false;
    }
    return true;
}

void FrostedGlassBlurPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::FROSTED_GLASS_BLUR, OnUnmarshalling);
}

bool FrostedGlassBlurPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::FROSTED_GLASS_BLUR) {
        RS_LOGE("[ui_effect] FrostedGlassBlurPara OnUnmarshalling read type failed");
        return false;
    }

    float radius = 0.0f;
    float radiusScaleK = 0.0f;
    float refractOutPx = 0.0f;
    bool isSkipFrameEnable = false;
    bool isSuccess = parcel.ReadFloat(radius) && parcel.ReadFloat(radiusScaleK) && parcel.ReadFloat(refractOutPx) &&
                     parcel.ReadBool(isSkipFrameEnable);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] FrostedGlassBlurPara OnUnmarshalling read para failed");
        return false;
    }

    auto frostedGlassBlurPara = std::make_shared<FrostedGlassBlurPara>();
    frostedGlassBlurPara->SetBlurRadius(radius);
    frostedGlassBlurPara->SetBlurRadiusScaleK(radiusScaleK);
    frostedGlassBlurPara->SetRefractOutPx(refractOutPx);
    frostedGlassBlurPara->SetSkipFrameEnable(isSkipFrameEnable);
    val = std::move(frostedGlassBlurPara);
    return true;
}

std::shared_ptr<FilterPara> FrostedGlassBlurPara::Clone() const
{
    return std::make_shared<FrostedGlassBlurPara>(*this);
}

} // namespace Rosen
} // namespace OHOS