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

#include "animation/rs_animation.h"

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_common.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

AnimationId RSAnimation::GenerateId()
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> currentId_ = 0;

    ++currentId_;
    if (currentId_ == UINT32_MAX) {
        // TODO:handle the overflow situation
        ROSEN_LOGE("Animation Id overflow");
    }

    return ((AnimationId)pid_ << 32) | (currentId_);
}

RSAnimation::RSAnimation() : id_(GenerateId()) {}

void RSAnimation::SetFinishCallback(const std::function<void()>& finishCallback)
{
    if (finishCallback == nullptr) {
        ROSEN_LOGE("Failed to set finish callback, callback is null!");
        return;
    }

    SetFinishCallback(std::make_shared<AnimationFinishCallback>(finishCallback));
}

void RSAnimation::SetFinishCallback(const std::shared_ptr<AnimationFinishCallback>& finishCallback)
{
    finishCallback_ = finishCallback;
}

void RSAnimation::CallFinishCallback()
{
    finishCallback_.reset();
    state_ = AnimationState::FINISHED;
}

AnimationId RSAnimation::GetId() const
{
    return id_;
}

bool RSAnimation::IsStarted() const
{
    return state_ != AnimationState::INITIALIZED;
}

bool RSAnimation::IsRunning() const
{
    return state_ == AnimationState::RUNNING;
}

bool RSAnimation::IsPaused() const
{
    return state_ == AnimationState::PAUSED;
}

bool RSAnimation::IsFinished() const
{
    return state_ == AnimationState::FINISHED;
}

void RSAnimation::Start(const std::shared_ptr<RSNode>& target)
{
    if (state_ != AnimationState::INITIALIZED) {
        ROSEN_LOGE("State error, animation is in [%d] when start.", state_);
        return;
    }

    if (target == nullptr) {
        ROSEN_LOGE("Failed to start animation, target is null!");
        return;
    }

    target->AddAnimation(shared_from_this());
}

void RSAnimation::StartInner(const std::shared_ptr<RSNode>& target)
{
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start animation, target is null!");
        return;
    }

    target_ = target;
    state_ = AnimationState::RUNNING;
    OnStart();
    UpdateStagingValue(true);
}

bool RSAnimation::IsReversed() const
{
    return isReversed_;
}

const std::weak_ptr<RSNode> RSAnimation::GetTarget() const
{
    return target_;
}

void RSAnimation::Pause()
{
    if (state_ != AnimationState::RUNNING) {
        ROSEN_LOGE("State error, animation is in [%d] when pause", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to pause animation, target is null!");
        return;
    }

    state_ = AnimationState::PAUSED;
    OnPause();
}

void RSAnimation::OnPause()
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to pause animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationPause>(target->GetId(), id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}

void RSAnimation::Resume()
{
    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGE("State error, animation is in [%d] when Resume", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to resume animation, target is null!");
        return;
    }

    state_ = AnimationState::RUNNING;
    OnResume();
}

void RSAnimation::OnResume()
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to resume animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationResume>(target->GetId(), id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}

void RSAnimation::Finish()
{
    if (state_ != AnimationState::RUNNING && state_ != AnimationState::PAUSED) {
        ROSEN_LOGE("State error, animation is in [%d] when Finish", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to finish animation, target is null!");
        return;
    }

    state_ = AnimationState::FINISHED;
    OnFinish();
}

void RSAnimation::OnFinish()
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to finish animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationFinish>(target->GetId(), id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}

void RSAnimation::Reverse()
{
    if (state_ != AnimationState::RUNNING && state_ != AnimationState::PAUSED) {
        ROSEN_LOGE("State error, animation is in [%d] when Reverse", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to reverse animation, target is null!");
        return;
    }

    isReversed_ = !isReversed_;
    OnReverse();
    UpdateStagingValue(false);
}

void RSAnimation::OnReverse()
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to reverse animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationReverse>(target->GetId(), id_, isReversed_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}

void RSAnimation::SetFraction(float fraction)
{
    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
        ROSEN_LOGE("Fraction[%f] is invalid!", fraction);
        return;
    }

    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGE("State error, animation is in [%d] when SetFraction", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to set fraction, target is null!");
        return;
    }

    OnSetFraction(fraction);
}

void RSAnimation::OnSetFraction(float fraction)
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to set fraction, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationSetFraction>(target->GetId(), id_, fraction);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode());
    }
}

RSAnimatableProperty RSAnimation::GetProperty() const
{
    return RSAnimatableProperty::INVALID;
}

void RSAnimation::UpdateStagingValue(bool isFirstStart)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::FORWARDS || fillMode == FillMode::BOTH) {
        OnUpdateStagingValue(isFirstStart);
    }
}
} // namespace Rosen
} // namespace OHOS
