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

#include "display_engine/transaction/zidl/rs_display_engine_control_stub.h"

#include <message_parcel.h>

#include "display_engine/rs_display_engine_control.h"
#include "display_engine/rs_luminance_control.h"
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code_access_verifier.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSDisplayEngineControlStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != RSIDisplayEngineControl::GetDescriptor()) {
        RS_LOGE("%{public}s: Read interfaceToken failed!", __func__);
        return ERR_INVALID_STATE;
    }

    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSDisplayEngineControlStub::OnRemoteRequest no permission code:%{public}u", code);
        return ERR_INVALID_STATE;
    }
 
    switch (code) {
        case static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE): {
            uint32_t sceneKey = 0;
            if (!data.ReadUint32(sceneKey)) {
                RS_LOGE("RSDisplayEngineControlStub::NOTIFY_DE_STATUS_CHANGE ReadUint32 failed");
                return ERR_INVALID_DATA;
            }
            std::vector<uint8_t> values;
            if (!data.ReadUInt8Vector(&values)) {
                RS_LOGE("RSDisplayEngineControlStub::NOTIFY_DE_STATUS_CHANGE ReadUInt8Vector failed");
                return ERR_INVALID_DATA;
            }
            int32_t ret = NotifyDEStatusChange(sceneKey, values);
            reply.WriteInt32(ret);
            return ERR_NONE;
        }
        case static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK): {
            sptr<IRemoteObject> callbackObj = data.ReadRemoteObject();
            if (callbackObj == nullptr) {
                RS_LOGE("RSDisplayEngineControlStub::REGISTER_DE_STATUS_CHANGE_CALLBACK ReadRemoteObject failed");
                return ERR_INVALID_DATA;
            }
            sptr<RSIDEStatusChangeCallback> callback = iface_cast<RSIDEStatusChangeCallback>(callbackObj);
            if (callback == nullptr) {
                RS_LOGE("RSDisplayEngineControlStub::REGISTER_DE_STATUS_CHANGE_CALLBACK iface_cast failed");
                return ERR_INVALID_DATA;
            }
            int32_t ret = RegisterDEStatusChangeCallback(callback);
            reply.WriteInt32(ret);
            return ERR_NONE;
        }
        case static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::UNREGISTER_DE_STATUS_CHANGE_CALLBACK): {
            int32_t ret = UnregisterDEStatusChangeCallback();
            reply.WriteInt32(ret);
            return ERR_NONE;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t RSDisplayEngineControlStub::NotifyDEStatusChange(const uint32_t sceneKey,
    const std::vector<uint8_t>& values)
{
    return RSDisplayEngineControl::GetInstance().NotifyDEStatusChange(sceneKey, values);
}

int32_t RSDisplayEngineControlStub::RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback)
{
    return RSDisplayEngineControl::GetInstance().RegisterDEStatusChangeCallback(callback);
}

int32_t RSDisplayEngineControlStub::UnregisterDEStatusChangeCallback()
{
    return RSDisplayEngineControl::GetInstance().UnregisterDEStatusChangeCallback();
}

const RSInterfaceCodeSecurityManager RSDisplayEngineControlStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSIDisplayEngineControlInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS