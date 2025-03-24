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
#ifndef GRAPHICS_EFFECT_GE_WATER_RIPPLE_FILTER_H
#define GRAPHICS_EFFECT_GE_WATER_RIPPLE_FILTER_H
 
#include <memory>
 
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
 
#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"
 
namespace OHOS {
namespace Rosen {
class GEWaterRippleFilter : public GEShaderFilter {
public:
    GEWaterRippleFilter(const Drawing::GEWaterRippleFilterParams& params);
    ~GEWaterRippleFilter() override = default;
 
    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;
 
private:
    std::shared_ptr<Drawing::RuntimeEffect> GetWaterRippleEffect();
    std::shared_ptr<Drawing::RuntimeEffect> GetWaterRippleEffectSM(const int rippleMode);
    std::shared_ptr<Drawing::RuntimeEffect> GetWaterRippleEffectSS();
    std::shared_ptr<Drawing::RuntimeEffect> GetWaterRippleEffectMR();
    float progress_ = 0.0f;
    uint32_t waveCount_ = 2;
    float rippleCenterX_ = 0.5f;
    float rippleCenterY_ = 0.7f;
    uint32_t rippleMode_ = 1;

    inline static const std::string shaderStringSMsend = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half progress;
        uniform half waveCount;
        uniform half2 rippleCenter;

        const half basicSlope = 0.5;
        const half gAmplSupress = 0.012;
        const half waveFreq = 31.0;
        const half wavePropRatio = 2.0;
        const half ampSupArea = 0.45;
        const half intensity = 0.15;

        half calcWave(half dis)
        {
            half preWave = smoothstep(0., -0.3, dis);
            half waveForm = (waveCount == 1.) ?
                smoothstep(-0.4, -0.2, dis) * smoothstep(0., -0.2, dis) :
                (waveCount == 2.) ?
                smoothstep(-0.6, -0.3, dis) * preWave :
                smoothstep(-0.9, -0.6, dis) * step(abs(dis + 0.45), 0.45) * preWave;
            return -sin(waveFreq * dis) * waveForm;
        }

        half waveGenerator(half propDis, half t)
        {
            half dis = propDis - wavePropRatio * t;
            half h = 1e-3;
            half d1 = dis - h;
            half d2 = dis + h;
            return (calcWave(d2) - calcWave(d1)) / (2. * h);
        }

        half4 main(vec2 fragCoord)
        {
            half shortEdge = min(iResolution.x, iResolution.y);
            half2 uv = fragCoord.xy / iResolution.xy;
            half2 uvHomo = fragCoord.xy / shortEdge;
            half2 resRatio = iResolution.xy / shortEdge;

            half progSlope = basicSlope + 0.1 * waveCount;
            half t = progSlope * progress;

            half2 waveCenter = rippleCenter * resRatio;
            half propDis = distance(uvHomo, waveCenter);
            half2 v = uvHomo - waveCenter;
            half ampDecayByT = (propDis < 1.) ? pow((1. - propDis), 4.) : 0.;
            
            half ampSupByDis = smoothstep(0., ampSupArea, propDis);
            half hIntense = waveGenerator(propDis, t) * ampDecayByT * ampSupByDis * gAmplSupress;
            half2 circles = normalize(v) * hIntense;

            half3 norm = vec3(circles, hIntense);
            half2 expandUV = (uv - intensity * norm.xy) * iResolution.xy;
            half3 color = image.eval(expandUV).rgb;
            color += 150. * pow(clamp(dot(norm, normalize(vec3(0., 4., -0.5))), 0., 1.), 2.5);
            
            return half4(color, 1.0);
        }
    )";
    
