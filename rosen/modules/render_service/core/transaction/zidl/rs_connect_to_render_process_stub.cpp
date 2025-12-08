/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "message_parcel.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcessStub"

namespace OHOS {
namespace Rosen {
int RSConnectToRenderProcessStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIConnectToRenderProcessInterfaceCode::FORK_CONNECTION) : {
            RS_LOGE("ccc: RSConnectToRenderProcessStub::FORK_CONNECTION !");
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != RSIConnectToRenderProcess::GetDescriptor()) {
                RS_LOGE("RSConnectToRenderProcessStub::FORK_CONNECTION ReadInterfaceToken failed");
                ret = ERR_INVALID_STATE;
                break;
            }
            auto remoteObj = data.ReadRemoteObject();
            if (remoteObj == nullptr) {
                RS_LOGE("RSConnectToRenderProcessStub::FORK_CONNECTION Read remoteObj failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            if (!remoteObj->IsProxyObject()) {
                RS_LOGE("RSConnectToRenderProcessStub::FORK_CONNECTION remoteObj !remoteObj->IsProxyObject() failed");
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }

            auto token = iface_cast<RSIConnectionToken>(remoteObj);
            auto newRenderConn = CreateRenderConnection(token);
            reply.WriteBool(newRenderConn != nullptr);
            RS_LOGE("RSConnectToRenderProcessStub::FORK_CONNECTION %{public}p", newRenderConn.GetRefPtr());
            if (newRenderConn) {
                auto replyObj = newRenderConn->AsObject();
                reply.WriteRemoteObject(replyObj);
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
