/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef UTILS_INCLUDE_SYNC_FENCE_TRACKER_H
#define UTILS_INCLUDE_SYNC_FENCE_TRACKER_H

#include <atomic>
#include <event_handler.h>
#include "sync_fence.h"

namespace OHOS {
class SyncFenceTracker {
public:
    explicit SyncFenceTracker(const std::string threadName);

    SyncFenceTracker() = delete;
    ~SyncFenceTracker() = default;

    void TrackFence(const sptr<SyncFence>& fence);

private:
    const uint32_t SYNC_TIME_OUT = 3000;
    const int32_t GPU_SUBHEALTH_EVENT_LIMIT = 200;
    const int32_t GPU_SUBHEALTH_EVENT_THRESHOLD = 12;
    const std::string GPU_LOAD = "/sys/class/devfreq/gpufreq/gpu_scene_aware/utilisation";
    const std::string threadName_;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
    std::atomic<uint32_t> fencesQueued_;
    std::atomic<uint32_t> fencesSignaled_;
    int32_t gpuSubhealthEventNum = 0;
    int32_t gpuSubhealthEventDay;
    void Loop(const sptr<SyncFence>& fence);
    bool CheckGpuSubhealthEventLimit();
    void ReportEventGpuSubhealth(int32_t duration);
    inline int32_t GetValue(const std::string& fileName);
};
}
#endif // UTILS_INCLUDE_SYNC_FENCE_TRACKER_H