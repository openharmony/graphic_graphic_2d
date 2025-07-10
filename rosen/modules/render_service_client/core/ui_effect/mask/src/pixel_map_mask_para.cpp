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

#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include "media_errors.h"
#endif

namespace OHOS {
namespace Rosen {

PixelMapMaskPara::PixelMapMaskPara(const PixelMapMaskPara& other)
    : pixelMap_(nullptr), src_(other.src_), dst_(other.dst_), fillColor_(other.fillColor_)
{
#ifdef ROSEN_OHOS
    if (other.pixelMap_ != nullptr) {
        int32_t errorCode = Media::ERR_MEDIA_INVALID_VALUE;
        auto pixelMap = other.pixelMap_->Clone(errorCode);
        pixelMap_ = std::move(pixelMap);
        if (errorCode != Media::SUCCESS || pixelMap_ == nullptr) {
            RS_LOGE("[ui_effect] PixelMapMaskPara clone pixelMap failed");
        }
    }
#endif
}

bool PixelMapMaskPara::Marshalling(Parcel& parcel) const
{
    if (!(parcel.WriteUint16(static_cast<uint16_t>(type_)) && parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        pixelMap_ != nullptr && pixelMap_->Marshalling(parcel) &&
        parcel.WriteFloat(src_.x_) && parcel.WriteFloat(src_.y_) &&
        parcel.WriteFloat(src_.z_) && parcel.WriteFloat(src_.w_) &&
        parcel.WriteFloat(dst_.x_) && parcel.WriteFloat(dst_.y_) &&
        parcel.WriteFloat(dst_.z_) && parcel.WriteFloat(dst_.w_) &&
        parcel.WriteFloat(fillColor_.x_) && parcel.WriteFloat(fillColor_.y_) &&
        parcel.WriteFloat(fillColor_.z_) && parcel.WriteFloat(fillColor_.w_))) {
        RS_LOGE("[ui_effect] PixelMapMaskPara Marshalling write para failed");
        return false;
    }
    return true;
}

void PixelMapMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::PIXEL_MAP_MASK, OnUnmarshalling);
}

bool PixelMapMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::PIXEL_MAP_MASK) {
        RS_LOGE("[ui_effect] PixelMapMaskPara OnUnmarshalling read type failed");
        return false;
    }

    auto para = std::make_shared<PixelMapMaskPara>();
    auto pixelMap = Media::PixelMap::Unmarshalling(parcel);
    if (pixelMap == nullptr) {
        RS_LOGE("[ui_effect] PixelMapMaskPara OnUnmarshalling pixelMap failed");
        return false;
    }
    auto pixelMapTmp = std::shared_ptr<Media::PixelMap>(pixelMap);
    para->SetPixelMap(pixelMapTmp);

    Vector4f src;
    Vector4f dst;
    Vector4f fillColor;
    if (!(parcel.ReadFloat(src.x_) && parcel.ReadFloat(src.y_) &&
        parcel.ReadFloat(src.z_) && parcel.ReadFloat(src.w_) &&
        parcel.ReadFloat(dst.x_) && parcel.ReadFloat(dst.y_) &&
        parcel.ReadFloat(dst.z_) && parcel.ReadFloat(dst.w_) &&
        parcel.ReadFloat(fillColor.x_) && parcel.ReadFloat(fillColor.y_) &&
        parcel.ReadFloat(fillColor.z_) && parcel.ReadFloat(fillColor.w_))) {
        RS_LOGE("[ui_effect] PixelMapMaskPara OnUnmarshalling read para failed");
        return false;
    }
    para->SetSrc(src);
    para->SetDst(dst);
    para->SetFillColor(fillColor);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> PixelMapMaskPara::Clone() const
{
    return std::make_shared<PixelMapMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS