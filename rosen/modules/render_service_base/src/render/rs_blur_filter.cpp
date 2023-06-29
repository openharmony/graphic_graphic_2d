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

#include "render/rs_blur_filter.h"

#include "src/core/SkOpts.h"

#include "common/rs_common_def.h"

#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
#include "include/core/SkTileMode.h"
#include "include/effects/SkImageFilters.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkImageFilters::Blur(blurRadiusX,
    blurRadiusY, SkTileMode::kClamp, nullptr)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);

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
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkBlurImageFilter::Make(blurRadiusX,
    blurRadiusY, nullptr, nullptr, SkBlurImageFilter::kClamp_TileMode)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);

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
#endif
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY) : RSDrawingFilter(
    Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY, Drawing::TileMode::CLAMP, nullptr)),
    blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);
}
#endif

RSBlurFilter::~RSBlurFilter() = default;

float RSBlurFilter::GetBlurRadiusX()
{
    return blurRadiusX_;
}

float RSBlurFilter::GetBlurRadiusY()
{
    return blurRadiusY_;
}

std::string RSBlurFilter::GetDescription()
{
    return "RSBlurFilter blur radius is " + std::to_string(blurRadiusX_) + " sigma";
}

bool RSBlurFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    if (ROSEN_EQ(blurRadiusX_, 0.f, epsilon) && ROSEN_EQ(blurRadiusY_, 0.f, epsilon)) {
        return false;
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSSkiaFilter> RSBlurFilter::Compose(const std::shared_ptr<RSSkiaFilter>& other) const
#else
std::shared_ptr<RSDrawingFilter> RSBlurFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
#endif
{
    std::shared_ptr<RSBlurFilter> result = std::make_shared<RSBlurFilter>(blurRadiusX_, blurRadiusY_);
#ifndef USE_ROSEN_DRAWING
    result->imageFilter_ = SkImageFilters::Compose(imageFilter_, other->GetImageFilter());
#else
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
#endif
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSFilter> RSBlurFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ + blurR->GetBlurRadiusX(), blurRadiusY_ + blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ - blurR->GetBlurRadiusX(), blurRadiusY_ - blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Multiply(float rhs)
{
    return std::make_shared<RSBlurFilter>(blurRadiusX_ * rhs, blurRadiusY_ * rhs);
}

std::shared_ptr<RSFilter> RSBlurFilter::Negate()
{
    return std::make_shared<RSBlurFilter>(-blurRadiusX_, -blurRadiusY_);
}

bool RSBlurFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherBlurFilter = std::static_pointer_cast<RSBlurFilter>(other);
    if (otherBlurFilter == nullptr) {
        return true;
    }
    return ROSEN_EQ(blurRadiusX_, otherBlurFilter->GetBlurRadiusX(), threshold) &&
           ROSEN_EQ(blurRadiusY_, otherBlurFilter->GetBlurRadiusY(), threshold);
}

bool RSBlurFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(blurRadiusX_, 0.0f, threshold) && ROSEN_EQ(blurRadiusY_, 0.0f, threshold);
}

void RSBlurFilter::DrawImageRect(
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

    int blurRadius = blurRadiusX_ * 3;
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
    blurBuilder.child("imgInput") = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);
    blurBuilder.uniform("in_blurOffset") = SkV2{stepX * kBlurScale, stepY * kBlurScale};
    blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * kBlurScale, dst.height() * kBlurScale};

    sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));

    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        const float stepScale = (float)i * kBlurScale;
        blurBuilder.child("imgInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);
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
    canvas.getTotalMatrix.invert(&inputMatrix);
    if (tmpBlur.width() == image.width() && tmpBlur.height() == image.height()) {
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
