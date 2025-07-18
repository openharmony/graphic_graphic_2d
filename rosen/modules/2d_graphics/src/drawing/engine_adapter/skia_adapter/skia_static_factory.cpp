/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "skia_static_factory.h"

#include "include/gpu/vk/GrVulkanTrackerInterface.h"

#include "skia_adapter/skia_blender.h"
#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_data.h"
#include "skia_document.h"
#include "skia_adapter/skia_font_style_set.h"
#include "skia_adapter/skia_hm_symbol.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_text_blob.h"
#include "skia_adapter/skia_typeface.h"
#include "skia_adapter/skia_utils.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<TextBlob> SkiaStaticFactory::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    return SkiaTextBlob::MakeFromText(text, byteLength, font, encoding);
}

std::shared_ptr<TextBlob> SkiaStaticFactory::MakeFromPosText(const void* text, size_t byteLength,
    const Point pos[], const Font& font, TextEncoding encoding)
{
    return SkiaTextBlob::MakeFromPosText(text, byteLength, pos, font, encoding);
}

std::shared_ptr<TextBlob> SkiaStaticFactory::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    return SkiaTextBlob::MakeFromRSXform(text, byteLength, xform, font, encoding);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeDefault()
{
    return SkiaTypeface::MakeDefault();
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromFile(const char path[], int index)
{
    return SkiaTypeface::MakeFromFile(path, index);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromFile(const char path[], const FontArguments& fontArguments)
{
    return SkiaTypeface::MakeFromFile(path, fontArguments);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
    return SkiaTypeface::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream,
    const FontArguments& fontArguments)
{
    return SkiaTypeface::MakeFromStream(std::move(memoryStream), fontArguments);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromName(const char familyName[], FontStyle fontStyle)
{
    return SkiaTypeface::MakeFromName(familyName, fontStyle);
}

std::vector<std::shared_ptr<Typeface>> SkiaStaticFactory::GetSystemFonts()
{
    return SkiaTypeface::GetSystemFonts();
}

#ifdef RS_ENABLE_GPU
#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> SkiaStaticFactory::MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
    void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (!SystemProperties::IsUseVulkan()) {
        return nullptr;
    }
    return SkiaSurface::MakeFromBackendRenderTarget(gpuContext, info, origin,
        colorType, colorSpace, deleteVkImage, cleanHelper);
}
#endif
std::shared_ptr<Surface> SkiaStaticFactory::MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, int sampleCnt, ColorType colorType,
    std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper)
{
    return SkiaSurface::MakeFromBackendTexture(gpuContext, info, origin, sampleCnt, colorType,
        colorSpace, deleteVkImage, cleanHelper);
}

std::shared_ptr<Surface> SkiaStaticFactory::MakeRenderTarget(GPUContext* gpuContext,
    bool budgeted, const ImageInfo& imageInfo)
{
    return SkiaSurface::MakeRenderTarget(gpuContext, budgeted, imageInfo);
}

std::shared_ptr<Image> SkiaStaticFactory::MakeFromYUVAPixmaps(GPUContext& gpuContext, const YUVInfo& info, void* memory)
{
    return SkiaImage::MakeFromYUVAPixmaps(gpuContext, info, memory);
}
#endif

std::shared_ptr<Surface> SkiaStaticFactory::MakeRaster(const ImageInfo& imageInfo)
{
    return SkiaSurface::MakeRaster(imageInfo);
}

std::shared_ptr<Surface> SkiaStaticFactory::MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes)
{
    return SkiaSurface::MakeRasterDirect(imageInfo, pixels, rowBytes);
}

std::shared_ptr<Surface> SkiaStaticFactory::MakeRasterN32Premul(int32_t width, int32_t height)
{
    return SkiaSurface::MakeRasterN32Premul(width, height);
}

std::shared_ptr<Image> SkiaStaticFactory::MakeFromRaster(const Pixmap& pixmap,
    RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext)
{
    return SkiaImage::MakeFromRaster(pixmap, rasterReleaseProc, releaseContext);
}

std::shared_ptr<Document> SkiaStaticFactory::MakeMultiPictureDocument(FileWStream* fileStream,
    SerialProcs* procs, std::unique_ptr<SharingSerialContext>& serialContext)
{
    return SkiaDocument::MakeMultiPictureDocument(fileStream, procs, serialContext);
}

std::shared_ptr<Image> SkiaStaticFactory::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
    return SkiaImage::MakeRasterData(info, pixels, rowBytes);
}

std::shared_ptr<TextBlob> SkiaStaticFactory::DeserializeTextBlob(const void* data, size_t size, void* ctx)
{
    return SkiaTextBlob::Deserialize(data, size, ctx);
}

std::shared_ptr<Typeface> SkiaStaticFactory::DeserializeTypeface(const void* data, size_t size)
{
    return SkiaTypeface::Deserialize(data, size);
}

bool SkiaStaticFactory::GetFillPath(const Pen& pen, const Path& src, Path& dst, const Rect* rect, const Matrix& matrix)
{
    return SkiaPaint::GetFillPath(pen, src, dst, rect, matrix);
}

bool SkiaStaticFactory::CanComputeFastBounds(const Brush& brush)
{
    return SkiaPaint::CanComputeFastBounds(brush);
}

