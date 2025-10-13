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

#include "ui_effect/filter/include/filter_gasify_blur_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

GasifyBlurPara::GasifyBlurPara(const GasifyBlurPara& other)
{
    this->type_ = other.type_;
    this->progress_ = other.progress_;
    this->sourceImage_ = other.sourceImage_;
    this->maskImage_ = other.maskImage_;
}

bool GasifyBlurPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] GasifyBlurPara Marshalling write type failed");
        return false;
    }
    bool isImagevalid = false;
    isImagevalid = sourceImage_->Marshalling(parcel) && maskImage_->Marshalling(parcel);
    if (!isImagevalid) {
        RS_LOGE("[ui_effect] GasifyBlurPara Marshalling write image failed");
        return false;
    }
    bool isInvalid = !parcel.WriteFloat(progress_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyBlurPara Marshalling write progress failed");
        return false;
    }
    return true;
}

void GasifyBlurPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::GASIFY_BLUR, OnUnmarshalling);
}

bool GasifyBlurPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::GASIFY_BLUR) {
        RS_LOGE("[ui_effect] GasifyBlurPara OnUnmarshalling read type failed");
        return false;
    }

    auto srcPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto mkPixelMap = Media::PixelMap::Unmarshalling(parcel);
    auto srcPixelMapTmp = std::shared_ptr<Media::PixelMap>(srcPixelMap);
    auto mkPixelMapTmp = std::shared_ptr<Media::PixelMap>(mkPixelMap);
    bool isInvalidImage = !srcPixelMapTmp || !mkPixelMapTmp;
    if (isInvalidImage) {
        RS_LOGE("[ui_effect] GasifyBlurPara OnUnmarshalling read image failed");
        return false;
    }
    auto gasifyBlurPara = std::make_shared<GasifyBlurPara>();
    gasifyBlurPara->SetSourceImage(srcPixelMapTmp);
    gasifyBlurPara->SetMaskImage(mkPixelMapTmp);
    float progress;
    bool isInvalid = (!parcel.ReadFloat(progress));
    if (isInvalid) {
        RS_LOGE("[ui_effect] GasifyBlurPara OnUnmarshalling read para failed");
        return false;
    }
    gasifyBlurPara->SetProgress(progress);
    val = std::move(gasifyBlurPara);
    return true;
}

std::shared_ptr<FilterPara> GasifyBlurPara::Clone() const
{
    return std::make_shared<GasifyBlurPara>(*this);
}

} // namespace Rosen
} // namespace OHOS