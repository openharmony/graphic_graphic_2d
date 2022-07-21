/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H

#include <memory>

#include "animation/rs_animation.h"
#include "animation/rs_animation_manager_map.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_ui_animation_manager.h"
#include "common/rs_macros.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RS_EXPORT RSPropertyAnimation : public RSAnimation {
public:
    RSPropertyAnimation() = delete;
    virtual ~RSPropertyAnimation() = default;

protected:
    RSPropertyAnimation(RSAnimatableProperty<T>& property) : property_(property)
    {
        InitAdditiveMode();
    }

    void SetAdditive(bool isAdditive)
    {
        isAdditive_ = isAdditive;
    }

    bool GetAdditive() const
    {
        return isAdditive_;
    }

    auto GetOriginValue() const
    {
        return originValue_;
    }

    void SetPropertyValue(const T& value)
    {
        property_.stagingValue_ = value;
    }

    auto GetPropertyValue() const
    {
        return property_.stagingValue_;
    }

    PropertyId GetPropertyId() const override
    {
        return property_.id_;
    }

    void OnStart() override
    {
        if (!hasOriginValue_) {
            originValue_ = GetPropertyValue();
            hasOriginValue_ = true;
        }
        InitInterpolationValue();
    }

    void SetOriginValue(const T& originValue)
    {
        if (!hasOriginValue_) {
            originValue_ = originValue;
            hasOriginValue_ = true;
        }
    }

    virtual void InitInterpolationValue()
    {
        if (isDelta_) {
            startValue_ = originValue_;
            endValue_ = originValue_ + byValue_;
        } else {
            byValue_ = endValue_ - startValue_;
        }
    }

    void OnUpdateStagingValue(bool isFirstStart) override
    {
        auto startValue = startValue_;
        auto endValue = endValue_;
        if (!GetDirection()) {
            std::swap(startValue, endValue);
        }
        auto byValue = endValue - startValue;
        auto targetValue = endValue;
        if (isFirstStart) {
            if (GetAutoReverse() && GetRepeatCount() % 2 == 0) {
                targetValue = startValue;
            } else {
                targetValue = endValue;
            }
        } else {
            auto currentValue = GetPropertyValue();
            if (GetAutoReverse() && GetRepeatCount() % 2 == 0) {
                targetValue = IsReversed() ? currentValue + byValue : currentValue - byValue;
            } else {
                targetValue = IsReversed() ? currentValue - byValue : currentValue + byValue;
            }
        }

        SetPropertyValue(targetValue);
    }

    void StartCustomPropertyAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
    {
        auto target = GetTarget().lock();
        if (target == nullptr) {
            ROSEN_LOGE("Failed to start custom animation, target is null!");
            return;
        }

        auto animationManager = RSAnimationManagerMap::Instance()->GetAnimationManager(gettid());
        if (animationManager == nullptr) {
            ROSEN_LOGE("Failed to start custom animation, UI animation manager is null!");
            return;
        }

        auto renderProperty = animationManager->GetRenderProperty(property_.id_);
        if (renderProperty == nullptr) {
            renderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<RSAnimatableBase>>>(
                originValue_, property_.id_);
        }
        auto modifier = std::static_pointer_cast<RSAnimatableModifier<std::shared_ptr<RSAnimatableBase>>>(
            target->GetModifier(property_.id_));
        if (modifier != nullptr) {
            auto uiProperty = std::static_pointer_cast<RSAnimatableProperty<std::shared_ptr<RSAnimatableBase>>>(
                modifier->GetProperty());
            animationManager->AddAnimatableProp(property_.id_, uiProperty, renderProperty);
            if (uiProperty != nullptr) {
                uiProperty->AttachModifier(modifier);
            }
        }
        UpdateParamToRenderAnimation(animation);
        animation->AttachRenderProperty(renderProperty);
        animation->Start();
        animationManager->AddAnimation(animation, shared_from_this());
    }

    void SetPropertyOnAllAnimationFinish() override
    {
        property_.UpdateToRender(property_.Get(), false, true);
    }

    RSAnimatableProperty<T>& property_;
    T byValue_ {};
    T startValue_ {};
    T endValue_ {};
    T originValue_ {};
    bool isAdditive_ { true };
    bool isDelta_ { false };
    bool hasOriginValue_ { false };

private:
    void InitAdditiveMode()
    {
        switch (property_.type_) {
            case RSModifierType::QUATERNION:
                SetAdditive(false);
                break;
            default:
                break;
        }
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H
