
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
#include <charconv>
#include <cinttypes>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "ffrt_inner.h"
#include "ressched_event_listener.h"
#include "res_sched_client.h"
#include "res_type.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

constexpr uint32_t DEFAULT_PID = 0;
constexpr uint32_t DEFAULT_TYPE = 0;
constexpr double EPSILON = 0.1;


std::once_flag ResschedEventListener::createFlag_;
sptr<ResschedEventListener> ResschedEventListener::instance_ = nullptr;
std::shared_ptr<ffrt::queue> ResschedEventListener::ffrtHighPriorityQueue_ = nullptr;
std::mutex ResschedEventListener::ffrtGetHighFrequenceQueueMutex_;
constexpr uint64_t SAMPLE_TIME = 100000000;
const std::string RS_RESSCHED_LISTENER_QUEUE_HIGH_PRIOTITY = "res_ressched_event_listener_high_priotity_queue";

sptr<ResschedEventListener> ResschedEventListener::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new ResschedEventListener();
    });
    return instance_;
}

void ResschedEventListener::OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
    std::unordered_map<std::string, std::string> extInfo)
{
    if (eventType == ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT) {
        HandleDrawFrameEventReport(eventValue, extInfo);
    } else if (eventType == ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS) {
        HandleFrameRateStatisticsReport(eventValue, extInfo);
    }
}

void ResschedEventListener::HandleDrawFrameEventReport(uint32_t eventValue,
    const std::unordered_map<std::string, std::string>& extInfo)
{
    uint64_t screenId = DEFAULT_SCREEN_ID;
    auto it = extInfo.find("screenId");
    if (it != extInfo.end()) {
        auto result = std::from_chars(it->second.data(),
            it->second.data() + it->second.size(), screenId);
        if (result.ec != std::errc()) {
            screenId = DEFAULT_SCREEN_ID;
        }
    }

    auto ctx = GetOrCreateScreenContext(screenId);
    if (ctx == nullptr) {
        return;
    }
    if (eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START) {
        ctx->isNeedReport.store(true);
        ctx->isFirstReport.store(true);
        ctx->lastReportTime.store(0);
    } else if (eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP) {
        RemoveScreenContext(screenId);
    }
    RS_TRACE_NAME_FMT("HandleDrawFrameEventReport eventValue:%u screenId:%" PRIu64, eventValue, screenId);
}


bool ResschedEventListener::IsSampleTimeReached(uint64_t lastReportTime, uint64_t currTime)
{
    return currTime > lastReportTime && currTime - lastReportTime >= SAMPLE_TIME;
}

void ResschedEventListener::ReportFrameToRSS(uint64_t screenId)
{
    auto ctx = GetScreenContext(screenId);
    if (ctx == nullptr || !ctx->isNeedReport.load()) {
        return;
    }

    uint64_t currTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    if (ctx->isFirstReport.load() || ctx->lastReportTime.load() == 0 ||
        IsSampleTimeReached(ctx->lastReportTime.load(), currTime)) {
        RS_TRACE_NAME_FMT("ReportFrameToRSS screenId:%" PRIu64, screenId);
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_SEND_FRAME_EVENT;
        int64_t value = 0;
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["screenId"] = std::to_string(screenId);
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
        ctx->isFirstReport.store(false);
        ctx->lastReportTime.store(static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count()));
    }
}

std::shared_ptr<ScreenReportContext> ResschedEventListener::GetOrCreateScreenContext(uint64_t screenId)
{
    std::lock_guard<std::mutex> lock(screenContextsMutex_);
    auto it = screenContexts_.find(screenId);
    if (it != screenContexts_.end()) {
        return it->second;
    }
    auto ctx = std::make_shared<ScreenReportContext>();
    screenContexts_[screenId] = ctx;
    return ctx;
}

std::shared_ptr<ScreenReportContext> ResschedEventListener::GetScreenContext(uint64_t screenId) const
{
    std::lock_guard<std::mutex> lock(screenContextsMutex_);
    auto it = screenContexts_.find(screenId);
    if (it != screenContexts_.end()) {
        return it->second;
    }
    return nullptr;
}

void ResschedEventListener::RemoveScreenContext(uint64_t screenId)
{
    std::lock_guard<std::mutex> lock(screenContextsMutex_);
    screenContexts_.erase(screenId);
}

bool ResschedEventListener::GetIsNeedReport() const
{
    auto ctx = GetScreenContext(DEFAULT_SCREEN_ID);
    return ctx != nullptr && ctx->isNeedReport.load();
}

bool ResschedEventListener::GetIsFirstReport() const
{
    auto ctx = GetScreenContext(DEFAULT_SCREEN_ID);
    return ctx != nullptr && ctx->isFirstReport.load();
}

void ResschedEventListener::SetIsFirstReport(bool value)
{
    auto ctx = GetOrCreateScreenContext(DEFAULT_SCREEN_ID);
    if (ctx != nullptr) {
        ctx->isFirstReport.store(value);
    }
}

