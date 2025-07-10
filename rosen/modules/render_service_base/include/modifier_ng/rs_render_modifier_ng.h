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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H

#include "drawable/rs_misc_drawable.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
class RSRenderNode;
namespace ModifierNG {
// =============================================
// life cycle of RSRenderModifier
// 1. Create & animate
//   Create instance of RSRenderModifier and attach RSRenderProperty(s) to it, animate may change RSRenderProperty's
//   value
// 2. Apply
//   Apply RSRenderProperty(s) to staging value
class RSB_EXPORT RSRenderModifier : public std::enable_shared_from_this<RSRenderModifier> {
public:
    RSRenderModifier() = default;
    RSRenderModifier(ModifierId id) : id_(id) {}
    virtual ~RSRenderModifier() = default;

    // RSRenderProperty(s) to staging value
    virtual void Apply(RSPaintFilterCanvas* canvas, RSProperties& properties) {}
    // Temporary solution, apply Modifier to RSProperties, used in legacy code
    void ApplyLegacyProperty(RSProperties& properties);

    // For attaching/detaching RSRenderProperty to RSRenderModifier
    void AttachProperty(RSPropertyType type, const std::shared_ptr<RSRenderPropertyBase>& property);
    void DetachProperty(RSPropertyType type);

    // For attaching/detaching RSRenderModifier to RSRenderNode
    bool IsAttached() const;
    void OnAttachModifier(RSRenderNode& node);
    void OnDetachModifier();
    void SetDirty();

    // Only use in dump without consideration of time performance
    RSPropertyType FindPropertyType(const std::shared_ptr<RSRenderPropertyBase> target) const;

    virtual void SetSingleFrameModifier(bool value) {}

    virtual bool GetSingleFrameModifier() const
    {
        return false;
    }

    virtual bool IsCustom() const
    {
        return false;
    }

    virtual Drawing::DrawCmdListPtr GetPropertyDrawCmdList() const
    {
        return nullptr;
    }

    void Dump(std::string& out, const std::string& splitStr) const
    {
        for (auto& [type, property] : properties_) {
            out += RSModifierTypeString::GetPropertyTypeString(type) + (IsCustom() ? ":[" : "");
            property->Dump(out);
            out += (IsCustom() ? "]" : "") + splitStr;
        }
    }

    template<typename T>
    inline T Getter(RSPropertyType type, const T& defaultValue = {}) const
    {
        auto it = properties_.find(type);
        if (it == properties_.end()) {
            return defaultValue;
        }
        auto property = std::static_pointer_cast<RSRenderProperty<T>>(it->second);
        return property->Get();
    }

    template<typename T>
    inline void Setter(RSPropertyType type, const T& value)
    {
        auto it = properties_.find(type);
        if (it != properties_.end()) {
            auto property = std::static_pointer_cast<RSRenderProperty<T>>(it->second);
            property->Set(value);
        } else {
            // should not happen
        }
    }

    virtual RSModifierType GetType() const = 0;

    ModifierId GetId() const
    {
        return id_;
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty(RSPropertyType type)
    {
        auto it = properties_.find(type);
        if (it == properties_.end()) {
            return nullptr;
        }
        return it->second;
    }

    bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSRenderModifier* Unmarshalling(Parcel& parcel);

    inline bool HasProperty(RSPropertyType type) const
    {
        return properties_.count(type);
    }

    size_t GetPropertySize()
    {
        size_t size = 0;
        for (auto& [type, property] : properties_) {
            if (property != nullptr) {
                size += property->GetSize();
            }
        }
        return size;
    }

    template<typename T>
    static std::shared_ptr<RSRenderModifier> MakeRenderModifier(RSModifierType modifierType,
        const std::shared_ptr<RSRenderProperty<T>>& property, ModifierId id = 0,
        RSPropertyType propertyType = RSPropertyType::INVALID)
    {
        const auto& constructor = RSRenderModifier::ConstructorLUT_[static_cast<uint16_t>(modifierType)];
        if (constructor == nullptr) {
            return nullptr;
        }
        auto rawPointer = constructor();
        if (rawPointer == nullptr) {
            return nullptr;
        }
        std::shared_ptr<RSRenderModifier> renderModifier(rawPointer);
        if (propertyType == RSPropertyType::INVALID) {
            renderModifier->properties_.emplace(ModifierTypeConvertor::GetPropertyType(modifierType), property);
        } else {
            renderModifier->properties_.emplace(propertyType, property);
        }
        if (id > 0) {
            renderModifier->id_ = id;
        }
        return renderModifier;
    }

    using ResetFunc = void (*)(RSProperties& properties);
    static const std::unordered_map<RSModifierType, ResetFunc>& GetResetFuncMap();

protected:
    // only accept properties on white list ?
    ModifierId id_ = 0;
    bool dirty_ = true;
    std::weak_ptr<RSRenderNode> target_;

    // sub-class should not directly access properties_, use GetPropertyValue instead
    std::map<RSPropertyType, std::shared_ptr<RSRenderPropertyBase>> properties_;

    virtual void AttachRenderFilterProperty(
        const std::shared_ptr<RSRenderPropertyBase>& property, ModifierNG::RSPropertyType type)
    {}
    virtual void DetachRenderFilterProperty(
        const std::shared_ptr<RSRenderPropertyBase>& property, ModifierNG::RSPropertyType type)
    {}

    template<typename T, auto Setter>
    static void PropertyApplyHelper(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        (properties.*Setter)(value);
    }

    template<typename T, auto Setter, auto Getter>
    static void PropertyApplyHelperAdd(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& orig = (properties.*Getter)();
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        const auto newValue = Add(orig, value);
        (properties.*Setter)(newValue);
    }

    template<typename T, auto Setter, auto Getter>
    static void PropertyApplyHelperMultiply(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& orig = (properties.*Getter)();
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        const auto newValue = Multiply(orig, value);
        (properties.*Setter)(newValue);
    }

    virtual void OnSetDirty();

    using LegacyPropertyApplier = std::function<void(RSProperties& context, RSRenderPropertyBase&)>;
    using LegacyPropertyApplierMap = std::unordered_map<RSPropertyType, LegacyPropertyApplier>;

    static const LegacyPropertyApplierMap emptyLegacyPropertyApplierMap_;

    virtual const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const
    {
        return emptyLegacyPropertyApplierMap_;
    };

private:
    using Constructor = std::function<RSRenderModifier*()>;
    static std::array<Constructor, MODIFIER_TYPE_COUNT> ConstructorLUT_;

    template<typename T>
    static T Add(const T& a, const T& b)
    {
        return a + b;
    }

    template<typename T>
    static T Add(const std::optional<T>& a, const T& b)
    {
        return a.has_value() ? *a + b : b;
    }

    template<typename T>
    static T Add(const std::optional<T>& a, const std::optional<T>& b)
    {
        if (a.has_value() && b.has_value()) {
            return *a + *b;
        }
        if (!a.has_value() && b.has_value()) {
            return *b;
        }
        if (a.has_value() && !b.has_value()) {
            return *a;
        }
        return {};
    }

    template<typename T>
    static T Multiply(const T& a, const T& b)
    {
        return a * b;
    }

    template<typename T>
    static T Multiply(const std::optional<T>& a, const T& b)
    {
        return a.has_value() ? *a * b : b;
    }

    friend class RSModifier;
    friend class OHOS::Rosen::DrawableV2::RSCustomClipToFrameDrawable;
    friend class OHOS::Rosen::DrawableV2::RSEnvFGColorDrawable;
    friend class OHOS::Rosen::DrawableV2::RSEnvFGColorStrategyDrawable;
    friend class OHOS::Rosen::RSRenderNode;
};

// =============================================
// modifiers that can be recorded as display list
class RSB_EXPORT RSDisplayListRenderModifier : public RSRenderModifier {
protected:
    RSDisplayListRenderModifier() = default;
    ~RSDisplayListRenderModifier() override = default;
};

// Holds DrawCmdList
template<RSModifierType type>
class RSB_EXPORT RSCustomRenderModifier : public RSDisplayListRenderModifier {
public:
    RSCustomRenderModifier() = default;
    ~RSCustomRenderModifier() override = default;

    static inline constexpr auto Type = type;

    RSModifierType GetType() const override
    {
        return Type;
    };

    bool IsCustom() const override
    {
        return true;
    }

    void SetSingleFrameModifier(bool value) override
    {
        isSingleFrameModifier_ = value;
    }

    bool GetSingleFrameModifier() const override
    {
        return isSingleFrameModifier_;
    }

    Drawing::DrawCmdListPtr GetPropertyDrawCmdList() const override
    {
        return Getter<Drawing::DrawCmdListPtr>(ModifierTypeConvertor::GetPropertyType(GetType()), nullptr);
    }

    void Apply(RSPaintFilterCanvas* canvas, RSProperties& properties) override;

protected:
    bool isSingleFrameModifier_ = false;

private:
    void OnSetDirty() override;
};

class RSB_EXPORT RSFilterRenderModifier : public RSRenderModifier {
protected:
    RSFilterRenderModifier() = default;
    ~RSFilterRenderModifier() override = default;

    virtual bool IsForeground() const
    {
        return false;
    }
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H
