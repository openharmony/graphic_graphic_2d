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

#ifndef SKIA_STATIC_FACTORY_H
#define SKIA_STATIC_FACTORY_H

#include <cstdint>
#include <vector>

#include "draw/brush.h"
#include "draw/path.h"
#include "draw/surface.h"
#include "image/pixmap.h"
#include "text/font_style_set.h"
#include "text/text_blob.h"
#include "text/typeface.h"
#include "utils/data.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaStaticFactory {
public:
    static std::shared_ptr<TextBlob> MakeFromText(const void* text, size_t byteLength,
        const Font& font, TextEncoding encoding);
    static std::shared_ptr<TextBlob> MakeFromPosText(const void* text, size_t byteLength,
        const Point pos[], const Font& font, TextEncoding encoding);
    static std::shared_ptr<TextBlob> MakeFromRSXform(const void* text, size_t byteLength,
        const RSXform xform[], const Font& font, TextEncoding encoding);
    static std::shared_ptr<Typeface> MakeDefault();
    static std::shared_ptr<Typeface> MakeFromFile(const char path[], int index);
    static std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index);
    static std::shared_ptr<Typeface> MakeFromName(const char familyName[], FontStyle fontStyle);
#ifdef ACE_ENABLE_GPU
#ifdef RS_ENABLE_VK
    static std::shared_ptr<Surface> MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
        TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
        void (*deleteVkImage)(void *), void* cleanHelper);
        static std::shared_ptr<Surface> MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
            TextureOrigin origin, int sampleCnt, ColorType colorType,
            std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper);
#endif
    static std::shared_ptr<Surface> MakeRenderTarget(GPUContext* gpuContext, bool budgeted, const ImageInfo& imageInfo);
#endif
    static std::shared_ptr<Surface> MakeRaster(const ImageInfo& imageInfo);
    static std::shared_ptr<Surface> MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes);
    static std::shared_ptr<Surface> MakeRasterN32Premul(int32_t width, int32_t height);
    static std::shared_ptr<Image> MakeFromRaster(const Pixmap& pixmap,
        RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext);
    static std::shared_ptr<Image> MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
        size_t rowBytes);
    static std::shared_ptr<TextBlob> DeserializeTextBlob(const void* data, size_t size);
    static bool CanComputeFastBounds(const Brush& brush);
    static const Rect& ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage);
    static bool AsBlendMode(const Brush& brush);
    static std::shared_ptr<Data> MakeDataFromFileName(const char path[]);
    static void PathOutlineDecompose(const Path& path, std::vector<Path>& paths);
    static void MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
        const std::vector<Path>& paths, std::vector<Path>& multPaths);
    static void GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds);
    static Path GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob);
    static void GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points);
    static std::shared_ptr<DrawingSymbolLayersGroups> GetSymbolLayersGroups(uint32_t glyphId);
    static std::shared_ptr<std::vector<std::vector<DrawingPiecewiseParameter>>> GetGroupParameters(
        DrawingAnimationType type, DrawingAnimationSubType subType, int animationMode);
    static FontStyleSet* CreateEmpty();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_STATIC_FACTORY_H