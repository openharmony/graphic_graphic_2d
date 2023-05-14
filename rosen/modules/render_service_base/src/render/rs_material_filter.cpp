/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    : RSSkiaFilter(nullptr), colorMode_(mode)
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
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
}

std::string RSMaterialFilter::GetDescription()
{
    return "RSMaterialFilter blur radius is " + std::to_string(radius_) + " sigma";
}

std::shared_ptr<RSSkiaFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> material = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
    material->imageFilter_ = SkImageFilters::Compose(imageFilter_, inner->GetImageFilter());
    return material;
}

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

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
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

void RSMaterialFilter::PreProcess(sk_sp<SkImage> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        SkColor colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(
            SkColorGetR(colorPicker), SkColorGetG(colorPicker), SkColorGetB(colorPicker), maskColor_.GetAlpha());
    }
}

void RSMaterialFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    SkPaint paint;
    paint.setColor(maskColor_.AsArgbInt());
    canvas.drawPaint(paint);
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
} // namespace Rosen
} // namespace OHOS
