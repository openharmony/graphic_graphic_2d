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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H

#include "animation/rs_animation_timing_protocol.h"

namespace OHOS {
namespace Rosen {
enum class ForwardDirection {
    NORMAL = 0,
    REVERSE,
};

class RSAnimationFraction : public RSAnimationTimingProtocol {
public:
    RSAnimationFraction();
    ~RSAnimationFraction() = default;

    float GetAnimationFraction(int64_t time, bool& isInStartDelay, bool& isFinished);
    void UpdateRemainTimeFraction(float fraction, int remainTime = 0);
    float GetStartFraction() const;
    float GetEndFraction() const;
    void SetDirectionAfterStart(const ForwardDirection& direction);
    void SetLastFrameTime(int64_t lastFrameTime);

private:
    bool IsFinished() const;
    void UpdateReverseState(bool finish);
    ForwardDirection direction_ { ForwardDirection::NORMAL };
    int64_t playTime_ { 0 };
    float curTimeFraction_ { 0.0f };
    int curRepeatCount_ { 0 };
    int64_t runningTime_ { 0 };
    bool currentIsReverseCycle_ { false };
    int64_t lastFrameTime_ { -1 };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H
