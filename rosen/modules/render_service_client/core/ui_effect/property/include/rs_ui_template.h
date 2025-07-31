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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_TEMPLATE_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_TEMPLATE_H
#include <tuple>
#include <type_traits>

#include "ui_effect/property/include/rs_ui_property_tag.h"

#include "effect/rs_render_effect_template.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

template <typename Derived, typename RenderEffect>
class RSNGEffectBase : public std::enable_shared_from_this<Derived> {
public:
    using RenderEffectBase = RenderEffect;

    virtual ~RSNGEffectBase() = default;
    virtual RSNGEffectType GetType() const = 0;
    virtual std::shared_ptr<RenderEffectBase> GetRenderEffect() = 0;
    virtual bool SetValue(
        const std::shared_ptr<Derived>& other, RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier) = 0;
    virtual void Attach(RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier) = 0;
    virtual void Detach() = 0;

    // Should only be called on not-attached effect.
    bool Append(const std::shared_ptr<Derived>& effect)
    {
        if (!effect) {
            return true;
        }

        if (effect->nextEffect_) {
            ROSEN_LOGE("Append failed, only single effect is allowed to append");
            return false;
        }

        auto current = Derived::shared_from_this();
        size_t count = 0;
        while (current->nextEffect_ && count < RenderEffectBase::EFFECT_COUNT_LIMIT) {
            count++;
            current = current->nextEffect_;
        }
        if (count >= RenderEffectBase::EFFECT_COUNT_LIMIT) {
            ROSEN_LOGE("Append failed, current effect already too long");
            return false;
        }

        if (current == effect) {
            ROSEN_LOGE("Append failed, candidate effect is same with tail effect");
            return false;
        }
        current->nextEffect_ = effect;
        return true;
    }

protected:
    inline void SetNextEffect(
        const std::shared_ptr<Derived>& effect, RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier)
    {
        if (nextEffect_) {
            nextEffect_->Detach();
        }
        nextEffect_ = effect;
        if (nextEffect_) {
            nextEffect_->Attach(node, modifier);
        }
    }

    size_t GetEffectCount() const
    {
        size_t count = 1;
        auto current = nextEffect_;
        while (current) {
            count++;
            current = current->nextEffect_;
        }
        return count;
    }

    std::shared_ptr<Derived> nextEffect_ = nullptr;
};

template <typename T>
struct is_property_tag : std::false_type {};

template <const char* Name, class PropertyType>
struct is_property_tag<PropertyTagBase<Name, PropertyType>> : std::true_type {};

template <typename T>
inline constexpr bool is_property_tag_v = is_property_tag<T>::value;

template <typename Base, RSNGEffectType Type, typename... PropertyTags>
class RSNGEffectTemplate : public Base {
    static_assert(std::is_base_of_v<RSNGEffectBase<Base, typename Base::RenderEffectBase>, Base>,
        "RSNGEffectTemplate: Base must be a subclass of RSNGEffectBase<Base>");
    static_assert(Type != RSNGEffectType::INVALID, "RSNGEffectTemplate: Type cannot be INVALID");
    static_assert((is_property_tag_v<PropertyTags> && ...), "RSNGEffectTemplate: All properties must be PropertyTags");

public:
    using RenderEffectTemplate = RSNGRenderEffectTemplate<typename Base::RenderEffectBase,
        Type, typename PropertyTags::RenderPropertyTagType...>;
    using ValuesTypeTuple = std::tuple<typename PropertyTags::ValueType...>;

    RSNGEffectTemplate() = default;
    ~RSNGEffectTemplate() override = default;

    RSNGEffectType GetType() const override
    {
        return Type;
    }

    std::shared_ptr<typename Base::RenderEffectBase> GetRenderEffect() override
    {
        // 1. fill render filter's properties
        auto renderProperties = std::apply(
            [](auto&&... elems) { return std::make_tuple(elems.CreateRenderPropertyTag()...); }, properties_);

        // 2. Create render filter by type and fill properties
        auto current = std::make_shared<RenderEffectTemplate>(renderProperties);
        if (auto& nextEffect = Base::nextEffect_) {
            current->nextEffect_ = nextEffect->GetRenderEffect();
        }
        return current;
    }

    bool SetValue(const std::shared_ptr<Base>& other, RSNode& node,
        const std::weak_ptr<ModifierNG::RSModifier>& modifier) override
    {
        if (other == nullptr || GetType() != other->GetType()) {
            return false;
        }

        auto otherDown = std::static_pointer_cast<RSNGEffectTemplate>(other);
        auto& otherProps = otherDown->GetProperties();
        std::apply([&otherProps](const auto&... args) {
                (args.value_->Set(std::get<std::decay_t<decltype(args)>>(otherProps).value_->Get()), ...);
            },
            properties_);

        auto& otherNextEffect = otherDown->nextEffect_;
        if (!Base::nextEffect_ || !Base::nextEffect_->SetValue(otherNextEffect, node, modifier)) {
            Base::SetNextEffect(otherNextEffect, node, modifier);
        }
        return true;
    }

    void Attach(RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier) override
    {
        RS_OPTIONAL_TRACE_NAME_FMT("RSNGEffectTemplate::Attach, Type:%s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str());
        std::apply([&node, &modifier](const auto&... args) {
                (RSNGEffectUtils::Attach(args.value_, node, modifier), ...);
            },
            properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Attach(node, modifier);
        }
    }

    void Detach() override
    {
        RS_OPTIONAL_TRACE_NAME_FMT("RSNGEffectTemplate::Detach, Type:%s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str());
        std::apply([](const auto&... args) { (RSNGEffectUtils::Detach(args.value_), ...); }, properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Detach();
        }
    }

    template<typename Tag>
    static constexpr bool Contains()
    {
        static_assert(is_property_tag_v<Tag>, "Tag must be a property tag.");
        return (std::is_same_v<Tag, PropertyTags> || ...);
    };

    template<typename Tag>
    constexpr const auto& Getter() const
    {
        static_assert(is_property_tag_v<Tag>, "Tag must be a property tag.");
        static_assert(propTagsSize_ > 0, "Cannot call Getter: No properties are defined in this effect.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        return std::get<Tag>(properties_).value_;
    }

    template<typename Tag, typename T>
    constexpr void Setter(T&& value)
    {
        static_assert(is_property_tag_v<Tag>, "Tag must be a property tag.");
        static_assert(propTagsSize_ > 0, "Cannot call Setter: No properties are defined in this effect.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        // IMPORTANT: Implicit type conversion is not allowed.
        // For example, double or int is NOT allowed where float is expected.
        static_assert(std::is_same_v<typename Tag::ValueType, std::decay_t<T>> ||
            std::is_base_of_v<std::decay_t<T>, typename Tag::ValueType>,
            "Setter type mismatch, explicit conversion required.");
        return std::get<Tag>(properties_).value_->Set(std::forward<T>(value));
    }

    template<std::size_t Index>
    using PropertyTagAt = typename std::tuple_element<Index, std::tuple<PropertyTags...>>::type;

    template<std::size_t Index>
    constexpr const auto& Getter() const
    {
        static_assert(propTagsSize_ > 0, "Cannot call Setter: No properties are defined in this effect.");
        static_assert(Index < propTagsSize_, "Cannot call Setter: Index exceeds the size of properties.");
        return std::get<Index>(properties_).value_;
    }

    template<std::size_t Index, typename T>
    constexpr void Setter(T&& value) const
    {
        static_assert(propTagsSize_ > 0, "Cannot call Setter: No properties are defined in this effect.");
        static_assert(Index < propTagsSize_, "Cannot call Setter: Index exceeds the size of properties.");
        // IMPORTANT: Implicit type conversion is not allowed.
        // For example, double or int is NOT allowed where float is expected.
        using ValueTypeIn = typename PropertyTagAt<Index>::ValueType;
        static_assert(std::is_same_v<ValueTypeIn, std::decay_t<T>> || std::is_base_of_v<std::decay_t<T>, ValueTypeIn>,
            "Setter type mismatch, explicit conversion requeired.");
        return std::get<Index>(properties_).value_->Set(std::forward<T>(value));
    }

    template<typename ValueTuple>
    constexpr void Setter(ValueTuple&& values)
    {
        static_assert(std::tuple_size_v<std::decay_t<ValueTuple>> == propTagsSize_,
            "The size of ValueTuple must match the size of properties.");
        SetterWithIndex<ValueTuple>(std::forward<ValueTuple>(values), std::make_index_sequence<propTagsSize_>{});
    }

    const std::tuple<PropertyTags...>& GetProperties() const
    {
        return properties_;
    }

private:
    RSNGEffectTemplate(std::tuple<PropertyTags...>&& properties) : properties_(std::move(properties)) {}

    template<typename ValueTuple, std::size_t... Index>
    constexpr void SetterWithIndex(ValueTuple&& values, std::index_sequence<Index...>)
    {
        std::apply(
            [this](auto&&... value) {
                (Setter<Index>(std::forward<decltype(value)>(value)), ...);
            },
            std::forward<ValueTuple>(values));
    }

    static constexpr size_t propTagsSize_ = sizeof...(PropertyTags);
    std::tuple<PropertyTags...> properties_;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_TEMPLATE_H