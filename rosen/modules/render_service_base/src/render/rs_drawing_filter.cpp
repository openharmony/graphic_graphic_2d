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

#include "render/rs_drawing_filter.h"

#include <memory>

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"
#include "common/rs_optional_trace.h"
#include "draw/blend_mode.h"
#include "effect/rs_render_filter_base.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_render_aibar_filter.h"
#include "render/rs_render_grey_filter.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_light_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"

#include "rs_profiler.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "render/rs_shader_filter.h"
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int SHADER_FILTERS_SIZE = 10;
const std::map<int, std::string> FILTER_TYPE_MAP {
    { RSFilter::BLUR, "RSBlurFilterBlur" },
    { RSFilter::MATERIAL, "RSMaterialFilterBlur" },
    { RSFilter::AIBAR, "RSAIBarFilterBlur" },
    { RSFilter::LINEAR_GRADIENT_BLUR, "RSLinearGradientBlurFilterBlur" },
    { RSFilter::MAGNIFIER, "RSMagnifierFilter" },
    { RSFilter::WATER_RIPPLE, "RSWaterRippleFilter" },
    { RSFilter::COMPOUND_EFFECT, "CompoundEffect" },
    { RSFilter::FLY_OUT, "FlyOut" },
    { RSFilter::DISTORT, "RSDistortionFilter" },
    { RSFilter::EDGE_LIGHT, "EdgeLight" },
    { RSFilter::DISPERSION, "Dispersion" },
    { RSFilter::COLOR_GRADIENT, "ColorGradient" },
};
}

static bool CanApplyColorFilterInsteadOfBlur(float radius)
{
    static constexpr float epsilon = 0.999f;
    return ROSEN_LE(radius, epsilon);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    imageFilterHash_ = hash;
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<RSRenderFilterParaBase> shaderFilter)
{
    hash_ = shaderFilter->Hash();
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
    imageFilterHash_ = static_cast<uint32_t>(0);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::shared_ptr<RSRenderFilterParaBase> shaderFilter, uint32_t hash) : RSFilter(), imageFilter_(imageFilter)
{
    hash_ = shaderFilter->Hash();
    imageFilterHash_ = hash;
    shaderFilters_.reserve(SHADER_FILTERS_SIZE);
    shaderFilters_.emplace_back(shaderFilter);
}

RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    shaderFilters_ = shaderFilters;
    for (const auto& shaderFilter : shaderFilters_) {
        uint32_t hash = shaderFilter->Hash();
#ifdef USE_M133_SKIA
        const auto hashFunc = SkChecksum::Hash32;
#else
        const auto hashFunc = SkOpts::hash;
#endif
        hash_ = hashFunc(&hash, sizeof(hash), hash_);
    }
    imageFilterHash_ = hash;
}

RSDrawingFilter::~RSDrawingFilter() {};

std::string RSDrawingFilter::GetDescription()
{
    std::string filterString = GetFilterTypeString();
    for (const auto& shaderFilter : shaderFilters_) {
        if (shaderFilter == nullptr) {
            continue;
        }
        switch (shaderFilter->GetType()) {
            case RSUIFilterType::KAWASE: {
                auto filter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(shaderFilter);
                filter->GetDescription(filterString);
                break;
            }
            case RSUIFilterType::MESA: {
                auto filter = std::static_pointer_cast<RSMESABlurShaderFilter>(shaderFilter);
                int radius = filter->GetRadius();
                filterString = filterString + ", MESA radius: " + std::to_string(radius) + " sigma";
                break;
            }
            case RSUIFilterType::LINEAR_GRADIENT_BLUR: {
                auto filter4 = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(shaderFilter);
                float radius2 = filter4->GetLinearGradientBlurRadius();
                filterString = filterString + ", radius: " + std::to_string(radius2);
                break;
            }
            default:
                break;
        }
    }

    return filterString;
}

