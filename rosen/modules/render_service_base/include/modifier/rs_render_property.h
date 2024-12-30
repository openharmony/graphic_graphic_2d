/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H

#include "recording/draw_cmd_list.h"

#include "animation/rs_render_particle.h"
#include "animation/rs_value_estimator.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties_def.h"
#include "transaction/rs_marshalling_helper.h"
#ifdef USE_M133_SKIA
#include "include/core/SkMatrix.h"
#endif

namespace OHOS {
namespace Rosen {
class RSNGRenderFilterBase;
class RSRenderMaskPara;
class RSRenderNode;
enum class ForegroundColorStrategyType;
enum class Gravity;
namespace ModifierNG {
class RSRenderModifier;
}

template<class...>
struct make_void { using type = void; };
template<class... T>
using void_t = typename make_void<T...>::type;

template<class T, class = void>
struct supports_arithmetic : std::false_type {};
template<class T>
struct supports_arithmetic<T,
    void_t<decltype(std::declval<T>() == std::declval<T>())>>
        : std::true_type {};

template<class T, class = void>
struct supports_animatable_arithmetic : std::false_type {};
template<class T>
struct supports_animatable_arithmetic<T,
    void_t<decltype(std::declval<T>() + std::declval<T>()),
        decltype(std::declval<T>() - std::declval<T>()),
        decltype(std::declval<T>() * std::declval<float>()),
        decltype(std::declval<T>() == std::declval<T>())>>
    : std::true_type {};

enum PropertyUpdateType : uint8_t {
    UPDATE_TYPE_OVERWRITE = 0,   // overwrite by given value
    UPDATE_TYPE_INCREMENTAL,     // incremental update by given value
    UPDATE_TYPE_FORCE_OVERWRITE, // overwrite and cancel all previous animations
};

enum class RSPropertyType : uint8_t {
    INVALID = 0,
    BOOL,
    INT,
    FLOAT,
    VECTOR4_UINT32,
    RS_COLOR,
    MATRIX3F,
    QUATERNION,
    VECTOR2F,
    VECTOR3F,
    VECTOR4F,
    VECTOR4_COLOR,
    SK_MATRIX,
    RRECT,
    DRAW_CMD_LIST,
    FOREGROUND_COLOR_STRATEGY,
    RS_SHADER,
    RS_IMAGE,
    RS_PATH,
    GRAVITY,
    DRAWING_MATRIX,
    LINEAR_GRADIENT_BLUR_PARA,
    MAGNIFIER_PARAMS,
    MOTION_BLUR_PARAM,
    VECTOR_EMITTER_UPDATER,
    PARTICLE_NOISE_FIELD,
    RS_MASK,
    WATER_RIPPLE_PARAMS,
    FLY_OUT_PARAMS,
    RENDER_PARTICLE_VECTOR,
    SHADER_PARAM,
    UI_FILTER,
    PIXEL_MAP,
    DYNAMIC_BRIGHTNESS_PARA,
    RS_RENDER_FILTER,
    VECTOR_FLOAT,
    RS_NG_RENDER_FILTER_BASE,
    RS_NG_RENDER_MASK_BASE,
};

enum class RSPropertyUnit : uint8_t {
    UNKNOWN = 0,
    PIXEL_POSITION,
    PIXEL_SIZE,
    RATIO_SCALE,
    ANGLE_ROTATION,
};

class RSB_EXPORT RSRenderPropertyBase : public std::enable_shared_from_this<RSRenderPropertyBase> {
public:
    RSRenderPropertyBase() = default;
    RSRenderPropertyBase(const PropertyId& id) : id_(id) {}
    RSRenderPropertyBase(const RSRenderPropertyBase&) = delete;
    RSRenderPropertyBase(const RSRenderPropertyBase&&) = delete;
    RSRenderPropertyBase& operator=(const RSRenderPropertyBase&) = delete;
    RSRenderPropertyBase& operator=(const RSRenderPropertyBase&&) = delete;
    virtual ~RSRenderPropertyBase() = default;

    PropertyId GetId() const
    {
        return id_;
    }

    void Attach(std::weak_ptr<RSRenderNode> node);

    void Detach(std::weak_ptr<RSRenderNode> node);

    void Attach(std::weak_ptr<ModifierNG::RSRenderModifier> modifier)
    {
        modifier_ = modifier;
    }

    // deprecated
    RSModifierType GetModifierType() const
    {
        return modifierType_;
    }
    // deprecated
    void SetModifierType(RSModifierType type)
    {
        modifierType_ = type;
        UpdatePropertyUnit(type);
        OnSetModifierType();
    }

    virtual void Dump(std::string& out) const = 0;
    virtual size_t GetSize() const = 0;

    virtual bool RSB_EXPORT Marshalling(Parcel& parcel) = 0;
    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);

protected:
    virtual bool Marshalling(Parcel& parcel) = 0;
    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);

    void OnChange() const;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnSetModifierType() {}

    void UpdatePropertyUnit(RSModifierType type);

    virtual const std::shared_ptr<RSRenderPropertyBase> Clone() const
    {
        return nullptr;
    }

    virtual void SetValue(const std::shared_ptr<RSRenderPropertyBase>& value) {}

    virtual RSPropertyType GetPropertyType() const = 0;

    virtual void SetPropertyUnit(RSPropertyUnit unit) {}

    virtual RSPropertyUnit GetPropertyUnit() const
    {
        return RSPropertyUnit::UNKNOWN;
    }

    virtual float ToFloat() const
    {
        return 1.f;
    }

    virtual std::shared_ptr<RSValueEstimator> CreateRSValueEstimator(const RSValueEstimatorType type)
    {
        return nullptr;
    }

    virtual std::shared_ptr<RSSpringValueEstimatorBase> CreateRSSpringValueEstimator()
    {
        return nullptr;
    }

    virtual bool IsNearEqual(const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
    {
        return true;
    }

    PropertyId id_;
    // deprecated
    std::weak_ptr<RSRenderNode> node_;
    // deprecated
    RSModifierType modifierType_ { RSModifierType::INVALID };

    std::weak_ptr<ModifierNG::RSRenderModifier> modifier_;

    using UnmarshallingFunc = std::function<bool (Parcel&, std::shared_ptr<RSRenderPropertyBase>&)>;
    inline static std::unordered_map<uint16_t, UnmarshallingFunc> UnmarshallingFuncs_;

    class RSPropertyUnmarshallingFuncRegister {
    public:
        RSPropertyUnmarshallingFuncRegister(bool isAnimatable, RSPropertyType type, UnmarshallingFunc func)
        {
            if (type != RSPropertyType::INVALID) {
                uint16_t key = static_cast<uint16_t>(isAnimatable) << 8 | static_cast<uint16_t>(type);
                UnmarshallingFuncs_[key] = func;
            }
        }
    };

private:
    virtual std::shared_ptr<RSRenderPropertyBase> Add(const std::shared_ptr<const RSRenderPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSRenderPropertyBase> Minus(const std::shared_ptr<const RSRenderPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSRenderPropertyBase> Multiply(const float scale)
    {
        return shared_from_this();
    }

    virtual bool IsEqual(const std::shared_ptr<const RSRenderPropertyBase>& value) const
    {
        return true;
    }

    friend std::shared_ptr<RSRenderPropertyBase> operator+=(
        const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend std::shared_ptr<RSRenderPropertyBase> operator-=(
        const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend std::shared_ptr<RSRenderPropertyBase> operator*=(
        const std::shared_ptr<RSRenderPropertyBase>& value, const float scale);
    friend std::shared_ptr<RSRenderPropertyBase> operator+(
        const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend std::shared_ptr<RSRenderPropertyBase> operator-(
        const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend std::shared_ptr<RSRenderPropertyBase> operator*(
        const std::shared_ptr<const RSRenderPropertyBase>& value, const float scale);
    friend bool operator==(
        const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend bool operator!=(
        const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b);
    friend class RSAnimationRateDecider;
    friend class RSRenderPropertyAnimation;
    friend class RSMarshallingHelper;
    friend class RSValueEstimator;
    friend class RSRenderPathAnimation;
    friend class RSRenderSpringAnimation;
    friend class RSRenderInterpolatingSpringAnimation;
    friend class RSRenderCurveAnimation;
    friend class RSRenderKeyframeAnimation;
    template<typename T>
    friend class RSSpringModel;
    friend class RSTransitionCustom;
    friend class RSAnimationTraceUtils;
};

template<typename T>
class RSB_EXPORT_TMP RSRenderProperty : public RSRenderPropertyBase {
public:
    RSRenderProperty() : RSRenderPropertyBase(0) {}
    RSRenderProperty(const T& value, const PropertyId& id) : RSRenderPropertyBase(id), stagingValue_(value) {}
    RSRenderProperty(const T& value, const PropertyId& id, const RSPropertyType type)
        : RSRenderPropertyBase(id), stagingValue_(value)
    {}
    ~RSRenderProperty() override = default;

    using ValueType = T;

    virtual void Set(const T& value, PropertyUpdateType type = UPDATE_TYPE_OVERWRITE)
    {
        if (value == stagingValue_) {
            return;
        }
        stagingValue_ = value;
        OnChange();
        if (updateUIPropertyFunc_) {
            updateUIPropertyFunc_(shared_from_this());
        }
    }

    T Get() const
    {
        return stagingValue_;
    }

    T& GetRef()
    {
        return stagingValue_;
    }

    size_t GetSize() const override
    {
        return sizeof(*this);
    }

    void Dump(std::string& out) const override {}

    void SetUpdateUIPropertyFunc(
        const std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)>& updateUIPropertyFunc)
    {
        updateUIPropertyFunc_ = updateUIPropertyFunc;
    }

    RSPropertyType GetPropertyType() const override
    {
        return type_;
    }

protected:
    T stagingValue_;
    inline static const RSPropertyType type_ = RSPropertyType::INVALID;

    std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)> updateUIPropertyFunc_;

    bool RSB_EXPORT Marshalling(Parcel& parcel) override
    {
        if (this->type_ == RSPropertyType::INVALID) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, false)) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, type_)) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, GetId())) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, stagingValue_)) {
            return false;
        }
        return true;
    }

    void SetUpdateUIPropertyFunc(
        const std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)>& updateUIPropertyFunc)
    {
        updateUIPropertyFunc_ = updateUIPropertyFunc;
    }

protected:
    T stagingValue_{};
    inline static const RSPropertyType type_ = RSPropertyType::INVALID;
    std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)> updateUIPropertyFunc_;

    void OnAttach() override {}
    void OnDetach() override {}
    void OnSetModifierType() override {}

    RSPropertyType GetPropertyType() const override
    {
        return type_;
    }

    static bool onUnmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);
    inline static RSPropertyUnmarshallingFuncRegister unmarshallingFuncRegister_ { false, type_, onUnmarshalling };

    friend class RSMarshallingHelper;
};

template<typename T>
class RSB_EXPORT_TMP RSRenderAnimatableProperty : public RSRenderProperty<T> {
    static_assert(std::is_floating_point_v<T> || std::is_same_v<Color, T> || std::is_same_v<Matrix3f, T> ||
                  std::is_same_v<Vector2f, T> || std::is_same_v<Vector3f, T> || std::is_same_v<Vector4f, T> ||
                  std::is_same_v<Quaternion, T> || std::is_same_v<Vector4<Color>, T> ||
                  supports_animatable_arithmetic<T>::value || std::is_base_of_v<RSAnimatableArithmetic<T>, T> ||
                  std::is_same_v<RRect, T>);

public:
    RSRenderAnimatableProperty() : RSRenderProperty<T>() {}
    RSRenderAnimatableProperty(const T& value) : RSRenderProperty<T>(value, 0) {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id) : RSRenderProperty<T>(value, id) {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id, const RSPropertyType type)
        : RSRenderProperty<T>(value, id)
    {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id, const RSPropertyUnit unit)
        : RSRenderProperty<T>(value, id), unit_(unit)
    {}
    virtual ~RSRenderAnimatableProperty() = default;

    using ValueType = T;

    void Set(const T& value, PropertyUpdateType type = UPDATE_TYPE_OVERWRITE) override
    {
        if (type == UPDATE_TYPE_OVERWRITE && value == RSRenderProperty<T>::stagingValue_) {
            return;
        }
        RSRenderProperty<T>::stagingValue_ =
            type == UPDATE_TYPE_INCREMENTAL ? static_cast<T>(RSRenderProperty<T>::Get() + value) : value;
        RSRenderProperty<T>::OnChange();
        if (RSRenderProperty<T>::updateUIPropertyFunc_) {
            RSRenderProperty<T>::updateUIPropertyFunc_(RSRenderProperty<T>::shared_from_this());
        }
    }

    bool Marshalling(Parcel& parcel) override
    {
        // Planning: use static_assert to limit the types that can be used with RSRenderAnimatableProperty.
        if constexpr (RSRenderProperty<T>::type_ == RSPropertyType::INVALID) {
            return false;
        }

        auto result = RSMarshallingHelper::Marshalling(parcel, RSRenderProperty<T>::type_) &&
                      RSMarshallingHelper::Marshalling(parcel, true) && // for animatable properties
                      RSMarshallingHelper::Marshalling(parcel, RSRenderProperty<T>::GetId()) &&
                      RSMarshallingHelper::Marshalling(parcel, RSRenderProperty<T>::stagingValue_) &&
                      RSMarshallingHelper::Marshalling(parcel, unit_);
        return result;
    }

protected:
    const std::shared_ptr<RSRenderPropertyBase> Clone() const override
    {
        return std::make_shared<RSRenderAnimatableProperty<T>>(
            RSRenderProperty<T>::stagingValue_, RSRenderProperty<T>::id_, unit_);
    }

    void SetValue(const std::shared_ptr<RSRenderPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(value);
        if (property != nullptr && property->GetPropertyType() == RSRenderProperty<T>::type_) {
            RSRenderProperty<T>::Set(property->Get());
        }
    }

    void SetPropertyUnit(RSPropertyUnit unit) override
    {
        unit_ = unit;
    }

    RSPropertyUnit GetPropertyUnit() const override
    {
        return unit_;
    }

    float ToFloat() const override
    {
        return 1.f;
    }

    bool IsNearEqual(const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const override
    {
        return IsEqual(value);
    }

    std::shared_ptr<RSValueEstimator> CreateRSValueEstimator(const RSValueEstimatorType type) override
    {
        switch (type) {
            case RSValueEstimatorType::CURVE_VALUE_ESTIMATOR: {
                return std::make_shared<RSCurveValueEstimator<T>>();
            }
            case RSValueEstimatorType::KEYFRAME_VALUE_ESTIMATOR: {
                return std::make_shared<RSKeyframeValueEstimator<T>>();
            }
            default: {
                return nullptr;
            }
        }
    }

    std::shared_ptr<RSSpringValueEstimatorBase> CreateRSSpringValueEstimator() override
    {
        return std::make_shared<RSSpringValueEstimator<T>>();
    }

    bool RSB_EXPORT Marshalling(Parcel& parcel) override
    {
        if (this->type_ == RSPropertyType::INVALID) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, true)) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, this->type_)) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, this->GetId())) {
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, this->stagingValue_)) {
            return false;
        }

        if (!RSMarshallingHelper::Marshalling(parcel, GetPropertyUnit())) {
            return false;
        }
        return true;
    }

    static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);

private:
    RSPropertyUnit unit_ = RSPropertyUnit::UNKNOWN;

    std::shared_ptr<RSRenderPropertyBase> Add(const std::shared_ptr<const RSRenderPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSRenderProperty<T>::stagingValue_ = RSRenderProperty<T>::stagingValue_ + animatableProperty->stagingValue_;
        }
        return RSRenderProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSRenderPropertyBase> Minus(const std::shared_ptr<const RSRenderPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSRenderProperty<T>::stagingValue_ = RSRenderProperty<T>::stagingValue_ - animatableProperty->stagingValue_;
        }
        return RSRenderProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSRenderPropertyBase> Multiply(const float scale) override
    {
        RSRenderProperty<T>::stagingValue_ = RSRenderProperty<T>::stagingValue_ * scale;
        return RSRenderProperty<T>::shared_from_this();
    }

    bool IsEqual(const std::shared_ptr<const RSRenderPropertyBase>& value) const override
    {
        auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            return RSRenderProperty<T>::stagingValue_ == animatableProperty->stagingValue_;
        }
        return true;
    }

    inline static RSRenderPropertyBase::RSPropertyUnmarshallingFuncRegister unmarshallingFuncRegister_ { true,
        RSRenderProperty<T>::type_, RSRenderAnimatableProperty<T>::OnUnmarshalling };
    friend class RSMarshallingHelper;
    friend class RSRenderPathAnimation;
    friend class RSRenderPropertyBase;
};

