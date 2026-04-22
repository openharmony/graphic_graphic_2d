/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H

#include <refbase.h>
#include <unordered_map>
#include <vector>

#include "animation/rs_animation_common.h"
#include "animation/rs_animation_fraction.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSContext;
class RSRenderAnimation;

enum class GroupAnimatorState {
    INITIALIZED,
    RUNNING,
    PAUSED,
    FINISHED,
};

class RSB_EXPORT RSRenderInteractiveImplictAnimator
    : public std::enable_shared_from_this<RSRenderInteractiveImplictAnimator> {
public:
    explicit RSRenderInteractiveImplictAnimator(
        InteractiveImplictAnimatorId id, const std::weak_ptr<RSContext>& context = {});
    virtual ~RSRenderInteractiveImplictAnimator() = default;

    void AddAnimations(std::vector<std::pair<NodeId, AnimationId>> animations);

    virtual void PauseAnimator();
    virtual void ContinueAnimator();
    virtual void FinishAnimator(RSInteractiveAnimationPosition finishPos);
    virtual void ReverseAnimator();
    virtual void SetFractionAnimator(float fraction);

    inline InteractiveImplictAnimatorId GetId() const { return id_; }

    virtual bool IsTimeDriven() const { return false; }

    virtual bool IsRunning() const { return false; }

protected:
    explicit RSRenderInteractiveImplictAnimator() = default;

    InteractiveImplictAnimatorId id_ = 0;
    std::weak_ptr<RSContext> context_ = {};
    std::vector<std::weak_ptr<RSRenderAnimation>> cachedAnimations_;

    friend class RSContext;
};

class RSB_EXPORT RSRenderTimeDrivenGroupAnimator : public RSRenderInteractiveImplictAnimator {
public:
    RSRenderTimeDrivenGroupAnimator(InteractiveImplictAnimatorId id,
        const std::weak_ptr<RSContext>& context, const RSAnimationTimingProtocol& timingProtocol);
    ~RSRenderTimeDrivenGroupAnimator() override = default;

    void OnAnimate(int64_t timestamp, int64_t& minLeftDelayTime);
    void SetStartTime(int64_t time);

    bool IsTimeDriven() const override { return true; }

    bool IsRunning() const override { return state_ == GroupAnimatorState::RUNNING; }

    const RSAnimationTimingProtocol& GetTimingProtocol() const { return timingProtocol_; }

    void StartAnimator();
    void PauseAnimator() override;
    void ContinueAnimator() override;
    void FinishAnimator(RSInteractiveAnimationPosition finishPos) override;
    void RemoveActiveChildAnimation(AnimationId animationId);

private:
    bool IsStarted() const { return state_ != GroupAnimatorState::INITIALIZED; }
    RSAnimationTimingProtocol timingProtocol_;
    GroupAnimatorState state_ = GroupAnimatorState::INITIALIZED;
    bool needUpdateStartTime_ = false;
    RSAnimationFraction animationFraction_;

    // Record GROUP_WAITING animations before pause for restoration on resume
    std::unordered_set<AnimationId> groupWaitingAnimationIds_;

    // Track active child animations (empty when all finished)
    std::unordered_set<AnimationId> activeChildAnimations_;

    void StartChildAnimations();
    void ResetChildAnimations();
    void UpdateFraction(int64_t timestamp, int64_t& minLeftDelayTime);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H