const Rect& SkiaStaticFactory::ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage)
{
    return SkiaPaint::ComputeFastBounds(brush, orig, storage);
}

std::shared_ptr<Data> SkiaStaticFactory::MakeDataFromFileName(const char path[])
{
    return SkiaData::MakeFromFileName(path);
}

bool SkiaStaticFactory::AsBlendMode(const Brush& brush)
{
    return SkiaPaint::AsBlendMode(brush);
}

void SkiaStaticFactory::PathOutlineDecompose(const Path& path, std::vector<Path>& paths)
{
    SkiaHMSymbol::PathOutlineDecompose(path, paths);
}

void SkiaStaticFactory::MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
    const std::vector<Path>& paths, std::vector<Path>& multPaths)
{
    SkiaHMSymbol::MultilayerPath(multMap, paths, multPaths);
}

void SkiaStaticFactory::GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
{
    SkiaTextBlob::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
}

Path SkiaStaticFactory::GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob)
{
    return SkiaTextBlob::GetDrawingPathforTextBlob(glyphId, blob);
}

void SkiaStaticFactory::GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points)
{
    return SkiaTextBlob::GetDrawingPointsForTextBlob(blob, points);
}

std::shared_ptr<Blender> SkiaStaticFactory::CreateWithBlendMode(BlendMode mode)
{
    return SkiaBlender::CreateWithBlendMode(mode);
}

void SkiaStaticFactory::SetVmaCacheStatus(bool flag)
{
    SkiaUtils::SetVmaCacheStatus(flag);
}

void SkiaStaticFactory::ResetStatsData()
{
#ifndef TODO_M133_SKIA
    GrPerfMonitorReporter::GetInstance().resetStatsData();
#endif
}

void SkiaStaticFactory::ResetPerfEventData()
{
#ifndef TODO_M133_SKIA
    GrPerfMonitorReporter::GetInstance().resetPerfEventData();
#endif
}

std::map<std::string, std::vector<uint16_t>> SkiaStaticFactory::GetBlurStatsData()
{
#ifdef TODO_M133_SKIA
    return std::map<std::string, std::vector<uint16_t>> {};
#else
    return GrPerfMonitorReporter::GetInstance().getBlurStatsData();
#endif
}

std::map<std::string, RsBlurEvent> SkiaStaticFactory::GetBlurPerfEventData()
{
    std::map<std::string, RsBlurEvent> rsBlurEvent;
#ifndef TODO_M133_SKIA
    std::map<std::string, BlurEvent> grBlurEvent =
        GrPerfMonitorReporter::GetInstance().getBlurPerfEventData();
    GrBlurEventConvert2Rs(rsBlurEvent, grBlurEvent);
#endif
    return rsBlurEvent;
}

std::map<std::string, std::vector<uint16_t>> SkiaStaticFactory::GetTextureStatsData()
{
#ifdef TODO_M133_SKIA
    return std::map<std::string, std::vector<uint16_t>> {};
#else
    return GrPerfMonitorReporter::GetInstance().getTextureStatsData();
#endif
}

std::map<std::string, RsTextureEvent> SkiaStaticFactory::GetTexturePerfEventData()
{
    std::map<std::string, RsTextureEvent> rsTextureEvent;
#ifndef TODO_M133_SKIA
    std::map<std::string, TextureEvent> grTextureEvent =
        GrPerfMonitorReporter::GetInstance().getTexturePerfEventData();
    GrTextureEventConvert2Rs(rsTextureEvent, grTextureEvent);
#endif
    return rsTextureEvent;
}

int16_t SkiaStaticFactory::GetSplitRange(int64_t duration)
{
#ifdef TODO_M133_SKIA
    return 0;
#else
    return GrPerfMonitorReporter::getSplitRange(duration);
#endif
}

bool SkiaStaticFactory::IsOpenPerf()
{
#ifdef TODO_M133_SKIA
    return false;
#else
    return GrPerfMonitorReporter::GetInstance().isOpenPerf();
#endif
}

int64_t SkiaStaticFactory::GetCurrentTime()
{
#ifdef TODO_M133_SKIA
    return 0;
#else
    return GrPerfMonitorReporter::getCurrentTime();
#endif
}

void SkiaStaticFactory::SetCurrentNodeId(uint64_t nodeId)
{
    RECORD_GPU_RESOURCE_DRAWABLE_CALLER(nodeId);
}

#ifndef TODO_M133_SKIA
void SkiaStaticFactory::GrTextureEventConvert2Rs(std::map<std::string, RsTextureEvent>& rsTextureEvent,
    const std::map<std::string, TextureEvent>& grTextureEvent)
{
    for (const auto& [nodeName, node] : grTextureEvent) {
        RsTextureEvent event;
        SkiaConvertUtils::DrawingTextureEventToRsTextureEvent(node, event);
        rsTextureEvent[nodeName] = event;
    }
}

void SkiaStaticFactory::GrBlurEventConvert2Rs(std::map<std::string, RsBlurEvent>& rsBlurEvent,
    const std::map<std::string, BlurEvent>& grBlurEvent)
{
    for (const auto& [nodeName, node] : grBlurEvent) {
        RsBlurEvent event;
        SkiaConvertUtils::DrawingBlurEventToRsBlurEvent(node, event);
        rsBlurEvent[nodeName] = event;
    }
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
