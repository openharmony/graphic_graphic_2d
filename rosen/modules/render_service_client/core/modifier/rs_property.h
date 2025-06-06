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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_property.h
 *
 * @brief Defines the properties and methods of RSPropertyBase and its derived classes.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H

#include <type_traits>
#include <unistd.h>

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_motion_path_option.h"
#include "command/rs_node_showing_command.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_node_map.h"
#include "property/rs_properties_def.h"
#include "platform/common/rs_log.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_context.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {

/**
 * @class RSUIFilter
 *
 * @brief The class for UI filter.
 */
class RSUIFilter;

/**
 * @brief Defines different types of thresholds for spring animation.
 *
 * Used to determine when the spring animation ends visually
 */
enum class ThresholdType {
    /** 0.5f for properties like position, as the difference in properties by 0.5 appears visually unchanged. */
    LAYOUT,
    /** 1.0f / 256.0f */
    COARSE,
    /** 1.0f / 1000.0f */
    MEDIUM,
    /** 1.0f / 3072.0f */
    FINE,
    /** 0.0f */
    COLOR,
    /** 1.0f / 256.0f */
    DEFAULT,
    /** 0.0f for noanimatable property */
    ZERO,
};


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

/**
 * @class RSPropertyBase
 *
 * @brief The base class for properties.
 *
 * This class provides a common interface for all property types.
 */
class RSC_EXPORT RSPropertyBase : public std::enable_shared_from_this<RSPropertyBase> {
public:
    /**
     * @brief Constructor for RSPropertyBase.
     */
    RSPropertyBase();

    /**
     * @brief Destructor for RSPropertyBase.
     */
    virtual ~RSPropertyBase() = default;

    /**
     * @brief Gets the ID of the property.
     *
     * @return The ID of the property.
     */
    PropertyId GetId() const
    {
        return id_;
    }

    /**
     * @brief Sets the threshold type for the property.
     *
     * @param thresholdType Indicates the threshold type to be set.
     */
    virtual void SetThresholdType(ThresholdType thresholdType) {}

    /**
     * @brief Gets the threshold type for the property.
     *
     * @return The threshold type of the property.
     */
    virtual float GetThreshold() const
    {
        return 0.0f;
    }

    /**
     * @brief Sets the value of the property from the render.
     *
     * @param rsRenderPropertyBase A shared pointer to the RSRenderPropertyBase object.
     */
    virtual void SetValueFromRender(const std::shared_ptr<const RSRenderPropertyBase>& rsRenderPropertyBase) {};

    virtual std::shared_ptr<RSRenderPropertyBase> GetRenderProperty()
    {
        return std::make_shared<RSRenderPropertyBase>(id_);
    }

protected:
    virtual void SetIsCustom(bool isCustom) {}

    virtual bool GetIsCustom() const
    {
        return false;
    }

    virtual void SetValue(const std::shared_ptr<RSPropertyBase>& value) {}

    virtual std::shared_ptr<RSPropertyBase> Clone() const
    {
        return std::make_shared<RSPropertyBase>();
    }

    virtual void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) {}

    virtual RSPropertyType GetPropertyType() const
    {
        return RSPropertyType::INVALID;
    }

    float GetThresholdByModifierType() const;

    virtual void UpdateOnAllAnimationFinish() {}
    virtual void UpdateCustomAnimation() {}

    virtual void AddPathAnimation() {}

    virtual void RemovePathAnimation() {}

    virtual void UpdateShowingValue(const std::shared_ptr<const RSRenderPropertyBase>& property) {}

    void AttachModifier(const std::shared_ptr<RSModifier>& modifier)
    {
        modifier_ = modifier;
    }

    void MarkModifierDirty();

    void MarkNodeDirty();

    void UpdateExtendModifierForGeometry(const std::shared_ptr<RSNode>& node);

    virtual bool GetShowingValueAndCancelAnimation()
    {
        return false;
    }

    float GetThresholdByThresholdType(ThresholdType thresholdType) const;

    PropertyId id_;
    RSModifierType type_ { RSModifierType::INVALID };
    std::weak_ptr<RSNode> target_;
    std::weak_ptr<RSModifier> modifier_;

private:
    virtual std::shared_ptr<RSPropertyBase> Add(const std::shared_ptr<const RSPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSPropertyBase> Minus(const std::shared_ptr<const RSPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSPropertyBase> Multiply(const float scale)
    {
        return shared_from_this();
    }

    virtual bool IsEqual(const std::shared_ptr<const RSPropertyBase>& value) const
    {
        return true;
    }

    friend std::shared_ptr<RSPropertyBase> operator+=(
        const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator-=(
        const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator*=(const std::shared_ptr<RSPropertyBase>& value, const float scale);
    friend std::shared_ptr<RSPropertyBase> operator+(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator-(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator*(
        const std::shared_ptr<const RSPropertyBase>& value, const float scale);
    friend bool operator==(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend bool operator!=(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend class RSTransition;
    friend class RSSpringAnimation;
    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSModifier;
    friend class RSBackgroundUIFilterModifier;
    friend class RSForegroundUIFilterModifier;
    friend class RSKeyframeAnimation;
    friend class RSInterpolatingSpringAnimation;
    friend class RSImplicitTransitionParam;
    friend class RSImplicitSpringAnimationParam;
    friend class RSImplicitKeyframeAnimationParam;
    friend class RSImplicitInterpolatingSpringAnimationParam;
    friend class RSImplicitCancelAnimationParam;
    friend class RSImplicitCurveAnimationParam;
    friend class RSImplicitAnimator;
    friend class RSGeometryTransModifier;
    friend class RSExtendedModifier;
    friend class RSCustomTransitionEffect;
    friend class RSCurveAnimation;
    template<typename T1>
    friend class RSAnimatableProperty;
    template<uint16_t commandType, uint16_t commandSubType>
    friend class RSGetShowingValueAndCancelAnimationTask;
};

/**
 * @class RSProperty
 *
 * @brief The class for properties.
 */
template<typename T>
class RSProperty : public RSPropertyBase {
    static_assert(std::is_base_of_v<RSArithmetic<T>, T> || supports_arithmetic<T>::value);

public:
    /**
     * @brief Constructor for RSProperty.
     */
    RSProperty() : RSPropertyBase() {}

    /**
     * @brief Constructor for RSProperty.
     *
     * @param value Indicates the staging value.
     */
    explicit RSProperty(const T& value) : RSPropertyBase()
    {
        stagingValue_ = value;
    }

    /**
     * @brief Destructor for RSProperty.
     */
    virtual ~RSProperty() = default;

    virtual void Set(const T& value)
    {
        if (ROSEN_EQ(value, stagingValue_) || !IsValid(value)) {
            return;
        }

        stagingValue_ = value;
        auto node = target_.lock();
        if (node == nullptr) {
            ROSEN_LOGE("RSProperty::Set node is null");
            return;
        }

        MarkNodeDirty();
        UpdateExtendModifierForGeometry(node);
        if (isCustom_) {
            MarkModifierDirty();
        } else {
            UpdateToRender(stagingValue_, UPDATE_TYPE_OVERWRITE);
        }
    }

    /**
     * @brief Gets the value of the staging.
     *
     * @return The value of the staging.
     */
    virtual T Get() const
    {
        return stagingValue_;
    }

protected:
    void UpdateToRender(const T& value, PropertyUpdateType type) const
    {}

    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSProperty<T>>(value);
        if (property != nullptr) {
            stagingValue_ = property->stagingValue_;
        }
    }

    std::shared_ptr<RSPropertyBase> Clone() const override
    {
        return std::make_shared<RSProperty<T>>(stagingValue_);
    }

    bool IsValid(const T& value)
    {
        return true;
    }

    void SetIsCustom(bool isCustom) override
    {
        isCustom_ = isCustom;
    }

    bool GetIsCustom() const override
    {
        return isCustom_;
    }

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() override
    {
        return std::make_shared<RSRenderProperty<T>>(stagingValue_, id_);
    }

    T stagingValue_ {};
    bool isCustom_ { false };

    friend class RSPathAnimation;
    friend class RSImplicitAnimator;
    friend class RSExtendedModifier;
    friend class RSModifier;
};

/**
 * @class RSAnimatableProperty
 *
 * @brief The class for animatable properties.
 */
template<typename T>
class RSAnimatableProperty : public RSProperty<T> {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<Color, T> ||
                  std::is_same_v<Matrix3f, T> || std::is_same_v<Vector2f, T> ||
                  std::is_same_v<Vector3f, T> || std::is_same_v<Vector4f, T> ||
                  std::is_same_v<Quaternion, T> ||
                  std::is_same_v<Vector4<Color>, T> || std::is_base_of_v<RSAnimatableArithmetic<T>, T> ||
                  supports_animatable_arithmetic<T>::value || std::is_same_v<RRect, T> ||
                  std::is_same_v<std::vector<float>, T>);

public:
    /**
     * @brief Constructor for RSAnimatableProperty.
     */
    RSAnimatableProperty() : RSProperty<T>() {}

    /**
     * @brief Constructor for RSAnimatableProperty.
     *
     * @param value Indicates the showing value.
     */
    explicit RSAnimatableProperty(const T& value) : RSProperty<T>(value)
    {
        showingValue_ = value;
    }

    /**
     * @brief Destructor for RSAnimatableProperty.
     */
    virtual ~RSAnimatableProperty() = default;

    void Set(const T& value) override
    {
        if (ROSEN_EQ(value, RSProperty<T>::stagingValue_) || !RSProperty<T>::IsValid(value)) {
            return;
        }

        auto node = RSProperty<T>::target_.lock();
        if (node == nullptr) {
            RSProperty<T>::stagingValue_ = value;
            return;
        }

        RSProperty<T>::MarkNodeDirty();
        RSProperty<T>::UpdateExtendModifierForGeometry(node);
        auto rsUIContext = node->GetRSUIContext();
        auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator() :
            RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
        if (implicitAnimator && implicitAnimator->NeedImplicitAnimation()) {
            auto startValue = std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
            auto endValue = std::make_shared<RSAnimatableProperty<T>>(value);
            if (motionPathOption_ != nullptr) {
                implicitAnimator->BeginImplicitPathAnimation(motionPathOption_);
                implicitAnimator->CreateImplicitAnimation(
                    node, RSProperty<T>::shared_from_this(), startValue, endValue);
                implicitAnimator->EndImplicitPathAnimation();
            } else {
                implicitAnimator->CreateImplicitAnimation(
                    node, RSProperty<T>::shared_from_this(), startValue, endValue);
            }
            return;
        }

        if (runningPathNum_ > 0) {
            return;
        }

        bool hasPropertyAnimation = node->HasPropertyAnimation(RSProperty<T>::id_);
        T sendValue = value;
        if (hasPropertyAnimation) {
            sendValue = value - RSProperty<T>::stagingValue_;
            auto endValue = std::make_shared<RSAnimatableProperty<T>>(value);
            RSAnimationTraceUtils::GetInstance().AddChangeAnimationValueTrace(
                RSProperty<T>::id_, endValue->GetRenderProperty());
        }
        RSProperty<T>::stagingValue_ = value;
        if (RSProperty<T>::isCustom_) {
            UpdateExtendedAnimatableProperty(sendValue, hasPropertyAnimation);
        } else {
            RSProperty<T>::UpdateToRender(
                sendValue, hasPropertyAnimation ? UPDATE_TYPE_INCREMENTAL : UPDATE_TYPE_OVERWRITE);
        }
    }

    /**
     * @brief Requests the cancellation of an ongoing implicit animation.
     */
    void RequestCancelAnimation()
    {
        auto node = RSProperty<T>::target_.lock();
        if (node == nullptr) {
            ROSEN_LOGE("RSAnimatableProperty::RequestCancelAnimation node is null");
            return;
        }
        auto rsUIContext = node->GetRSUIContext();
        auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator() :
            RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
        if (implicitAnimator && implicitAnimator->NeedImplicitAnimation()) {
            implicitAnimator->CancelImplicitAnimation(node, RSProperty<T>::shared_from_this());
        }
    }

    T Get() const override
    {
        if (RSProperty<T>::isCustom_) {
            return showingValue_;
        }
        return RSProperty<T>::stagingValue_;
    }

    /**
     * @brief Gets the staging value of the property.
     * @return The staging value.
     */
    T GetStagingValue() const
    {
        return RSProperty<T>::stagingValue_;
    }

    /**
     * @brief Gets the showing value of the property and cancels the animation.
     *
     * @return true if the showing value was successfully got and the animation was canceled, or if no
     *         animation was present or the property is custom; false if the task timed out or the property
     *         could not be retrieved.
     */
    bool GetShowingValueAndCancelAnimation() override
    {
        auto node = RSProperty<T>::target_.lock();
        if (node == nullptr) {
            return false;
        }
        if (!node->HasPropertyAnimation(this->id_) || this->GetIsCustom()) {
            return true;
        }
        auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(node->GetId(), GetRenderProperty());
        auto rsUIContext = node->GetRSUIContext();
        if (rsUIContext != nullptr) {
            auto transaction = rsUIContext->GetRSTransaction();
            if (transaction != nullptr) {
                transaction->ExecuteSynchronousTask(task, node->IsRenderServiceNode());
            }
        } else {
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->ExecuteSynchronousTask(task, node->IsRenderServiceNode());
            }
        }

        // when task is timeout, the caller need to decide whether to call this function again
        if (!task || task->IsTimeout()) {
            return false;
        }

        // We also need to send a command to cancel animation, cause:
        // case 1. some new animations have been added, but not flushed to render side,
        // resulting these animations not canceled in task.
        // case 2. the node or modifier has not yet been created on the render side, resulting in task failure.
        node->CancelAnimationByProperty(this->id_, !RSProperty<T>::isCustom_);

        if (!task->IsSuccess()) {
            // corresponding to case 2, as the new showing value is the same as staging value,
            // need not to update the value
            return true;
        }

        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(task->GetProperty());
        if (!renderProperty) {
            return false;
        }
        RSProperty<T>::stagingValue_ = renderProperty->Get();
        return true;
    }

    /**
     * @brief Sets the value of the property from render.
     *
     * @param rsRenderPropertyBase A shared pointer to the RSRenderPropertyBase object.
     */
    void SetValueFromRender(const std::shared_ptr<const RSRenderPropertyBase>& rsRenderPropertyBase) override
    {
        auto renderProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<T>>(rsRenderPropertyBase);
        if (!renderProperty) {
            ROSEN_LOGE("RSAnimatableProperty::SetValueFromRender renderProperty is null");
            return;
        }
        RSProperty<T>::stagingValue_ = renderProperty->Get();
    }

    /**
     * @brief Sets the callback function to be called when the property is updated.
     *
     * @param updateCallback The callback function to be set.
     */
    void SetUpdateCallback(const std::function<void(T)>& updateCallback)
    {
        propertyChangeListener_ = updateCallback;
    }

    /**
     * @brief Plays the animation.
     *
     * @param timingProtocol The protocol specifying the timing parameters for the animation (e.g., duration, delay).
     * @param timingCurve The curve defining the pacing of the animation (e.g., linear, ease-in, custom).
     * @param targetValue The target value to animate to, wrapped in a shared pointer to RSPropertyBase.
     * @param velocity (Optional) The initial velocity for the animation, wrapped in a shared pointer to RSAnimatableProperty<T>.
     * @param finishCallback (Optional) A callback function to be invoked when the animation finishes.
     * @param repeatCallback (Optional) A callback function to be invoked when the animation repeats.
     * @return std::vector<std::shared_ptr<RSAnimation>> A vector of shared pointers to the created RSAnimation objects.
     *         Returns an empty vector if the animation could not be created.
     */
    std::vector<std::shared_ptr<RSAnimation>> AnimateWithInitialVelocity(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
        const std::shared_ptr<RSPropertyBase>& targetValue,
        const std::shared_ptr<RSAnimatableProperty<T>>& velocity = nullptr,
        const std::function<void()>& finishCallback = nullptr, const std::function<void()>& repeatCallback = nullptr)
    {
        auto endValue = std::static_pointer_cast<RSAnimatableProperty<T>>(targetValue);
        if (!endValue) {
            return {};
        }

        auto node = RSProperty<T>::target_.lock();
        if (!node) {
            RSProperty<T>::stagingValue_ = endValue->Get();
            return {};
        }
        auto rsUIContext = node->GetRSUIContext();
        const auto& implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator() :
            RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
        if (!implicitAnimator) {
            RSProperty<T>::stagingValue_ = endValue->Get();
            return {};
        }

        std::shared_ptr<AnimationFinishCallback> animationFinishCallback;
        if (finishCallback) {
            animationFinishCallback =
                std::make_shared<AnimationFinishCallback>(finishCallback, timingProtocol.GetFinishCallbackType());
        }

        std::shared_ptr<AnimationRepeatCallback> animationRepeatCallback;
        if (repeatCallback) {
            animationRepeatCallback = std::make_shared<AnimationRepeatCallback>(repeatCallback);
        }

        implicitAnimator->OpenImplicitAnimation(
            timingProtocol, timingCurve, std::move(animationFinishCallback), std::move(animationRepeatCallback));
        auto startValue = std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
        if (velocity) {
            implicitAnimator->CreateImplicitAnimationWithInitialVelocity(
                node, RSProperty<T>::shared_from_this(), startValue, endValue, velocity);
        } else {
            implicitAnimator->CreateImplicitAnimation(node, RSProperty<T>::shared_from_this(), startValue, endValue);
        }

        return implicitAnimator->CloseImplicitAnimation();
    }

    /**
     * @brief Sets the property unit.
     *
     * @param unit The property unit to be set.
     */
    void SetPropertyUnit(RSPropertyUnit unit)
    {
        propertyUnit_ = unit;
    }

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() override
    {
        if (!RSProperty<T>::isCustom_) {
            return std::make_shared<RSRenderAnimatableProperty<T>>(
                RSProperty<T>::stagingValue_, RSProperty<T>::id_, GetPropertyType(), propertyUnit_);
        }

        if (renderProperty_ == nullptr) {
            renderProperty_ = std::make_shared<RSRenderAnimatableProperty<T>>(
                RSProperty<T>::stagingValue_, RSProperty<T>::id_, GetPropertyType(), propertyUnit_);
            auto weak = RSProperty<T>::weak_from_this();
            renderProperty_->SetUpdateUIPropertyFunc(
                [weak](const std::shared_ptr<RSRenderPropertyBase>& renderProperty) {
                    auto property = weak.lock();
                    if (property == nullptr) {
                        return;
                    }
                    property->UpdateShowingValue(renderProperty);
                });
        }
        return renderProperty_;
    }

protected:
    void UpdateOnAllAnimationFinish() override
    {
        RSProperty<T>::UpdateToRender(RSProperty<T>::stagingValue_, UPDATE_TYPE_FORCE_OVERWRITE);
    }

    void UpdateCustomAnimation() override
    {
        if (RSProperty<T>::isCustom_) {
            UpdateExtendedAnimatableProperty(RSProperty<T>::stagingValue_, false);
        }
    }

    void UpdateExtendedAnimatableProperty(const T& value, bool isDelta)
    {
        if (isDelta) {
            if (renderProperty_ != nullptr) {
                renderProperty_->Set(renderProperty_->Get() + value);
            }
        } else {
            showingValue_ = value;
            RSProperty<T>::MarkModifierDirty();
            if (renderProperty_ != nullptr) {
                renderProperty_->Set(value);
            } else {
                NotifyPropertyChange();
            }
        }
    }

    void AddPathAnimation() override
    {
        runningPathNum_ += 1;
    }

    void RemovePathAnimation() override
    {
        runningPathNum_ -= 1;
    }

    void UpdateShowingValue(const std::shared_ptr<const RSRenderPropertyBase>& property) override
    {
        auto renderProperty = std::static_pointer_cast<const RSRenderProperty<T>>(property);
        if (renderProperty != nullptr) {
            showingValue_ = renderProperty->Get();
            NotifyPropertyChange();
            RSProperty<T>::MarkModifierDirty();
        }
    }

    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(value);
        if (property != nullptr && property->GetPropertyType() == GetPropertyType()) {
            RSProperty<T>::stagingValue_ = property->stagingValue_;
        }
    }

    std::shared_ptr<RSPropertyBase> Clone() const override
    {
        return std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
    }

    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) override
    {
        motionPathOption_ = motionPathOption;
    }

    void NotifyPropertyChange()
    {
        if (propertyChangeListener_) {
            propertyChangeListener_(showingValue_);
        }
    }

    void SetThresholdType(ThresholdType thresholdType) override
    {
        thresholdType_ = thresholdType;
    }

    float GetThreshold() const override
    {
        return RSProperty<T>::GetThresholdByThresholdType(thresholdType_);
    }

    T showingValue_ {};
    std::shared_ptr<RSRenderAnimatableProperty<T>> renderProperty_;
    int runningPathNum_ { 0 };
    std::shared_ptr<RSMotionPathOption> motionPathOption_ {};
    std::function<void(T)> propertyChangeListener_;
    ThresholdType thresholdType_ { ThresholdType::DEFAULT };
    RSPropertyUnit propertyUnit_ { RSPropertyUnit::UNKNOWN };

private:
    RSPropertyType GetPropertyType() const override
    {
        return RSPropertyType::INVALID;
    }

    std::shared_ptr<RSPropertyBase> Add(const std::shared_ptr<const RSPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ + animatableProperty->stagingValue_;
        }
        return RSProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSPropertyBase> Minus(const std::shared_ptr<const RSPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ - animatableProperty->stagingValue_;
        }
        return RSProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSPropertyBase> Multiply(const float scale) override
    {
        RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ * scale;
        return RSProperty<T>::shared_from_this();
    }

    bool IsEqual(const std::shared_ptr<const RSPropertyBase>& value) const override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            return RSProperty<T>::stagingValue_ == animatableProperty->stagingValue_;
        }
        return true;
    }

    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSExtendedModifier;
    friend class RSModifier;
};

template<>
RSC_EXPORT void RSProperty<bool>::UpdateToRender(const bool& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<float>::UpdateToRender(const float& value, PropertyUpdateType type) const;
template <>
RSC_EXPORT void RSProperty<std::vector<float>>::UpdateToRender(
    const std::vector<float> &value, PropertyUpdateType type) const;
template <>
RSC_EXPORT void RSProperty<int>::UpdateToRender(const int &value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Color>::UpdateToRender(const Color& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Gravity>::UpdateToRender(const Gravity& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Matrix3f>::UpdateToRender(const Matrix3f& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Quaternion>::UpdateToRender(const Quaternion& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSImage>>::UpdateToRender(
    const std::shared_ptr<RSImage>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSMask>>::UpdateToRender(
    const std::shared_ptr<RSMask>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSPath>>::UpdateToRender(
    const std::shared_ptr<RSPath>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<RSDynamicBrightnessPara>::UpdateToRender(
    const RSDynamicBrightnessPara& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<RSWaterRipplePara>::UpdateToRender(
    const RSWaterRipplePara& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<RSFlyOutPara>::UpdateToRender(
    const RSFlyOutPara& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>::UpdateToRender(
    const std::shared_ptr<RSLinearGradientBlurPara>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<MotionBlurParam>>::UpdateToRender(
    const std::shared_ptr<MotionBlurParam>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSMagnifierParams>>::UpdateToRender(
    const std::shared_ptr<RSMagnifierParams>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::UpdateToRender(
    const std::vector<std::shared_ptr<EmitterUpdater>>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<ParticleNoiseFields>>::UpdateToRender(
    const std::shared_ptr<ParticleNoiseFields>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSShader>>::UpdateToRender(
    const std::shared_ptr<RSShader>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Vector2f>::UpdateToRender(const Vector2f& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Vector3f>::UpdateToRender(const Vector3f& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Vector4<uint32_t>>::UpdateToRender(
    const Vector4<uint32_t>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Vector4<Color>>::UpdateToRender(
    const Vector4<Color>& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<Vector4f>::UpdateToRender(const Vector4f& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<RRect>::UpdateToRender(const RRect& value, PropertyUpdateType type) const;
template<>
RSC_EXPORT void RSProperty<std::shared_ptr<RSUIFilter>>::UpdateToRender(
    const std::shared_ptr<RSUIFilter>& value, PropertyUpdateType type) const;

template<>
RSC_EXPORT bool RSProperty<float>::IsValid(const float& value);
template<>
RSC_EXPORT bool RSProperty<Vector2f>::IsValid(const Vector2f& value);
template<>
RSC_EXPORT bool RSProperty<Vector4f>::IsValid(const Vector4f& value);

template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<float>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Color>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Matrix3f>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Vector2f>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Vector3f>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Vector4f>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Quaternion>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<Vector4<Color>>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<RRect>::GetPropertyType() const;
template<>
RSC_EXPORT RSPropertyType RSAnimatableProperty<std::vector<float>>::GetPropertyType() const;
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H
