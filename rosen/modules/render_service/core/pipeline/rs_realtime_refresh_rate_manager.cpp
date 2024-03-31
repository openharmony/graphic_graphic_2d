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

#include "rs_realtime_refresh_rate_manager.h"

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "rs_trace.h"
#include "rs_main_thread.h"

namespace OHOS::Rosen {
RSRealtimeRefreshRateManager& RSRealtimeRefreshRateManager::Instance()
{
    static RSRealtimeRefreshRateManager instance;
    return instance;
}

void RSRealtimeRefreshRateManager::SetShowRefreshRateEnabled(bool enable)
{
    if (enableState_ == enable) {
        return;
    }
    RS_LOGI("RSRealtimeRefreshRateManager state change: %{public}d -> %{public}d",
        static_cast<bool>(enableState_), enable);
    enableState_ = enable;

    if (enableState_) {
        updateFpsThread_ = std::thread([&]() {
            auto &hgmCore = HgmCore::Instance();
            uint32_t lastRefreshRate = 0;
            uint32_t lastRealtimeRefreshRate = 0;
            currRealtimeRefreshRate_ = 1;
            while (enableState_) {
                std::unique_lock<std::mutex> lock(threadMutex_);

                auto st = std::chrono::steady_clock::now();
                realtimeFrameCount_ = 0;
                threadCondVar_.wait_for(lock, NS_FPS_SHOW_INTERVAL_);
                uint32_t realtimeFrameCount = realtimeFrameCount_;
                auto et = std::chrono::steady_clock::now();

                RS_TRACE_BEGIN("RSRealtimeRefreshRateManager:Cal draw fps");

                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(et - st);
                uint32_t fps = std::round(realtimeFrameCount * static_cast<float>(NS_PER_S_) / diff.count());
                fps = std::max(1u, fps);
                if (fps <= IDLE_FPS_THRESHOLD_) {
                    fps = 1;
                }

                currRealtimeRefreshRate_ = fps;

                auto screenId = hgmCore.GetFrameRateMgr()->GetCurScreenId();
                auto refreshRate = hgmCore.GetScreenCurrentRefreshRate(screenId);
                // draw
                if (lastRealtimeRefreshRate != currRealtimeRefreshRate_ || lastRefreshRate != refreshRate) {
                    lastRefreshRate = refreshRate;
                    lastRealtimeRefreshRate = currRealtimeRefreshRate_;
                    RSMainThread::Instance()->SetDirtyFlag();
                    RSMainThread::Instance()->RequestNextVSync();
                    RS_LOGD("RSRealtimeRefreshRateManager: fps update: %{public}ud", currRealtimeRefreshRate_);
                }

                RS_TRACE_END();
            }
        });
        RS_LOGD("RSRealtimeRefreshRateManager: enable");
    } else {
        threadCondVar_.notify_all();
        if (updateFpsThread_.joinable()) {
            updateFpsThread_.join();
        }
        RS_LOGD("RSRealtimeRefreshRateManager: disable");
    }
}
}