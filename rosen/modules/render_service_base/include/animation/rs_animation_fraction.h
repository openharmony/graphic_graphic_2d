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

    /**
     * @brief Normalize the animation time percentage into a fraction
     * @param time Vsync time
     * @param minLeftDelayTime [in/out] minimum left delay time in ms. For nodes on tree, delayTime only.
     *        For nodes not on tree, delayTime + remainingTime.
     * @param isCustom whether this is a custom animation
     * @param isOnTree whether the target node is on the render tree
     * @return tuple<fraction, isInStartDelay, isFinished, isRepeatFinished>
     */
    std::tuple<float, bool, bool, bool> GetAnimationFraction(
        int64_t time, int64_t& minLeftDelayTime, bool isCustom, bool isOnTree);
    void UpdateRemainTimeFraction(float fraction, int remainTime = 0);
    float GetStartFraction() const;
    float GetEndFraction() const;
    void SetDirectionAfterStart(const ForwardDirection& direction);
    void FlipDirection();
    void SetLastFrameTime(int64_t lastFrameTime);
    int64_t GetLastFrameTime() const;
    void ResetFraction();
    int GetRemainingRepeatCount() const;
    bool GetCurrentIsReverseCycle() const;
    float GetCurrentTimeFraction() const { return currentTimeFraction_; }

    void SetRepeatCallbackEnable(bool isEnable) { isRepeatCallbackEnable_ = isEnable; }

    bool GetRepeatCallbackEnable() const { return isRepeatCallbackEnable_; }

    int64_t GetRunningTime() const { return runningTime_; }

    bool UpdateGroupWaitingTime(int64_t deltaTime, bool isCustom);

private:
    bool IsInRepeat() const;
    bool IsFinished(bool isCustom) const;
    void UpdateReverseState(bool finish);
    bool IsStartRunning(const int64_t deltaTime, const int64_t startDelayNs, bool isCustom);
    int64_t CalculateLeftDelayTime(const int64_t startDelayNs, bool isCustom);
    /**
     * @brief Handle animation start delay phase
     * @param startDelayNs Start delay time in nanoseconds
     * @param deltaTime Time delta from last frame in nanoseconds
     * @param isCustom Whether this is a custom animation
     * @param minLeftDelayTime [in/out] Minimum left delay time in ms. For on-tree nodes: delayTime only;
     *        for off-tree nodes: delayTime + duration * repeatCount (total estimated time)
     * @param isInStartDelay [out] Whether animation is in start delay phase
     * @param isOnTree Whether the target node is on the render tree
     * @return Optional tuple containing {fraction, isInStartDelay, isFinished, isRepeatFinished} if in delay phase;
     *         nullopt if delay phase has passed
     */
    std::optional<std::tuple<float, bool, bool, bool>> HandleStartDelayPhase(int64_t startDelayNs, int64_t deltaTime,
        bool isCustom, int64_t& minLeftDelayTime, bool& isInStartDelay, bool isOnTree);
    void CalculateRemainingTime(int64_t durationNs, int64_t& minLeftDelayTime, bool isOnTree);

    static std::atomic<float> animationScale_;
    static std::atomic<bool> isInitialized_;

    ForwardDirection direction_ { ForwardDirection::NORMAL };
    int64_t playTime_ { 0 };
    float currentTimeFraction_ { 0.0f };
    int currentRepeatCount_ { 0 };
    // the animation's running duration: Unit in ns
    int64_t runningTime_ { 0 };
    bool currentIsReverseCycle_ { false };
    int64_t lastFrameTime_ { -1 };
    bool isRepeatCallbackEnable_ {false};
    // Time accumulated during GROUP_WAITING state, used for autoReverse delay(Unit in ns)
    int64_t groupWaitingTime_ { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_FRACTION_H
