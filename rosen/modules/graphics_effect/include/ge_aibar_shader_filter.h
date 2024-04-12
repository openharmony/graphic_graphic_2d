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
#ifndef GRAPHICS_EFFECT_GE_AIBAR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_AIBAR_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT GEAIBarShaderFilter : public GEShaderFilter {
public:
    GEAIBarShaderFilter(const Drawing::GEAIBarShaderFilterParams& params)
        : aiBarLow_(params.aiBarLow), aiBarHigh_(params.aiBarHigh), aiBarThreshold_(params.aiBarThreshold),
          aiBarOpacity_(params.aiBarOpacity), aiBarSaturation_(params.aiBarSaturation)
    {}
    GEAIBarShaderFilter(const GEAIBarShaderFilter&) = delete;
    GEAIBarShaderFilter operator=(const GEAIBarShaderFilter&) = delete;
    ~GEAIBarShaderFilter() override;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    std::string GetDescription();

private:
    float aiBarLow_;
    float aiBarHigh_;
    float aiBarThreshold_;
    float aiBarOpacity_;
    float aiBarSaturation_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeBinarizationShader(
        float imageWidth, float imageHeight, std::shared_ptr<Drawing::ShaderEffect> imageShader);
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_AIBAR_SHADER_FILTER_H
