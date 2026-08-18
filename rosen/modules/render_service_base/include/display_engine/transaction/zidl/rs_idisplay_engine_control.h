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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_ZIDL_RS_IDISPLAY_ENGINE_CONTROL_H
#define RENDER_SERVICE_BASE_TRANSACTION_ZIDL_RS_IDISPLAY_ENGINE_CONTROL_H

#include <iremote_broker.h>

#include "display_engine/ipc_callbacks/rs_ide_status_change_callback.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSIDisplayEngineControl : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.DisplayEngineControl");

    RSIDisplayEngineControl() = default;
    virtual ~RSIDisplayEngineControl() noexcept = default;

    virtual int32_t NotifyDEStatusChange(const uint32_t sceneKey, const std::vector<uint8_t>& values) = 0;
    virtual int32_t RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback) = 0;
    virtual int32_t UnregisterDEStatusChangeCallback() = 0;
};
} // namespace Rosen
} // namespace OHOS
 
#endif // RENDER_SERVICE_BASE_TRANSACTION_ZIDL_RS_IDISPLAY_ENGINE_CONTROL_H