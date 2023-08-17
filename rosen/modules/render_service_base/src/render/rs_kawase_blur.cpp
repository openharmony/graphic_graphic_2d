/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "render/rs_kawase_blur.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"
#include "include/gpu/GrDirectContext.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
KawaseBlurFilter::KawaseBlurFilter()
{
    SkString blurString(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 1, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 1, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 1, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 1, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 1, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 1, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 1, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 1, in_maxSizeXY.y)));
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
    if (!blurEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader blurEffect error: %s\n", error.c_str());
        return;
    }
    blurEffect_ = std::move(blurEffect);

    auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
    if (!mixEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader mixEffect error: %s\n", error2.c_str());
        return;
    }
    mixEffect_ = std::move(mixEffect);

    useKawaseOriginal_ = RSSystemProperties::GetKawaseOriginalEnabled();
}

KawaseBlurFilter::~KawaseBlurFilter() = default;

SkMatrix KawaseBlurFilter::GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale)
{
    auto matrix = SkMatrix::Scale(scale, scale);
    matrix.postConcat(SkMatrix::Translate(blurRect.fLeft, blurRect.fTop));
    return matrix;
}

void KawaseBlurFilter::OutputOriginalImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param)
{
    auto src = param.src;
    auto dst = param.dst;
    SkPaint paint;
    if (param.colorFilter) {
        paint.setColorFilter(param.colorFilter);
    }
    SkMatrix inputMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
    inputMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    const auto inputShader = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &inputMatrix);
    paint.setShader(inputShader);
    canvas.drawRect(dst, paint);
}

bool KawaseBlurFilter::ApplyKawaseBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param)
{
    if (!blurEffect_ || !mixEffect_) {
        ROSEN_LOGE("KawaseBlurFilter::shader error, use Gauss\n");
        return false;
    }
    auto src = param.src;
    auto dst = param.dst;
    auto srcRect = SkIRect::MakeLTRB(src.left(), src.top(), src.right(), src.bottom());
    auto input = image;
    if (image->bounds() != srcRect && image->bounds().contains(srcRect)) {
        if (auto resizedImage = image->makeSubset(srcRect, canvas.recordingContext()->asDirectContext())) {
            input = resizedImage;
        }
    }
    static auto useKawaseOriginal = RSSystemProperties::GetKawaseOriginalEnabled();
    if (param.radius <= 0 || useKawaseOriginal) {
        OutputOriginalImage(canvas, input, param);
        return true;
    }
    ComputeRadiusAndScale(param.radius);
    RS_OPTIONAL_TRACE_BEGIN("ApplyKawaseBlur " + GetDescription());
    int maxPasses = supportLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
    float dilatedConvolutionFactor = supportLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
    float tmpRadius = static_cast<float>(blurRadius_) / dilatedConvolutionFactor;
    int numberOfPasses = std::min(maxPasses, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num
    float radiusByPasses = tmpRadius / numberOfPasses;
    auto width = std::max(static_cast<int>(std::ceil(dst.width())), input->width());
    auto height = std::max(static_cast<int>(std::ceil(dst.height())), input->height());
    SkImageInfo scaledInfo = input->imageInfo().makeWH(std::ceil(width * blurScale_), std::ceil(height * blurScale_));
    SkMatrix blurMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
    blurMatrix.postScale(blurScale_, blurScale_);
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkRuntimeShaderBuilder blurBuilder(blurEffect_);
    blurBuilder.child("imageInput") = input->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);
    blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * blurScale_, radiusByPasses * blurScale_};
    blurBuilder.uniform("in_maxSizeXY") = SkV2{width * blurScale_, height * blurScale_};
    sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));
    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        const float stepScale = static_cast<float>(i) * blurScale_;
        blurBuilder.child("imageInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);
        blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * stepScale, radiusByPasses * stepScale};
        blurBuilder.uniform("in_maxSizeXY") = SkV2{width * blurScale_, height * blurScale_};
        tmpBlur = blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false);
    }
    RS_OPTIONAL_TRACE_END();
    return ApplyBlur(canvas, input, tmpBlur, param);
}

bool KawaseBlurFilter::ApplyBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage,
    const KawaseParameter& param) const
{
    auto src = param.src;
    auto dst = param.dst;
    float invBlurScale = 1.0f / blurScale_;
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    const auto blurMatrix = GetShaderTransform(&canvas, dst, invBlurScale);
    const auto blurShader = blurImage->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &blurMatrix);
    SkPaint paint;
    paint.setAlphaf(param.alpha);
    if (param.colorFilter) {
        paint.setColorFilter(param.colorFilter);
    }
    if (blurRadius_ < kMaxCrossFadeRadius) {
        SkMatrix inputMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
        inputMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
        SkRuntimeShaderBuilder mixBuilder(mixEffect_);
        mixBuilder.child("blurredInput") = blurShader;
        mixBuilder.child("originalInput") = image->makeShader(
            SkTileMode::kClamp, SkTileMode::kClamp, linear, inputMatrix);
        mixBuilder.uniform("mixFactor") = std::min(1.0f, blurRadius_ * 1.f / kMaxCrossFadeRadius);
        paint.setShader(mixBuilder.makeShader(nullptr, true));
    } else {
        paint.setShader(blurShader);
    }
    canvas.drawRect(dst, paint);
    return true;
}

void KawaseBlurFilter::ComputeRadiusAndScale(int radius)
{
    static constexpr int noiseFactor = 3; // 3 : smooth the radius change
    blurRadius_ = radius * 4 / noiseFactor * noiseFactor; // 4 : scale between gauss radius and kawase
    AdjustRadiusAndScale();
}

void KawaseBlurFilter::AdjustRadiusAndScale()
{
    static constexpr int radiusStep1 = 50; // 50 : radius step1
    static constexpr int radiusStep2 = 150; // 150 : radius step2
    static constexpr int radiusStep3 = 480; // 480 : radius step3
    static constexpr float scaleFactor1 = 0.25f; // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f; // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.03f; // 0.03 : downSample scale for step3
    float scale = baseBlurScale;
    if (blurRadius_ > radiusStep3) {
        scale = scaleFactor3;
    } else if (blurRadius_ > radiusStep2) {
        scale = scaleFactor2;
    } else if (blurRadius_ > radiusStep1) {
        scale = scaleFactor1;
    }
    blurScale_ = scale;
}

std::string KawaseBlurFilter::GetDescription() const
{
    return "blur radius is " + std::to_string(blurRadius_);
}
#endif
} // namespace Rosen
} // namespace OHOS