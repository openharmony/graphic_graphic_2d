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

#include "rs_uni_hwc_event_manager.h"

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_pointer_window_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcEventManager"

namespace OHOS {
namespace Rosen {
RSUniHwcEventManager& RSUniHwcEventManager::GetInstance()
{
    static RSUniHwcEventManager instance;
    return instance;
}

void RSUniHwcEventManager::Init()
{
    RS_LOGI("[%{public}s]:register OnHwcEvent Func", __func__);
    RSPointerWindowManager::Instance().SetTuiEnabled(false);
    auto hdiBackend = HdiBackend::GetInstance();
    if (!hdiBackend) {
        RS_LOGW("[%{public}s]:hdiBackend is nullptr", __func__);
        return;
    }
    if (hdiBackend->RegHwcEventCallback(&RSUniHwcEventManager::OnHwcEvent, this) != 0) {
        RS_LOGW("[%{public}s]:Failed to register OnHwcEvent Func", __func__);
        return;
    }
}

void RSUniHwcEventManager::OnHwcEvent(
    uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData, void* data)
{
    RS_LOGI("RSUniHwcEventManager::OnHwcEvent: deviceId is %{public}u, eventId is %{public}u", devId, eventId);
    switch (eventId) {
        case HwcEvent::HWCEVENT_TUI_ENTER: {
            RSMainThread::Instance()->PostTask([]() {
                RSPointerWindowManager::Instance().SetTuiEnabled(true);
            });
            break;
        }
        case HwcEvent::HWCEVENT_TUI_EXIT: {
            RSMainThread::Instance()->PostTask([]() {
                RSPointerWindowManager::Instance().SetTuiEnabled(false);
            });
            break;
        }
        default: {
            RSUniHwcPrevalidateUtil::GetInstance().OnHwcEvent(devId, eventId, eventData);
            break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS