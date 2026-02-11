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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_FILTER_BASE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_FILTER_BASE_H

#include "draw/canvas.h"
#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSNGRenderFilterBase
 *
 * @brief The base class for filter, defines interfaces for ng filters.
 *
 * This class provides a common interface for all ng filters.
 */
class RSB_EXPORT RSNGRenderFilterBase : public RSNGRenderEffectBase<RSNGRenderFilterBase> {
public:
    ~RSNGRenderFilterBase() override = default;

    static std::shared_ptr<RSNGRenderFilterBase> Create(RSNGEffectType type);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val);

    virtual void GenerateGEVisualEffect() {}

    virtual void OnSync() {}

protected:
    std::shared_ptr<Drawing::GEVisualEffect> geFilter_;
    static void UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect>& src,
                                std::shared_ptr<Drawing::GEVisualEffect>& dest);

private:
    friend class RSNGFilterBase;
    friend class RSNGRenderFilterHelper;
};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderFilterTemplate :
    public RSNGRenderEffectTemplate<RSNGRenderFilterBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderFilterBase, Type, PropertyTags...>;

    RSNGRenderFilterTemplate() : EffectTemplateBase() {}
    ~RSNGRenderFilterTemplate() override = default;
    RSNGRenderFilterTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    void GenerateGEVisualEffect() override
    {
        RS_OPTIONAL_TRACE_FMT("RSRenderFilter, Type: %s paramStr: %s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str(),
            EffectTemplateBase::DumpProperties().c_str());
        auto geFilter = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        OnGenerateGEVisualEffect(geFilter);
        std::apply(
            [&geFilter](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(geFilter, propTag),
                    ...);
            },
            this->EffectTemplateBase::properties_);
        RSNGRenderFilterBase::UpdateCacheData(RSNGRenderFilterBase::geFilter_, geFilter);
        RSNGRenderFilterBase::geFilter_ = std::move(geFilter);

        if (EffectTemplateBase::nextEffect_) {
            EffectTemplateBase::nextEffect_->GenerateGEVisualEffect();
        }
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};

class RSNGRenderFilterHelper {
public:
    template<RSNGEffectType Type, typename... PropertyTags>
    static std::shared_ptr<RSNGRenderFilterTemplate<Type, PropertyTags...>> CreateRenderFilterByTuple(
        const std::tuple<PropertyTags...>& properties)
    {
        return std::make_shared<RSNGRenderFilterTemplate<Type, PropertyTags...>>(properties);
    }

    static void GenerateGEVisualEffect(std::shared_ptr<RSNGRenderFilterBase>& filter);

    static void UpdateToGEContainer(std::shared_ptr<RSNGRenderFilterBase>& filter,
        std::shared_ptr<Drawing::GEVisualEffectContainer>& container);

    static bool CheckEnableEDR(std::shared_ptr<RSNGRenderFilterBase>& filter);
    
    static void UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect>& src,
                                std::shared_ptr<Drawing::GEVisualEffect>& target);
    
    static void SetRotationAngle(std::shared_ptr<RSNGRenderFilterBase>& filter,
        const Vector3f& rotationAngle);

    static RectF CalcRect(const std::shared_ptr<RSNGRenderFilterBase>& filter, const RectF& bound,
        EffectRectType rectType);
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag
#define DECLARE_FILTER(FilterName, FilterType, ...) \
    using RSNGRender##FilterName##Filter = RSNGRenderFilterTemplate<RSNGEffectType::FilterType, __VA_ARGS__>

#include "effect/rs_render_filter_def.in"

// Note: if the definition is inconsistent with the client, place it here instead of in the .in file.
DECLARE_FILTER(ColorGradient, COLOR_GRADIENT,
    ADD_PROPERTY_TAG(ColorGradient, Colors),
    ADD_PROPERTY_TAG(ColorGradient, Positions),
    ADD_PROPERTY_TAG(ColorGradient, Strengths),
    ADD_PROPERTY_TAG(ColorGradient, Mask)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_FILTER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_FILTER_BASE_H
