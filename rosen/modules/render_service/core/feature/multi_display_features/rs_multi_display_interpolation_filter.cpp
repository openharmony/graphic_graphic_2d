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
#include "rs_multi_display_interpolation_filter.h"

#include "common/rs_optional_trace.h"
#include "draw/brush.h"
#include "effect/shader_effect.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr char INTERPOLATION_SHADER_PROG[] = R"(
    uniform shader imageInput;
    uniform float A;
    uniform float N;
    uniform float srcWidth;
    uniform float srcHeight;

    half4 main(float2 xy) {
        if (N <= 0.0 || A <= 0.0 || srcWidth <= 0.0 || srcHeight <= 0.0) {
            return half4(0.0, 0.0, 0.0, 1.0); // opaque black padding
        }
        const float HALF_PIXEL_OFFSET = 0.5;
        const half4 OPAQUE_BLACK = half4(0.0, 0.0, 0.0, 1.0);
        int dstY = int(floor(xy.y));
        int block = dstY / int(N);
        int offset = dstY - block * int(N);
        if (offset < int(A)) {
            int srcY = block * int(A) + offset;
            if (srcY >= 0 && srcY < int(srcHeight)) {
                float2 srcCoord = float2(xy.x / srcWidth,
                    (float(srcY) + HALF_PIXEL_OFFSET) / srcHeight);
                float2 pixelCoord = srcCoord * float2(srcWidth, srcHeight);
                return imageInput.eval(pixelCoord);
            }
        }
        return OPAQUE_BLACK;
    }
)";

std::shared_ptr<Drawing::RuntimeEffect> CreateInterpolationEffect()
{
    return Drawing::RuntimeEffect::CreateForShader(INTERPOLATION_SHADER_PROG);
}
} // namespace

RSMultiDisplayInterpolationFilter::RSMultiDisplayInterpolationFilter(
    float paramA, float paramN, float realWidth, float realHeight)
    : paramA_(paramA), paramN_(paramN), realWidth_(realWidth), realHeight_(realHeight) {}

std::string RSMultiDisplayInterpolationFilter::GetDescription() const
{
    return "RSMultiDisplayInterpolationFilter : paramA:" + std::to_string(paramA_) +
           ", paramN: " + std::to_string(paramN_) + ", realWidth: " + std::to_string(realWidth_) +
           ", realHeight: " + std::to_string(realHeight_);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSMultiDisplayInterpolationFilter::MakeInterpolationShaderBuilder()
{
    static std::shared_ptr<Drawing::RuntimeEffect> interpolationEffect = CreateInterpolationEffect();
    if (interpolationEffect == nullptr) {
        ROSEN_LOGE("RSMultiDisplayInterpolationFilter::RuntimeShader interpolationEffect create failed");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(interpolationEffect);
}

void RSMultiDisplayInterpolationFilter::DrawInterpolation(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& dstRect) const
{
    if (image == nullptr || image->GetWidth() <= 0 || image->GetHeight() <= 0) {
        ROSEN_LOGE("%{public}s: invalid image", __func__);
        return;
    }
    auto interpolationBuilder = MakeInterpolationShaderBuilder();
    if (interpolationBuilder == nullptr) {
        ROSEN_LOGE("RSMultiDisplayInterpolationFilter::shader error");
        canvas.DrawImage(*image, 0.0f, 0.0f, Drawing::SamplingOptions());
        return;
    }
    RS_OPTIONAL_TRACE_NAME("DrawInterpolation");
    float srcWidth = static_cast<float>(image->GetWidth());
    float srcHeight = static_cast<float>(image->GetHeight());

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::DECAL, Drawing::TileMode::DECAL, linear, Drawing::Matrix());
    interpolationBuilder->SetChild("imageInput", imageShader);
    interpolationBuilder->SetUniform("A", paramA_);
    interpolationBuilder->SetUniform("N", paramN_);
    interpolationBuilder->SetUniform("srcWidth", srcWidth);
    interpolationBuilder->SetUniform("srcHeight", srcHeight);

    std::shared_ptr<Drawing::ShaderEffect> interpolationShader = interpolationBuilder->MakeShader(nullptr, false);
    Drawing::Brush brush;
    brush.SetShaderEffect(interpolationShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dstRect);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS
