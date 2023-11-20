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
#include "frame_rate_report.h"
#include <string>
#ifdef QOS_MANAGER
#include "concurrent_task_client.h"
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

} // namespace FRAME_TRACE