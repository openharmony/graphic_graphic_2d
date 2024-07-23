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
    bool InitWaterRippleEffect();
    bool InitWaterRipplePcEffect();
    float progress_ = 0.0f;
    float waveCount_ = 2.0f;
    float rippleCenterX_ = 0.5f;
    float rippleCenterY_ = 0.7f;
    inline static std::string shaderString = R"(
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

    inline static std::string shaderPcString = R"(
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
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_WATER_RIPPLE_FILTER_H