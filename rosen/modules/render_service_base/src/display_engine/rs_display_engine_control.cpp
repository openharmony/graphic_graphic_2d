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

#include "display_engine/rs_display_engine_control.h"

#include <message_option.h>
#include <message_parcel.h>

#include "display_engine/rs_luminance_control.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDisplayEngineControl& RSDisplayEngineControl::GetInstance()
{
    static RSDisplayEngineControl instance;
    return instance;
}

int32_t RSDisplayEngineControl::NotifyDEStatusChange(const uint32_t sceneKey, const std::vector<uint8_t>& values)
{
    return RSLuminanceControl::Get().NotifyDEStatusChange(sceneKey, values);
}

int32_t RSDisplayEngineControl::RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback)
{
    if (callback == nullptr) {
        RS_LOGE("RSDisplayEngineControl::RegisterDEStatusChangeCallback fail, callback is nullptr");
        return -1;
    }
    std::lock_guard<std::mutex> lock(callbackMutex_);
    deStatusChangeCallback_ = callback;
    RS_LOGI("RSDisplayEngineControl::RegisterDEStatusChangeCallback success");
    return 0;
}

int32_t RSDisplayEngineControl::UnregisterDEStatusChangeCallback()
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (deStatusChangeCallback_ == nullptr) {
        RS_LOGW("RSDisplayEngineControl no need unregister deStatusChangeCallback_");
        return 0;
    }
    deStatusChangeCallback_ = nullptr;
    RS_LOGI("RSDisplayEngineControl::UnregisterDEStatusChangeCallback success");
    return 0;
}

int32_t RSDisplayEngineControl::NotifyDEStatusChangeDone(const uint32_t sceneKey, const std::vector<uint8_t>& result)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (deStatusChangeCallback_ == nullptr) {
        RS_LOGW("RSDisplayEngineControl::NotifyDEStatusChangeDone deStatusChangeCallback_ is nullptr");
        return -1;
    }
    deStatusChangeCallback_->OnNotifyDEStatusChangeDone(sceneKey, result);
    RS_LOGI("RSDisplayEngineControl::NotifyDEStatusChangeDone success");
    return 0;
}
} // namespace Rosen
} // namespace OHOS