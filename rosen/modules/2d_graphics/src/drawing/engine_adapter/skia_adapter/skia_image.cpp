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
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();

    skiaImage_ = SkImage::MakeCrossContextFromPixmap(grContext_.get(), skBitmap.pixmap(), false);

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

static SkColorType ConvertToSkColorType(const ColorType& format)
{
    switch (format) {
        case COLORTYPE_UNKNOWN:
            return kUnknown_SkColorType;
        case COLORTYPE_ALPHA_8:
            return kAlpha_8_SkColorType;
        case COLORTYPE_RGB_565:
            return kRGB_565_SkColorType;
        case COLORTYPE_ARGB_4444:
            return kARGB_4444_SkColorType;
        case COLORTYPE_RGBA_8888:
            return kRGBA_8888_SkColorType;
        case COLORTYPE_BGRA_8888:
            return kBGRA_8888_SkColorType;
        case COLORTYPE_N32:
            return kN32_SkColorType;
        default:
            return kUnknown_SkColorType;
    }
}

static SkAlphaType ConvertToSkAlphaType(const AlphaType& format)
{
    switch (format) {
        case ALPHATYPE_UNKNOWN:
            return kUnknown_SkAlphaType;
        case ALPHATYPE_OPAQUE:
            return kOpaque_SkAlphaType;
        case ALPHATYPE_PREMUL:
            return kPremul_SkAlphaType;
        case ALPHATYPE_UNPREMUL:
            return kUnpremul_SkAlphaType;
        default:
            return kUnknown_SkAlphaType;
    }
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
        ConvertToGrSurfaceOrigin(origin), ConvertToSkColorType(bitmapFormat.colorType),
        ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace);

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

static ColorType ConvertToColorType(const SkColorType& format)
{
    switch (format) {
        case kUnknown_SkColorType:
            return COLORTYPE_UNKNOWN;
        case kAlpha_8_SkColorType:
            return COLORTYPE_ALPHA_8;
        case kRGB_565_SkColorType:
            return COLORTYPE_RGB_565;
        case kARGB_4444_SkColorType:
            return COLORTYPE_ARGB_4444;
        case kRGBA_8888_SkColorType:
            return COLORTYPE_RGBA_8888;
        case kBGRA_8888_SkColorType:
            return COLORTYPE_BGRA_8888;
        default:
            return COLORTYPE_UNKNOWN;
    }
}

static AlphaType ConvertToAlphaType(const SkAlphaType& format)
{
    switch (format) {
        case kUnknown_SkAlphaType:
            return ALPHATYPE_UNKNOWN;
        case kOpaque_SkAlphaType:
            return ALPHATYPE_OPAQUE;
        case kPremul_SkAlphaType:
            return ALPHATYPE_PREMUL;
        case kUnpremul_SkAlphaType:
            return ALPHATYPE_UNPREMUL;
        default:
            return ALPHATYPE_UNKNOWN;
    }
}

ColorType SkiaImage::GetColorType() const
{
    return (skiaImage_ == nullptr) ? ColorType::COLORTYPE_UNKNOWN : ConvertToColorType(skiaImage_->colorType());
}

AlphaType SkiaImage::GetAlphaType() const
{
    return (skiaImage_ == nullptr) ? AlphaType::ALPHATYPE_UNKNOWN : ConvertToAlphaType(skiaImage_->alphaType());
}

uint32_t SkiaImage::GetUniqueID() const
{
    return (skiaImage_ == nullptr) ? 0 : skiaImage_->uniqueID();
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
