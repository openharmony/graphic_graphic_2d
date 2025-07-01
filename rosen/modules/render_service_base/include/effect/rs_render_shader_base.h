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

#include "effect/rs_render_property_tag.h"
#include "effect/rs_render_effect_template.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
    class GEVisualEffectContainer;
    class GEVisualEffect;
    class GEShader;
} // namespace Drawing

class RSB_EXPORT RSNGRenderShaderHelper {
public:
    static std::shared_ptr<Drawing::GEVisualEffect> CreateGEFilter(RSNGEffectType type);
    static void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge,
        std::shared_ptr<Drawing::GEVisualEffect> geShader);

    static std::string GetShaderTypeString(RSNGEffectType type)
    {
        switch (type) {
            case RSNGEffectType::INVALID: return "Invalid";
            case RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT: return "ContourDiagonalFlowLight";
            case RSNGEffectType::WAVY_RIPPLE_LIGHT: return "WavyRippleLight";
            case RSNGEffectType::AURORA_NOISE: return "AuroraNoise";
            default:
                return "UNKNOWN";	                return "UNKNOWN";
        }
    }

    template<typename Tag>
    static void UpdateVisualEffectParam(std::shared_ptr<Drawing::GEVisualEffect> geFilter, const Tag& propTag)
    {
        if (!geFilter) {
            return;
        }
        UpdateVisualEffectParamImpl(geFilter, Tag::NAME, propTag.value_->Get());
    }

private:
    static void UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
        const std::string& desc, float value);

    static void UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
        const std::string& desc, const Vector4f& value);

    static void UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
        const std::string& desc, const Vector2f& value);

    static void UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
        const std::string& desc, std::shared_ptr<RSPath> value);

    static void UpdateVisualEffectParamImpl(std::shared_ptr<Drawing::GEVisualEffect> geFilter,
        const std::string& desc, std::shared_ptr<RSNGRenderMaskBase> value);
};

class RSB_EXPORT RSNGRenderShaderBase : public RSNGRenderEffectBase<RSNGRenderShaderBase> {
public:
    static std::shared_ptr<RSNGRenderShaderBase> Create(RSNGEffectType type);

    virtual void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge) {};

    virtual void OnSync()
    {
    }

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShaderBase>& val);
    void Dump(std::string& out) const;
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
        auto geShader = RSNGRenderShaderHelper::CreateGEFilter(Type);
        std::apply(
            [&geShader](const auto&... propTag) {
                (RSNGRenderShaderHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geShader, propTag), ...);
                },
                EffectTemplateBase::properties_);
        RSNGRenderShaderHelper::AppendToGEContainer(ge, geShader);
        if (EffectTemplateBase::nextEffect_) {
            EffectTemplateBase::nextEffect_->AppendToGEContainer(ge);
        }
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
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
    ADD_PROPERTY_TAG(ContourDiagonalFlowLight, Line2Color)
);

DECLARE_SHADER(WavyRippleLight, WAVY_RIPPLE_LIGHT,
    ADD_PROPERTY_TAG(WavyRippleLight, Center),
    ADD_PROPERTY_TAG(WavyRippleLight, Radius)
);

DECLARE_SHADER(AuroraNoise, AURORA_NOISE,
    ADD_PROPERTY_TAG(AuroraNoise, Noise)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHADER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_MASK_BASE_H
