/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "render/rs_material_filter.h"

#include <unordered_map>

#if defined(NEW_SKIA)
#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif

#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float BLUR_SIGMA_SCALE = 0.57735f;
// style to MaterialParam map
std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> materialParams_ {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 23.0f,  1.05, 1.05, RSColor(0xFFFFFF33) } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  1.2,  RSColor(0xFAFAFA99) } },
    { STYLE_CARD_THICK_LIGHT,        { 57.0f,  1.2,  1.1,  RSColor(0xFFFFFF8C) } },
    { STYLE_CARD_THIN_DARK,          { 75.0f,  1.35, 1.0,  RSColor(0x1A1A1A6B) } },
    { STYLE_CARD_DARK,               { 50.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    { STYLE_CARD_THICK_DARK,         { 75.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    // background blur params
    { STYLE_BACKGROUND_SMALL_LIGHT,  { 23.0f,  1.05, 1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_MEDIUM_LIGHT, { 29.0f,  1.1,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_LARGE_LIGHT,  { 57.0f,  1.2,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_XLARGE_LIGHT, { 120.0f, 1.3,  1.0,  RSColor(0x6666664C) } },
    { STYLE_BACKGROUND_SMALL_DARK,   { 15.0f,  1.1,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_MEDIUM_DARK,  { 55.0f,  1.15, 1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_LARGE_DARK,   { 75.0f,  1.5,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_XLARGE_DARK,  { 130.0f, 1.3,  1.0,  RSColor(0x0D0D0D80) } },
};
} // namespace

RSMaterialFilter::RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(nullptr), colorMode_(mode)
#else
    : RSDrawingFilter(nullptr), colorMode_(mode)
#endif
{
    imageFilter_ = RSMaterialFilter::CreateMaterialStyle(static_cast<MATERIAL_BLUR_STYLE>(style), dipScale, ratio);
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode)
    : RSSkiaFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
      brightness_(materialParam.brightness), maskColor_(materialParam.maskColor)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialFilter(
        materialParam.radius, materialParam.saturation, materialParam.brightness);
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::~RSMaterialFilter() = default;

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale)
{
    float radiusPx = radiusVp * dipScale;
#ifndef USE_ROSEN_DRAWING
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
#else
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + 0.5f : 0.0f;
#endif
}

std::string RSMaterialFilter::GetDescription()
{
    return "RSMaterialFilter blur radius is " + std::to_string(radius_) + " sigma";
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSSkiaFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> material = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
    material->imageFilter_ = SkImageFilters::Compose(imageFilter_, inner->GetImageFilter());
    return material;
}
#else
std::shared_ptr<RSDrawingFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSDrawingFilter>& inner)
{
    if (inner == nullptr) {
        return nullptr;
    }
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> material = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
    material->imageFilter_ = Drawing::ColorFilter::CreateComposeColorFilter(imageFilter_, inner->GetImageFilter());
    return material;
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, float brightness)
{
#if defined(NEW_SKIA)
    sk_sp<SkImageFilter> blurFilter = SkImageFilters::Blur(radius, radius, SkTileMode::kClamp, nullptr); // blur
#else
    sk_sp<SkImageFilter> blurFilter = SkBlurImageFilter::Make(radius, radius, nullptr, nullptr,
        SkBlurImageFilter::kClamp_TileMode); // blur
#endif
    float normalizedDegree = brightness - 1.0;
    const float brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    
    sk_sp<SkColorFilter> brightnessFilter = SkColorFilters::Matrix(brightnessMat); // brightness
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm); // saturation
    sk_sp<SkColorFilter> filterCompose = SkColorFilters::Compose(satFilter, brightnessFilter);

    return SkImageFilters::ColorFilter(filterCompose, blurFilter);
}
#else
std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, float brightness)
{
    std::shared_ptr<Drawing::ImageFilter> blurFilter =
        Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP, nullptr);
    float normalizedDegree = brightness - 1.0;
    const Drawing::scalar brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };

    Drawing::ColorMatrix bm;
    bm.SetArray(brightnessMat);
    std::shared_ptr<Drawing::ColorFilter> brightnessFilter =
        Drawing::ColorFilter::CreateMatrixColorFilter(bm); // brightness
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    std::shared_ptr<Drawing::ColorFilter> satFilter = Drawing::ColorFilter::CreateMatrixColorFilter(cm); // saturation
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(brightnessFilter, satFilter); // saturation

    return Drawing::ImageFilter::CreateColorFilterImageFilter(*filterCompose, blurFilter);
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
#else
std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialStyle(
    MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
#endif
{
    if (materialParams_.find(style) != materialParams_.end()) {
        MaterialParam materialParam = materialParams_[style];
        maskColor_ = RSColor(materialParam.maskColor.AsRgbaInt());
        maskColor_.MultiplyAlpha(ratio);
        radius_ = RSMaterialFilter::RadiusVp2Sigma(materialParam.radius, dipScale) * ratio;
        saturation_ = (materialParam.saturation - 1) * ratio + 1.0;
        brightness_ = (materialParam.brightness - 1) * ratio + 1.0;
        return RSMaterialFilter::CreateMaterialFilter(radius_, saturation_, brightness_);
    }
    return nullptr;
}

#ifndef USE_ROSEN_DRAWING
void RSMaterialFilter::PreProcess(sk_sp<SkImage> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        SkColor colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(
            SkColorGetR(colorPicker), SkColorGetG(colorPicker), SkColorGetB(colorPicker), maskColor_.GetAlpha());
    }
}
#else
void RSMaterialFilter::PreProcess(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        auto avgColor = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        avgColor.SetAlpha(maskColor_.GetAlpha());
        maskColor_ = avgColor;
    }
}
#endif

void RSMaterialFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setColor(maskColor_.AsArgbInt());
    canvas.drawPaint(paint);
#else
    Drawing::Brush brush;
    brush.SetColor(maskColor_);
    canvas.DrawBackground(brush);
#endif
}

std::shared_ptr<RSFilter> RSMaterialFilter::TransformFilter(float fraction)
{
    MaterialParam materialParam;
    materialParam.radius = radius_ * fraction;
    materialParam.saturation = (saturation_ - 1) * fraction + 1.0;
    materialParam.brightness = (brightness_ - 1) * fraction + 1.0;
    materialParam.maskColor = RSColor(maskColor_.GetRed(), maskColor_.GetGreen(),
        maskColor_.GetBlue(), maskColor_.GetAlpha() * fraction);
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

bool RSMaterialFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    if (ROSEN_EQ(radius_, 0.f, epsilon)) {
        return false;
    }
    return true;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto materialR = std::static_pointer_cast<RSMaterialFilter>(rhs);

    MaterialParam materialParam;
    materialParam.radius = radius_ + materialR->radius_;
    materialParam.saturation = saturation_ + materialR->saturation_;
    materialParam.brightness = brightness_ + materialR->brightness_;
    materialParam.maskColor = maskColor_ + materialR->maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, materialR->colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto materialR = std::static_pointer_cast<RSMaterialFilter>(rhs);
    MaterialParam materialParam;
    materialParam.radius = radius_ - materialR->radius_;
    materialParam.saturation = saturation_ - materialR->saturation_;
    materialParam.brightness = brightness_ - materialR->brightness_;
    materialParam.maskColor = maskColor_ - materialR->maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, materialR->colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    MaterialParam materialParam;
    materialParam.radius = radius_ * rhs;
    materialParam.saturation = saturation_ * rhs;
    materialParam.brightness = brightness_ * rhs;
    materialParam.maskColor = maskColor_ * rhs;
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    MaterialParam materialParam;
    materialParam.radius = -radius_;
    materialParam.saturation = -saturation_;
    materialParam.brightness = -brightness_;
    materialParam.maskColor = RSColor(0x00000000) - maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

bool RSMaterialFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherMaterialFilter = std::static_pointer_cast<RSMaterialFilter>(other);
    if (otherMaterialFilter == nullptr) {
        return true;
    }
    return ROSEN_EQ(radius_, otherMaterialFilter->radius_, 1.0f) &&
           ROSEN_EQ(saturation_, otherMaterialFilter->saturation_, threshold) &&
           ROSEN_EQ(brightness_, otherMaterialFilter->brightness_, threshold) &&
           maskColor_.IsNearEqual(otherMaterialFilter->maskColor_, 1);
}

bool RSMaterialFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(radius_, 0.0f, threshold);
}
} // namespace Rosen
} // namespace OHOS
