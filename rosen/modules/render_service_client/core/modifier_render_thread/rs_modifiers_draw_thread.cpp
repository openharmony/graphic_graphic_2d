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

#include "modifier_render_thread/rs_modifiers_draw_thread.h"

#include "concurrent_task_client.h"
#include "qos.h"

#include "platform/common/rs_log.h"
#include "render_context/shader_cache.h"

namespace OHOS {
namespace Rosen {
RSModifiersDrawThread::RSModifiersDrawThread()
{
    modifiersDraw_ = std::make_shared<RSModifiersDraw>();
}

RSModifiersDrawThread::~RSModifiersDrawThread() = default;

void RSModifiersDrawThread::Start()
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
        // Init shader cache, shader save delay time differs between uni-render and hybrid-render.
        std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
        auto size = vkVersion.size();
        auto& cache = ShaderCache::Instance();
        cache.InitShaderCache(vkVersion.c_str(), size, false);
    });
    RS_LOGI("%{public}s RSModifiersDrawThread started", __func__);
}

void RSModifiersDrawThread::WaitAllTasksFinish()
{
    PostSyncTask([]() { RS_TRACE_NAME_FMT("RSModifiersDrawThread::WaitAllTasksFinish"); });
}

void RSModifiersDrawThread::Destroy()
{
    destroyed_.store(true);
    if (!started_.load()) {
        return;
    }
    started_.store(false);

    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    if (runner_ != nullptr) {
        runner_->Stop();
        runner_ = nullptr;
    }
}

void RSModifiersDrawThread::PostTask(const std::function<void()>& task, const std::string& name, int64_t delayTime)
{
    if (!started_.load() || handler_ == nullptr) {
        return;
    }
    handler_->PostTask(task, name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSModifiersDrawThread::PostSyncTask(const std::function<void()>& task)
{
    if (!started_.load() || handler_ == nullptr) {
        return;
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSModifiersDrawThread::CommitTransaction(std::shared_ptr<RSCanvasModifiersDrawAgent> canvasModifiersDrawAgent,
    std::shared_ptr<RSRenderPipelineClient> renderPiplineClient, std::unique_ptr<RSTransactionData> transactionData,
    uint32_t& transactionDataIndex)
{
    std::vector<RSTransactionConfig> transactionConfigList;
    canvasModifiersDrawAgent->SwapTransactionConfigList(transactionConfigList);
    modifiersDraw_->ConvertTransaction(transactionData, transactionConfigList);
    renderPiplineClient->CommitTransaction(transactionData);
    transactionDataIndex = transactionData->GetIndex();
}
} // namespace Rosen
} // namespace OHOS