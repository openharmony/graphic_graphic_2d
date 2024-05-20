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
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;

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

    void SetModifierType(RSModifierType type)
    {
        modifierType_ = type;
        UpdatePropertyUnit(type);
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
        : RSRenderPropertyBase(id), stagingValue_(value)
    {}
    virtual ~RSRenderProperty() = default;

    void Set(const T& value)
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

    void SetUpdateUIPropertyFunc(
        const std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)>& updateUIPropertyFunc)
    {
        updateUIPropertyFunc_ = updateUIPropertyFunc;
    }

protected:
    T stagingValue_;
    std::function<void(const std::shared_ptr<RSRenderPropertyBase>&)> updateUIPropertyFunc_;
    RSRenderPropertyType GetPropertyType() const
    {
        return RSRenderPropertyType::INVALID;
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
        : RSRenderProperty<T>(value, id), type_(type)
    {}
    RSRenderAnimatableProperty(const T& value, const PropertyId& id,
        const RSRenderPropertyType type, const RSPropertyUnit unit)
        : RSRenderProperty<T>(value, id), type_(type), unit_(unit)
    {}
    virtual ~RSRenderAnimatableProperty() = default;

protected:
    const std::shared_ptr<RSRenderPropertyBase> Clone() const override
    {
        return std::make_shared<RSRenderAnimatableProperty<T>>(
            RSRenderProperty<T>::stagingValue_, RSRenderProperty<T>::id_, type_, unit_);
    }

    void SetValue(const std::shared_ptr<RSRenderPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(value);
        if (property != nullptr && property->GetPropertyType() == type_) {
            RSRenderProperty<T>::Set(property->Get());
        }
    }

    void SetPropertyType(const RSRenderPropertyType type) override
    {
        type_ = type;
    }

    virtual RSRenderPropertyType GetPropertyType() const override
    {
        return type_;
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
    RSRenderPropertyType type_ = RSRenderPropertyType::INVALID;
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

#if defined(_WIN32)
extern template class RSRenderAnimatableProperty<float>;
extern template class RSRenderAnimatableProperty<Vector4f>;
extern template class RSRenderAnimatableProperty<Quaternion>;
extern template class RSRenderAnimatableProperty<Vector2f>;
extern template class RSRenderAnimatableProperty<RRect>;
extern template class RSRenderAnimatableProperty<Matrix3f>;
extern template class RSRenderAnimatableProperty<Color>;
extern template class RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>;
extern template class RSRenderAnimatableProperty<Vector4<Color>>;
#endif
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
