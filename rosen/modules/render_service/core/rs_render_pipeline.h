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

#ifndef RS_RENDER_PIPELINE_H
#define RS_RENDER_PIPELINE_H

#include <event_handler.h>
#include <map>
#include <unordered_set>
#include "common/rs_common_def.h"
#include "common/rs_thread_handler.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"
#include "info_collection/rs_hdr_collection.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "irs_render_to_composer_connection.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/image_enhance/image_enhance_manager.h"
#include "platform/ohos/transaction/zidl/rs_iclient_to_service_connection.h"
#include "rs_render_composer_client.h"
#include "vsync_receiver.h"
#include "screen_manager/screen_types.h"
#include "feature/hyper_graphic_manager/hgm_client.h"
#include "memory/rs_memory_manager.h"

namespace OHOS {
namespace Rosen {
namespace ST {
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
} // namespace ST

class RSMainThread;
class RSUniRenderThread;
class RSBufferThread;
class RSScreenProperty;
class RSHwcContext;
class RSRenderPipeline final : public RefBase {
public:
    static std::shared_ptr<RSRenderPipeline> Create(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);

    void PostMainThreadTask(RSTaskMessage::RSTask task);

    void PostUniRenderThreadTask(RSTaskMessage::RSTask task);

    void PostMainThreadSyncTask(RSTaskMessage::RSTask task);

    void PostUniRenderThreadSyncTask(RSTaskMessage::RSTask task);

    void PostMainThreadTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleMainThreadTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = ST::ScheduledTask<Task>::Create(std::move(task));
        PostMainThreadTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

    void OnScreenConnected(const sptr<RSScreenProperty>& rsScreenProperty,
        const std::shared_ptr<RSRenderComposerClient>& composerClient);
    void OnScreenDisconnected(ScreenId screenId);
    void OnScreenPropertyChanged(const sptr<RSScreenProperty>& rsScreenProperty);
    void OnScreenRefresh(ScreenId screenId);
    void InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent);
    void RegisterScreenSwitchFinishCallback(const sptr<RSIRenderToServiceConnection>& conn);

private:
    void Init(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    void InitEnvironment();
    void InitUniRenderConfig();
    void InitCCMConfig();
    // RS Filter CCM init
    void FilterCCMInit();
    void RegisterRcdMsg();
    void InitMainThread(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    void InitUniRenderThread();
    void InitDumper();

    bool isRcdServiceRegister_ = false;

    RSMainThread* mainThread_ = nullptr;
    RSUniRenderThread* uniRenderThread_ = nullptr;
    RSBufferThread* uniBufferThread_ = nullptr;

    std::shared_ptr<ImageEnhanceManager> imageEnhanceManager_ = nullptr;

    friend class RSServiceToRenderConnection;
    friend class RSRenderProcessAgent;
    friend class RSRenderServiceAgent;
    friend class RSRenderPipelineAgent;

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // Rosen
} // OHOS

#endif // RS_RENDER_PIPELINE_H