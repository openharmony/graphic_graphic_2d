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

#include "animation/rs_interactive_implict_animator.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace Rosen {
RSUIContext::RSUIContext(uint64_t token, sptr<IRemoteObject>& connectToRenderRemote) : token_(token)
{
    connectToRenderRemote_ = connectToRenderRemote;
    rsRenderInterface_ = std::shared_ptr<RSRenderInterface>(new RSRenderInterface(connectToRenderRemote));
    rsTransactionHandler_ =
        std::make_shared<RSTransactionHandler>(token, rsRenderInterface_->GetRSRenderPipelineClient());
    rsSyncTransactionHandler_ = std::shared_ptr<RSSyncTransactionHandler>(new RSSyncTransactionHandler());
    rsSyncTransactionHandler_->SetTransactionHandler(rsTransactionHandler_);
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        modifiersDrawThread_ = std::make_shared<RSModifiersDrawThread>();
        canvasModifiersDrawAgent_ = std::make_shared<RSCanvasModifiersDrawAgent>();
    }
#endif
}

RSUIContext::~RSUIContext()
{
    RS_LOGI("~RSUIContext: Token:%{public}" PRIu64, token_);
    DestroyModifiersDraw();
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
    static bool isUniRender = RSSystemProperties::GetUniRenderEnabled();
    if (isUniRender) {
        if (rsModifierManager_ == nullptr) {
            rsModifierManager_ = std::make_shared<RSModifierManager>();
        }
        return rsModifierManager_;
    } else {
        std::lock_guard<std::mutex> lock(rsModifierManagerMutex_);
        auto it = rsModifierManagerMap_.find(gettid());
        if (it != rsModifierManagerMap_.end()) {
            return it->second;
        }

        if (!rsModifierManagerMap_.empty()) {
            RS_LOGI_LIMIT("Too many threads are using the same ModifierManagerMap");
        }

        auto rsModifierManager = std::make_shared<RSModifierManager>();
        rsModifierManagerMap_.emplace(gettid(), rsModifierManager);
        return rsModifierManager;
    }
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

void RSUIContext::AddInteractiveImplictAnimator(const std::shared_ptr<RSInteractiveImplictAnimator>& animator)
{
    if (!animator) {
        ROSEN_LOGE("RSUIContext::AddInteractiveImplictAnimator - animator is null");
        return;
    }
    std::lock_guard<std::mutex> lock(interactiveImplictAnimatorMutex_);
    interactiveImplictAnimators_.emplace(animator->GetId(), animator);
}

void RSUIContext::RemoveInteractiveImplictAnimator(InteractiveImplictAnimatorId id)
{
    std::lock_guard<std::mutex> lock(interactiveImplictAnimatorMutex_);
    interactiveImplictAnimators_.erase(id);
}

void RSUIContext::SetRebuildState(RebuildState state)
{
    std::lock_guard<std::mutex> lock(rebuildStateMutex_);
    rebuildState_ = state;
    if (state == RebuildState::Normal) {
        rebuildStateCV_.notify_all();
    }
}

bool RSUIContext::WaitForRebuildNormal(uint32_t timeoutMs)
{
    std::unique_lock<std::mutex> lock(rebuildStateMutex_);
    if (rebuildState_ == RebuildState::Normal) {
        return true;
    }
    auto timeoutDuration = std::chrono::milliseconds(timeoutMs);
    rebuildStateCV_.wait_for(lock, timeoutDuration, [this] {
        return rebuildState_ == RebuildState::Normal;
    });
    if (rebuildState_ != RebuildState::Normal) {
        ROSEN_LOGW("WaitForRebuildNormal timeout after %{public}u ms", timeoutMs);
        return false;
    }
    return true;
}

void RSUIContext::DestroyModifiersDraw()
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        return;
    }
    if (auto transaction = GetRSTransaction()) {
        transaction->SetCommitTransactionCallback(nullptr);
    }
    if (modifiersDrawThread_ != nullptr) {
        modifiersDrawThread_->WaitAllTasksFinish();
        modifiersDrawThread_->Destroy();
        modifiersDrawThread_ = nullptr;
    }
    if (canvasModifiersDrawAgent_ != nullptr) {
        canvasModifiersDrawAgent_->WaitAllTasksFinish();
        canvasModifiersDrawAgent_->Destroy();
        canvasModifiersDrawAgent_ = nullptr;
    }
#endif
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
void RSUIContext::UnblockUIThread()
{
    std::unique_lock<std::mutex> uiLock(uiMutex_);
    if (canBlockUIThread_) {
        canBlockUIThread_ = false;
        uiCV_.notify_all();
    }
}

CommitTransactionCallback RSUIContext::CreateCommitTransactionCallback()
{
    if (modifiersDrawThread_ == nullptr) {
        RS_LOGE("RSUIContext::CreateCommitTransactionCallback, null modifiersDrawThread.");
        return nullptr;
    }

    modifiersDrawThread_->Start();
    std::weak_ptr<RSUIContext> weakContext = shared_from_this();
    return [weakContext](std::shared_ptr<RSRenderPipelineClient>& renderPiplineClient,
        std::unique_ptr<RSTransactionData>&& rsTransactionData, uint32_t& transactionDataIndex) {
        if (renderPiplineClient == nullptr) {
            RS_LOGE("RSUIContext::CreateCommitTransactionCallback, null renderPiplineClient.");
            return;
        }
        if (rsTransactionData == nullptr) {
            RS_LOGE("RSUIContext::CreateCommitTransactionCallback, null transactionData.");
            return;
        }
        auto uiContext = weakContext.lock();
        if (uiContext == nullptr) {
            RS_LOGE("RSUIContext::CreateCommitTransactionCallback, null uiContext.");
            return;
        }
        if (uiContext->modifiersDrawThread_ == nullptr) {
            RS_LOGE("RSUIContext::CreateCommitTransactionCallback, null modifiersDrawThread inner.");
            return;
        }
        static uint32_t commitIndex = 0;
        std::unique_lock<std::mutex> uiLock(uiContext->uiMutex_);
        uiContext->canBlockUIThread_ = true;
        commitIndex++;
        RS_TRACE_NAME_FMT("Post CommitTransaction, index=%u", commitIndex);
        uiContext->modifiersDrawThread_->ScheduleTask(
            [uiContext, renderPiplineClient, transactionData = std::move(rsTransactionData),
                &transactionDataIndex, index = commitIndex]() mutable {
                if (uiContext->modifiersDrawThread_ != nullptr && uiContext->canvasModifiersDrawAgent_ != nullptr) {
                    RS_TRACE_NAME_FMT("Do CommitTransaction, index=%u", index);
                    uiContext->modifiersDrawThread_->CommitTransaction(uiContext->canvasModifiersDrawAgent_,
                        renderPiplineClient, std::move(transactionData), transactionDataIndex);
                }
                uiContext->UnblockUIThread();
            });
    };
}

void RSUIContext::FlushCanvasDrawingNodeBuffers()
{
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        return;
    }
    if (!canvasDrawingNodeUpdated_) {
        return;
    }
    auto transaction = GetRSTransaction();
    if (transaction == nullptr) {
        return;
    }
    RS_TRACE_NAME_FMT("RSUIContext::FlushCanvasDrawingNodeBuffers, token=%" PRIu64, token_);
    if (canvasModifiersDrawAgent_ != nullptr) {
        canvasModifiersDrawAgent_->SubmitAndCollectCanvasBuffers();
    }
    canvasDrawingNodeUpdated_ = false;
}
#endif
} // namespace Rosen
} // namespace OHOS