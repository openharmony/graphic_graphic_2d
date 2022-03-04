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

#include "animation/rs_render_animation.h"

#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderAnimation::RSRenderAnimation(AnimationId id) : id_(id) {}
#ifdef ROSEN_OHOS
bool RSRenderAnimation::Marshalling(Parcel& parcel) const
{
    // animationId, targetId
    if (!(parcel.WriteUint64(id_))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write id failed");
        return false;
    }
    // RSAnimationTimingProtocal
    if (!(parcel.WriteInt32(animationFraction_.GetDuration()) &&
        parcel.WriteInt32(animationFraction_.GetStartDelay()) &&
        parcel.WriteFloat(animationFraction_.GetSpeed()) &&
        parcel.WriteInt32(animationFraction_.GetRepeatCount()) &&
        parcel.WriteBool(animationFraction_.GetAutoReverse()) &&
        parcel.WriteBool(animationFraction_.GetDirection()) &&
        parcel.WriteInt32(static_cast<std::underlying_type<FillMode>::type>(animationFraction_.GetFillMode())))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write param failed");
        return false;
    }
    return true;
}

bool RSRenderAnimation::ParseParam(Parcel& parcel)
{
    int32_t duration = 0;
    int32_t startDelay = 0;
    int32_t repeatCount = 0;
    int32_t fillMode = 0;
    float speed = 0.0;
    bool autoReverse = false;
    bool direction = false;
    if (!(parcel.ReadUint64(id_) && parcel.ReadInt32(duration) && parcel.ReadInt32(startDelay) &&
            parcel.ReadFloat(speed) && parcel.ReadInt32(repeatCount) && parcel.ReadBool(autoReverse) &&
            parcel.ReadBool(direction) && parcel.ReadInt32(fillMode))) {
        ROSEN_LOGE("RSRenderAnimation::ParseParam, read param failed");
        return false;
    }
    SetDuration(duration);
    SetStartDelay(startDelay);
    SetRepeatCount(repeatCount);
    SetAutoReverse(autoReverse);
    SetSpeed(speed);
    SetDirection(direction);
    SetFillMode(static_cast<FillMode>(fillMode));
    return true;
}
#endif
AnimationId RSRenderAnimation::GetAnimationId() const
{
    return id_;
}

bool RSRenderAnimation::IsStarted() const
{
    return state_ != AnimationState::INITIALIZED;
}

bool RSRenderAnimation::IsRunning() const
{
    return state_ == AnimationState::RUNNING;
}

bool RSRenderAnimation::IsPaused() const
{
    return state_ == AnimationState::PAUSED;
}

bool RSRenderAnimation::IsFinished() const
{
    return state_ == AnimationState::FINISHED;
}

RSAnimatableProperty RSRenderAnimation::GetProperty() const
{
    return RSAnimatableProperty::INVALID;
}

void RSRenderAnimation::Attach(RSRenderNode* renderNode)
{
    target_ = renderNode;
    OnAttach();
}

void RSRenderAnimation::Detach()
{
    OnDetach();
    target_ = nullptr;
}

void RSRenderAnimation::Start()
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to start animation, animation has started!");
        return;
    }

    state_ = AnimationState::RUNNING;
    firstToRunning_ = true;
    ProcessFillModeOnStart(animationFraction_.GetStartFraction());
}

void RSRenderAnimation::Finish()
{
    if (!IsPaused() && !IsRunning()) {
        ROSEN_LOGE("Failed to finish animation, animation is not running!");
        return;
    }

    state_ = AnimationState::FINISHED;
    ProcessFillModeOnFinish(animationFraction_.GetEndFraction());
}

void RSRenderAnimation::Pause()
{
    if (!IsRunning()) {
        ROSEN_LOGE("Failed to pause animation, animation is not running!");
        return;
    }

    state_ = AnimationState::PAUSED;
}

void RSRenderAnimation::Resume()
{
    if (!IsPaused()) {
        ROSEN_LOGE("Failed to resume animation, animation is not paused!");
        return;
    }

    state_ = AnimationState::RUNNING;
    firstToRunning_ = true;
}

void RSRenderAnimation::SetFraction(float fraction)
{
    if (!IsPaused()) {
        ROSEN_LOGE("Failed to set fraction, animation is not paused!");
        return;
    }

    fraction = std::min(std::max(fraction, 0.0f), 1.0f);
    OnSetFraction(fraction);
}

void RSRenderAnimation::SetReversed(bool isReversed)
{
    if (!IsPaused() && !IsRunning()) {
        ROSEN_LOGE("Failed to reverse animation, animation is not running!");
        return;
    }

    animationFraction_.SetDirectionAfterStart(isReversed ? ForwardDirection::REVERSE : ForwardDirection::NORMAL);
}

RSRenderNode* RSRenderAnimation::GetTarget() const
{
    return target_;
}

void RSRenderAnimation::SetFractionInner(float fraction)
{
    animationFraction_.UpdateRemainTimeFraction(fraction);
}

void RSRenderAnimation::OnSetFraction(float fraction)
{
    SetFractionInner(fraction);
}

void RSRenderAnimation::ProcessFillModeOnStart(float startFraction)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::BACKWARDS || fillMode == FillMode::BOTH) {
        OnAnimate(startFraction);
    }
}

void RSRenderAnimation::ProcessFillModeOnFinish(float endFraction)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::FORWARDS || fillMode == FillMode::BOTH) {
        OnAnimate(endFraction);
    } else {
        OnRemoveOnCompletion();
    }
}

bool RSRenderAnimation::Animate(int64_t time)
{
    if (!IsRunning()) {
        ROSEN_LOGI("RSRenderAnimation::Animate, IsRunning is false!");
        return state_ == AnimationState::FINISHED;
    }

    if (firstToRunning_) {
        animationFraction_.SetLastFrameTime(time);
        firstToRunning_ = false;
    }

    bool isInStartDelay = false;
    bool isFinished = false;
    float fraction = animationFraction_.GetAnimationFraction(time, isInStartDelay, isFinished);
    if (isInStartDelay) {
        ProcessFillModeOnStart(fraction);
        ROSEN_LOGI("RSRenderAnimation::Animate, isInStartDelay is true");
        return false;
    }

    OnAnimate(fraction);
    if (isFinished) {
        ProcessFillModeOnFinish(fraction);
        ROSEN_LOGI("RSRenderAnimation::Animate, isFinished is true");
        return true;
    }
    return isFinished;
}
} // namespace Rosen
} // namespace OHOS
