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

#include <mutex>

#include "event_handler.h"
#include "hdi_backend.h"
#include "rs_main_thread.h"

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
    void CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers);
    void ReleaseBuffer(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence, sptr<IConsumerSurface> cSurface);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }
private:
    RSHardwareThread() = default;
    ~RSHardwareThread() = default;
    RSHardwareThread(const RSHardwareThread&);
    RSHardwareThread(const RSHardwareThread&&);
    RSHardwareThread& operator=(const RSHardwareThread&);
    RSHardwareThread& operator=(const RSHardwareThread&&);

    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    void Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId);
    void ReleaseLayers(OutputPtr output, const std::unordered_map<uint32_t, LayerPtr>& layerMap);
    void LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<IConsumerSurface>& surface) const;
    void PerformSetActiveMode();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    HdiBackend *hdiBackend_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    UniFallbackCallback redrawCb_;
    std::mutex mutex_;

    bool lockRefreshRateOnce_ = false;
    HgmRefreshRates hgmRefreshRates_;
    HgmRefreshRateModes hgmRefreshRateModes_;
};
}
#endif // RS_HARDWARE_THREAD_H
