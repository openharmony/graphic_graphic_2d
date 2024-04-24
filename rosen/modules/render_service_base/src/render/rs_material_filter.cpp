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

#include "src/core/SkOpts.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "platform/common/rs_system_properties.h"
#include "ge_render.h"
#include "ge_visual_effect.h"

#if defined(NEW_SKIA)
#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr float BLUR_SIGMA_SCALE = 0.57735f;
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

RSMaterialFilter::RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio)
    : RSDrawingFilter(nullptr), colorMode_(mode)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialStyle(static_cast<MATERIAL_BLUR_STYLE>(style), dipScale, ratio);
    type_ = FilterType::MATERIAL;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&style, sizeof(style), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = SkOpts::hash(&ratio, sizeof(ratio), hash_);
}

RSMaterialFilter::RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode)
    : RSDrawingFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
      brightness_(materialParam.brightness), maskColor_(materialParam.maskColor)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialFilter(
        materialParam.radius, materialParam.saturation, materialParam.brightness);
    type_ = FilterType::MATERIAL;
    if (colorMode_ == FASTAVERAGE) {
        colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
    }

    float radiusForHash = DecreasePrecision(radius_);
    float saturationForHash = DecreasePrecision(saturation_);
    float brightnessForHash = DecreasePrecision(brightness_);
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&radiusForHash, sizeof(radiusForHash), hash_);
    hash_ = SkOpts::hash(&saturationForHash, sizeof(saturationForHash), hash_);
    hash_ = SkOpts::hash(&brightnessForHash, sizeof(brightnessForHash), hash_);
    hash_ = SkOpts::hash(&maskColor_, sizeof(maskColor_), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
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

std::shared_ptr<RSDrawingFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
{
    if (other == nullptr) {
        return nullptr;
    }
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> result = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
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
    return Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter_, radius, radius, blurType);
}

std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialStyle(
    MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
{
    const auto& materialParams = KAWASE_BLUR_ENABLED ? KAWASE_MATERIAL_PARAM : MATERIAL_PARAM;
    if (materialParams.find(style) != materialParams.end()) {
        const auto& materialParam = materialParams.at(style);
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
    } else if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled &&
        imageSnapshot != nullptr) {
        RSColor color;
        if (colorPickerTask_->GetWaitRelease()) {
            if (colorPickerTask_->GetColor(color) && colorPickerTask_->GetFirstGetColorFinished()) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            return;
        }
        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, imageSnapshot)) {
            if (colorPickerTask_->GetColor(color)) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        }
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

void RSMaterialFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto brush = GetBrush();
    // if kawase blur failed, use gauss blur
    std::shared_ptr<Drawing::Image> greyImage = image;
    if (greyCoef_.has_value()) {
        auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        if (!visualEffectContainer) {
            return;
        }
        auto greyFilter = std::make_shared<Drawing::GEVisualEffect>("GREY", Drawing::DrawingPaintType::BRUSH);
        greyFilter->SetParam("GREY_COEF_1", greyCoef_.value()[0]);
        greyFilter->SetParam("GREY_COEF_2", greyCoef_.value()[1]);
        visualEffectContainer->AddToChainedFilter(greyFilter);
        auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
        if (!geRender) {
            return;
        }
        greyImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer,
            image, src, src, Drawing::SamplingOptions());
    }
    if (greyImage == nullptr) {
        greyImage = image;
    }

    // if hps blur failed, use kawase blur
    if (HPS_BLUR_ENABLED &&
        canvas.DrawBlurImage(*greyImage, Drawing::HpsBlurParameter(src, dst, GetRadius(), saturation_, brightness_))) {
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

bool RSMaterialFilter::CanSkipFrame() const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius_ > HEAVY_BLUR_THRESHOLD;
};

bool RSMaterialFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherMaterialFilter = std::static_pointer_cast<RSMaterialFilter>(other);
    if (otherMaterialFilter == nullptr) {
        ROSEN_LOGE("RSMaterialFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    return ROSEN_EQ(radius_, otherMaterialFilter->radius_, 1.0f) &&
           ROSEN_EQ(saturation_, otherMaterialFilter->saturation_, threshold) &&
           ROSEN_EQ(brightness_, otherMaterialFilter->brightness_, threshold) &&
           maskColor_.IsNearEqual(otherMaterialFilter->maskColor_, 0);
}

bool RSMaterialFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(radius_, 0.0f, threshold);
}

const std::shared_ptr<RSColorPickerCacheTask>& RSMaterialFilter::GetColorPickerCacheTask() const
{
    return colorPickerTask_;
}

void RSMaterialFilter::ReleaseColorPickerFilter()
{
    if (colorPickerTask_ == nullptr) {
        return;
    }
    colorPickerTask_->ReleaseColorPicker();
}

} // namespace Rosen
} // namespace OHOS
