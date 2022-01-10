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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_KEYFRAME_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_KEYFRAME_ANIMATION_H

#include "animation/rs_interpolator.h"
#include "animation/rs_render_property_animation.h"
#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSRenderKeyframeAnimation : public RSRenderPropertyAnimation<T> {
public:
    RSRenderKeyframeAnimation(AnimationId id, const RSAnimatableProperty& property, const T& originValue)
        : RSRenderPropertyAnimation<T>(id, property, originValue)
    {}
    ~RSRenderKeyframeAnimation() {}

    void AddKeyframe(float fraction, const T& value, const std::shared_ptr<RSInterpolator>& interpolator)
    {
        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
            ROSEN_LOGE("Failed to add key frame, fraction is invalid!");
            return;
        }

        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to add key frame, animation has started!");
            return;
        }

        keyframes_.push_back({ fraction, value, interpolator });
    }

    void AddKeyframes(const std::vector<std::tuple<float, T, std::shared_ptr<RSInterpolator>>>& keyframes)
    {
        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to add key frame, animation has started!");
            return;
        }

        keyframes_ = keyframes;
    }
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderPropertyAnimation<T>::Marshalling(parcel)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, RenderPropertyAnimation failed");
            return false;
        }
        uint32_t size = keyframes_.size();
        if (!parcel.WriteUint32(size)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write size failed");
            return false;
        }
        for (auto data : keyframes_) {
            if (!(parcel.WriteFloat(std::get<0>(data)) && RSMarshallingHelper::Marshalling(parcel, std::get<1>(data)) &&
                    std::get<2>(data)->Marshalling(parcel))) {
                ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write value failed");
                return false;
            }
        }
        return true;
    }
    static RSRenderKeyframeAnimation* Unmarshalling(Parcel& parcel)
    {
        RSRenderKeyframeAnimation* renderKeyframeAnimation = new RSRenderKeyframeAnimation<T>();
        if (!renderKeyframeAnimation->ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Unmarshalling, ParseParam failed");
            delete renderKeyframeAnimation;
            return nullptr;
        }
        return renderKeyframeAnimation;
    }
#endif
protected:
    void OnAnimate(float fraction) override
    {
        if (keyframes_.empty()) {
            ROSEN_LOGE("Failed to animate key frame, keyframes is empty!");
            return;
        }

        float preKeyframeFraction = std::get<0>(keyframes_.front());
        auto preKeyframeValue = std::get<1>(keyframes_.front());
        for (const auto& keyframe : keyframes_) {
            // the index of tuple
            float keyframeFraction = std::get<0>(keyframe);
            auto keyframeValue = std::get<1>(keyframe);
            auto keyframeInterpolator = std::get<2>(keyframe);
            if (fraction <= keyframeFraction) {
                if (ROSEN_EQ(keyframeFraction, preKeyframeFraction)) {
                    continue;
                }

                float intervalFraction = (fraction - preKeyframeFraction) / (keyframeFraction - preKeyframeFraction);
                auto interpolationValue = valueEstimator_->Estimate(
                    keyframeInterpolator->Interpolate(intervalFraction), preKeyframeValue, keyframeValue);
                RSRenderPropertyAnimation<T>::SetAnimationValue(interpolationValue);
                break;
            }

            preKeyframeFraction = keyframeFraction;
            preKeyframeValue = keyframeValue;
        }
    }

private:
    RSRenderKeyframeAnimation() = default;
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (!RSRenderPropertyAnimation<T>::ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, RenderPropertyAnimation fail");
            return false;
        }
        uint32_t size = 0;
        if (!parcel.ReadUint32(size)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Parse Keyframes size fail");
            return false;
        }
        float tupValue0 = 0;
        T tupValue1;
        std::shared_ptr<RSInterpolator> interpolator;
        keyframes_.clear();
        for (u_int32_t i = 0; i < size; i++) {
            if (!(parcel.ReadFloat(tupValue0) && RSMarshallingHelper::Unmarshalling(parcel, tupValue1))) {
                ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling value failed");
                return false;
            }
            std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
            keyframes_.emplace_back(std::make_tuple(tupValue0, tupValue1, interpolator));
        }
        return true;
    }
#endif
    std::vector<std::tuple<float, T, std::shared_ptr<RSInterpolator>>> keyframes_;
    std::shared_ptr<RSValueEstimator> valueEstimator_ { std::make_shared<RSValueEstimator>() };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_KEYFRAME_ANIMATION_H
