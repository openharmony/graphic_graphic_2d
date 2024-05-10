/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "frame_rate_report.h"
#include <string>
#ifdef QOS_MANAGER
#include "concurrent_task_client.h"
#endif
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif

namespace FRAME_TRACE {

FrameRateReport* FrameRateReport::instance_ = nullptr;
bool FrameRateReport::SendFrameRates(const std::unordered_map<int, uint32_t>& rates)
{
    if (rates.empty()) {
        return true;
    }
    #ifdef QOS_MANAGER
    OHOS::ConcurrentTask::DeadlineReply ddlReply;
    OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().QueryDeadline(
        OHOS::ConcurrentTask::DDL_RATE, ddlReply, rates);
    return ddlReply.setStatus;
    #else
    return true;
    #endif
}

void FrameRateReport::SendFrameRatesToRss(const std::unordered_map<int, uint32_t>& rates)
{
    if (rates.empty()) {
        return;
    }
    #ifdef RES_SCHED_ENABLE
    std::unordered_map<std::string, std::string> mapPayload;
    for (auto it = rates.begin(); it != rates.end(); ++it) {
        mapPayload[std::to_string(it->first)] = std::to_string(it->second);
    }
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        OHOS::ResourceSchedule::ResType::RES_TYPE_FRAME_RATE_REPORT, 0, mapPayload);
    #endif
}

} // namespace FRAME_TRACE