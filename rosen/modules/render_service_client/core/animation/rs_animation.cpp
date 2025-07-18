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

#include "animation/rs_animation.h"

#include "sandbox_utils.h"

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_common.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {

AnimationId RSAnimation::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        ROSEN_LOGE("Animation Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((AnimationId)pid_ << 32) | (currentId);
}

RSAnimation::RSAnimation() : id_(GenerateId()) {}

RSAnimation::~RSAnimation()
{
    RSNodeMap::MutableInstance().UnregisterAnimation(id_);
}

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
    auto target = target_.lock();
    if (target != nullptr) {
        RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(
            "Animation Set FinishCallback", target->GetId(), id_, true);
    }
}

void RSAnimation::SetRepeatCallback(const std::shared_ptr<AnimationRepeatCallback>& repeatCallback)
{
    repeatCallback_ = repeatCallback;
}

void RSAnimation::SetInteractiveFinishCallback(
    const std::shared_ptr<InteractiveAnimatorFinishCallback>& finishCallback)
{
    interactiveFinishCallback_ = finishCallback;
}

void RSAnimation::CallFinishCallback()
{
    finishCallback_.reset();
    interactiveFinishCallback_.reset();
    state_ = AnimationState::FINISHED;
    OnCallFinishCallback();
    auto target = target_.lock();
    if (target == nullptr) {
        return;
    }
#if defined(MODIFIER_NG)
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(target->GetId(), id_, GetPropertyType(), true);
#else
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(target->GetId(), id_, GetModifierType(), true);
#endif
}

void RSAnimation::CallRepeatCallback()
{
    if (repeatCallback_ == nullptr) {
        return;
    }
    repeatCallback_->Execute();
}

void RSAnimation::CallLogicallyFinishCallback()
{
    finishCallback_.reset();
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
        ROSEN_LOGD("State error, animation is in [%{public}d] when start.", state_);
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
        ROSEN_LOGD("State error, animation is in [%{public}d] when pause", state_);
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
    if (uiAnimation_ != nullptr) {
        uiAnimation_->Pause();
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to pause animation, target is null!");
        return;
    }
    RS_LOGI_LIMIT("Animation[%{public}" PRIu64 "] send pause", id_);
    RS_TRACE_NAME_FMT("Animation[%llu] send pause", id_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationPause>(target->GetId(), id_);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSAnimationPause>(target->GetId(), id_);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
    if (finishCallback_) {
        finishCallback_->SetAnimationBeenPaused();
    }
}

bool RSAnimation::IsUiAnimation() const
{
    return uiAnimation_ != nullptr;
}

void RSAnimation::InteractivePause()
{
    if (state_ != AnimationState::RUNNING) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when pause", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to interactive pause animation, target is null!");
        return;
    }

    state_ = AnimationState::PAUSED;

    if (uiAnimation_ != nullptr) {
        uiAnimation_->Pause();
    }
    if (finishCallback_) {
        finishCallback_->SetAnimationBeenPaused();
    }
}

void RSAnimation::InteractiveContinue()
{
    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when continue", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to interactive continue animation, target is null!");
        return;
    }

    state_ = AnimationState::RUNNING;

    if (uiAnimation_ != nullptr) {
        uiAnimation_->Resume();
    }
}

void RSAnimation::InteractiveFinish(RSInteractiveAnimationPosition pos)
{
    if (state_ != AnimationState::RUNNING && state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("Animation is in [%{public}d] when Finish", state_);
        return;
    }
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to interactive finish animation, target is null!");
        return;
    }
    state_ = AnimationState::FINISHED;

    if (uiAnimation_ != nullptr) {
        uiAnimation_->FinishOnPosition(pos);
    }
    UpdateStagingValueOnInteractiveFinish(pos);
}

void RSAnimation::InteractiveReverse()
{
    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("Animation is in [%{public}d] when Reverse", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to pause animation, target is null!");
        return;
    }
    isReversed_ = true;
    state_ = AnimationState::RUNNING;

    OnUpdateStagingValue(false);

    if (uiAnimation_ != nullptr) {
        uiAnimation_->SetReversedAndContinue();
        return;
    }
}

void RSAnimation::InteractiveSetFraction(float fraction)
{
    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when pause", state_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to pause animation, target is null!");
        return;
    }

    if (uiAnimation_ != nullptr) {
        uiAnimation_->SetFraction(fraction);
    }
}

void RSAnimation::Resume()
{
    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when Resume", state_);
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
    if (uiAnimation_ != nullptr) {
        uiAnimation_->Resume();
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to resume animation, target is null!");
        return;
    }
    RS_LOGI_LIMIT("Animation[%{public}" PRIu64 "] send resume", id_);
    RS_TRACE_NAME_FMT("Animation[%llu] send resume", id_);

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationResume>(target->GetId(), id_);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSAnimationResume>(target->GetId(), id_);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}

void RSAnimation::Finish()
{
    if (state_ != AnimationState::RUNNING && state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("Animation is in [%{public}d] when Finish", state_);
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
    if (uiAnimation_ != nullptr) {
        uiAnimation_->Finish();
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to finish animation, target is null!");
        return;
    }
    RS_LOGI_LIMIT("Animation[%{public}" PRIu64 "] send finish", id_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationFinish>(target->GetId(), id_);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSAnimationFinish>(target->GetId(), id_);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}

void RSAnimation::Reverse()
{
    if (state_ != AnimationState::RUNNING && state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when Reverse", state_);
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
    if (uiAnimation_ != nullptr) {
        uiAnimation_->SetReversed(isReversed_);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to reverse animation, target is null!");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationReverse>(target->GetId(), id_, isReversed_);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSAnimationReverse>(target->GetId(), id_, isReversed_);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}

void RSAnimation::SetFraction(float fraction)
{
    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
        ROSEN_LOGE("Fraction[%{public}f] is invalid!", fraction);
        return;
    }

    if (state_ != AnimationState::PAUSED) {
        ROSEN_LOGD("State error, animation is in [%{public}d] when SetFraction", state_);
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
    if (uiAnimation_ != nullptr) {
        uiAnimation_->SetFraction(fraction);
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to set fraction, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationSetFraction>(target->GetId(), id_, fraction);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSAnimationSetFraction>(target->GetId(), id_, fraction);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}

PropertyId RSAnimation::GetPropertyId() const
{
    return 0;
}

void RSAnimation::UpdateStagingValue(bool isFirstStart)
{
    OnUpdateStagingValue(isFirstStart);
}

void RSAnimation::UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGD("Animation is null, failed to update param.");
        return;
    }
    animation->SetDuration(GetDuration());
    animation->SetStartDelay(GetStartDelay());
    animation->SetRepeatCount(GetRepeatCount());
    animation->SetAutoReverse(GetAutoReverse());
    animation->SetSpeed(GetSpeed());
    animation->SetDirection(GetDirection());
    animation->SetFillMode(GetFillMode());
    animation->SetRepeatCallbackEnable(repeatCallback_ != nullptr);
    // only process FrameRateRange(rs) here
    if (uiAnimation_ == nullptr) {
        auto range = GetFrameRateRange();
        // Transfer frame rate and component informations
        if (range.IsValid() || range.componentScene_ != ComponentScene::UNKNOWN_SCENE) {
            animation->SetFrameRateRange(range);
        }
    }
    // set token to RSRenderAnimation
    if (auto target = target_.lock()) {
        if (auto context = target->GetRSUIContext()) {
            animation->SetToken(context->GetToken());
        }
    } else {
        ROSEN_LOGE("multi-instance, RSAnimation::UpdateParamToRenderAnimation, target is null!");
    }
}

void RSAnimation::StartCustomAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("multi-instance, RSAnimation::StartCustomAnimation, target is null!");
        return;
    }
    auto rsUIContext = target->GetRSUIContext();
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                       : RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager == nullptr || animation == nullptr) {
        ROSEN_LOGE("Failed to start custom animation, modifier manager is null  animationId: %{public}" PRIu64 "!",
            GetId());
        return;
    }

    uiAnimation_ = animation;
    animation->targetId_ = target->GetId();
    animation->Start();
    modifierManager->AddAnimation(animation);
}

std::string RSAnimation::DumpAnimation() const
{
    std::string dumpInfo;
    dumpInfo.append("[id:").append(std::to_string(GetId()));
    dumpInfo.append(", AnimationState:").append(std::to_string(static_cast<int>(state_)));
    DumpAnimationInfo(dumpInfo);
    dumpInfo.append(", Duration:").append(std::to_string(GetDuration()));
    dumpInfo.append(", StartDelay:").append(std::to_string(GetStartDelay()));
    dumpInfo.append(", RepeatCount:").append(std::to_string(GetRepeatCount())).append("]");
    return dumpInfo;
}

} // namespace Rosen
} // namespace OHOS
