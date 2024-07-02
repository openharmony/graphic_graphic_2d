
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
#ifdef COMPOSER_SCHED_ENABLE
#include "vsync_res_event_listener.h"
#include "vsync_log.h"

#include "res_sched_client.h"
#include "res_type.h"

namespace OHOS {
namespace Rosen {
std::once_flag VSyncResEventListener::createFlag_;
sptr<VSyncResEventListener> VSyncResEventListener::instance_ = nullptr;
sptr<VSyncResEventListener> VSyncResEventListener::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new VSyncResEventListener();
    });
}

void VSyncResEventListener::OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
    std::unordered_map<std::string, std::string> extInfo)
{
    if (eventType == ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT &&
        eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START) {
        isNeedReport_ = true;
    } else if (eventType == ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT &&
        eventValue == ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP) {
        isNeedReport_ = false;
    }
}

bool VSyncResEventListener::GetIsNeedReport()
{
    return isNeedReport_.load();
}
} // namespace Rosen
} // namespace OHOS
#endif