    inline static const std::string shaderStringSSmutual = R"(
        uniform shader image;
        uniform vec2 iResolution;
        uniform float progress;
        uniform float waveCount;
        uniform vec2 rippleCenter;
        // small
        const float s_basicSlope = 0.5;
        const float s_ampSupress = 0.04;
        const float s_waveFreq = 31.0;
        float s_wavePropRatio = 2.;
        const float s_ampSupArea = 0.3;
        const float s_intensity = 0.15;
        const float s_decayExp = 4. ;
        const float s_luminance = 60.;
        const vec3 s_lightDirect = vec3(0., -4., -0.2);
        // big
        const float b_ampSupress = 0.01;
        const float b_waveFreq = 7.0;
        float b_wavePropRatio = 6.9;
        const float b_intensity = 0.15;
        const float b_decayExp = 4.;
        const float b_luminance = 30.;

        const vec3 waveAxis = vec3(2., 3., 5.);

        vec3 lightBlend(vec3 colorA, vec3 colorB)
        {
            vec3 oneVec = vec3(1.);
            return oneVec - ((oneVec - colorA) * (oneVec - colorB));
        }

        float calcWave(float count, float freq, float dis)
        {
            float axisVal = (count == 1.) ? waveAxis.x : (count == 2.) ? waveAxis.y : waveAxis.z;
            float axisPoint = -(axisVal * 3.1416) / freq;
            float waveForm = smoothstep(axisPoint * 2., axisPoint, dis) * smoothstep(0., axisPoint, dis);
            float downCond = (count == 3.) ? -1. : 1.;
            return sin(freq * dis) * waveForm * downCond;
        }

        float calcBLight(float dis, float freq)
        {
            float currentX = pow(dis + (6.2832 / freq), 2.);
            return 1.2 * exp(-55. * currentX);
        }

        float calcSLight(float dis, float freq, float yShift)
        {
            float pivot1 = pow(dis + (9.4248 / freq) - 0.14, 2.);
            float pivot2 = pow(dis + (9.4248 / freq) + 0.01, 2.);
            return 2. * yShift * (exp(-1000. * pivot2) + exp(-1000. * pivot1));
        }

        vec2 waveGenerator(float propDis, float t, float count, float freq, float prop, float yShift)
        {
            float dis = propDis - prop * t;
            float h = 1e-3;
            float d1 = dis - h;
            float d2 = dis + h;
            float waveVal = (calcWave(count, freq, d2) - calcWave(count, freq, d1)) / (2. * h);
            float lightAdjust = (freq < 10.) ? calcBLight(dis, freq) : calcSLight(dis, freq, yShift);
            return vec2(waveVal, lightAdjust);
        }

        vec4 main(vec2 fragCoord)
        {
            float s_waveCount = waveCount;
            vec2 b_rippleCenter = rippleCenter;
            float shortEdge = min(iResolution.x, iResolution.y);
            vec2 uv = fragCoord.xy / iResolution.xy;
            vec2 uvHomo = fragCoord.xy / shortEdge;
            vec2 resRatio = iResolution.xy / shortEdge;

            float b_progSlope = 0.4;
            float s_progSlope = s_basicSlope + 0.1 * s_waveCount;
            float b_t = b_progSlope * (progress + 0.4);
            // float s_t = fract(s_progSlope * progress);
            float s_t =  s_progSlope * (progress + 0.11);

            float veloDecay = 1. - 0.04 * (smoothstep(0.2, 0.16, progress) + smoothstep(0.2, 1.2, progress));
            b_wavePropRatio *= veloDecay;
            s_wavePropRatio *= veloDecay;

            vec2 b_waveCenter = b_rippleCenter * resRatio;
            vec2 s_rippleCenter = vec2(0.5, 0.);
            s_rippleCenter.x = (b_rippleCenter.x == 0.5) ? 0.5 : floor(b_rippleCenter.x + 0.5);
            s_rippleCenter.y = (b_rippleCenter.y == 0.5) ? 0.5 : floor(b_rippleCenter.y + 0.5);
            vec2 s_waveCenter = s_rippleCenter * resRatio;
            float b_propDis = distance(uvHomo, b_waveCenter);
            float s_propDis = distance(uvHomo, s_waveCenter);
            vec2 b_vec = uvHomo - b_waveCenter;
            vec2 s_vec = uvHomo - s_waveCenter;
            float b_ampDecayByDis = (b_propDis < 1.9) ? clamp(pow((1.9 - b_propDis), b_decayExp), 0., 1.): 0.;
            float s_ampDecayByDis = (s_propDis < 0.7) ? clamp(pow((0.7 - s_propDis), s_decayExp), 0., 1.): 0.;

            float s_ampSupCenter = smoothstep(0., s_ampSupArea, s_propDis);
            vec2 b_waveRes = waveGenerator(b_propDis, b_t, 1., b_waveFreq, b_wavePropRatio, 1.);
            vec2 s_waveRes = waveGenerator(
                s_propDis, s_t, s_waveCount, s_waveFreq, s_wavePropRatio, abs(normalize(s_vec)[1]));
            float b_intense = b_waveRes[0] * b_ampDecayByDis * b_ampSupress;
            float s_intense = s_waveRes[0] * s_ampDecayByDis * s_ampSupCenter * s_ampSupress;
            float b_Prime = b_waveRes[1] * b_ampDecayByDis * b_ampSupress;
            float s_Prime = s_waveRes[1] * s_ampDecayByDis * s_ampSupCenter * s_ampSupress;
            vec2 b_circles = normalize(b_vec) * b_intense;
            vec2 s_circles = normalize(s_vec) * s_intense;

            vec3 b_norm = vec3(b_circles, b_intense);
            vec3 s_norm = vec3(s_circles, s_intense);

            vec2 warp = (b_intensity * b_norm.xy + s_intensity * s_norm.xy) * smoothstep(0., 0.07, progress);
            vec2 expandUV = (uv - warp) * iResolution.xy;
            vec3 color = image.eval(expandUV).rgb;

            float b_light = b_luminance * clamp(b_Prime, 0., 1.) * smoothstep(0., 0.125, progress);;
            float s_light = s_luminance * clamp(s_Prime, 0., 1.);

            color += s_light;
            color = lightBlend(color, vec3(b_light));
            // color = lightBlend(color, vec3(s_light));

            return vec4(color, 1.0);
        }
    )";

    inline static const std::string shaderStringSMrecv = R"(
        uniform shader image;
        uniform vec2 iResolution;
        uniform float progress;
        uniform float waveCount;
        uniform vec2 rippleCenter;

        const float basicSlope = 0.7;
        const float gAmplSupress = 0.003;
        float waveFreq = 25.0;
        const float wavePropRatio = 2.0;
        const float ampSupArea = 0.45;
        const float intensity = 0.15;

        const float decayExp = 1.5;
        float luminance = 230.;
        const float highLightExp = 2.5;
        const vec3 lightDirect = half3(0., -4., 0.5);
        const vec3 waveAxis = half3(2., 3., 5.);

        float calcWave(float dis)
        {
            float axisPoint = -6.283 / waveFreq;
            float waveForm = smoothstep(axisPoint * 2., axisPoint, dis) * smoothstep(0., axisPoint, dis);
            return sin(waveFreq * dis) * waveForm;
        }

        float waveGenerator(float propDis, float t)
        {
            float dis = propDis - wavePropRatio * t;
            float h = 1e-3;
            float d1 = dis - h;
            float d2 = dis + h;
            return (calcWave(d2) - calcWave(d1)) / (2. * h);
        }

        vec4 main(vec2 fragCoord)
        {
            float shortEdge = min(iResolution.x, iResolution.y);
            vec2 uv = fragCoord.xy / iResolution.xy;
            vec2 uvHomo = fragCoord.xy / shortEdge;
            vec2 resRatio = iResolution.xy / shortEdge;

            float progSlope = basicSlope + 0.1 * waveCount;
            float t = progSlope * progress;
            waveFreq -= t * 20.;
            waveFreq = (waveFreq > 15.) ? waveFreq : 15.;

            vec2 waveCenter = rippleCenter * resRatio;
            float propDis = distance(uvHomo, waveCenter);
            vec2 v = uvHomo - waveCenter;
            float ampDecayByT = (propDis < 1.3) ? clamp(pow((1.3 - propDis), decayExp), 0., 1.): 0.;
            
            float ampSupByDis = smoothstep(0., ampSupArea, propDis);
            float hIntense = waveGenerator(propDis, t) * ampDecayByT * ampSupByDis * gAmplSupress;
            vec2 circles = normalize(v) * hIntense;

            vec3 norm = vec3(circles, hIntense);
            vec2 expandUV = (uv - intensity * norm.xy) * iResolution.xy;
            vec3 color = image.eval(expandUV).rgb;
            color += luminance * pow(clamp(dot(norm, normalize(lightDirect)), 0., 1.), highLightExp);
            
            return vec4(color, 1.0);
        }
    )";

    inline static const std::string shaderStringMiniRecv = R"(
        uniform shader image;
        uniform vec2 iResolution;
        uniform float progress;
        uniform float waveCount;
        uniform vec2 rippleCenter;

        const float rippleSize = 0.3;
        const float basicSlope = 0.7;
        const float gAmplSupress = 0.003;
        float waveFreq = 25.0;
        const float wavePropRatio = 2.0;
        const float ampSupArea = 0.45;
        const float intensity = 0.15;

        const float decayExp = 1.5;
        float luminance = 230.;
        const float highLightExp = 2.5;
        const vec3 lightDirect = half3(0., -4., 0.5);
        const vec3 waveAxis = half3(2., 3., 5.);

        float calcWave(float dis)
        {
            float axisPoint = -6.283 / waveFreq;
            float waveForm = smoothstep(axisPoint * 2., axisPoint, dis) * smoothstep(0., axisPoint, dis);
            return sin(waveFreq * dis) * waveForm;
        }

        float waveGenerator(float propDis, float t)
        {
            float dis = propDis - wavePropRatio * t;
            float h = 1e-3;
            float d1 = dis - h;
            float d2 = dis + h;
            return (calcWave(d2) - calcWave(d1)) / (2. * h);
        }

        vec4 main(vec2 fragCoord)
        {
            float shortEdge = min(iResolution.x, iResolution.y) * rippleSize;
            shortEdge = (shortEdge < 1e-6) ? min(iResolution.x, iResolution.y) : shortEdge;
            vec2 uv = fragCoord.xy / iResolution.xy;
            vec2 uvHomo = fragCoord.xy / shortEdge;
            vec2 resRatio = iResolution.xy / shortEdge;

            float progSlope = basicSlope + 0.1 * waveCount;
            float t = progSlope * progress;
            waveFreq -= t * 20.;
            waveFreq = (waveFreq > 15.) ? waveFreq : 15.;

            vec2 waveCenter = rippleCenter * resRatio;
            float propDis = distance(uvHomo, waveCenter);
            vec2 v = uvHomo - waveCenter;
            float ampDecayByT = (propDis < 1.3) ? clamp(pow((1.3 - propDis), decayExp), 0., 1.): 0.;
            
            float ampSupByDis = smoothstep(0., ampSupArea, propDis);
            float hIntense = waveGenerator(propDis, t) * ampDecayByT * ampSupByDis * gAmplSupress;
            vec2 circles = normalize(v) * hIntense;

            vec3 norm = vec3(circles, hIntense);
            vec2 expandUV = (uv - intensity * norm.xy) * iResolution.xy;
            vec3 color = image.eval(expandUV).rgb;
            color += luminance * pow(clamp(dot(norm, normalize(lightDirect)), 0., 1.), highLightExp);
            
            return vec4(color, 1.0);
        }
    )";
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_WATER_RIPPLE_FILTER_H