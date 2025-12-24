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
namespace OHOS::Rosen {

void RSColorAdaptiveFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto effect = Drawing::RuntimeEffect::CreateForShader(R"(
            uniform shader img;
            uniform float color;

            half4 main(float2 p) {
                half4 c = img.eval(p);
                half eps = 0.001;

                if (c.a > eps && abs(c.r - c.g) < eps &&
                    abs(c.r - c.b) < eps) {
                    return half4(color, color, color, c.a);
                }
                return c;
            }
        )");

    std::shared_ptr<Drawing::RuntimeShaderBuilder> effectBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    effectBuilder->SetChild("img", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
                                       Drawing::TileMode::CLAMP, Drawing::SamplingOptions {}, Drawing::Matrix {}));
    effectBuilder->SetUniform("color", grayScale_);
    Drawing::Brush brush;
    brush.SetShaderEffect(effectBuilder->MakeShader(nullptr, false));
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}
} // namespace OHOS::Rosen