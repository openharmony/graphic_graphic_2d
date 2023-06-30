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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H

#include "include/core/SkCanvas.h"
#include "include/core/SkData.h"
#include "include/core/SkPaint.h"
#include "include/core/SkSize.h"
#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkRuntimeEffect.h"
#include "tools/Resources.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class KawaseBlur {
public:
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

    KawaseBlur()
    {
        SkString blurString(R"(
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
            ROSEN_LOGE("KawaseBlurFilter::RuntimeShader error: %s\n", error.c_str());
        }
        fBlurEffect = std::move(blurEffect);

        auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
        if (!mixEffect) {
            ROSEN_LOGE("KawaseBlurFilter::RuntimeShader error: %s\n", error2.c_str());
        }
        fMixEffect = std::move(mixEffect);
    }

    static SkMatrix getShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale) {
        auto matrix = SkMatrix::Scale(scale, scale);
        matrix.postConcat(SkMatrix::Translate(blurRect.fLeft, blurRect.fTop));
        SkMatrix drawInverse;
        if (canvas != nullptr && canvas->getTotalMatrix().invert(&drawInverse)) {
            matrix.postConcat(drawInverse);
        }
        return matrix;
    }

    void ApplyKawaseBlur(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
    {
        int blurRadius = 347;;
        uint32_t maxPasses = supporteLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
        float dilatedConvolutionFactor = supporteLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
        float tmpRadius = static_cast<float>(blurRadius) / dilatedConvolutionFactor;
        float numberOfPasses = std::min(maxPasses, static_cast<uint32_t>(ceil(tmpRadius)));
        float radiusByPasses = tmpRadius / (float)numberOfPasses;
        // create blur surface with the bit depth and colorspace of the original surface
        SkImageInfo scaledInfo = image->imageInfo().makeWH(std::ceil(dst.width() * kBlurScale), 
            std::ceil(dst.height() * kBlurScale));
        SkMatrix blurMatrix = SkMatrix::Translate(-dst.fLeft, -dst.fTop);
        blurMatrix.postScale(kBlurScale, kBlurScale);
        // start by downscaling and doing the first blur pass
        SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
        SkRuntimeShaderBuilder blurBuilder(fBlurEffect);
        blurBuilder.child("imageInput") = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);
        blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * kBlurScale, radiusByPasses * kBlurScale};
        blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * kBlurScale, dst.height() * kBlurScale};
        sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));
        // And now we'll build our chain of scaled blur stages
        for (auto i = 1; i < numberOfPasses; i++) {
            const float stepScale = (float)i * kBlurScale;
            blurBuilder.child("imageInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);
            blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * stepScale, radiusByPasses * stepScale};
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

private:
    sk_sp<SkRuntimeEffect> fBlurEffect;
    sk_sp<SkRuntimeEffect> fMixEffect;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H