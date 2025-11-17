/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "drawing_canvas_utils.h"

#include <memory>

#include "pipeline/rs_recording_canvas.h"
#include "pixel_map.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_pixel_map_shader.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;

OH_Drawing_ErrorCode DrawingCanvasUtils::DrawPixelMapNine(Drawing::Canvas* canvas,
    std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* center, const Drawing::Rect* dst,
    const Drawing::FilterMode mode)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !dst) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Drawing::RectI centerRectI;
    if (center) {
        centerRectI = Drawing::RectI(Drawing::DrawingFloatSaturate2Int(center->GetLeft()),
            Drawing::DrawingFloatSaturate2Int(center->GetTop()),
            Drawing::DrawingFloatSaturate2Int(center->GetRight()),
            Drawing::DrawingFloatSaturate2Int(center->GetBottom()));
    } else {
        centerRectI = Drawing::RectI(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
    }
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapNine(pixelMap, centerRectI, *dst, mode);
        return OH_DRAWING_SUCCESS;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (!image) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    canvas->DrawImageNine(image.get(), centerRectI, *dst, mode);
#endif
    return OH_DRAWING_SUCCESS;
}

namespace {
void DrawPixelMapMeshBuilderProcess(std::shared_ptr<Media::PixelMap> pixelMap,
    uint32_t meshWidth, uint32_t meshHeight, Drawing::Point* texsPoint, uint16_t* indices)
{
    const float height = static_cast<float>(pixelMap->GetHeight());
    const float width = static_cast<float>(pixelMap->GetWidth());
    const float dy = height / meshHeight;
    const float dx = width / meshWidth;
    Drawing::Point* texsPit = texsPoint;
    float y = 0;
    for (uint32_t i = 0; i <= meshWidth; i++) {
        if (i == meshWidth) {
            y = height;
        }
        float x = 0;
        for (uint32_t j = 0; j < meshHeight; j++) {
            texsPit->Set(x, y);
            texsPit += 1;
            x += dx;
        }
        texsPit->Set(width, y);
        texsPit += 1;
        y += dy;
    }
    uint16_t* dexIndices = indices;
    int indexIndices = 0;
    int indexSteps = 2;
    for (uint32_t i = 0; i < meshWidth; i++) {
        for (uint32_t j = 0; j < meshHeight; j++) {
            *dexIndices++ = indexIndices;
            *dexIndices++ = indexIndices + meshHeight + 1;
            *dexIndices++ = indexIndices + meshHeight + indexSteps;
            *dexIndices++ = indexIndices;
            *dexIndices++ = indexIndices + meshHeight + indexSteps;
            *dexIndices++ = indexIndices + 1;
            indexIndices += 1;
        }
        indexIndices += 1;
    }
}
}

OH_Drawing_ErrorCode DrawingCanvasUtils::DrawPixelMapMesh(Drawing::Canvas* canvas,
    std::shared_ptr<Media::PixelMap> pixelMap, uint32_t meshWidth, uint32_t meshHeight, const float* vertices,
    uint32_t verticesSize, uint32_t vertOffset, const uint32_t* colors, uint32_t colorsSize, uint32_t colorOffset)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !vertices || meshWidth == 0 || meshHeight == 0) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    uint64_t tempVerticeSize =
        ((static_cast<uint64_t>(meshWidth) + 1) * (static_cast<uint64_t>(meshHeight) + 1) + vertOffset) * 2;
    if (tempVerticeSize != static_cast<uint64_t>(verticesSize)) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    const float* verticesMesh = verticesSize ? (vertices + vertOffset * 2) : nullptr;
    if (!colors) {
        return DrawPixelMapMeshInternal(canvas, pixelMap, meshWidth, meshHeight, verticesMesh, nullptr);
    }
    uint64_t tempColorsSize =
        (static_cast<uint64_t>(meshWidth) + 1) * (static_cast<uint64_t>(meshHeight) + 1) + colorOffset;
    if (tempColorsSize != static_cast<uint64_t>(colorsSize)) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    const uint32_t* colorsMesh = colors + colorOffset;
    return DrawPixelMapMeshInternal(canvas, pixelMap, meshWidth, meshHeight, verticesMesh, colorsMesh);
#endif
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode DrawingCanvasUtils::DrawPixelMapMeshInternal(Drawing::Canvas* canvas,
    std::shared_ptr<Media::PixelMap> pixelMap, uint32_t meshWidth, uint32_t meshHeight, const float* vertices,
    const uint32_t* colors)
{
#ifdef OHOS_PLATFORM
    if (!canvas->GetMutableBrush().IsValid()) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    const int vertCounts = (meshWidth + 1) * (meshHeight + 1);
    int32_t size = 6;
    const int indexCount = meshWidth * meshHeight * size;
    uint32_t flags = colors ?
        Drawing::BuilderFlags::HAS_TEXCOORDS_BUILDER_FLAG | Drawing::BuilderFlags::HAS_COLORS_BUILDER_FLAG :
        Drawing::BuilderFlags::HAS_TEXCOORDS_BUILDER_FLAG;
    Drawing::Vertices::Builder builder(Drawing::VertexMode::TRIANGLES_VERTEXMODE, vertCounts, indexCount, flags);
    if (memcpy_s(builder.Positions(), vertCounts * sizeof(Drawing::Point),
        vertices, vertCounts * sizeof(Drawing::Point)) != 0) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    int32_t colorSize = 4;
    if (colors != nullptr &&
       (memcpy_s(builder.Colors(), vertCounts * colorSize, colors, vertCounts * colorSize) != 0)) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    if (!builder.TexCoords() || !builder.Indices()) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    DrawPixelMapMeshBuilderProcess(pixelMap, meshWidth, meshHeight, builder.TexCoords(), builder.Indices());
    if (canvas->GetDrawingType() != Drawing::DrawingType::RECORDING) {
        std::shared_ptr<Drawing::Image> image = OHOS::Rosen::RSPixelMapUtil::ExtractDrawingImage(pixelMap);
        if (image == nullptr) {
            return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
        }
        auto shader = Drawing::ShaderEffect::CreateImageShader(*image,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
        canvas->GetMutableBrush().SetShaderEffect(shader);
    } else {
        auto shader = Drawing::ShaderEffect::CreateExtendShader(std::make_shared<RSPixelMapShader>(pixelMap,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix()));
        canvas->GetMutableBrush().SetShaderEffect(shader);
    }
    canvas->DrawVertices(*builder.Detach(), Drawing::BlendMode::MODULATE);
#endif
    return OH_DRAWING_SUCCESS;
}

void DrawingCanvasUtils::DrawPixelMapRect(Drawing::Canvas* canvas, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* src, const Drawing::Rect* dst, const Drawing::SamplingOptions* sampling)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !dst) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapRect(pixelMap,
            src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
            *dst, sampling ? *sampling : Drawing::SamplingOptions());
        return;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (!image) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawImageRect(*image, src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
        *dst, sampling ? *sampling : Drawing::SamplingOptions());
#endif
}

OH_Drawing_ErrorCode DrawingCanvasUtils::DrawPixelMapRectConstraint(Drawing::Canvas* canvas,
    std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* src, const Drawing::Rect* dst,
    const Drawing::SamplingOptions* sampling, const Drawing::SrcRectConstraint constraint)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !dst) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapRect(pixelMap,
            src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
            *dst, sampling ? *sampling : Drawing::SamplingOptions(), constraint);
        return OH_DRAWING_SUCCESS;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (!image) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    canvas->DrawImageRect(*image, src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
        *dst, sampling ? *sampling : Drawing::SamplingOptions(), constraint);
#endif
    return OH_DRAWING_SUCCESS;
}

void DrawingCanvasUtils::DrawRecordCmd(Drawing::Canvas* canvas,
    const std::shared_ptr<Drawing::RecordCmd> recordCmd, bool canNesting)
{
    if (!canNesting && canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        if (canvas_->IsRecordCmd()) {
            LOGE("RecordingCanvas::DrawRecordCmd, operation is unsupported!");
            return;
        }
    }
    canvas->DrawRecordCmd(recordCmd);
}

#ifdef OHOS_PLATFORM
static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(Media::ColorSpace colorSpace)
{
    switch (colorSpace) {
        case Media::ColorSpace::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case Media::ColorSpace::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case Media::ColorSpace::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

static Drawing::ColorType PixelFormatToDrawingColorType(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case Media::PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case Media::PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case Media::PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case Media::PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

static Drawing::AlphaType AlphaTypeToDrawingAlphaType(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

bool DrawingCanvasUtils::ExtractDrawingBitmap(std::shared_ptr<Media::PixelMap> pixelMap, Drawing::Bitmap& bitmap)
{
    if (!pixelMap) {
        return false;
    }
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    bitmap.Build(drawingImageInfo, pixelMap->GetRowStride());
    bitmap.SetPixels(const_cast<void*>(reinterpret_cast<const void*>(pixelMap->GetPixels())));
    return true;
}
#endif