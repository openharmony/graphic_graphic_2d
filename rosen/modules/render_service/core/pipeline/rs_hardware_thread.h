/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_HARDWARE_THREAD_H
#define RS_HARDWARE_THREAD_H

#include <atomic>
#include <mutex>

#include "event_handler.h"
#include "hdi_backend.h"
#include "rs_main_thread.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS::Rosen {
using UniFallbackCallback = std::function<void(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers,
    uint32_t screenId)>;
using OutputPtr = std::shared_ptr<HdiOutput>;
using LayerPtr = std::shared_ptr<HdiLayer>;
class ScheduledTask;
class RSHardwareThread {
public:
    static RSHardwareThread& Instance();
    void Start();
    void PostTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, int64_t delayTime);
    void CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }
    uint32_t GetunExcuteTaskNum();
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    int GetHardwareTid() const;
    GSError ClearFrameBuffers(OutputPtr output);
private:
    RSHardwareThread() = default;
    ~RSHardwareThread() = default;
    RSHardwareThread(const RSHardwareThread&);
    RSHardwareThread(const RSHardwareThread&&);
    RSHardwareThread& operator=(const RSHardwareThread&);
    RSHardwareThread& operator=(const RSHardwareThread&&);

    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    void Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId);
    void PerformSetActiveMode(OutputPtr output, uint64_t timestamp);
    void ExecuteSwitchRefreshRate(uint32_t rate);
    void AddRefreshRateCount();
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
    static GraphicColorGamut ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers);
    static GraphicPixelFormat ComputeTargetPixelFormat(const std::vector<LayerInfoPtr>& layers);
#endif

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    HdiBackend *hdiBackend_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    UniFallbackCallback redrawCb_;
    std::mutex mutex_;
    std::atomic<uint32_t> unExcuteTaskNum_ = 0;
    int hardwareTid_ = -1;

    HgmRefreshRates hgmRefreshRates_;

    std::map<uint32_t, uint64_t> refreshRateCounts_;
};
}
#endif // RS_HARDWARE_THREAD_H
