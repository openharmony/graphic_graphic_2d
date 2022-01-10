/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "animation/rs_property_accessors.h"
#include "common/rs_macros.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RS_EXPORT RSPropertyAnimation : public RSAnimation {
public:
    RSPropertyAnimation() = delete;
    virtual ~RSPropertyAnimation() = default;

protected:
    RSPropertyAnimation(const RSAnimatableProperty& property) : property_(property)
    {
        propertyAccess_ = std::static_pointer_cast<RSPropertyAccessors<T>>(
            RSBasePropertyAccessors::GetAccessor(property));
        InitAdditiveMode();
    }

    void SetAdditive(bool isAdditive)
    {
        isAdditive_ = true;
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
        auto target = GetTarget().lock();
        if (target == nullptr || propertyAccess_->setter_ == nullptr) {
            return;
        }

        (target->stagingProperties_.*propertyAccess_->setter_)(value);
    }

    auto GetPropertyValue() const
    {
        auto target = GetTarget().lock();
        if (target == nullptr || propertyAccess_->getter_ == nullptr) {
            return startValue_;
        }

        return (target->stagingProperties_.*propertyAccess_->getter_)();
    }

    RSAnimatableProperty GetProperty() const override
    {
        return property_;
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
        switch (GetProperty()) {
            case RSAnimatableProperty::ROTATION_3D:
                SetAdditive(false);
                break;
            default:
                break;
        }
    }

    RSAnimatableProperty property_ { RSAnimatableProperty::INVALID };
    std::shared_ptr<RSPropertyAccessors<T>> propertyAccess_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H
