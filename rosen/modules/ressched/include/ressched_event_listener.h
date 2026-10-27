/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef VSYNC_RES_EVENT_LISTENER_H
#define VSYNC_RES_EVENT_LISTENER_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "ffrt.h"
#include "res_sched_event_listener.h"

namespace OHOS {
namespace Rosen {
struct ScreenReportContext {
    std::atomic<bool> isNeedReport {false};
    std::atomic<bool> isFirstReport {false};
    std::atomic<uint64_t> lastReportTime {0};
};

class ResschedEventListener : public ResourceSchedule::ResSchedEventListener {
public:
    static sptr<ResschedEventListener> GetInstance() noexcept;
    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
        std::unordered_map<std::string, std::string> extInfo) override;
    bool GetIsNeedReport() const;
    bool GetIsFirstReport() const;
    void SetIsFirstReport(bool value);
    void ReportFrameToRSS(uint64_t screenId = 0);

    void ReportFrameCountAsync(uint32_t rowPid);
    uint32_t GetCurrentPid();
private:
    static constexpr uint64_t DEFAULT_SCREEN_ID = 0;
    static std::once_flag createFlag_;
    static sptr<ResschedEventListener> instance_;
    std::unordered_map<uint64_t, std::shared_ptr<ScreenReportContext>> screenContexts_;
    mutable std::mutex screenContextsMutex_;
    std::shared_ptr<ScreenReportContext> GetOrCreateScreenContext(uint64_t screenId);
    std::shared_ptr<ScreenReportContext> GetScreenContext(uint64_t screenId) const;
    std::atomic<uint32_t> currentPid_;
    uint32_t currentType_;
    std::chrono::steady_clock::time_point beginTimeStamp_;
    std::chrono::steady_clock::time_point endTimeStamp_;
    uint64_t frameCountNum_;
    bool isFrameRateFirstReport_ = false;
    static std::mutex ffrtGetHighFrequenceQueueMutex_;
    static std::shared_ptr<ffrt::queue> ffrtHighPriorityQueue_;

    void HandleDrawFrameEventReport(uint32_t eventValue,
        const std::unordered_map<std::string, std::string>& extInfo);
    void HandleFrameRateStatisticsReport(uint32_t eventValue, std::unordered_map<std::string, std::string> extInfo);
    void HandleFrameRateStatisticsBeginAsync(uint32_t pid, uint32_t type);
    void HandleFrameRateStatisticsEndAsync(uint32_t pid, uint32_t type);
    void HandleFrameRateStatisticsBreakAsync(uint32_t pid, uint32_t type);
    void ReportFrameRateToRSS(const std::unordered_map<std::string, std::string>& mapPayload);
    static bool GetFfrtHighPriorityQueue();
};
} // namespace Rosen
} // namespace OHOS
#endif