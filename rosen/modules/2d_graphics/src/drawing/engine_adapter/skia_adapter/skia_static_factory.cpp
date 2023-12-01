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

#include "skia_adapter/skia_data.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_font_style_set.h"
#include "skia_adapter/skia_text_blob.h"
#include "skia_adapter/skia_typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<TextBlob> SkiaStaticFactory::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    return SkiaTextBlob::MakeFromText(text, byteLength, font, encoding);
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

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
    return SkiaTypeface::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> SkiaStaticFactory::MakeFromName(const char familyName[], FontStyle fontStyle)
{
    return SkiaTypeface::MakeFromName(familyName, fontStyle);
}

#ifdef ACE_ENABLE_GPU
#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> SkiaStaticFactory::MakeFromBackendRenderTarget(GPUContext* gpuuContext,
    const VKTextureInfo& info, TextureOrigin origin, void (*deleteVkImage)(void *), void* cleanHelper)
{
    return SkiaSurface::MakeFromBackendRenderTarget(gpuuContext, info, origin, deleteVkImage, cleanHelper);
}
#endif
std::shared_ptr<Surface> SkiaStaticFactory::MakeRenderTarget(GPUContext* gpuContext,
    bool budgeted, const ImageInfo& imageInfo)
{
    return SkiaSurface::MakeRenderTarget(gpuContext, budgeted, imageInfo);
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

std::shared_ptr<Image> SkiaStaticFactory::MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
    size_t rowBytes)
{
    return SkiaImage::MakeRasterData(info, pixels, rowBytes);
}

std::shared_ptr<TextBlob> SkiaStaticFactory::DeserializeTextBlob(const void* data, size_t size)
{
    return SkiaTextBlob::Deserialize(data, size);
}

bool SkiaStaticFactory::CanComputeFastBounds(const Brush& brush)
{
    return SkiaPaint::CanComputeFastBounds(brush);
}

const Rect& SkiaStaticFactory::ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage)
{
    return SkiaPaint::ComputeFastBounds(brush, orig, storage);
}

FontStyleSet* SkiaStaticFactory::CreateEmptyFontStyleSet()
{
    return SkiaFontStyleSet::CreateEmpty();
}

std::shared_ptr<Data> SkiaStaticFactory::MakeDataFromFileName(const char path[])
{
    return SkiaData::MakeFromFileName(path);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS