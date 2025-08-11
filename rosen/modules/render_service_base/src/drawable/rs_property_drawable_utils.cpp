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

#include "drawable/rs_property_drawable_utils.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_color_picker.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_effect_luminance_manager.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_magnifier_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int TRACE_LEVEL_TWO = 2;
constexpr int PARAM_DOUBLE = 2;
}

std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::binarizationShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::dynamicDimShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::dynamicBrightnessBlenderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::dynamicBrightnessLinearBlenderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::shadowBlenderEffect_ = nullptr;

Drawing::RoundRect RSPropertyDrawableUtils::RRect2DrawingRRect(const RRect& rr)
{
    Drawing::Rect rect = Drawing::Rect(
        rr.rect_.left_, rr.rect_.top_, rr.rect_.left_ + rr.rect_.width_, rr.rect_.top_ + rr.rect_.height_);

    // set radius for all 4 corner of RRect
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
    std::vector<Drawing::Point> radii(NUM_OF_CORNERS_IN_RECT);
    for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
        radii.at(i).SetX(rr.radius_[i].x_);
        radii.at(i).SetY(rr.radius_[i].y_);
    }
    return { rect, radii };
}

Drawing::Rect RSPropertyDrawableUtils::Rect2DrawingRect(const RectF& r)
{
    return { r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_ };
}

RRect RSPropertyDrawableUtils::GetRRectForDrawingBorder(
    const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return {};
    }

    return isOutline ? RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour())
                     : properties.GetRRect();
}

RRect RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(
    const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return {};
    }
    return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
}

Color RSPropertyDrawableUtils::GetColorForShadowSyn(Drawing::Canvas* canvas, Drawing::Path& path, const Color& color,
    const int& colorStrategy)
{
    RSColor colorPicked;
    auto shadowAlpha = color.GetAlpha();
    auto matrix = canvas->GetTotalMatrix();
    if (PickColorSyn(canvas, path, matrix, colorPicked, colorStrategy)) {
        GetDarkColor(colorPicked);
    } else {
        shadowAlpha = 0;
    }
    return Color(colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue(), shadowAlpha);
}

std::shared_ptr<Drawing::Image> RSPropertyDrawableUtils::GetShadowRegionImage(Drawing::Canvas* canvas,
    Drawing::Path& drPath, Drawing::Matrix& matrix)
{
    Drawing::Rect clipBounds = drPath.GetBounds();
    Drawing::RectI clipIBounds = { static_cast<int>(clipBounds.GetLeft()), static_cast<int>(clipBounds.GetTop()),
        static_cast<int>(clipBounds.GetRight()), static_cast<int>(clipBounds.GetBottom()) };
    Drawing::Surface* drSurface = canvas->GetSurface();
    if (drSurface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::GetShadowRegionImage drSurface is null");
        return nullptr;
    }
    const int deviceWidth = drSurface->Width();
    const int deviceHeight = drSurface->Height();
    Drawing::Rect regionRect = {0, 0, clipIBounds.GetWidth(), clipIBounds.GetHeight()};
    Drawing::Rect regionRectDev;
    matrix.MapRect(regionRectDev, regionRect);
    int32_t fLeft = std::clamp(int(regionRectDev.GetLeft()), 0, deviceWidth - 1);
    int32_t fTop = std::clamp(int(regionRectDev.GetTop()), 0, deviceHeight - 1);
    int32_t fRight = std::clamp(int(regionRectDev.GetRight()), 0, deviceWidth - 1);
    int32_t fBottom = std::clamp(int(regionRectDev.GetBottom()), 0, deviceHeight - 1);
    if (fLeft == fRight || fTop == fBottom) {
        ROSEN_LOGE("RSPropertyDrawableUtils::GetShadowRegionImage Shadow Rect Invalid");
        return nullptr;
    }
    Drawing::RectI regionBounds = { fLeft, fTop, fRight, fBottom };
    std::shared_ptr<Drawing::Image> shadowRegionImage = drSurface->GetImageSnapshot(regionBounds);
    return shadowRegionImage;
}

bool RSPropertyDrawableUtils::PickColorSyn(Drawing::Canvas* canvas, Drawing::Path& drPath, Drawing::Matrix& matrix,
    RSColor& colorPicked, const int& colorStrategy)
{
    std::shared_ptr<Drawing::Image> shadowRegionImage = GetShadowRegionImage(canvas, drPath, matrix);
    if (shadowRegionImage == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::PickColorSyn GetImageSnapshot Failed");
        return false;
    }
    auto scaledImage = GpuScaleImage(canvas, shadowRegionImage);
    if (scaledImage == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::PickColorSyn GpuScaleImageids null");
        return false;
    }
    std::shared_ptr<Drawing::Pixmap> dst;
    const int buffLen = scaledImage->GetWidth() * scaledImage->GetHeight();
    auto pixelPtr = std::make_unique<uint32_t[]>(buffLen);
    auto info = scaledImage->GetImageInfo();
    dst = std::make_shared<Drawing::Pixmap>(info, pixelPtr.get(), info.GetWidth() * info.GetBytesPerPixel());
    bool flag = scaledImage->ReadPixels(*dst, 0, 0);
    if (!flag) {
        ROSEN_LOGE("RSPropertyDrawableUtils::PickColorSyn ReadPixel Failed");
        return false;
    }
    uint32_t errorCode = 0;
    std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
    if (colorPicker == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::PickColorSyn colorPicker is null");
        return false;
    }
    if (errorCode != 0) {
        return false;
    }
    Drawing::ColorQuad color;
    if (colorStrategy == SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN) {
        colorPicker->GetLargestProportionColor(color);
    } else {
        colorPicker->GetAverageColor(color);
    }
    colorPicked = RSColor(Drawing::Color::ColorQuadGetR(color), Drawing::Color::ColorQuadGetG(color),
        Drawing::Color::ColorQuadGetB(color), Drawing::Color::ColorQuadGetA(color));
    return true;
}

std::shared_ptr<Drawing::Image> RSPropertyDrawableUtils::GpuScaleImage(Drawing::Canvas* canvas,
    const std::shared_ptr<Drawing::Image> image)
{
    std::string shaderString(R"(
        uniform shader imageInput;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb, 1.0);
        }
    )");

    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(shaderString);
    if (!effect) {
        ROSEN_LOGE("RSPropertyDrawableUtils::GpuScaleImage effect is null");
        return nullptr;
    }

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> effectBulider =
        std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    Drawing::ImageInfo pcInfo;
    Drawing::Matrix matrix;
    matrix.SetScale(1.0, 1.0);
    if (image->GetWidth() * image->GetHeight() < 10000) { // 10000 = 100 * 100 pixels
        pcInfo = Drawing::ImageInfo::MakeN32Premul(10, 10); // 10 * 10 pixels
        matrix.SetScale(10.0 / image->GetWidth(), 10.0 / image->GetHeight()); // 10.0 pixels
    } else {
        pcInfo = Drawing::ImageInfo::MakeN32Premul(100, 100); // 100 * 100 pixels
        matrix.SetScale(100.0 / image->GetWidth(), 100.0 / image->GetHeight());  // 100.0 pixels
    }
    effectBulider->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, matrix));
    std::shared_ptr<Drawing::Image> tmpColorImg = effectBulider->MakeImage(
        canvas->GetGPUContext().get(), nullptr, pcInfo, false);

    return tmpColorImg;
}

void RSPropertyDrawableUtils::GetDarkColor(RSColor& color)
{
    // convert to lab
    float minColorRange = 0;
    float maxColorRange = 255;
    float R = float(color.GetRed()) / maxColorRange;
    float G = float(color.GetGreen()) / maxColorRange;
    float B = float(color.GetBlue()) / maxColorRange;

    float X = 0.4124 * R + 0.3576 * G + 0.1805 * B;
    float Y = 0.2126 * R + 0.7152 * G + 0.0722 * B;
    float Z = 0.0193 * R + 0.1192 * G + 0.9505 * B;

    float Xn = 0.9505;
    float Yn = 1.0000;
    float Zn = 1.0889999;
    float Fx = (X / Xn) > 0.008856 ? pow((X / Xn), 1.0 / 3) : (7.787 * (X / Xn) + 16.0 / 116);
    float Fy = (Y / Yn) > 0.008856 ? pow((Y / Yn), 1.0 / 3) : (7.787 * (Y / Yn) + 16.0 / 116);
    float Fz = (Z / Zn) > 0.008856 ? pow((Z / Zn), 1.0 / 3) : (7.787 * (Z / Zn) + 16.0 / 116);
    float L = 116 * Fy - 16;
    float a = 500 * (Fx - Fy);
    float b = 200 * (Fy - Fz);

    float standardLightness = 75.0;
    if (L > standardLightness) {
        float L1 = standardLightness;
        float xw = 0.9505;
        float yw = 1.0000;
        float zw = 1.0889999;

        float fy = (L1 + 16) / 116;
        float fx = fy + (a / 500);
        float fz = fy - (b / 200);

        float X1 = xw * ((pow(fx, 3) > 0.008856) ? pow(fx, 3) : ((fx - 16.0 / 116) / 7.787));
        float Y1 = yw * ((pow(fy, 3) > 0.008856) ? pow(fy, 3) : ((fy - 16.0 / 116) / 7.787));
        float Z1 = zw * ((pow(fz, 3) > 0.008856) ? pow(fz, 3) : ((fz - 16.0 / 116) / 7.787));

        float DarkR = 3.2406 * X1 - 1.5372 * Y1 - 0.4986 * Z1;
        float DarkG = -0.9689 * X1 + 1.8758 * Y1 + 0.0415 * Z1;
        float DarkB = 0.0557 * X1 - 0.2040 * Y1 + 1.0570 * Z1;

        DarkR = std::clamp(maxColorRange * DarkR, minColorRange, maxColorRange);
        DarkG = std::clamp(maxColorRange * DarkG, minColorRange, maxColorRange);
        DarkB = std::clamp(maxColorRange * DarkB, minColorRange, maxColorRange);

        color = RSColor(DarkR, DarkG, DarkB, color.GetAlpha());
    }
}

void RSPropertyDrawableUtils::DrawFilter(Drawing::Canvas* canvas,
    const std::shared_ptr<RSFilter>& rsFilter, const std::unique_ptr<RSFilterCacheManager>& cacheManager,
    NodeId nodeId, const bool isForegroundFilter)
{
    if (!RSSystemProperties::GetBlurEnabled()) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawFilter close blur.");
        return;
    }
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter null filter.");
        return;
    }
    if (canvas == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter null canvas.");
        return;
    }

    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    auto clipIBounds = canvas->GetRoundInDeviceClipBounds();
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "DrawFilter, filterType: %d, %s, bounds: %s",
        isForegroundFilter, rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
    ROSEN_LOGD("RSPropertyDrawableUtils::DrawFilter filterType: %{public}d, %{public}s, bounds: %{public}s",
        isForegroundFilter, rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
    g_blurCnt++;

    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter surface null");
        Drawing::Brush brush = filter->GetBrush();
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
        canvas->SaveLayer(slr);
        filter->PostProcess(*canvas);
        return;
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (paintFilterCanvas == nullptr) {
        return;
    }
    RSAutoCanvasRestore autoCanvasStore(paintFilterCanvas, isForegroundFilter ?
        RSPaintFilterCanvas::SaveType::kAlpha : RSPaintFilterCanvas::SaveType::kNone);
    if (isForegroundFilter) {
        paintFilterCanvas->SetAlpha(1.0);
    }
    auto imageClipIBounds = clipIBounds;
    auto magnifierShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::MAGNIFIER);
    if (magnifierShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSMagnifierShaderFilter>(magnifierShaderFilter);
        auto canvasMatrix = canvas->GetTotalMatrix();
        tmpFilter->SetMagnifierOffset(canvasMatrix);
        imageClipIBounds.Offset(tmpFilter->GetMagnifierOffsetX(), tmpFilter->GetMagnifierOffsetY());
    }

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    auto enableCache = (!paintFilterCanvas->GetDisableFilterCache() && cacheManager != nullptr &&
        RSProperties::filterCacheEnabled_);
    if (enableCache) {
        if (cacheManager->GetCachedType() == FilterCacheType::FILTERED_SNAPSHOT) {
            g_blurCnt--;
        }

        auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
        if (rsShaderFilter != nullptr) {
            auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
            tmpFilter->IsOffscreenCanvas(true);
        }
        cacheManager->DrawFilter(*paintFilterCanvas, filter, nodeId);
        cacheManager->CompactFilterCache(); // flag for clear witch cache after drawing
        return;
    }
#endif

    auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
    if (rsShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
        tmpFilter->IsOffscreenCanvas(true);
    }

    if (imageClipIBounds.IsEmpty()) {
        return;
    }
    auto imageSnapshot = surface->GetImageSnapshot(imageClipIBounds, false);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->GetWidth(), imageSnapshot->GetHeight())) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawFilter cache image resource(w:%{public}d, h:%{public}d).",
            imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
        imageSnapshot->HintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    Drawing::Rect srcRect = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    Drawing::Rect dstRect = clipIBounds;
    filter->DrawImageRect(*canvas, imageSnapshot, srcRect, dstRect);
    filter->PostProcess(*canvas);
}

void RSPropertyDrawableUtils::BeginForegroundFilter(RSPaintFilterCanvas& canvas, const RectF& bounds)
{
    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::BeginForegroundFilter");
    auto surface = canvas.GetSurface();
    if (!surface) {
        return;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(bounds.width_, bounds.height_);
    if (!offscreenSurface) {
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    canvas.StoreCanvas();
    canvas.ReplaceMainScreenData(offscreenSurface, offscreenCanvas);
    offscreenCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    canvas.SavePCanvasList();
    canvas.RemoveAll();
    canvas.AddCanvas(offscreenCanvas.get());
}

void RSPropertyDrawableUtils::DrawForegroundFilter(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSFilter>& rsFilter)
{
    RS_OPTIONAL_TRACE_NAME("DrawForegroundFilter restore");
    auto surface = canvas.GetSurface();
    std::shared_ptr<Drawing::Image> imageSnapshot = nullptr;
    if (surface) {
        imageSnapshot = surface->GetImageSnapshot();
    } else {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawForegroundFilter Surface null");
    }

    canvas.RestorePCanvasList();
    canvas.SwapBackMainScreenData();

    if (rsFilter == nullptr) {
        return;
    }

    if (imageSnapshot == nullptr) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawForegroundFilter image null");
        return;
    }
    auto rsdrawingFilter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    if (rsdrawingFilter != nullptr && (rsFilter->GetFilterType() == RSFilter::BEZIER_WARP)) {
        rsdrawingFilter->DrawImageRect(canvas, imageSnapshot, Drawing::Rect(0, 0, imageSnapshot->GetWidth(),
            imageSnapshot->GetHeight()), Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight()));
        return;
    }

    if (rsdrawingFilter != nullptr && (rsFilter->GetFilterType() == RSFilter::CONTENT_LIGHT)) {
        rsdrawingFilter->DrawImageRect(canvas, imageSnapshot, Drawing::Rect(0, 0, imageSnapshot->GetWidth(),
            imageSnapshot->GetHeight()), Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight()));
        return;
    }

    auto foregroundFilter = std::static_pointer_cast<RSDrawingFilterOriginal>(rsFilter);
    if (foregroundFilter->GetFilterType() == RSFilter::MOTION_BLUR) {
        if (canvas.GetDisableFilterCache()) {
            foregroundFilter->DisableMotionBlur(true);
        } else {
            foregroundFilter->DisableMotionBlur(false);
        }
    }

    foregroundFilter->DrawImageRect(canvas, imageSnapshot, Drawing::Rect(0, 0, imageSnapshot->GetWidth(),
        imageSnapshot->GetHeight()), Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight()));
}

int RSPropertyDrawableUtils::GetAndResetBlurCnt()
{
    auto blurCnt = g_blurCnt;
    g_blurCnt = 0;
    return blurCnt;
}

void RSPropertyDrawableUtils::DrawBackgroundEffect(
    RSPaintFilterCanvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
    const std::unique_ptr<RSFilterCacheManager>& cacheManager,
    Drawing::RectI& bounds, bool behindWindow)
{
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect null filter");
        return;
    }
    if (canvas == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect null canvas");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect surface null");
        return;
    }
    g_blurCnt++;
    auto clipIBounds = bounds;
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::DrawBackgroundEffect " + rsFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "EffectComponent, %s, bounds: %s",
        rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
    ROSEN_LOGD("RSPropertyDrawableUtils::DrawBackgroundEffect EffectComponent, %{public}s, bounds: %{public}s",
        rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (RSProperties::filterCacheEnabled_ && cacheManager != nullptr && !canvas->GetDisableFilterCache()) {
        if (cacheManager->GetCachedType() == FilterCacheType::FILTERED_SNAPSHOT) {
            g_blurCnt--;
        }
        auto&& data = cacheManager->GeneratedCachedEffectData(*canvas, filter, clipIBounds, clipIBounds);
        cacheManager->CompactFilterCache(); // flag for clear witch cache after drawing
        behindWindow ? canvas->SetBehindWindowData(data) : canvas->SetEffectData(data);
        cacheManager->ClearEffectCacheWithDrawnRegion(*canvas, clipIBounds);
        return;
    }
#endif
    auto imageRect = clipIBounds;
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect image snapshot null");
        return;
    }
    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = Drawing::Rect(0, 0, imageRect.GetWidth(), imageRect.GetHeight());
    auto imageSnapshotBounds = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, imageSnapshotBounds, clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache), std::move(imageRect));
    behindWindow ? canvas->SetBehindWindowData(std::move(data)) : canvas->SetEffectData(std::move(data));
}

void RSPropertyDrawableUtils::DrawColorFilter(
    Drawing::Canvas* canvas, const std::shared_ptr<Drawing::ColorFilter>& colorFilter)
{
    if (colorFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawColorFilter null colorFilter.");
        return;
    }

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawColorFilter surface is null");
        return;
    }
    auto clipBounds = canvas->GetLocalClipBounds();
    Drawing::AutoCanvasRestore acr(*canvas, false);
    Drawing::SaveLayerOps slo(&clipBounds, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
    canvas->SaveLayer(slo);
}

void RSPropertyDrawableUtils::DrawLightUpEffect(Drawing::Canvas* canvas, const float lightUpEffectDegree)
{
    if (!ROSEN_GE(lightUpEffectDegree, 0.0) || !ROSEN_LNE(lightUpEffectDegree, 1.0)) {
        ROSEN_LOGE(
            "RSPropertyDrawableUtils::DrawLightUpEffect Invalid lightUpEffectDegree %{public}f", lightUpEffectDegree);
        return;
    }

    Drawing::Surface* surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawLightUpEffect surface is null");
        return;
    }

    auto clipBounds = canvas->GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds, false);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawLightUpEffect image is null");
        return;
    }
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = Drawing::ShaderEffect::CreateLightUp(lightUpEffectDegree, *imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

void RSPropertyDrawableUtils::DrawDynamicDim(Drawing::Canvas* canvas, const float dynamicDimDegree)
{
    if (!ROSEN_GE(dynamicDimDegree, 0.0) || !ROSEN_LNE(dynamicDimDegree, 1.0)) {
        ROSEN_LOGE(
            "RSPropertyDrawableUtils::DrawDynamicDim Invalid dynamicDimDegree %{public}f", dynamicDimDegree);
        return;
    }

    Drawing::Surface* surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim surface is null");
        return;
    }

    auto clipBounds = canvas->GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds, false);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim image is null");
        return;
    }

    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeDynamicDimShader(dynamicDimDegree, imageShader);
    if (shader == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim shader is null");
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertyDrawableUtils::MakeDynamicDimShader(
    float dynamicDimDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half dynamicDimDeg;
        uniform shader imageShader;

        vec3 rgb2hsv(in vec3 c)
        {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }
        vec3 hsv2rgb(in vec3 c)
        {
            vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
            return c.z * mix(vec3(1.0), rgb, c.y);
        }
        half4 main(float2 coord)
        {
            vec3 hsv = rgb2hsv(imageShader.eval(coord).rgb);
            float value = max(0.8, dynamicDimDeg); // 0.8 is min saturation ratio.
            hsv.y = hsv.y * (1.75 - value * 0.75); // saturation value [1.0 , 1.15].
            hsv.z = min(hsv.z * dynamicDimDeg, 1.0);
            return vec4(hsv2rgb(hsv), 1.0);
        }
    )";
    if (dynamicDimShaderEffect_ == nullptr) {
        dynamicDimShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (dynamicDimShaderEffect_ == nullptr) {
            ROSEN_LOGE("MakeDynamicDimShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&dynamicDimDeg, sizeof(dynamicDimDeg));
    return dynamicDimShaderEffect_->MakeShader(data, children, childCount, nullptr, false);
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertyDrawableUtils::MakeBinarizationShader(
    float low, float high, float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform mediump float ubo_low;
        uniform mediump float ubo_high;
        uniform mediump float ubo_thresholdLow;
        uniform mediump float ubo_thresholdHigh;
        uniform shader imageShader;
        mediump vec4 main(vec2 drawing_coord) {
            mediump vec3 c = imageShader(drawing_coord).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(ubo_high, -1.0, step(ubo_thresholdLow, gray));
            float highRes = mix(-1.0, ubo_low, step(ubo_thresholdHigh, gray));
            float midRes = (ubo_thresholdHigh - gray) * (ubo_high - ubo_low) /
            (ubo_thresholdHigh - ubo_thresholdLow) + ubo_low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            mediump vec3 invert = vec3(invertedGray);
            mediump vec4 res = vec4(invert, 1.0);
            return res;
        }
    )";
    if (binarizationShaderEffect_ == nullptr) {
        binarizationShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (binarizationShaderEffect_ == nullptr) {
            ROSEN_LOGE("RSPropertyDrawableUtils::MakeBinarizationShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(binarizationShaderEffect_);
    thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("ubo_low", low);
    builder->SetUniform("ubo_high", high);
    builder->SetUniform("ubo_thresholdLow", thresholdLow);
    builder->SetUniform("ubo_thresholdHigh", thresholdHigh);
    return builder->MakeShader(nullptr, false);
}

std::shared_ptr<Drawing::Blender> RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(
    const RSDynamicBrightnessPara& params)
{
    if (!RSSystemProperties::GetDynamicBrightnessEnabled()) {
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder;
    if (ROSEN_EQ(params.rates_.x_, 0.f) && ROSEN_EQ(params.rates_.y_, 0.f) && ROSEN_EQ(params.fraction_, 0.f)) {
        builder = MakeDynamicBrightnessLinearBuilder();
        ROSEN_LOGD("RSPropertyDrawableUtils::MakeDynamicBrightnessBlender equivalent blender in the linear case");
        if (!builder) {
            ROSEN_LOGE("RSPropertyDrawableUtils::MakeDynamicBrightnessBlender make linear builder fail");
            return nullptr;
        }
        // pre-process for pos and neg params
        float minusSat = 1.f - params.saturation_;
        bool swap = ROSEN_LE(minusSat, 0.f);
        const auto& posCoeffTmp = swap ? params.negCoeff_ : params.posCoeff_;
        const auto& negCoeffTmp = swap ? params.posCoeff_ : params.negCoeff_;
        builder->SetUniform("ubo_posr", posCoeffTmp.x_ * minusSat);
        builder->SetUniform("ubo_posg", posCoeffTmp.y_ * minusSat);
        builder->SetUniform("ubo_posb", posCoeffTmp.z_ * minusSat);
        builder->SetUniform("ubo_negr", negCoeffTmp.x_ * minusSat);
        builder->SetUniform("ubo_negg", negCoeffTmp.y_ * minusSat);
        builder->SetUniform("ubo_negb", negCoeffTmp.z_ * minusSat);
    } else {
        builder = MakeDynamicBrightnessBuilder();
        if (!builder) {
            ROSEN_LOGE("RSPropertyDrawableUtils::MakeDynamicBrightnessBlender make builder fail");
            return nullptr;
        }
        builder->SetUniform("ubo_fract", std::clamp(params.fraction_, 0.0f, 1.0f));
        builder->SetUniform("ubo_cubic", params.rates_.x_);
        builder->SetUniform("ubo_quad", params.rates_.y_);
        builder->SetUniform("ubo_baseSat", params.saturation_);
        builder->SetUniform("ubo_posr", params.posCoeff_.x_);
        builder->SetUniform("ubo_posg", params.posCoeff_.y_);
        builder->SetUniform("ubo_posb", params.posCoeff_.z_);
        builder->SetUniform("ubo_negr", params.negCoeff_.x_);
        builder->SetUniform("ubo_negg", params.negCoeff_.y_);
        builder->SetUniform("ubo_negb", params.negCoeff_.z_);
    }

    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::MakeDynamicBrightnessBlender");
    builder->SetUniform("ubo_rate", params.rates_.z_);
    builder->SetUniform("ubo_degree", params.rates_.w_);
    builder->SetUniform("ubo_headroom", params.enableHdr_ ? EFFECT_MAX_LUMINANCE : 1.0f);
    return builder->MakeBlender();
}

std::shared_ptr<Drawing::RuntimeBlenderBuilder> RSPropertyDrawableUtils::MakeDynamicBrightnessLinearBuilder()
{
    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::MakeDynamicBrightnessLinearBuilder");
    static constexpr char prog[] = R"(
        uniform half ubo_rate;
        uniform half ubo_degree;
        uniform half ubo_posr;
        uniform half ubo_posg;
        uniform half ubo_posb;
        uniform half ubo_negr;
        uniform half ubo_negg;
        uniform half ubo_negb;
        uniform half ubo_headroom;

        const half3 baseVec = half3(0.2412016, 0.6922296, 0.0665688);
        half3 sat(half3 inColor, half3 pos, half3 neg) {
            half3 delta = dot(inColor, baseVec) - inColor;
            half3 v = mix(neg, pos, step(0, delta));
            return clamp(inColor + delta * v, half3(0.0), half3(ubo_headroom));
        }

        half4 main(half4 src, half4 dst) {
            half2 coeff = half2(ubo_rate, ubo_degree);
            half3 color = coeff.x * dst.rgb + coeff.y * dst.a;
            half3 pos = half3(ubo_posr, ubo_posg, ubo_posb);
            half3 neg = half3(ubo_negr, ubo_negg, ubo_negb);
            color = sat(color, pos, neg);
            return half4(mix(dst.rgb, color, src.a), dst.a);
        }
    )";
    if (dynamicBrightnessLinearBlenderEffect_ == nullptr) {
        dynamicBrightnessLinearBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicBrightnessLinearBlenderEffect_ == nullptr) { return nullptr; }
    }
    return std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicBrightnessLinearBlenderEffect_);
}

std::shared_ptr<Drawing::RuntimeBlenderBuilder> RSPropertyDrawableUtils::MakeDynamicBrightnessBuilder()
{
    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::MakeDynamicBrightnessBuilder");
    static constexpr char prog[] = R"(
        uniform half ubo_fract;
        uniform half ubo_rate;
        uniform half ubo_degree;
        uniform half ubo_cubic;
        uniform half ubo_quad;
        uniform half ubo_baseSat;
        uniform half ubo_posr;
        uniform half ubo_posg;
        uniform half ubo_posb;
        uniform half ubo_negr;
        uniform half ubo_negg;
        uniform half ubo_negb;
        uniform half ubo_headroom;
 
        const vec3 baseVec = vec3(0.2412016, 0.6922296, 0.0665688);
        const half eps = 1e-5;
        half3 getUnpremulRGB(half4 color) {
            half factor = 1.0 / (max(color.a, eps));
            return saturate(color.rgb * factor);
        }
        half3 gray(half3 x, half4 coeff) {
            return coeff.x * x * x * x + coeff.y * x * x + coeff.z * x + coeff.w;
        }
        half3 sat(half3 inColor, half n, half3 pos, half3 neg) {
            half3 r = (1.0 - n) * inColor;
            half base = dot(r, baseVec);
            half3 delta = base - r;
            half3 v = mix(neg, pos, step(0, delta));
            return clamp(inColor + delta * v, half3(0.0), half3(ubo_headroom));
        }

        half4 main(half4 src, half4 dst) {
            half4 coeff = half4(ubo_cubic, ubo_quad, ubo_rate, ubo_degree);
            half3 color = gray(getUnpremulRGB(dst), coeff);
            half3 pos = half3(ubo_posr, ubo_posg, ubo_posb);
            half3 neg = half3(ubo_negr, ubo_negg, ubo_negb);
            color = sat(color, ubo_baseSat, pos, neg);
            color = mix(color, getUnpremulRGB(src), ubo_fract);
            return half4(mix(dst.rgb, color * dst.a, src.a), dst.a);
        }
    )";
    if (dynamicBrightnessBlenderEffect_ == nullptr) {
        dynamicBrightnessBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicBrightnessBlenderEffect_ == nullptr) { return nullptr; }
    }
    return std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicBrightnessBlenderEffect_);
}

std::shared_ptr<Drawing::Blender> RSPropertyDrawableUtils::MakeShadowBlender(const RSShadowBlenderPara& params)
{
    static constexpr char prog[] = R"(
        uniform half ubo_cubic;
        uniform half ubo_quadratic;
        uniform half ubo_linear;
        uniform half ubo_constant;

        float4 main(float4 src, float4 dst) {
            float lum = dot(unpremul(dst).rgb, float3(0.2126, 0.7152, 0.0722));
            float opacity = saturate(ubo_cubic * lum * lum * lum +
                ubo_quadratic * lum * lum + ubo_linear * lum + ubo_constant);
            half4 srcNew = src * opacity;
            return mix(dst, vec4(srcNew.rgb, 1.0), src.a);
        }
    )";
    if (shadowBlenderEffect_ == nullptr) {
        shadowBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
    }
    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder =
        std::make_shared<Drawing::RuntimeBlenderBuilder>(shadowBlenderEffect_);
    if (!builder) {
        ROSEN_LOGE("RSPropertyDrawableUtils::MakeShadowBlender make builder fail");
        return nullptr;
    }
    builder->SetUniform("ubo_cubic", params.cubic_);
    builder->SetUniform("ubo_quadratic", params.quadratic_);
    builder->SetUniform("ubo_linear", params.linear_);
    builder->SetUniform("ubo_constant", params.constant_);
    RS_OPTIONAL_TRACE_FMT("RSPropertyDrawableUtils::MakeShadowBlender params[%f,%f,%f,%f]",
        params.cubic_, params.quadratic_, params.linear_, params.constant_);
    return builder->MakeBlender();
}

void RSPropertyDrawableUtils::DrawBinarization(Drawing::Canvas* canvas, const std::optional<Vector4f>& aiInvert)
{
    if (!aiInvert.has_value()) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization aiInvert has no value");
        return;
    }
    auto drSurface = canvas->GetSurface();
    if (drSurface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization drSurface is null");
        return;
    }
    auto clipBounds = canvas->GetDeviceClipBounds();
    auto imageSnapshot = drSurface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization image is null");
        return;
    }
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*imageSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float thresholdLow = aiInvert->z_ - aiInvert->w_;
    float thresholdHigh = aiInvert->z_ + aiInvert->w_;
    auto shader = MakeBinarizationShader(aiInvert->x_, aiInvert->y_, thresholdLow, thresholdHigh, imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    brush.SetAntiAlias(true);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

void RSPropertyDrawableUtils::DrawPixelStretch(Drawing::Canvas* canvas,
    const std::optional<Vector4f>& pixelStretch, const RectF& boundsRect,
    const bool boundsGeoValid, const Drawing::TileMode pixelStretchTileMode)
{
    if (!pixelStretch.has_value()) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawPixelStretch pixelStretch has no value");
        return;
    }
    if (std::isinf(pixelStretch->x_) || std::isinf(pixelStretch->y_) ||
        std::isinf(pixelStretch->z_) || std::isinf(pixelStretch->w_)) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawPixelStretch skip original pixelStretch");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch surface null");
        return;
    }

    /*  Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    Drawing::Matrix worldToLocalMat;
    if (!canvas->GetTotalMatrix().Invert(worldToLocalMat)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch get invert matrix failed.");
        return;
    }
    Drawing::Rect localClipBounds;
    canvas->Save();
    canvas->ClipRect(Rect2DrawingRect(boundsRect), Drawing::ClipOp::INTERSECT, false);
    auto tmpBounds = canvas->GetDeviceClipBounds();
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSPropertyDrawableUtils::DrawPixelStretch, tmpBounds: %s", tmpBounds.ToString().c_str());
    canvas->Restore();
    Drawing::Rect clipBounds(
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetRight() - 1, tmpBounds.GetBottom() - 1);
    Drawing::Rect fClipBounds(clipBounds.GetLeft(), clipBounds.GetTop(), clipBounds.GetRight(), clipBounds.GetBottom());

    if (!worldToLocalMat.MapRect(localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch map rect failed.");
        return;
    }
    auto bounds = Rect2DrawingRect(boundsRect);
    if (!bounds.Intersect(localClipBounds)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch intersect clipbounds failed");
        return;
    }

    auto scaledBounds = Drawing::Rect(bounds.GetLeft() - pixelStretch->x_, bounds.GetTop() - pixelStretch->y_,
        bounds.GetRight() + pixelStretch->z_, bounds.GetBottom() + pixelStretch->w_);
    if (!scaledBounds.IsValid() || !bounds.IsValid() || !clipBounds.IsValid()) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch invalid scaled bounds");
        return;
    }

    Drawing::RectI rectI(static_cast<int>(fClipBounds.GetLeft()), static_cast<int>(fClipBounds.GetTop()),
        static_cast<int>(fClipBounds.GetRight()), static_cast<int>(fClipBounds.GetBottom()));
    auto image = surface->GetImageSnapshot(rectI);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch image null");
        return;
    }

    Drawing::Brush brush;
    Drawing::Matrix inverseMat;
    Drawing::Matrix rotateMat;
    if (boundsGeoValid) {
        auto transMat = canvas->GetTotalMatrix();
        /* transMat.getSkewY() is the sin of the rotation angle(sin0 = 0,sin90 =1 sin180 = 0,sin270 = -1),
            if transMat.getSkewY() is not 0 or -1 or 1,the rotation angle is not a multiple of 90,not Stretch*/
        auto skewY = transMat.Get(Drawing::Matrix::SKEW_Y);
        if (ROSEN_EQ(skewY, 0.f) || ROSEN_EQ(skewY, 1.f) || ROSEN_EQ(skewY, -1.f)) {
        } else {
            ROSEN_LOGD("rotate degree is not 0 or 90 or 180 or 270,return.");
            return;
        }
        rotateMat.SetScale(transMat.Get(Drawing::Matrix::SCALE_X), transMat.Get(Drawing::Matrix::SCALE_Y));
        rotateMat.Set(Drawing::Matrix::SKEW_X, transMat.Get(Drawing::Matrix::SKEW_X));
        rotateMat.Set(Drawing::Matrix::SKEW_Y, transMat.Get(Drawing::Matrix::SKEW_Y));
        rotateMat.PreTranslate(-bounds.GetLeft(), -bounds.GetTop());
        rotateMat.PostTranslate(bounds.GetLeft(), bounds.GetTop());

        Drawing::Rect transBounds;
        rotateMat.MapRect(transBounds, bounds);
        rotateMat.Set(Drawing::Matrix::TRANS_X, bounds.GetLeft() - transBounds.GetLeft());
        rotateMat.Set(Drawing::Matrix::TRANS_Y, bounds.GetTop() - transBounds.GetTop());
        if (!rotateMat.Invert(inverseMat)) {
            ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch get invert matrix failed.");
        }
    }

    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->Translate(bounds.GetLeft(), bounds.GetTop());
    Drawing::SamplingOptions samplingOptions;
    constexpr static float EPS = 1e-5f;
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, pixelStretchTileMode, pixelStretchTileMode, samplingOptions, inverseMat));
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas->DetachBrush();
    } else {
        inverseMat.PostScale(
            scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, pixelStretchTileMode, pixelStretchTileMode, samplingOptions, inverseMat));

        canvas->Translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(pixelStretch->x_, pixelStretch->y_, pixelStretch->x_ + bounds.GetWidth(),
            pixelStretch->y_ + bounds.GetHeight()));
        canvas->DetachBrush();
    }
}

Drawing::Path RSPropertyDrawableUtils::CreateShadowPath(const std::shared_ptr<RSPath> rsPath,
    const std::shared_ptr<RSPath>& clipBounds, const RRect& rrect)
{
    Drawing::Path path;
    if (rsPath && rsPath->GetDrawingPath().IsValid()) {
        path = rsPath->GetDrawingPath();
    } else if (clipBounds) {
        path = clipBounds->GetDrawingPath();
    } else {
        path.AddRoundRect(RRect2DrawingRRect(rrect));
    }
    return path;
}

void RSPropertyDrawableUtils::DrawShadow(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX,
    const float& offsetY, const float& elevation, const bool& isFilled, Color spotColor)
{
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSPropertyDrawableUtils::DrawShadow, ShadowElevation: %f, ShadowOffsetX: "
        "%f, ShadowOffsetY: %f, bounds: %s",
        elevation, offsetX, offsetY, path.GetBounds().ToString().c_str());
    Drawing::AutoCanvasRestore acr(*canvas, true);
    if (!isFilled) {
        canvas->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    }
    path.Offset(offsetX, offsetY);
    Drawing::Point3 planeParams = {0.0f, 0.0f, elevation};
    std::vector<Drawing::Point> pt{{path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2,
        path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2}};
    canvas->GetTotalMatrix().MapPoints(pt, pt, 1);
    Drawing::Point3 lightPos = {pt[0].GetX(), pt[0].GetY(), DEFAULT_LIGHT_HEIGHT};
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas->GetAlpha());
    spotColor.MultiplyAlpha(canvas->GetAlpha());
    canvas->DrawShadowStyle(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, Drawing::Color(ambientColor.AsArgbInt()),
        Drawing::Color(spotColor.AsArgbInt()), Drawing::ShadowFlags::TRANSPARENT_OCCLUDER, true);
}

void RSPropertyDrawableUtils::DrawShadowMaskFilter(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX,
    const float& offsetY, const float& radius, const bool& isFilled, Color spotColor)
{
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSPropertyDrawableUtils::DrawShadowMaskFilter, Radius: %f, ShadowOffsetX: "
        "%f, ShadowOffsetY: %f, bounds: %s",
        radius, offsetX, offsetY, path.GetBounds().ToString().c_str());
    Drawing::AutoCanvasRestore acr(*canvas, true);
    if (!isFilled) {
        canvas->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    }
    path.Offset(offsetX, offsetY);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(
        spotColor.GetAlpha(), spotColor.GetRed(), spotColor.GetGreen(), spotColor.GetBlue()));
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetMaskFilter(
        Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, radius));
    brush.SetFilter(filter);
    canvas->AttachBrush(brush);
    auto stencilVal = canvas->GetStencilVal();
    if (stencilVal > 0) {
        --stencilVal; // shadow positioned under app window
    }
    if (stencilVal > Drawing::Canvas::INVALID_STENCIL_VAL && stencilVal < canvas->GetMaxStencilVal()) {
        RS_OPTIONAL_TRACE_NAME_FMT("DrawPathWithStencil, stencilVal: %" PRId64 "", stencilVal);
        canvas->DrawPathWithStencil(path, static_cast<uint32_t>(stencilVal));
    } else {
        canvas->DrawPath(path);
    }
    canvas->DetachBrush();
}

void RSPropertyDrawableUtils::DrawUseEffect(RSPaintFilterCanvas* canvas, UseEffectType useEffectType)
{
    const auto& effectData = useEffectType == UseEffectType::EFFECT_COMPONENT ?
        canvas->GetEffectData() : canvas->GetBehindWindowData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr ||
        !(RSSystemProperties::GetEffectMergeEnabled() && RSFilterCacheManager::isCCMEffectMergeEnable_)) {
        return;
    }
    RS_TRACE_FUNC();
    Drawing::AutoCanvasRestore acr(*canvas, true);
    // Align the current coordinate system with the one that the effect data was generated from. In most cases,
    // the two coordinate systems are the same, so the cachedMatrix_ should be set to identity.
    canvas->SetMatrix(effectData->cachedMatrix_);
    auto visibleRect = canvas->GetVisibleRect();
    visibleRect.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int>(visibleRect.GetLeft()), static_cast<int>(visibleRect.GetTop()),
        static_cast<int>(visibleRect.GetRight()), static_cast<int>(visibleRect.GetBottom()));
    if (!visibleIRect.IsEmpty()) {
        canvas->ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
    Drawing::Brush brush;
    canvas->AttachBrush(brush);
    // Draw the cached image in the coordinate system where the effect data is generated. The image content
    // outside the device clip bounds will be automatically clipped.
    canvas->DrawImage(*effectData->cachedImage_, static_cast<float>(effectData->cachedRect_.GetLeft()),
        static_cast<float>(effectData->cachedRect_.GetTop()), Drawing::SamplingOptions());
    RS_OPTIONAL_TRACE_NAME_FMT("RSPropertyDrawableUtils::DrawUseEffect cachedRect_:%s, DeviceClipBounds:%s, "
        "IdentityMatrix: %d", effectData->cachedRect_.ToString().c_str(),
        canvas->GetDeviceClipBounds().ToString().c_str(), effectData->cachedMatrix_.IsIdentity());
    ROSEN_LOGD("RSPropertyDrawableUtils::DrawUseEffect cachedRect_:%{public}s, DeviceClipBounds:%{public}s, "
        "IdentityMatrix: %{public}d", effectData->cachedRect_.ToString().c_str(),
        canvas->GetDeviceClipBounds().ToString().c_str(), effectData->cachedMatrix_.IsIdentity());
    canvas->DetachBrush();
}

bool RSPropertyDrawableUtils::IsDangerousBlendMode(int blendMode, int blendApplyType)
{
    static const uint32_t fastDangerousBit =
        (1 << static_cast<int>(Drawing::BlendMode::CLEAR)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::XOR));
    static const uint32_t offscreenDangerousBit =
        (1 << static_cast<int>(Drawing::BlendMode::CLEAR)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_IN)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_IN)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_ATOP)) +
        (1 << static_cast<int>(Drawing::BlendMode::XOR)) +
        (1 << static_cast<int>(Drawing::BlendMode::MODULATE));
    uint32_t tmp = 1 << blendMode;
    if (blendApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        return tmp & fastDangerousBit;
    }
    return tmp & offscreenDangerousBit;
}

