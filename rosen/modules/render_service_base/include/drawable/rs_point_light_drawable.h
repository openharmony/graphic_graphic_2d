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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_POINT_LIGHT_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_POINT_LIGHT_DRAWABLE_H
#include "common/rs_rect.h"
#include "common/rs_vector4.h"
#include "drawable/rs_property_drawable.h"
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
class RSPointLightDrawable : public RSDrawable {
public:
    RSPointLightDrawable() = default;
    ~RSPointLightDrawable() override = default;
    void OnSync() override;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;
    bool GetEnableEDR() const override
    {
        return stagingEnableEDREffect_;
    }

private:
    std::vector<std::pair<RSLightSource, Vector4f>> lightSourcesAndPosVec_;
    std::vector<std::pair<RSLightSource, Vector4f>> stagingLightSourcesAndPosVec_;
    IlluminatedType illuminatedType_ = IlluminatedType::INVALID;
    IlluminatedType stagingIlluminatedType_ = IlluminatedType::INVALID;
    float borderWidth_ = 0.0f;
    float stagingBorderWidth_ = 0.0f;
    NodeId screenNodeId_ = INVALID_NODEID;
    NodeId stagingScreenNodeId_ = INVALID_NODEID;
    NodeId nodeId_ = INVALID_NODEID;
    NodeId stagingNodeId_ = INVALID_NODEID;
    RRect stagingRRect_ = {};
    bool enableEDREffect_ = false;
    bool stagingEnableEDREffect_ = false;
    std::shared_ptr<Drawing::ShaderEffect> stagingSDFShaderEffect_;
    std::shared_ptr<Drawing::ShaderEffect> sdfShaderEffect_;

    Drawing::RoundRect borderRRect_ = {};
    Drawing::RoundRect contentRRect_ = {};

    bool needSync_ = false;
    float displayHeadroom_ = 0.0f;
    void DrawLight(Drawing::Canvas* canvas) const;
    void ProcessLightSourcesData(std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> CreateShaderBuilder() const;
    void DrawLightByIlluminatedType(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
        const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPhongShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFeatheringBorderLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetNormalLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFBorderLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFContentLightShaderBuilder();

    static float GetBrightnessMapping(float headroom, float input);
    static bool NeedToneMapping(float supportHeadroom);
    static std::optional<float> CalcBezierResultY(
        const Vector2f& start, const Vector2f& end, const Vector2f& control, float input);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeFeatheringBoardLightShaderBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeNormalLightShaderBuilder() const;
    void DrawContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    bool DrawSDFContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect,
        Drawing::Brush& brush) const;
    void DrawBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    bool DrawSDFBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect) const;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif