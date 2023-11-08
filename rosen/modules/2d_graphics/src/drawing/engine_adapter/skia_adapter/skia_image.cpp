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
#include "skia_image_info.h"

#ifdef ACE_ENABLE_GPU
#include "skia_gpu_context.h"
#endif
#ifdef ROSEN_OHOS
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
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
    return nullptr;
}

#ifdef ACE_ENABLE_GPU
bool SkiaImage::BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap)
{
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();

    skiaImage_ = SkImage::MakeCrossContextFromPixmap(grContext_.get(), skBitmap.pixmap(), false);

    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::MakeFromEncoded(const std::shared_ptr<Data>& data)
{
    if (data == nullptr) {
        LOGE("SkiaImage::MakeFromEncoded failed, data is invalid");
        return false;
    }

    auto skData = data->GetImpl<SkiaData>()->GetSkData();
    skiaImage_ = SkImage::MakeFromEncoded(skData);
    return (skiaImage_ != nullptr);
}

bool SkiaImage::BuildSubset(const std::shared_ptr<Image> image, const RectI& rect, GPUContext& gpuContext)
{
    if (image == nullptr) {
        LOGE("SkiaImage::BuildSubset failed, origin Image is invaild");
        return false;
    }
    auto skiaImage = image->GetImpl<SkiaImage>()->GetImage();
    auto skiaRect = SkIRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    skiaImage_ = skiaImage->makeSubset(skiaRect, grContext_.get());
    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
    CompressedType type)
{
    if (data == nullptr) {
        LOGE("SkiaImage::BuildFromCompressed, build failed, data is invalid");
        return false;
    }

    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto skData = data->GetImpl<SkiaData>()->GetSkData();
#ifdef NEW_SKIA
    skiaImage_ = SkImage::MakeTextureFromCompressed(grContext_.get(),
        skData, width, height, static_cast<SkImage::CompressionType>(type));
#else
    skiaImage_ = SkImage::MakeFromCompressed(grContext_.get(),
        skData, width, height, static_cast<SkImage::CompressionType>(type));
#endif
    return (skiaImage_ != nullptr) ? true : false;
}

static GrBackendTexture ConvertToGrBackendTexture(const TextureInfo& info)
{
    GrGLTextureInfo grGLTextureInfo = { info.GetTarget(), info.GetID(), info.GetFormat() };
    GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(), static_cast<GrMipMapped>(info.GetIsMipMapped()),
        grGLTextureInfo);

    return backendTexture;
}

static GrSurfaceOrigin ConvertToGrSurfaceOrigin(const TextureOrigin& origin)
{
    switch (origin) {
        case TextureOrigin::TOP_LEFT:
            return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
        case TextureOrigin::BOTTOM_LEFT:
            return GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;
        default:
            return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
    }
}

bool SkiaImage::BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace)
{
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (colorSpace != nullptr) {
        skColorSpace = colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    }

    skiaImage_ = SkImage::MakeFromTexture(grContext_.get(), ConvertToGrBackendTexture(info),
        ConvertToGrSurfaceOrigin(origin), SkiaImageInfo::ConvertToSkColorType(bitmapFormat.colorType),
        SkiaImageInfo::ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace);

    return (skiaImage_ != nullptr) ? true : false;
}

static TextureInfo ConvertToTextureInfo(const GrBackendTexture& grBackendTexture)
{
    GrGLTextureInfo* grGLTextureInfo = new GrGLTextureInfo();
    grBackendTexture.getGLTextureInfo(grGLTextureInfo);
    TextureInfo textureInfo;
    textureInfo.SetWidth(grBackendTexture.width());
    textureInfo.SetHeight(grBackendTexture.height());
    textureInfo.SetIsMipMapped(static_cast<bool>(grBackendTexture.mipmapped()));
    textureInfo.SetTarget(grGLTextureInfo->fTarget);
    textureInfo.SetID(grGLTextureInfo->fID);
    textureInfo.SetFormat(grGLTextureInfo->fFormat);

    return textureInfo;
}

void SkiaImage::SetGrBackendTexture(const GrBackendTexture& grBackendTexture)
{
    grBackendTexture_ = grBackendTexture;
}

BackendTexture SkiaImage::GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin)
{
    if (skiaImage_ == nullptr) {
        LOGE("SkiaImage::GetBackendTexture, SkImage is nullptr!");
        return BackendTexture(false); // invalid
    }
    GrBackendTexture skBackendTexture;
    if (origin == nullptr) {
        skBackendTexture =
            skiaImage_->getBackendTexture(flushPendingGrContextIO);
    } else {
        GrSurfaceOrigin grOrigin = ConvertToGrSurfaceOrigin(*origin);
        skBackendTexture =
            skiaImage_->getBackendTexture(flushPendingGrContextIO, &grOrigin);
    }
    if (!skBackendTexture.isValid()) {
        LOGE("SkiaImage::GetBackendTexture, skBackendTexture is nullptr!");
        return BackendTexture(false); // invalid
    }
    auto backendTexture = BackendTexture(true);
    SetGrBackendTexture(skBackendTexture);
    backendTexture.SetTextureInfo(ConvertToTextureInfo(skBackendTexture));
    return backendTexture;
}

bool SkiaImage::IsValid(GPUContext* context) const
{
    if (skiaImage_ == nullptr) {
        LOGE("SkiaImage::IsValid, skiaImage_ is nullptr!");
        return false;
    }
    if (context == nullptr) {
        return skiaImage_->isValid(nullptr);
    }
    return skiaImage_->isValid(context->GetImpl<SkiaGPUContext>()->GetGrContext().get());
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

ColorType SkiaImage::GetColorType() const
{
    return (skiaImage_ == nullptr) ? ColorType::COLORTYPE_UNKNOWN :
                                     SkiaImageInfo::ConvertToColorType(skiaImage_->colorType());
}

AlphaType SkiaImage::GetAlphaType() const
{
    return (skiaImage_ == nullptr) ? AlphaType::ALPHATYPE_UNKNOWN :
                                     SkiaImageInfo::ConvertToAlphaType(skiaImage_->alphaType());
}

uint32_t SkiaImage::GetUniqueID() const
{
    return (skiaImage_ == nullptr) ? 0 : skiaImage_->uniqueID();
}

ImageInfo SkiaImage::GetImageInfo()
{
    if (skiaImage_ == nullptr) {
        return {};
    }
    return SkiaImageInfo::ConvertToRSImageInfo(skiaImage_->imageInfo());
}

bool SkiaImage::ReadPixels(Bitmap& bitmap, int x, int y)
{
    const auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    const auto& skPixmap = skBitmap.pixmap();

    return (skiaImage_ == nullptr) ? false : skiaImage_->readPixels(skPixmap, x, y);
}

bool SkiaImage::IsTextureBacked() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isTextureBacked();
}

bool SkiaImage::ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling, bool allowCachingHint) const
{
    const auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    const auto& skPixmap = skBitmap.pixmap();

    SkSamplingOptions samplingOptions;
    if (sampling.GetUseCubic()) {
        samplingOptions = SkSamplingOptions({ sampling.GetCubicCoffB(), sampling.GetCubicCoffC() });
    } else {
        samplingOptions = SkSamplingOptions(static_cast<SkFilterMode>(sampling.GetFilterMode()),
            static_cast<SkMipmapMode>(sampling.GetMipmapMode()));
    }

    SkImage::CachingHint skCachingHint;
    if (allowCachingHint) {
        skCachingHint = SkImage::CachingHint::kAllow_CachingHint;
    } else {
        skCachingHint = SkImage::CachingHint::kDisallow_CachingHint;
    }

    return (skiaImage_ == nullptr) ? false : skiaImage_->scalePixels(skPixmap, samplingOptions, skCachingHint);
}

std::shared_ptr<Data> SkiaImage::EncodeToData(EncodedImageFormat& encodedImageFormat, int quality) const
{
    SkEncodedImageFormat skEncodedImageFormat = SkiaImageInfo::ConvertToSkEncodedImageFormat(encodedImageFormat);
    if (skiaImage_ == nullptr) {
        LOGE("SkiaImage::EncodeToData, skiaImage_ is null!");
        return nullptr;
    }
    auto skData = skiaImage_->encodeToData(skEncodedImageFormat, quality);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);
    return data;
}

bool SkiaImage::IsLazyGenerated() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isLazyGenerated();
}

bool SkiaImage::IsOpaque() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isOpaque();
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
#ifdef NEW_SKIA
sk_sp<GrDirectContext> SkiaImage::GetGrContext() const
#else
sk_sp<GrContext> SkiaImage::GetGrContext() const
#endif
{
    return grContext_;
}
#endif

std::shared_ptr<Data> SkiaImage::Serialize() const
{
#ifdef ROSEN_OHOS
    if (skiaImage_ == nullptr) {
        LOGE("SkiaImage::Serialize, SkImage is nullptr!");
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    writer.writeImage(skiaImage_.get());
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
#else
    return nullptr;
#endif
}

bool SkiaImage::Deserialize(std::shared_ptr<Data> data)
{
#ifdef ROSEN_OHOS
    if (data == nullptr) {
        LOGE("SkiaImage::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    skiaImage_ = reader.readImage();
    return skiaImage_ != nullptr;
#else
    return false;
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
