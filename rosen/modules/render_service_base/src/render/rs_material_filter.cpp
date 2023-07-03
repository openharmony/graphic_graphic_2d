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
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "platform/common/rs_system_properties.h"

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

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&style, sizeof(style), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = SkOpts::hash(&ratio, sizeof(ratio), hash_);

   SkString blurString(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2( clamp( in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp( in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp( in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp( in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));                                                          
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    SkString mixString(R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;

        half4 main(float2 xy) {
            return half4(mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor));
        }
    )");

    auto [blurEffect, error] = SkRuntimeEffect::MakeForShader(blurString);
    fBlurEffect = std::move(blurEffect);

    auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
    fMixEffect = std::move(mixEffect);
}

RSMaterialFilter::RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
#else
    : RSDrawingFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
#endif
      brightness_(materialParam.brightness), maskColor_(materialParam.maskColor)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialFilter(
        materialParam.radius, materialParam.saturation, materialParam.brightness);
    type_ = FilterType::MATERIAL;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&materialParam, sizeof(materialParam), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);

    SkString blurString(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2( clamp( in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp( in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp( in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp( in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2( clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                         clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));                                                          
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    SkString mixString(R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;

        half4 main(float2 xy) {
            return half4(mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor));
        }
    )");

    auto [blurEffect, error] = SkRuntimeEffect::MakeForShader(blurString);
    fBlurEffect = std::move(blurEffect);

    auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
    fMixEffect = std::move(mixEffect);
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
std::shared_ptr<RSSkiaFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSSkiaFilter>& other) const
#else
std::shared_ptr<RSDrawingFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSDrawingFilter>& inner) const
#endif
{
    if (other == nullptr) {
        return nullptr;
    }
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> result = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
#ifndef USE_ROSEN_DRAWING
    result->imageFilter_ = SkImageFilters::Compose(imageFilter_, other->GetImageFilter());
#else
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
#endif
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

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
        auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
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
    brush.SetColor(maskColor_.AsArgbInt());
    canvas.DrawBackground(brush);
#endif
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

void RSMaterialFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
{
    if (!useKawase) {
        auto paint = GetPaint();
#ifdef NEW_SKIA
        canvas.drawImageRect(image.get(), src, dst, SkSamplingOptions(), &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
        canvas.drawImageRect(image.get(), src, dst, &paint);
#endif
        return;
    }
    // Kawase is an approximation of Gaussian, but it behaves differently from it.
    // A radius transformation is required for approximating them, and also to introduce
    // non-integer steps, necessary to smoothly interpolate large radii.
    // Downsample scale factor used to improve performance
    static constexpr float kBlurScale = 0.03f;
    // Maximum number of render passes
    static constexpr uint32_t kMaxPasses = 4;
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolution = 2.0f;
    static constexpr float kDilatedConvolutionLargeRadius = 4.0f;
    // To avoid downscaling artifacts, we interpolate the blurred fbo with the full composited
    // image, up to this radius.
    static constexpr float kMaxCrossFadeRadius = 10.0f;
    bool supporteLargeRadius = true;

    int blurRadius = radius_ * 3;
    uint32_t maxPasses = supporteLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
    float dilatedConvolutionFactor = supporteLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
    float tmpRadius = static_cast<float>(blurRadius) / dilatedConvolutionFactor;
    float numberOfPasses = std::min(maxPasses, static_cast<uint32_t>(ceil(tmpRadius)));
    float radiusByPasses = tmpRadius / (float)numberOfPasses;

    // create blur surface with the bit depth and colorspace of the original surface
    SkImageInfo scaledInfo = image->imageInfo().makeWH(std::ceil(dst.width() * kBlurScale), std::ceil(dst.height() * kBlurScale));

    // For sampling Skia's API expects the inverse of what logically seems appropriate. In this
    // case you might expect Translate(blurRect.fLeft, blurRect.fTop) X Scale(kInverseInputScale)
    // but instead we must do the inverse.
    SkMatrix blurMatrix = SkMatrix::Translate(-dst.fLeft, -dst.fTop);
    blurMatrix.postScale(kBlurScale, kBlurScale);

    const float stepX = radiusByPasses;
    const float stepY = radiusByPasses;

    // start by downscaling and doing the first blur pass
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkRuntimeShaderBuilder blurBuilder(fBlurEffect);
    blurBuilder.child("imageInput") = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);
    blurBuilder.uniform("in_blurOffset") = SkV2{stepX * kBlurScale, stepY * kBlurScale};
    blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * kBlurScale, dst.height() * kBlurScale};

    sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        const float stepScale = (float)i * kBlurScale;
        blurBuilder.child("imageInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);
        blurBuilder.uniform("in_blurOffset") = SkV2{stepX * stepScale, stepY * stepScale};
        blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * kBlurScale, dst.height() * kBlurScale};
        tmpBlur = blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false);
    }

    float invBlurScale = 1 / kBlurScale;
    blurMatrix = SkMatrix::Scale(invBlurScale, invBlurScale);
    blurMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
    SkMatrix drawInverse;
    if (canvas.getTotalMatrix().invert(&drawInverse)) {
        blurMatrix.postConcat(drawInverse);
    }
    const auto blurShader = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &blurMatrix);

    SkMatrix inputMatrix;
    if (!canvas.getTotalMatrix().invert(&inputMatrix)) {
        inputMatrix.setIdentity();
    }
    if (tmpBlur->width() == image->width() && tmpBlur->height() == image->height()) {
        inputMatrix.preScale(invBlurScale, invBlurScale);
    }

    SkRuntimeShaderBuilder mixBuilder(fMixEffect);
    mixBuilder.child("blurredInput") = blurShader;
    mixBuilder.child("originalInput") = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, inputMatrix);
    mixBuilder.uniform("mixFactor") = std::min(1.0f, (float)blurRadius / kMaxCrossFadeRadius);

    SkPaint paint;
    paint.setShader(mixBuilder.makeShader(nullptr, true));
    canvas.drawIRect(image->bounds(), paint);
}
} // namespace Rosen
} // namespace OHOS
