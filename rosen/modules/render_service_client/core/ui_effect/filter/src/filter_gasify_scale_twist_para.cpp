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

#include "ui_effect/filter/include/filter_gasify_scale_twist_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

GasifyScaleTwistPara::GasifyScaleTwistPara(const GasifyScaleTwistPara& other)
{
    this->type_ = other.type_;
    this->progress_ = other.progress_;
    this->sourceImage_ = other.sourceImage_;
    this->maskImage_ = other.maskImage_;
    this->scale_ = other.scale_;
}

bool GasifyScaleTwistPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara Marshalling write type failed");
        return false;
    }
    bool isImagevalid = false;
    isImagevalid = sourceImage_->Marshalling(parcel) && maskImage_->Marshalling(parcel);
    if (!isImagevalid) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara Marshalling write image failed");
        return false;
    }
    bool isInvalid = !parcel.WriteFloat(progress_) || !parcel.WriteFloat(scale_.x_) || !parcel.WriteFloat(scale_.y_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara Marshalling write progress failed");
        return false;
    }
    return true;
}

void GasifyScaleTwistPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::GASIFY_SCALE_TWIST, OnUnmarshalling);
}

bool GasifyScaleTwistPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::GASIFY_SCALE_TWIST) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara OnUnmarshalling read type failed");
        return false;
    }

    auto srcPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto mkPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto srcPixelMapTmp = std::shared_ptr<Media::PixelMap>(srcPixelMap);
    auto mkPixelMapTmp = std::shared_ptr<Media::PixelMap>(mkPixelMap);
    bool isInvalidImage = !srcPixelMapTmp || !mkPixelMapTmp;
    if (isInvalidImage) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara OnUnmarshalling read image failed");
        return false;
    }
    auto gasifyScaleTwistPara = std::make_shared<GasifyScaleTwistPara>();
    gasifyScaleTwistPara->SetSourceImage(srcPixelMapTmp);
    gasifyScaleTwistPara->SetMaskImage(mkPixelMapTmp);
    float progress;
    Vector2f scale;
    bool isInvalid = (!parcel.ReadFloat(progress) || !parcel.ReadFloat(scale.x_) || !parcel.ReadFloat(scale.y_));
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyScaleTwistPara OnUnmarshalling read para failed");
        return false;
    }
    gasifyScaleTwistPara->SetProgress(progress);
    gasifyScaleTwistPara->SetScale(scale);
    val = std::move(gasifyScaleTwistPara);
    return true;
}

std::shared_ptr<FilterPara> GasifyScaleTwistPara::Clone() const
{
    return std::make_shared<GasifyScaleTwistPara>(*this);
}

} // namespace Rosen
} // namespace OHOS