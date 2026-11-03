/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "animation/rs_render_atlas_info_animation.h"

#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"
#include <cmath>

namespace OHOS {
namespace Rosen {

RSRenderAtlasInfoAnimation::RSRenderAtlasInfoAnimation(
    AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue,
    const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue)
{
    auto startProp = std::static_pointer_cast<RSRenderAnimatableProperty<AtlasInfo>>(startValue);
    auto endProp = std::static_pointer_cast<RSRenderAnimatableProperty<AtlasInfo>>(endValue);
    if (startProp != nullptr) {
        startValue_ = startProp->Get();
    }
    if (endProp != nullptr) {
        endValue_ = endProp->Get();
    }
}

void RSRenderAtlasInfoAnimation::OnSetPropertyOnAdd()
{
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(property_);
    if (prop == nullptr) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::OnSetPropertyOnAdd, property cast failed");
        return;
    }
    prop->Set(startValue_);
}

bool RSRenderAtlasInfoAnimation::OnAnimate(float fraction)
{
    float interpolatedFrame = startValue_.frameIndex +
        (endValue_.frameIndex - startValue_.frameIndex) * fraction;
    AtlasInfo result = startValue_;
    result.frameIndex = interpolatedFrame;

    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(property_);
    if (prop == nullptr) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::OnAnimate, property cast failed");
        return false;
    }
    prop->Set(result);
    return false;
}

void RSRenderAtlasInfoAnimation::RebuildPropertyValue(float fraction)
{
    OnAnimate(fraction);
}

void RSRenderAtlasInfoAnimation::InitValueEstimator()
{
    // Empty — AtlasInfo uses custom OnAnimate, not standard value estimators
}

bool RSRenderAtlasInfoAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::Marshalling, base Marshalling failed");
        return false;
    }
    // PixelMap + context scalars serialized once (start and end share identical context).
    bool ret = RSMarshallingHelper::Marshalling(parcel, startValue_) &&
        RSMarshallingHelper::Marshalling(parcel, endValue_.frameIndex);
    if (!ret) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::Marshalling failed");
    }
    return ret;
}

RSRenderAtlasInfoAnimation* RSRenderAtlasInfoAnimation::Unmarshalling(Parcel& parcel)
{
    auto animation = new RSRenderAtlasInfoAnimation();
    if (!animation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::Unmarshalling, ParseParam failed");
        delete animation;
        return nullptr;
    }
    return animation;
}

bool RSRenderAtlasInfoAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::ParseParam, base ParseParam failed");
        return false;
    }
    // PixelMap + context scalars unmarshalled once, shared between start and end.
    if (!RSMarshallingHelper::Unmarshalling(parcel, startValue_)) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::ParseParam, AtlasInfo Unmarshalling failed");
        return false;
    }
    float endFrameIndex = 0.0f;
    if (!RSMarshallingHelper::Unmarshalling(parcel, endFrameIndex)) {
        ROSEN_LOGE("RSRenderAtlasInfoAnimation::ParseParam, endFrameIndex Unmarshalling failed");
        return false;
    }
    endValue_ = startValue_;
    endValue_.frameIndex = endFrameIndex;
    return true;
}

} // namespace Rosen
} // namespace OHOS
