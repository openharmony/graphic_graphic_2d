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
#include "command/rs_command.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "pipeline/rs_context.h"
#include "platform/drawing/rs_vsync_client.h"
#include "refbase.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_helper.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"

#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#include "rs_egl_image_manager.h"
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
    void RenderServiceTreeDump(std::string& dumpString);

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

#ifdef RS_ENABLE_EGLIMAGE
    std::shared_ptr<RSEglImageManager> GetRSEglImageManager() const
    {
        return eglImageManager_;
    }
#endif // RS_ENABLE_EGLIMAGE

    RSContext& GetContext()
    {
        return context_;
    }
    std::thread::id Id() const
    {
        return mainThreadId_;
    }
    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);
    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);

    void RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void CleanOcclusionListener();

    void WaitUtilUniRenderFinished();
    void NotifyUniRenderFinish();

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
    void ConsumeAndUpdateAllNodes();
    void ReleaseAllNodesBuffer();
    void Render();
    void CalcOcclusion(RSBaseRenderNode::SharedPtr node);
    void SendCommands();

    std::mutex transitionDataMutex_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unordered_map<NodeId, uint64_t> bufferTimestamps_;
    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> > cacheCommand_;
    std::vector<std::unique_ptr<RSCommand>> effectCommand_;

    uint64_t timestamp_ = 0;
    std::unordered_map<uint32_t, sptr<IApplicationAgent>> applicationAgentMap_;

    RSContext context_;
    std::thread::id mainThreadId_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::vector<sptr<RSIOcclusionChangeCallback>> occlusionListeners_;

    mutable std::mutex uniRenderMutex_;
    bool uniRenderFinished_ = false;
    std::condition_variable uniRenderCond_;
    
#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> renderContext_;
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    std::shared_ptr<RSEglImageManager> eglImageManager_;
#endif // RS_ENABLE_EGLIMAGE
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_MAIN_THREAD
