/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rspropertydrawableboundsgeometry_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "common/rs_color.h"
#include "draw/canvas.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_content.h"
#include "property/rs_properties.h"
#include "property/rs_property_drawable_bounds_geometry.h"
#include "skia_adapter/skia_surface.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoDraw(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    auto& properties = content.GetMutableRenderProperties();
    float value = GetData<float>();
    properties.SetBounds(value);
    auto rsBound = std::make_shared<RSBoundsGeometryDrawable>();
    rsBound->Draw(content, cacheCanvas);
    auto rsPoint = std::make_shared<RSPointLightDrawable>();
    rsPoint->Draw(content, cacheCanvas);
    auto mask = std::make_shared<RSMask>();
    auto rssvgDomMaskDrawable = std::make_shared<RSSvgDomMaskDrawable>(mask);
    rssvgDomMaskDrawable->Draw(content, cacheCanvas);
    auto rsGradientMaskDrawable = std::make_shared<RSGradientMaskDrawable>(mask);
    rsGradientMaskDrawable->Draw(content, cacheCanvas);
    auto rsPathMaskDrawable = std::make_shared<RSPathMaskDrawable>(mask);
    rsPathMaskDrawable->Draw(content, cacheCanvas);
    auto rsPixelMapMaskDrawable = std::make_shared<RSPixelMapMaskDrawable>(mask);
    rsPixelMapMaskDrawable->Draw(content, cacheCanvas);
    auto rsColorfulShadowDrawable = std::make_shared<RSColorfulShadowDrawable>(properties);
    rsColorfulShadowDrawable->Draw(content, cacheCanvas);
    auto rsHardwareAccelerationShadowDrawable = std::make_shared<RSHardwareAccelerationShadowDrawable>(properties);
    rsHardwareAccelerationShadowDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoGenerate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSRenderContent content;
    RSBoundsGeometryDrawable::Generate(content);
    RSPointLightDrawable::Generate(content);
    RSShadowBaseDrawable::Generate(content);
    Brush brush;
    Pen pen;
    RSBorderDrawable::Generate(content);
    RSOutlineDrawable::Generate(content);
    RSMaskDrawable::Generate(content);
    float value = GetData<float>();
    uint32_t color = GetData<uint32_t>();
    BorderStyle borderStyle = static_cast<BorderStyle>(GetData<uint32_t>() % 4);
    auto& properties = content.GetMutableRenderProperties();
    properties.SetBorderWidth(value);
    properties.SetBorderColor(RSColor::FromArgbInt(color));
    properties.SetBorderStyle(static_cast<uint32_t>(borderStyle));
    RSBorderDrawable::Generate(content);
    Vector4f radius = GetData<Vector4f>();
    properties.SetOutlineRadius(radius);
    properties.GetOutline()->SetWidth(value);
    properties.GetOutline()->SetColor(RSColor::FromArgbInt(color));
    properties.GetOutline()->SetStyle(borderStyle);
    RSOutlineDrawable::Generate(content);
    auto mask = std::make_shared<RSMask>();
    MaskType maskType = static_cast<MaskType>(GetData<uint32_t>() % 5);
    mask->SetMaskType(maskType);
    RSMaskDrawable::Generate(content);
    RSShadowBaseDrawable::Generate(content);
    return true;
}

bool DoRSClipBoundsDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    auto rsClipBoundsDrawable = std::make_shared<RSClipBoundsDrawable>();
    rsClipBoundsDrawable->Generate(content);
    rsClipBoundsDrawable->Draw(content, cacheCanvas);
    
    return true;
}

bool DoRSBorderDRRectDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    int rgba = GetData<int>();
    Pen pen(rgba);
    RSProperties properties;
    auto rsBorderDrrect = std::make_shared<RSBorderDRRectDrawable>(std::move(brush), std::move(pen), properties, true);
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBorderDrrect->Draw(content, cacheCanvas);
    rsBorderDrrect->OnBoundsChange(properties);
    return true;
}

bool DoRSBorderFourLineDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    auto rsBorderFourLineDrawable =
        std::make_shared<RSBorderFourLineDrawable>(std::move(brush), std::move(pen), properties, true);
    Vector4f borderWidth = GetData<Vector4f>();
    properties.SetBorderWidth(borderWidth);
    rsBorderFourLineDrawable->Draw(content, cacheCanvas);
    rsBorderFourLineDrawable->OnBoundsChange(properties);
    return true;
}

bool DoRSBorderPathDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    RSProperties properties;
    auto border = std::make_shared<RSBorder>();
    float value = GetData<float>();
    properties.SetBorderWidth(value);
    properties.SetOutlineWidth(value);
    auto rsBorderPath =
        std::make_shared<RSBorderPathDrawable>(std::move(brush), std::move(pen), properties, true);
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBorderPath->Draw(content, cacheCanvas);
    rsBorderPath->OnBoundsChange(properties);
    return true;
}

bool DoRSBorderFourLineRoundCornerDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    RSProperties& properties = content.GetMutableRenderProperties();
    auto rsBorderFourLineRoundCornerDrawable =
        std::make_shared<RSBorderFourLineRoundCornerDrawable>(std::move(brush), std::move(pen), properties, true);
    Vector4f borderWidth = GetData<Vector4f>();
    properties.SetBorderWidth(borderWidth);
    rsBorderFourLineRoundCornerDrawable->Draw(content, cacheCanvas);
    rsBorderFourLineRoundCornerDrawable->OnBoundsChange(properties);
    return true;
}

bool DoRSShadowDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSProperties properties;
    auto rsShadowDrawable = std::make_shared<RSShadowDrawable>(properties);
    RSRenderContent content;
    int l = GetData<int>();
    int t = GetData<int>();
    int r = GetData<int>();
    int b = GetData<int>();
    OHOS::Rosen::Drawing::RectI deviceClipBounds(l, t, r, b);
    Path skPath;
    Matrix matrix;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsShadowDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSDynamicLightUpDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);

    auto rsDynamicLightUpDrawable = std::make_shared<RSDynamicLightUpDrawable>();
    RSDynamicLightUpDrawable::Generate(content);
    rsDynamicLightUpDrawable->Draw(content, cacheCanvas);
    rsDynamicLightUpDrawable->Update(content);
    return true;
}

bool DoRSBinarizationDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBinarizationDrawable = std::make_shared<RSBinarizationDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBinarizationDrawable->Draw(content, cacheCanvas);
    RSBinarizationDrawable::Generate(content);
    return true;
}

bool DoRSLightUpEffectDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsLightUpEffectDrawable = std::make_shared<RSLightUpEffectDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsLightUpEffectDrawable->Draw(content, cacheCanvas);
    RSLightUpEffectDrawable::Generate(content);
    rsLightUpEffectDrawable->Update(content);
    return true;
}

bool DoRSDynamicDimDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsDynamicDimDrawable = std::make_shared<RSDynamicDimDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsDynamicDimDrawable->Draw(content, cacheCanvas);
    RSDynamicDimDrawable::Generate(content);
    rsDynamicDimDrawable->Update(content);
    return true;
}

bool DoRSBackgroundFilterDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBackgroundFilterDrawable = std::make_shared<RSBackgroundFilterDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBackgroundFilterDrawable->Draw(content, cacheCanvas);
    RSBackgroundFilterDrawable::Generate(content);
    rsBackgroundFilterDrawable->Update(content);
    return true;
}

bool DoRSCompositingFilterDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsCompositingFilterDrawable = std::make_shared<RSCompositingFilterDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsCompositingFilterDrawable->Draw(content, cacheCanvas);
    RSCompositingFilterDrawable::Generate(content);
    rsCompositingFilterDrawable->Update(content);
    return true;
}

bool DoRSForegroundFilterDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsForegroundFilterDrawable = std::make_shared<RSForegroundFilterDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    std::shared_ptr<Drawing::SkiaSurface> skiaSurface = std::make_shared<Drawing::SkiaSurface>();
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterN32Premul(1, 1);
    auto surface = std::make_shared<Drawing::Surface>();
    skiaSurface->skSurface_ = skSurface;
    surface->impl_ = skiaSurface;
    cacheCanvas.surface_ = surface.get();
    rsForegroundFilterDrawable->Draw(content, cacheCanvas);
    RSForegroundFilterDrawable::Generate(content);
    rsForegroundFilterDrawable->Update(content);
    return true;
}

bool DoRSForegroundFilterRestoreDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsForegroundFilterRestoreDrawable = std::make_shared<RSForegroundFilterRestoreDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsForegroundFilterRestoreDrawable->Draw(content, cacheCanvas);
    RSForegroundFilterRestoreDrawable::Generate(content);
    rsForegroundFilterRestoreDrawable->Update(content);
    return true;
}

bool DoRSEffectDataGenerateDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsEffectDataGenerateDrawable = std::make_shared<RSEffectDataGenerateDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsEffectDataGenerateDrawable->Draw(content, cacheCanvas);
    rsEffectDataGenerateDrawable->Update(content);
    return true;
}

bool DoRSEffectDataApplyDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsEffectDataApplyDrawable = std::make_shared<RSEffectDataApplyDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsEffectDataApplyDrawable->Draw(content, cacheCanvas);
    RSEffectDataApplyDrawable::Generate(content);
    rsEffectDataApplyDrawable->Update(content);
    return true;
}

bool DoRSForegroundColorDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    auto rsForegroundColorDrawable = std::make_shared<RSForegroundColorDrawable>(std::move(brush));
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsForegroundColorDrawable->Draw(content, cacheCanvas);
    RSForegroundColorDrawable::Generate(content);
    rsForegroundColorDrawable->Update(content);
    return true;
}

