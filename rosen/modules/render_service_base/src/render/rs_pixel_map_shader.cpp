/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "render/rs_pixel_map_shader.h"

#include "render/rs_pixel_map_util.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSPixelMapShader::RSPixelMapShader(
    std::shared_ptr<Media::PixelMap> pixelMap, Drawing::TileMode tileX, Drawing::TileMode tileY,
    const Drawing::SamplingOptions& sampling, const Drawing::Matrix& matrix)
    : pixelMap_(pixelMap), tileX_(tileX), tileY_(tileY), sampling_(sampling), matrix_(matrix) {}

#ifdef ROSEN_OHOS
bool RSPixelMapShader::Marshalling(Parcel& parcel)
{
    return RSMarshallingHelper::Marshalling(parcel, pixelMap_) &&
           RSMarshallingHelper::Marshalling(parcel, tileX_) &&
           RSMarshallingHelper::Marshalling(parcel, tileY_) &&
           RSMarshallingHelper::Marshalling(parcel, sampling_) &&
           RSMarshallingHelper::Marshalling(parcel, matrix_);
}

bool RSPixelMapShader::Unmarshalling(Parcel& parcel)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, pixelMap_)) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, tileX_)) {
        return false;
    }
    if (tileX_ < Drawing::TileMode::CLAMP || tileX_ > Drawing::TileMode::DECAL) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, tileY_)) {
        return false;
    }
    if (tileY_ < Drawing::TileMode::CLAMP || tileY_ > Drawing::TileMode::DECAL) {
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, sampling_)) {
        return false;
    }
    if (sampling_.GetFilterMode() < Drawing::FilterMode::NEAREST ||
        sampling_.GetFilterMode() > Drawing::FilterMode::LINEAR) {
        return false;
    }
    if (sampling_.GetMipmapMode() < Drawing::MipmapMode::NONE ||
        sampling_.GetMipmapMode() > Drawing::MipmapMode::LINEAR) {
        return false;
    }
    return RSMarshallingHelper::Unmarshalling(parcel, matrix_);
}
#endif

void* RSPixelMapShader::GenerateBaseObject()
{
#ifdef ROSEN_OHOS
    if (pixelMap_) {
        pixelMap_->ReMap();
    }
#endif
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
    if (!image) {
        return nullptr;
    }
    // Be Careful: the Object Ptr should be managed by Caller.
    return new Drawing::ShaderEffect(
        Drawing::ShaderEffect::ShaderEffectType::IMAGE, *image, tileX_, tileY_, sampling_, matrix_);
}
} // namespace Rosen
} // namespace OHOS