std::string RSDrawingFilter::GetDetailedDescription()
{
    std::string filterString = GetFilterTypeString();
    for (const auto& shaderFilter : shaderFilters_) {
        switch (shaderFilter->GetType()) {
            case RSUIFilterType::KAWASE: {
                auto filter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(shaderFilter);
                filter->GetDescription(filterString);
                break;
            }
            case RSUIFilterType::GREY: {
                auto filter2 = std::static_pointer_cast<RSGreyShaderFilter>(shaderFilter);
                float greyCoefLow = filter2->GetGreyCoefLow();
                float greyCoefHigh = filter2->GetGreyCoefHigh();
                filterString = filterString + ", greyCoef1: " + std::to_string(greyCoefLow);
                filterString = filterString + ", greyCoef2: " + std::to_string(greyCoefHigh);
                break;
            }
            case RSUIFilterType::MESA: {
                auto filter = std::static_pointer_cast<RSMESABlurShaderFilter>(shaderFilter);
                filterString = filterString + filter->GetDetailedDescription();
                break;
            }
            case RSUIFilterType::MASK_COLOR: {
                auto filter3 = std::static_pointer_cast<RSMaskColorShaderFilter>(shaderFilter);
                int colorMode = filter3->GetColorMode();
                RSColor maskColor = filter3->GetMaskColor();
                char maskColorStr[UINT8_MAX] = { 0 };
                auto ret = memset_s(maskColorStr, UINT8_MAX, 0, UINT8_MAX);
                if (ret != EOK) {
                    return "Failed to memset_s for maskColorStr, ret=" + std::to_string(ret);
                }
                if (sprintf_s(maskColorStr, UINT8_MAX, "%08X", maskColor.AsArgbInt()) != -1) {
                    filterString =
                        filterString + ", maskColor: " + maskColorStr + ", colorMode: " + std::to_string(colorMode);
                }
                break;
            }
            case RSUIFilterType::LINEAR_GRADIENT_BLUR: {
                auto filter4 = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(shaderFilter);
                float radius2 = filter4->GetLinearGradientBlurRadius();
                filterString = filterString + ", radius: " + std::to_string(radius2);
                break;
            }
            default:
                break;
        }
    }

    return filterString;
}

std::string RSDrawingFilter::GetFilterTypeString() const
{
    auto iter = FILTER_TYPE_MAP.find(RSFilter::type_);
    std::string filterString = "not found.";
    if (iter != FILTER_TYPE_MAP.end()) {
        filterString = iter->second;
    }
    return filterString;
}

Drawing::Brush RSDrawingFilter::GetBrush(float brushAlpha) const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    auto imageFilter = ProcessImageFilter(brushAlpha);
    if (imageFilter) {
        Drawing::Filter filter;
        filter.SetImageFilter(imageFilter);
        brush.SetFilter(filter);
    }
    return brush;
}

void RSDrawingFilter::GenerateAndUpdateGEVisualEffect()
{
    visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    for (const auto& filter : shaderFilters_) {
        filter->GenerateGEVisualEffect(visualEffectContainer_);
    }
    if (!renderFilter_) {
        return;
    }
    RSUIFilterHelper::GenerateGEVisualEffect(renderFilter_);
    RSUIFilterHelper::UpdateToGEContainer(renderFilter_, visualEffectContainer_);
}

void RSDrawingFilter::SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight)
{
    if (visualEffectContainer_) {
        visualEffectContainer_->SetGeometry(canvas, geoWidth, geoHeight);
    }
}

void RSDrawingFilter::SetDisplayHeadroom(float headroom)
{
    if (visualEffectContainer_) {
        visualEffectContainer_->SetDisplayHeadroom(headroom);
    }
}

bool RSDrawingFilter::CanSkipFrame(float radius)
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius > HEAVY_BLUR_THRESHOLD;
}

void RSDrawingFilter::OnSync()
{
    if (renderFilter_) {
        renderFilter_->OnSync();
    }
    GenerateAndUpdateGEVisualEffect();
}

uint32_t RSDrawingFilter::Hash() const
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    auto hash = hashFunc(&imageFilterHash_, sizeof(imageFilterHash_), hash_);
    return hash;
}

uint32_t RSDrawingFilter::ShaderHash() const
{
    return hash_;
}

uint32_t RSDrawingFilter::ImageHash() const
{
    return imageFilterHash_;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSDrawingFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    for (auto item : other->GetShaderFilters()) {
        result->InsertShaderFilter(item);
    }
    auto otherShaderHash = other->ShaderHash();
    auto otherImageHash = other->ImageHash();
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    result->hash_ = hashFunc(&otherShaderHash, sizeof(otherShaderHash), hash_);
    result->imageFilterHash_ = hashFunc(&otherImageHash, sizeof(otherImageHash), imageFilterHash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSRenderFilterParaBase> other) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->InsertShaderFilter(other);
    auto otherHash = other->Hash();
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    result->hash_ = hashFunc(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(
    const std::shared_ptr<Drawing::ImageFilter> other, uint32_t hash) const
{
    std::shared_ptr<RSDrawingFilter> result =
        std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other);
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    result->imageFilterHash_ = hashFunc(&hash, sizeof(hash), imageFilterHash_);
    return result;
}

std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::GetImageFilter() const
{
    return imageFilter_;
}

void RSDrawingFilter::SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
{
    imageFilter_ = imageFilter;
}

std::vector<std::shared_ptr<RSRenderFilterParaBase>> RSDrawingFilter::GetShaderFilters() const
{
    return shaderFilters_;
}

void RSDrawingFilter::InsertShaderFilter(std::shared_ptr<RSRenderFilterParaBase> shaderFilter)
{
    shaderFilters_.emplace_back(shaderFilter);
}

std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::ProcessImageFilter(float brushAlpha) const
{
    std::shared_ptr<Drawing::ImageFilter> resFilter = imageFilter_;
    if (!ROSEN_EQ(brushAlpha, 1.0f)) {
        Drawing::ColorMatrix alphaMatrix;
        alphaMatrix.SetScale(1.0f, 1.0f, 1.0f, brushAlpha);
        std::shared_ptr<Drawing::ColorFilter> alphaFilter =
            Drawing::ColorFilter::CreateMatrixColorFilter(alphaMatrix, Drawing::Clamp::NO_CLAMP);
        auto alphaImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*alphaFilter, nullptr);
        resFilter = resFilter ? Drawing::ImageFilter::CreateComposeImageFilter(alphaImageFilter, resFilter) :
            alphaImageFilter;
    }
    return resFilter;
}

void RSDrawingFilter::ApplyColorFilter(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst, float brushAlpha)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSDrawingFilter::ApplyColorFilter image is nullptr");
        return;
    }

    Drawing::Brush brush;
    std::shared_ptr<Drawing::ImageFilter> resFilter = ProcessImageFilter(brushAlpha);
    if (resFilter) {
        Drawing::Filter filter;
        filter.SetImageFilter(resFilter);
        brush.SetFilter(filter);
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();

    return;
}

bool RSDrawingFilter::ApplyImageEffectWithLightBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const DrawImageRectAttributes& attr, const Drawing::Brush& brush)
{
    if (image == nullptr) {
        return false;
    }
    auto lightBlurShaderFilter = GetShaderFilterWithType(RSUIFilterType::LIGHT_BLUR);
    if (lightBlurShaderFilter == nullptr) {
        return false;
    }
    auto tmpFilter = std::static_pointer_cast<RSLightBlurShaderFilter>(lightBlurShaderFilter);
    if (CanApplyColorFilterInsteadOfBlur(tmpFilter->GetRadius())) {
        ApplyColorFilter(canvas, image, attr.src, attr.dst, attr.brushAlpha);
        return true;
    }
    RS_OPTIONAL_TRACE_NAME("ApplyLightBlur width: " + std::to_string(attr.dst.GetWidth()) +
        ", height: " + std::to_string(attr.dst.GetHeight()));
    LightBlurParameter para { attr.src, attr.dst, brush };
    tmpFilter->ApplyLightBlur(canvas, image, para);
    RS_PROFILER_ADD_LIGHT_BLUR_METRICS(static_cast<uint32_t>(image->GetWidth() * image->GetHeight()));
    return true;
}

bool RSDrawingFilter::IsHpsBlurApplied(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& outImage,
    const DrawImageRectAttributes& attr, const Drawing::Brush& brush, float radius)
{
    if (RSSystemProperties::GetHpsBlurEnabled() && GetFilterType() == RSFilter::MATERIAL) {
        auto hpsParam = Drawing::HpsBlurParameter(attr.src, attr.dst, radius, saturationForHPS_, brightnessForHPS_);
        RSColor maskColorForHPS = RSColor();
        if (ROSEN_EQ(attr.brushAlpha, 1.0f)) {
            auto maskColorShaderFilter = GetShaderFilterWithType(RSUIFilterType::MASK_COLOR);
            if (maskColorShaderFilter != nullptr) {
                auto maskColorFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(maskColorShaderFilter);
                maskColorForHPS = maskColorFilter->GetMaskColor();
            }
        }
        bool isHpsBlurApplied = HpsBlurFilter::GetHpsBlurFilter().ApplyHpsBlur(canvas, outImage, hpsParam,
            brush.GetColor().GetAlphaF() * attr.brushAlpha, brush.GetFilter().GetColorFilter(), maskColorForHPS);
        if (isHpsBlurApplied) {
            canSkipMaskColor_ = maskColorForHPS != RSColor();
        }
        return isHpsBlurApplied;
    }
    return false;
}

void RSDrawingFilter::ProfilerLogImageEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer,
    const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
    const std::shared_ptr<Drawing::Image>& outImage)
{
    if (!visualEffectContainer) {
        return;
    }
    if (!RS_PROFILER_IS_RECORDING_MODE()) {
        return;
    }
    for (auto vef : visualEffectContainer->GetFilters()) {
        auto ve = vef->GetImpl();
        switch (ve->GetFilterType()) {
            case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
                RS_PROFILER_LOG_SHADER_CALL("KAWASE_BLUR", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR: {
                RS_PROFILER_LOG_SHADER_CALL("MESA_BLUR", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
                RS_PROFILER_LOG_SHADER_CALL("AIBAR", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::GREY: {
                RS_PROFILER_LOG_SHADER_CALL("GREY", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
                RS_PROFILER_LOG_SHADER_CALL("LINEAR_GRADIENT_BLUR", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER: {
                RS_PROFILER_LOG_SHADER_CALL("MAGNIFIER", image, src, outImage);
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE: {
                RS_PROFILER_LOG_SHADER_CALL("WATER_RIPPLE", image, src, outImage);
                break;
            }
            default:
                break;
        }
    }
}

bool RSDrawingFilter::ApplyHpsImageEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Image>& outImage, const DrawImageRectAttributes& attr, Drawing::Brush& brush)
{
    canSkipMaskColor_ = false;
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    auto hpsVisualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    for (const auto& filter : shaderFilters_) {
        filter->GenerateGEVisualEffect(hpsVisualEffectContainer);
    }
    RSColor maskColorForHPS = RSColor();
    // if alpha equal 1.0, apply maskcolor
    if (ROSEN_EQ(attr.brushAlpha, 1.0f)) {
        auto maskColorShaderFilter = GetShaderFilterWithType(RSUIFilterType::MASK_COLOR);
        if (maskColorShaderFilter != nullptr) {
            auto maskColorFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(maskColorShaderFilter);
            maskColorForHPS = maskColorFilter->GetMaskColor();
        }
    }
    auto maskColor = maskColorForHPS.AsArgbInt();
    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context = {
        image, attr.src, attr.dst, Drawing::SamplingOptions(), true, brush.GetColor().GetAlphaF() * attr.brushAlpha,
        brush.GetFilter().GetColorFilter(), maskColor, saturationForHPS_, brightnessForHPS_};

    bool kawaseHpsProcess = geRender->ApplyHpsGEImageEffect(canvas, *hpsVisualEffectContainer,
        context, outImage, brush);
    if (outImage == nullptr) {
        ROSEN_LOGD("RSDrawingFilter::ApplyHpsImageEffect ApplyHpsGEEffect failed");
    }
    if (kawaseHpsProcess) {
        canSkipMaskColor_ = maskColorForHPS != RgbPalette::Transparent();
    }
    return kawaseHpsProcess;
}

void RSDrawingFilter::DrawKawaseEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& outImage,
    const DrawImageRectAttributes& attr, const Drawing::Brush& brush,
    std::shared_ptr<RSRenderFilterParaBase>& kawaseShaderFilter)
{
    auto effectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto tmpFilter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(kawaseShaderFilter);
    tmpFilter->GenerateGEVisualEffect(effectContainer);
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    auto blurImage = geRender->ApplyImageEffect(
        canvas, *effectContainer, outImage, attr.src, attr.src, Drawing::SamplingOptions());
    if (blurImage == nullptr) {
        ROSEN_LOGE("RSDrawingFilter::DrawImageRect blurImage is null");
        return;
    }
    RS_PROFILER_ADD_KAWASE_BLUR_METRICS(static_cast<uint32_t>(outImage->GetWidth() * outImage->GetHeight()));
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*blurImage, attr.src, attr.dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
    RS_OPTIONAL_TRACE_NAME("ApplyKawaseBlur " + std::to_string(tmpFilter->GetRadius()));
}

void RSDrawingFilter::ApplyImageEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Drawing::GEVisualEffectContainer>& visualEffectContainer,
    const DrawImageRectAttributes& attr)
{
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (geRender == nullptr) {
        ROSEN_LOGE("RSDrawingFilter::DrawImageRect geRender is null");
        return;
    }
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    auto brush = GetBrush(attr.brushAlpha);
    /*
     * When calling ApplyHpsImageEffect(),
       if outImage == nullptr means:
           HPS draw shaderFilters_ fail, and func will return false,
       if outImage != nullptr and return true means:
           shaderFilters_ contain Kawase or Mesa and HPS draw shaderFilters_ succ,
       if outImage != nullptr and return false means:
           shaderFilters_ not contain Kawase and Mesa and HPS draw shaderFilters_ succ.
     * Why are visualEffectContainer NOT used when calling ApplyHpsImageEffect()?
       Due to compatibility issues with HPS 1.0, Kawase filter is skipped when generating visualEffectContainer
       in RSDrawingFilter::DrawImageRectInternal() and applied later in the following lines. If calling HPS
       successfully, it will return, skipping the following lines. Therefore, visualEffectContainer must be regenerated
       in ApplyHpsImageEffect instead of passing visualEffectContainer to get full visualEffectContainer.
     */
    if (ApplyHpsImageEffect(canvas, image, outImage, attr, brush)) {
        return;
    }
    if (outImage == nullptr) {
        outImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer, image, attr.src, attr.src,
            Drawing::SamplingOptions());

        ProfilerLogImageEffect(visualEffectContainer, image, attr.src, outImage);

        if (outImage == nullptr) {
            ROSEN_LOGE("RSDrawingFilter::DrawImageRect outImage is null");
            return;
        }
    }
    if (ApplyImageEffectWithLightBlur(canvas, outImage, attr, brush)) {
        return;
    }

    auto kawaseShaderFilter = GetShaderFilterWithType(RSUIFilterType::KAWASE);
    if (kawaseShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(kawaseShaderFilter);
        auto radius = tmpFilter->GetRadius();
        if (CanApplyColorFilterInsteadOfBlur(radius)) {
            ApplyColorFilter(canvas, outImage, attr.src, attr.dst, attr.brushAlpha);
            return;
        }
        canSkipMaskColor_ = false;
        if (IsHpsBlurApplied(canvas, outImage, attr, brush, radius)) {
            RS_OPTIONAL_TRACE_NAME("ApplyHPSBlur " + std::to_string(radius));
        } else {
            DrawKawaseEffect(canvas, outImage, attr, brush, kawaseShaderFilter);
        }
        return;
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*outImage, attr.src, attr.dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

float RSDrawingFilter::PrepareAlphaForOnScreenDraw(RSPaintFilterCanvas& paintFilterCanvas)
{
    // if canvas alpha is 1.0 or 0.0 - result is same
    float canvasAlpha = paintFilterCanvas.GetAlpha();
    float newAlpha = 1.0f;
    if (ROSEN_EQ(canvasAlpha, 1.0f) || ROSEN_EQ(canvasAlpha, 0.0f)) {
        return newAlpha;
    }
    auto maskColorShaderFilter = GetShaderFilterWithType(RSUIFilterType::MASK_COLOR);
    if (maskColorShaderFilter != nullptr) {
        auto maskColorFilter = std::static_pointer_cast<RSMaskColorShaderFilter>(maskColorShaderFilter);
        float postAlpha = maskColorFilter->GetPostProcessAlpha();
        newAlpha = (1.0f - postAlpha) / (1.0f - postAlpha  * canvasAlpha);
    }
    paintFilterCanvas.SetAlpha(1.0f);
    return newAlpha * canvasAlpha;
}

void RSDrawingFilter::DrawImageRectInternal(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const DrawImageRectAttributes& attr)
{
    if (visualEffectContainer_ == nullptr) {
        ROSEN_LOGE("RSDrawingFilter::DrawImageRect visualEffectContainer is null");
        return;
    }
    auto kawaseHpsFilter = std::any_of(shaderFilters_.begin(), shaderFilters_.end(), [](const auto& filter) {
        // If this Kawase skip is removed, set the compatiblity switch
        // for HpsGEImageEffectContext to false in ApplyHpsImageEffect()
        return filter->GetType() == RSUIFilterType::KAWASE;
    });
    auto brush = GetBrush(attr.brushAlpha);
    if (attr.discardCanvas && kawaseHpsFilter && ROSEN_EQ(brush.GetColor().GetAlphaF(), 1.0f)) {
        canvas.Discard();
    }
    ApplyImageEffect(canvas, image, visualEffectContainer_, attr);
}

void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst, const DrawImageRectParams params)
{
    float canvasAlpha = canvas.GetAlpha();
    DrawImageRectAttributes attr = { src, dst, params.discardCanvas, 1.0f };
    if (params.offscreenDraw || ROSEN_EQ(canvasAlpha, 1.0f) || ROSEN_EQ(canvasAlpha, 0.0f)) {
        DrawImageRectInternal(canvas, image, attr);
        return;
    }
    // Only RSPaintFilterCanvas can have non-zero alpha stack
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore autoCanvasRestore(paintFilterCanvas, RSPaintFilterCanvas::kAlpha);
    attr.brushAlpha = PrepareAlphaForOnScreenDraw(*paintFilterCanvas);
    DrawImageRectInternal(canvas, image, attr);
}

void RSDrawingFilter::PreProcess(std::shared_ptr<Drawing::Image>& image)
{
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->PreProcess(image);
    });
}

void RSDrawingFilter::PostProcess(Drawing::Canvas& canvas)
{
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        if ((!canSkipMaskColor_) || (!(filter->GetType() == RSUIFilterType::MASK_COLOR))) {
            filter->PostProcess(canvas);
        }
    });
}

bool RSDrawingFilter::NeedForceSubmit() const
{
    auto found = find_if(shaderFilters_.begin(), shaderFilters_.end(),
        [](const auto& filter) { return filter != nullptr && filter->NeedForceSubmit(); });
    return found != shaderFilters_.end();
}
} // namespace Rosen
} // namespace OHOS
