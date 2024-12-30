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

#include "modifier_ng/rs_foreground_render_modifier.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties.h"
#include "render/rs_grey_shader_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_maskcolor_shader_filter.h"
#include "render/rs_mesa_blur_shader_filter.h"

namespace OHOS::Rosen::ModifierNG {
const RSCompositingFilterRenderModifier::LegacyPropertyApplierMap
    RSCompositingFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::AIINVERT, RSRenderModifier::PropertyApplyHelperAdd<std::optional<Vector4f>,
                                        &RSProperties::SetAiInvert, &RSProperties::GetAiInvert> },
        { RSPropertyType::GRAY_SCALE, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                          &RSProperties::SetGrayScale, &RSProperties::GetGrayScale> },
        { RSPropertyType::BRIGHTNESS, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                          &RSProperties::SetBrightness, &RSProperties::GetBrightness> },
        { RSPropertyType::CONTRAST, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                        &RSProperties::SetContrast, &RSProperties::GetContrast> },
        { RSPropertyType::SATURATE, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                        &RSProperties::SetSaturate, &RSProperties::GetSaturate> },
        { RSPropertyType::SEPIA, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>, &RSProperties::SetSepia,
                                     &RSProperties::GetSepia> },
        { RSPropertyType::INVERT, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                      &RSProperties::SetInvert, &RSProperties::GetInvert> },
        { RSPropertyType::HUE_ROTATE, RSRenderModifier::PropertyApplyHelperAdd<std::optional<float>,
                                          &RSProperties::SetHueRotate, &RSProperties::GetHueRotate> },
        { RSPropertyType::COLOR_BLEND, RSRenderModifier::PropertyApplyHelperAdd<std::optional<Color>,
                                           &RSProperties::SetColorBlend, &RSProperties::GetColorBlend> },
        { RSPropertyType::LIGHT_UP_EFFECT_DEGREE,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetLightUpEffect> },
        { RSPropertyType::DYNAMIC_DIM_DEGREE,
            RSRenderModifier::PropertyApplyHelper<std::optional<float>, &RSProperties::SetDynamicDimDegree> },
        { RSPropertyType::FILTER,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSFilter>, &RSProperties::SetFilter> },
        { RSPropertyType::LINEAR_GRADIENT_BLUR_PARA,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSLinearGradientBlurPara>,
                &RSProperties::SetLinearGradientBlurPara> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadius,
                &RSProperties::GetForegroundBlurRadius> },
        { RSPropertyType::FOREGROUND_BLUR_SATURATION,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurSaturation,
                &RSProperties::GetForegroundBlurSaturation> },
        { RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurBrightness,
                &RSProperties::GetForegroundBlurBrightness> },
        { RSPropertyType::FOREGROUND_BLUR_MASK_COLOR,
            RSRenderModifier::PropertyApplyHelperAdd<Color, &RSProperties::SetForegroundBlurMaskColor,
                &RSProperties::GetForegroundBlurMaskColor> },
        { RSPropertyType::FOREGROUND_BLUR_COLOR_MODE,
            RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetForegroundBlurColorMode> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS_X,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadiusX,
                &RSProperties::GetForegroundBlurRadiusX> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS_Y,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadiusY,
                &RSProperties::GetForegroundBlurRadiusY> },
    };

void RSCompositingFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetAiInvert({});
    properties.SetGrayScale({});
    properties.SetBrightness({});
    properties.SetContrast({});
    properties.SetSaturate({});
    properties.SetSepia({});
    properties.SetInvert({});
    properties.SetHueRotate({});
    properties.SetColorBlend({});
    properties.SetLightUpEffect(1.f);
    properties.SetDynamicDimDegree({});
    properties.SetFilter({});
    properties.SetLinearGradientBlurPara({});
    properties.SetForegroundBlurRadius(0.f);
    properties.SetForegroundBlurSaturation({});
    properties.SetForegroundBlurBrightness({});
    properties.SetForegroundBlurMaskColor(RSColor());
    properties.SetForegroundBlurColorMode(BLUR_COLOR_MODE::DEFAULT);
    properties.SetForegroundBlurRadiusX(0.f);
    properties.SetForegroundBlurRadiusY(0.f);
    properties.SetFgBlurDisableSystemAdaptation(true);
}

bool RSCompositingFilterRenderModifier::IsLinearGradientBlurValid()
{
    if (HasProperty(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA)) {
        return ROSEN_GE(
            Getter<std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA)->blurRadius_,
            0.0);
    }
    return false;
}

