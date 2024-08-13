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

#include <cstdint>
#include <mutex>

#include "res_sched_event_listener.h"

namespace OHOS {
namespace Rosen {
class ResschedEventListener : public ResourceSchedule::ResSchedEventListener {
public:
    static sptr<ResschedEventListener> GetInstance() noexcept;
    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
        std::unordered_map<std::string, std::string> extInfo) override;
    bool GetIsNeedReport() const;
    bool GetIsFirstReport() const;
    void SetIsFirstReport(bool value);
    void ReportFrameToRSS();
private:
    static std::once_flag createFlag_;
    static sptr<ResschedEventListener> instance_;
    std::atomic<bool> isNeedReport_ = {false};
    std::atomic<bool> isFirstReport_ = {false};
    uint64_t lastReportTime_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif