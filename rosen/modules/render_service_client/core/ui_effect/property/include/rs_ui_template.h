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
    virtual bool SetValue(const std::shared_ptr<Derived>& other, std::shared_ptr<RSNode> node) = 0;
    virtual void Attach(const std::shared_ptr<RSNode>& node) = 0;
    virtual void Detach() = 0;

    bool Append(const std::shared_ptr<Derived>& effect)
    {
        if (!effect) {
            return true;
        }

        if (effect.get() == this || effect->GetNextEffect()) {
            ROSEN_LOGE("Append failed, only single effect not same with head is allowed to append");
            return false;
        }

        if (!nextEffect_) {
            nextEffect_ = effect;
            return true;
        }

        auto current = nextEffect_;
        size_t count = 1;
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
    [[nodiscard]] virtual bool OnSetValue(const std::shared_ptr<Derived>& other, std::shared_ptr<RSNode> node)
    {
        auto otherNextEffect = other->GetNextEffect();
        if (!nextEffect_ || !nextEffect_->SetValue(otherNextEffect, node)) {
            SetNextEffect(otherNextEffect, node);
        }
        return true;
    }
    
    virtual void OnAttach(const std::shared_ptr<RSNode>& node)
    {
        if (nextEffect_) {
            nextEffect_->Attach(node);
        }
    }
    virtual void OnDetach()
    {
        if (nextEffect_) {
            nextEffect_->Detach();
        }
    }

    inline const std::shared_ptr<Derived>& GetNextEffect() const
    {
        return nextEffect_;
    }

    inline void SetNextEffect(const std::shared_ptr<Derived>& nextEffect, std::shared_ptr<RSNode> node)
    {
        OnDetach();
        nextEffect_ = nextEffect;
        OnAttach(node);
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

    RSNGEffectTemplate() = default;
    virtual ~RSNGEffectTemplate() override = default;

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
        if (auto nextEffect = Base::GetNextEffect(); nextEffect) {
            auto nextRenderEffect = nextEffect->GetRenderEffect();
            current->SetNextEffect(nextRenderEffect);
        }
        return current;
    }

    bool SetValue(const std::shared_ptr<Base>& other, std::shared_ptr<RSNode> node) override
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
        
        return Base::OnSetValue(other, node);
    }

    void Attach(const std::shared_ptr<RSNode>& node) override
    {
        std::apply([&node](const auto&... args) { (RSUIFilterUtils::Attach(args.value_, node), ...); }, properties_);
        Base::OnAttach(node);
    }

    void Detach() override
    {
        std::apply([](const auto&... args) { (RSUIFilterUtils::Detach(args.value_), ...); }, properties_);
        Base::OnDetach();
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
        static_assert(sizeof...(PropertyTags) > 0, "Cannot call Getter: No properties are defined in this group.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        return std::get<Tag>(properties_).value_;
    }

    template<typename Tag>
    constexpr void Setter(typename Tag::ValueType value)
    {
        static_assert(is_property_tag_v<Tag>, "Tag must be a property tag.");
        static_assert(sizeof...(PropertyTags) > 0, "Cannot call Setter: No properties are defined in this group.");
        static_assert(Contains<Tag>(), "Target property not registered.");
        return std::get<Tag>(properties_).value_->Set(value);
    }

    const std::tuple<PropertyTags...>& GetProperties() const
    {
        return properties_;
    }

private:
    RSNGEffectTemplate(std::tuple<PropertyTags...>&& properties) : properties_(std::move(properties)) {}
    std::tuple<PropertyTags...> properties_;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_TEMPLATE_H

