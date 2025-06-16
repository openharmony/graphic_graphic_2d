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

#include "animation/rs_interactive_implict_animator.h"

#include "modifier/rs_extended_modifier.h"
#include "platform/common/rs_log.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
enum class StartAnimationErrorCode : int32_t {
    SUCCESS = 0,
    INVALID_STATUS,
    INVALID_ANIMATIONS,
    INVALID_PROXY,
    INVALID_CONTEXT,
};

static bool g_isUniRenderEnabled = false;

InteractiveImplictAnimatorId RSInteractiveImplictAnimator::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        ROSEN_LOGE("InteractiveImplictAnimatorId Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((InteractiveImplictAnimatorId)pid_ << 32) | (currentId);
}

std::shared_ptr<RSInteractiveImplictAnimator> RSInteractiveImplictAnimator::Create(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
{
    return std::shared_ptr<RSInteractiveImplictAnimator>(
        new RSInteractiveImplictAnimator(nullptr, timingProtocol, timingCurve));
}

std::shared_ptr<RSInteractiveImplictAnimator> RSInteractiveImplictAnimator::Create(
    const std::shared_ptr<RSUIContext> rsUIContext, const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve)
{
    return std::shared_ptr<RSInteractiveImplictAnimator>(
        new RSInteractiveImplictAnimator(rsUIContext, timingProtocol, timingCurve));
}

RSInteractiveImplictAnimator::RSInteractiveImplictAnimator(const std::shared_ptr<RSUIContext> rsUIContext,
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
    : id_(GenerateId()), rsUIContext_(rsUIContext), timingProtocol_(timingProtocol), timingCurve_(timingCurve)
{
    InitUniRenderEnabled();
}

RSInteractiveImplictAnimator::~RSInteractiveImplictAnimator()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorDestory>(id_);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSInteractiveAnimatorDestory>(id_);
        AddCommand(commandForRemote, true);
    }
}

bool RSInteractiveImplictAnimator::IsUniRenderEnabled() const
{
    return g_isUniRenderEnabled;
}

void RSInteractiveImplictAnimator::InitUniRenderEnabled()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        g_isUniRenderEnabled = RSSystemProperties::GetUniRenderEnabled();
        ROSEN_LOGD("RSInteractiveImplictAnimator::InitUniRenderEnabled:%{public}d", g_isUniRenderEnabled);
    }
}

std::shared_ptr<InteractiveAnimatorFinishCallback> RSInteractiveImplictAnimator::GetAnimatorFinishCallback()
{
    auto callback = animatorCallback_.lock();
    if (callback) {
        return callback;
    }
    auto animatorCallback = std::make_shared<InteractiveAnimatorFinishCallback>([weak = weak_from_this()]() {
        auto self = weak.lock();
        if (self) {
            self->CallFinishCallback();
        }
    });
    animatorCallback_ = animatorCallback;
    return animatorCallback;
}

size_t RSInteractiveImplictAnimator::AddImplictAnimation(std::function<void()> callback)
{
    if (state_ != RSInteractiveAnimationState::INACTIVE && state_ != RSInteractiveAnimationState::ACTIVE) {
        ROSEN_LOGE("AddAnimation failed, state_ is error");
        return 0;
    }

    auto rsUIContext = rsUIContext_.lock();
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator() :
        RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return 0;
    }

    if (timingProtocol_.GetDuration() <= 0) {
        return 0;
    }

    implicitAnimator->OpenInterActiveImplicitAnimation(true, timingProtocol_, timingCurve_, nullptr);
    callback();
    auto animations = implicitAnimator->CloseInterActiveImplicitAnimation(true);

    for (auto& [animation, nodeId] : animations) {
        if (fractionAnimationId_ == 0 && animation->IsSupportInteractiveAnimator()) {
            fractionAnimationId_ = animation->GetId();
            fractionNodeId_ = nodeId;
        }
        std::weak_ptr<RSAnimation> weakAnimation = animation;
        animation->SetInteractiveFinishCallback(GetAnimatorFinishCallback());
        animations_.emplace_back(weakAnimation, nodeId);
    }
    state_ = RSInteractiveAnimationState::ACTIVE;
    return animations.size();
}

size_t RSInteractiveImplictAnimator::AddAnimation(std::function<void()> callback)
{
    if (state_ != RSInteractiveAnimationState::INACTIVE && state_ != RSInteractiveAnimationState::ACTIVE) {
        ROSEN_LOGE("AddAnimation failed, state_ is error");
        return 0;
    }
    auto rsUIContext = rsUIContext_.lock();
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator() :
        RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return 0;
    }

    implicitAnimator->OpenInterActiveImplicitAnimation(false, timingProtocol_, timingCurve_, nullptr);
    callback();
    auto animations = implicitAnimator->CloseInterActiveImplicitAnimation(false);

    for (auto& [animation, nodeId] : animations) {
        if (fractionAnimationId_ == 0 && animation->IsSupportInteractiveAnimator()) {
            fractionAnimationId_ = animation->GetId();
            fractionNodeId_ = nodeId;
        }
        std::weak_ptr<RSAnimation> weakAnimation = animation;
        animation->SetInteractiveFinishCallback(GetAnimatorFinishCallback());
        animations_.emplace_back(weakAnimation, nodeId);
    }
    state_ = RSInteractiveAnimationState::ACTIVE;
    return animations.size();
}

int32_t RSInteractiveImplictAnimator::StartAnimation()
{
    if (state_ != RSInteractiveAnimationState::ACTIVE) {
        ROSEN_LOGE("StartAnimation failed, state_ is error");
        return static_cast<int32_t>(StartAnimationErrorCode::INVALID_STATUS);
    }

    if (animations_.empty()) {
        ROSEN_LOGE("StartAnimation failed, animations size is error");
        return static_cast<int32_t>(StartAnimationErrorCode::INVALID_ANIMATIONS);
    }
    auto rsUIContext = rsUIContext_.lock();
    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (target != nullptr && animation != nullptr) {
            animation->InteractiveContinue();
            if (!animation->IsUiAnimation()) {
                renderAnimations.emplace_back(nodeId, animation->GetId());
            }
        }
    }
    state_ = RSInteractiveAnimationState::RUNNING;

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("Failed to start interactive animation, transaction proxy is null!");
        return static_cast<int32_t>(StartAnimationErrorCode::INVALID_PROXY);
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorCreate>(id_, renderAnimations, true);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSInteractiveAnimatorCreate>(id_, renderAnimations, true);
        AddCommand(commandForRemote, true);
    }
    return static_cast<int32_t>(StartAnimationErrorCode::SUCCESS);
}

void RSInteractiveImplictAnimator::PauseAnimation()
{
    if (state_ != RSInteractiveAnimationState::RUNNING) {
        ROSEN_LOGE("PauseAnimation failed, state_ is error");
        return;
    }
    state_ = RSInteractiveAnimationState::PAUSED;

    auto rsUIContext = rsUIContext_.lock();
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (target != nullptr && animation != nullptr) {
            animation->InteractivePause();
        }
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("RSTransactionProxy is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorPause>(id_);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSInteractiveAnimatorPause>(id_);
        AddCommand(commandForRemote, true);
    }
}

void RSInteractiveImplictAnimator::ContinueAnimation()
{
    if (state_ != RSInteractiveAnimationState::PAUSED) {
        ROSEN_LOGE("ContinueAnimation failed, state_ is error");
        return;
    }

    state_ = RSInteractiveAnimationState::RUNNING;

    auto rsUIContext = rsUIContext_.lock();
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (target != nullptr && animation != nullptr) {
            animation->InteractiveContinue();
        }
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("RSTransactionProxy is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorContinue>(id_);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSInteractiveAnimatorContinue>(id_);
        AddCommand(commandForRemote, true);
    }
}

void RSInteractiveImplictAnimator::FinishAnimation(RSInteractiveAnimationPosition position)
{
    if (state_ != RSInteractiveAnimationState::RUNNING && state_ != RSInteractiveAnimationState::PAUSED) {
        ROSEN_LOGE("FinishAnimation failed, state_ is error");
        return;
    }
    state_ = RSInteractiveAnimationState::INACTIVE;

    auto rsUIContext = rsUIContext_.lock();
    if (position == RSInteractiveAnimationPosition::START || position == RSInteractiveAnimationPosition::END) {
        for (auto& [item, nodeId] : animations_) {
            auto animation = item.lock();
            auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
                RSNodeMap::Instance().GetNode<RSNode>(nodeId);
            if (target == nullptr || animation == nullptr) {
                continue;
            }
            animation->InteractiveFinish(position);
        }
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            ROSEN_LOGE("RSTransactionProxy is nullptr");
            return;
        }
        std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorFinish>(id_, position);
        AddCommand(command, IsUniRenderEnabled());
        if (!IsUniRenderEnabled()) {
            std::unique_ptr<RSCommand> commandForRemote =
                std::make_unique<RSInteractiveAnimatorFinish>(id_, position);
            AddCommand(commandForRemote, true);
        }
    } else if (position == RSInteractiveAnimationPosition::CURRENT) {
        FinishOnCurrent();
    }
}

