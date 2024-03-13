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

#include "skia_kawase_blur_filter.h"
#include "effect/runtime_shader_builder.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
// Advanced Filter
static constexpr uint32_t BLUR_SAMPLE_COUNT = 5;
// Advanced Filter: we can get normalized uv offset from width and height
struct OffsetInfo {
    float offsetX;
    float offsetY;
    int width;
    int height;
};
// keep advanced Filter close
static const bool IS_ADVANCED_FILTER_USABLE_CHECK_ONCE = false;

SkiaKawaseBlurFilter::SkiaKawaseBlurFilter()
{
    SetupBlurEffect();
    SetupMixEffect();
    // Advanced Filter
    if (IS_ADVANCED_FILTER_USABLE_CHECK_ONCE) {
        SetupBlurEffectAdvancedFilter();
    }
}

SkiaKawaseBlurFilter::~SkiaKawaseBlurFilter() = default;

void SkiaKawaseBlurFilter::SetupBlurEffect()
{
    std::string blurString(
        R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    auto blurEffect = Drawing::RuntimeEffect::CreateForShader(blurString);
    if (!blurEffect) {
        LOGE("SkiaKawaseBlurFilter::RuntimeShader blurEffect create failed");
        return;
    }
    blurEffect_ = std::move(blurEffect);
}

void SkiaKawaseBlurFilter::SetupMixEffect()
{
    std::string mixString(
        R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;
        uniform float inColorFactor;

        highp float random(float2 xy) {
            float t = dot(xy, float2(78.233, 12.9898));
            return fract(sin(t) * 43758.5453);
        }
        half4 main(float2 xy) {
            highp float noiseGranularity = inColorFactor / 255.0;
            half4 finalColor = mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor);
            float noise  = mix(-noiseGranularity, noiseGranularity, random(xy));
            finalColor.rgb += noise;
            return finalColor;
        }
    )");

    auto mixEffect = Drawing::RuntimeEffect::CreateForShader(mixString);
    if (!mixEffect) {
        LOGE("SkiaKawaseBlurFilter::RuntimeShader mixEffect create failed");
        return;
    }
    mixEffect_ = std::move(mixEffect);
}
// Advanced Filter
void SkiaKawaseBlurFilter::SetupBlurEffectAdvancedFilter()
{
    std::string blurStringAF(
        R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset[5];

        half4 main(float2 xy) {
            half4 c = half4(0, 0, 0, 0);
            for (int i = 0; i < 5; ++i) {
                c += imageInput.eval(float2(xy.x + in_blurOffset[i].x, xy.y + in_blurOffset[i].y));
            }
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    Drawing::RuntimeEffectOptions ops;
    ops.useAF = true;
    auto blurEffectAF = Drawing::RuntimeEffect::CreateForShader(blurStringAF, ops);
    if (!blurEffectAF) {
        LOGE("%s: RuntimeShader blurEffectAF create failed", __func__);
        return;
    }
    blurEffectAF_ = std::move(blurEffectAF);
}

static void getNormalizedOffset(SkV2* offsets, const uint32_t offsetCount, const OffsetInfo& offsetInfo)
{
    if (offsets == nullptr || offsetCount != BLUR_SAMPLE_COUNT) {
        LOGE("%s: Invalid offsets.", __func__);
        return;
    }
    if (std::fabs(offsetInfo.width) < 1e-6 || std::fabs(offsetInfo.height) < 1e-6) {
        LOGE("%s: Invalid width or height.", __func__);
        return;
    }
    SkV2 normalizedOffsets[BLUR_SAMPLE_COUNT] = {
        SkV2{0.0f, 0.0f},
        SkV2{offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height},
        SkV2{-offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height},
        SkV2{offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height},
        SkV2{-offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height}
    };
    for (uint32_t i = 0; i < BLUR_SAMPLE_COUNT; ++i) {
        offsets[i] = normalizedOffsets[i];
    }
}

Drawing::Matrix SkiaKawaseBlurFilter::GetShaderTransform(const Drawing::Canvas* canvas, const Drawing::Rect& blurRect,
    float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

void SkiaKawaseBlurFilter::CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const KawaseParameters& param, std::shared_ptr<Drawing::Image>& checkedImage)
{
    auto src = param.src;
    auto srcRect = Drawing::RectI(src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom());
    if (image && image->GetImageInfo().GetBound() != srcRect) {
        auto resizedImage = std::make_shared<Drawing::Image>();
        if (resizedImage->BuildSubset(image, srcRect, *canvas.GetGPUContext())) {
            checkedImage = resizedImage;
            LOGD("SkiaKawaseBlurFilter::resize image success");
        } else {
            LOGE("SkiaKawaseBlurFilter::resize image failed, use original image");
        }
    }
}

void SkiaKawaseBlurFilter::OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const KawaseParameters& param)
{
    auto src = param.src;
    auto dst = param.dst;
    Drawing::Brush brush;
    if (param.colorFilter) {
        Drawing::Filter filter;
        filter.SetColorFilter(param.colorFilter);
        brush.SetFilter(filter);
    }
    Drawing::Matrix inputMatrix;
    float scaleW = dst.GetWidth() / image->GetWidth();
    float scaleH = dst.GetHeight() / image->GetHeight();
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(scaleW, scaleH);
    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto inputShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, inputMatrix);
    brush.SetShaderEffect(inputShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

bool SkiaKawaseBlurFilter::ApplyKawaseBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const KawaseParameters& param, const KawaseProperties& properties)
{
    if (!blurEffect_ || !mixEffect_ || !image) {
        LOGE("SkiaKawaseBlurFilter::shader error, use Gauss instead");
        return false;
    }
    static bool useKawaseOriginal = properties.useKawaseOriginal;
    if (param.radius <= 0 || useKawaseOriginal) {
        LOGD("SkiaKawaseBlurFilter::input invalid radius : %{public}d", param.radius);
        OutputOriginalImage(canvas, image, param);
        return true;
    }
    auto input = image;
    CheckInputImage(canvas, image, param, input);
    ComputeRadiusAndScale(param.radius);
    int maxPasses = supportLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
    float dilatedConvolutionFactor = supportLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
    if (abs(dilatedConvolutionFactor) <= 1e-6) {
        return false;
    }
    float tmpRadius = static_cast<float>(blurRadius_) / dilatedConvolutionFactor;
    int numberOfPasses = std::min(maxPasses, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num
    if (numberOfPasses < 1) {
        return false;
    }
    float radiusByPasses = tmpRadius / numberOfPasses;
    LOGD("SkiaKawaseBlurFilter::kawase radius : %{public}f, scale : %{public}f, pass num : %{public}d",
        blurRadius_, blurScale_, numberOfPasses);
    auto width = std::max(static_cast<int>(std::ceil(param.dst.GetWidth())), input->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(param.dst.GetHeight())), input->GetHeight());
    auto blurParams = BlurParams{numberOfPasses, width, height, radiusByPasses};
    auto blurImage = ExecutePingPongBlur(canvas, input, param, blurParams, properties);
    if (!blurImage) {
        return false;
    }
    return ApplyBlur(canvas, input, blurImage, param, properties);
}

std::shared_ptr<Drawing::Image> SkiaKawaseBlurFilter::ExecutePingPongBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& input, const KawaseParameters& kParam, const BlurParams& bParam,
    const KawaseProperties& properties) const
{
    auto originImageInfo = input->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(bParam.width * blurScale_), std::ceil(bParam.height * blurScale_),
        originImageInfo.GetColorType(), originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::Matrix blurMatrix;
    blurMatrix.Translate(-kParam.src.GetLeft(), -kParam.src.GetTop());
    float scaleW = static_cast<float>(scaledInfo.GetWidth()) / input->GetWidth();
    float scaleH = static_cast<float>(scaledInfo.GetHeight()) / input->GetHeight();
    blurMatrix.PostScale(scaleW, scaleH);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    // Advanced Filter: check is AF usable only the first time
    bool isUsingAF = properties.useAF && blurEffectAF_ != nullptr;
    Drawing::RuntimeShaderBuilder blurBuilder(isUsingAF ? blurEffectAF_ : blurEffect_);
    blurBuilder.SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*input, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, blurMatrix));

    if (isUsingAF) {
        SkV2 firstPassOffsets[BLUR_SAMPLE_COUNT];
        OffsetInfo firstPassOffsetInfo = {bParam.radiusByPass * blurScale_, bParam.radiusByPass * blurScale_,
            scaledInfo.GetWidth(), scaledInfo.GetHeight()};
        getNormalizedOffset(firstPassOffsets, BLUR_SAMPLE_COUNT, firstPassOffsetInfo);
        blurBuilder.SetUniform("in_blurOffset", firstPassOffsetInfo.offsetX, firstPassOffsetInfo.offsetY,
            firstPassOffsetInfo.width, firstPassOffsetInfo.height);
    } else {
        blurBuilder.SetUniform("in_blurOffset", bParam.radiusByPass * blurScale_, bParam.radiusByPass * blurScale_);
        blurBuilder.SetUniform("in_maxSizeXY", bParam.width * blurScale_, bParam.height * blurScale_);
    }

    std::shared_ptr<Drawing::Image> tmpBlur(blurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));
    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < bParam.numberOfPasses; i++) {
        const float stepScale = static_cast<float>(i) * blurScale_;
        blurBuilder.SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*tmpBlur, Drawing::TileMode::CLAMP,
            Drawing::TileMode::CLAMP, linear, Drawing::Matrix()));

        // Advanced Filter
        if (isUsingAF) {
            SkV2 offsets[BLUR_SAMPLE_COUNT];
            OffsetInfo offsetInfo = {bParam.radiusByPass * stepScale, bParam.radiusByPass * stepScale,
                scaledInfo.GetWidth(), scaledInfo.GetHeight()};
            getNormalizedOffset(offsets, BLUR_SAMPLE_COUNT, offsetInfo);
            blurBuilder.SetUniform("in_blurOffset", offsetInfo.offsetX, offsetInfo.offsetY, offsetInfo.width,
                offsetInfo.height);
        } else {
            blurBuilder.SetUniform("in_blurOffset", bParam.radiusByPass * stepScale, bParam.radiusByPass * stepScale);
            blurBuilder.SetUniform("in_maxSizeXY", bParam.width * blurScale_, bParam.height * blurScale_);
        }
        tmpBlur = blurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
    }
    return tmpBlur;
}

