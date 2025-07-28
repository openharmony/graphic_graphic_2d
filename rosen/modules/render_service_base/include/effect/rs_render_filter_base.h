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
#include "effect/runtime_shader_builder.h"
#include "render/rs_filter.h"

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
    static std::shared_ptr<RSNGRenderFilterBase> Create(RSNGEffectType type);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val);

    virtual void GenerateGEVisualEffect() {};

    virtual void OnSync()
    {
        GenerateGEVisualEffect();
    }

protected:
    std::shared_ptr<Drawing::GEVisualEffect> geFilter_;
    static void UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect> src,
                                std::shared_ptr<Drawing::GEVisualEffect> dest);
 

private:
    friend class RSNGFilterBase;
    friend class RSUIFilterHelper;
};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderFilterTemplate : public RSNGRenderEffectTemplate<RSNGRenderFilterBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderFilterBase, Type, PropertyTags...>;

    RSNGRenderFilterTemplate() : EffectTemplateBase() {}
    ~RSNGRenderFilterTemplate() override = default;
    RSNGRenderFilterTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    void GenerateGEVisualEffect() override
    {
        auto geFilter = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        OnGenerateGEVisualEffect(geFilter);
        std::apply([&geFilter](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geFilter, propTag), ...);
            }, EffectTemplateBase::properties_);
        RSNGRenderFilterBase::UpdateCacheData(RSNGRenderFilterBase::geFilter_, geFilter);
        RSNGRenderFilterBase::geFilter_ = std::move(geFilter);

        if (EffectTemplateBase::nextEffect_) {
            EffectTemplateBase::nextEffect_->GenerateGEVisualEffect();
        }
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};

class RSB_EXPORT RSUIFilterHelper {
public:
    template<RSNGEffectType Type, typename... PropertyTags>
    static std::shared_ptr<RSNGRenderFilterTemplate<Type, PropertyTags...>> CreateRenderFilterByTuple(
        const std::tuple<PropertyTags...>& properties)
    {
        return std::make_shared<RSNGRenderFilterTemplate<Type, PropertyTags...>>(properties);
    }

    static void UpdateToGEContainer(std::shared_ptr<RSNGRenderFilterBase> filter,
        std::shared_ptr<Drawing::GEVisualEffectContainer> container);

    static void SetGeometry(std::shared_ptr<RSNGRenderFilterBase> filter,
        const Drawing::Canvas& canvas, float geoWidth, float geoHeight);

    static void UpdateCacheData(std::shared_ptr<Drawing::GEVisualEffect> src,
                                std::shared_ptr<Drawing::GEVisualEffect> target);
    
    static void SetRotationAngle(std::shared_ptr<RSNGRenderFilterBase> filter,
        const Vector3f& rotationAngle);
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag

#define DECLARE_FILTER(FilterName, FilterType, ...) \
    using RSNGRender##FilterName##Filter = RSNGRenderFilterTemplate<RSNGEffectType::FilterType, __VA_ARGS__>

DECLARE_FILTER(Blur, BLUR,
    ADD_PROPERTY_TAG(Blur, RadiusX),
    ADD_PROPERTY_TAG(Blur, RadiusY)
);

DECLARE_FILTER(DispDistort, DISPLACEMENT_DISTORT,
    ADD_PROPERTY_TAG(DispDistort, Factor),
    ADD_PROPERTY_TAG(DispDistort, Mask)
);

DECLARE_FILTER(SoundWave, SOUND_WAVE,
    ADD_PROPERTY_TAG(SoundWave, ColorA),
    ADD_PROPERTY_TAG(SoundWave, ColorB),
    ADD_PROPERTY_TAG(SoundWave, ColorC),
    ADD_PROPERTY_TAG(SoundWave, ColorProgress),
    ADD_PROPERTY_TAG(SoundWave, CenterBrightness),
    ADD_PROPERTY_TAG(SoundWave, Intensity),
    ADD_PROPERTY_TAG(SoundWave, AlphaA),
    ADD_PROPERTY_TAG(SoundWave, AlphaB),
    ADD_PROPERTY_TAG(SoundWave, ProgressA),
    ADD_PROPERTY_TAG(SoundWave, ProgressB)
);

DECLARE_FILTER(EdgeLight, EDGE_LIGHT,
    ADD_PROPERTY_TAG(EdgeLight, Color),
    ADD_PROPERTY_TAG(EdgeLight, Alpha),
    ADD_PROPERTY_TAG(EdgeLight, Mask),
    ADD_PROPERTY_TAG(EdgeLight, Bloom),
    ADD_PROPERTY_TAG(EdgeLight, UseRawColor)
);

DECLARE_FILTER(Dispersion, DISPERSION,
    ADD_PROPERTY_TAG(Dispersion, Mask),
    ADD_PROPERTY_TAG(Dispersion, Opacity),
    ADD_PROPERTY_TAG(Dispersion, RedOffset),
    ADD_PROPERTY_TAG(Dispersion, GreenOffset),
    ADD_PROPERTY_TAG(Dispersion, BlueOffset)
);

DECLARE_FILTER(ColorGradient, COLOR_GRADIENT,
    ADD_PROPERTY_TAG(ColorGradient, Colors),
    ADD_PROPERTY_TAG(ColorGradient, Positions),
    ADD_PROPERTY_TAG(ColorGradient, Strengths),
    ADD_PROPERTY_TAG(ColorGradient, Mask)
);

DECLARE_FILTER(BezierWarp, BEZIER_WARP,
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint0),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint1),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint2),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint3),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint4),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint5),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint6),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint7),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint8),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint9),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint10),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint11)
);

DECLARE_FILTER(DirectionLight, DIRECTION_LIGHT,
    ADD_PROPERTY_TAG(DirectionLight, Mask),
    ADD_PROPERTY_TAG(DirectionLight, Factor),
    ADD_PROPERTY_TAG(DirectionLight, Direction),
    ADD_PROPERTY_TAG(DirectionLight, Color),
    ADD_PROPERTY_TAG(DirectionLight, Intensity)
);

DECLARE_FILTER(MaskTransition, MASK_TRANSITION,
    ADD_PROPERTY_TAG(MaskTransition, Mask),
    ADD_PROPERTY_TAG(MaskTransition, Factor),
    ADD_PROPERTY_TAG(MaskTransition, Inverse)
);

DECLARE_FILTER(VariableRadiusBlur, VARIABLE_RADIUS_BLUR,
    ADD_PROPERTY_TAG(VariableRadiusBlur, Radius),
    ADD_PROPERTY_TAG(VariableRadiusBlur, Mask)
);

DECLARE_FILTER(ContentLight, CONTENT_LIGHT,
    ADD_PROPERTY_TAG(ContentLight, Position),
    ADD_PROPERTY_TAG(ContentLight, Color),
    ADD_PROPERTY_TAG(ContentLight, Intensity),
    ADD_PROPERTY_TAG(ContentLight, RotationAngle)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_FILTER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_FILTER_BASE_H
