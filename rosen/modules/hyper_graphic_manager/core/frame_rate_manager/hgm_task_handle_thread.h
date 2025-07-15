/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_TASK_HANDLE_THREAD_H
#define HGM_TASK_HANDLE_THREAD_H

#include <future>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "event_handler.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "refbase.h"

namespace OHOS::Rosen {
namespace HgmDetail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(t, t->task_.get_future());
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
} // namespace HgmDetail

class HgmTaskHandleThread {
public:
    static HgmTaskHandleThread& Instance();
    const std::shared_ptr<ffrt::queue> GetQueue() const
    {
        return queue_;
    }
    void PostTask(const std::function<void()>& task, int64_t delayTime = 0);
    bool PostSyncTask(const std::function<void()>& task);
    void PostEvent(std::string eventId, const std::function<void()>& task, int64_t delayTime = 0);
    void DetectMultiThreadingCalls();
    void RemoveEvent(std::string eventId);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = HgmDetail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

private:
    HgmTaskHandleThread();
    ~HgmTaskHandleThread()
    {
        queue_ = nullptr;
    }
    HgmTaskHandleThread(const HgmTaskHandleThread&);
    HgmTaskHandleThread(const HgmTaskHandleThread&&);
    HgmTaskHandleThread& operator=(const HgmTaskHandleThread&);
    HgmTaskHandleThread& operator=(const HgmTaskHandleThread&&);

    int32_t curThreadId_ = -1;
    std::shared_ptr<ffrt::queue> queue_ = nullptr;
};
}
#endif // HGM_TASK_HANDLE_THREAD_H