bool DoRSParticleDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsParticleDrawable = std::make_shared<RSParticleDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsParticleDrawable->Draw(content, cacheCanvas);
    RSParticleDrawable::Generate(content);
    rsParticleDrawable->Update(content);
    return true;
}

bool DoRSPixelStretchDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsPixelStretchDrawable = std::make_shared<RSPixelStretchDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsPixelStretchDrawable->Draw(content, cacheCanvas);
    RSPixelStretchDrawable::Generate(content);
    rsPixelStretchDrawable->Update(content);
    return true;
}

bool DoRSBackgroundDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBackgroundDrawable = std::make_shared<RSBackgroundDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBackgroundDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSBackgroundColorDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    auto rsBackgroundColorDrawable = std::make_shared<RSBackgroundColorDrawable>(c);
    RSRenderContent content;
    rsBackgroundColorDrawable->Update(content);
    RSBackgroundColorDrawable::Generate(content);
    return true;
}

bool DoRSBackgroundShaderDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto filter = std::make_shared<ShaderEffect>();
    auto rsBackgroundShaderDrawable = std::make_shared<RSBackgroundShaderDrawable>(filter);
    RSRenderContent content;
    rsBackgroundShaderDrawable->Update(content);
    RSBackgroundShaderDrawable::Generate(content);
    return true;
}

bool DoRSBackgroundImageDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto rsBackgroundImageDrawable = std::make_shared<RSBackgroundImageDrawable>();
    RSRenderContent content;
    RSBackgroundImageDrawable::Generate(content);
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBackgroundImageDrawable->Update(content);
    rsBackgroundImageDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSBlendSaveLayerDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBlendSaveLayerDrawable = std::make_shared<RSBlendSaveLayerDrawable>(2, 1);
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBlendSaveLayerDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSBlendFastDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBlendFastDrawable = std::make_shared<RSBlendFastDrawable>(2);
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBlendFastDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSBlendSaveLayerRestoreDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBlendSaveLayerRestoreDrawable = std::make_shared<RSBlendSaveLayerRestoreDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBlendSaveLayerRestoreDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSBlendFastRestoreDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsBlendFastRestoreDrawable = std::make_shared<RSBlendFastRestoreDrawable>();
    RSRenderContent content;
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    rsBlendFastRestoreDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoBlendSaveDrawableGenerate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSRenderContent content;
    auto result1 = BlendSaveDrawableGenerate(content);
    auto result2 = BlendRestoreDrawableGenerate(content);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoDraw(data, size);
    OHOS::Rosen::DoGenerate(data, size);
    OHOS::Rosen::DoRSBorderDRRectDrawable(data, size);
    OHOS::Rosen::DoRSBorderFourLineDrawable(data, size);
    OHOS::Rosen::DoRSShadowDrawable(data, size);
    OHOS::Rosen::DoRSDynamicLightUpDrawable(data, size);
    OHOS::Rosen::DoRSBinarizationDrawable(data, size);
    OHOS::Rosen::DoRSLightUpEffectDrawable(data, size);
    OHOS::Rosen::DoRSDynamicDimDrawable(data, size);
    OHOS::Rosen::DoRSBackgroundFilterDrawable(data, size);
    OHOS::Rosen::DoRSForegroundFilterDrawable(data, size);
    OHOS::Rosen::DoRSEffectDataGenerateDrawable(data, size);
    OHOS::Rosen::DoRSEffectDataApplyDrawable(data, size);
    OHOS::Rosen::DoRSForegroundColorDrawable(data, size);
    OHOS::Rosen::DoRSParticleDrawable(data, size);
    OHOS::Rosen::DoRSPixelStretchDrawable(data, size);
    OHOS::Rosen::DoRSBackgroundDrawable(data, size);
    OHOS::Rosen::DoRSBackgroundColorDrawable(data, size);
    OHOS::Rosen::DoRSBackgroundShaderDrawable(data, size);
    OHOS::Rosen::DoRSBackgroundImageDrawable(data, size);
    OHOS::Rosen::DoRSBlendSaveLayerDrawable(data, size);
    OHOS::Rosen::DoRSBlendFastDrawable(data, size);
    OHOS::Rosen::DoRSBlendSaveLayerRestoreDrawable(data, size);
    OHOS::Rosen::DoRSBlendFastRestoreDrawable(data, size);
    OHOS::Rosen::DoRSClipBoundsDrawable(data, size);
    OHOS::Rosen::DoRSBorderPathDrawable(data, size);
    OHOS::Rosen::DoRSBorderFourLineRoundCornerDrawable(data, size);
    OHOS::Rosen::DoRSCompositingFilterDrawable(data, size);
    OHOS::Rosen::DoRSForegroundFilterRestoreDrawable(data, size);
    OHOS::Rosen::DoBlendSaveDrawableGenerate(data, size);
    return 0;
}