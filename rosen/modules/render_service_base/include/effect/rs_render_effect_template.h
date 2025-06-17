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

template <typename Derived, typename RenderEffect>
class RSNGEffectBase;

template <typename Base, RSUIFilterType Type, typename... PropertyTags>
class RSNGEffectTemplate;

template <typename Derived, size_t EffectCountLimit = 1000>
class RSNGRenderEffectBase : public std::enable_shared_from_this<Derived> {
public:
    static constexpr size_t EFFECT_COUNT_LIMIT = EffectCountLimit;

    virtual ~RSNGRenderEffectBase() = default;
    virtual RSUIFilterType GetType() const = 0;
    virtual bool Marshalling(Parcel& parcel) const = 0;
    virtual void Attach(const std::shared_ptr<RSRenderNode>& node) = 0;
    virtual void Detach(const std::shared_ptr<RSRenderNode>& node) = 0;
    virtual void SetModifierType(RSModifierType inType) = 0;

protected:
    [[nodiscard]] virtual bool OnUnmarshalling(Parcel& parcel) { return true; }
    
    virtual void DumpProperty(std::string& out) const {}

    inline const std::shared_ptr<Derived>& GetNextEffect() const
    {
        return nextEffect_;
    }

    inline void SetNextEffect(const std::shared_ptr<Derived>& nextEffect)
    {
        nextEffect_ = nextEffect;
    }

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

    template <typename U, RSUIFilterType T, typename... Tags>
    friend class RSNGEffectTemplate;
};

template <typename T>
struct is_render_property_tag : std::false_type {};

template <const char* Name, class PropertyType>
struct is_render_property_tag<RenderPropertyTagBase<Name, PropertyType>> : std::true_type {};

template <typename T>
inline constexpr bool is_render_property_tag_v = is_render_property_tag<T>::value;

template <typename Base, RSUIFilterType Type, typename... PropertyTags>
class RSNGRenderEffectTemplate : public Base {
    static_assert(std::is_base_of_v<RSNGRenderEffectBase<Base>, Base>,
        "RSNGRenderEffectTemplate: Base must be a subclass of RSNGRenderEffectBase<Base>");
    static_assert(Type != RSUIFilterType::INVALID,
        "RSNGRenderEffectTemplate: Type cannot be INVALID");
    static_assert((is_render_property_tag_v<PropertyTags> && ...),
        "RSNGRenderEffectTemplate: All properties must be render property tags");

public:
    RSNGRenderEffectTemplate() = default;
    virtual ~RSNGRenderEffectTemplate() override = default;
    RSNGRenderEffectTemplate(std::tuple<PropertyTags...> properties) noexcept
        : properties_(std::move(properties)) {}
    RSUIFilterType GetType() const override
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

        if (!RSMarshallingHelper::Marshalling(parcel, static_cast<RSUIFilterTypeUnderlying>(Type))) {
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

    void DumpProperty(std::string& out) const override
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

    void Attach(const std::shared_ptr<RSRenderNode>& node) override
    {
        std::apply([&node](const auto&... props) { (props.value_->Attach(node), ...); }, properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Attach(node);
        }
    }

    void Detach(const std::shared_ptr<RSRenderNode>& node) override
    {
        std::apply([&node](const auto&... props) { (props.value_->Detach(node), ...); }, properties_);
        if (Base::nextEffect_) {
            Base::nextEffect_->Detach(node);
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
protected:
    std::tuple<PropertyTags...> properties_;

    template <typename U, typename R>
    friend class RSNGEffectBase;

    template <typename U, RSUIFilterType T, typename... Tags>
    friend class RSNGEffectTemplate;
};

} // namespace OHOS
} // namespace Rosen

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_EFFECT_TEMPLATE_H
