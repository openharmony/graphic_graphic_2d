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

#include "rs_connect_to_render_process_stub.h"

#include <iremote_proxy.h>
#include "transaction/rs_connect_to_render_process.h"
#include "transaction/rs_client_to_render_connection.h"
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"

#include "message_parcel.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcessStub"

namespace OHOS {
namespace Rosen {
int RSConnectToRenderProcessStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    if (auto interfaceToken = data.ReadInterfaceToken();
        interfaceToken != RSIConnectToRenderProcess::GetDescriptor()) {
        RS_LOGE("%{public}s: Read interfaceToken failed!", __func__);
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case static_cast<uint32_t>(RSIConnectToRenderProcessInterfaceCode::CREATE_CONNECTION) : {
            uint64_t tokenMaskId = INVALID_TOKEN_MASK_ID;
            if (!data.ReadUint64(tokenMaskId)) {
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION ReadUint64 tokenMaskId failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto remoteObj = data.ReadRemoteObject();
            if (remoteObj == nullptr) {
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION Read remoteObj failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            if (!remoteObj->IsProxyObject()) {
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION remoteObj !remoteObj->IsProxyObject() failed");
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }
            bool needRefresh = false;
            if (!data.ReadBool(needRefresh)) {
                RS_LOGW("RSConnectToRenderProcessStub::CREATE_CONNECTION remoteObj Read needRefresh Faild");
            }
            auto token = iface_cast<RSIConnectionToken>(remoteObj);
            auto [newRenderConn, returnTokenMaskId] = CreateRenderConnection(tokenMaskId, token, needRefresh);
            if (newRenderConn == nullptr) {
                ret = ERR_UNKNOWN_OBJECT;
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION WriteBool replyBool failed");
                break;
            }

            auto replyObj = newRenderConn->AsObject();
            if (replyObj == nullptr || !reply.WriteRemoteObject(replyObj)) {
                ret = ERR_UNKNOWN_OBJECT;
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION WriteRemoteObject replyObj failed");
                break;
            }

            if (!reply.WriteUint64(returnTokenMaskId)) {
                RS_LOGE("RSConnectToRenderProcessStub::CREATE_CONNECTION WriteUint64 returnTokenMaskId failed");
                ret = ERR_INVALID_STATE;
                break;
            }
            break;
        }
        case static_cast<uint32_t>(RSIConnectToRenderProcessInterfaceCode::REMOVE_CONNECTION) : {
            uint64_t tokenMaskId = INVALID_TOKEN_MASK_ID;
            if (!data.ReadUint64(tokenMaskId) || tokenMaskId == INVALID_TOKEN_MASK_ID) {
                RS_LOGE("RSConnectToRenderProcessStub::REMOVE_CONNECTION ReadUint64 tokenMaskId failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }

            auto [renderConnection, pidTokenMaskId] =
                static_cast<RSConnectToRenderProcess*>(this)->FindClientToRenderConnection();
            bool result = false;
            if (tokenMaskId == pidTokenMaskId && renderConnection != nullptr) {
                auto connection = static_cast<RSClientToRenderConnection*>(renderConnection.GetRefPtr());
                connection->CleanAll(true);
                result = true;
            } else {
                RS_LOGE("RSConnectToRenderProcessStub::pidTokenMaskId != tokenMaskId or renderConnection is nullptr");
            }

            if (!reply.WriteBool(result)) {
                RS_LOGE("RSConnectToRenderProcessStub::REMOVE_CONNECTION WriteBool failed!");
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS
