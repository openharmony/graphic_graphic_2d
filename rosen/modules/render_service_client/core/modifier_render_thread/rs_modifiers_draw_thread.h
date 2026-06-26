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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_THREAD_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_THREAD_H

#include <future>

#include "event_handler.h"
#include "modifier_render_thread/rs_canvas_modifiers_draw_agent.h"
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "refbase.h"

#include "common/rs_macros.h"
#include "transaction/rs_render_pipeline_client.h"

namespace OHOS {
namespace Rosen {
class RSUIContext;
namespace Detail {
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
} // namespace Detail

class RSB_EXPORT RSModifiersDrawThread final {
public:
    RSModifiersDrawThread();
    ~RSModifiersDrawThread();
    RSModifiersDrawThread(const RSModifiersDrawThread&) = delete;
    RSModifiersDrawThread(const RSModifiersDrawThread&&) = delete;
    RSModifiersDrawThread& operator=(const RSModifiersDrawThread&) = delete;
    RSModifiersDrawThread& operator=(const RSModifiersDrawThread&&) = delete;

private:
    void Start();

    void PostTask(const std::function<void()>&& task, const std::string& name = std::string(), int64_t delayTime = 0);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([scheduledTask_(std::move(scheduledTask))]() { scheduledTask_->Run(); });
        return std::move(taskFuture);
    }

    void CommitTransaction(std::shared_ptr<RSCanvasModifiersDrawAgent> canvasModifiersDrawAgent,
        std::shared_ptr<RSRenderPipelineClient> renderPiplineClient, std::unique_ptr<RSTransactionData> transactionData,
        uint32_t& transactionDataIndex);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<RSModifiersDraw> modifiersDraw_ = nullptr;
    bool started_ = false;

    friend class RSUIContext;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_THREAD_H
