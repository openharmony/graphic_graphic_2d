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

#include "ge_grey_shader_filter.h"

#include "ge_log.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

GEGreyShaderFilter::GEGreyShaderFilter(const Drawing::GEGreyShaderFilterParams& params)
    : greyCoef1_(params.greyCoef1), greyCoef2_(params.greyCoef2)
{
    if (!InitGreyAdjustmentEffect()) {
        LOGE("GEGreyShaderFilter::GEGreyShaderFilter failed to construct when initializing GreyAdjustmentEffect.");
        return;
    }
}

GEGreyShaderFilter::~GEGreyShaderFilter() {};

static std::shared_ptr<Drawing::RuntimeEffect> GREYADJUSTEFFECT;

std::shared_ptr<Drawing::Image> GEGreyShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEGreyShaderFilter::input image is null");
        return image;
    }

    if (!GREYADJUSTEFFECT) {
        LOGE("GEGreyShaderFilter::DrawGreyAdjustment greyAdjustEffect is null");
        return nullptr;
    }
    Drawing::RuntimeShaderBuilder builder(GREYADJUSTEFFECT);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);

    builder.SetChild("imageShader", imageShader);
    builder.SetUniform("coefficient1", greyCoef1_);
    builder.SetUniform("coefficient2", greyCoef2_);
    auto greyImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (greyImage == nullptr) {
        LOGE("DrawGreyAdjustment successful");
        return image;
    }
    return greyImage;
};

bool GEGreyShaderFilter::InitGreyAdjustmentEffect()
{
    static std::string GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        half4 main(float2 coord) {
            vec3 color = vec3(imageShader.eval(coord).r, imageShader.eval(coord).g, imageShader.eval(coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");

    if (GREYADJUSTEFFECT == nullptr) {
        GREYADJUSTEFFECT = Drawing::RuntimeEffect::CreateForShader(GreyGradationString);
        if (GREYADJUSTEFFECT == nullptr) {
            LOGE("GEKawaseBlurShaderFilter::RuntimeShader blurEffect create failed");
            return false;
        }
    }

    return true;
}

} // namespace Rosen
} // namespace OHOS
