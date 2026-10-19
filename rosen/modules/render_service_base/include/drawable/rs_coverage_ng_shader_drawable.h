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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_COVERAGE_NG_SHADER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_COVERAGE_NG_SHADER_DRAWABLE_H
#include "common/rs_rect.h"
#include "common/rs_vector4.h"
#include "drawable/rs_shader_drawable.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
namespace Drawing {
class RuntimeShaderBuilder;
}
namespace DrawableV2 {
namespace {
constexpr int MAX_LIGHT_SOURCES = 12;
}
class RSCoverageNGShaderDrawable : public RSShaderDrawable {
public:
    RSCoverageNGShaderDrawable() = default;
    ~RSCoverageNGShaderDrawable() override = default;
    void OnSync() override;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    // Hybrid update: the shader branch delegates to the base template, the light branch stages
    // the illuminated state when no coverage shader is set.
    bool OnUpdate(const RSRenderNode& node) override;
    // Hybrid draw: the shader mode reuses the base OnDraw, the light mode falls back to
    // DrawLight when no coverage shader container has been published.
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;
protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;

private:
    std::vector<std::pair<RSLightSource, Vector4f>> lightSourcesAndPosVec_;
    std::vector<std::pair<RSLightSource, Vector4f>> stagingLightSourcesAndPosVec_;
    IlluminatedType illuminatedType_ = IlluminatedType::INVALID;
    IlluminatedType stagingIlluminatedType_ = IlluminatedType::INVALID;
    float borderWidth_ = 0.0f;
    float stagingBorderWidth_ = 0.0f;
    RRect stagingRRect_ = {};
    std::shared_ptr<Drawing::ShaderEffect> stagingSDFShaderEffect_;
    std::shared_ptr<Drawing::ShaderEffect> sdfShaderEffect_;
    Drawing::Rect sdfDrawRect_;
    Drawing::Rect stagingSdfDrawRect_;

    Drawing::RoundRect borderRRect_ = {};
    Drawing::RoundRect contentRRect_ = {};

    float displayHeadroom_ = 0.0f;

    // Stage the illuminated (point light) state: light sources, type, border width, rrect,
    // SDF shader effect and the EDR pair. Returns false when the illuminated state is invalid.
    bool StageLightProperties(const RSRenderNode& node, const RSProperties& properties);

    // Publish the staged illuminated (point light) state to RT members (sorted light sources,
    // type, border rrects, SDF shader effect and the EDR pair with headroom) and reset needSync_
    // when served; with no sync request the light sources are cleared and nothing is published.
    void SyncLightProperties();

    void DrawLight(Drawing::Canvas* canvas) const;
    void ProcessLightSourcesData(std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> CreateShaderBuilder(size_t lightLength) const;
    void DrawLightByIlluminatedType(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
        const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPhongShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFeatheringBorderLightShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetNormalLightShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFBorderLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFContentLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFContentAndBorderLightShaderBuilder();

    static float GetBrightnessMapping(float headroom, float input);
    static bool NeedToneMapping(float supportHeadroom);
    static std::optional<float> CalcBezierResultY(
        const Vector2f& start, const Vector2f& end, const Vector2f& control, float input);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeFeatheringBoardLightShaderBuilder(size_t lightLength) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeNormalLightShaderBuilder(size_t lightLength) const;
    void DrawContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    bool DrawSDFContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect,
        Drawing::Brush& brush) const;
    void DrawBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    bool DrawSDFBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect) const;

    void DrawContentAndBorderLight(Drawing::Canvas& canvas,
 	         std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
 	         const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawSDFContentAndBorderLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffectContent,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const;

    void ProcessSingleLightSourcesData(std::array<float, 1>& lightIntensityArray,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    void DrawSingleLightByIlluminatedType(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder, const std::array<float, 1>& lightIntensityArray) const;
    void DrawSingleContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, 1>& lightIntensityArray) const;
    void DrawSingleBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, 1>& lightIntensityArray) const;
    void DrawSingleContentAndBorderLight(Drawing::Canvas& canvas,
 	    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
 	    const std::array<float, 1>& lightIntensityArray) const;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif