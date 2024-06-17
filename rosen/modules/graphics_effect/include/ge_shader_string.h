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

#ifndef GRAPHICS_EFFECT_SHADER_STRING_H
#define GRAPHICS_EFFECT_SHADER_STRING_H
#include <string>

namespace OHOS {
namespace Rosen {

static std::string g_aibarString(R"(
    uniform half low;
    uniform half high;
    uniform half threshold;
    uniform half opacity;
    uniform half saturation;
    uniform shader imageShader;

    const vec3 toLuminance = vec3(0.3086, 0.6094, 0.0820);

    half4 main(float2 coord) {
        half3 c = imageShader.eval(coord).rgb;
        float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
        float bin = mix(high, low, step(threshold, gray));
        float luminance = dot(c, toLuminance);
        half3 satAdjust = mix(vec3(luminance), c, saturation);
        half3 res = satAdjust - (opacity + 1.0) * gray + bin;
        return half4(res, 1.0);
    }
)");

static std::string g_greyGradationString(R"(
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
        return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) :
            (rgb - coefficient2 * pow((1 - t_r), 3));
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

static std::string g_blurStringAF(R"(
    uniform shader imageShader;
    uniform float2 in_blurOffset[5];

    half4 main(float2 coord) {
        half4 c = half4(0, 0, 0, 0);
        for (int i = 0; i < 5; ++i) {
            c += imageShader.eval(float2(coord.x + in_blurOffset[i].x, coord.y + in_blurOffset[i].y));
        }
        return half4(c.rgb * 0.2, 1.0);
    }
)");

static std::string g_mixString(R"(
    uniform shader blurredInput;
    uniform shader originalInput;
    uniform float mixFactor;
    uniform float inColorFactor;

    highp float random(float2 xy) {
        float t = dot(xy, float2(78.233, 12.9898));
        return fract(sin(t) * 43758.5453);
    }
    half4 main(float2 coord) {
        highp float noiseGranularity = inColorFactor / 255.0;
        half4 finalColor = mix(originalInput.eval(coord), blurredInput.eval(coord), mixFactor);
        float noise  = mix(-noiseGranularity, noiseGranularity, random(coord));
        finalColor.rgb += noise;
        return finalColor;
    }
)");

static std::string g_blurString(R"(
    uniform shader imageShader;
    uniform float2 in_blurOffset;
    uniform float2 in_maxSizeXY;

    half4 main(float2 coord) {
        half4 c = imageShader.eval(coord);
        c += imageShader.eval(float2(clamp(in_blurOffset.x + coord.x, 0, in_maxSizeXY.x),
                                    clamp(in_blurOffset.y + coord.y, 0, in_maxSizeXY.y)));
        c += imageShader.eval(float2(clamp(in_blurOffset.x + coord.x, 0, in_maxSizeXY.x),
                                    clamp(-in_blurOffset.y + coord.y, 0, in_maxSizeXY.y)));
        c += imageShader.eval(float2(clamp(-in_blurOffset.x + coord.x, 0, in_maxSizeXY.x),
                                    clamp(in_blurOffset.y + coord.y, 0, in_maxSizeXY.y)));
        c += imageShader.eval(float2(clamp(-in_blurOffset.x + coord.x, 0, in_maxSizeXY.x),
                                    clamp(-in_blurOffset.y + coord.y, 0, in_maxSizeXY.y)));
        return half4(c.rgb * 0.2, 1.0);
    }
)");

static std::string g_simpleString(R"(
    uniform shader imageShader;
    half4 main(float2 coord) {
        return imageShader.eval(coord);
    }
)");

static const std::string g_hBlurString(R"(
    uniform half r;
    uniform shader imageShader;
    uniform shader gradientShader;
    half4 meanFilter(float2 coord, half radius)
    {
        half4 sum = vec4(0.0);
        half div = 0;
        for (half x = -30.0; x < 30.0; x += 1.0) {
            if (x > radius) {
                break;
            }
            if (abs(x) < radius) {
                div += 1;
                sum += imageShader.eval(coord + float2(x, 0));
            }
        }
        return half4(sum.xyz / div, 1.0);
    }
    half4 main(float2 coord)
    {
        if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
            return imageShader.eval(coord);
        }
        else {
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    }
)");

static const std::string g_vBlurString(R"(
    uniform half r;
    uniform shader imageShader;
    uniform shader gradientShader;
    half4 meanFilter(float2 coord, half radius)
    {
        half4 sum = vec4(0.0);
        half div = 0;
        for (half y = -30.0; y < 30.0; y += 1.0) {
            if (y > radius) {
                break;
            }
            if (abs(y) < radius) {
                div += 1;
                sum += imageShader.eval(coord + float2(0, y));
            }
        }
        return half4(sum.xyz / div, 1.0);
    }
    half4 main(float2 coord)
    {
        if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
            return imageShader.eval(coord);
        }
        else {
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    }
)");

static const std::string g_maskBlurString(R"(
    uniform shader srcImageShader;
    uniform shader blurImageShader;
    uniform shader gradientShader;
    half4 meanFilter(float2 coord)
    {
        vec3 srcColor = vec3(srcImageShader.eval(coord).r,
            srcImageShader.eval(coord).g, srcImageShader.eval(coord).b);
        vec3 blurColor = vec3(blurImageShader.eval(coord).r,
            blurImageShader.eval(coord).g, blurImageShader.eval(coord).b);
        float gradient = gradientShader.eval(coord).a;

        vec3 color = blurColor * gradient + srcColor * (1 - gradient);
        return vec4(color, 1.0);
    }
    half4 main(float2 coord)
    {
        if (abs(gradientShader.eval(coord).a) < 0.001) {
            return srcImageShader.eval(coord);
        }
        else {
            if (abs(gradientShader.eval(coord).a) > 0.999) {
                return blurImageShader.eval(coord);
            }
            else {
                return meanFilter(coord);
            }
        }
    }
)");

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_SHADER_STRING_H