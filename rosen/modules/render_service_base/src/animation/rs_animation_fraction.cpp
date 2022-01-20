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

#include "animation/rs_animation_fraction.h"

#include <string>

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int INFINITE = -1;
static constexpr int64_t MS_TO_NS = 1000000;
static constexpr int REVERSE_COUNT = 2;
} // namespace
RSAnimationFraction::RSAnimationFraction()
{
    currentIsReverseCycle_ = !isForward_;
}

void RSAnimationFraction::SetDirectionAfterStart(const ForwardDirection& direction)
{
    direction_ = direction;
}

void RSAnimationFraction::SetLastFrameTime(int64_t lastFrameTime)
{
    lastFrameTime_ = lastFrameTime;
}

float RSAnimationFraction::GetAnimationFraction(int64_t time, bool& isInstartDelay, bool& isFinished)
{
    int64_t durationNs = duration_ * MS_TO_NS;
    int64_t startDelayNs = startDelay_ * MS_TO_NS;
    int64_t deltaTime = time - lastFrameTime_;
    lastFrameTime_ = time;
    isInstartDelay = false;
    if (durationNs <= 0 || (repeatCount_ <= 0 && repeatCount_ != INFINITE)) {
        isFinished = true;
        return GetEndFraction();
    }
    // 1. Calculates the total running fraction of animation
    if (direction_ == ForwardDirection::NORMAL) {
        runningTime_ += static_cast<int64_t>(deltaTime * speed_);
    } else {
        runningTime_ -= static_cast<int64_t>(deltaTime * speed_);
    }
    if (runningTime_ < startDelayNs) {
        isFinished = IsFinished();
        isInstartDelay = isFinished ? false : true;
        return GetStartFraction();
    }

    // 2. Calculate the running time of the current cycle animation.
    int64_t realPlayTime = runningTime_ - startDelayNs - (curRepeatCount_ * durationNs);

    // 3. Update the number of cycles and the corresponding animation fraction.
    if (direction_ == ForwardDirection::NORMAL) {
        curRepeatCount_ += realPlayTime / durationNs;
    } else {
        while (curRepeatCount_ > 0 && realPlayTime < 0) {
            curRepeatCount_--;
            realPlayTime += durationNs;
        }
    }
    playTime_ = realPlayTime % durationNs;

    // 4. update status for auto reverse
    isFinished = IsFinished();
    UpdateReverseState(isFinished);

    // 5. get final animation fraction
    if (isFinished) {
        return GetEndFraction();
    }
    curTimeFraction_ = static_cast<float>(playTime_) / durationNs;
    curTimeFraction_ = currentIsReverseCycle_ ? (1.0f - curTimeFraction_) : curTimeFraction_;
    curTimeFraction_ = std::min(std::max(curTimeFraction_, 0.0f), 1.0f);
    return curTimeFraction_;
}

bool RSAnimationFraction::IsFinished() const
{
    if (direction_ == ForwardDirection::NORMAL) {
        if (repeatCount_ == INFINITE) {
            return false;
        }
        int64_t totalDuration = (duration_ * repeatCount_ + startDelay_) * MS_TO_NS;
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
            finish ? (curRepeatCount_ % REVERSE_COUNT == 0) : (curRepeatCount_ % REVERSE_COUNT == 1);
    } else {
        if (!autoReverse_) {
            currentIsReverseCycle_ = true;
            return;
        }
        currentIsReverseCycle_ =
            finish ? (curRepeatCount_ % REVERSE_COUNT == 1) : (curRepeatCount_ % REVERSE_COUNT == 0);
    }
}

void RSAnimationFraction::UpdateRemainTimeFraction(float fraction, int remainTime)
{
    int64_t remainTimeNS = remainTime * MS_TO_NS;
    int64_t durationNs = duration_ * MS_TO_NS;
    int64_t startDelayNs = startDelay_ * MS_TO_NS;
    float curRemianProgress = currentIsReverseCycle_ ? curTimeFraction_ : (1.0f - curTimeFraction_);
    float ratio = 1.0f;
    if (remainTime != 0) {
        ratio = curRemianProgress * durationNs / remainTimeNS;
    }

    if (runningTime_ > startDelayNs || fabs(fraction) > 1e-6) {
        if (currentIsReverseCycle_) {
            runningTime_ =
                static_cast<int64_t>(durationNs * (1.0f - fraction)) + startDelayNs + curRepeatCount_ * durationNs;
        } else {
            runningTime_ = static_cast<int64_t>(durationNs * fraction) + startDelayNs + curRepeatCount_ * durationNs;
        }
    }

    speed_ = speed_ * ratio;
    curTimeFraction_ = fraction;
}
} // namespace Rosen
} // namespace OHOS
