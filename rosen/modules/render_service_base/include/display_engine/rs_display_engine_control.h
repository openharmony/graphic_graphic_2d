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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_ENGINE_CONTROL_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_ENGINE_CONTROL_H

#include <mutex>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "display_engine/ipc_callbacks/rs_ide_status_change_callback.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSDisplayEngineControl {
public:
    RSDisplayEngineControl(const RSDisplayEngineControl&) = delete;
    RSDisplayEngineControl& operator=(const RSDisplayEngineControl&) = delete;
    RSDisplayEngineControl(RSDisplayEngineControl&&) = delete;
    RSDisplayEngineControl& operator=(RSDisplayEngineControl&&) = delete;

    static RSDisplayEngineControl& GetInstance();

    // for IPC call
    int32_t NotifyDEStatusChange(const uint32_t sceneKey, const std::vector<uint8_t>& values);
    int32_t RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback);
    int32_t UnregisterDEStatusChangeCallback();

    // for inner modules to trigger callback
    int32_t NotifyDEStatusChangeDone(const uint32_t sceneKey, const std::vector<uint8_t>& result);

private:
    RSDisplayEngineControl() = default;
    ~RSDisplayEngineControl() = default;

    sptr<RSIDEStatusChangeCallback> deStatusChangeCallback_ = nullptr;
    std::mutex callbackMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_ENGINE_CONTROL_H