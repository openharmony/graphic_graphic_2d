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

namespace OHOS {
namespace Rosen {
namespace Drawing {
using EngineStaticFactory = SkiaStaticFactory;

std::shared_ptr<TextBlob> StaticFactory::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    return EngineStaticFactory::MakeFromText(text, byteLength, font, encoding);
}

std::shared_ptr<TextBlob> StaticFactory::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    return EngineStaticFactory::MakeFromRSXform(text, byteLength, xform, font, encoding);
}

std::shared_ptr<Typeface> StaticFactory::MakeDefault()
{
    return EngineStaticFactory::MakeDefault();
}

std::shared_ptr<Typeface> StaticFactory::MakeFromFile(const char path[], int index)
{
    return EngineStaticFactory::MakeFromFile(path, index);
}

std::shared_ptr<Typeface> StaticFactory::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
    return EngineStaticFactory::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> StaticFactory::MakeFromName(const char familyName[], FontStyle fontStyle)
{
    return EngineStaticFactory::MakeFromName(familyName, fontStyle);
}

#ifdef ACE_ENABLE_GPU
#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> StaticFactory::MakeFromBackendRenderTarget(GPUContext* gpuContext, TextureInfo& info,
    TextureOrigin origin, void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    return EngineStaticFactory::MakeFromBackendRenderTarget(gpuContext, info, origin, deleteVkImage, cleanHelper);
}
#endif
std::shared_ptr<Surface> StaticFactory::MakeRenderTarget(GPUContext* gpuContext,
    bool budgeted, const ImageInfo& imageInfo)
{
    return EngineStaticFactory::MakeRenderTarget(gpuContext, budgeted, imageInfo);
}
#endif

std::shared_ptr<Surface> StaticFactory::MakeRaster(const ImageInfo& imageInfo)
{
    return EngineStaticFactory::MakeRaster(imageInfo);
}

std::shared_ptr<Surface> StaticFactory::MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes)
{
    return EngineStaticFactory::MakeRasterDirect(imageInfo, pixels, rowBytes);
}

std::shared_ptr<Surface> StaticFactory::MakeRasterN32Premul(int32_t width, int32_t height)
{
    return EngineStaticFactory::MakeRasterN32Premul(width, height);
}

std::shared_ptr<Image> StaticFactory::MakeFromRaster(const Pixmap& pixmap,
    RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext)
{
    return EngineStaticFactory::MakeFromRaster(pixmap, rasterReleaseProc, releaseContext);
}

std::shared_ptr<Image> StaticFactory::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
    return EngineStaticFactory::MakeRasterData(info, pixels, rowBytes);
}

std::shared_ptr<TextBlob> StaticFactory::DeserializeTextBlob(const void* data, size_t size)
{
    return EngineStaticFactory::DeserializeTextBlob(data, size);
}

bool StaticFactory::CanComputeFastBounds(const Brush& brush)
{
    return EngineStaticFactory::CanComputeFastBounds(brush);
}

const Rect& StaticFactory::ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage)
{
    return EngineStaticFactory::ComputeFastBounds(brush, orig, storage);
}

bool StaticFactory::AsBlendMode(const Brush& brush)
{
    return EngineStaticFactory::AsBlendMode(brush);
}
std::shared_ptr<Data> StaticFactory::MakeDataFromFileName(const char path[])
{
    return EngineStaticFactory::MakeDataFromFileName(path);
}

void StaticFactory::PathOutlineDecompose(const Path& path, std::vector<Path>& paths)
{
    EngineStaticFactory::PathOutlineDecompose(path, paths);
}

void StaticFactory::MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
    const std::vector<Path>& paths, std::vector<Path>& multPaths)
{
    EngineStaticFactory::MultilayerPath(multMap, paths, multPaths);
}

void StaticFactory::GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
{
    EngineStaticFactory::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
}

Path StaticFactory::GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob)
{
    return EngineStaticFactory::GetDrawingPathforTextBlob(glyphId, blob);
}

DrawingSymbolLayersGroups* StaticFactory::GetSymbolLayersGroups(uint32_t glyphId)
{
    return EngineStaticFactory::GetSymbolLayersGroups(glyphId);
}

FontStyleSet* StaticFactory::CreateEmpty()
{
    return EngineStaticFactory::CreateEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS