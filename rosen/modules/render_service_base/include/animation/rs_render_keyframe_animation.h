/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_property_animation.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSInterpolator;

class RSB_EXPORT RSRenderKeyframeAnimation : public RSRenderPropertyAnimation {
public:
    RSRenderKeyframeAnimation(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue);
    ~RSRenderKeyframeAnimation() {}

    void DumpAnimationType(std::string& out) const override;

    void AddKeyframe(float fraction, const std::shared_ptr<RSRenderPropertyBase>& value,
        const std::shared_ptr<RSInterpolator>& interpolator);

    void AddKeyframes(const std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>>& keyframes);

    void AddKeyframe(int startDuration, int endDuration, const std::shared_ptr<RSRenderPropertyBase>& value,
        const std::shared_ptr<RSInterpolator>& interpolator);
    void SetDurationKeyframe(bool isDuration);
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSRenderKeyframeAnimation* Unmarshalling(Parcel& parcel);
protected:
    void OnAnimate(float fraction) override;

    void InitValueEstimator() override;

private:
    RSRenderKeyframeAnimation() = default;
    bool ParseParam(Parcel& parcel) override;
    bool ParseDurationKeyframesParam(Parcel& parcel, int keyframeSize);
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>> keyframes_;
    std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> durationKeyframes_;
    bool isDurationKeyframe_ { false };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_KEYFRAME_ANIMATION_H
