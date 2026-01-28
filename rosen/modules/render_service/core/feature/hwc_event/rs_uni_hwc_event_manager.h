/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef RS_UNI_HWC_EVENT_MANAGER_H
#define RS_UNI_HWC_EVENT_MANAGER_H

#include <cstdint>
#include <vector>

namespace OHOS {
namespace Rosen {
enum HwcEvent {
    PREVALIDATE_LOW_TEMP = 0,
    PREVALIDATE_DFR_MODE,       // Screen refresh mode
    PREVALIDATE_DSI_MODE,       // Screen interrupt mode
    PREVALIDATE_DSTCOLOR_MODE,  // Screen color space mode
    HWCEVENT_TUI_ENTER,         // enter tui layer
    HWCEVENT_TUI_EXIT,          // exit tui layer
    HWCEVENT_CALLBACK_MAX,
};

class RSUniHwcEventManager {
public:
    static RSUniHwcEventManager& GetInstance();
    void Init();
    static void OnHwcEvent(uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData, void* data);
private:
    RSUniHwcEventManager() = default;
    ~RSUniHwcEventManager() = default;
    RSUniHwcEventManager(const RSUniHwcEventManager&) = delete;
    RSUniHwcEventManager(const RSUniHwcEventManager&&) = delete;
    RSUniHwcEventManager& operator=(const RSUniHwcEventManager&) = delete;
    RSUniHwcEventManager& operator=(const RSUniHwcEventManager&&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_EVENT_MANAGER_H