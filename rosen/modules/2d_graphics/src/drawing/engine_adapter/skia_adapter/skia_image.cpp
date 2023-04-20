/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifdef ACE_ENABLE_GPU
#include "include/gpu/GrBackendSurface.h"
#endif

#include "skia_bitmap.h"
#include "skia_data.h"
#ifdef ACE_ENABLE_GPU
#include "skia_gpu_context.h"
#endif

#include "image/bitmap.h"
#include "image/image.h"
#include "image/picture.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaImage::SkiaImage() noexcept : skiaImage_(nullptr) {}

SkiaImage::SkiaImage(sk_sp<SkImage> skImg) noexcept : skiaImage_(skImg) {}

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
    LOGD("+++++++ TestBuildFromPicture");
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
    LOGD("------- TestBuildFromPicture");
    return nullptr;
}

#ifdef ACE_ENABLE_GPU
bool SkiaImage::BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap)
{
    if (gpuContext.GetImpl<SkiaGPUContext>() == nullptr) {
        LOGE("SkiaImage::BuildFromBitmap, build failed, GPUContext is invalid");
        return false;
    }
    if (bitmap.GetImpl<SkiaBitmap>() == nullptr) {
        LOGE("SkiaImage::BuildFromBitmap, build failed, bitmap is invalid");
        return false;
    }
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();

    skiaImage_ = SkImage::MakeCrossContextFromPixmap(grContext_.get(), skBitmap.pixmap(), false);

    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
    CompressedType type)
{
    if (gpuContext.GetImpl<SkiaGPUContext>() == nullptr) {
        LOGE("SkiaImage::BuildFromCompressed, build failed, GPUContext is invalid");
        return false;
    }
    if (data == nullptr || data->GetImpl<SkiaData>() == nullptr) {
        LOGE("SkiaImage::BuildFromCompressed, build failed, data is invalid");
        return false;
    }

    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto skData = data->GetImpl<SkiaData>()->GetSkData();

    skiaImage_ = SkImage::MakeFromCompressed(grContext_.get(),
        skData, width, height, static_cast<SkImage::CompressionType>(type));

    return (skiaImage_ != nullptr) ? true : false;
}
#endif

int SkiaImage::GetWidth() const
{
    return (skiaImage_ == nullptr) ? 0 : skiaImage_->width();
}

int SkiaImage::GetHeight() const
{
    return (skiaImage_ == nullptr) ? 0 : skiaImage_->height();
}

uint32_t SkiaImage::GetUniqueID() const
{
    return (skiaImage_ == nullptr) ? 0 : skiaImage_->uniqueID();
}

bool SkiaImage::ReadPixels(Bitmap& bitmap, int x, int y)
{
    auto skiaBitmap = bitmap.GetImpl<SkiaBitmap>();
    if (skiaBitmap == nullptr) {
        LOGE("SkiaImage::ReadPixels, bitmap is invalid");
        return false;
    }

    const auto& skBitmap = skiaBitmap->ExportSkiaBitmap();
    const auto& skPixmap = skBitmap.pixmap();

    return (skiaImage_ == nullptr) ? false : skiaImage_->readPixels(skPixmap, x, y);
}

bool SkiaImage::IsTextureBacked() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isTextureBacked();
}

const sk_sp<SkImage> SkiaImage::GetImage() const
{
    return skiaImage_;
}

void SkiaImage::SetSkImage(const sk_sp<SkImage>& skImage)
{
    skiaImage_ = skImage;
}

#ifdef ACE_ENABLE_GPU
sk_sp<GrContext> SkiaImage::GetGrContext() const
{
    return grContext_;
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
