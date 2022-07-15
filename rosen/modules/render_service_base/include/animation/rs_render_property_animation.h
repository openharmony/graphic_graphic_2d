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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H

#include "animation/rs_animation_log.h"
#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSRenderPropertyAnimation : public RSRenderAnimation {
public:
    virtual ~RSRenderPropertyAnimation() = default;

    PropertyId GetPropertyId() const override
    {
        return propertyId_;
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

    void AttachRenderProperty(const std::shared_ptr<RSRenderPropertyBase>& property) override
    {
        auto animatableProperty = std::static_pointer_cast<RSRenderProperty<T>>(property);
        if (animatableProperty != nullptr) {
            property_ = animatableProperty;
        }
    }
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderAnimation::Marshalling(parcel)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, RenderAnimation failed");
            return false;
        }
        if (!parcel.WriteUint64(propertyId_)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write PropertyId failed");
            return false;
        }
        if (!(RSMarshallingHelper::Marshalling(parcel, originValue_) &&
                RSMarshallingHelper::Marshalling(parcel, isAdditive_))) {
            ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write value failed");
            return false;
        }
        return true;
    }
#endif
protected:
    RSRenderPropertyAnimation(AnimationId id, const PropertyId& propertyId,
        const T& originValue) : RSRenderAnimation(id), propertyId_(propertyId),
        originValue_(originValue), lastValue_(originValue)
    {}
    RSRenderPropertyAnimation() =default;
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (!RSRenderAnimation::ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, RenderAnimation failed");
            return false;
        }

        if (!(parcel.ReadUint64(propertyId_) && RSMarshallingHelper::Unmarshalling(parcel, originValue_) &&
                RSMarshallingHelper::Unmarshalling(parcel, isAdditive_))) {
            ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, Unmarshalling failed");
            return false;
        }
        lastValue_ = originValue_;

        return true;
    }
#endif
    void SetPropertyValue(const T& value)
    {
        if (property_ != nullptr) {
            property_->Set(value);
        }
    }

    auto GetPropertyValue() const
    {
        if (property_ != nullptr) {
            return property_->Get();
        }
        return lastValue_;
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
        SetPropertyValue(GetAnimationValue(value));
    }

    T GetAnimationValue(const T& value)
    {
        T animationValue;
        if (GetAdditive()) {
            animationValue = GetPropertyValue() + value - lastValue_;
        } else {
            animationValue = value;
        }

        lastValue_ = value;
        return animationValue;
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

    void WriteAnimationValueToLog(const T& value)
    {
        auto node = GetTarget();
        if (node == nullptr) {
            return;
        }

        UpdateNeedWriteLog(node->GetId());
        if (needWriteToLog_) {
            animationLog_->WriteAnimationValueToLog(value, propertyId_, node->GetId());
        }
    }

    void WriteAnimationInfoToLog(const T& startValue, const T& endValue)
    {
        if (hasWriteInfo_) {
            return;
        }

        hasWriteInfo_ = true;
        auto node = GetTarget();
        if (node == nullptr) {
            return;
        }

        UpdateNeedWriteLog(node->GetId());
        if (needWriteToLog_) {
            animationLog_->WriteAnimationInfoToLog(propertyId_, GetAnimationId(), startValue, endValue);
        }
    }

    void UpdateNeedWriteLog(const NodeId id)
    {
        if (!hasUpdateNeedWriteLog_) {
            hasUpdateNeedWriteLog_ = true;
            needWriteToLog_ = animationLog_->IsNeedWriteLog(propertyId_, id);
        }
    }

protected:
    PropertyId propertyId_;
    T originValue_;
    T lastValue_;

private:
    bool isAdditive_ { true };
    bool hasUpdateNeedWriteLog_ { false };
    bool needWriteToLog_ { false };
    bool hasWriteInfo_ { false };
    inline static std::shared_ptr<RSAnimationLog> animationLog_ = std::make_shared<RSAnimationLog>();
    std::shared_ptr<RSRenderProperty<T>> property_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROPERTY_ANIMATION_H
