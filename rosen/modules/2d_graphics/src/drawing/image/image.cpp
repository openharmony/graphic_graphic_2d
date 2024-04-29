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

#include "image/image.h"

#include "engine_adapter/skia_adapter/skia_gpu_context.h"
#include "image/gpu_context.h"
#include "impl_factory.h"
#include "skia_adapter/skia_image.h"
#include "static_factory.h"
#include "src/core/SkImagePriv.h"
#include "src/image/SkImage_Base.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
BackendTexture::BackendTexture() noexcept
    : isValid_(false) {}

BackendTexture::BackendTexture(bool isValid) noexcept
    : isValid_(isValid) {}

bool BackendTexture::IsValid() const
{
    return isValid_;
}

void BackendTexture::SetTextureInfo(const TextureInfo& textureInfo)
{
    textureInfo_ = textureInfo;
}

const TextureInfo& BackendTexture::GetTextureInfo() const
{
    return textureInfo_;
}

Image::Image() noexcept : imageImplPtr(ImplFactory::CreateImageImpl()) {}

Image::Image(std::shared_ptr<ImageImpl> imageImpl) : imageImplPtr(imageImpl) {}

Image::Image(void* rawImg) noexcept : imageImplPtr(ImplFactory::CreateImageImpl(rawImg)) {}

bool Image::BuildFromBitmap(const Bitmap& bitmap)
{
    return imageImplPtr->BuildFromBitmap(bitmap);
}

std::shared_ptr<Image> Image::MakeFromRaster(const Pixmap& pixmap,
    RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext)
{
    return StaticFactory::MakeFromRaster(pixmap, rasterReleaseProc, releaseContext);
}

std::shared_ptr<Image> Image::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
    return StaticFactory::MakeRasterData(info, pixels, rowBytes);
}

#ifdef ACE_ENABLE_GPU
std::shared_ptr<Image> Image::MakeFromYUVAPixmaps(GPUContext& gpuContext, const YUVInfo& info, void* memory)
{
    return StaticFactory::MakeFromYUVAPixmaps(gpuContext, info, memory);
}

bool Image::BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap)
{
    return imageImplPtr->BuildFromBitmap(gpuContext, bitmap);
}

bool Image::MakeFromEncoded(const std::shared_ptr<Data>& data)
{
    return imageImplPtr->MakeFromEncoded(data);
}

bool Image::BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
    CompressedType type)
{
    return imageImplPtr->BuildFromCompressed(gpuContext, data, width, height, type);
}

bool Image::BuildFromSurface(GPUContext& gpuContext, Surface& surface, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace)
{
    return imageImplPtr->BuildFromSurface(gpuContext, surface, origin, bitmapFormat, colorSpace);
}

bool Image::BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace,
    void (*deleteFunc)(void*), void* cleanupHelper)
{
    return imageImplPtr->BuildFromTexture(gpuContext, info, origin, bitmapFormat,
        colorSpace, deleteFunc, cleanupHelper);
}

bool Image::BuildSubset(const std::shared_ptr<Image>& image, const RectI& rect, GPUContext& gpuContext)
{
    return imageImplPtr->BuildSubset(image, rect, gpuContext);
}

BackendTexture Image::GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin) const
{
    return imageImplPtr->GetBackendTexture(flushPendingGrContextIO, origin);
}

bool Image::IsValid(GPUContext* context) const
{
    return imageImplPtr->IsValid(context);
}

bool Image::pinAsTexture(GPUContext& context)
{
    auto image = ExportSkImage().get();
    auto skGpuContext = context.GetImpl<SkiaGPUContext>();
    if (skGpuContext == nullptr) {
        return false;
    }
    auto skContext = skGpuContext->GetGrContext().get();
    return image != nullptr && skContext != nullptr && SkImage_pinAsTexture(image, skContext);
}
#endif

bool Image::AsLegacyBitmap(Bitmap& bitmap) const
{
    return imageImplPtr->AsLegacyBitmap(bitmap);
}

int Image::GetWidth() const
{
    return imageImplPtr->GetWidth();
}

int Image::GetHeight() const
{
    return imageImplPtr->GetHeight();
}

ColorType Image::GetColorType() const
{
    return imageImplPtr->GetColorType();
}

AlphaType Image::GetAlphaType() const
{
    return imageImplPtr->GetAlphaType();
}

std::shared_ptr<ColorSpace> Image::GetColorSpace() const
{
    return imageImplPtr->GetColorSpace();
}

uint32_t Image::GetUniqueID() const
{
    return imageImplPtr->GetUniqueID();
}

ImageInfo Image::GetImageInfo()
{
    return imageImplPtr->GetImageInfo();
}

bool Image::ReadPixels(Bitmap& bitmap, int x, int y)
{
    return imageImplPtr->ReadPixels(bitmap, x, y);
}

bool Image::ReadPixels(Pixmap& pixmap, int x, int y)
{
    return imageImplPtr->ReadPixels(pixmap, x, y);
}

bool Image::ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
    int32_t srcX, int32_t srcY) const
{
    return imageImplPtr->ReadPixels(dstInfo, dstPixels, dstRowBytes, srcX, srcY);
}

bool Image::IsTextureBacked() const
{
    return imageImplPtr->IsTextureBacked();
}

bool Image::ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling, bool allowCachingHint) const
{
    return imageImplPtr->ScalePixels(bitmap, sampling, allowCachingHint);
}

std::shared_ptr<Data> Image::EncodeToData(EncodedImageFormat encodedImageFormat, int quality) const
{
    return imageImplPtr->EncodeToData(encodedImageFormat, quality);
}

bool Image::IsLazyGenerated() const
{
    return imageImplPtr->IsLazyGenerated();
}

bool Image::GetROPixels(Bitmap& bitmap) const
{
    return imageImplPtr->GetROPixels(bitmap);
}

std::shared_ptr<Image> Image::MakeRasterImage() const
{
    return imageImplPtr->MakeRasterImage();
}

bool Image::CanPeekPixels() const
{
    return imageImplPtr->CanPeekPixels();
}

bool Image::IsOpaque() const
{
    return imageImplPtr->IsOpaque();
}

void Image::HintCacheGpuResource() const
{
    imageImplPtr->HintCacheGpuResource();
}

std::shared_ptr<Data> Image::Serialize() const
{
    return imageImplPtr->Serialize();
}

bool Image::Deserialize(std::shared_ptr<Data> data)
{
    return imageImplPtr->Deserialize(data);
}

const sk_sp<SkImage> Image::ExportSkImage()
{
    return GetImpl<SkiaImage>()->GetImage();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
