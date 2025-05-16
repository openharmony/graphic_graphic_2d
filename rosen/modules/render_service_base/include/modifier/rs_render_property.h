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

#include "animation/rs_value_estimator.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_modifier_type.h"
#include "recording/draw_cmd_list.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSRenderMaskPara;
class RSRenderNode;
enum class ForegroundColorStrategyType;
enum class Gravity;

enum PropertyUpdateType : int8_t {
    UPDATE_TYPE_OVERWRITE,       // overwrite by given value
    UPDATE_TYPE_INCREMENTAL,     // incremental update by given value
    UPDATE_TYPE_FORCE_OVERWRITE, // overwrite and cancel all previous animations
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

    void Attach(std::weak_ptr<RSRenderNode> node)
    {
        node_ = node;
        OnChange();
    }

    RSModifierType GetModifierType() const
    {
        return modifierType_;
    }

    void SetModifierType(RSModifierType type)
    {
        modifierType_ = type;
        UpdatePropertyUnit(type);
    }

    virtual void Dump(std::string& out) const
    {
    }

    virtual size_t GetSize() const
    {
        return sizeof(*this);
    }

    static bool Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val);
    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);

protected:
    void OnChange() const;

    void UpdatePropertyUnit(RSModifierType type);

    virtual const std::shared_ptr<RSRenderPropertyBase> Clone() const
    {
        return nullptr;
    }

    virtual void SetValue(const std::shared_ptr<RSRenderPropertyBase>& value) {}

    virtual void SetPropertyType(const RSRenderPropertyType type) {}

    virtual RSRenderPropertyType GetPropertyType() const
    {
        return RSRenderPropertyType::INVALID;
    }

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
    std::weak_ptr<RSRenderNode> node_;
    RSModifierType modifierType_ { RSModifierType::INVALID };

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
    RSRenderProperty(const T& value, const PropertyId& id, const RSRenderPropertyType type)
        : RSRenderPropertyBase(id), stagingValue_(value), type_(type)
    {}
    virtual ~RSRenderProperty() = default;

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

    virtual size_t GetSize() const override
    {
        return sizeof(*this);
    }

    void Dump(std::string& out) const override
    {
    }

    void SetUpdateUIPropertyFunc(
        const std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)>& updateUIPropertyFunc)
    {
        updateUIPropertyFunc_ = updateUIPropertyFunc;
    }

protected:
    T stagingValue_;
    RSRenderPropertyType type_ = RSRenderPropertyType::INVALID;
    std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)> updateUIPropertyFunc_;
    RSRenderPropertyType GetPropertyType() const override
    {
        return type_;
    }

    friend class RSMarshallingHelper;
};

template<typename T>
class RSB_EXPORT_TMP RSRenderAnimatableProperty : public RSRenderProperty<T> {
public:
    RSRenderAnimatableProperty() : RSRenderProperty<T>() {}
    RSRenderAnimatableProperty(const T& value) : RSRenderProperty<T>(value, 0) {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id) : RSRenderProperty<T>(value, id) {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id, const RSRenderPropertyType type)
        : RSRenderProperty<T>(value, id, type)
    {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id,
        const RSRenderPropertyType type, const RSPropertyUnit unit)
        : RSRenderProperty<T>(value, id, type), unit_(unit)
    {}
    virtual ~RSRenderAnimatableProperty() = default;

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

protected:
    const std::shared_ptr<RSRenderPropertyBase> Clone() const override
    {
        return std::make_shared<RSRenderAnimatableProperty<T>>(
            RSRenderProperty<T>::stagingValue_, RSRenderProperty<T>::id_, RSRenderProperty<T>::type_, unit_);
    }

    void SetValue(const std::shared_ptr<RSRenderPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(value);
        if (property != nullptr && property->GetPropertyType() == RSRenderProperty<T>::type_) {
            RSRenderProperty<T>::Set(property->Get());
        }
    }

    void SetPropertyType(const RSRenderPropertyType type) override
    {
        RSRenderProperty<T>::type_ = type;
    }

    virtual RSRenderPropertyType GetPropertyType() const override
    {
        return RSRenderProperty<T>::type_;
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

    friend class RSMarshallingHelper;
    friend class RSRenderPathAnimation;
    friend class RSRenderPropertyBase;
};

template<>
RSB_EXPORT float RSRenderAnimatableProperty<float>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Vector4f>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Quaternion>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Vector2f>::ToFloat() const;
template<>
RSB_EXPORT float RSRenderAnimatableProperty<Vector3f>::ToFloat() const;

template<>
RSB_EXPORT void RSRenderProperty<bool>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<int>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<float>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector4<uint32_t>>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector4f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Quaternion>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector2f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Vector3f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Matrix3f>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<Color>::Dump(std::string& out) const;
template<>
RSB_EXPORT void RSRenderProperty<std::shared_ptr<RSFilter>>::Dump(std::string& out) const;
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
RSB_EXPORT bool RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<Vector4<Color>>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<RRect>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const;
template<>
RSB_EXPORT bool RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsEqual(
    const std::shared_ptr<const RSRenderPropertyBase>& value) const;

template<>
RSB_EXPORT size_t RSRenderProperty<Drawing::DrawCmdListPtr>::GetSize() const;

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
extern template class RSRenderProperty<std::shared_ptr<RSFilter>>;
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

extern template class RSRenderAnimatableProperty<float>;
extern template class RSRenderAnimatableProperty<Vector4f>;
extern template class RSRenderAnimatableProperty<Quaternion>;
extern template class RSRenderAnimatableProperty<Vector2f>;
extern template class RSRenderAnimatableProperty<Vector3f>;
extern template class RSRenderAnimatableProperty<RRect>;
extern template class RSRenderAnimatableProperty<Matrix3f>;
extern template class RSRenderAnimatableProperty<Color>;
extern template class RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>;
extern template class RSRenderAnimatableProperty<Vector4<Color>>;
extern template class RSRenderAnimatableProperty<std::vector<float>>;
#endif
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
