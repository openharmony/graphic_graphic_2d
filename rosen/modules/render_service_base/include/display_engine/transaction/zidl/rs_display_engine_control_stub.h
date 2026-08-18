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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_ZIDL_RS_DISPLAY_ENGINE_CONTROL_STUB_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_ZIDL_RS_DISPLAY_ENGINE_CONTROL_STUB_H

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "common/rs_macros.h"
#include "display_engine/ipc_callbacks/rs_ide_status_change_callback.h"
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code.h"
#include "display_engine/transaction/zidl/rs_idisplay_engine_control.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSDisplayEngineControlStub : public IRemoteStub<RSIDisplayEngineControl> {
public:
    RSDisplayEngineControlStub() = default;
    ~RSDisplayEngineControlStub() noexcept override = default;
 
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    int32_t NotifyDEStatusChange(const uint32_t sceneKey, const std::vector<uint8_t>& values) override;
    int32_t RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback) override;
    int32_t UnregisterDEStatusChangeCallback() override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_ZIDL_RS_DISPLAY_ENGINE_CONTROL_STUB_H