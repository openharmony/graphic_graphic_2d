/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "property/rs_effect_properties.h"

#include <algorithm>

#include "common/rs_optional_trace.h"
#include "effect/rs_render_filter_base.h"
#include "render/rs_drawing_filter.h"

/**
 * Usage:
 * WITH_FILTER_EFFECT(optionalMember.reset());
 * WITH_FILTER_EFFECT(rawPointer = nullptr);
 * WITH_FILTER_EFFECT(optionalMem = std::nullopt);
 * WITH_FILTER_EFFECT(container.clear());
 */
#undef WITH_FILTER_EFFECT
#define WITH_FILTER_EFFECT(expr) \
    do { \
        if (filterEffect_) { \
            (filterEffect_->expr); \
        } \
    } while (0)

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_4 = 4;
constexpr int32_t INDEX_5 = 5;
constexpr int32_t INDEX_6 = 6;
constexpr int32_t INDEX_7 = 7;
constexpr int32_t INDEX_9 = 9;
constexpr int32_t INDEX_10 = 10;
constexpr int32_t INDEX_11 = 11;
constexpr int32_t INDEX_12 = 12;
constexpr int32_t INDEX_14 = 14;
constexpr int32_t INDEX_18 = 18;

constexpr float SPHERIZE_VALID_EPSILON = 0.001f; // used to judge if spherize valid
constexpr float ATTRACTION_VALID_EPSILON = 0.001f; // used to judge if attraction valid
} // namespace

// filter effect params
void RSEffectProperties::SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para)
{
    GetFilterEffect().motionBlurPara_ = para;
}

const std::shared_ptr<MotionBlurParam>& RSEffectProperties::GetMotionBlurPara() const
{
    static const std::shared_ptr<MotionBlurParam> defaultValue = nullptr;
    if (filterEffect_ != nullptr) {
        return filterEffect_->motionBlurPara_;
    }
    return defaultValue;
}

void RSEffectProperties::SetWaterRippleProgress(const float& progress)
{
    GetFilterEffect().waterRippleProgress_ = progress;
}

float RSEffectProperties::GetWaterRippleProgress() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->waterRippleProgress_;
    }
    return 0.0f;
}

void RSEffectProperties::SetWaterRippleParams(const std::optional<RSWaterRipplePara>& params)
{
    GetFilterEffect().waterRippleParams_ = params;
}

std::optional<RSWaterRipplePara> RSEffectProperties::GetWaterRippleParams() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->waterRippleParams_;
    }
    return std::nullopt;
}

bool RSEffectProperties::IsWaterRippleValid() const
{
    uint32_t WAVE_COUNT_MAX = 3;
    uint32_t WAVE_COUNT_MIN = 1;
    float waterRippleProgress = GetWaterRippleProgress();
    const auto& waterRippleParams = GetWaterRippleParams();
    return ROSEN_GE(waterRippleProgress, 0.0f) && ROSEN_LE(waterRippleProgress, 1.0f) &&
           waterRippleParams.has_value() && waterRippleParams->waveCount >= WAVE_COUNT_MIN &&
           waterRippleParams->waveCount <= WAVE_COUNT_MAX;
}

void RSEffectProperties::SetFlyOutDegree(const float& degree)
{
    GetFilterEffect().flyOutDegree_ = degree;
}

float RSEffectProperties::GetFlyOutDegree() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->flyOutDegree_;
    }
    return 0.0f;
}

void RSEffectProperties::SetFlyOutParams(const std::optional<RSFlyOutPara>& params)
{
    GetFilterEffect().flyOutParams_ = params;
}

std::optional<RSFlyOutPara> RSEffectProperties::GetFlyOutParams() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->flyOutParams_;
    }
    return std::nullopt;
}

bool RSEffectProperties::IsFlyOutValid() const
{
    const auto& flyOutDegree = GetFlyOutDegree();
    return ROSEN_GE(flyOutDegree, 0.0f) && ROSEN_LE(flyOutDegree, 1.0f) && GetFlyOutParams().has_value();
}

void RSEffectProperties::SetSpherize(float spherizeDegree)
{
    GetFilterEffect().spherizeDegree_ = spherizeDegree;
}

float RSEffectProperties::GetSpherize() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->spherizeDegree_;
    }
    return 0.f;
}

bool RSEffectProperties::IsSpherizeValid() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->spherizeDegree_ > SPHERIZE_VALID_EPSILON;
    }
    return false;
}

void RSEffectProperties::SetLightUpEffect(float lightUpEffectDegree)
{
    GetFilterEffect().lightUpEffectDegree_ = lightUpEffectDegree;
}

float RSEffectProperties::GetLightUpEffect() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->lightUpEffectDegree_;
    }
    return 1.0f;
}

bool RSEffectProperties::IsLightUpEffectValid() const
{
    return ROSEN_GE(GetLightUpEffect(), 0.0) && ROSEN_LNE(GetLightUpEffect(), 1.0);
}

void RSEffectProperties::SetForegroundEffectRadius(float foregroundEffectRadius)
{
    GetFilterEffect().foregroundEffectRadius_ = foregroundEffectRadius;
}

float RSEffectProperties::GetForegroundEffectRadius() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->foregroundEffectRadius_;
    }
    return 0.f;
}

bool RSEffectProperties::IsForegroundEffectRadiusValid() const
{
    return ROSEN_GNE(GetForegroundEffectRadius(), 0.999f); // if blur radius < 1, no need to draw
}

void RSEffectProperties::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    GetFilterEffect().linearGradientBlurPara_ = para;
}

const std::shared_ptr<RSLinearGradientBlurPara>& RSEffectProperties::GetLinearGradientBlurPara() const
{
    static const std::shared_ptr<RSLinearGradientBlurPara> defaultValue = nullptr;
    if (filterEffect_ != nullptr) {
        return filterEffect_->linearGradientBlurPara_;
    }
    return defaultValue;
}

void RSEffectProperties::IfLinearGradientBlurInvalid()
{
    if (GetLinearGradientBlurPara() != nullptr) {
        bool isValid = ROSEN_GE(GetLinearGradientBlurPara()->blurRadius_, 0.0);
        if (!isValid) {
            WITH_FILTER_EFFECT(linearGradientBlurPara_.reset());
        }
    }
}

void RSEffectProperties::SetGreyCoef(const std::optional<Vector2f>& greyCoef)
{
    GetFilterEffect().greyCoef_ = greyCoef;
    GetFilterEffect().greyCoefNeedUpdate_ = true;
}

const std::optional<Vector2f>& RSEffectProperties::GetGreyCoef() const
{
    static const std::optional<Vector2f> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->greyCoef_;
    }
    return defaultValue;
}

bool RSEffectProperties::IsGreyCoefNeedUpdate() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->greyCoefNeedUpdate_;
    }
    return false;
}

void RSEffectProperties::SetGreyCoefNeedUpdate(bool needUpdate)
{
    if (filterEffect_ != nullptr) {
        filterEffect_->greyCoefNeedUpdate_ = needUpdate;
    }
}

void RSEffectProperties::CheckGreyCoef()
{
    const auto& greyCoef_ = GetGreyCoef();
    if (!greyCoef_.has_value()) {
        return;
    }
    // 127.0 half of 255.0
    if (ROSEN_LNE(greyCoef_->x_, 0.f) || ROSEN_GNE(greyCoef_->x_, 127.f) ||
        ROSEN_LNE(greyCoef_->y_, 0.f) || ROSEN_GNE(greyCoef_->y_, 127.f) ||
        (ROSEN_EQ(greyCoef_->x_, 0.f) && ROSEN_EQ(greyCoef_->y_, 0.f))) {
        WITH_FILTER_EFFECT(greyCoef_ = std::nullopt);
    }
}

void RSEffectProperties::SetDynamicDimDegree(const std::optional<float>& dimDegree)
{
    GetFilterEffect().dynamicDimDegree_ = dimDegree;
}

const std::optional<float>& RSEffectProperties::GetDynamicDimDegree() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->dynamicDimDegree_;
    }
    return defaultValue;
}

bool RSEffectProperties::IsDynamicDimValid() const
{
    const auto& dynamicDimDegree = GetDynamicDimDegree();
    return dynamicDimDegree.has_value() &&
           ROSEN_GE(*dynamicDimDegree, 0.0) && ROSEN_LNE(*dynamicDimDegree, 1.0);
}

void RSEffectProperties::SetGrayScale(const std::optional<float>& grayScale)
{
    GetFilterEffect().grayScale_ = grayScale;
}

const std::optional<float>& RSEffectProperties::GetGrayScale() const
{
    static const RS_HIDDEN std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->grayScale_;
    }
    return defaultValue;
}

void RSEffectProperties::SetBrightness(const std::optional<float>& brightness)
{
    GetFilterEffect().brightness_ = brightness;
}

const std::optional<float>& RSEffectProperties::GetBrightness() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->brightness_;
    }
    return defaultValue;
}

void RSEffectProperties::SetContrast(const std::optional<float>& contrast)
{
    GetFilterEffect().contrast_ = contrast;
}

const std::optional<float>& RSEffectProperties::GetContrast() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->contrast_;
    }
    return defaultValue;
}

void RSEffectProperties::SetSaturate(const std::optional<float>& saturate)
{
    GetFilterEffect().saturate_ = saturate;
}

const std::optional<float>& RSEffectProperties::GetSaturate() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->saturate_;
    }
    return defaultValue;
}

void RSEffectProperties::SetSepia(const std::optional<float>& sepia)
{
    GetFilterEffect().sepia_ = sepia;
}

const std::optional<float>& RSEffectProperties::GetSepia() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->sepia_;
    }
    return defaultValue;
}

void RSEffectProperties::SetInvert(const std::optional<float>& invert)
{
    GetFilterEffect().invert_ = invert;
}

const std::optional<float>& RSEffectProperties::GetInvert() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->invert_;
    }
    return defaultValue;
}

void RSEffectProperties::SetHueRotate(const std::optional<float>& hueRotate)
{
    GetFilterEffect().hueRotate_ = hueRotate;
}

const std::optional<float>& RSEffectProperties::GetHueRotate() const
{
    static const std::optional<float> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->hueRotate_;
    }
    return defaultValue;
}

void RSEffectProperties::SetAiInvert(const std::optional<Vector4f>& aiInvert)
{
    GetFilterEffect().aiInvert_ = aiInvert;
}

const std::optional<Vector4f>& RSEffectProperties::GetAiInvert() const
{
    static const std::optional<Vector4f> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->aiInvert_;
    }
    return defaultValue;
}

void RSEffectProperties::SetSystemBarEffect(bool systemBarEffect)
{
    GetFilterEffect().systemBarEffect_ = systemBarEffect;
}

bool RSEffectProperties::GetSystemBarEffect() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->systemBarEffect_;
    }
    return false;
}

bool RSEffectProperties::GetColorAdaptive() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->colorAdaptive_;
    }
    return false;
}

void RSEffectProperties::SetColorAdaptive(bool value)
{
    GetFilterEffect().colorAdaptive_ = value;
}

const std::shared_ptr<Drawing::ColorFilter>& RSEffectProperties::GetColorFilter() const
{
    static const std::shared_ptr<Drawing::ColorFilter> defaultValue = nullptr;
    if (filterEffect_ != nullptr) {
        return filterEffect_->colorFilter_;
    }
    return defaultValue;
}

bool RSEffectProperties::GenerateColorFilter()
{
    WITH_FILTER_EFFECT(colorFilter_ = nullptr);
    if (!GetGrayScale() && !GetBrightness() && !GetContrast() && !GetSaturate() && !GetSepia() &&
        !GetInvert() && !GetHueRotate() && !GetColorBlend()) {
        return false;
    }
    GenerateColorFilterByGrayScale();
    GenerateColorFilterByBrightness();
    GenerateColorFilterByContrast();
    GenerateColorFilterBySaturate();
    GenerateColorFilterBySepia();
    GenerateColorFilterByInvert();
    GenerateColorFilterByHueRotate();
    GenerateColorFilterByColorBlend();
    return true;
}

void RSEffectProperties::GenerateColorFilterByGrayScale()
{
    const auto& grayScale_ = GetGrayScale();
    if (grayScale_.has_value() && ROSEN_GNE(*grayScale_, 0.f)) {
        auto grayScale = grayScale_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_5] = matrix[INDEX_10] = 0.2126f * grayScale; // 0.2126 : gray scale coefficient
        matrix[1] = matrix[INDEX_6] = matrix[INDEX_11] = 0.7152f * grayScale; // 0.7152 : gray scale coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = matrix[INDEX_12] = 0.0722f * grayScale; // 0.0722 : gray scale coefficient
        matrix[INDEX_18] = 1.0 * grayScale;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterByBrightness()
{
    const auto& brightness_ = GetBrightness();
    if (brightness_.has_value() && !ROSEN_EQ<float>(*brightness_, 1.0, 0.001f)) {
        auto brightness = brightness_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        // shift brightness to (-1, 1)
        brightness = brightness - 1;
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = matrix[INDEX_18] = 1.0f;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = brightness;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterByContrast()
{
    const auto& contrast_ = GetContrast();
    if (contrast_.has_value() && !ROSEN_EQ<float>(*contrast_, 1.0, 0.001f)) {
        auto contrast = contrast_.value();
        uint32_t contrastValue128 = 128;
        uint32_t contrastValue255 = 255;
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = contrast;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = contrastValue128 * (1 - contrast) / contrastValue255;
        matrix[INDEX_18] = 1.0f;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterBySaturate()
{
    const auto& saturate_ = GetSaturate();
    if (saturate_.has_value() && !ROSEN_EQ<float>(*saturate_, 1.0, 0.001f) && ROSEN_GE(*saturate_, 0.0)) {
        auto saturate = saturate_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.3086f * (1 - saturate) + saturate; // 0.3086 : saturate coefficient
        matrix[1] = matrix[INDEX_11] = 0.6094f * (1 - saturate); // 0.6094 : saturate coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = 0.0820f * (1 - saturate); // 0.0820 : saturate coefficient
        matrix[INDEX_5] = matrix[INDEX_10] = 0.3086f * (1 - saturate); // 0.3086 : saturate coefficient
        matrix[INDEX_6] = 0.6094f * (1 - saturate) + saturate; // 0.6094 : saturate coefficient
        matrix[INDEX_12] = 0.0820f * (1 - saturate) + saturate; // 0.0820 : saturate coefficient
        matrix[INDEX_18] = 1.0f;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterBySepia()
{
    const auto& sepia_ = GetSepia();
    if (sepia_.has_value() && ROSEN_GNE(*sepia_, 0.0)) {
        auto sepia = sepia_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.393f * sepia;
        matrix[1] = 0.769f * sepia;
        matrix[INDEX_2] = 0.189f * sepia;

        matrix[INDEX_5] = 0.349f * sepia;
        matrix[INDEX_6] = 0.686f * sepia;
        matrix[INDEX_7] = 0.168f * sepia;

        matrix[INDEX_10] = 0.272f * sepia;
        matrix[INDEX_11] = 0.534f * sepia;
        matrix[INDEX_12] = 0.131f * sepia;
        matrix[INDEX_18] = 1.0f * sepia;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterByInvert()
{
    const auto& invert_ = GetInvert();
    if (invert_.has_value() && ROSEN_GNE(*invert_, 0.0)) {
        auto invert = invert_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        if (invert > 1.0) {
            invert = 1.0;
        }
        // complete color invert when dstRGB = 1 - srcRGB
        // map (0, 1) to (1, -1)
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1.0 - 2.0 * invert; // 2.0: invert
        matrix[INDEX_18] = 1.0f;
        // invert = 0.5 -> RGB = (0.5, 0.5, 0.5) -> image completely gray
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = invert;
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterByHueRotate()
{
    const auto& hueRotate_ = GetHueRotate();
    if (hueRotate_.has_value() && ROSEN_GNE(*hueRotate_, 0.0)) {
        auto hueRotate = hueRotate_.value();
        while (ROSEN_GE(hueRotate, 360)) { // 360 : degree
            hueRotate -= 360; // 360 : degree
        }
        float matrix[20] = { 0.0f }; // 20 : matrix size
        int32_t type = hueRotate / 120; // 120 : degree
        float N = (hueRotate - 120 * type) / 120; // 120 : degree
        switch (type) {
            case 0:
                // color change = R->G, G->B, B->R
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = N;
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 1:
                // compare to original: R->B, G->R, B->G
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = N;
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 2: // 2: back to normal color
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = N;
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            default:
                break;
        }
        std::shared_ptr<Drawing::ColorFilter> filter =
            Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::GenerateColorFilterByColorBlend()
{
    const auto& colorBlend_ = GetColorBlend();
    if (colorBlend_.has_value() && *colorBlend_ != RgbPalette::Transparent()) {
        auto colorBlend = colorBlend_.value();
        std::shared_ptr<Drawing::ColorFilter>  filter = Drawing::ColorFilter::CreateBlendModeColorFilter(
            colorBlend.AsArgbInt(), Drawing::BlendMode::PLUS);
        if (GetColorFilter()) {
            filter->Compose(*GetColorFilter());
        }
        GetFilterEffect().colorFilter_ = filter;
    }
}

void RSEffectProperties::SetBackgroundBlurRadius(float radius)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->radius = radius;
}

float RSEffectProperties::GetBackgroundBlurRadius() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->radius;
    }
    return 0.f;
}

bool RSEffectProperties::IsBackgroundBlurRadiusValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadius(), 0.9f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

void RSEffectProperties::SetBackgroundBlurSaturation(float saturation)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->saturation = saturation;
}

float RSEffectProperties::GetBackgroundBlurSaturation() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->saturation;
    }
    return 1.f;
}

bool RSEffectProperties::IsBackgroundBlurSaturationValid() const
{
    return (!ROSEN_EQ(GetBackgroundBlurSaturation(), 1.0f)) && ROSEN_GE(GetBackgroundBlurSaturation(), 0.0f);
}

void RSEffectProperties::SetBackgroundBlurBrightness(float brightness)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->brightness = brightness;
}

float RSEffectProperties::GetBackgroundBlurBrightness() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->brightness;
    }
    return 1.f;
}

bool RSEffectProperties::IsBackgroundBlurBrightnessValid() const
{
    return (!ROSEN_EQ(GetBackgroundBlurBrightness(), 1.0f)) && ROSEN_GE(GetBackgroundBlurBrightness(), 0.0f);
}

void RSEffectProperties::SetBackgroundBlurMaskColor(Color maskColor)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->maskColor = maskColor;
}

const Color& RSEffectProperties::GetBackgroundBlurMaskColor() const
{
    static const Color defaultValue = RSColor();
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->maskColor;
    }
    return defaultValue;
}

bool RSEffectProperties::IsBackgroundBlurMaskColorValid() const
{
    return GetBackgroundBlurMaskColor() != RSColor();
}

void RSEffectProperties::SetBackgroundBlurColorMode(int colorMode)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->colorMode = colorMode;
}

int RSEffectProperties::GetBackgroundBlurColorMode() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->colorMode;
    }
    return BLUR_COLOR_MODE::DEFAULT;
}

void RSEffectProperties::SetBackgroundBlurRadiusX(float radiusX)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->radiusX = radiusX;
}

float RSEffectProperties::GetBackgroundBlurRadiusX() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->radiusX;
    }
    return 0.f;
}

bool RSEffectProperties::IsBackgroundBlurRadiusXValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadiusX(), 0.999f);
}

void RSEffectProperties::SetBackgroundBlurRadiusY(float radiusY)
{
    if (!GetFilterEffect().backgroundBlurPara_) {
        GetFilterEffect().backgroundBlurPara_ = std::make_unique<RSBackgroundBlurPara>();
    }
    GetFilterEffect().backgroundBlurPara_->radiusY = radiusY;
}

float RSEffectProperties::GetBackgroundBlurRadiusY() const
{
    if (filterEffect_ && filterEffect_->backgroundBlurPara_) {
        return filterEffect_->backgroundBlurPara_->radiusY;
    }
    return 0.f;
}

bool RSEffectProperties::IsBackgroundBlurRadiusYValid() const
{
    return ROSEN_GNE(GetBackgroundBlurRadiusY(), 0.999f);
}

void RSEffectProperties::SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    GetFilterEffect().bgBlurDisableSystemAdaptation = disableSystemAdaptation;
}

bool RSEffectProperties::GetBgBlurDisableSystemAdaptation() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->bgBlurDisableSystemAdaptation;
    }
    return true;
}

void RSEffectProperties::SetForegroundBlurRadius(float radius)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->radius = radius;
}

float RSEffectProperties::GetForegroundBlurRadius() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->radius;
    }
    return 0.f;
}

bool RSEffectProperties::IsForegroundBlurRadiusValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadius(), 0.9f); // Adjust the materialBlur radius to 0.9 for the spring curve
}

void RSEffectProperties::SetForegroundBlurSaturation(float saturation)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->saturation = saturation;
}

float RSEffectProperties::GetForegroundBlurSaturation() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->saturation;
    }
    return 1.f;
}

bool RSEffectProperties::IsForegroundBlurSaturationValid() const
{
    return ROSEN_GE(GetForegroundBlurSaturation(), 1.0);
}

void RSEffectProperties::SetForegroundBlurBrightness(float brightness)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->brightness = brightness;
}

float RSEffectProperties::GetForegroundBlurBrightness() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->brightness;
    }
    return 1.f;
}

bool RSEffectProperties::IsForegroundBlurBrightnessValid() const
{
    return ROSEN_GE(GetForegroundBlurBrightness(), 1.0);
}

void RSEffectProperties::SetForegroundBlurMaskColor(Color maskColor)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->maskColor = maskColor;
}

const Color& RSEffectProperties::GetForegroundBlurMaskColor() const
{
    static const Color defaultValue = RSColor();
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->maskColor;
    }
    return defaultValue;
}

bool RSEffectProperties::IsForegroundBlurMaskColorValid() const
{
    return GetForegroundBlurMaskColor() != RSColor();
}

void RSEffectProperties::SetForegroundBlurColorMode(int colorMode)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->colorMode = colorMode;
}

int RSEffectProperties::GetForegroundBlurColorMode() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->colorMode;
    }
    return BLUR_COLOR_MODE::DEFAULT;
}

void RSEffectProperties::SetForegroundBlurRadiusX(float radiusX)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->radiusX = radiusX;
}

float RSEffectProperties::GetForegroundBlurRadiusX() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->radiusX;
    }
    return 0.f;
}

bool RSEffectProperties::IsForegroundBlurRadiusXValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadiusX(), 0.999f);
}

void RSEffectProperties::SetForegroundBlurRadiusY(float radiusY)
{
    if (!GetFilterEffect().foregroundBlurPara_) {
        GetFilterEffect().foregroundBlurPara_ = std::make_unique<RSForegroundBlurPara>();
    }
    GetFilterEffect().foregroundBlurPara_->radiusY = radiusY;
}

float RSEffectProperties::GetForegroundBlurRadiusY() const
{
    if (filterEffect_ && filterEffect_->foregroundBlurPara_) {
        return filterEffect_->foregroundBlurPara_->radiusY;
    }
    return 0.f;
}

bool RSEffectProperties::IsForegroundBlurRadiusYValid() const
{
    return ROSEN_GNE(GetForegroundBlurRadiusY(), 0.999f);
}

void RSEffectProperties::SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    GetFilterEffect().fgBlurDisableSystemAdaptation = disableSystemAdaptation;
}

bool RSEffectProperties::GetFgBlurDisableSystemAdaptation() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->fgBlurDisableSystemAdaptation;
    }
    return true;
}

void RSEffectProperties::SetAlwaysSnapshot(bool enable)
{
    GetFilterEffect().alwaysSnapshot_ = enable;
}

bool RSEffectProperties::GetAlwaysSnapshot() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->alwaysSnapshot_;
    }
    return false;
}

void RSEffectProperties::SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    GetFilterEffect().bgNGRenderFilter_ = filter;
}

std::shared_ptr<RSNGRenderFilterBase> RSEffectProperties::GetBackgroundNGFilter() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->bgNGRenderFilter_;
    }
    return nullptr;
}

void RSEffectProperties::SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    GetFilterEffect().fgNGRenderFilter_ = filter;
}

std::shared_ptr<RSNGRenderFilterBase> RSEffectProperties::GetForegroundNGFilter() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->fgNGRenderFilter_;
    }
    return nullptr;
}

void RSEffectProperties::SetMaterialNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    GetFilterEffect().mtNGRenderFilter_ = filter;
}

std::shared_ptr<RSNGRenderFilterBase> RSEffectProperties::GetMaterialNGFilter() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->mtNGRenderFilter_;
    }
    return nullptr;
}

void RSEffectProperties::SetCompositingNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter)
{
    GetFilterEffect().cgNGRenderFilter_ = filter;
}

std::shared_ptr<RSNGRenderFilterBase> RSEffectProperties::GetCompositingNGFilter() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->cgNGRenderFilter_;
    }
    return nullptr;
}

const std::shared_ptr<RSFilter>& RSEffectProperties::GetMaterialFilter() const
{
    static const std::shared_ptr<RSFilter> defaultValue = nullptr;
    if (filterEffect_ != nullptr) {
        return filterEffect_->materialFilter_;
    }
    return defaultValue;
}

void RSEffectProperties::SetMaterialFilter(const std::shared_ptr<RSFilter>& filter)
{
    GetFilterEffect().materialFilter_ = filter;
}

void RSEffectProperties::GenerateMaterialFilter()
{
    // not support compose yet, so do not use ComposeNGRenderFilter
    if (!GetMaterialNGFilter()) {
        WITH_FILTER_EFFECT(materialFilter_ = nullptr);
        return;
    }
    auto filter = std::make_shared<RSDrawingFilter>();
    filter->SetNGRenderFilter(GetMaterialNGFilter());
    filter->SetFilterType(RSFilter::COMPOUND_EFFECT);
    if (GetMaterialNGFilter()->GetType() == RSNGEffectType::FROSTED_GLASS) {
        std::string dumpStr = "";
        RS_OPTIONAL_TRACE_NAME_FMT("FROSTED_GLASS skip = %d dump: %s",
            static_cast<int>(GetMaterialNGFilter()->CanSkipFrame()),
            (GetMaterialNGFilter()->Dump(dumpStr), dumpStr.c_str()));
        filter->SetSkipFrame(GetMaterialNGFilter()->CanSkipFrame());
    }
    GetFilterEffect().materialFilter_ = filter;
}

void RSEffectProperties::SetPixelStretch(const std::optional<Vector4f>& stretchSize)
{
    if (stretchSize.has_value()) {
        if (!GetFilterEffect().pixelStretchPara_) {
            GetFilterEffect().pixelStretchPara_ = std::make_unique<RSPixelStretchPara>();
        }
        GetFilterEffect().pixelStretchPara_->size = stretchSize.value();
    } else if (GetFilterEffect().pixelStretchPara_) {
        GetFilterEffect().pixelStretchPara_->size = Vector4f();
    }
}

void RSEffectProperties::SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent)
{
    if (stretchPercent.has_value()) {
        if (!GetFilterEffect().pixelStretchPara_) {
            GetFilterEffect().pixelStretchPara_ = std::make_unique<RSPixelStretchPara>();
        }
        GetFilterEffect().pixelStretchPara_->percent = stretchPercent.value();
    } else if (GetFilterEffect().pixelStretchPara_) {
        GetFilterEffect().pixelStretchPara_->percent = Vector4f();
    }
}

void RSEffectProperties::SetPixelStretchTileMode(int tileMode)
{
    if (!GetFilterEffect().pixelStretchPara_) {
        GetFilterEffect().pixelStretchPara_ = std::make_unique<RSPixelStretchPara>();
    }
    GetFilterEffect().pixelStretchPara_->tileMode = std::clamp<int>(tileMode,
        static_cast<int>(Drawing::TileMode::CLAMP), static_cast<int>(Drawing::TileMode::DECAL));
}

int RSEffectProperties::GetPixelStretchTileMode() const
{
    if (filterEffect_ && filterEffect_->pixelStretchPara_) {
        return filterEffect_->pixelStretchPara_->tileMode;
    }
    return 0;
}

Vector4f RSEffectProperties::GetPixelStretch() const
{
    return filterEffect_ && filterEffect_->pixelStretchPara_ ? filterEffect_->pixelStretchPara_->size : Vector4f();
}

Vector4f RSEffectProperties::GetPixelStretchPercent() const
{
    return filterEffect_ && filterEffect_->pixelStretchPara_ ? filterEffect_->pixelStretchPara_->percent : Vector4f();
}

void RSEffectProperties::ConvertPixelStretchPercentToSize(float width, float height)
{
    if (!GetFilterEffect().pixelStretchPara_) {
        GetFilterEffect().pixelStretchPara_ = std::make_unique<RSPixelStretchPara>();
    }
    GetFilterEffect().pixelStretchPara_->size = GetPixelStretchPercent() * Vector4f(width, height, width, height);
}

void RSEffectProperties::ResetPixelStretchPara()
{
    if (filterEffect_ != nullptr) {
        filterEffect_->pixelStretchPara_.reset();
    }
}

void RSEffectProperties::SetDistortionK(const std::optional<float>& distortionK)
{
    if (distortionK.has_value()) {
        if (!GetFilterEffect().distortionPara_) {
            GetFilterEffect().distortionPara_ = std::make_unique<RSDistortionPara>();
        }
        GetFilterEffect().distortionPara_->distortionK = distortionK.value();
        GetFilterEffect().distortionPara_->dirty = ROSEN_GNE(*distortionK, 0.0f) && ROSEN_LE(*distortionK, 1.0f);
    } else if (GetFilterEffect().distortionPara_) {
        GetFilterEffect().distortionPara_->distortionK = 0;
        GetFilterEffect().distortionPara_->dirty = false;
    }
}

std::optional<float> RSEffectProperties::GetDistortionK() const
{
    return filterEffect_ && filterEffect_->distortionPara_ ?
        std::optional<float>(filterEffect_->distortionPara_->distortionK) : std::nullopt;
}

void RSEffectProperties::SetDistortionDirty(bool distortionEffectDirty)
{
    GetFilterEffect().distortionPara_->dirty = distortionEffectDirty;
}

bool RSEffectProperties::GetDistortionDirty() const
{
    if (filterEffect_ && filterEffect_->distortionPara_) {
        return filterEffect_->distortionPara_->dirty;
    }
    return false;
}

bool RSEffectProperties::IsDistortionKValid() const
{
    const auto& distortionK = GetDistortionK();
    return distortionK.has_value() && ROSEN_GE(*distortionK, -1.0f) && ROSEN_LE(*distortionK, 1.0f);
}

bool RSEffectProperties::IsBackgroundMaterialFilterValid() const
{
    return IsBackgroundBlurRadiusValid() || IsBackgroundBlurBrightnessValid() || IsBackgroundBlurSaturationValid();
}

bool RSEffectProperties::IsForegroundMaterialFilterValid() const
{
    return IsForegroundBlurRadiusValid();
}

bool RSEffectProperties::SetAttractionFraction(float fraction)
{
    auto& filter = GetFilterEffect();
    filter.attractFraction_ = fraction;
    filter.isAttractionValid_ = fraction > ATTRACTION_VALID_EPSILON;
    return filter.isAttractionValid_;
}

float RSEffectProperties::GetAttractionFraction() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->attractFraction_;
    }
    return 0.f;
}

void RSEffectProperties::SetAttractionDstPoint(Vector2f dstPoint)
{
    GetFilterEffect().attractDstPoint_ = dstPoint;
}

Vector2f RSEffectProperties::GetAttractionDstPoint() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->attractDstPoint_;
    }
    return {0.f, 0.f};
}

bool RSEffectProperties::IsAttractionValid() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->isAttractionValid_;
    }
    return false;
}

RectI RSEffectProperties::GetAttractionEffectCurrentDirtyRegion() const
{
    if (filterEffect_ != nullptr) {
        return filterEffect_->attractionEffectCurrentDirtyRegion_;
    }
    return {0, 0, 0, 0};
}

void RSEffectProperties::SetAttractionEffectCurrentDirtyRegion(const RectI& dirtyRegion)
{
    GetFilterEffect().attractionEffectCurrentDirtyRegion_ = dirtyRegion;
}

void RSEffectProperties::SetShadowColor(Color color)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetColor(color);
}

void RSEffectProperties::SetShadowOffsetX(float offsetX)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetOffsetX(offsetX);
}

void RSEffectProperties::SetShadowOffsetY(float offsetY)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetOffsetY(offsetY);
}

void RSEffectProperties::SetShadowElevation(float elevation)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetElevation(elevation);
}

void RSEffectProperties::SetShadowRadius(float radius)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetRadius(radius);
}

void RSEffectProperties::SetShadowPath(std::shared_ptr<RSPath> shadowPath)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetPath(shadowPath);
}

void RSEffectProperties::SetShadowMask(int shadowMask)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetMask(shadowMask);
}

void RSEffectProperties::SetShadowIsFilled(bool shadowIsFilled)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetIsFilled(shadowIsFilled);
}

void RSEffectProperties::SetShadowColorStrategy(int shadowColorStrategy)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetColorStrategy(shadowColorStrategy);
}

void RSEffectProperties::SetShadowDisableSDFBlur(bool disable)
{
    if (!GetShadow().has_value()) {
        GetFilterEffect().shadow_ = std::make_optional<RSShadow>();
    }
    GetFilterEffect().shadow_->SetDisableSDFBlur(disable);
}

const std::optional<RSShadow>& RSEffectProperties::GetShadow() const
{
    static const std::optional<RSShadow> defaultValue = std::nullopt;
    if (filterEffect_ != nullptr) {
        return filterEffect_->shadow_;
    }
    return defaultValue;
}

bool RSEffectProperties::IsShadowMaskValid() const
{
    const auto& shadow = GetShadow();
    if (!shadow.has_value()) {
        return false;
    }
    return (shadow->GetMask() > SHADOW_MASK_STRATEGY::MASK_NONE) && (
        shadow->GetMask() <= SHADOW_MASK_STRATEGY::MASK_COLOR_BLUR);
}

bool RSEffectProperties::IsShadowValid() const
{
    return GetShadow() && GetShadow()->IsValid();
}

// shader effect params
void RSEffectProperties::SetComplexShaderParam(const std::vector<float>& param)
{
    GetShaderEffect().complexShaderParam_ = param;
}

std::optional<std::vector<float>> RSEffectProperties::GetComplexShaderParam() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->complexShaderParam_;
    }
    return std::nullopt;
}

void RSEffectProperties::SetBackgroundNGShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader)
{
    GetShaderEffect().bgNGRenderShader_ = renderShader;
}

std::shared_ptr<RSNGRenderShaderBase> RSEffectProperties::GetBackgroundNGShader() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->bgNGRenderShader_;
    }
    return nullptr;
}

void RSEffectProperties::SetForegroundShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader)
{
    GetShaderEffect().fgRenderShader_ = renderShader;
}

std::shared_ptr<RSNGRenderShaderBase> RSEffectProperties::GetForegroundShader() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->fgRenderShader_;
    }
    return nullptr;
}

void RSEffectProperties::SetMaterialShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader)
{
    GetShaderEffect().mtRenderShader_ = renderShader;
}

std::shared_ptr<RSNGRenderShaderBase> RSEffectProperties::GetMaterialShader() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->mtRenderShader_;
    }
    return nullptr;
}

void RSEffectProperties::SetCoverageNGShader(const std::shared_ptr<RSNGRenderShaderBase>& coverageShader)
{
    GetShaderEffect().coRenderShader_ = coverageShader;
}

std::shared_ptr<RSNGRenderShaderBase> RSEffectProperties::GetCoverageNGShader() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->coRenderShader_;
    }
    return nullptr;
}

void RSEffectProperties::SetOverlayNGShader(const std::shared_ptr<RSNGRenderShaderBase>& overlayShader)
{
    GetShaderEffect().olRenderShader_ = overlayShader;
}

std::shared_ptr<RSNGRenderShaderBase> RSEffectProperties::GetOverlayNGShader() const
{
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->olRenderShader_;
    }
    return nullptr;
}

const std::shared_ptr<RSLightSource>& RSEffectProperties::GetLightSource() const
{
    static const std::shared_ptr<RSLightSource> defaultValue = nullptr;
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->lightSourcePtr_;
    }
    return defaultValue;
}

const std::shared_ptr<RSIlluminated>& RSEffectProperties::GetIlluminated() const
{
    static const std::shared_ptr<RSIlluminated> defaultValue = nullptr;
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->illuminatedPtr_;
    }
    return defaultValue;
}

bool RSEffectProperties::SetLightIntensity(float lightIntensity)
{
    if (!GetShaderEffect().lightSourcePtr_) {
        GetShaderEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    if (ROSEN_EQ(lightIntensity, INVALID_INTENSITY)) { // skip when resetFunc call
        return false;
    }
    GetShaderEffect().lightSourcePtr_->SetLightIntensity(lightIntensity);
    return true;
}

void RSEffectProperties::SetLightColor(Color lightColor)
{
    if (!GetShaderEffect().lightSourcePtr_) {
        GetShaderEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    GetShaderEffect().lightSourcePtr_->SetLightColor(lightColor);
}

void RSEffectProperties::SetLightPosition(const Vector4f& lightPosition)
{
    if (!GetShaderEffect().lightSourcePtr_) {
        GetShaderEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    GetShaderEffect().lightSourcePtr_->SetLightPosition(lightPosition);
}

void RSEffectProperties::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    if (!GetShaderEffect().illuminatedPtr_) {
        GetShaderEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    GetShaderEffect().illuminatedPtr_->SetIlluminatedBorderWidth(illuminatedBorderWidth);
}

void RSEffectProperties::SetIlluminatedType(int illuminatedType)
{
    if (!GetShaderEffect().illuminatedPtr_) {
        GetShaderEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    auto curIlluminateType = IlluminatedType(illuminatedType);
    if (curIlluminateType == IlluminatedType::INVALID) { // skip when resetFunc call
        return;
    }
    GetShaderEffect().illuminatedPtr_->SetIlluminatedType(curIlluminateType);
}

void RSEffectProperties::SetBloom(float bloomIntensity)
{
    if (!GetShaderEffect().illuminatedPtr_) {
        GetShaderEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    GetShaderEffect().illuminatedPtr_->SetBloomIntensity(bloomIntensity);
}

void RSEffectProperties::SetParticles(const RSRenderParticleVector& particles)
{
    GetShaderEffect().particles_ = particles;
}

const RSRenderParticleVector& RSEffectProperties::GetParticles() const
{
    static const RSRenderParticleVector defaultValue = RSRenderParticleVector();
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->particles_;
    }
    return defaultValue;
}

void RSEffectProperties::SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para)
{
    GetShaderEffect().emitterUpdater_ = para;
}

const std::vector<std::shared_ptr<EmitterUpdater>>& RSEffectProperties::GetEmitterUpdater() const
{
    static const std::vector<std::shared_ptr<EmitterUpdater>> defaultValue = {};
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->emitterUpdater_;
    }
    return defaultValue;
}

void RSEffectProperties::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
    GetShaderEffect().particleNoiseFields_ = para;
}

const std::shared_ptr<ParticleNoiseFields>& RSEffectProperties::GetParticleNoiseFields() const
{
    static const std::shared_ptr<ParticleNoiseFields> defaultValue = nullptr;
    if (shaderEffect_ != nullptr) {
        return shaderEffect_->particleNoiseFields_;
    }
    return defaultValue;
}

// blender effect params
void RSEffectProperties::SetFgBrightnessRates(const Vector4f& rates)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->rates_ = rates;
}

void RSEffectProperties::SetFgBrightnessSaturation(const float& saturation)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->saturation_ = saturation;
}

void RSEffectProperties::SetFgBrightnessPosCoeff(const Vector4f& coeff)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->posCoeff_ = coeff;
}

void RSEffectProperties::SetFgBrightnessNegCoeff(const Vector4f& coeff)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->negCoeff_ = coeff;
}

void RSEffectProperties::SetFgBrightnessFract(const float& fraction)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->fraction_ = fraction;
}

void RSEffectProperties::SetFgBrightnessHdr(bool enableHdr)
{
    if (!GetFgBrightnessParams().has_value()) {
        GetBlenderEffect().fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    }
    GetBlenderEffect().fgBrightnessParams_->enableHdr_ = enableHdr;
}

void RSEffectProperties::SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params)
{
    GetBlenderEffect().fgBrightnessParams_ = params;
}

std::optional<RSDynamicBrightnessPara> RSEffectProperties::GetFgBrightnessParams() const
{
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->fgBrightnessParams_;
    }
    return std::nullopt;
}

bool RSEffectProperties::IsFgBrightnessValid() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams.has_value() && fgBrightnessParams->IsValid();
}

bool RSEffectProperties::GetFgBrightnessEnableEDR() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    return fgBrightnessParams.has_value() && fgBrightnessParams->enableHdr_ && IsFgBrightnessValid();
}

std::string RSEffectProperties::GetFgBrightnessDescription() const
{
    const auto& fgBrightnessParams = GetFgBrightnessParams();
    if (!fgBrightnessParams.has_value()) {
        return "fgBrightnessParams_ is nullopt";
    }
    std::string description =
        "ForegroundBrightness, cubicCoeff: " + std::to_string(fgBrightnessParams->rates_.x_) +
        ", quadCoeff: " + std::to_string(fgBrightnessParams->rates_.y_) +
        ", rate: " + std::to_string(fgBrightnessParams->rates_.z_) +
        ", lightUpDegree: " + std::to_string(fgBrightnessParams->rates_.w_) +
        ", saturation: " + std::to_string(fgBrightnessParams->saturation_) +
        ", fgBrightnessFract: " + std::to_string(fgBrightnessParams->fraction_) +
        ", fgBrightnessHdr: " + std::to_string(fgBrightnessParams->enableHdr_);
    return description;
}

void RSEffectProperties::SetShadowBlenderParams(const std::optional<RSShadowBlenderPara>& params)
{
    GetBlenderEffect().shadowBlenderParams_ = params;
}

std::optional<RSShadowBlenderPara> RSEffectProperties::GetShadowBlenderParams() const
{
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->shadowBlenderParams_;
    }
    return std::nullopt;
}

bool RSEffectProperties::IsShadowBlenderValid() const
{
    const auto& shadowBlenderParams = GetShadowBlenderParams();
    return shadowBlenderParams.has_value() && shadowBlenderParams->IsValid();
}

std::string RSEffectProperties::GetShadowBlenderDescription() const
{
    const auto& shadowBlenderParams = GetShadowBlenderParams();
    if (!shadowBlenderParams.has_value()) {
        return "shadowBlenderParams_ is nullopt";
    }
    std::string description =
        "ShadowBlender, cubic: " + std::to_string(shadowBlenderParams->cubic_) +
        ", quadratic: " + std::to_string(shadowBlenderParams->quadratic_) +
        ", linear: " + std::to_string(shadowBlenderParams->linear_) +
        ", constant: " + std::to_string(shadowBlenderParams->constant_);
    return description;
}

void RSEffectProperties::SetHdrDarkenBlenderParams(const std::optional<RSHdrDarkenBlenderPara>& params)
{
    GetBlenderEffect().hdrDarkenBlenderParams_ = params;
}

std::optional<RSHdrDarkenBlenderPara> RSEffectProperties::GetHdrDarkenBlenderParams() const
{
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->hdrDarkenBlenderParams_;
    }
    return std::nullopt;
}

bool RSEffectProperties::IsHdrDarkenBlenderValid() const
{
    const auto& hdrDarkenBlenderParams = GetHdrDarkenBlenderParams();
    return hdrDarkenBlenderParams.has_value();
}

std::string RSEffectProperties::GetHdrDarkenBlenderDescription() const
{
    const auto& hdrDarkenBlenderParams = GetHdrDarkenBlenderParams();
    if (!hdrDarkenBlenderParams.has_value()) {
        return "hdrDarkenBlenderParams is nullopt";
    }
    std::string description =
        "HdrDarkenBlender, hdrBrightnessRatio: " + std::to_string(hdrDarkenBlenderParams->hdrBrightnessRatio_) +
        ", grayscaleFactor.r: " + std::to_string(hdrDarkenBlenderParams->grayscaleFactor_.x_) +
        ", grayscaleFactor.g: " + std::to_string(hdrDarkenBlenderParams->grayscaleFactor_.y_) +
        ", grayscaleFactor.b: " + std::to_string(hdrDarkenBlenderParams->grayscaleFactor_.z_);
    return description;
}

void RSEffectProperties::SetColorfulBrightnessBlenderParams(std::unique_ptr<RSColorfulBrightnessBlenderPara> params)
{
    GetBlenderEffect().colorfulBrightnessBlenderParams_ = std::move(params);
}

const RSColorfulBrightnessBlenderPara* RSEffectProperties::GetColorfulBrightnessBlenderParams() const
{
    if (blenderEffect_ && blenderEffect_->colorfulBrightnessBlenderParams_) {
        return blenderEffect_->colorfulBrightnessBlenderParams_.get();
    }
    return nullptr;
}

bool RSEffectProperties::IsColorfulBrightnessBlenderValid() const
{
    return GetColorfulBrightnessBlenderParams() != nullptr;
}

std::string RSEffectProperties::GetColorfulBrightnessBlenderDescription() const
{
    const auto* params = GetColorfulBrightnessBlenderParams();
    if (params == nullptr) {
        return "colorfulBrightnessBlenderParams is nullopt";
    }
    return "ColorfulBrightnessBlender, darkenWeight: " + std::to_string(params->darkenWeight_) +
           ", fraction: " + std::to_string(params->fraction_) +
           ", cubicRate: " + std::to_string(params->cubicRate_) +
           ", quadRate: " + std::to_string(params->quadRate_) +
           ", linearRate: " + std::to_string(params->linearRate_) +
           ", degree: " + std::to_string(params->degree_) +
           ", saturation: " + std::to_string(params->saturation_) +
           ", positiveCoeff.x: " + std::to_string(params->positiveCoeff_.x_) +
           ", positiveCoeff.y: " + std::to_string(params->positiveCoeff_.y_) +
           ", positiveCoeff.z: " + std::to_string(params->positiveCoeff_.z_) +
           ", negativeCoeff.x: " + std::to_string(params->negativeCoeff_.x_) +
           ", negativeCoeff.y: " + std::to_string(params->negativeCoeff_.y_) +
           ", negativeCoeff.z: " + std::to_string(params->negativeCoeff_.z_) +
           ", vibrancyStrength: " + std::to_string(params->vibrancyStrength_) +
           ", lumaDiff: " + std::to_string(params->lumaDiff_) +
           ", hdrEnabled: " + std::to_string(params->hdrEnabled_) +
           ", tintedColorPercent: " + std::to_string(params->tintedColorPercent_);
}

void RSEffectProperties::SetColorBlendMode(int colorBlendMode)
{
    GetBlenderEffect().colorBlendMode_ = std::clamp<int>(colorBlendMode, 0, static_cast<int>(RSColorBlendMode::MAX));
}

int RSEffectProperties::GetColorBlendMode() const
{
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->colorBlendMode_;
    }
    return 0;
}

void RSEffectProperties::SetColorBlendApplyType(int colorBlendApplyType)
{
    GetBlenderEffect().colorBlendApplyType_ =
        std::clamp<int>(colorBlendApplyType, 0, static_cast<int>(RSColorBlendApplyType::MAX));
}

int RSEffectProperties::GetColorBlendApplyType() const
{
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->colorBlendApplyType_;
    }
    return 0;
}

void RSEffectProperties::SetColorBlend(const std::optional<Color>& colorBlend)
{
    GetBlenderEffect().colorBlend_ = colorBlend;
}

const std::optional<Color>& RSEffectProperties::GetColorBlend() const
{
    static const std::optional<Color> defaultValue = std::nullopt;
    if (blenderEffect_ != nullptr) {
        return blenderEffect_->colorBlend_;
    }
    return defaultValue;
}

void RSEffectProperties::SetDynamicLightUpRate(const std::optional<float>& rate)
{
    if (rate.has_value()) {
        if (!GetBlenderEffect().dynamicLightUpPara_) {
            GetBlenderEffect().dynamicLightUpPara_ = std::make_unique<RSDynamicLightUpPara>();
        }
        GetBlenderEffect().dynamicLightUpPara_->rate = rate.value();
    } else if (GetBlenderEffect().dynamicLightUpPara_) {
        GetBlenderEffect().dynamicLightUpPara_->rate = 0;
    }
}

void RSEffectProperties::SetDynamicLightUpDegree(const std::optional<float>& degree)
{
    if (degree.has_value()) {
        if (!GetBlenderEffect().dynamicLightUpPara_) {
            GetBlenderEffect().dynamicLightUpPara_ = std::make_unique<RSDynamicLightUpPara>();
        }
        GetBlenderEffect().dynamicLightUpPara_->degree = degree.value();
    } else if (GetBlenderEffect().dynamicLightUpPara_) {
        GetBlenderEffect().dynamicLightUpPara_->degree = 0;
    }
}

float RSEffectProperties::GetDynamicLightUpRate() const
{
    return blenderEffect_ && blenderEffect_->dynamicLightUpPara_ ? blenderEffect_->dynamicLightUpPara_->rate : 0.f;
}

float RSEffectProperties::GetDynamicLightUpDegree() const
{
    return blenderEffect_ && blenderEffect_->dynamicLightUpPara_ ? blenderEffect_->dynamicLightUpPara_->degree : 0.f;
}

bool RSEffectProperties::IsDynamicLightUpValid() const
{
    return blenderEffect_ && blenderEffect_->dynamicLightUpPara_ &&
           ROSEN_GNE(blenderEffect_->dynamicLightUpPara_->rate, 0.0) &&
           ROSEN_GE(blenderEffect_->dynamicLightUpPara_->degree, -1.0) &&
           ROSEN_LE(blenderEffect_->dynamicLightUpPara_->degree, 1.0);
}

// nodes effect params
void RSEffectProperties::SetUseEffect(bool useEffect)
{
    GetNodesEffect().useEffect_ = useEffect;
}

bool RSEffectProperties::GetUseEffect() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->useEffect_;
    }
    return false;
}

void RSEffectProperties::SetUseEffectType(int useEffectType)
{
    GetNodesEffect().useEffectType_ = std::clamp<int>(useEffectType, 0, static_cast<int>(UseEffectType::MAX));
}

int RSEffectProperties::GetUseEffectType() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->useEffectType_;
    }
    return 0;
}

void RSEffectProperties::SetNeedDrawBehindWindow(bool needDrawBehindWindow)
{
    GetNodesEffect().needDrawBehindWindow_ = needDrawBehindWindow;
}

bool RSEffectProperties::GetNeedDrawBehindWindow() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->needDrawBehindWindow_;
    }
    return false;
}

void RSEffectProperties::SetHaveEffectRegion(bool haveEffectRegion)
{
    GetNodesEffect().haveEffectRegion_ = haveEffectRegion;
}

bool RSEffectProperties::GetHaveEffectRegion() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->haveEffectRegion_;
    }
    return false;
}

void RSEffectProperties::SetUseShadowBatching(bool useShadowBatching)
{
    GetNodesEffect().useShadowBatching_ = useShadowBatching;
}

bool RSEffectProperties::GetUseShadowBatching() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->useShadowBatching_;
    }
    return false;
}

void RSEffectProperties::SetUseUnion(bool useUnion)
{
    GetNodesEffect().useUnion_ = useUnion;
}

bool RSEffectProperties::GetUseUnion() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->useUnion_;
    }
    return false;
}

void RSEffectProperties::SetUnionSpacing(float spacing)
{
    GetNodesEffect().unionSpacing_ = spacing;
}

float RSEffectProperties::GetUnionSpacing() const
{
    if (nodesEffect_ != nullptr) {
        return nodesEffect_->unionSpacing_;
    }
    return 0.f;
}

// depth and spatial effect params
void RSEffectProperties::SetDepthImage(const std::shared_ptr<RSImage>& depthImage)
{
    GetDepthEffect().depthImage_ = depthImage;
}

std::shared_ptr<RSImage> RSEffectProperties::GetDepthImage() const
{
    return depthEffect_ ? depthEffect_->depthImage_ : nullptr;
}

void RSEffectProperties::SetDepthCameraPara(const DepthCameraPara& depthCameraPara)
{
    GetDepthEffect().depthCameraPara_ = depthCameraPara;
}

std::optional<DepthCameraPara> RSEffectProperties::GetDepthCameraPara() const
{
    return depthEffect_ ? depthEffect_->depthCameraPara_ : std::nullopt;
}

void RSEffectProperties::SetDepthLightPara(const DepthLightPara& depthLightPara)
{
    GetDepthEffect().depthLightPara_ = depthLightPara;
}

std::optional<DepthLightPara> RSEffectProperties::GetDepthLightPara() const
{
    return depthEffect_ ? depthEffect_->depthLightPara_ : std::nullopt;
}

void RSEffectProperties::SetDepthImageMatrix(const Matrix3f& imageMatrix)
{
    GetDepthEffect().depthImageMatrix_ = imageMatrix;
}

std::optional<Matrix3f> RSEffectProperties::GetDepthImageMatrix() const
{
    return depthEffect_ ? depthEffect_->depthImageMatrix_ : std::nullopt;
}

void RSEffectProperties::SetSpatialEffectDepth(float depth)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value() || spatialEffectVariantPara->PerspectiveEnabled()) {
        spatialEffectVariantPara = DepthEffectPara();
    }

    std::get<float>(spatialEffectVariantPara->position) = depth;
}

void RSEffectProperties::SetSpatialEffectLeftTop(const Vector3f& leftTop)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value() || !spatialEffectVariantPara->PerspectiveEnabled()) {
        spatialEffectVariantPara = SpatialEffectPara();
    }

    std::get<SpatialEffectPara::CornerPositions>(spatialEffectVariantPara->position)
        [SpatialEffectPara::LEFT_TOP_INDEX] = leftTop;
}

void RSEffectProperties::SetSpatialEffectRightTop(const Vector3f& rightTop)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value() || !spatialEffectVariantPara->PerspectiveEnabled()) {
        spatialEffectVariantPara = SpatialEffectPara();
    }

    std::get<SpatialEffectPara::CornerPositions>(spatialEffectVariantPara->position)
        [SpatialEffectPara::RIGHT_TOP_INDEX] = rightTop;
}

void RSEffectProperties::SetSpatialEffectLeftBottom(const Vector3f& leftBottom)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value() || !spatialEffectVariantPara->PerspectiveEnabled()) {
        spatialEffectVariantPara = SpatialEffectPara();
    }

    std::get<SpatialEffectPara::CornerPositions>(spatialEffectVariantPara->position)
        [SpatialEffectPara::LEFT_BOTTOM_INDEX] = leftBottom;
}

void RSEffectProperties::SetSpatialEffectRightBottom(const Vector3f& rightBottom)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value() || !spatialEffectVariantPara->PerspectiveEnabled()) {
        spatialEffectVariantPara = SpatialEffectPara();
    }

    std::get<SpatialEffectPara::CornerPositions>(spatialEffectVariantPara->position)
        [SpatialEffectPara::RIGHT_BOTTOM_INDEX] = rightBottom;
}

void RSEffectProperties::SetSpatialEffectOcclusionWeight(float occlusionWeight)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value()) {
        spatialEffectVariantPara = SpatialEffectVariantPara();
    }

    spatialEffectVariantPara->occlusionWeight = occlusionWeight;
}

void RSEffectProperties::SetDepthEffectPara(const std::optional<DepthEffectPara>& depthEffectPara)
{
    GetDepthEffect().spatialEffectVariantPara_ = depthEffectPara;
}

std::optional<DepthEffectPara> RSEffectProperties::GetDepthEffectPara() const
{
    if (!depthEffect_) {
        return std::nullopt;
    }
    const auto& spatialEffectVariantPara = depthEffect_->spatialEffectVariantPara_;
    if (!spatialEffectVariantPara) {
        return std::nullopt;
    }
    if (spatialEffectVariantPara->PerspectiveEnabled()) {
        return std::nullopt;
    }

    DepthEffectPara ret;
    ret.depth = std::get<float>(spatialEffectVariantPara->position);
    ret.occlusionWeight = spatialEffectVariantPara->occlusionWeight;
    return ret;
}

void RSEffectProperties::SetSpatialEffectPara(const std::optional<SpatialEffectPara>& spatialEffectPara)
{
    GetDepthEffect().spatialEffectVariantPara_ = spatialEffectPara;
}

std::optional<SpatialEffectPara> RSEffectProperties::GetSpatialEffectPara() const
{
    if (!depthEffect_) {
        return std::nullopt;
    }
    const auto& spatialEffectVariantPara = depthEffect_->spatialEffectVariantPara_;
    if (!spatialEffectVariantPara) {
        return std::nullopt;
    }
    if (!spatialEffectVariantPara->PerspectiveEnabled()) {
        return std::nullopt;
    }

    SpatialEffectPara ret;
    ret.spatialEffectMode = spatialEffectVariantPara->spatialEffectMode;
    ret.corners = std::get<SpatialEffectPara::CornerPositions>(spatialEffectVariantPara->position);
    ret.xyzCorners = spatialEffectVariantPara->xyzCornerPoints;
    ret.occlusionWeight = spatialEffectVariantPara->occlusionWeight;
    return ret;
}

void RSEffectProperties::SetSpatialEffectMode(int mode)
{
    auto& spatialEffectVariantPara = GetDepthEffect().spatialEffectVariantPara_;
    if (!spatialEffectVariantPara.has_value()) {
        spatialEffectVariantPara = SpatialEffectVariantPara();
    }
    spatialEffectVariantPara->spatialEffectMode = static_cast<SpatialEffectMode>(mode);
}

void RSEffectProperties::SetSpatialEffectVariantPara(
    const std::optional<SpatialEffectVariantPara>& spatialEffectVariantPara)
{
    GetDepthEffect().spatialEffectVariantPara_ = spatialEffectVariantPara;
}

std::optional<SpatialEffectVariantPara> RSEffectProperties::GetSpatialEffectVariantPara() const
{
    return depthEffect_ ? depthEffect_->spatialEffectVariantPara_ : std::nullopt;
}

bool RSEffectProperties::GetSpatialEffectOcclusionEnabled() const
{
    if (!depthEffect_) {
        return false;
    }
    if (!depthEffect_->spatialEffectVariantPara_) {
        return false;
    }
    return ROSEN_GNE(depthEffect_->spatialEffectVariantPara_->occlusionWeight, 0.0f);
}

void RSEffectProperties::SetSpatialEffectDstPoints(const std::optional<std::vector<Drawing::Point>>& dstPoints)
{
    GetDepthEffect().spatialEffectDstPoints_ = dstPoints;
}

std::optional<std::vector<Drawing::Point>> RSEffectProperties::GetSpatialEffectDstPoints() const
{
    return depthEffect_ ? depthEffect_->spatialEffectDstPoints_ : std::nullopt;
}

void RSEffectProperties::SetSpatialEffectXyzCornerPoints(
    const SpatialEffectPara::CornerPositions& xyzCornerPoints)
{
    if (GetDepthEffect().spatialEffectVariantPara_.has_value()) {
        GetDepthEffect().spatialEffectVariantPara_->xyzCornerPoints = xyzCornerPoints;
    }
}
} // namespace Rosen
} // namespace OHOS
