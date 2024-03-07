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

    static constexpr uint8_t IDLE_FPS_THRESHOLD = 8;
    static auto NS_PER_S = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    static auto NS_FPS_SHOW_INTERVAL =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(250));

    static std::mutex threadMutex;
    static std::condition_variable threadCondVar;
    static std::thread updateFpsThread;

    if (enableState_) {
        updateFpsThread = std::thread([&]() {
            uint32_t lastRealtimeRefreshRate = 0;
            currRealtimeRefreshRate_ = 1;
            while (enableState_) {
                std::unique_lock<std::mutex> lock(threadMutex);

                auto st = std::chrono::steady_clock::now();
                realtimeFrameCount_ = 0;
                threadCondVar.wait_for(lock, NS_FPS_SHOW_INTERVAL);
                uint32_t realtimeFrameCount = realtimeFrameCount_;
                auto et = std::chrono::steady_clock::now();

                RS_TRACE_BEGIN("RSRealtimeRefreshRateManager:Cal draw fps");

                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(et - st);
                uint32_t fps = std::round(realtimeFrameCount * static_cast<float>(NS_PER_S) / diff.count());
                fps = std::max(1u, fps);
                if (fps <= IDLE_FPS_THRESHOLD) {
                    fps = 1;
                }

                currRealtimeRefreshRate_ = fps;

                // draw
                if (lastRealtimeRefreshRate != currRealtimeRefreshRate_) {
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
        threadCondVar.notify_all();
        if (updateFpsThread.joinable()) {
            updateFpsThread.join();
        }
        RS_LOGD("RSRealtimeRefreshRateManager: disable");
    }
}
}