bool RSCompositingFilterRenderModifier::IsForegroundBlurRadiusXValid() const
{
    return ROSEN_GNE(Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, 0.f),
        0.999f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

bool RSCompositingFilterRenderModifier::IsForegroundBlurRadiusYValid() const
{
    return ROSEN_GNE(Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, 0.f),
        0.999f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

bool RSCompositingFilterRenderModifier::IsForegroundMaterialFilterVaild() const
{
    return ROSEN_GNE(Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS, 0.f),
        0.9f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

void RSCompositingFilterRenderModifier::GenerateLinearGradientBlurFilter(const RSModifierContext& context)
{
    auto linearBlurFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(
        Getter<std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA),
        context.frame_.GetWidth(), context.frame_.GetHeight());
    std::shared_ptr<RSDrawingFilter> originalFilter = std::make_shared<RSDrawingFilter>(linearBlurFilter);

    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
}

void RSCompositingFilterRenderModifier::GenerateForegroundMaterialBlurFilter()
{
    auto foregroundColorMode = Getter<int>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT);
    if (foregroundColorMode == BLUR_COLOR_MODE::FASTAVERAGE) {
        foregroundColorMode = BLUR_COLOR_MODE::AVERAGE;
    }

    auto foregroundBlurRadius = Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS, 0.f);
    auto foregroundBlurSaturation = Getter<float>(RSPropertyType::FOREGROUND_BLUR_SATURATION, 1.f);
    auto foregroundBlurBrightness = Getter<float>(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, 1.f);
    uint32_t hash = SkOpts::hash(&foregroundBlurRadius, sizeof(foregroundBlurRadius), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter =
        GetMaterialColorFilter(foregroundBlurSaturation, foregroundBlurBrightness);
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter =
        Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter, foregroundBlurRadius, foregroundBlurRadius);

    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        GenerateCompositingMaterialFuzedBlurFilter();
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
            std::make_shared<RSKawaseBlurShaderFilter>(foregroundBlurRadius);
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        originalFilter = originalFilter ? originalFilter->Compose(colorImageFilter, hash)
                                        : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(kawaseBlurFilter));
    } else {
        hash = SkOpts::hash(&foregroundBlurSaturation, sizeof(foregroundBlurSaturation), hash);
        hash = SkOpts::hash(&foregroundBlurBrightness, sizeof(foregroundBlurBrightness), hash);
        originalFilter = originalFilter ? originalFilter->Compose(blurColorFilter, hash)
                                        : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        foregroundColorMode, Getter<Color>(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, RSColor()));
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(foregroundBlurRadius));
    originalFilter->SetSaturationForHPS(foregroundBlurSaturation);
    originalFilter->SetBrightnessForHPS(foregroundBlurBrightness);
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSCompositingFilterRenderModifier::GenerateCompositingMaterialFuzedBlurFilter()
{
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;
    std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
    auto foregroundBlurRadius = Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS, 0.f);
    auto foregroundBlurSaturation = Getter<float>(RSPropertyType::FOREGROUND_BLUR_SATURATION, 1.f);
    auto foregroundBlurBrightness = Getter<float>(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, 1.f);
    if (HasProperty(RSPropertyType::GREY_COEF)) {
        auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(foregroundBlurRadius, greyCoef.x_, greyCoef.y_);
    } else {
        mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(foregroundBlurRadius);
    }
    originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
    uint32_t hash = SkOpts::hash(&foregroundBlurRadius, sizeof(foregroundBlurRadius), 0);
    std::shared_ptr<Drawing::ColorFilter> colorFilter =
        GetMaterialColorFilter(foregroundBlurSaturation, foregroundBlurBrightness);
    auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    originalFilter = originalFilter->Compose(colorImageFilter, hash);

    auto foregroundColorMode = Getter<int>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT);
    if (foregroundColorMode == BLUR_COLOR_MODE::FASTAVERAGE) {
        foregroundColorMode = BLUR_COLOR_MODE::AVERAGE;
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(
        foregroundColorMode, Getter<Color>(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, RSColor()));
    originalFilter = originalFilter->Compose(std::static_pointer_cast<RSShaderFilter>(maskColorShaderFilter));
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(foregroundBlurRadius));
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::MATERIAL);
}

void RSCompositingFilterRenderModifier::GenerateForegroundBlurFilter()
{
    auto foregroundBlurRadiusX = Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, 0.f);
    auto foregroundBlurRadiusY = Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, 0.f);
    std::shared_ptr<Drawing::ImageFilter> blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(
        foregroundBlurRadiusX, foregroundBlurRadiusY, Drawing::TileMode::CLAMP, nullptr);
    uint32_t hash = SkOpts::hash(&foregroundBlurRadiusX, sizeof(foregroundBlurRadiusX), 0);
    std::shared_ptr<RSDrawingFilter> originalFilter = nullptr;

    // fuse grey-adjustment and pixel-stretch with blur filter
    if (NeedBlurFuzed()) {
        std::shared_ptr<RSMESABlurShaderFilter> mesaBlurShaderFilter;
        if (HasProperty(RSPropertyType::GREY_COEF)) {
            auto greyCoef = Getter<Vector2f>(RSPropertyType::GREY_COEF);
            mesaBlurShaderFilter =
                std::make_shared<RSMESABlurShaderFilter>(foregroundBlurRadiusX, greyCoef.x_, greyCoef.y_);
        } else {
            mesaBlurShaderFilter = std::make_shared<RSMESABlurShaderFilter>(foregroundBlurRadiusX);
        }
        originalFilter = std::make_shared<RSDrawingFilter>(mesaBlurShaderFilter);
        originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(foregroundBlurRadiusX));
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
            std::make_shared<RSKawaseBlurShaderFilter>(foregroundBlurRadiusX);
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
    originalFilter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(foregroundBlurRadiusX));
    stagingFilter_ = originalFilter;
    stagingFilter_->SetFilterType(RSFilter::BLUR);
}

bool RSCompositingFilterRenderModifier::OnApply(RSModifierContext& context)
{
    if (IsLinearGradientBlurValid()) {
        GenerateLinearGradientBlurFilter(context);
    } else if (IsForegroundMaterialFilterVaild()) {
        GenerateForegroundMaterialBlurFilter();
    } else if (IsForegroundBlurRadiusXValid() && IsForegroundBlurRadiusYValid()) {
        GenerateForegroundBlurFilter();
    } else {
        stagingFilter_ = nullptr;
    }
    if (stagingFilter_ == nullptr) {
        ROSEN_LOGD("RSCompositingFilterRenderModifier::onApply filter is null");
    }
    if (stagingFilter_ != nullptr && !stagingFilter_->IsValid()) {
        stagingFilter_.reset();
    }
    return stagingFilter_ != nullptr;
}
} // namespace OHOS::Rosen::ModifierNG
