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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H

#include "atomic"

#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
enum class ForwardDirection {
    NORMAL = 0,
    REVERSE,
};

class RSB_EXPORT RSAnimationFraction : public RSAnimationTimingProtocol {
public:
    RSAnimationFraction();
    ~RSAnimationFraction() = default;

    static void Init();
    static float GetAnimationScale();
    static void SetAnimationScale(float animationScale);
    static void OnAnimationScaleChangedCallback(const char *key, const char *value, void *context);

    // return <fraction, isInStartDelay, isFinished, isRepeatFinished> as tuple
    std::tuple<float, bool, bool, bool> GetAnimationFraction(int64_t time);
    void UpdateRemainTimeFraction(float fraction, int remainTime = 0);
    float GetStartFraction() const;
    float GetEndFraction() const;
    void SetDirectionAfterStart(const ForwardDirection& direction);
    void SetLastFrameTime(int64_t lastFrameTime);
    int64_t GetLastFrameTime() const;
    void ResetFraction();
    int GetRemainingRepeatCount() const;
    bool GetCurrentIsReverseCycle() const;

    void SetRepeatCallbackEnable(bool isEnable)
    {
        isRepeatCallbackEnable_ = isEnable;
    }

    bool GetRepeatCallbackEnable() const
    {
        return isRepeatCallbackEnable_;
    }

private:
    bool IsInRepeat() const;
    bool IsFinished() const;
    void UpdateReverseState(bool finish);
    bool IsStartRunning(const int64_t deltaTime, const int64_t startDelayNs);

    static std::atomic<float> animationScale_;
    static bool isInitialized_;

    ForwardDirection direction_ { ForwardDirection::NORMAL };
    int64_t playTime_ { 0 };
    float currentTimeFraction_ { 0.0f };
    int currentRepeatCount_ { 0 };
    int64_t runningTime_ { 0 };
    bool currentIsReverseCycle_ { false };
    int64_t lastFrameTime_ { -1 };
    bool isRepeatCallbackEnable_ {false};
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H