void RSPropertyDrawableUtils::BeginBlender(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Blender> blender,
    int blendModeApplyType, bool isDangerous)
{
    if (isDangerous && !canvas.HasOffscreenLayer()) {
        Drawing::SaveLayerOps maskLayerRec(nullptr, nullptr, 0);
        canvas.SaveLayer(maskLayerRec);
        ROSEN_LOGD("Dangerous offscreen blendmode may produce transparent pixels, add extra offscreen here.");
    }

    // fast blender
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        canvas.SetBlender(blender);
        return;
    }

    // save layer mode
    Drawing::Brush blendBrush_;

    // The savelayer in the RSPaintFilterCanvas will automatically apply alpha to the brush.
    // If the alpha of the canvas is set to the brush here, the final transparency will be
    // alpha * alpha, not alpha.
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::SAVE_LAYER_ALPHA)) {
        blendBrush_.SetAlphaF(canvas.GetAlpha());
    }
    blendBrush_.SetBlender(blender);
    uint32_t saveLayerFlag = 0;
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::SAVE_LAYER_INIT_WITH_PREVIOUS_CONTENT)) {
        // currently we only support DDGR backend, we thus use 1 << 1 as indicated in their code.
        saveLayerFlag = 1 << 1;
    }
    Drawing::SaveLayerOps maskLayerRec(nullptr, &blendBrush_, saveLayerFlag);
    canvas.SaveLayer(maskLayerRec);
    canvas.SetBlender(nullptr);
    canvas.SaveAlpha();
    canvas.SetAlpha(1.0f);
}

void RSPropertyDrawableUtils::DrawFilterWithDRM(Drawing::Canvas* canvas, bool isDark)
{
    Drawing::Brush brush;
    int16_t alpha = 245; // give a nearly opaque mask to replace blur effect
    RSColor demoColor;
    if (isDark) {
        int16_t rgb_dark = 80;
        demoColor = RSColor(rgb_dark, rgb_dark, rgb_dark, alpha);
    } else {
        int16_t rgb_light = 235;
        demoColor = RSColor(rgb_light, rgb_light, rgb_light, alpha);
    }

    float sat = 1.0f;
    float brightness = 0.9f;
    float normalizedDegree = brightness - sat;
    const float brightnessMat[] = {
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        normalizedDegree,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        normalizedDegree,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        normalizedDegree,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    float cmArray[Drawing::ColorMatrix::MATRIX_SIZE];
    cm.GetArray(cmArray);
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat);
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*filterCompose, nullptr);
    Drawing::Filter filter;
    filter.SetImageFilter(colorImageFilter);
    brush.SetFilter(filter);
    brush.SetColor(demoColor.AsArgbInt());
    canvas->DrawBackground(brush);
}

void RSPropertyDrawableUtils::EndBlender(RSPaintFilterCanvas& canvas, int blendModeApplyType)
{
    // RSRenderNodeDrawable will do other necessary work (restore canvas & env), we only need to restore alpha
    if (blendModeApplyType != static_cast<int>(RSColorBlendApplyType::FAST)) {
        canvas.RestoreAlpha();
    }
}

Color RSPropertyDrawableUtils::CalculateInvertColor(const Color& backgroundColor)
{
    uint32_t a = backgroundColor.GetAlpha();
    uint32_t r = 255 - backgroundColor.GetRed(); // 255 max red channel value
    uint32_t g = 255 - backgroundColor.GetGreen(); // 255 max green channel value
    uint32_t b = 255 - backgroundColor.GetBlue(); // 255 max blue channel value
    return Color(r, g, b, a);
}

Color RSPropertyDrawableUtils::GetInvertBackgroundColor(RSPaintFilterCanvas& canvas, bool needClipToBounds,
    const Vector4f& boundsRect, const Color& backgroundColor)
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (!needClipToBounds) {
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor not GetClipToBounds");
        Drawing::Rect rect = Drawing::Rect(0, 0, boundsRect.z_, boundsRect.w_);
        canvas.ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    }
    if (backgroundColor.GetAlpha() == 0xff) { // 0xff = 255, max alpha value
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor not alpha");
        return RSPropertyDrawableUtils::CalculateInvertColor(backgroundColor);
    }
    auto surface = canvas.GetSurface();
    std::shared_ptr<Drawing::Image> imageSnapshot = surface == nullptr ?
        nullptr :
        surface->GetImageSnapshot(canvas.GetDeviceClipBounds());
    if (imageSnapshot == nullptr) {
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor imageSnapshot null");
        return Color(0);
    }
    auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
    return RSPropertyDrawableUtils::CalculateInvertColor(Color(
        Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
        Drawing::Color::ColorQuadGetB(colorPicker), Drawing::Color::ColorQuadGetA(colorPicker)));
}

bool RSPropertyDrawableUtils::GetGravityMatrix(const Gravity& gravity, const Drawing::Rect& rect, const float& w,
    const float& h, Drawing::Matrix& mat)
{
    if (w == rect.GetWidth() && h == rect.GetHeight()) {
        return false;
    }
    mat = Drawing::Matrix();

    switch (gravity) {
        case Gravity::CENTER: {
            mat.PreTranslate((rect.GetWidth() - w) / PARAM_DOUBLE, (rect.GetHeight() - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP: {
            mat.PreTranslate((rect.GetWidth() - w) / PARAM_DOUBLE, 0);
            return true;
        }
        case Gravity::BOTTOM: {
            mat.PreTranslate((rect.GetWidth() - w) / PARAM_DOUBLE, rect.GetHeight() - h);
            return true;
        }
        case Gravity::LEFT: {
            mat.PreTranslate(0, (rect.GetHeight() - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RIGHT: {
            mat.PreTranslate(rect.GetWidth() - w, (rect.GetHeight() - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
            mat.PreTranslate(rect.GetWidth() - w, 0);
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
            mat.PreTranslate(0, rect.GetHeight() - h);
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
            mat.PreTranslate(rect.GetWidth() - w, rect.GetHeight() - h);
            return true;
        }
        case Gravity::RESIZE: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            mat.PreScale(rect.GetWidth() / w, rect.GetHeight() / h);
            return true;
        }
        case Gravity::RESIZE_ASPECT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.GetWidth() / w, rect.GetHeight() / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.GetWidth() / scale - w) / PARAM_DOUBLE,
                (rect.GetHeight() / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.GetWidth() / w, rect.GetHeight() / h);
            mat.PreScale(scale, scale);
            return true;
        }
        case Gravity::RESIZE_ASPECT_BOTTOM_RIGHT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.GetWidth() / w, rect.GetHeight() / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.GetWidth() / scale - w, rect.GetHeight() / scale - h);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.GetWidth() / w, rect.GetHeight() / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.GetWidth() / scale - w) / PARAM_DOUBLE,
                (rect.GetHeight() / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.GetWidth() / w, rect.GetHeight() / h);
            mat.PreScale(scale, scale);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.GetWidth() / w, rect.GetHeight() / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.GetWidth() / scale - w, rect.GetHeight() / scale - h);
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%{public}d]", gravity);
            return false;
        }
    }
}

bool RSPropertyDrawableUtils::RSFilterSetPixelStretch(const RSProperties& property,
    const std::shared_ptr<RSFilter>& filter)
{
    if (!filter || !RSSystemProperties::GetMESABlurFuzedEnabled()) {
        return false;
    }
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(filter);
    auto mesaShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MESA);
    if (!mesaShaderFilter) {
        return false;
    }

    auto& pixelStretch = property.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }

    constexpr static float EPS = 1e-5f;
    // The pixel stretch is fuzed only when the stretch factors are negative
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        return false;
    }

    ROSEN_LOGD("RSPropertyDrawableUtils::DrawPixelStretch fuzed with MESABlur.");
    const auto& boundsRect = property.GetBoundsRect();
    auto tileMode = property.GetPixelStretchTileMode();
    auto pixelStretchParams = std::make_shared<RSPixelStretchParams>(std::abs(pixelStretch->x_),
                                                                     std::abs(pixelStretch->y_),
                                                                     std::abs(pixelStretch->z_),
                                                                     std::abs(pixelStretch->w_),
                                                                     tileMode,
                                                                     boundsRect.width_, boundsRect.height_);
    auto mesaBlurFilter = std::static_pointer_cast<RSMESABlurShaderFilter>(mesaShaderFilter);
    mesaBlurFilter->SetPixelStretchParams(pixelStretchParams);
    return true;
}

void RSPropertyDrawableUtils::RSFilterRemovePixelStretch(const std::shared_ptr<RSFilter>& filter)
{
    if (!filter) {
        return;
    }
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(filter);
    auto mesaShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MESA);
    if (!mesaShaderFilter) {
        return;
    }

    ROSEN_LOGD("RSPropertyDrawableUtils::remove pixel stretch from the fuzed blur");
    auto mesaBlurFilter = std::static_pointer_cast<RSMESABlurShaderFilter>(mesaShaderFilter);
    std::shared_ptr<RSPixelStretchParams> pixelStretchParams = nullptr;
    mesaBlurFilter->SetPixelStretchParams(pixelStretchParams);
    return;
}

std::shared_ptr<RSFilter> RSPropertyDrawableUtils::GenerateBehindWindowFilter(float radius,
    float saturation, float brightness, RSColor maskColor)
{
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSPropertyDrawableUtils::GenerateBehindWindowFilter, Radius: %f, Saturation: %f, "
        "Brightness: %f, MaskColor: %08X", radius, saturation, brightness, maskColor.AsArgbInt());
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    uint32_t hash = hashFunc(&radius, sizeof(radius), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = GenerateMaterialColorFilter(saturation, brightness);
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter =
        Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter, radius, radius);
    std::shared_ptr<RSDrawingFilter> filter = nullptr;
    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        filter = filter?
            filter->Compose(colorImageFilter, hash) : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        filter = filter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
    } else {
        hash = hashFunc(&saturation, sizeof(saturation), hash);
        hash = hashFunc(&brightness, sizeof(brightness), hash);
        filter = filter?
            filter->Compose(blurColorFilter, hash) : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        BLUR_COLOR_MODE::DEFAULT, maskColor);
    filter = filter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    filter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(radius));
    filter->SetSaturationForHPS(saturation);
    filter->SetBrightnessForHPS(brightness);
    filter->SetFilterType(RSFilter::MATERIAL);
    return filter;
}

std::shared_ptr<Drawing::ColorFilter> RSPropertyDrawableUtils::GenerateMaterialColorFilter(float sat, float brt)
{
    float normalizedDegree = brt - 1.0;
    const float brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    float cmArray[Drawing::ColorMatrix::MATRIX_SIZE];
    cm.GetArray(cmArray);
    return Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat, Drawing::Clamp::NO_CLAMP);
}

bool RSPropertyDrawableUtils::IsBlurFilterType(const RSFilter::FilterType& filterType)
{
    switch (filterType) {
        case RSFilter::BLUR :
        case RSFilter::MATERIAL :
        case RSFilter::LINEAR_GRADIENT_BLUR :
        case RSFilter::FOREGROUND_EFFECT :
        case RSFilter::MOTION_BLUR :
            return true;
        default:
            return false;
    }
}

float RSPropertyDrawableUtils::GetBlurFilterRadius(const std::shared_ptr<RSFilter>& rsFilter)
{
    if (!rsFilter) {
        return 0;
    }
    switch (rsFilter->GetFilterType()) {
        case RSFilter::BLUR :
            return std::static_pointer_cast<RSBlurFilter>(rsFilter)->GetBlurRadiusX();
        case RSFilter::MATERIAL :
            return std::static_pointer_cast<RSMaterialFilter>(rsFilter)->GetRadius();
        case RSFilter::LINEAR_GRADIENT_BLUR :
            {
                auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
                auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
                if (rsShaderFilter != nullptr) {
                    auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                    return tmpFilter->GetLinearGradientBlurRadius();
                }
                return 0;
            }
        case RSFilter::FOREGROUND_EFFECT :
            return std::static_pointer_cast<RSForegroundEffectFilter>(rsFilter)->GetRadius();
        case RSFilter::MOTION_BLUR :
            return std::static_pointer_cast<RSMotionBlurFilter>(rsFilter)->GetRadius();
        default:
            return 0;
    }
}

Drawing::RectI RSPropertyDrawableUtils::GetRectByStrategy(
    const Drawing::Rect& rect, RoundingStrategyType roundingStrategy)
{
    switch (roundingStrategy) {
        case RoundingStrategyType::ROUND_IN:
            return Drawing::RectI(std::ceil(rect.GetLeft()), std::ceil(rect.GetTop()), std::floor(rect.GetRight()),
                std::floor(rect.GetBottom()));
        case RoundingStrategyType::ROUND_OUT:
            return Drawing::RectI(std::floor(rect.GetLeft()), std::floor(rect.GetTop()), std::ceil(rect.GetRight()),
                std::ceil(rect.GetBottom()));
        case RoundingStrategyType::ROUND_OFF:
            return Drawing::RectI(std::round(rect.GetLeft()), std::round(rect.GetTop()), std::round(rect.GetRight()),
                std::round(rect.GetBottom()));
        case RoundingStrategyType::ROUND_STATIC_CAST_INT:
            return Drawing::RectI(static_cast<int>(rect.GetLeft()), static_cast<int>(rect.GetTop()),
                static_cast<int>(rect.GetRight()), static_cast<int>(rect.GetBottom()));
        default:
            return Drawing::RectI(std::floor(rect.GetLeft()), std::floor(rect.GetTop()), std::ceil(rect.GetRight()),
                std::ceil(rect.GetBottom()));
    }
}

Drawing::RectI RSPropertyDrawableUtils::GetAbsRectByStrategy(const Drawing::Surface* surface,
    const Drawing::Matrix& totalMatrix, const Drawing::Rect& relativeRect, RoundingStrategyType roundingStrategy)
{
    Drawing::Rect absRect;
    totalMatrix.MapRect(absRect, relativeRect);
    Drawing::RectI absRectI = GetRectByStrategy(absRect, roundingStrategy);
    Drawing::RectI deviceRect(0, 0, surface->Width(), surface->Height());
    absRectI.Intersect(deviceRect);
    return absRectI;
}

std::tuple<Drawing::RectI, Drawing::RectI> RSPropertyDrawableUtils::GetAbsRectByStrategyForImage(
    const Drawing::Surface* surface, const Drawing::Matrix& totalMatrix, const Drawing::Rect& relativeRect)
{
    Drawing::Rect absRect;
    totalMatrix.MapRect(absRect, relativeRect);
    Drawing::RectI deviceRect(0, 0, surface->Width(), surface->Height());

    Drawing::RectI absImageRect = GetRectByStrategy(absRect, RoundingStrategyType::ROUND_IN);
    absImageRect.Intersect(deviceRect);

    Drawing::RectI absDrawRect = GetRectByStrategy(absRect, RoundingStrategyType::ROUND_OUT);
    absDrawRect.Intersect(deviceRect);

    return {absImageRect, absDrawRect};
}

} // namespace Rosen
} // namespace OHOS
