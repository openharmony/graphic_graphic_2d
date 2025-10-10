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

#include "ui_effect/mask/include/harmonium_effect_mask_para.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include "media_errors.h"
#endif

namespace OHOS {
namespace Rosen {

HarmoniumEffectMaskPara::HarmoniumEffectMaskPara(const HarmoniumEffectMaskPara& other)
    : MaskPara(other), pixelMap_(nullptr)
{
#ifdef ROSEN_OHOS
    if (other.pixelMap_ != nullptr) {
        int32_t errorCode = Media::ERR_MEDIA_INVALID_VALUE;
        auto pixelMap = other.pixelMap_->Clone(errorCode);
        pixelMap_ = std::move(pixelMap);
        if (errorCode != Media::SUCCESS || pixelMap_ == nullptr) {
            RS_LOGE("[ui_effect] HarmoniumEffectMaskPara clone pixelMap failed");
        }
    }
#endif
}

bool HarmoniumEffectMaskPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        pixelMap_ != nullptr && pixelMap_->Marshalling(parcel);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] HarmoniumEffectMaskPara Marshalling write para failed");
        return false;
    }
    return true;
}

void HarmoniumEffectMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::HARMONIUM_EFFECT_MASK, OnUnmarshalling);
}

bool HarmoniumEffectMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::HARMONIUM_EFFECT_MASK) {
        RS_LOGE("[ui_effect] HarmoniumEffectMaskPara OnUnmarshalling read type failed");
        return false;
    }

    auto para = std::make_shared<HarmoniumEffectMaskPara>();
    auto pixelMap = Media::PixelMap::Unmarshalling(parcel);
    if (pixelMap == nullptr) {
        RS_LOGE("[ui_effect] HarmoniumEffectMaskPara OnUnmarshalling pixelMap failed");
        return false;
    }
    auto pixelMapTmp = std::shared_ptr<Media::PixelMap>(pixelMap);
    para->SetPixelMap(pixelMapTmp);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> HarmoniumEffectMaskPara::Clone() const
{
    return std::make_shared<HarmoniumEffectMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS