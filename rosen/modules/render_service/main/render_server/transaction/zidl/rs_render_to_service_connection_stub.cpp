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

#include "rs_render_to_service_connection_stub.h"

#include "gfx/dump/rs_dump_manager.h"
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "rs_irender_to_service_connection_ipc_interface_code.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToServiceConnectionStub"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_SCREEN_ID_COUNT = 64;
}

int RSRenderToServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    if (auto interfaceToken = data.ReadInterfaceToken();
        interfaceToken != RSIRenderToServiceConnection::GetDescriptor()) {
        RS_LOGE("%{public}s: Read interfaceToken failed.", __func__);
        return ERR_INVALID_STATE;
    }
    bool isTokenTypeValid = true;
    bool isNonSystemAppCalling = false;
    RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
    if (!isTokenTypeValid) {
        RS_LOGE("%{public}s: invalid token type", __func__);
        return ERR_INVALID_STATE;
    }
    if (isNonSystemAppCalling) {
        RS_LOGE("%{public}s: isNonSystemAppCalling", __func__);
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_RENDER_PROCESS_READY): {
            sptr<IRemoteObject> serviceToRenderConnection = data.ReadRemoteObject();
            if (!serviceToRenderConnection) {
                RS_LOGE("%{public}s::NOTIFY_RENDER_PROCESS_READY Read serviceToRenderConnection failed.", __func__);
                return ERR_INVALID_STATE;
            }
            sptr<IRemoteObject> connectToRenderConnection = data.ReadRemoteObject();
            if (!connectToRenderConnection) {
                RS_LOGE("%{public}s::NOTIFY_RENDER_PROCESS_READY Read connectToRenderConnection failed.", __func__);
                return ERR_INVALID_STATE;
            }
            auto result = NotifyRenderProcessInitFinished(serviceToRenderConnection, connectToRenderConnection);
            if (!reply.WriteBool(result)) {
                RS_LOGE("%{public}s::NOTIFY_RENDER_PROCESS_READY WriteBool failed.", __func__);
                return ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::SEND_PROCESS_INFO): {
            auto connectToServiceInfo = sptr<ConnectToServiceInfo>(data.ReadParcelable<ConnectToServiceInfo>());
            if (!connectToServiceInfo) {
                RS_LOGE("%{public}s::SEND_PROCESS_INFO ReadParcelable failed", __func__);
                return ERR_INVALID_STATE;
            }
            auto replyToRenderInfo = SendProcessInfo(connectToServiceInfo);
            if (!reply.WriteParcelable(replyToRenderInfo.GetRefPtr())) {
                RS_LOGE("%{public}s::SEND_PROCESS_INFO writeParcelable failed", __func__);
                ret = ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_PROCESS_FRAME_RATE): {
            return ERR_INVALID_STATE;
        }
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCH_FINISHED): {
            ScreenId id = 0;
            if (!data.ReadUint64(id)) {
                RS_LOGE("%{public}s::NOTIFY_SCREEN_SWITCH_FINISHED ReadUint64 failed.", __func__);
                return ERR_INVALID_STATE;
            }
            NotifyScreenSwitchFinished(id);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

} // namespace Rosen
} // namespace OHOS