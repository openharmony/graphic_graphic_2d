
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
#include "ressched_event_listener.h"

#include "res_sched_client.h"
#include "res_type.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
std::once_flag ResschedEventListener::createFlag_;
sptr<ResschedEventListener> ResschedEventListener::instance_ = nullptr;
constexpr uint64_t SAMPLE_TIME = 100000000;
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
    if (eventType == ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT &&
        eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START) {
        isNeedReport_ = true;
        isFirstReport_ = true;
    } else if (eventType == ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT &&
        eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP) {
        isNeedReport_ = false;
        isFirstReport_ = false;
    }
}

void ResschedEventListener::ReportFrameToRSS()
{
    if (GetIsNeedReport()) {
        uint64_t currTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
        if (GetIsFirstReport() ||
            lastReportTime_ == 0 || currTime - lastReportTime_ >= SAMPLE_TIME) {
            RS_TRACE_BEGIN("ReportFrameToRSS");
            uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_SEND_FRAME_EVENT;
            int64_t value = 0;
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
            SetIsFirstReport(false);
            lastReportTime_ = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());
            RS_TRACE_END();
        }
    }
}

bool ResschedEventListener::GetIsNeedReport() const
{
    return isNeedReport_.load();
}

bool ResschedEventListener::GetIsFirstReport() const
{
    return isFirstReport_.load();
}

void ResschedEventListener::SetIsFirstReport(bool value)
{
    isFirstReport_ = value;
}
} // namespace Rosen
} // namespace OHOS
