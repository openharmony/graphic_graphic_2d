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
#include "ge_magnifier_shader_filter.h"

#include "ge_log.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static uint8_t COLOR_CHANNEL = 4; // 4 len of rgba
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> GEMagnifierShaderFilter::g_magnifierShaderEffect = nullptr;

GEMagnifierShaderFilter::GEMagnifierShaderFilter(const Drawing::GEMagnifierShaderFilterParams& params)
{
    magnifierPara_ = std::make_shared<GEMagnifierParams>();
    if (!magnifierPara_) {
        return;
    }
    magnifierPara_->factor_ = params.factor;
    magnifierPara_->width_ = params.width;
    magnifierPara_->height_ = params.height;
    magnifierPara_->cornerRadius_ = params.cornerRadius;
    magnifierPara_->borderWidth_ = params.borderWidth;
    magnifierPara_->shadowOffsetX_ = params.shadowOffsetX;
    magnifierPara_->shadowOffsetY_ = params.shadowOffsetY;
    magnifierPara_->shadowSize_ = params.shadowSize;
    magnifierPara_->shadowStrength_ = params.shadowStrength;
    magnifierPara_->gradientMaskColor1_ = params.gradientMaskColor1;
    magnifierPara_->gradientMaskColor2_ = params.gradientMaskColor2;
    magnifierPara_->outerContourColor1_ = params.outerContourColor1;
    magnifierPara_->outerContourColor2_ = params.outerContourColor2;
    magnifierPara_->rotateDegree_ = params.rotateDegree;
}

std::shared_ptr<Drawing::Image> GEMagnifierShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || magnifierPara_ == nullptr) {
        LOGE("GEMagnifierShaderFilter::ProcessImage image or para is null");
        return image;
    }

    Drawing::Matrix matrix;
    matrix.Rotate(magnifierPara_->rotateDegree_, src.GetLeft() + src.GetWidth() / 2.0f,
        src.GetTop() + src.GetHeight() / 2.0f); // 2.0 center of rect
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float imageWidth = image->GetWidth();
    float imageHeight = image->GetHeight();
    auto builder = MakeMagnifierShader(imageShader, imageWidth, imageHeight);
    if (builder == nullptr) {
        LOGE("GEMagnifierShaderFilter::ProcessImage builder is null");
        return image;
    }

    Drawing::Matrix invMatrix;
    invMatrix.Rotate(-magnifierPara_->rotateDegree_, src.GetLeft() + src.GetWidth() / 2.0f,
        src.GetTop() + src.GetHeight() / 2.0f); // 2.0 center of rect
#ifdef RS_ENABLE_GPU
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), &invMatrix, image->GetImageInfo(), false);
#else
    auto resultImage = builder->MakeImage(nullptr, &invMatrix, image->GetImageInfo(), false);
#endif
    if (resultImage == nullptr) {
        LOGE("GEMagnifierShaderFilter::ProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEMagnifierShaderFilter::MakeMagnifierShader(
    std::shared_ptr<Drawing::ShaderEffect> imageShader, float imageWidth, float imageHeight)
{
    if (g_magnifierShaderEffect == nullptr) {
        if (!InitMagnifierEffect()) {
            LOGE("GEMagnifierShaderFilter::failed when initializing MagnifierEffect.");
            return nullptr;
        }
    }

    if (magnifierPara_ == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(g_magnifierShaderEffect);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("iResolution", imageWidth, imageHeight);

    builder->SetUniform("factor", magnifierPara_->factor_);
    builder->SetUniform("size", magnifierPara_->width_, magnifierPara_->height_);
    builder->SetUniform("cornerRadius", magnifierPara_->cornerRadius_);
    builder->SetUniform("borderWidth", magnifierPara_->borderWidth_);

    builder->SetUniform("shadowOffset", magnifierPara_->shadowOffsetX_, magnifierPara_->shadowOffsetY_);
    builder->SetUniform("shadowSize", magnifierPara_->shadowSize_);
    builder->SetUniform("shadowStrength", magnifierPara_->shadowStrength_);

    float maskColor1[COLOR_CHANNEL] = { 0.0f };
    float maskColor2[COLOR_CHANNEL] = { 0.0f };
    float outColor1[COLOR_CHANNEL] = { 0.0f };
    float outColor2[COLOR_CHANNEL] = { 0.0f };
    ConvertToRgba(magnifierPara_->gradientMaskColor1_, maskColor1, COLOR_CHANNEL);
    ConvertToRgba(magnifierPara_->gradientMaskColor2_, maskColor2, COLOR_CHANNEL);
    ConvertToRgba(magnifierPara_->outerContourColor1_, outColor1, COLOR_CHANNEL);
    ConvertToRgba(magnifierPara_->outerContourColor2_, outColor2, COLOR_CHANNEL);
    builder->SetUniform("gradientMaskColor1", maskColor1, COLOR_CHANNEL);
    builder->SetUniform("gradientMaskColor2", maskColor2, COLOR_CHANNEL);
    builder->SetUniform("outerContourColor1", outColor1, COLOR_CHANNEL);
    builder->SetUniform("outerContourColor2", outColor2, COLOR_CHANNEL);

    return builder;
}

bool GEMagnifierShaderFilter::InitMagnifierEffect()
{
    if (g_magnifierShaderEffect == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader imageShader;
            uniform float2 iResolution;

            uniform float factor;
            uniform float borderWidth;
            uniform float cornerRadius;
            uniform float2 size;

            uniform float2 shadowOffset;
            uniform float shadowSize;
            uniform float shadowStrength;

            uniform vec4 gradientMaskColor1;
            uniform vec4 gradientMaskColor2;
            uniform vec4 outerContourColor1;
            uniform vec4 outerContourColor2;

            // refraction
            const float refractionStrength = 0.02;           // 0.02 refraction strength
            const float epsilon = 1e-4;

            vec4 sdfRect(vec2 position, vec2 R1, float R2, float curvature, out float isInBorder)
            {
                // calculate normal
                vec2 d = max(abs(position) - R1, 0.0);
                float dist = length(d) / R2;
                vec2 dir = normalize(sign(position) * d);
                float borderHeightRatio = min(size.x, size.y) / (borderWidth * 2.8); // 2.8 borderWidth
                float posInBorder = mix(1.0 - borderHeightRatio, 1.0, dist);
                float weight = max(posInBorder, 0.0);
                vec3 normal = normalize(mix(vec3(0.0, 0.0, 1.0), vec3(dir, 0.0), weight));
                isInBorder = smoothstep(0.0, 0.3, posInBorder); // 0.3 alpha threshold

                // calculate shadow
                position -= shadowOffset / iResolution.x;
                float R2Shadow = R2 + 0.5 * shadowSize / iResolution.x; // 0.5 half of shader size
                float distShadow = length(max(abs(position) - R1, 0.)) / R2Shadow;
                float shadowSizeHeightRatio = min(size.x, size.y) / (shadowSize / (curvature + epsilon) * 2.0);
                float weightShadow = max(mix(1.0 - shadowSizeHeightRatio, 1.0, distShadow), 0.0);
                float shadow = mix(1.0 - shadowStrength, 1.0, min(abs(weightShadow - 0.5) * 2.0, 1.0)); // 0.5 2.0 num

                return vec4(normal, shadow);
            }

            vec4 main(float2 fragCoord)
            {
                vec2 uv = fragCoord.xy / iResolution.x;
                vec2 boxPosition = iResolution / 2.0 / iResolution.x; // 2.0 center of rect
                vec2 halfBoxSize = size / iResolution.x / 2.0; // 2.0 half of resolution
                float curvature = cornerRadius / min(size.x, size.y) * 2.0; // 2.0 double of radius
                float mn = min(halfBoxSize.x, halfBoxSize.y) * (curvature + epsilon);

                float isInBorder = 0;
                vec4 magnifyingGlass = sdfRect(uv - boxPosition, halfBoxSize - vec2(mn), mn, curvature, isInBorder);
                vec4 finalColor = vec4(outerContourColor1.xyz, 1.0);

                // add refraction
                float red = magnifyingGlass.x;
                float green = magnifyingGlass.y;
                float offsetX = refractionStrength * sign(red) * red * red;
                float offsetY = -refractionStrength * sign(green) * green * green;
                vec2 sampleUV = (uv - boxPosition) / factor + boxPosition;
                vec4 refraction = imageShader.eval((sampleUV + vec2(offsetX, offsetY)) * iResolution.x);

                // add gradient mask
                float yDistToCenter = (uv.y - boxPosition.y) / halfBoxSize.y;
                float yValue = (yDistToCenter + 1.0) / 2.0; // 2.0 half of height
                vec4 gradientMask = mix(gradientMaskColor1, gradientMaskColor2, yValue);
                refraction.xyz = mix(refraction.xyz, gradientMask.xyz, gradientMask.w);

                // only apply refraction if z-value is not zero
                float mask = smoothstep(0.0, 0.3, magnifyingGlass.z); // 0.3 alpha threshold
                finalColor = mix(finalColor, refraction, mask);

                // add outer_contour color
                float xValue = (uv.x - boxPosition.x) / halfBoxSize.x;
                vec4 gradientContour = mix(outerContourColor1, outerContourColor2, abs(xValue));
                finalColor.xyz = mix(finalColor.xyz, gradientContour.xyz, gradientContour.w * isInBorder * mask);

                // add shadow
                finalColor.xyz *= magnifyingGlass.w;
                vec4 shadowColor = vec4(0.0, 0.0, 0.0, 1.0) * (1.0 - magnifyingGlass.w);
                finalColor = mix(shadowColor, finalColor, mask);

                return finalColor;
            }
        )";

        g_magnifierShaderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
        if (g_magnifierShaderEffect == nullptr) {
            LOGE("MakeMagnifierShader::RuntimeShader effect error\n");
            return false;
        }
    }
    return true;
}

void GEMagnifierShaderFilter::ConvertToRgba(uint32_t rgba, float* color, int tupleSize)
{
    if (!color || tupleSize < 4) { // 4 len of rgba
        return;
    }
    int16_t alpha = static_cast<int16_t>(rgba & 0xFF);               // 0xff byte mask
    int16_t red = static_cast<int16_t>((rgba & 0xFF000000) >> 24);   // 0xff000000 red mask, 24 red shift
    int16_t green = static_cast<int16_t>((rgba & 0x00FF0000) >> 16); // 0x00ff0000 green mask, 16 green shift
    int16_t blue = static_cast<int16_t>((rgba & 0x0000FF00) >> 8);   // 0x0000ff00 blue mask, 8 blue shift

    color[0] = red * 1.0f / 255.0f;     // 255.0f is the max value, 0 red
    color[1] = green * 1.0f / 255.0f;   // 255.0f is the max value, 1 green
    color[2] = blue * 1.0f / 255.0f;    // 255.0f is the max value, 2 blue
    color[3] = alpha * 1.0f / 255.0f;   // 255.0f is the max value, 3 alpha
}

const std::string GEMagnifierShaderFilter::GetDescription() const
{
    return "GEMagnifierShaderFilter";
}

} // namespace Rosen
} // namespace OHOS
