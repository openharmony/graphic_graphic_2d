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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H

#include <string>
#include <vector>

#include "animation/rs_animation_common.h"
#include "animation/rs_property_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSPath;

class RSC_EXPORT RSPathAnimation : public RSPropertyAnimation {
public:
    RSPathAnimation(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPath>& animationPath);
    RSPathAnimation(std::shared_ptr<RSPropertyBase> property, const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue);

    virtual ~RSPathAnimation() = default;

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve);

    const RSAnimationTimingCurve& GetTimingCurve() const;

    void SetRotationMode(const RotationMode& rotationMode);

    RotationMode GetRotationMode() const;

    void SetBeginFraction(float fraction);

    float GetBeginFraction() const;

    void SetEndFraction(float fraction);

    float GetEndFraction() const;

    void SetPathNeedAddOrigin(bool needAddOrigin);

    bool GetPathNeedAddOrigin() const;

protected:
    void OnStart() override;

    void InitInterpolationValue() override;

    void OnUpdateStagingValue(bool isFirstStart) override;

    void InitRotationId(const std::shared_ptr<RSNode>& node);

    PropertyId GetRotationPropertyId(const std::shared_ptr<RSNode>& node);

    void SetRotation(const std::shared_ptr<RSNode>& node, const float rotation);

    void OnCallFinishCallback() override;

    void SetPropertyOnAllAnimationFinish() override {}

private:
    void ReplaceSubString(std::string& sourceStr, const std::string& subStr, const std::string& newStr) const;

    const std::shared_ptr<RSPath> ProcessPath(const std::string& path, const float startX, const float startY,
        const float endX, const float endY) const;

    const std::shared_ptr<RSPath> PreProcessPath(const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue) const;

    void InitNeedPath(const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue);

    bool InitInterpolationVector2f(const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue);

    bool InitInterpolationVector4f(const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue);

    void UpdateVector2fValueAddOrigin(Vector2f& startValue, Vector2f& endValue, Vector2f& deltaValue);

    void UpdateVector4fValueAddOrigin(Vector4f& startValue, Vector4f& endValue, Vector4f& deltaValue);

    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    float startTangent_ { 0.0f };
    float endTangent_ { 0.0f };
    bool isNeedPath_ { true };
    bool needAddOrigin_ { true };
    PropertyId rotationId_;
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
    RSAnimationTimingCurve timingCurve_ { RSAnimationTimingCurve::DEFAULT };
    std::shared_ptr<RSPath> animationPath_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
