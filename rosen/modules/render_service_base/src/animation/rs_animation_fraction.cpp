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

#include "animation/rs_animation_fraction.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "animation/rs_animation_common.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int INFINITE = -1;
static constexpr int REVERSE_COUNT = 2;
static constexpr int MAX_SPEED = 1000000;
constexpr const char* ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
} // namespace

std::atomic<bool> RSAnimationFraction::isInitialized_ = false;
std::atomic<float> RSAnimationFraction::animationScale_ = 1.0f;

RSAnimationFraction::RSAnimationFraction()
{
    currentIsReverseCycle_ = !isForward_;
}

void RSAnimationFraction::Init()
{
    if (isInitialized_) {
        return;
    }
    SetAnimationScale(RSSystemProperties::GetAnimationScale());
    RSSystemProperties::WatchSystemProperty(ANIMATION_SCALE_NAME, OnAnimationScaleChangedCallback, nullptr);
    isInitialized_ = true;
}

float RSAnimationFraction::GetAnimationScale()
{
    return animationScale_.load(std::memory_order_relaxed);
}

void RSAnimationFraction::SetAnimationScale(float animationScale)
{
    animationScale_.store(std::max(0.0f, animationScale), std::memory_order_relaxed);
}

void RSAnimationFraction::OnAnimationScaleChangedCallback(const char* key, const char* value, void* context)
{
    if (strcmp(key, ANIMATION_SCALE_NAME) != 0) {
        return;
    }
    float animationScale = std::atof(value);
    if (ROSEN_NE(animationScale, 1.0f)) {
        ROSEN_LOGW("Animation scale changed to %{public}f", animationScale);
    }
    SetAnimationScale(animationScale);
}

void RSAnimationFraction::SetDirectionAfterStart(const ForwardDirection& direction)
{
    direction_ = direction;
}

void RSAnimationFraction::FlipDirection()
{
    direction_ = (direction_ == ForwardDirection::NORMAL) ? ForwardDirection::REVERSE : ForwardDirection::NORMAL;
}

void RSAnimationFraction::SetLastFrameTime(int64_t lastFrameTime)
{
    lastFrameTime_ = lastFrameTime;
}

int64_t RSAnimationFraction::GetLastFrameTime() const
{
    return lastFrameTime_;
}

bool RSAnimationFraction::IsStartRunning(const int64_t deltaTime, const int64_t startDelayNs, bool isCustom)
{
    const float animationScale = isCustom ? 1.0f : GetAnimationScale();
    if (direction_ == ForwardDirection::NORMAL) {
        if (ROSEN_EQ(animationScale, 0.0f)) {
            runningTime_ += static_cast<int64_t>(deltaTime * MAX_SPEED);
        } else {
            runningTime_ += static_cast<int64_t>(deltaTime * speed_ / animationScale);
        }
    } else {
        if (ROSEN_EQ(animationScale, 0.0f)) {
            runningTime_ -= static_cast<int64_t>(deltaTime * MAX_SPEED);
        } else {
            runningTime_ -= static_cast<int64_t>(deltaTime * speed_ / animationScale);
        }
    }
    return runningTime_ > startDelayNs;
}

bool RSAnimationFraction::UpdateGroupWaitingTime(int64_t deltaTime, bool isCustom)
{
    // When doing round-trip animation, runningTime_ will be less than 0 on the second pass, and the state becomes
    // groupwaiting. In this case, we should jump out directly and return true.
    if (runningTime_ <= 0) {
        return true;
    }

    const float animationScale = isCustom ? 1.0f : GetAnimationScale();
    int64_t deltaWithSpeed = ROSEN_EQ(animationScale, 0.0f) ? static_cast<int64_t>(deltaTime * MAX_SPEED)
        : static_cast<int64_t>(deltaTime * speed_ / animationScale);

    if (direction_ == ForwardDirection::NORMAL) {
        // Prevent overflow: check if addition would exceed INT64_MAX
        if (deltaWithSpeed > 0 && groupWaitingTime_ > INT64_MAX - deltaWithSpeed) {
            ROSEN_LOGW(
                "RSAnimationFraction::UpdateGroupWaitingTime groupWaitingTime_ would overflow, clamp to INT64_MAX");
            groupWaitingTime_ = INT64_MAX;
        } else {
            groupWaitingTime_ += deltaWithSpeed;
        }
        return false;
    } else {
        // Prevent underflow: check if subtraction would go below INT64_MIN
        if (deltaWithSpeed > 0 && groupWaitingTime_ < INT64_MIN + deltaWithSpeed) {
            ROSEN_LOGW(
                "RSAnimationFraction::UpdateGroupWaitingTime groupWaitingTime_ would underflow, clamp to INT64_MIN");
            groupWaitingTime_ = INT64_MIN;
        } else {
            groupWaitingTime_ -= deltaWithSpeed;
        }
        return groupWaitingTime_ <= 0;
    }
}

int64_t RSAnimationFraction::CalculateLeftDelayTime(const int64_t startDelayNs, bool isCustom)
{
    if (ROSEN_LE(speed_, 0.0f)) {
        return 0;
    }
    if (runningTime_ > startDelayNs) {
        return 0;
    }
    const float animationScale = isCustom ? 1.0f : GetAnimationScale();
    int64_t leftDelayTimeOrigin;
    if (direction_ == ForwardDirection::NORMAL) {
        leftDelayTimeOrigin = startDelayNs - runningTime_;
    } else {
        leftDelayTimeOrigin = runningTime_;
    }

    double ret = static_cast<double>(leftDelayTimeOrigin) / MS_TO_NS / speed_ * animationScale;
    if (ret > static_cast<double>(INT64_MAX) || ret < static_cast<double>(INT64_MIN)) {
        return 0;
    }
    return static_cast<int64_t>(ret);
}

std::tuple<float, bool, bool, bool> RSAnimationFraction::GetAnimationFraction(
    int64_t time, int64_t& minLeftDelayTime, bool isCustom)
{
    int64_t durationNs = duration_ * MS_TO_NS;
    int64_t startDelayNs = startDelay_ * MS_TO_NS;
    int64_t deltaTime = time - lastFrameTime_;
    bool isInStartDelay = false;
    bool isRepeatFinished = false;
    bool isFinished = true;

    // When the UI animation and spring animation are inherited, time will be passed the default value of -1 for
    // lastFrameTime_, which is a normal situation.
    if (deltaTime < 0 && time != -1) {
        ROSEN_LOGE("RSAnimationFraction::GetAnimationFraction, "
            "current time: %{public}lld is earlier than last frame time: %{public}lld",
            static_cast<long long>(time), static_cast<long long>(lastFrameTime_));
    }
    lastFrameTime_ = time;

    if (durationNs <= 0 || (repeatCount_ <= 0 && repeatCount_ != INFINITE)) {
        isFinished = true;
        minLeftDelayTime = 0;
        return { GetEndFraction(), isInStartDelay, isFinished, isRepeatFinished };
    }
    // 1. Calculates the total running fraction of animation
    if (!IsStartRunning(deltaTime, startDelayNs, isCustom)) {
        if (IsFinished(isCustom)) {
            minLeftDelayTime = 0;
            return { GetStartFraction(), isInStartDelay, true, isRepeatFinished };
        }
        isInStartDelay = true;
        if (minLeftDelayTime > 0) {
            minLeftDelayTime = std::min(CalculateLeftDelayTime(startDelayNs, isCustom), minLeftDelayTime);
        }
        return { GetStartFraction(), isInStartDelay, false, isRepeatFinished };
    }
    minLeftDelayTime = 0;

    // 2. Calculate the running time of the current cycle animation.
    int64_t realPlayTime = runningTime_ - startDelayNs - (currentRepeatCount_ * durationNs);

    // 3. Update the number of cycles and the corresponding animation fraction.
    if (direction_ == ForwardDirection::NORMAL) {
        currentRepeatCount_ += realPlayTime / durationNs;
    } else {
        while (currentRepeatCount_ > 0 && realPlayTime < 0) {
            currentRepeatCount_--;
            realPlayTime += durationNs;
        }
    }

    playTime_ = realPlayTime % durationNs;
    if (IsInRepeat()) {
        isRepeatFinished = ((playTime_ + deltaTime) >= durationNs);
    }

    // 4. update status for auto reverse
    isFinished = IsFinished(isCustom);
    UpdateReverseState(isFinished);

    // 5. get final animation fraction
    if (isFinished) {
        return { GetEndFraction(), isInStartDelay, isFinished, isRepeatCallbackEnable_ };
    }
    currentTimeFraction_ = static_cast<float>(playTime_) / durationNs;
    currentTimeFraction_ = currentIsReverseCycle_ ? (1.0f - currentTimeFraction_) : currentTimeFraction_;
    currentTimeFraction_ = std::clamp(currentTimeFraction_, 0.0f, 1.0f);
    return { currentTimeFraction_, isInStartDelay, isFinished, isRepeatFinished };
}

bool RSAnimationFraction::IsInRepeat() const
{
    if (isRepeatCallbackEnable_ && ((repeatCount_ == INFINITE) || ((currentRepeatCount_ + 1) < repeatCount_))) {
        return true;
    }
    return false;
}

bool RSAnimationFraction::IsFinished(bool isCustom) const
{
    if (direction_ == ForwardDirection::NORMAL) {
        if (repeatCount_ == INFINITE) {
            // When the animation scale is zero, the infinitely looping animation is considered to be finished
            const float animationScale = isCustom ? 1.0f : GetAnimationScale();
            return ROSEN_EQ(animationScale, 0.0f);
        }
        int64_t totalDuration = (static_cast<int64_t>(duration_) * repeatCount_ + startDelay_) * MS_TO_NS;
        return runningTime_ >= totalDuration;
    } else {
        return runningTime_ <= 0;
    }
}

float RSAnimationFraction::GetStartFraction() const
{
    return isForward_ ? 0.0f : 1.0f;
}

float RSAnimationFraction::GetEndFraction() const
{
    float endFraction = 1.0f;
    if ((autoReverse_ && repeatCount_ % REVERSE_COUNT == 0) || direction_ == ForwardDirection::REVERSE) {
        endFraction = 0.0f;
    }
    endFraction = isForward_ ? endFraction : 1.0 - endFraction;
    return endFraction;
}

void RSAnimationFraction::UpdateReverseState(bool finish)
{
    if (isForward_) {
        if (!autoReverse_) {
            currentIsReverseCycle_ = false;
            return;
        }
        currentIsReverseCycle_ =
            finish ? (currentRepeatCount_ % REVERSE_COUNT == 0) : (currentRepeatCount_ % REVERSE_COUNT == 1);
    } else {
        if (!autoReverse_) {
            currentIsReverseCycle_ = true;
            return;
        }
        currentIsReverseCycle_ =
            finish ? (currentRepeatCount_ % REVERSE_COUNT == 1) : (currentRepeatCount_ % REVERSE_COUNT == 0);
    }
}

void RSAnimationFraction::UpdateRemainTimeFraction(float fraction, int remainTime)
{
    int64_t remainTimeNS = remainTime * MS_TO_NS;
    int64_t durationNs = duration_ * MS_TO_NS;
    int64_t startDelayNs = startDelay_ * MS_TO_NS;
    float curRemainProgress = currentIsReverseCycle_ ? currentTimeFraction_ : (1.0f - currentTimeFraction_);
    float ratio = 1.0f;
    if (remainTime != 0) {
        ratio = curRemainProgress * durationNs / remainTimeNS;
    }

    if (runningTime_ > startDelayNs || fabs(fraction) > 1e-6) {
        if (currentIsReverseCycle_) {
            runningTime_ =
                static_cast<int64_t>(durationNs * (1.0f - fraction)) + startDelayNs + currentRepeatCount_ * durationNs;
        } else {
            runningTime_ =
                static_cast<int64_t>(durationNs * fraction) + startDelayNs + currentRepeatCount_ * durationNs;
        }
    }

    speed_ = speed_ * ratio;
    currentTimeFraction_ = fraction;
}

void RSAnimationFraction::ResetFraction()
{
    runningTime_ = 0;
    playTime_ = 0;
    currentTimeFraction_ = 0.0f;
    currentRepeatCount_ = 0;
    currentIsReverseCycle_ = false;
    groupWaitingTime_ = 0;
}

int RSAnimationFraction::GetRemainingRepeatCount() const
{
    if (repeatCount_ == INFINITE) {
        return INFINITE;
    }
    if (currentRepeatCount_ >= repeatCount_) {
        return 0;
    } else {
        return repeatCount_ - currentRepeatCount_;
    }
}

bool RSAnimationFraction::GetCurrentIsReverseCycle() const
{
    return currentIsReverseCycle_;
}
} // namespace Rosen
} // namespace OHOS