void ResschedEventListener::HandleFrameRateStatisticsReport(uint32_t eventValue,
    std::unordered_map<std::string, std::string> extInfo)
{
    uint32_t pid;
    uint32_t type;
    auto resultPid =
        std::from_chars(extInfo.at("pid").data(), extInfo.at("pid").data() + extInfo.at("pid").size(), pid);
    auto resultType =
        std::from_chars(extInfo.at("type").data(), extInfo.at("type").data() + extInfo.at("type").size(), type);
    if (resultPid.ec != std::errc() || resultType.ec != std::errc()) {
        return;
    }

    switch (eventValue) {
        case ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START:
            HandleFrameRateStatisticsBeginAsync(pid, type);
            break;
        case ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK:
            HandleFrameRateStatisticsBreakAsync(pid, type);
            break;
        case ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_END:
            HandleFrameRateStatisticsEndAsync(pid, type);
            break;
    }
}

void ResschedEventListener::ReportFrameRateToRSS(const std::unordered_map<std::string, std::string>& mapPayload)
{
    if (GetFfrtHighPriorityQueue()) {
        ffrtHighPriorityQueue_->submit([mapPayload]() {
            RS_TRACE_BEGIN("FrameRateStatistics ReportFrameRateToRSS");
            uint32_t type = ResourceSchedule::ResType::RES_TYPE_FRAME_RATE_REPORT_FROM_RS;
            int64_t value = ResourceSchedule::ResType::FrameRateReportState::FRAME_RATE_COMMON_REPORT;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
            RS_TRACE_END();
        });
    }
}

void ResschedEventListener::ReportFrameCountAsync(uint32_t pid)
{
    if (!GetFfrtHighPriorityQueue()) {
        RS_TRACE_NAME("ReportFrameCountAsync get ffrtqueue failed!");
        return;
    }
    ffrtHighPriorityQueue_->submit([pid, this]() {
        if (currentType_ == DEFAULT_TYPE) {
            return;
        }
        if (pid != currentPid_.load()) {
            return;
        }
        if (isFrameRateFirstReport_) {
            isFrameRateFirstReport_ = false;
            beginTimeStamp_ = std::chrono::steady_clock::now();
        }
        endTimeStamp_ = std::chrono::steady_clock::now();
        frameCountNum_++;
    });
}

void ResschedEventListener::HandleFrameRateStatisticsBeginAsync(uint32_t pid, uint32_t type)
{
    if (GetFfrtHighPriorityQueue()) {
        ffrtHighPriorityQueue_->submit([pid, type, this]() {
            RS_TRACE_BEGIN("HandleFrameRateStatisticsBeginAsync");
            currentPid_.store(pid);
            currentType_ = type;
            frameCountNum_ = 0;
            isFrameRateFirstReport_ = true;
            RS_TRACE_END();
        });
    }
}

void ResschedEventListener::HandleFrameRateStatisticsBreakAsync(uint32_t pid, uint32_t type)
{
    if (pid == currentPid_.load() && GetFfrtHighPriorityQueue()) {
        ffrtHighPriorityQueue_->submit([this]() {
            RS_TRACE_BEGIN("HandleFrameRateStatisticsBreakAsync");
            currentPid_.store(DEFAULT_PID);
            currentType_ = DEFAULT_TYPE;
            RS_TRACE_END();
        });
    }
}

void ResschedEventListener::HandleFrameRateStatisticsEndAsync(uint32_t pid, uint32_t type)
{
    if (pid == currentPid_.load() && GetFfrtHighPriorityQueue()) {
        ffrtHighPriorityQueue_->submit([this]() {
            RS_TRACE_BEGIN("HandleFrameRateStatisticsEndAsync");
            std::chrono::duration<double> durationTime = endTimeStamp_ - beginTimeStamp_;
            if (std::fabs(durationTime.count()) > EPSILON) {
                int32_t frameRate = static_cast<int32_t>(std::round(frameCountNum_/durationTime.count()));
                std::unordered_map<std::string, std::string> mapPayload;
                mapPayload["pid"] = std::to_string(currentPid_.load());
                mapPayload["type"] = std::to_string(currentType_);
                mapPayload["frameRate"] = std::to_string(frameRate);
                ReportFrameRateToRSS(mapPayload);
            }
            currentPid_.store(DEFAULT_PID);
            currentType_ = DEFAULT_TYPE;
            RS_TRACE_END();
        });
    }
}

uint32_t ResschedEventListener::GetCurrentPid()
{
    return currentPid_.load();
}

bool ResschedEventListener::GetFfrtHighPriorityQueue()
{
    if (ffrtHighPriorityQueue_ != nullptr) {
        return true;
    }
    std::lock_guard<std::mutex> lock(ffrtGetHighFrequenceQueueMutex_);
    if (ffrtHighPriorityQueue_ == nullptr) {
        ffrtHighPriorityQueue_ = std::make_shared<ffrt::queue>(
            RS_RESSCHED_LISTENER_QUEUE_HIGH_PRIOTITY.c_str(), ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    }
    if (ffrtHighPriorityQueue_ == nullptr) {
        RS_TRACE_NAME("FrameRateStatistics Init HighPriority ffrtqueue failed!");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
