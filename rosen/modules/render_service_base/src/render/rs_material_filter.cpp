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

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "platform/common/rs_system_properties.h"

#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"

namespace OHOS {
namespace Rosen {
namespace {
// style to MaterialParam map
static const std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> MATERIAL_PARAM {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 23.0f,  1.05, 1.05, RSColor(0xFFFFFF33) } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  1.0,  RSColor(0xFAFAFA99) } },
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

static const std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> KAWASE_MATERIAL_PARAM {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 23.0f,  1.05, 1.05, RSColor(0xFFFFFF33) } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  1.0,  RSColor(0xFAFAFA99) } },
    { STYLE_CARD_THICK_LIGHT,        { 57.0f,  1.2,  1.1,  RSColor(0xFFFFFF8C) } },
    { STYLE_CARD_THIN_DARK,          { 75.0f,  1.35, 1.0,  RSColor(0x1A1A1A6B) } },
    { STYLE_CARD_DARK,               { 50.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    { STYLE_CARD_THICK_DARK,         { 75.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    // background blur params
    { STYLE_BACKGROUND_SMALL_LIGHT,  { 12.0f,  1.05, 1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_MEDIUM_LIGHT, { 29.0f,  1.1,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_LARGE_LIGHT,  { 45.0f,  1.2,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_XLARGE_LIGHT, { 120.0f, 1.3,  1.0,  RSColor(0x6666664C) } },
    { STYLE_BACKGROUND_SMALL_DARK,   { 15.0f,  1.1,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_MEDIUM_DARK,  { 55.0f,  1.15, 1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_LARGE_DARK,   { 75.0f,  1.5,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_XLARGE_DARK,  { 130.0f, 1.3,  1.0,  RSColor(0x0D0D0D80) } },
};
} // namespace

const bool KAWASE_BLUR_ENABLED = RSSystemProperties::GetKawaseEnabled();
const bool HPS_BLUR_ENABLED = RSSystemProperties::GetHpsBlurEnabled();

RSMaterialFilter::RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio,
    bool disableSystemAdaptation)
    : RSDrawingFilterOriginal(nullptr), colorMode_(mode), disableSystemAdaptation_(disableSystemAdaptation)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialStyle(static_cast<MATERIAL_BLUR_STYLE>(style), dipScale, ratio);
    type_ = FilterType::MATERIAL;

#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
    hash_ = hashFunc(&style, sizeof(style), hash_);
    hash_ = hashFunc(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = hashFunc(&ratio, sizeof(ratio), hash_);
    hash_ = hashFunc(&disableSystemAdaptation_, sizeof(disableSystemAdaptation_), hash_);
}

RSMaterialFilter::RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode)
    : RSDrawingFilterOriginal(nullptr), colorMode_(mode),
      radius_(materialParam.radius), saturation_(materialParam.saturation),
      brightness_(materialParam.brightness), maskColor_(materialParam.maskColor),
      disableSystemAdaptation_(materialParam.disableSystemAdaptation)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialFilter(
        materialParam.radius, materialParam.saturation, materialParam.brightness);
    type_ = FilterType::MATERIAL;
    if (colorMode_ == FASTAVERAGE) {
        colorMode_ = AVERAGE;
    }
    float radiusForHash = DecreasePrecision(radius_);
    float saturationForHash = DecreasePrecision(saturation_);
    float brightnessForHash = DecreasePrecision(brightness_);

#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
    hash_ = hashFunc(&radiusForHash, sizeof(radiusForHash), hash_);
    hash_ = hashFunc(&saturationForHash, sizeof(saturationForHash), hash_);
    hash_ = hashFunc(&brightnessForHash, sizeof(brightnessForHash), hash_);
    hash_ = hashFunc(&maskColor_, sizeof(maskColor_), hash_);
    hash_ = hashFunc(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = hashFunc(&disableSystemAdaptation_, sizeof(disableSystemAdaptation_), hash_);
}

RSMaterialFilter::~RSMaterialFilter() = default;

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale)
{
    float radiusPx = radiusVp * dipScale;
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + 0.5f : 0.0f;
}

std::string RSMaterialFilter::GetDescription()
{
    return "RSMaterialFilter blur radius is " + std::to_string(radius_) + " sigma";
}

std::string RSMaterialFilter::GetDetailedDescription()
{
    char maskColorStr[UINT8_MAX] = { 0 };
    auto ret = memset_s(maskColorStr, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for maskColorStr, ret=" + std::to_string(ret);
    }
    if (sprintf_s(maskColorStr, UINT8_MAX, "%08X", maskColor_.AsArgbInt()) != -1) {
        return "RSMaterialFilterBlur, radius: " + std::to_string(radius_) + " sigma" +
            ", saturation: " + std::to_string(saturation_) + ", brightness: " + std::to_string(brightness_) +
            ", greyCoef1: " + std::to_string(greyCoef_ == std::nullopt ? 0.0f : greyCoef_->x_) +
            ", greyCoef2: " + std::to_string(greyCoef_ == std::nullopt ? 0.0f : greyCoef_->y_) +
            ", color: " + maskColorStr + ", colorMode: " + std::to_string(colorMode_);
    };
    return "RSMaterialFilterBlur, maskColorStr failed";
}

std::shared_ptr<RSDrawingFilterOriginal> RSMaterialFilter::Compose(
    const std::shared_ptr<RSDrawingFilterOriginal>& other) const
{
    if (other == nullptr) {
        return nullptr;
    }
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_, disableSystemAdaptation_};
    std::shared_ptr<RSMaterialFilter> result = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    auto otherHash = other->Hash();
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    result->hash_ = hashFunc(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<Drawing::ColorFilter> RSMaterialFilter::GetColorFilter(float sat, float brightness)
{
    float normalizedDegree = brightness - 1.0;
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
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat);
    return filterCompose;
}

std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, float brightness)
{
    colorFilter_ = GetColorFilter(sat, brightness);
    auto blurType = KAWASE_BLUR_ENABLED ? Drawing::ImageBlurType::KAWASE : Drawing::ImageBlurType::GAUSS;
    if (colorFilter_) {
        return Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter_, radius, radius, blurType);
    }
    return Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP, nullptr, blurType);
}

std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialStyle(
    MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
{
    const auto& materialParams = KAWASE_BLUR_ENABLED ? KAWASE_MATERIAL_PARAM : MATERIAL_PARAM;
    if (auto iter = materialParams.find(style); iter != materialParams.end()) {
        const auto& materialParam = iter->second;
        maskColor_ = RSColor(materialParam.maskColor.AsRgbaInt());
        maskColor_.MultiplyAlpha(ratio);
        radius_ = RSMaterialFilter::RadiusVp2Sigma(materialParam.radius, dipScale) * ratio;
        saturation_ = (materialParam.saturation - 1) * ratio + 1.0;
        brightness_ = (materialParam.brightness - 1) * ratio + 1.0;
        return RSMaterialFilter::CreateMaterialFilter(radius_, saturation_, brightness_);
    }
    return nullptr;
}

void RSMaterialFilter::PreProcess(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
    }
}

void RSMaterialFilter::PostProcess(Drawing::Canvas& canvas)
{
    Drawing::Brush brush;
    brush.SetColor(maskColor_.AsArgbInt());
    canvas.DrawBackground(brush);
}

std::shared_ptr<RSFilter> RSMaterialFilter::TransformFilter(float fraction) const
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
    constexpr float epsilon = 0.999f;
    return radius_ > epsilon;
}

bool RSMaterialFilter::NeedForceSubmit() const
{
    return colorMode_ == AVERAGE;
}

void RSMaterialFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto brush = GetBrush();
    // if kawase blur failed, use gauss blur
    std::shared_ptr<Drawing::Image> greyImage = image;
    if (greyCoef_.has_value()) {
        greyImage = RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoef_.value());
    }
    if (greyImage == nullptr) {
        greyImage = image;
    }

    // if hps blur failed, use kawase blur
    Drawing::HpsBlurParameter hpsParam = Drawing::HpsBlurParameter(src, dst, GetRadius(), saturation_, brightness_);
    if (HPS_BLUR_ENABLED &&
        HpsBlurFilter::GetHpsBlurFilter().ApplyHpsBlur(canvas, greyImage, hpsParam, brush.GetColor().GetAlphaF())) {
        RS_OPTIONAL_TRACE_NAME("ApplyHPSBlur " + std::to_string(GetRadius()));
        return;
    }

    static bool DDGR_ENABLED = RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    KawaseParameter param = KawaseParameter(src, dst, radius_, colorFilter_, brush.GetColor().GetAlphaF());
    if (!DDGR_ENABLED && KAWASE_BLUR_ENABLED &&
        KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, greyImage, param)) {
        return;
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*greyImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

void RSMaterialFilter::SetGreyCoef(const std::optional<Vector2f>& greyCoef)
{
    greyCoef_ = greyCoef;
}

float RSMaterialFilter::GetRadius() const
{
    return radius_;
}

float RSMaterialFilter::GetSaturation() const
{
    return saturation_;
}

float RSMaterialFilter::GetBrightness() const
{
    return brightness_;
}

RSColor RSMaterialFilter::GetMaskColor() const
{
    return maskColor_;
}

BLUR_COLOR_MODE RSMaterialFilter::GetColorMode() const
{
    return colorMode_;
}

bool RSMaterialFilter::GetDisableSystemAdaptation() const
{
    return disableSystemAdaptation_;
}

bool RSMaterialFilter::CanSkipFrame() const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius_ > HEAVY_BLUR_THRESHOLD;
};

} // namespace Rosen
} // namespace OHOS
