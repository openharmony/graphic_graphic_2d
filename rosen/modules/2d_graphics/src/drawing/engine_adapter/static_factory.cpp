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

#include "static_factory.h"

#include "skia_adapter/skia_static_factory.h"
#include "utils/system_properties.h"
#ifdef ENABLE_DDGR_OPTIMIZE
#include "ddgr_static_factory.h"
#endif
namespace OHOS {
namespace Rosen {
namespace Drawing {
using EngineStaticFactory = SkiaStaticFactory;

std::shared_ptr<TextBlob> StaticFactory::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromText(text, byteLength, font, encoding);
    }
#endif
    return EngineStaticFactory::MakeFromText(text, byteLength, font, encoding);
}

std::shared_ptr<TextBlob> StaticFactory::MakeFromPosText(const void* text, size_t byteLength,
    const Point pos[], const Font& font, TextEncoding encoding)
{
    return EngineStaticFactory::MakeFromPosText(text, byteLength, pos, font, encoding);
}

std::shared_ptr<TextBlob> StaticFactory::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromRSXform(text, byteLength, xform, font, encoding);
    }
#endif
    return EngineStaticFactory::MakeFromRSXform(text, byteLength, xform, font, encoding);
}

std::shared_ptr<Typeface> StaticFactory::MakeDefault()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeDefault();
    }
#endif
    return EngineStaticFactory::MakeDefault();
}

std::shared_ptr<Typeface> StaticFactory::MakeFromFile(const char path[], int index)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromFile(path, index);
    }
#endif
    return EngineStaticFactory::MakeFromFile(path, index);
}

std::shared_ptr<Typeface> StaticFactory::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromStream(std::move(memoryStream), index);
    }
#endif
    return EngineStaticFactory::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> StaticFactory::MakeFromName(const char familyName[], FontStyle fontStyle)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromName(familyName, fontStyle);
    }
#endif
    return EngineStaticFactory::MakeFromName(familyName, fontStyle);
}

#ifdef ACE_ENABLE_GPU
#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> StaticFactory::MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
    void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (!SystemProperties::IsUseVulkan()) {
        return nullptr;
    }
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromBackendRenderTarget(gpuContext, info, origin,
            colorType, colorSpace, deleteVkImage, cleanHelper);
    }
#endif
    return EngineStaticFactory::MakeFromBackendRenderTarget(gpuContext, info, origin,
        colorType, colorSpace, deleteVkImage, cleanHelper);
}
std::shared_ptr<Surface> StaticFactory::MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, int sampleCnt, ColorType colorType,
    std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (!SystemProperties::IsUseVulkan()) {
        return nullptr;
    }
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromBackendTexture(gpuContext, info, origin, sampleCnt, colorType,
            colorSpace, deleteVkImage, cleanHelper);
    }
#endif
    return EngineStaticFactory::MakeFromBackendTexture(gpuContext, info, origin, sampleCnt, colorType,
        colorSpace, deleteVkImage, cleanHelper);
}
#endif
std::shared_ptr<Surface> StaticFactory::MakeRenderTarget(GPUContext* gpuContext,
    bool budgeted, const ImageInfo& imageInfo)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeRenderTarget(gpuContext, imageInfo, budgeted);
    }
#endif
    return EngineStaticFactory::MakeRenderTarget(gpuContext, budgeted, imageInfo);
}
#endif

std::shared_ptr<Surface> StaticFactory::MakeRaster(const ImageInfo& imageInfo)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeRaster(imageInfo);
    }
#endif
    return EngineStaticFactory::MakeRaster(imageInfo);
}

std::shared_ptr<Surface> StaticFactory::MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeRasterDirect(imageInfo, pixels, rowBytes);
    }
#endif
    return EngineStaticFactory::MakeRasterDirect(imageInfo, pixels, rowBytes);
}

std::shared_ptr<Surface> StaticFactory::MakeRasterN32Premul(int32_t width, int32_t height)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeRasterN32Premul(width, height);
    }
#endif
    return EngineStaticFactory::MakeRasterN32Premul(width, height);
}

std::shared_ptr<Image> StaticFactory::MakeFromRaster(const Pixmap& pixmap,
    RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeFromRaster(pixmap, rasterReleaseProc, releaseContext);
    }
#endif
    return EngineStaticFactory::MakeFromRaster(pixmap, rasterReleaseProc, releaseContext);
}

std::shared_ptr<Image> StaticFactory::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeRasterData(info, pixels, rowBytes);
    }
#endif
    return EngineStaticFactory::MakeRasterData(info, pixels, rowBytes);
}

std::shared_ptr<TextBlob> StaticFactory::DeserializeTextBlob(const void* data, size_t size, void* ctx)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::DeserializeTextBlob(data, size);
    }
#endif
    return EngineStaticFactory::DeserializeTextBlob(data, size, ctx);
}

std::shared_ptr<Typeface> StaticFactory::DeserializeTypeface(const void* data, size_t size)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        // DDGR need to be adapted
        return nullptr;
    }
#endif
    return EngineStaticFactory::DeserializeTypeface(data, size);
}

bool StaticFactory::CanComputeFastBounds(const Brush& brush)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::CanComputeFastBounds(brush);
    }
#endif
    return EngineStaticFactory::CanComputeFastBounds(brush);
}

const Rect& StaticFactory::ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::ComputeFastBounds(brush, orig, storage);
    }
#endif
    return EngineStaticFactory::ComputeFastBounds(brush, orig, storage);
}

bool StaticFactory::AsBlendMode(const Brush& brush)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::AsBlendMode(brush);
    }
#endif
    return EngineStaticFactory::AsBlendMode(brush);
}

std::shared_ptr<Data> StaticFactory::MakeDataFromFileName(const char path[])
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MakeDataFromFileName(path);
    }
#endif
    return EngineStaticFactory::MakeDataFromFileName(path);
}

void StaticFactory::PathOutlineDecompose(const Path& path, std::vector<Path>& paths)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::PathOutlineDecompose(path, paths);
    }
#endif
    EngineStaticFactory::PathOutlineDecompose(path, paths);
}

void StaticFactory::MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
    const std::vector<Path>& paths, std::vector<Path>& multPaths)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::MultilayerPath(multMap, paths, multPaths);
    }
#endif
    EngineStaticFactory::MultilayerPath(multMap, paths, multPaths);
}

void StaticFactory::GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
    }
#endif
    EngineStaticFactory::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
}

Path StaticFactory::GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::GetDrawingPathforTextBlob(glyphId, blob);
    }
#endif
    return EngineStaticFactory::GetDrawingPathforTextBlob(glyphId, blob);
}

void StaticFactory::GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points)
{
    return EngineStaticFactory::GetDrawingPointsForTextBlob(blob, points);
}

std::shared_ptr<DrawingSymbolLayersGroups> StaticFactory::GetSymbolLayersGroups(uint32_t glyphId)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::GetSymbolLayersGroups(glyphId);
    }
#endif
    return EngineStaticFactory::GetSymbolLayersGroups(glyphId);
}

std::shared_ptr<std::vector<std::vector<DrawingPiecewiseParameter>>> StaticFactory::GetGroupParameters(
    DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode)
{
    return EngineStaticFactory::GetGroupParameters(type, groupSum, animationMode);
}

FontStyleSet* StaticFactory::CreateEmpty()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRStaticFactory::CreateEmpty();
    }
#endif
    return EngineStaticFactory::CreateEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS