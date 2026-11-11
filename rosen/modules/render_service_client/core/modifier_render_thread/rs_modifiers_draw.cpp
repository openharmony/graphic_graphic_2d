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

#include "modifier_render_thread/rs_modifiers_draw.h"

#include "concurrent_task_client.h"
#include "modifier_render_thread/rs_canvas_modifiers_draw_agent.h"
#include "qos.h"

#include "platform/common/rs_log.h"
#include "transaction/rs_render_pipeline_client.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
RSModifiersDraw::RSModifiersDraw()
{
    StartThread();
}

RSModifiersDraw::~RSModifiersDraw() = default;

// Thread-related methods
void RSModifiersDraw::StartThread()
{
    if (started_.load() || destroyed_.load()) {
        return;
    }
    runner_ = AppExecFwk::EventRunner::Create("ModifiersDraw");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    started_.store(true);
    PostTask([] {
        OHOS::ConcurrentTask::IntervalReply reply;
        reply.tid = gettid();
        OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().QueryInterval(
            OHOS::ConcurrentTask::QUERY_MODIFIER_DRAW, reply);
        SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
    });
    RS_LOGI("%{public}s RSModifiersDraw started", __func__);
}

void RSModifiersDraw::Destroy()
{
    destroyed_.store(true);
    if (!started_.load()) {
        return;
    }
    started_.store(false);

    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
    }
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    runner_ = nullptr;
    handler_ = nullptr;
}

void RSModifiersDraw::PostTask(const std::function<void()>& task, const std::string& name, int64_t delayTime)
{
    if (!started_.load() || handler_ == nullptr) {
        return;
    }
    handler_->PostTask(task, name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSModifiersDraw::PostSyncTask(const std::function<void()>& task)
{
    if (!started_.load() || handler_ == nullptr) {
        return;
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
// End of thread-related methods

void RSModifiersDraw::ReleaseResource()
{
    RS_TRACE_NAME_FMT("RSModifiersDraw::ReleaseResource");
}

void RSModifiersDraw::CommitTransaction(std::shared_ptr<RSCanvasModifiersDrawAgent> canvasModifiersDrawAgent,
    std::shared_ptr<RSRenderPipelineClient> renderPiplineClient, std::unique_ptr<RSTransactionData>& transactionData,
    std::atomic<uint32_t>& transactionDataIndex)
{
    if (canvasModifiersDrawAgent != nullptr) {
        auto command = canvasModifiersDrawAgent->SyncAndWait();
        if (command != nullptr) {
            transactionData->AddCommand(command, command->GetNodeId(), FollowType::NONE);
        }
    }
    renderPiplineClient->CommitTransaction(transactionData);
    transactionDataIndex.store(transactionData->GetIndex(), std::memory_order_relaxed);
}
} // namespace OHOS::Rosen