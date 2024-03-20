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
#include "drawing/engine_adapter/impl_interface/bitmap_impl.h"

namespace OHOS {
namespace Rosen {
using namespace Media;
namespace {
    constexpr float HALF_F = 2;
}

static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(ColorManager::ColorSpaceName
 colorSpaceName)
{
    switch (colorSpaceName) {
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
    explicit PixelMapReleaseContext(std::shared_ptr<PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
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
    Drawing::Pixmap imagePixmap(drawingImageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowStride());
    PixelMapReleaseContext* releaseContext = new PixelMapReleaseContext(pixelMap);
    auto image = Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (!image) {
        RS_LOGE("RSPixelMapUtil::ExtractDrawingImage fail");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}


void RSPixelMapUtil::TransformDataSetForAstc(std::shared_ptr<Media::PixelMap> pixelMap,
                                             Drawing::Rect& src, Drawing::Rect& dst, Drawing::Canvas& canvas)
{
    TransformData transformData;
    pixelMap->GetTransformData(transformData);
    Size realSize;
    pixelMap->GetAstcRealSize(realSize);
    dst.SetLeft(dst.GetLeft() - (realSize.width - src.GetRight()) / HALF_F);
    dst.SetTop(dst.GetTop() - (realSize.height - src.GetBottom()) / HALF_F);
    dst.SetRight(dst.GetRight() + (realSize.width - src.GetRight()) / HALF_F);
    dst.SetBottom(dst.GetBottom() + (realSize.height - src.GetBottom()) / HALF_F);
    if (transformData.scaleX != 0 && transformData.scaleY != 0) {
        src.SetRight(src.GetRight() / abs(transformData.scaleX));
        src.SetBottom(src.GetBottom() / abs(transformData.scaleY));
    }
    Drawing::Matrix matrix;
    matrix.PostScale(transformData.scaleX, transformData.scaleY, dst.GetLeft() / HALF_F + dst.GetRight() / HALF_F,
                     dst.GetTop() / HALF_F + dst.GetBottom() / HALF_F);
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
        float rightMinus = src.GetRight() - transformData.cropLeft - transformData.cropWidth;
        float bottomMinus = src.GetBottom() - transformData.cropTop - transformData.cropHeight;
        src.SetLeft(src.GetLeft() + transformData.cropLeft);
        src.SetTop(src.GetTop() + transformData.cropTop);
        src.SetRight(src.GetRight() - rightMinus);
        src.SetBottom(src.GetBottom() - bottomMinus);
        dst.SetLeft(dst.GetLeft() + (realSize.width - transformData.cropWidth) / HALF_F);
        dst.SetTop(dst.GetTop() + (realSize.height - transformData.cropHeight) / HALF_F);
        dst.SetRight(dst.GetRight() - (realSize.width - transformData.cropWidth) / HALF_F);
        dst.SetBottom(dst.GetBottom() - (realSize.height - transformData.cropHeight) / HALF_F);
    }
    if (transformData.scaleX != 0 && transformData.scaleY != 0) {
        dst.SetLeft(dst.GetLeft() + transformData.translateX / HALF_F / abs(transformData.scaleX));
        dst.SetTop(dst.GetTop() + transformData.translateY / HALF_F / abs(transformData.scaleY));
        dst.SetRight(dst.GetRight() + transformData.translateX / HALF_F / abs(transformData.scaleX));
        dst.SetBottom(dst.GetBottom() + transformData.translateY / HALF_F / abs(transformData.scaleY));
    }
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
        drawingImageInfo, (void*)pixelMap.GetWritablePixels(), static_cast<uint32_t>(pixelMap.GetRowBytes()));
    canvas.DrawBitmap(pixelBitmap, px, py);
}
} // namespace Rosen
} // namespace OHOS
