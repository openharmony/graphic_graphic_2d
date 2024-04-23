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

#include "skia_bitmap.h"
#include "skia_data.h"
#include "skia_image_info.h"
#include "skia_pixmap.h"
#include "skia_surface.h"
#include "skia_texture_info.h"
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "src/image/SkImage_Base.h"

#include "draw/surface.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "image/picture.h"
#include "utils/data.h"
#include "utils/log.h"
#include "utils/system_properties.h"

#ifdef ACE_ENABLE_GPU
#include "include/core/SkYUVAPixmaps.h"
#include "skia_gpu_context.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaImage::SkiaImage() noexcept : skiaImage_(nullptr) {}

SkiaImage::SkiaImage(sk_sp<SkImage> skImg) noexcept
{
    PostSkImgToTargetThread();
    skiaImage_ = skImg;
}

SkiaImage::~SkiaImage()
{
    PostSkImgToTargetThread();
}

void SkiaImage::PostSkImgToTargetThread()
{
    if (skiaImage_ == nullptr) {
        return;
    }
    auto context = as_IB(skiaImage_.get())->directContext();
    auto func = SkiaGPUContext::GetPostFunc(sk_ref_sp(context));
    if (func) {
        auto image = skiaImage_;
        func([image]() {});
    }
}

std::shared_ptr<Image> SkiaImage::MakeFromRaster(const Pixmap& pixmap,
    RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext)
{
    auto& skPixmap = pixmap.GetImpl<SkiaPixmap>()->ExportSkiaPixmap();
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skPixmap, rasterReleaseProc, releaseContext);
    if (skImage == nullptr) {
        LOGD("SkiaImage::MakeFromRaster failed");
        return nullptr;
    }
    std::shared_ptr<ImageImpl> imageImpl = std::make_shared<SkiaImage>(skImage);
    return std::make_shared<Image>(imageImpl);
}

std::shared_ptr<Image> SkiaImage::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
    if (pixels == nullptr) {
        LOGD("SkiaImage::MakeRasterData pixels is nullptr");
        return nullptr;
    }
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(info);
    auto skData = pixels->GetImpl<SkiaData>()->GetSkData();

    sk_sp<SkImage> skImage = SkImage::MakeRasterData(skImageInfo, skData, rowBytes);
    if (skImage == nullptr) {
        LOGD("skImage nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<ImageImpl> imageImpl = std::make_shared<SkiaImage>(skImage);
    return std::make_shared<Image>(imageImpl);
}

bool SkiaImage::BuildFromBitmap(const Bitmap& bitmap)
{
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        const SkBitmap skBitmap = skBitmapImpl->ExportSkiaBitmap();
        PostSkImgToTargetThread();
        skiaImage_ = SkImage::MakeFromBitmap(skBitmap);
        return skiaImage_ != nullptr;
    }
    return false;
}

#ifdef ACE_ENABLE_GPU
std::shared_ptr<Image> SkiaImage::MakeFromYUVAPixmaps(GPUContext& gpuContext, const YUVInfo& info, void* memory)
{
    if (!memory) {
        LOGD("memory nullprt, %{public}s, %{public}d",  __FUNCTION__, __LINE__);
        return nullptr;
    }
    
    auto grContext = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    if (grContext == nullptr) {
        LOGD("grContext nullprt, %{public}s, %{public}d",  __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkYUVAPixmapInfo pixmapInfo({{info.GetWidth(), info.GetHeight()},
                                 SkiaYUVInfo::ConvertToSkPlaneConfig(info.GetConfig()),
                                 SkiaYUVInfo::ConvertToSkSubSampling(info.GetSampling()),
                                 SkiaYUVInfo::ConvertToSkYUVColorSpace(info.GetColorSpace())},
                                 SkYUVAPixmapInfo::DataType::kUnorm8,
                                 nullptr);
    auto skYUVAPixmaps = SkYUVAPixmaps::FromExternalMemory(pixmapInfo, memory);
    auto skImage = SkImage::MakeFromYUVAPixmaps(grContext.get(), skYUVAPixmaps);
    if (skImage == nullptr) {
        LOGD("skImage nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<ImageImpl> imageImpl = std::make_shared<SkiaImage>(skImage);
    return std::make_shared<Image>(imageImpl);
}

bool SkiaImage::BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap)
{
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto& skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    PostSkImgToTargetThread();
    skiaImage_ = SkImage::MakeCrossContextFromPixmap(grContext_.get(), skBitmap.pixmap(), false);

    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::MakeFromEncoded(const std::shared_ptr<Data>& data)
{
    if (data == nullptr) {
        LOGD("SkiaImage::MakeFromEncoded failed, data is invalid");
        return false;
    }

    auto skData = data->GetImpl<SkiaData>()->GetSkData();
    PostSkImgToTargetThread();
    skiaImage_ = SkImage::MakeFromEncoded(skData);
    return (skiaImage_ != nullptr);
}

bool SkiaImage::BuildSubset(const std::shared_ptr<Image> image, const RectI& rect, GPUContext& gpuContext)
{
    if (image == nullptr) {
        LOGD("SkiaImage::BuildSubset failed, origin Image is invaild");
        return false;
    }
    auto skiaImage = image->GetImpl<SkiaImage>()->GetImage();
    auto skiaRect = SkIRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    PostSkImgToTargetThread();
    skiaImage_ = skiaImage->makeSubset(skiaRect, grContext_.get());
    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
    CompressedType type)
{
    if (data == nullptr) {
        LOGD("SkiaImage::BuildFromCompressed, build failed, data is invalid");
        return false;
    }
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    auto skData = data->GetImpl<SkiaData>()->GetSkData();
    PostSkImgToTargetThread();
#ifdef NEW_SKIA
    skiaImage_ = SkImage::MakeTextureFromCompressed(grContext_.get(),
        skData, width, height, static_cast<SkImage::CompressionType>(type));
#else
    skiaImage_ = SkImage::MakeFromCompressed(grContext_.get(),
        skData, width, height, static_cast<SkImage::CompressionType>(type));
#endif
    return (skiaImage_ != nullptr) ? true : false;
}

bool SkiaImage::BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace,
    void (*deleteFunc)(void*), void* cleanupHelper)
{
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();
    if (!grContext_) {
        LOGD("SkiaImage BuildFromTexture grContext_ is null");
        return false;
    }

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (colorSpace != nullptr) {
        skColorSpace = colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    } else {
        skColorSpace = SkColorSpace::MakeSRGB();
    }

#ifdef RS_ENABLE_VK
    if (SystemProperties::IsUseVulkan()) {
        const auto& backendTexture = SkiaTextureInfo::ConvertToGrBackendTexture(info);
        if (!backendTexture.isValid()) {
            LOGE("SkiaImage BuildFromTexture backend texture is not valid!!!!");
            return false;
        }
        PostSkImgToTargetThread();
        skiaImage_ = SkImage::MakeFromTexture(grContext_.get(), backendTexture,
            SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin),
            SkiaImageInfo::ConvertToSkColorType(bitmapFormat.colorType),
            SkiaImageInfo::ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace, deleteFunc, cleanupHelper);
    } else {
        PostSkImgToTargetThread();
        skiaImage_ = SkImage::MakeFromTexture(grContext_.get(),  SkiaTextureInfo::ConvertToGrBackendTexture(info),
            SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin),
            SkiaImageInfo::ConvertToSkColorType(bitmapFormat.colorType),
            SkiaImageInfo::ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace);
    }
#else
    PostSkImgToTargetThread();
    skiaImage_ = SkImage::MakeFromTexture(grContext_.get(),  SkiaTextureInfo::ConvertToGrBackendTexture(info),
        SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin), SkiaImageInfo::ConvertToSkColorType(bitmapFormat.colorType),
        SkiaImageInfo::ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace);
#endif
    if (skiaImage_ == nullptr) {
        LOGE("SkiaImage::MakeFromTexture skiaImage_ is nullptr!!!! "
            "TextureInfo format:%u, w:%d, h:%d , bitmapFormat.colorType is %d",
            info.GetFormat(), info.GetWidth(), info.GetHeight(), static_cast<int>(bitmapFormat.colorType));
        return false;
    }
    return true;
}

bool SkiaImage::BuildFromSurface(GPUContext& gpuContext, Surface& surface, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace)
{
    auto skSurface = surface.GetImpl<SkiaSurface>()->GetSkSurface();
    if (!skSurface) {
        LOGD("SkiaImage::BuildFromSurface skSurface is null");
        return false;
    }

    GrBackendTexture grBackendTexture
            = skSurface->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
    if (!grBackendTexture.isValid()) {
        LOGD("SkiaImage::BuildFromSurface grBackendTexture is invalid");
        return false;
    }
    grContext_ = gpuContext.GetImpl<SkiaGPUContext>()->GetGrContext();

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (colorSpace != nullptr) {
        skColorSpace = colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    }
    PostSkImgToTargetThread();
    skiaImage_ = SkImage::MakeFromTexture(grContext_.get(), grBackendTexture,
        SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin), SkiaImageInfo::ConvertToSkColorType(bitmapFormat.colorType),
        SkiaImageInfo::ConvertToSkAlphaType(bitmapFormat.alphaType), skColorSpace);
    return (skiaImage_ != nullptr) ? true : false;
}

void SkiaImage::SetGrBackendTexture(const GrBackendTexture& grBackendTexture)
{
    grBackendTexture_ = grBackendTexture;
}

BackendTexture SkiaImage::GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin)
{
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::GetBackendTexture, SkImage is nullptr!");
        return BackendTexture(false); // invalid
    }
    GrBackendTexture skBackendTexture;
    if (origin == nullptr) {
        skBackendTexture =
            skiaImage_->getBackendTexture(flushPendingGrContextIO);
    } else {
        GrSurfaceOrigin grOrigin = SkiaTextureInfo::ConvertToGrSurfaceOrigin(*origin);
        skBackendTexture =
            skiaImage_->getBackendTexture(flushPendingGrContextIO, &grOrigin);
    }
    if (!skBackendTexture.isValid()) {
        LOGD("SkiaImage::GetBackendTexture, skBackendTexture is nullptr!");
        return BackendTexture(false); // invalid
    }
    auto backendTexture = BackendTexture(true);
    SetGrBackendTexture(skBackendTexture);
#ifdef RS_ENABLE_VK
    if (SystemProperties::IsUseVulkan()) {
        TextureInfo info;
        SkiaTextureInfo::ConvertToVKTexture(skBackendTexture, info);
        backendTexture.SetTextureInfo(info);
    } else {
        backendTexture.SetTextureInfo(SkiaTextureInfo::ConvertToTextureInfo(skBackendTexture));
    }
#else
    backendTexture.SetTextureInfo(SkiaTextureInfo::ConvertToTextureInfo(skBackendTexture));
#endif
    return backendTexture;
}

bool SkiaImage::IsValid(GPUContext* context) const
{
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::IsValid, skiaImage_ is nullptr!");
        return false;
    }
    if (context == nullptr) {
        return skiaImage_->isValid(nullptr);
    }
    return skiaImage_->isValid(context->GetImpl<SkiaGPUContext>()->GetGrContext().get());
}
#endif

bool SkiaImage::AsLegacyBitmap(Bitmap& bitmap) const
{
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::IsValid, skiaImage_ is nullptr!");
        return false;
    }
    SkBitmap newBitmap;
    if (!skiaImage_->asLegacyBitmap(&newBitmap)) {
        LOGD("SkiaImage::AsLegacyBitmap failed!");
        return false;
    }
    bitmap.GetImpl<SkiaBitmap>()->SetSkBitmap(newBitmap);
    return true;
}

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

std::shared_ptr<ColorSpace> SkiaImage::GetColorSpace() const
{
    if (skiaImage_ == nullptr) {
        return nullptr;
    }
    sk_sp<SkColorSpace> skColorSpace = skiaImage_->refColorSpace();
    if (skColorSpace == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    colorSpace->GetImpl<SkiaColorSpace>()->SetColorSpace(skColorSpace);
    return colorSpace;
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

bool SkiaImage::ReadPixels(Pixmap& pixmap, int x, int y)
{
    auto& skPixmap = pixmap.GetImpl<SkiaPixmap>()->ExportSkiaPixmap();
    return (skiaImage_ == nullptr) ? false : skiaImage_->readPixels(skPixmap, x, y);
}

bool SkiaImage::ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
    int32_t srcX, int32_t srcY) const
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(dstInfo);
    return (skiaImage_ == nullptr) ? false : skiaImage_->readPixels(skImageInfo, dstPixels, dstRowBytes, srcX, srcY);
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

std::shared_ptr<Data> SkiaImage::EncodeToData(EncodedImageFormat encodedImageFormat, int quality) const
{
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::EncodeToData, skiaImage_ is null!");
        return nullptr;
    }
    SkEncodedImageFormat skEncodedImageFormat = SkiaImageInfo::ConvertToSkEncodedImageFormat(encodedImageFormat);
    auto skData = skiaImage_->encodeToData(skEncodedImageFormat, quality);
    if (skData == nullptr) {
        LOGD("SkiaImage::EncodeToData, skData null!");
        return nullptr;
    }
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);
    return data;
}

bool SkiaImage::IsLazyGenerated() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isLazyGenerated();
}

bool SkiaImage::GetROPixels(Bitmap& bitmap) const
{
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::GetROPixels, skiaImage_ is null!");
        return false;
    }
    auto context = as_IB(skiaImage_.get())->directContext();
    SkBitmap skiaBitmap;
    if (!as_IB(skiaImage_.get())->getROPixels(context, &skiaBitmap)) {
        LOGD("skiaImge getROPixels failed");
        return false;
    }
    bitmap.GetImpl<SkiaBitmap>()->SetSkBitmap(skiaBitmap);
    return true;
}

std::shared_ptr<Image> SkiaImage::MakeRasterImage() const
{
    if (skiaImage_ == nullptr) {
        return nullptr;
    }
    sk_sp<SkImage> skImage = skiaImage_->makeRasterImage();
    if (skImage == nullptr) {
        LOGD("skImage nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<ImageImpl> imageImpl = std::make_shared<SkiaImage>(skImage);
    return std::make_shared<Image>(imageImpl);
}

bool SkiaImage::CanPeekPixels() const
{
    SkPixmap pixmap;
    if (skiaImage_ == nullptr || !skiaImage_->peekPixels(&pixmap)) {
        return false;
    }
    return true;
}

bool SkiaImage::IsOpaque() const
{
    return (skiaImage_ == nullptr) ? false : skiaImage_->isOpaque();
}

void SkiaImage::HintCacheGpuResource() const
{
    as_IB(skiaImage_.get())->hintCacheGpuResource();
}

const sk_sp<SkImage> SkiaImage::GetImage() const
{
    return skiaImage_;
}

void SkiaImage::SetSkImage(const sk_sp<SkImage>& skImage)
{
    PostSkImgToTargetThread();
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
    if (skiaImage_ == nullptr) {
        LOGD("SkiaImage::Serialize, SkImage is nullptr!");
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    bool type = skiaImage_->isLazyGenerated();
    writer.writeBool(type);
    if (type) {
        writer.writeImage(skiaImage_.get());
        size_t length = writer.bytesWritten();
        std::shared_ptr<Data> data = std::make_shared<Data>();
        data->BuildUninitialized(length);
        writer.writeToMemory(data->WritableData());
        return data;
    } else {
        SkBitmap bitmap;

        auto context = as_IB(skiaImage_.get())->directContext();
        if (!as_IB(skiaImage_.get())->getROPixels(context, &bitmap)) {
            LOGD("SkiaImage::SerializeNoLazyImage SkImage getROPixels failed");
            return nullptr;
        }
        SkPixmap pixmap;
        if (!bitmap.peekPixels(&pixmap)) {
            LOGD("SkiaImage::SerializeNoLazyImage SkImage peekPixels failed");
            return nullptr;
        }
        size_t rb = pixmap.rowBytes();
        int32_t width = pixmap.width();
        int32_t height = pixmap.height();
        const void* addr = pixmap.addr();
        size_t size = rb * static_cast<size_t>(height);

        writer.writeUInt(size);
        writer.writeByteArray(addr, size);
        writer.writeUInt(rb);
        writer.write32(width);
        writer.write32(height);

        writer.writeUInt(pixmap.colorType());
        writer.writeUInt(pixmap.alphaType());

        if (pixmap.colorSpace() == nullptr) {
            writer.writeUInt(0);
        } else {
            auto data = pixmap.colorSpace()->serialize();
            writer.writeUInt(data->size());
            writer.writeByteArray(data->data(), data->size());
        }
        size_t length = writer.bytesWritten();
        std::shared_ptr<Data> data = std::make_shared<Data>();
        data->BuildUninitialized(length);
        writer.writeToMemory(data->WritableData());
        return data;
    }
}

bool SkiaImage::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaImage::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    bool type = reader.readBool();

    if (type) {
        PostSkImgToTargetThread();
        skiaImage_ = reader.readImage();
        return skiaImage_ != nullptr;
    } else {
        size_t pixmapSize = reader.readUInt();
        SkAutoMalloc pixBuffer(pixmapSize);
        if (!reader.readByteArray(pixBuffer.get(), pixmapSize)) {
            return false;
        }

        size_t rb = reader.readUInt();
        int32_t width = reader.read32();
        int32_t height = reader.read32();

        SkColorType colorType = static_cast<SkColorType>(reader.readUInt());
        SkAlphaType alphaType = static_cast<SkAlphaType>(reader.readUInt());
        sk_sp<SkColorSpace> colorSpace;

        size_t size = reader.readUInt();
        if (size == 0) {
            colorSpace = nullptr;
        } else {
            SkAutoMalloc colorBuffer(size);
            if (!reader.readByteArray(colorBuffer.get(), size)) {
                return false;
            }
            colorSpace = SkColorSpace::Deserialize(colorBuffer.get(), size);
        }

        SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
        auto skData = SkData::MakeWithCopy(const_cast<void*>(pixBuffer.get()), pixmapSize);
        PostSkImgToTargetThread();
        skiaImage_ = SkImage::MakeRasterData(imageInfo, skData, rb);
        return true;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
