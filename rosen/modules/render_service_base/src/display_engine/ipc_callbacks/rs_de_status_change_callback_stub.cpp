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

#include "display_engine/ipc_callbacks/rs_de_status_change_callback_stub.h"

namespace OHOS {
namespace Rosen {
int RSDEStatusChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIDEStatusChangeCallback::GetDescriptor()) {
        ROSEN_LOGE("RSDEStatusChangeCallbackStub::OnRemoteRequest invalid token");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE): {
            uint32_t sceneKey = 0;
            std::vector<uint8_t> result;
            if (!data.ReadUint32(sceneKey)) {
                ROSEN_LOGE("RSDEStatusChangeCallbackStub::ReadParams ReadUint32 sceneKey failed");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!data.ReadUInt8Vector(&result)) {
                ROSEN_LOGE("RSDEStatusChangeCallbackStub::ReadParams ReadUInt8Vector result failed");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnNotifyDEStatusChangeDone(sceneKey, result);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

void RSDEStatusChangeCallbackStub::OnNotifyDEStatusChangeDone(const uint32_t sceneKey,
    const std::vector<uint8_t>& result)
{
}
} // namespace Rosen
} // namespace OHOS