void RSInteractiveImplictAnimator::FinishOnCurrent()
{
    auto rsUIContext = rsUIContext_.lock();
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto node = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (node == nullptr || animation == nullptr) {
            continue;
        }
        if (!node->HasPropertyAnimation(animation->GetPropertyId()) || animation->IsUiAnimation()) {
            continue;
        }
        propertiesMap.emplace(std::make_pair<NodeId, PropertyId>(node->GetId(), animation->GetPropertyId()),
            std::make_pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>(
                nullptr, {animation->GetId()}));
    }
    if (propertiesMap.size() == 0) {
        return;
    }
    auto task = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(propertiesMap));
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, IsUniRenderEnabled());
    if (!task || !task->IsSuccess()) {
        return;
    }
    for (const auto& [key, value] : task->GetProperties()) {
        const auto& [nodeId, propertyId] = key;
        auto node = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        std::shared_ptr<RSPropertyBase> property = nullptr;
        if (auto prop = node->GetPropertyById(propertyId)) {
            property = prop;
        } else if (auto modifier = node->GetModifier(propertyId)) {
            property = modifier->GetProperty();
        } else {
            property = nullptr;
        }
        if (property == nullptr) {
            continue;
        }
        const auto& [propertyValue, animations] = value;
        if (propertyValue != nullptr) {
            property->SetValueFromRender(propertyValue);
        }
    }
}

void RSInteractiveImplictAnimator::ReverseAnimation()
{
    if (state_ != RSInteractiveAnimationState::PAUSED) {
        ROSEN_LOGE("ReverseAnimation failed, state_ is error");
        return;
    }

    state_ = RSInteractiveAnimationState::RUNNING;

    auto rsUIContext = rsUIContext_.lock();
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (target != nullptr && animation != nullptr) {
            animation->InteractiveReverse();
        }
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("RSTransactionProxy is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorReverse>(id_);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSInteractiveAnimatorReverse>(id_);
        AddCommand(commandForRemote, true);
    }
}

void RSInteractiveImplictAnimator::SetFraction(float fraction)
{
    if (state_ != RSInteractiveAnimationState::PAUSED) {
        ROSEN_LOGE("SetFraction failed, state_ is error");
        return;
    }

    auto rsUIContext = rsUIContext_.lock();
    for (auto& [item, nodeId] : animations_) {
        auto animation = item.lock();
        auto target = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId) :
            RSNodeMap::Instance().GetNode<RSNode>(nodeId);
        if (target != nullptr && animation != nullptr) {
            animation->InteractiveSetFraction(fraction);
        }
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("RSTransactionProxy is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSInteractiveAnimatorSetFraction>(id_, fraction);
    AddCommand(command, IsUniRenderEnabled());
    if (!IsUniRenderEnabled()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSInteractiveAnimatorSetFraction>(id_, fraction);
        AddCommand(commandForRemote, true);
    }
}

float RSInteractiveImplictAnimator::GetFraction()
{
    if (state_ != RSInteractiveAnimationState::PAUSED) {
        ROSEN_LOGE("GetFraction failed, state_ is error");
        return 0.0f;
    }
    if (fractionAnimationId_ == 0 || fractionNodeId_ == 0) {
        ROSEN_LOGE("GetFraction failed, id is error");
        return 0.0f;
    }
    auto task = std::make_shared<RSNodeGetAnimationsValueFraction>(1e8, fractionNodeId_, fractionAnimationId_);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, IsUniRenderEnabled());

    if (!task || !task->IsSuccess()) {
        ROSEN_LOGE("RSInteractiveImplictAnimator::ExecuteSyncGetFractionTask failed to execute task.");
        return 0.0f;
    }
    return task->GetFraction();
}

void RSInteractiveImplictAnimator::SetFinishCallBack(const std::function<void()>& finishCallback)
{
    finishCallback_ = finishCallback;
}

void RSInteractiveImplictAnimator::CallFinishCallback()
{
    animations_.clear();
    fractionAnimationId_ = 0;
    fractionNodeId_ = 0;
    if (finishCallback_) {
        finishCallback_();
    }
}

void RSInteractiveImplictAnimator::AddCommand(
    std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand, FollowType followType, NodeId nodeId) const
{
    if (rsUIContext_.lock() != nullptr) {
        auto rsTransaction = rsUIContext_.lock()->GetRSTransaction();
        if (rsTransaction == nullptr) {
            RS_LOGE("multi-instance: RSInteractiveImplictAnimator::AddCommand, transaction is nullptr");
            return;
        }
        rsTransaction->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            RS_LOGE("RSInteractiveImplictAnimator::AddCommand transactionProxy is nullptr");
            return;
        }
        transactionProxy->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS
