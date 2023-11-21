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

std::shared_ptr<Typeface> StaticFactory::MakeFromFile(const char path[])
{
    return EngineStaticFactory::MakeFromFile(path);
}

#ifdef ACE_ENABLE_GPU
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS