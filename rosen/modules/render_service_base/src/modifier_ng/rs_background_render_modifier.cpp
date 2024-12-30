/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/rs_background_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_aibar_shader_filter.h"
#include "render/rs_grey_shader_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_magnifier_shader_filter.h"
#include "render/rs_maskcolor_shader_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_mesa_blur_shader_filter.h"
#include "render/rs_water_ripple_shader_filter.h"

namespace OHOS::Rosen::ModifierNG {
const RSBackgroundFilterRenderModifier::LegacyPropertyApplierMap
    RSBackgroundFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::SYSTEMBAREFFECT,
            RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetSystemBarEffect> },
        { RSPropertyType::WATER_RIPPLE_PROGRESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetWaterRippleProgress,
                &RSProperties::GetWaterRippleProgress> },
        { RSPropertyType::WATER_RIPPLE_PARAMS, RSRenderModifier::PropertyApplyHelper<std::optional<RSWaterRipplePara>,
                                                   &RSProperties::SetWaterRippleParams> },
        { RSPropertyType::MAGNIFIER_PARA, RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSMagnifierParams>,
                                              &RSProperties::SetMagnifierParams> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadius,
                &RSProperties::GetBackgroundBlurRadius> },
        { RSPropertyType::BACKGROUND_BLUR_SATURATION,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurSaturation,
                &RSProperties::GetBackgroundBlurSaturation> },
        { RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurBrightness,
                &RSProperties::GetBackgroundBlurBrightness> },
        { RSPropertyType::BACKGROUND_BLUR_MASK_COLOR,
            RSRenderModifier::PropertyApplyHelperAdd<Color, &RSProperties::SetBackgroundBlurMaskColor,
                &RSProperties::GetBackgroundBlurMaskColor> },
        { RSPropertyType::BACKGROUND_BLUR_COLOR_MODE,
            RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetBackgroundBlurColorMode> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS_X,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadiusX,
                &RSProperties::GetBackgroundBlurRadiusX> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS_Y,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadiusY,
                &RSProperties::GetBackgroundBlurRadiusY> },
    };

void RSBackgroundFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetSystemBarEffect({});
    properties.SetWaterRippleProgress(0.0f);
    properties.SetWaterRippleParams({});
    properties.SetMagnifierParams({});
    properties.SetBackgroundBlurRadius(0.f);
    properties.SetBackgroundBlurSaturation({});
    properties.SetBackgroundBlurBrightness({});
    properties.SetBackgroundBlurMaskColor(RSColor());
    properties.SetBackgroundBlurColorMode(BLUR_COLOR_MODE::DEFAULT);
    properties.SetBackgroundBlurRadiusX(0.f);
    properties.SetBackgroundBlurRadiusY(0.f);
    properties.SetBgBlurDisableSystemAdaptation(true);
}

bool RSBackgroundFilterRenderModifier::IsBackgroundMaterialFilterValid() const
{
    auto backgroundBlurBrightness = Getter<float>(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, 1.f);
    auto backgroundBlurSaturation = Getter<float>(RSPropertyType::BACKGROUND_BLUR_SATURATION, 1.f);
    return ROSEN_GNE(Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS, 0.f), 0.9f) ||
           (!ROSEN_EQ(backgroundBlurBrightness, 1.0f) && ROSEN_GE(backgroundBlurBrightness, 0.0f)) ||
           (!ROSEN_EQ(backgroundBlurSaturation, 1.0f) && ROSEN_GE(backgroundBlurSaturation, 0.0f));
}

bool RSBackgroundFilterRenderModifier::IsBackgroundBlurRadiusXValid() const
{
    return ROSEN_GNE(Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS_X, 0.f), 0.999f);
}

bool RSBackgroundFilterRenderModifier::IsBackgroundBlurRadiusYValid() const
{
    return ROSEN_GNE(Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS_Y, 0.f), 0.999f);
}

bool RSBackgroundFilterRenderModifier::IsWaterRippleValid() const
{
    uint32_t WAVE_COUNT_MAX = 3;
    uint32_t WAVE_COUNT_MIN = 1;
    auto waterRippleProgress = Getter<float>(RSPropertyType::WATER_RIPPLE_PROGRESS, 0.f);
    auto waterRippleParams = HasProperty(RSPropertyType::WATER_RIPPLE_PARAMS)
                                 ? std::optional(Getter<RSWaterRipplePara>(RSPropertyType::WATER_RIPPLE_PARAMS))
                                 : std::nullopt;
    return ROSEN_GE(waterRippleProgress, 0.0f) && ROSEN_LE(waterRippleProgress, 1.0f) &&
           waterRippleParams.has_value() && waterRippleParams->waveCount >= WAVE_COUNT_MIN &&
           waterRippleParams->waveCount <= WAVE_COUNT_MAX;
}

void RSBackgroundFilterRenderModifier::GenerateAIBarFilter()
{
    std::vector<float> aiInvertCoef = RSAIBarShaderFilter::GetAiInvertCoef();
    float aiBarRadius = aiInvertCoef[5]; // aiInvertCoef[5] is filter_radius
    std::shared_ptr<Drawing::ImageFilter> blurFilter =
        Drawing::ImageFilter::CreateBlurImageFilter(aiBarRadius, aiBarRadius, Drawing::TileMode::CLAMP, nullptr);
    std::shared_ptr<RSAIBarShaderFilter> aiBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(aiBarShaderFilter);

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(aiBarRadius);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(kawaseBlurFilter));
    } else {
        uint32_t hash = SkOpts::hash(&aiBarRadius, sizeof(aiBarRadius), 0);
        originalFilter = originalFilter->Compose(blurFilter, hash);
    }
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::AIBAR);
}

void RSBackgroundFilterRenderModifier::GenerateMagnifierFilter()
{
    auto magnifierFilter = std::make_shared<RSMagnifierShaderFilter>(
        Getter<std::shared_ptr<RSMagnifierParams>>(RSPropertyType::MAGNIFIER_PARA));

    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(magnifierFilter);
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::MAGNIFIER);
}

void RSBackgroundFilterRenderModifier::GenerateBackgroundMaterialBlurFilter()
{
    auto backgroundColorMode = Getter<int>(RSPropertyType::BACKGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT);
    if (backgroundColorMode == BLUR_COLOR_MODE::FASTAVERAGE) {
        backgroundColorMode = BLUR_COLOR_MODE::AVERAGE;
    }
    auto backgroundBlurRadius = Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS, 0.f);
    auto backgroundBlurSaturation = Getter<float>(RSPropertyType::BACKGROUND_BLUR_SATURATION, 1.f);
    auto backgroundBlurBrightness = Getter<float>(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, 1.f);
    float radiusForHash = DecreasePrecision(backgroundBlurRadius);
    float saturationForHash = DecreasePrecision(backgroundBlurSaturation);
    float brightnessForHash = DecreasePrecision(backgroundBlurBrightness);
    uint32_t hash = SkOpts::hash(&radiusForHash, sizeof(radiusForHash), 0);
    hash = SkOpts::hash(&saturationForHash, sizeof(saturationForHash), hash);
    hash = SkOpts::hash(&brightnessForHash, sizeof(brightnessForHash), hash);

    std::shared_ptr<Drawing::ColorFilter> colorFilter =
        GetMaterialColorFilter(backgroundBlurSaturation, backgroundBlurBrightness);
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter =
        Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter, backgroundBlurRadius, backgroundBlurRadius);

    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        GenerateBackgroundMaterialFuzedBlurFilter();
        return;
    }

    if (HasProperty(RSPropertyType::GREY_COEF)) {
        auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef.x_, greyCoef.y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(backgroundBlurRadius);
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        originalFilter = originalFilter ? originalFilter->Compose(colorImageFilter, hash)
                                        : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(kawaseBlurFilter));
    } else {
        originalFilter = originalFilter ? originalFilter->Compose(blurColorFilter, hash)
                                        : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        backgroundColorMode, Getter<Color>(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, RSColor()));
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(backgroundBlurRadius));
    originalFilter->SetSaturationForHPS(backgroundBlurSaturation);
    originalFilter->SetBrightnessForHPS(backgroundBlurBrightness);
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSBackgroundFilterRenderModifier::GenerateBackgroundMaterialFuzedBlurFilter()
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
    auto backgroundBlurRadius = Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS, 0.f);
    if (HasProperty(RSPropertyType::GREY_COEF)) {
        auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(backgroundBlurRadius, greyCoef.x_, greyCoef.y_);
    } else {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(backgroundBlurRadius);
    }
    originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
    uint32_t hash = SkOpts::hash(&backgroundBlurRadius, sizeof(backgroundBlurRadius), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter =
        GetMaterialColorFilter(Getter<float>(RSPropertyType::BACKGROUND_BLUR_SATURATION, 1.f),
            Getter<float>(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, 1.f));
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    originalFilter = originalFilter->Compose(colorImageFilter, hash);

    auto backgroundColorMode = Getter<int>(RSPropertyType::BACKGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT);
    if (backgroundColorMode == BLUR_COLOR_MODE::FASTAVERAGE) {
        backgroundColorMode = BLUR_COLOR_MODE::AVERAGE;
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        backgroundColorMode, Getter<Color>(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, RSColor()));
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(backgroundBlurRadius));
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSBackgroundFilterRenderModifier::GenerateBackgroundBlurFilter()
{
    auto backgroundBlurRadiusX = Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS_X, 0.f);
    auto backgroundBlurRadiusY = Getter<float>(RSPropertyType::BACKGROUND_BLUR_RADIUS_Y, 0.f);
    std::shared_ptr<Drawing::ImageFilter> blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(
        backgroundBlurRadiusX, backgroundBlurRadiusY, Drawing::TileMode::CLAMP, nullptr);
    uint32_t hash = SkOpts::hash(&backgroundBlurRadiusX, sizeof(backgroundBlurRadiusX), 0);
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
        if (HasProperty(RSPropertyType::GREY_COEF)) {
            auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
            mesaBlurShaderFilter =
                std::make_shared<RSMESABlurShaderFilter>(backgroundBlurRadiusX, greyCoef.x_, greyCoef.y_);
        } else {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(backgroundBlurRadiusX);
        }
        originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
        originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(backgroundBlurRadiusX));
        stagingFilter_ = originalFilter;
        stagingFilter_->SetFilterType(RSFilter::BLUR);
        return;
    }

    if (HasProperty(RSPropertyType::GREY_COEF)) {
        auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
        std::shared_ptr<RSGreyShaderFilter> greyShaderFilter =
            std::make_shared<RSGreyShaderFilter>(greyCoef.x_, greyCoef.y_);
        originalFilter = std::make_shared<RSDrawingFilter>(greyShaderFilter);
    }

    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(backgroundBlurRadiusX);
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(kawaseBlurFilter);
        } else {
            originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(kawaseBlurFilter));
        }
    } else {
        if (originalFilter == nullptr) {
            originalFilter = std::make_shared<RSDrawingFilter>(blurFilter, hash);
        } else {
            originalFilter = originalFilter->Compose(blurFilter, hash);
        }
    }
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(backgroundBlurRadiusX));
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::BLUR);
}

void RSBackgroundFilterRenderModifier::GenerateWaterRippleFilter()
{
    if (!HasProperty(RSPropertyType::WATER_RIPPLE_PARAMS)) {
        return;
    }
    auto waterRippleParams = Getter<RSWaterRipplePara>(RSPropertyType::WATER_RIPPLE_PARAMS);
    uint32_t waveCount = waterRippleParams.waveCount;
    float rippleCenterX = waterRippleParams.rippleCenterX;
    float rippleCenterY = waterRippleParams.rippleCenterY;
    uint32_t rippleMode = waterRippleParams.rippleMode;
    std::shared_ptr<RSWaterRippleShaderFilter> waterRippleFilter = std::make_shared<RSWaterRippleShaderFilter>(
        Getter<float>(RSPropertyType::WATER_RIPPLE_PROGRESS, 0.f), waveCount, rippleCenterX, rippleCenterY, rippleMode);
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(waterRippleFilter);
    if (!stagingFilter_) {
        stagingFilter_ = originalFilter;
        stagingFilter_->SetFilterType(RSFilter::WATER_RIPPLE);
    } else {
        auto backgroudDrawingFilter = std::static_pointer_cast<RSDrawingFilter>(stagingFilter_);
        backgroudDrawingFilter->Compose(waterRippleFilter);
        backgroudDrawingFilter->SetFilterType(RSFilter::COMPOUND_EFFECT);
        stagingFilter_ = backgroudDrawingFilter;
    }
}

bool RSBackgroundFilterRenderModifier::OnApply(RSModifierContext& context)
{
    if (HasProperty(RSPropertyType::AIINVERT) || Getter<bool>(RSPropertyType::SYSTEMBAREFFECT, false)) {
        GenerateAIBarFilter();
    } else if (HasProperty(RSPropertyType::MAGNIFIER_PARA) &&
               ROSEN_GNE(Getter<std::shared_ptr<RSMagnifierParams>>(RSPropertyType::MAGNIFIER_PARA)->factor_, 0.f)) {
        GenerateMagnifierFilter();
    } else if (IsBackgroundMaterialFilterValid()) {
        GenerateBackgroundMaterialBlurFilter();
    } else if (IsBackgroundBlurRadiusXValid() && IsBackgroundBlurRadiusYValid()) {
        GenerateBackgroundBlurFilter();
    } else {
        stagingFilter_ = nullptr;
    }
    if (IsWaterRippleValid()) {
        GenerateWaterRippleFilter();
    }
    if (stagingFilter_ == nullptr) {
        ROSEN_LOGD("RSBackgroundFilterRenderModifier::onApply filter is null");
    }
    if (stagingFilter_ != nullptr && !stagingFilter_->IsValid()) {
        stagingFilter_.reset();
    }
    return stagingFilter_ != nullptr;
}
} // namespace OHOS::Rosen::ModifierNG
