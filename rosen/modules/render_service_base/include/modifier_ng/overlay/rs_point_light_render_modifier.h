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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_OVERLAY_RS_POINT_LIGHT_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_OVERLAY_RS_POINT_LIGHT_RENDER_MODIFIER_H

#include "effect/runtime_shader_builder.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen::ModifierNG {
namespace {
constexpr int MAX_LIGHT_SOURCES = 12;
}

class RSB_EXPORT RSPointLightRenderModifier : public RSDisplayListRenderModifier {
public:
    RSPointLightRenderModifier() = default;
    ~RSPointLightRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::POINT_LIGHT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;

    void ResetProperties(RSProperties& properties) override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    bool OnApply(RSModifierContext& context) override;

    void OnSync() override;

    std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
    std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;

    RRect rrect_ = {};

    // TODO: we don't need to declare these staging values actually as we can extact them from properties_
    RectI stagingRect_ = {};
    IlluminatedType stagingIlluminatedType_ = IlluminatedType::INVALID;
    float stagingBorderWidth_ { 0.0f };
    float stagingBloomIntensity_ { 0.0f };

    std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>> renderLightSourcesAndPosVec_;
    RectI renderRect_ = {};
    IlluminatedType renderIlluminatedType_ = IlluminatedType::INVALID;
    Drawing::RoundRect renderBorderRRect_ = {};
    Drawing::RoundRect renderContentRRect_ = {};
    float renderBorderWidth_ { 0.0f };
    float renderBloomIntensity_ { 0.0f };

    void DrawLight(Drawing::Canvas* canvas) const;
    static const std::shared_ptr<Drawing::RuntimeShaderBuilder>& GetPhongShaderBuilder();
    void DrawContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_OVERLAY_RS_POINT_LIGHT_RENDER_MODIFIER_H
