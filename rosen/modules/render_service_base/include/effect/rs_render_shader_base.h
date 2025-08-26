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

    virtual void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge) {};

    virtual void OnSync()
    {
    }

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
        std::apply(
            [&geShader](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geShader, propTag), ...);
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

class RSNGRenderShaderHelper {
public:
    static bool CheckEnableEDR(std::shared_ptr<RSNGRenderShaderBase> shader);

    static void SetRotationAngle(std::shared_ptr<RSNGRenderShaderBase> shader,
        const Vector3f& rotationAngle);

    static void SetCornerRadius(std::shared_ptr<RSNGRenderShaderBase> shader,
        float cornerRadius);
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag
#define DECLARE_SHADER(ShaderName, ShaderType, ...) \
    using RSNGRender##ShaderName = RSNGRenderShaderTemplate<RSNGEffectType::ShaderType, __VA_ARGS__>; \
    extern template class PROPERTY_EXPORT RSNGRenderShaderTemplate<RSNGEffectType::ShaderType, __VA_ARGS__>

#include "effect/rs_render_shader_def.in"

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHADER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_MASK_BASE_H
