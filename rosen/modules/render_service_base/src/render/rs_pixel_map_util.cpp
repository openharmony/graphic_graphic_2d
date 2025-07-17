/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render/rs_pixel_map_util.h"
#include <memory>

#include "pixel_map.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "drawing/engine_adapter/impl_interface/bitmap_impl.h"
#include "image/yuv_info.h"
#include "render/rs_image.h"
#ifdef ROSEN_OHOS
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace Media;
namespace {
    constexpr float HALF_F = 2;
}

static const std::map<PixelFormat, GraphicPixelFormat> PIXELMAP_SURFACEBUFFER_FORMAT_MAP = {
    {PixelFormat::RGB_565, GRAPHIC_PIXEL_FMT_RGB_565},
    {PixelFormat::RGBA_8888, GRAPHIC_PIXEL_FMT_RGBA_8888},
    {PixelFormat::BGRA_8888, GRAPHIC_PIXEL_FMT_BGRA_8888},
    {PixelFormat::RGB_888, GRAPHIC_PIXEL_FMT_RGB_888},
    {PixelFormat::RGBA_F16, GRAPHIC_PIXEL_FMT_RGBA16_FLOAT},
    {PixelFormat::NV21, GRAPHIC_PIXEL_FMT_YCRCB_420_SP},
    {PixelFormat::NV12, GRAPHIC_PIXEL_FMT_YCBCR_420_SP},
    {PixelFormat::RGBA_1010102, GRAPHIC_PIXEL_FMT_RGBA_1010102},
    {PixelFormat::YCBCR_P010, GRAPHIC_PIXEL_FMT_YCBCR_P010},
    {PixelFormat::YCRCB_P010, GRAPHIC_PIXEL_FMT_YCRCB_P010},
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    {PixelFormat::ASTC_4x4, GRAPHIC_PIXEL_FMT_BLOB},
    {PixelFormat::ASTC_6x6, GRAPHIC_PIXEL_FMT_BLOB},
    {PixelFormat::ASTC_8x8, GRAPHIC_PIXEL_FMT_BLOB},
#endif
};

static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(
    ColorManager::ColorSpaceName colorSpaceName)
{
    switch (colorSpaceName) {
        case ColorManager::ColorSpaceName::DCI_P3:
        case ColorManager::ColorSpaceName::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case ColorManager::ColorSpaceName::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case ColorManager::ColorSpaceName::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

static Drawing::ColorType PixelFormatToDrawingColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case PixelFormat::RGBA_1010102:
            return Drawing::ColorType::COLORTYPE_RGBA_1010102;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

static Drawing::AlphaType AlphaTypeToDrawingAlphaType(AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}


struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<PixelMap> pixelMap) : pixelMap_(pixelMap)
    {
#ifdef ROSEN_OHOS
        if (pixelMap_) {
            pixelMap_->IncreaseUseCount();
        }
#endif
    }

    ~PixelMapReleaseContext()
    {
#ifdef ROSEN_OHOS
        if (pixelMap_) {
            pixelMap_->DecreaseUseCount();
        }
#endif
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<PixelMap> pixelMap_;
};

static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }
}

std::shared_ptr<Drawing::ColorSpace> RSPixelMapUtil::GetPixelmapColorSpace(
    const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (!pixelMap) {
        return Drawing::ColorSpace::CreateSRGB();
    }
    return ColorSpaceToDrawingColorSpace(pixelMap->InnerGetGrColorSpace().GetColorSpaceName());
}

std::shared_ptr<Drawing::Image> RSPixelMapUtil::ExtractDrawingImage(
    std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        return nullptr;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(pixelMap->InnerGetGrColorSpace().GetColorSpaceName()) };
    Drawing::Pixmap imagePixmap(drawingImageInfo,
        reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowStride());
    PixelMapReleaseContext* releaseContext = new PixelMapReleaseContext(pixelMap);
    auto image = Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (!image) {
        RS_LOGE("RSPixelMapUtil::ExtractDrawingImage fail");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}

std::pair<float, float> calculateRotatedDimensions(float width, float height, float rotationDegrees)
{
    float radians = rotationDegrees * M_PI / 180.0f; // Convert degrees to radians

    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);

    float newWidth = std::abs(width * cosTheta) + std::abs(height * sinTheta);
    float newHeight = std::abs(width * sinTheta) + std::abs(height * cosTheta);

    return {newWidth, newHeight};
}

void ApplyRotationTransform(TransformData& transformData,
    Drawing::Rect& dst, Drawing::Matrix& matrix, const Size& realSize)
{
    float w = dst.GetWidth();
    float h = dst.GetHeight();
    scalar x = dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F;
    scalar y = dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F;
    matrix.PreRotate(transformData.rotateD, x, y);
    dst.SetLeft(x - realSize.width / HALF_F);
    dst.SetTop(y - realSize.height / HALF_F);
    dst.SetRight(x + realSize.width / HALF_F);
    dst.SetBottom(y + realSize.height / HALF_F);
    auto [newWidth, newHeight] =
        calculateRotatedDimensions(dst.GetWidth(), dst.GetHeight(), transformData.rotateD);
    float ratiox = w / (float)newWidth;
    float ratioy = h / (float)newHeight;
    matrix.PostScale(ratiox, ratioy, x, y);
}

void ApplyFitTransform(std::shared_ptr<Media::PixelMap> pixelMap,
    Drawing::Rect& src, Drawing::Rect& dst, Drawing::Canvas& canvas)
{
    const float kEpsilon = 1e-5f;
    TransformData transformData;
    pixelMap->GetTransformData(transformData);
    Size realSize;
    pixelMap->GetAstcRealSize(realSize);
    Size pixelmapSize = {pixelMap->GetWidth(), pixelMap->GetHeight()};
    src.SetRight(src.left_ + realSize.width);
    src.SetBottom(src.top_ + realSize.height);
    transformData.rotateD = fmod(fmod(transformData.rotateD, 360.0f) + 360.0f, 360.0f); // Normalize to [0, 360)
    Drawing::Matrix matrix;
    if (std::fabs(transformData.rotateD) > kEpsilon) {
        ApplyRotationTransform(transformData, dst, matrix, realSize);
    }
    if (transformData.translateX != 0 || transformData.translateY != 0) {
        float ratiox = realSize.width /(float)pixelmapSize.width;
        float ratioy = realSize.height /(float)pixelmapSize.height;
        if (transformData.translateX > 0) {
            dst.SetLeft(dst.GetLeft() + (dst.GetWidth() - (dst.GetWidth() * ratiox)));
        } else {
            src.SetLeft(src.GetLeft() - transformData.translateX);
        }
        if (transformData.translateY > 0) {
            dst.SetTop(dst.GetTop() + (dst.GetHeight() - (dst.GetHeight() * ratioy)));
        } else {
            src.SetTop(src.GetTop() - transformData.translateY);
        }
    }
    if (transformData.flipX) {
        matrix.PostScale(-1, 1, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
            dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
    }
    if (transformData.flipY) {
        matrix.PostScale(1, -1, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
            dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
    }

    if (transformData.cropLeft >= 0 && transformData.cropTop >= 0 && transformData.cropWidth > 0 &&
        transformData.cropHeight > 0 && transformData.cropLeft + transformData.cropWidth <= realSize.width &&
        transformData.cropTop + transformData.cropHeight <= realSize.height) {
        src.SetLeft(src.GetLeft() + transformData.cropLeft);
        src.SetTop(src.GetTop() + transformData.cropTop);
        src.SetRight(src.GetLeft() + pixelmapSize.width);
        src.SetBottom(src.GetTop() + pixelmapSize.height);
    }
    canvas.ConcatMatrix(matrix);
}

void ApplyNoneFitTransformstd(std::shared_ptr<Media::PixelMap> pixelMap,
    Drawing::Rect& src, Drawing::Rect& dst, Drawing::Canvas& canvas)
{
    const float kEpsilon = 1e-5f;
    TransformData transformData;
    pixelMap->GetTransformData(transformData);
    Size realSize;
    pixelMap->GetAstcRealSize(realSize);
    Size pixelmapSize = {pixelMap->GetWidth(), pixelMap->GetHeight()};
    src.SetRight(src.left_ + realSize.width);
    src.SetBottom(src.top_ + realSize.height);
    transformData.rotateD = fmod(fmod(transformData.rotateD, 360.0f) + 360.0f, 360.0f); // Normalize to [0, 360)
    if (std::fabs(transformData.rotateD) > kEpsilon) {
        dst.SetLeft(dst.GetLeft() - (realSize.width - pixelmapSize.width) / HALF_F);
        dst.SetTop(dst.GetTop() - (realSize.height - pixelmapSize.height) / HALF_F);
        dst.SetRight(dst.GetRight() + (realSize.width - pixelmapSize.width) / HALF_F);
        dst.SetBottom(dst.GetBottom() + (realSize.height - pixelmapSize.height) / HALF_F);
    }
    Drawing::Matrix matrix;
    matrix.PostRotate(transformData.rotateD, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
                      dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
    if (transformData.flipX) {
        matrix.PostScale(-1, 1, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
                         dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
    }
    if (transformData.flipY) {
        matrix.PostScale(1, -1, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
                         dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
    }
    canvas.ConcatMatrix(matrix);
    if (transformData.cropLeft >= 0 && transformData.cropTop >= 0 && transformData.cropWidth > 0 &&
        transformData.cropHeight > 0 && transformData.cropLeft + transformData.cropWidth <= realSize.width &&
        transformData.cropTop + transformData.cropHeight <= realSize.height) {
        src.SetLeft(src.GetLeft() + transformData.cropLeft);
        src.SetTop(src.GetTop() + transformData.cropTop);
        src.SetRight(src.GetLeft() + pixelmapSize.width);
        src.SetBottom(src.GetTop() + pixelmapSize.height);
    }
    if (transformData.translateX != 0 && transformData.translateY != 0) {
        if (transformData.translateX > 0) {
            dst.SetLeft(dst.GetLeft() + transformData.translateX);
        } else {
            src.SetLeft(src.GetLeft() - transformData.translateX);
        }
        if (transformData.translateY > 0) {
            dst.SetTop(dst.GetTop() + transformData.translateY);
        } else {
            src.SetTop(src.GetTop() - transformData.translateY);
        }
    }
}

void RSPixelMapUtil::TransformDataSetForAstc(std::shared_ptr<Media::PixelMap> pixelMap,
    Drawing::Rect& src, Drawing::Rect& dst, Drawing::Canvas& canvas, ImageFit imageFit)
{
    if (imageFit != ImageFit::NONE) {
        return ApplyFitTransform(pixelMap, src, dst, canvas);
    }
    return ApplyNoneFitTransformstd(pixelMap, src, dst, canvas);
}

void RSPixelMapUtil::DrawPixelMap(Drawing::Canvas& canvas, Media::PixelMap& pixelMap,
                                  const Drawing::scalar px, const Drawing::scalar py)
{
    ImageInfo imageInfo;
    pixelMap.GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(pixelMap.InnerGetGrColorSpace().GetColorSpaceName()) };
    Drawing::Bitmap pixelBitmap;
    pixelBitmap.InstallPixels(
        drawingImageInfo, reinterpret_cast<void*>(pixelMap.GetWritablePixels()),
        static_cast<uint32_t>(pixelMap.GetRowBytes()));
    canvas.DrawBitmap(pixelBitmap, px, py);
}

bool RSPixelMapUtil::IsYUVFormat(std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        return false;
    }
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        return false;
    }
#endif
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    return imageInfo.pixelFormat == PixelFormat::NV21 || imageInfo.pixelFormat == PixelFormat::NV12
        || imageInfo.pixelFormat == PixelFormat::YCBCR_P010
        || imageInfo.pixelFormat == PixelFormat::YCRCB_P010;
}

static Drawing::YUVInfo::PlaneConfig YUVPixelFormatToPlaneConfig(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::NV12:
        case PixelFormat::YCBCR_P010:
            return Drawing::YUVInfo::PlaneConfig::Y_UV;
        case PixelFormat::NV21:
        case PixelFormat::YCRCB_P010:
            return Drawing::YUVInfo::PlaneConfig::Y_VU;
        default:
            return Drawing::YUVInfo::PlaneConfig::UNKNOWN;
    }
}

static Drawing::YUVInfo::SubSampling YUVPixelFormatToSubSampling(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::NV12:
        case PixelFormat::NV21:
        case PixelFormat::YCBCR_P010:
        case PixelFormat::YCRCB_P010:
            return Drawing::YUVInfo::SubSampling::K420;
        default:
            return Drawing::YUVInfo::SubSampling::UNKNOWN;
    }
}

static Drawing::YUVInfo::YUVColorSpace YUVPixelFormatToYUVColorSpace(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            return Drawing::YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE;
        case PixelFormat::YCBCR_P010:
        case PixelFormat::YCRCB_P010:
            return Drawing::YUVInfo::YUVColorSpace::BT2020_10BIT_LIMITED_YUVCOLORSPACE;
        default:
            return Drawing::YUVInfo::YUVColorSpace::IDENTITY_YUVCOLORSPACE;
    }
}

static Drawing::YUVInfo::YUVDataType YUVPixelFormatToYUVDataType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            return Drawing::YUVInfo::YUVDataType::UNORM_8;
        case PixelFormat::YCBCR_P010:
        case PixelFormat::YCRCB_P010:
            return Drawing::YUVInfo::YUVDataType::UNORM_16;
        default:
            return Drawing::YUVInfo::YUVDataType::UNORM_8;
    }
}

std::shared_ptr<Drawing::Image> RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(
    std::shared_ptr<Drawing::GPUContext> gpuContext, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (gpuContext && pixelMap && IsYUVFormat(pixelMap)) {
        ImageInfo imageInfo;
        pixelMap->GetImageInfo(imageInfo);
        Drawing::YUVInfo info(pixelMap->GetWidth(), pixelMap->GetHeight(),
            YUVPixelFormatToPlaneConfig(imageInfo.pixelFormat),
            YUVPixelFormatToSubSampling(imageInfo.pixelFormat),
            YUVPixelFormatToYUVColorSpace(imageInfo.pixelFormat),
            YUVPixelFormatToYUVDataType(imageInfo.pixelFormat));
#ifdef RS_ENABLE_GPU
        return Drawing::Image::MakeFromYUVAPixmaps(*gpuContext, info,
            const_cast<void *>(reinterpret_cast<const void*>(pixelMap->GetPixels())));
#endif
    }
    return nullptr;
}

bool RSPixelMapUtil::IsSupportZeroCopy(std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::SamplingOptions& sampling)
{
    if (!(pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC)) {
        return false;
    }
    if (!CheckFormatConsistency(pixelMap)) {
        RS_LOGE("RSPixelMapUtil::CheckFormatConsistency fail");
        return false;
    }
    return RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL;
}

bool RSPixelMapUtil::CheckFormatConsistency(std::shared_ptr<Media::PixelMap> pixelMap)
{
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (pixelMap->GetFd() == nullptr ||
        !(pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC)) {
        return false;
    }
    SurfaceBuffer* surfaceBuffer = static_cast<SurfaceBuffer*>(pixelMap->GetFd());
    if (auto it = PIXELMAP_SURFACEBUFFER_FORMAT_MAP.find(pixelMap->GetPixelFormat());
        it != PIXELMAP_SURFACEBUFFER_FORMAT_MAP.end()) {
        return it->second == static_cast<GraphicPixelFormat>(surfaceBuffer->GetFormat());
    }
    return false;
#endif
    return false;
}
} // namespace Rosen
} // namespace OHOS
