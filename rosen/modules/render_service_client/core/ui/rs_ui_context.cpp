/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ui/rs_ui_context.h"

#include "command/rs_animation_command.h"
#include "command/rs_node_command.h"
#include "modifier/rs_modifier_manager_map.h"
#include "platform/common/rs_log.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace Rosen {
RSUIContext::RSUIContext(uint64_t token) : token_(token)
{
    RS_LOGI("RSUIContext ctor: Token:%{public}" PRIu64, token);
    rsTransactionHandler_ = std::make_shared<RSTransactionHandler>(token);
    rsSyncTransactionHandler_ = std::shared_ptr<RSSyncTransactionHandler>(new RSSyncTransactionHandler());
    rsSyncTransactionHandler_->SetTransactionHandler(rsTransactionHandler_);
}

RSUIContext::RSUIContext() : RSUIContext(0) {}

RSUIContext::~RSUIContext()
{
    RS_LOGI("~RSUIContext: Token:%{public}" PRIu64, token_);
}

const std::shared_ptr<RSImplicitAnimator> RSUIContext::GetRSImplicitAnimator()
{
    std::lock_guard<std::mutex> lock(implicitAnimatorMutex_);
    auto it = rsImplicitAnimators_.find(gettid());
    if (it != rsImplicitAnimators_.end()) {
        return it->second;
    } else {
        if (!rsImplicitAnimators_.empty()) {
            RS_LOGI_LIMIT("Too many threads are using the same animator");
        }
        auto rsImplicitAnimator = std::make_shared<RSImplicitAnimator>();
        rsImplicitAnimator->SetRSUIContext(weak_from_this());
        rsImplicitAnimators_.emplace(gettid(), rsImplicitAnimator);
        return rsImplicitAnimator;
    }
}

const std::shared_ptr<RSModifierManager> RSUIContext::GetRSModifierManager()
{
    if (rsModifierManager_ == nullptr) {
        rsModifierManager_ = std::make_shared<RSModifierManager>();
    }
    return rsModifierManager_;
}

bool RSUIContext::AnimationCallback(AnimationId animationId, AnimationCallbackEvent event)
{
    std::shared_ptr<RSAnimation> animation = nullptr;
    {
        std::unique_lock<std::recursive_mutex> lock(animationMutex_);
        auto animationItr = animations_.find(animationId);
        if (animationItr == animations_.end()) {
            ROSEN_LOGE("Failed to find animation[%{public}" PRIu64 "]!", animationId);
            return false;
        }
        animation = animationItr->second;
    }

    if (animation == nullptr) {
        ROSEN_LOGE("Failed to callback animation[%{public}" PRIu64 "], animation is null!", animationId);
        return false;
    }
    if (event == AnimationCallbackEvent::FINISHED) {
        RemoveAnimationInner(animation);
        animation->CallFinishCallback();
        return true;
    } else if (event == AnimationCallbackEvent::REPEAT_FINISHED) {
        animation->CallRepeatCallback();
        return true;
    } else if (event == AnimationCallbackEvent::LOGICALLY_FINISHED) {
        animation->CallLogicallyFinishCallback();
        return true;
    }
    ROSEN_LOGE("Failed to callback animation event[%{public}d], event is null!", static_cast<int>(event));
    return false;
}

void RSUIContext::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    animations_.emplace(animation->GetId(), animation);
    animatingPropertyNum_[animation->GetPropertyId()]++;
}

void RSUIContext::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    if (auto it = animatingPropertyNum_.find(animation->GetPropertyId()); it != animatingPropertyNum_.end()) {
        it->second--;
        if (it->second == 0) {
            animatingPropertyNum_.erase(it);
            animation->SetPropertyOnAllAnimationFinish();
        }
    }
    animations_.erase(animation->GetId());
}

void RSUIContext::SetRequestVsyncCallback(const std::function<void()>& callback)
{
    requestVsyncCallback_ = callback;
}

void RSUIContext::RequestVsyncCallback()
{
    if (requestVsyncCallback_ == nullptr) {
        ROSEN_LOGE("RSUIContext::RequestVsyncCallback failed requestVsyncCallback_ is null, token=%{public}" PRIu64 "",
            token_);
        return;
    }
    requestVsyncCallback_();
}

void RSUIContext::SetUITaskRunner(const TaskRunner& uiTaskRunner)
{
    RS_LOGI("RSUIContext::SetUITaskRunner, Context token:%{public}" PRIu64, GetToken());
    std::lock_guard<std::recursive_mutex> lock(uiTaskRunnersVisitorMutex_);
    taskRunner_ = uiTaskRunner;
    if (rsTransactionHandler_) {
        rsTransactionHandler_->SetUITaskRunner(uiTaskRunner);
    }
}

void RSUIContext::PostTask(const std::function<void()>& task)
{
    PostDelayTask(task, 0);
}

void RSUIContext::PostDelayTask(const std::function<void()>& task, uint32_t delay)
{
    std::lock_guard<std::recursive_mutex> lock(uiTaskRunnersVisitorMutex_);
    if (taskRunner_ == nullptr) {
        ROSEN_LOGW(
            "multi-instance RSUIContext::PostDelayTask failed, taskRunner is empty, token=%{public}" PRIu64, token_);
        auto ctx = RSUIContextManager::Instance().GetRandomUITaskRunnerCtx();
        if (ctx == nullptr) {
            ROSEN_LOGE("multi-instance RSUIContext::PostDelayTask, no taskRunner exists");
            return;
        }
        ctx->PostDelayTask(task, delay);
        return;
    }
    taskRunner_(task, delay);
    ROSEN_LOGD("multi-instance RSUIContext::PostDelayTask success token=%{public}" PRIu64, token_);
}

void RSUIContext::DumpNodeTreeProcessor(NodeId nodeId, pid_t pid, uint32_t taskId, std::string& out)
{
    auto transaction = GetRSTransaction();
    if (transaction == nullptr) {
        return;
    }

    if (auto node = GetNodeMap().GetNode(nodeId)) {
        out.append("transactionFlags:[ ")
            .append(std::to_string(pid))
            .append(", ")
            .append(std::to_string(transaction->GetTransactionDataIndex()))
            .append("]\r");
        transaction->DumpCommand(out);

        constexpr int TOP_LEVEL_DEPTH = 1;
        node->DumpTree(TOP_LEVEL_DEPTH, out);
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCommitDumpClientNodeTree>(nodeId, pid, taskId, out);
    transaction->AddCommand(command, true);
    transaction->FlushImplicitTransaction();
}

int32_t RSUIContext::GetUiPiplineNum()
{
    std::lock_guard<std::mutex> lock(uiPipelineNumMutex_);
    return uiPipelineNum_;
}

void RSUIContext::DetachFromUI()
{
    std::lock_guard<std::mutex> lock(uiPipelineNumMutex_);
    if (uiPipelineNum_ <= 0) {
        ROSEN_LOGE("RSUIContext::DetachFromUI failed. token: %{public}" PRIu64 " uiPiplineNum: %{public}d", token_,
            uiPipelineNum_);
        return;
    }
    uiPipelineNum_--;
    ROSEN_LOGI(
        "RSUIContext::DetachFromUI. token: %{public}" PRIu64 " uiPiplineNum: %{public}d", token_, uiPipelineNum_);
}

void RSUIContext::AttachFromUI()
{
    std::lock_guard<std::mutex> lock(uiPipelineNumMutex_);
    ROSEN_LOGI(
        "RSUIContext::AttachFromUI. token: %{public}" PRIu64 " uiPiplineNum: %{public}d", token_, uiPipelineNum_);
    if (uiPipelineNum_ == UI_PiPLINE_NUM_UNDEFINED) {
        uiPipelineNum_ = 1;
        return;
    }
    uiPipelineNum_++;
}

bool RSUIContext::HasTaskRunner()
{
    std::lock_guard<std::recursive_mutex> lock(uiTaskRunnersVisitorMutex_);
    return taskRunner_ != nullptr;
}

void RSUIContext::MoveModifier(std::shared_ptr<RSUIContext> dstUIContext, NodeId nodeId)
{
    if (!rsModifierManager_ || !dstUIContext) {
        return;
    }

    if (auto dstModifierManager = dstUIContext->GetRSModifierManager()) {
        rsModifierManager_->MoveModifier(dstModifierManager, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS