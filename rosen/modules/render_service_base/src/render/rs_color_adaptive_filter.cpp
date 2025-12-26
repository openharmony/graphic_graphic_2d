/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "render/rs_color_adaptive_filter.h"

#include "effect/runtime_effect.h"
#include "effect/shader_effect.h"
#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
namespace {
std::shared_ptr<Drawing::RuntimeShaderBuilder> GetEffectBuilder()
{
    static auto runtimeEffect = Drawing::RuntimeEffect::CreateForShader(R"(
            uniform shader img;
            uniform float color;

            half4 main(float2 p) {
                half4 c = img.eval(p);
                half eps = 0.001;

                if (c.a > 0.1 && abs(c.r - c.g) < eps &&
                    abs(c.r - c.b) < eps) {
                    return half4(color, color, color, c.a);
                }
                return c;
            }
        )");
    if (!runtimeEffect) {
        ROSEN_LOGE("RSColorAdaptiveFilter::GetEffectBuilder runtimeEffect create failed");
        return nullptr;
    }
    auto effectBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(runtimeEffect);
    return effectBuilder;
}
} // namespace
void RSColorAdaptiveFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto effectBuilder = GetEffectBuilder();
    if (!effectBuilder) {
        ROSEN_LOGE("RSColorAdaptiveFilter::DrawImageRect effectBuilder is null");
        return;
    }
    effectBuilder->SetChild("img", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
                                       Drawing::TileMode::CLAMP, Drawing::SamplingOptions {}, Drawing::Matrix {}));
    effectBuilder->SetUniform("color", grayScale_);
    Drawing::Brush brush;
    brush.SetShaderEffect(effectBuilder->MakeShader(nullptr, false));
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> RSColorAdaptiveFilter::ApplyFilter(
    std::shared_ptr<Drawing::GPUContext> ctx, std::shared_ptr<Drawing::Image> image, float grayScale)
{
    if (!ctx || !image) {
        return image;
    }
    auto effectBuilder = GetEffectBuilder();
    if (!effectBuilder) {
        ROSEN_LOGE("RSColorAdaptiveFilter::ApplyFilter effectBuilder is null");
        return image;
    }
    effectBuilder->SetChild("img", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
                                       Drawing::TileMode::CLAMP, Drawing::SamplingOptions {}, Drawing::Matrix {}));
    effectBuilder->SetUniform("color", grayScale);
    return effectBuilder->MakeImage(ctx.get(), nullptr, image->GetImageInfo(), false);
}
} // namespace OHOS::Rosen