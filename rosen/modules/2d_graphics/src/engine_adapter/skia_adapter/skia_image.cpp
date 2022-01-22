/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "skia_image.h"

#include "skia_bitmap.h"

#include "image/bitmap.h"
#include "image/image.h"
#include "image/picture.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaImage::SkiaImage() noexcept : skiaImage_(nullptr) {}

void* SkiaImage::BuildFromBitmap(const Bitmap& bitmap)
{
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        const SkBitmap skBitmap = skBitmapImpl->ExportSkiaBitmap();
        skiaImage_ = SkImage::MakeFromBitmap(skBitmap);
    }
    return nullptr;
}

void* SkiaImage::BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix,
    const Brush& brush, BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace)
{
    LOGI("+++++++ TestBuildFromPicture");
    auto skPictureImpl = picture.GetImpl<SkiaPicture>();
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    auto skColorSpaceImpl = colorSpace->GetImpl<SkiaColorSpace>();

    SkISize skISize = SkISize::Make(dimensions.Width(), dimensions.Height());
    SkPaint paint;
    skiaPaint_.BrushToSkPaint(brush, paint);
    SkImage::BitDepth b = static_cast<SkImage::BitDepth>(bitDepth);

    if (skPictureImpl != nullptr && skMatrixImpl != nullptr && skColorSpaceImpl != nullptr) {
        skiaImage_ = SkImage::MakeFromPicture(skPictureImpl->GetPicture(), skISize, &skMatrixImpl->ExportSkiaMatrix(),
            &paint, b, skColorSpaceImpl->GetColorSpace());
    }
    LOGI("------- TestBuildFromPicture");
    return nullptr;
}

int SkiaImage::GetWidth()
{
    return skiaImage_->width();
}

int SkiaImage::GetHeight()
{
    return skiaImage_->height();
}

const sk_sp<SkImage> SkiaImage::GetImage() const
{
    return skiaImage_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
