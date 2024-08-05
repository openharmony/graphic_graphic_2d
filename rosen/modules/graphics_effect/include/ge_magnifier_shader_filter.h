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
#ifndef GRAPHICS_EFFECT_GE_MAGNIFIER_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_MAGNIFIER_SHADER_FILTER_H

#include <cstdint>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

class GEMagnifierParams {
public:
    explicit GEMagnifierParams() {}
    ~GEMagnifierParams() = default;

    float factor_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float cornerRadius_ = 0.f;
    float borderWidth_ = 0.f;

    float shadowOffsetX_ = 0.f;
    float shadowOffsetY_ = 0.f;
    float shadowSize_ = 0.f;
    float shadowStrength_ = 0.f;

    // rgba
    uint32_t gradientMaskColor1_ = 0x00000000;
    uint32_t gradientMaskColor2_ = 0x00000000;
    uint32_t outerContourColor1_ = 0x00000000;
    uint32_t outerContourColor2_ = 0x00000000;

    int32_t rotateDegree_ = 0;
};

class GEMagnifierShaderFilter : public GEShaderFilter {
public:
    GEMagnifierShaderFilter(const Drawing::GEMagnifierShaderFilterParams& params);
    GEMagnifierShaderFilter(const GEMagnifierShaderFilter&) = delete;
    GEMagnifierShaderFilter operator=(const GEMagnifierShaderFilter&) = delete;
    ~GEMagnifierShaderFilter() override = default;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string GetDescription() const;

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeMagnifierShader(
        std::shared_ptr<Drawing::ShaderEffect> imageShader, float imageWidth, float imageHeight);
    bool InitMagnifierEffect();
    void ConvertToRgba(uint32_t rgba, float* color, int tupleSize);

    std::shared_ptr<GEMagnifierParams> magnifierPara_ = nullptr;
    static std::shared_ptr<Drawing::RuntimeEffect> g_magnifierShaderEffect;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_MAGNIFIER_SHADER_FILTER_H
