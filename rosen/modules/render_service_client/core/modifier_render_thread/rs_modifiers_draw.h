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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H

#include <future>

#include "event_handler.h"
#include "refbase.h"

#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSCanvasModifiersDrawAgent;
class RSRenderPipelineClient;
class RSTransactionData;
namespace ModifiersTaskDetail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> scheduledTask(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(scheduledTask, scheduledTask->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace ModifiersTaskDetail

class RSC_EXPORT RSModifiersDraw : public std::enable_shared_from_this<RSModifiersDraw> {
public:
    RSModifiersDraw();
    ~RSModifiersDraw();
    RSModifiersDraw(const RSModifiersDraw&) = delete;
    RSModifiersDraw(const RSModifiersDraw&&) = delete;
    RSModifiersDraw& operator=(const RSModifiersDraw&) = delete;
    RSModifiersDraw& operator=(const RSModifiersDraw&&) = delete;

private:
    // Thread-related methods
    void StartThread();
    void Destroy();

    void PostTask(const std::function<void()>& task, const std::string& name = std::string(), int64_t delayTime = 0);
    void PostSyncTask(const std::function<void()>& task);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = ModifiersTaskDetail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([scheduledTask_(std::move(scheduledTask))]() { scheduledTask_->Run(); });
        return std::move(taskFuture);
    }
    // End of thread-related methods

    void ReleaseResource();

    void CommitTransaction(std::shared_ptr<RSCanvasModifiersDrawAgent> canvasModifiersDrawAgent,
        std::shared_ptr<RSRenderPipelineClient> renderPiplineClient,
        std::unique_ptr<RSTransactionData>& transactionData, std::atomic<uint32_t>& transactionDataIndex);

    // Thread-related members
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::atomic<bool> started_ = false;
    std::atomic<bool> destroyed_ = false;
    // End of thread-related members

    friend class RSModifiersDrawAgent;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H