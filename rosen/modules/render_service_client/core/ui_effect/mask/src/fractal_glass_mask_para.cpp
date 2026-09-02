/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ui_effect/mask/include/fractal_glass_mask_para.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include "media_errors.h"
#endif

namespace OHOS {
namespace Rosen {

FractalGlassMaskPara::FractalGlassMaskPara(const FractalGlassMaskPara& other)
    : MaskPara(other), pixelMap_(nullptr), src_(other.src_), dst_(other.dst_),
      glassNum_(other.glassNum_), glassStrength_(other.glassStrength_), glassSoftness_(other.glassSoftness_),
      isSymmetric_(other.isSymmetric_)
{
#ifdef ROSEN_OHOS
    if (other.pixelMap_ != nullptr) {
        int32_t errorCode = Media::ERR_MEDIA_INVALID_VALUE;
        auto pixelMap = other.pixelMap_->Clone(errorCode);
        pixelMap_ = std::move(pixelMap);
        if (errorCode != Media::SUCCESS || pixelMap_ == nullptr) {
            RS_LOGE("[ui_effect] FractalGlassMaskPara clone pixelMap failed");
        }
    }
#endif
}

bool FractalGlassMaskPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        pixelMap_ != nullptr && pixelMap_->Marshalling(parcel) &&
        parcel.WriteFloat(src_.x_) && parcel.WriteFloat(src_.y_) &&
        parcel.WriteFloat(src_.z_) && parcel.WriteFloat(src_.w_) &&
        parcel.WriteFloat(dst_.x_) && parcel.WriteFloat(dst_.y_) &&
        parcel.WriteFloat(dst_.z_) && parcel.WriteFloat(dst_.w_) &&
        parcel.WriteFloat(glassNum_) && parcel.WriteFloat(glassStrength_) &&
        parcel.WriteFloat(glassSoftness_) &&
        parcel.WriteBool(isSymmetric_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] FractalGlassMaskPara Marshalling write para failed");
        return false;
    }
    return true;
}

void FractalGlassMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::FRACTAL_GLASS_MASK, OnUnmarshalling);
}

bool FractalGlassMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::FRACTAL_GLASS_MASK) {
        RS_LOGE("[ui_effect] FractalGlassMaskPara OnUnmarshalling read type failed, type=%{public}d", type);
        return false;
    }

    auto para = std::make_shared<FractalGlassMaskPara>();
    auto pixelMap = Media::PixelMap::Unmarshalling(parcel);
    if (pixelMap == nullptr) {
        RS_LOGE("[ui_effect] FractalGlassMaskPara OnUnmarshalling pixelMap failed");
        return false;
    }
    auto pixelMapTmp = std::shared_ptr<Media::PixelMap>(pixelMap);
    para->SetPixelMap(pixelMapTmp);

    Vector4f src;
    Vector4f dst;
    float glassNum = 0.0f;
    float glassStrength = 0.0f;
    float glassSoftness = 0.0f;
    bool isSymmetric = true;
    auto isSuccess = parcel.ReadFloat(src.x_) && parcel.ReadFloat(src.y_) &&
        parcel.ReadFloat(src.z_) && parcel.ReadFloat(src.w_) &&
        parcel.ReadFloat(dst.x_) && parcel.ReadFloat(dst.y_) &&
        parcel.ReadFloat(dst.z_) && parcel.ReadFloat(dst.w_) &&
        parcel.ReadFloat(glassNum) && parcel.ReadFloat(glassStrength) &&
        parcel.ReadFloat(glassSoftness) &&
        parcel.ReadBool(isSymmetric);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] FractalGlassMaskPara OnUnmarshalling read para failed");
        return false;
    }
    para->SetSrc(src);
    para->SetDst(dst);
    para->SetGlassNum(glassNum);
    para->SetGlassStrength(glassStrength);
    para->SetGlassSoftness(glassSoftness);
    para->SetIsSymmetric(isSymmetric);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> FractalGlassMaskPara::Clone() const
{
    return std::make_shared<FractalGlassMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS