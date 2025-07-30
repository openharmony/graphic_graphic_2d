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
#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_EFFECT_TEMPLATE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_EFFECT_TEMPLATE_H
#include <tuple>
#include <type_traits>

#include "effect/rs_render_property_tag.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

// forward declarations
namespace Drawing {
class GEVisualEffectContainer;
class GEVisualEffect;
} // namespace Drawing

class RSNGRenderMaskBase;

template <typename Derived, size_t EffectCountLimit = 1000>
class RSNGRenderEffectBase : public std::enable_shared_from_this<Derived> {
public:
    static constexpr size_t EFFECT_COUNT_LIMIT = EffectCountLimit;

    virtual ~RSNGRenderEffectBase() = default;
    virtual RSNGEffectType GetType() const = 0;
    virtual bool Marshalling(Parcel& parcel) const = 0;
    virtual void Attach(RSRenderNode& node, const std::weak_ptr<ModifierNG::RSRenderModifier>& modifier) = 0;
    virtual void Detach() = 0;
    virtual void SetModifierType(RSModifierType inType) = 0;
    virtual void Dump(std::string& out) const = 0;

    bool ContainsType(RSNGEffectType type)
    {
        auto current = this;
        while (current) {
            if (current->GetType() == type) {
                return true;
            }
        }
        current = current->nextEffect_.get();
        return false;
    }

protected:
    [[nodiscard]] virtual bool OnUnmarshalling(Parcel& parcel) = 0;

    virtual void DumpProperties(std::string& out) const {}

    size_t GetEffectCount() const
    {
        size_t count = 1;
        auto current = nextEffect_;
        while (current && count < EFFECT_COUNT_LIMIT) {
            count++;
            current = current->nextEffect_;
        }
        return count;
    }

    std::shared_ptr<Derived> nextEffect_ = nullptr;

    template <typename U, typename R>
    friend class RSNGEffectBase;

    template <typename U, RSNGEffectType T, typename... Tags>
    friend class RSNGEffectTemplate;
};

template <typename T>
struct is_render_property_tag : std::false_type {};

template <const char* Name, class PropertyType>
struct is_render_property_tag<RenderPropertyTagBase<Name, PropertyType>> : std::true_type {};

template <typename T>
inline constexpr bool is_render_property_tag_v = is_render_property_tag<T>::value;

template <typename Base, RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderEffectTemplate : public Base {
    static_assert(std::is_base_of_v<RSNGRenderEffectBase<Base>, Base>,
        "RSNGRenderEffectTemplate: Base must be a subclass of RSNGRenderEffectBase<Base>");
    static_assert(Type != RSNGEffectType::INVALID, "RSNGRenderEffectTemplate: Type cannot be INVALID");
    static_assert((is_render_property_tag_v<PropertyTags> && ...),
        "RSNGRenderEffectTemplate: All properties must be render property tags");

public:
    RSNGRenderEffectTemplate() = default;
    ~RSNGRenderEffectTemplate() override = default;
    RSNGRenderEffectTemplate(std::tuple<PropertyTags...> properties) noexcept : properties_(std::move(properties)) {}
    RSNGEffectType GetType() const override
    {
        return Type;
    }

    template<typename Tag>
    static constexpr bool Contains()
    {
        static_assert(is_render_property_tag_v<Tag>, "Tag must be a render property tag");
        return (std::is_same_v<Tag, PropertyTags> || ...);
    };

    template<typename Tag>
    constexpr const auto& Getter() const
    {
        static_assert(is_render_property_tag_v<Tag>, "Tag must be a render property tag");
        static_assert(sizeof...(PropertyTags) > 0, "Cannot call Getter: No properties are defined in this group.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        return std::get<Tag>(properties_).value_;
    }

    template<typename Tag>
    constexpr void Setter(typename Tag::ValueType value)
    {
        static_assert(is_render_property_tag_v<Tag>, "Tag must be a render property tag");
        static_assert(sizeof...(PropertyTags) > 0, "Cannot call Setter: No properties are defined in this group.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        return std::get<Tag>(properties_).value_->Set(value);
    }

    template<typename Tag>
    void Dump(std::string& out) const
    {
        static_assert(is_render_property_tag_v<Tag>, "Tag must be a render property tag");
        out += Tag::NAME;
        out += "[";
        Getter<Tag>()->Dump(out);
        out += "]";
    }

    bool Marshalling(Parcel& parcel) const override
    {
        auto count = Base::GetEffectCount();
        if (count > Base::EFFECT_COUNT_LIMIT) {
            return false;
        }

        if (!RSMarshallingHelper::Marshalling(parcel, static_cast<RSNGEffectTypeUnderlying>(Type))) {
            return false;
        }

        if (!std::apply(
            [&parcel](const auto&... propTag) {
                return (RSMarshallingHelper::Marshalling(parcel, propTag.value_) && ...);
            },
            properties_)) {
            return false;
        }

        if (Base::nextEffect_) {
            return Base::nextEffect_->Marshalling(parcel);
        }

        return RSMarshallingHelper::Marshalling(parcel, END_OF_CHAIN);
    }

    void Attach(RSRenderNode& node, const std::weak_ptr<ModifierNG::RSRenderModifier>& modifier) override
    {
        std::apply([&node, &modifier](const auto&... props) {
                (props.value_->Attach(node, modifier), ...);
            },
            properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Attach(node, modifier);
        }
    }

    void Detach() override
    {
        std::apply([](const auto&... props) { (props.value_->Detach(), ...); }, properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Detach();
        }
    }

    void SetModifierType(RSModifierType inType) override
    {
        std::apply(
            [&inType](const auto&... props) { (props.value_->SetModifierType(inType), ...); }, properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->SetModifierType(inType);
        }
    }

    void Dump(std::string& out) const override;

protected:
    [[nodiscard]] bool OnUnmarshalling(Parcel& parcel) override
    {
        // Type has been covered in Unmarshalling
        if (!std::apply(
            [&parcel](auto&... propTag) {
                return (RSMarshallingHelper::Unmarshalling(parcel, propTag.value_) && ...);
            },
            properties_)) {
            return false;
        }
        return true;
    }

    void DumpProperties(std::string& out) const override
    {
        std::string startStr = "[";
        std::string splitStr = ", ";
        std::string endStr = "]";

        out += startStr;
        bool first = true;

        auto dumpFunc = [&](auto& out, const auto& tag) {
            if (!first) out += splitStr;
            first = false;
            Dump<std::decay_t<decltype(tag)>>(out);
        };
        std::apply([&](const auto&... props) { (dumpFunc(out, props), ...); }, properties_);

        out += endStr;
    }

    std::tuple<PropertyTags...> properties_;

    template <typename U, typename R>
    friend class RSNGEffectBase;

    template <typename U, RSNGEffectType T, typename... Tags>
    friend class RSNGEffectTemplate;
};

class RSB_EXPORT RSNGRenderEffectHelper {
public:
    template<typename Tag>
    static void UpdateVisualEffectParam(std::shared_ptr<Drawing::GEVisualEffect> geFilter, const Tag& propTag)
    {
        if (!geFilter) {
            return;
        }
        UpdateVisualEffectParamImpl(*geFilter, Tag::NAME, propTag.value_->Get());
    }

    static std::string GetEffectTypeString(RSNGEffectType type)
    {
        switch (type) {
            case RSNGEffectType::INVALID: return "Invalid";
            case RSNGEffectType::NONE: return "None";
            case RSNGEffectType::BLUR: return "Blur";
            case RSNGEffectType::DISPLACEMENT_DISTORT: return "DispDistort";
            case RSNGEffectType::SOUND_WAVE: return "SoundWave";
            case RSNGEffectType::EDGE_LIGHT: return "EdgeLight";
            case RSNGEffectType::DISPERSION: return "Dispersion";
            case RSNGEffectType::DIRECTION_LIGHT: return "DirectionLight";
            case RSNGEffectType::BEZIER_WARP: return "BezierWarp";
            case RSNGEffectType::COLOR_GRADIENT: return "ColorGradient";
            case RSNGEffectType::RIPPLE_MASK: return "RippleMask";
            case RSNGEffectType::DOUBLE_RIPPLE_MASK: return "DoubleRippleMask";
            case RSNGEffectType::PIXEL_MAP_MASK: return "PixelMapMask";
            case RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT: return "ContourDiagonalFlowLight";
            case RSNGEffectType::WAVY_RIPPLE_LIGHT: return "WavyRippleLight";
            case RSNGEffectType::AURORA_NOISE: return "AuroraNoise";
            case RSNGEffectType::PARTICLE_CIRCULAR_HALO: return "ParticleCircularHalo";
            case RSNGEffectType::RADIAL_GRADIENT_MASK: return "RadialGradientMask";
            case RSNGEffectType::WAVE_GRADIENT_MASK: return "WaveGradientMask";
            case RSNGEffectType::MASK_TRANSITION: return "MaskTransition";
            case RSNGEffectType::VARIABLE_RADIUS_BLUR: return "VariableRadiusBlur";
            case RSNGEffectType::CONTENT_LIGHT: return "ContentLight";
            case RSNGEffectType::BORDER_LIGHT: return "BorderLight";
            default:
                return "UNKNOWN";
        }
    }

    static std::shared_ptr<Drawing::GEVisualEffect> CreateGEVisualEffect(RSNGEffectType type);
    static void AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge,
        std::shared_ptr<Drawing::GEVisualEffect> geShader);

private:
    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, float value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, bool value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, const Vector4f& value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, const Vector3f& value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, const Vector2f& value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, std::shared_ptr<RSNGRenderMaskBase> value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, const VectorVector2F& value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, std::shared_ptr<Media::PixelMap> value);

    static void UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
        const std::string& desc, const std::vector<float>& value);
};

template <typename Base, RSNGEffectType Type, typename... PropertyTags>
void RSNGRenderEffectTemplate<Base, Type, PropertyTags...>::Dump(std::string& out) const
{
    std::string descStr = ": ";
    std::string splitStr = "--";

    out += RSNGRenderEffectHelper::GetEffectTypeString(GetType());
    out += descStr;
    DumpProperties(out);
    if (Base::nextEffect_) {
        out += splitStr;
        Base::nextEffect_->Dump(out);
    }
}

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_EFFECT_TEMPLATE_H