template<>
RSB_EXPORT float RSRenderAnimatableProperty<float>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Vector2f>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Vector4f>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Quaternion>::ToFloat() const;

template<>
RSB_EXPORT void RSRenderProperty<bool>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<int>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<float>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector4<uint32_t>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Quaternion>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector2f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector3f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector4f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Color>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector4<Color>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<RRect>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Drawing::DrawCmdListPtr>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<ForegroundColorStrategyType>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<SkMatrix>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSShader>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSImage>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSPath>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Gravity>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Drawing::Matrix>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<MotionBlurParam>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSMagnifierParams>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSMask>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>::Dump(std::string& out) const;

template<>
RSB_EXPORT bool RSRenderAnimatableProperty<float>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Vector4f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Quaternion>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Vector2f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Vector3f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Matrix3f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Color>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Vector4<Color>>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<RRect>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;

template<>
RSB_EXPORT size_t RSRenderProperty<Drawing::DrawCmdListPtr>::GetSize() const;

template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>::OnAttach();
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>::OnDetach();
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>::OnSetModifierType();

#if defined(_WIN32)
extern template class RSRenderProperty<bool>;
extern template class RSRenderProperty<int>;
extern template class RSRenderProperty<float>;
extern template class RSRenderProperty<Vector4<uint32_t>>;
extern template class RSRenderProperty<Vector4f>;
extern template class RSRenderProperty<Quaternion>;
extern template class RSRenderProperty<Vector2f>;
extern template class RSRenderProperty<Vector3f>;
extern template class RSRenderProperty<Matrix3f>;
extern template class RSRenderProperty<Color>;
extern template class RSRenderProperty<Vector4<Color>>;
extern template class RSRenderProperty<RRect>;
extern template class RSRenderProperty<Drawing::DrawCmdListPtr>;
extern template class RSRenderProperty<ForegroundColorStrategyType>;
extern template class RSRenderProperty<SkMatrix>;
extern template class RSRenderProperty<std::shared_ptr<RSShader>>;
extern template class RSRenderProperty<std::shared_ptr<RSImage>>;
extern template class RSRenderProperty<std::shared_ptr<RSPath>>;
extern template class RSRenderProperty<Gravity>;
extern template class RSRenderProperty<Drawing::Matrix>;
extern template class RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>;
extern template class RSRenderProperty<std::shared_ptr<MotionBlurParam>>;
extern template class RSRenderProperty<std::shared_ptr<RSMagnifierParams>>;
extern template class RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>;
extern template class RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>;
extern template class RSRenderProperty<std::shared_ptr<RSMask>>;
extern template class RSRenderProperty<std::shared_ptr<RSRenderFilter>>;
extern template class RSRenderProperty<std::shared_ptr<RSRenderMaskPara>>;

extern template class RSRenderAnimatableProperty<Color>;
extern template class RSRenderAnimatableProperty<float>;
extern template class RSRenderAnimatableProperty<Matrix3f>;
extern template class RSRenderAnimatableProperty<Quaternion>;
extern template class RSRenderAnimatableProperty<RRect>;
extern template class RSRenderAnimatableProperty<Vector2f>;
extern template class RSRenderAnimatableProperty<Vector3f>;
extern template class RSRenderAnimatableProperty<Vector4f>;
extern template class RSRenderAnimatableProperty<Vector4<Color>>;
extern template class RSRenderAnimatableProperty<std::vector<float>>;
#endif

#include "modifier/rs_property_def.in"

#undef DECLARE_PROPERTY
#undef DECLARE_ANIMATABLE_PROPERTY


} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
