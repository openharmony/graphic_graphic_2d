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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H

#include <string>
#include <vector>

#include "animation/rs_animation_common.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_property_animation.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSPathAnimation : public RSPropertyAnimation<Vector2f> {
public:
    RSPathAnimation(const RSAnimatableProperty& property, const std::shared_ptr<RSPath>& animationPath);
    RSPathAnimation(const RSAnimatableProperty& property, const std::string& path, const Vector2f& startValue,
        const Vector2f& endValue);

    virtual ~RSPathAnimation() = default;

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve);

    const RSAnimationTimingCurve& GetTimingCurve() const;

    void SetRotationMode(const RotationMode& rotationMode);

    RotationMode GetRotationMode() const;

    void SetBeginFraction(float fraction);

    float GetBeginFraction() const;

    void SetEndFraction(float fraction);

    float GetEndFraction() const;

    static bool IsAnimatablePathProperty(const RSAnimatableProperty& property);

protected:
    void OnStart() override;

    void InitInterpolationValue() override;

    void OnUpdateStagingValue(bool isFirstStart) override;

private:
    void ReplaceSubString(std::string& sourceStr, const std::string& subStr, const std::string& newStr) const;

    const std::shared_ptr<RSPath> PreProcessPath(
        const std::string& path, const Vector2f& startValue, const Vector2f& endValue) const;

    const static std::vector<RSAnimatableProperty> PROP_FOR_PATH_ANIM;

    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    float startTangent_ { 0.0f };
    float endTangent_ { 0.0f };
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
    RSAnimationTimingCurve timingCurve_ { RSAnimationTimingCurve::DEFAULT };
    std::shared_ptr<RSPath> animationPath_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
