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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVICE_RS_RENDER_SERVICE_AGENT_H
#define RENDER_SERVICE_MAIN_RENDER_SERVICE_RS_RENDER_SERVICE_AGENT_H

#include "rs_render_service.h"

namespace OHOS {
namespace Rosen {
namespace St {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::move(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run() { task_(); }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace St

class RSRenderServiceAgent : public RefBase {
public:
    explicit RSRenderServiceAgent(RSRenderService& renderService) : renderService_(renderService) {}
    ~RSRenderServiceAgent() = default;

    void PostTaskImmediate(const std::function<void()>& task);
    void PostTaskImmediateInPlace(const std::function<void()>& task);
    void PostSyncTaskImmediate(const std::function<void()>& task);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = St::ScheduledTask<Task>::Create(std::move(task));
        PostTaskImmediate([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

private:
    RSRenderService& renderService_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVICE_RS_RENDER_SERVICE_AGENT_H