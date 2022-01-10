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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H

#include "animation/rs_property_accessors.h"
#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSRenderPropertyAnimation : public RSRenderAnimation {
public:
    virtual ~RSRenderPropertyAnimation() = default;

    RSAnimatableProperty GetProperty() const override
    {
        return property_;
    }

    void SetAdditive(bool isAdditive)
    {
        if (IsStarted()) {
            ROSEN_LOGE("Failed to set additive, animation has started!");
            return;
        }

        isAdditive_ = isAdditive;
    }

    bool GetAdditive()
    {
        return isAdditive_;
    }
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderAnimation::Marshalling(parcel)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, RenderAnimation failed");
            return false;
        }
        if (!parcel.WriteInt32(static_cast<std::underlying_type<RSAnimatableProperty>::type>(property_))) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write type failed");
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, originValue_)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write value failed");
            return false;
        }
        return true;
    }
#endif
protected:
    RSRenderPropertyAnimation(AnimationId id, const RSAnimatableProperty& property, const T& originValue)
        : RSRenderAnimation(id), property_(property), originValue_(originValue), lastValue_(originValue)
    {
        propertyAccess_ = std::static_pointer_cast<RSPropertyAccessors<T>>(
            RSBasePropertyAccessors::GetAccessor(property));
    }
    RSRenderPropertyAnimation() =default;
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (RSRenderAnimation::ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, RenderAnimation failed");
            return false;
        }

        int32_t property = 0;
        if (!(parcel.ReadInt32(property) && RSMarshallingHelper::Unmarshalling(parcel, originValue_))) {
            ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, Unmarshalling failed");
            return false;
        }
        property_ = static_cast<RSAnimatableProperty>(property);
        propertyAccess_ = std::static_pointer_cast<RSPropertyAccessors<T>>(
            RSBasePropertyAccessors::GetAccessor(property_));

        return true;
    }
#endif
    void SetPropertyValue(const T& value)
    {
        auto target = GetTarget();
        if (target == nullptr || propertyAccess_->getter_ == nullptr) {
            ROSEN_LOGE("Failed to set property value, target is null!");
            return;
        }
        (target->GetRenderProperties().*propertyAccess_->setter_)(value);
    }

    auto GetPropertyValue() const
    {
        auto target = GetTarget();
        if (target == nullptr || propertyAccess_->getter_ == nullptr) {
            ROSEN_LOGE("Failed to get property value, target is null!");
            return lastValue_;
        }

        return (target->GetRenderProperties().*propertyAccess_->getter_)();
    }

    auto GetOriginValue() const
    {
        return originValue_;
    }

    auto GetLastValue() const
    {
        return lastValue_;
    }

    void SetAnimationValue(const T& value)
    {
        T animationValue;
        if (GetAdditive()) {
            animationValue = GetPropertyValue() + value - lastValue_;
        } else {
            animationValue = value;
        }

        lastValue_ = value;
        SetPropertyValue(animationValue);
    }

    void OnRemoveOnCompletion() override
    {
        T backwardValue;
        if (GetAdditive()) {
            backwardValue = GetPropertyValue() + GetOriginValue() - lastValue_;
        } else {
            backwardValue = GetOriginValue();
        }

        SetPropertyValue(backwardValue);
    }

private:
    RSAnimatableProperty property_ { RSAnimatableProperty::INVALID };
    T originValue_;
    T lastValue_;
    bool isAdditive_ { true };
    std::shared_ptr<RSPropertyAccessors<T>> propertyAccess_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H