bool SkiaKawaseBlurFilter::ApplyBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Drawing::Image>& blurImage, const KawaseParameters& param,
    const KawaseProperties& properties) const
{
    auto src = param.src;
    auto dst = param.dst;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto blurMatrix = GetShaderTransform(&canvas, dst, dst.GetWidth() / blurImage->GetWidth(),
        dst.GetHeight() / blurImage->GetHeight());
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(*blurImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, blurMatrix);
    Drawing::Brush brush;
    brush.SetAlphaF(param.alpha);
    if (param.colorFilter) {
        Drawing::Filter filter;
        filter.SetColorFilter(param.colorFilter);
        brush.SetFilter(filter);
    }
    static auto addRandomColor = properties.useRandomColor;
    if (addRandomColor) {
        Drawing::Matrix inputMatrix;
        inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
        inputMatrix.PostScale(dst.GetWidth() / image->GetWidth(), dst.GetHeight() / image->GetHeight());
        Drawing::Matrix matrix;
        matrix.Translate(dst.GetLeft(), dst.GetTop());
        inputMatrix.PostConcat(matrix);
        Drawing::RuntimeShaderBuilder mixBuilder(mixEffect_);
        mixBuilder.SetChild("blurredInput", blurShader);
        mixBuilder.SetChild("originalInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
            Drawing::TileMode::CLAMP, linear, inputMatrix));
        float mixFactor = (abs(kMaxCrossFadeRadius) <= 1e-6) ? 1.f : (blurRadius_ / kMaxCrossFadeRadius);
        mixBuilder.SetUniform("mixFactor", std::min(1.0f, mixFactor));
        static auto factor = properties.randomColorFactor;
        mixBuilder.SetUniform("inColorFactor", factor);
        LOGD("SkiaKawaseBlurFilter::kawase random color factor : %{public}f", factor);
        brush.SetShaderEffect(mixBuilder.MakeShader(nullptr, image->IsOpaque()));
    } else {
        brush.SetShaderEffect(blurShader);
    }
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    return true;
}

void SkiaKawaseBlurFilter::ComputeRadiusAndScale(int radius)
{
    blurRadius_ = radius * 4; // 4 : scale between gauss radius and kawase
    AdjustRadiusAndScale();
}

void SkiaKawaseBlurFilter::AdjustRadiusAndScale()
{
    static constexpr int radiusStep1 = 50; // 50 : radius step1
    static constexpr int radiusStep2 = 150; // 150 : radius step2
    static constexpr int radiusStep3 = 400; // 400 : radius step3
    static constexpr float scaleFactor1 = 0.25f; // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f; // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.0625f; // 0.0625 : downSample scale for step3
    auto radius = static_cast<int>(blurRadius_);
    if (radius > radiusStep3) {
        blurScale_ = scaleFactor3;
    } else if (radius > radiusStep2) {
        blurScale_ = scaleFactor2;
    } else if (radius > radiusStep1) {
        blurScale_ = scaleFactor1;
    } else {
        blurScale_ = baseBlurScale;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS