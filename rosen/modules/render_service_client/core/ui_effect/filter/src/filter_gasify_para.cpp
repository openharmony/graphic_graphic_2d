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

#include "ui_effect/filter/include/filter_gasify_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

GasifyPara::GasifyPara(const GasifyPara& other)
{
    this->type_ = other.type_;
    this->progress_ = other.progress_;
    this->sourceImage_ = other.sourceImage_;
    this->maskImage_ = other.maskImage_;
}

bool GasifyPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] GasifyPara Marshalling write type failed");
        return false;
    }
    bool isImagevalid = false;
    isImagevalid = sourceImage_->Marshalling(parcel) && maskImage_->Marshalling(parcel);
    if (!isImagevalid) {
        RS_LOGE("[ui_effect] GasifyPara Marshalling write image failed");
        return false;
    }
    bool isInvalid = !parcel.WriteFloat(progress_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyPara Marshalling write progress failed");
        return false;
    }
    return true;
}

void GasifyPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::GASIFY, OnUnmarshalling);
}

bool GasifyPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::GASIFY) {
        RS_LOGE("[ui_effect] GasifyPara OnUnmarshalling read type failed");
        return false;
    }

    auto srcPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto mkPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto srcPixelMapTmp = std::shared_ptr<Media::PixelMap>(srcPixelMap);
    auto mkPixelMapTmp = std::shared_ptr<Media::PixelMap>(mkPixelMap);
    bool isInvalidImage = !srcPixelMapTmp || !mkPixelMapTmp;
    if (isInvalidImage) {
        RS_LOGE("[ui_effect] GasifyPara OnUnmarshalling read image failed");
        return false;
    }
    auto gasifyPara = std::make_shared<GasifyPara>();
    gasifyPara->SetSourceImage(srcPixelMapTmp);
    gasifyPara->SetMaskImage(mkPixelMapTmp);
    float progress;
    bool isInvalid = (!parcel.ReadFloat(progress));
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyPara OnUnmarshalling read para failed");
        return false;
    }
    gasifyPara->SetProgress(progress);
    val = std::move(gasifyPara);
    return true;
}

std::shared_ptr<FilterPara> GasifyPara::Clone() const
{
    return std::make_shared<GasifyPara>(*this);
}

} // namespace Rosen
} // namespace OHOS