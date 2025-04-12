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

#include "render/rs_pixel_map_filter.h"

#include "render/rs_pixel_map_util.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSPixelMapFilter::RSPixelMapFilter(
    std::shared_ptr<Media::PixelMap> pixelMap, Drawing::Rect srcRect, Drawing::Rect dstRect)
    : Drawing::ExtendObject(ExtendObjectType::IMAGE_FILTER), pixelMap_(pixelMap), srcRect_(srcRect), dstRect_(dstRect)
{}

#ifdef ROSEN_OHOS
bool RSPixelMapFilter::Marshalling(Parcel& parcel)
{
    return RSMarshallingHelper::Marshalling(parcel, pixelMap_) &&
        RSMarshallingHelper::Marshalling(parcel, srcRect_) &&
        RSMarshallingHelper::Marshalling(parcel, dstRect_);
}

bool RSPixelMapFilter::Unmarshalling(Parcel& parcel)
{
    return RSMarshallingHelper::Unmarshalling(parcel, pixelMap_) &&
        RSMarshallingHelper::Unmarshalling(parcel, srcRect_) &&
        RSMarshallingHelper::Unmarshalling(parcel, dstRect_);
}
#endif

std::shared_ptr<void> RSPixelMapFilter::GenerateBaseObject()
{
    if (baseObject_ != nullptr) {
        return baseObject_;
    }
#ifdef ROSEN_OHOS
    if (pixelMap_ && pixelMap_->IsUnMap()) {
        pixelMap_->ReMap();
    }
#endif
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
    if (!image) {
        return nullptr;
    }
    baseObject_ = std::make_shared<Drawing::ImageFilter>(
        Drawing::ImageFilter::FilterType::IMAGE, image, srcRect_, dstRect_);
    return baseObject_;
}
} // namespace Rosen
} // namespace OHOS