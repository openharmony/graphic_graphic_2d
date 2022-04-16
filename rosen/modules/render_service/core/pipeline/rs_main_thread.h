/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RS_MAIN_THREAD
#define RS_MAIN_THREAD

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "ipc_callbacks/iapplication_render_thread.h"
#include "pipeline/rs_context.h"
#include "platform/drawing/rs_vsync_client.h"
#include "refbase.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"

#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL

namespace OHOS {
namespace Rosen {
class RSTransactionData;

namespace Detail {
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
    ~ScheduledTask() {}

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Detail

class RSMainThread {
public:
    static RSMainThread* Instance();

    void Init();
    void Start();
    void RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RequestNextVSync();
    void PostTask(RSTaskMessage::RSTask task);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> GetRenderContext() const
    {
        return renderContext_;
    }
#endif // RS_ENABLE_GL

    RSContext& GetContext()
    {
        return context_;
    }
    std::thread::id Id() const
    {
        return mainThreadId_;
    }
    void RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app);
    void UnregisterApplicationRenderThread(sptr<IApplicationRenderThread> app);

    sptr<VSyncDistributor> rsVSyncDistributor_;
private:
    RSMainThread();
    ~RSMainThread() noexcept;
    RSMainThread(const RSMainThread&) = delete;
    RSMainThread(const RSMainThread&&) = delete;
    RSMainThread& operator=(const RSMainThread&) = delete;
    RSMainThread& operator=(const RSMainThread&&) = delete;

    void OnVsync(uint64_t timestamp, void *data);
    void ProcessCommand();
    void Animate(uint64_t timestamp);
    void Render();
    void SendCommands();

    std::mutex transitionDataMutex_;
    std::unique_ptr<RSThreadLooper> threadLooper_ = nullptr;
    std::unique_ptr<RSThreadHandler> threadHandler_ = nullptr;
    RSTaskHandle taskHandle_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::queue<std::unique_ptr<RSTransactionData>> cacheCommandQueue_;
    std::queue<std::unique_ptr<RSTransactionData>> effectCommandQueue_;

    uint64_t timestamp_ = 0;
    std::unordered_map<uint32_t, sptr<IApplicationRenderThread>> applicationRenderThreadMap_;

    RSContext context_;
    std::thread::id mainThreadId_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;

#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> renderContext_;
#endif // RS_ENABLE_GL
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_MAIN_THREAD
