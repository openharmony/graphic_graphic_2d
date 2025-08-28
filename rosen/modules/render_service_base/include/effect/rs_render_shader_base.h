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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHADER_BASE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHADER_BASE_H

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
    class GEVisualEffectContainer;
    class GEVisualEffect;
    class GEShader;
} // namespace Drawing

class RSB_EXPORT RSNGRenderShaderBase : public RSNGRenderEffectBase<RSNGRenderShaderBase> {
public:
    static std::shared_ptr<RSNGRenderShaderBase> Create(RSNGEffectType type);
    virtual void GenerateGEVisualEffect() {}

    virtual void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge) {}

    virtual void OnSync() {}

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShaderBase>& val);
    void Dump(std::string& out) const;

private:
    friend class RSNGRenderShaderHelper;
};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderShaderTemplate : public RSNGRenderEffectTemplate<RSNGRenderShaderBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderShaderBase, Type, PropertyTags...>;

    RSNGRenderShaderTemplate() : EffectTemplateBase() {}
    ~RSNGRenderShaderTemplate() override = default;
    RSNGRenderShaderTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge) override
    {
        if (ge == nullptr) {
            return;
        }
        RS_OPTIONAL_TRACE_FMT("RSNGRenderShaderTemplate::AppendToGEContainer, Type: %s paramStr: %s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str(),
            EffectTemplateBase::DumpProperties().c_str());
        auto geShader = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        std::apply([&geShader](const auto&... propTag) {
            (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(geShader, propTag), ...);
            },
            EffectTemplateBase::properties_);
        RSNGRenderEffectHelper::AppendToGEContainer(ge, geShader);
        if (EffectTemplateBase::nextEffect_) {
            EffectTemplateBase::nextEffect_->AppendToGEContainer(ge);
        }
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};

class RSB_EXPORT RSNGRenderShaderHelper {
public:
    static bool CheckEnableEDR(std::shared_ptr<RSNGRenderShaderBase>& shader);

    static void SetRotationAngle(std::shared_ptr<RSNGRenderShaderBase>& shader,
        const Vector3f& rotationAngle);

    static void SetCornerRadius(std::shared_ptr<RSNGRenderShaderBase>& shader,
        float cornerRadius);
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag

#define DECLARE_SHADER(ShaderName, ShaderType, ...) \
    using RSNGRender##ShaderName = RSNGRenderShaderTemplate<RSNGEffectType::ShaderType, __VA_ARGS__>

DECLARE_SHADER(ContourDiagonalFlowLight, CONTOUR_DIAGONAL_FLOW_LIGHT,
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Contour),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line1Start),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line1Length),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line1Color),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line2Start),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line2Length),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line2Color),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Thickness),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, HaloRadius),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, LightWeight),
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, HaloWeight)
);

DECLARE_SHADER(WavyRippleLight, WAVY_RIPPLE_LIGHT,
    ADD_PROPERTY_TAG(WavyRippleLight, Center),
    ADD_PROPERTY_TAG(WavyRippleLight, Radius),
    ADD_PROPERTY_TAG(WavyRippleLight, Thickness)
);

DECLARE_SHADER(AuroraNoise, AURORA_NOISE,
    ADD_PROPERTY_TAG(AuroraNoise, Noise)
);

DECLARE_SHADER(ParticleCircularHalo, PARTICLE_CIRCULAR_HALO,
    ADD_PROPERTY_TAG(ParticleCircularHalo, Center),
    ADD_PROPERTY_TAG(ParticleCircularHalo, Radius),
    ADD_PROPERTY_TAG(ParticleCircularHalo, Noise)
);

DECLARE_SHADER(LightCave, LIGHT_CAVE,
    ADD_PROPERTY_TAG(LightCave, ColorA),
    ADD_PROPERTY_TAG(LightCave, ColorB),
    ADD_PROPERTY_TAG(LightCave, ColorC),
    ADD_PROPERTY_TAG(LightCave, Position),
    ADD_PROPERTY_TAG(LightCave, RadiusXY),
    ADD_PROPERTY_TAG(LightCave, Progress)
);

DECLARE_SHADER(BorderLight, BORDER_LIGHT,
    ADD_PROPERTY_TAG(BorderLight, Position),
    ADD_PROPERTY_TAG(BorderLight, Color),
    ADD_PROPERTY_TAG(BorderLight, Intensity),
    ADD_PROPERTY_TAG(BorderLight, Width),
    ADD_PROPERTY_TAG(BorderLight, RotationAngle),
    ADD_PROPERTY_TAG(BorderLight, CornerRadius)
);

DECLARE_SHADER(ColorGradientEffect, COLOR_GRADIENT_EFFECT,
    ADD_PROPERTY_TAG(ColorGradientEffect, Color0),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color1),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color2),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color3),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color4),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color5),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color6),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color7),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color8),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color9),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color10),
    ADD_PROPERTY_TAG(ColorGradientEffect, Color11),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position0),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position1),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position2),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position3),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position4),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position5),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position6),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position7),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position8),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position9),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position10),
    ADD_PROPERTY_TAG(ColorGradientEffect, Position11),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength0),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength1),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength2),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength3),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength4),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength5),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength6),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength7),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength8),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength9),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength10),
    ADD_PROPERTY_TAG(ColorGradientEffect, Strength11),
    ADD_PROPERTY_TAG(ColorGradientEffect, Mask),
    ADD_PROPERTY_TAG(ColorGradientEffect, ColorNumber),
    ADD_PROPERTY_TAG(ColorGradientEffect, Blend),
    ADD_PROPERTY_TAG(ColorGradientEffect, BlendK)
);

DECLARE_SHADER(AIBarGlow, AIBAR_GLOW,
    ADD_PROPERTY_TAG(AIBarGlow, LTWH),
    ADD_PROPERTY_TAG(AIBarGlow, StretchFactor),
    ADD_PROPERTY_TAG(AIBarGlow, BarAngle),
    ADD_PROPERTY_TAG(AIBarGlow, Color0),
    ADD_PROPERTY_TAG(AIBarGlow, Color1),
    ADD_PROPERTY_TAG(AIBarGlow, Color2),
    ADD_PROPERTY_TAG(AIBarGlow, Color3),
    ADD_PROPERTY_TAG(AIBarGlow, Position0),
    ADD_PROPERTY_TAG(AIBarGlow, Position1),
    ADD_PROPERTY_TAG(AIBarGlow, Position2),
    ADD_PROPERTY_TAG(AIBarGlow, Position3),
    ADD_PROPERTY_TAG(AIBarGlow, Strength),
    ADD_PROPERTY_TAG(AIBarGlow, Brightness),
    ADD_PROPERTY_TAG(AIBarGlow, Progress)
);

DECLARE_SHADER(RoundedRectFlowlight, ROUNDED_RECT_FLOWLIGHT,
    ADD_PROPERTY_TAG(RoundedRectFlowlight, StartEndPosition),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, WaveLength),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, WaveTop),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, CornerRadius),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Brightness),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Scale),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Sharping),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Feathering),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, FeatheringBezierControlPoints),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, GradientBezierControlPoints),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Color),
    ADD_PROPERTY_TAG(RoundedRectFlowlight, Progress)
);

DECLARE_SHADER(GradientFlowColors, GRADIENT_FLOW_COLORS,
    ADD_PROPERTY_TAG(GradientFlowColors, Color0),
    ADD_PROPERTY_TAG(GradientFlowColors, Color1),
    ADD_PROPERTY_TAG(GradientFlowColors, Color2),
    ADD_PROPERTY_TAG(GradientFlowColors, Color3),
    ADD_PROPERTY_TAG(GradientFlowColors, GradientBegin),
    ADD_PROPERTY_TAG(GradientFlowColors, GradientEnd),
    ADD_PROPERTY_TAG(GradientFlowColors, EffectAlpha),
    ADD_PROPERTY_TAG(GradientFlowColors, Progress)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHADER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_MASK_BASE_H
