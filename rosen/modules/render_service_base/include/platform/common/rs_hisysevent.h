/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_HISYSEVENT_H
#define RENDER_SERVICE_BASE_COMMON_RS_HISYSEVENT_H

#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "platform/common/rs_log.h"
#include <string>
#include <unordered_map>
#include <utility>

namespace OHOS {
namespace Rosen {
namespace RSEventName {
    const std::string NO_DRAW = "NO_DRAW";
    const std::string JANK_FRAME_SKIP = "JANK_FRAME_SKIP";
    const std::string RS_COMPOSITION_TIMEOUT = "RS_COMPOSITION_TIMEOUT";
    const std::string EPS_LCD_FREQ = "EPS_LCD_FREQ";
    const std::string JANK_STATS_RS = "JANK_STATS_RS";
    const std::string JANK_FRAME_RS = "JANK_FRAME_RS";
    const std::string INTERACTION_RESPONSE_LATENCY = "INTERACTION_RESPONSE_LATENCY";
    const std::string INTERACTION_COMPLETED_LATENCY = "INTERACTION_COMPLETED_LATENCY";
    const std::string INTERACTION_RENDER_JANK = "INTERACTION_RENDER_JANK";
    const std::string INTERACTION_HITCH_TIME_RATIO = "INTERACTION_HITCH_TIME_RATIO";
    const std::string FIRST_FRAME_DRAWN = "FIRST_FRAME_DRAWN";
    const std::string RS_NODE_LIMIT_EXCEEDED = "RS_NODE_LIMIT_EXCEEDED";
    const std::string ANIMATION_CALLBACK_MISSING = "ANIMATION_CALLBACK_MISSING";
    const std::string HGM_VOTER_INFO = "HGM_VOTER_INFO";
    const std::string GPU_SUBHEALTH_MONITORING = "GPU_SUBHEALTH_MONITORING";
    const std::string IPC_DATA_OVER_ERROR = "IPC_DATA_OVER_ERROR";
    const std::string RENDER_MEMORY_OVER_WARNING = "RENDER_MEMORY_OVER_WARNING";
    const std::string RENDER_MEMORY_OVER_ERROR = "RENDER_MEMORY_OVER_ERROR";
    const std::string RS_HARDWARE_THREAD_LOAD_WARNING = "RS_HARDWARE_THREAD_LOAD_WARNING";
    const std::string RENDER_DRAWABLE_MULTI_ACCESS = "RENDER_DRAWABLE_MULTI_ACCESS";
    const std::string RS_RENDER_EXCEPTION = "RS_RENDER_EXCEPTION";
} // namespace RSEventName

enum RSEventType {
    RS_FAULT     = 1,    // system fault event
    RS_STATISTIC = 2,    // system statistic event
    RS_SECURITY  = 3,    // system security event
    RS_BEHAVIOR  = 4     // system behavior event
};

class RSHiSysEvent {
public:
    template<typename... Types>
    static int EventWrite(const std::string& eventName, RSEventType type, Types&&... types)
    {
        int hiSysEventWriteRet = HiSysEventWrite(HiDomain::GRAPHIC, eventName, rsEventTypeToEventTypeMap_[type],
            std::forward<Types>(types)...);
        if (hiSysEventWriteRet != 0) {
            RS_LOGW("Write RSHiSysEvent failed, eventName: %{public}s, ret: %{public}d",
                eventName.c_str(), hiSysEventWriteRet);
        }
        return hiSysEventWriteRet;
    }
private:
    using HiDomain = OHOS::HiviewDFX::HiSysEvent::Domain;
    using HiEventType = OHOS::HiviewDFX::HiSysEvent::EventType;
    static std::unordered_map<RSEventType, HiEventType> rsEventTypeToEventTypeMap_;
};

} // namespace Rosen
} // namespace OHOS
#endif