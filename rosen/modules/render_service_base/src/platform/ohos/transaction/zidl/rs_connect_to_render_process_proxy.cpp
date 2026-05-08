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

#include "rs_connect_to_render_process_proxy.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcessProxy"

namespace OHOS {
namespace Rosen {
RSConnectToRenderProcessProxy::RSConnectToRenderProcessProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIConnectToRenderProcess>(impl) {}

sptr<RSIClientToRenderConnection> RSConnectToRenderProcessProxy::CreateRenderConnection(
    const sptr<RSIConnectionToken>& token, bool needRefresh)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(RSIConnectToRenderProcess::GetDescriptor())) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection GetDescriptor err.");
        return nullptr;
    }

    if (!data.WriteRemoteObject(token->AsObject())) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection WriteRemoteObject callback->AsObject() err.");
        return nullptr;
    }
    if (!data.WriteBool(needRefresh)) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection needRefresh err.");
        return nullptr;
    }
    uint32_t code = static_cast<uint32_t>(RSIConnectToRenderProcessInterfaceCode::CREATE_CONNECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection: Send Request err.");
        return nullptr;
    }
    bool hasRemoteObj = false;
    if (!reply.ReadBool(hasRemoteObj) || !hasRemoteObj) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection: hasRemoteObj err.");
        return nullptr;
    }
    auto obj = reply.ReadRemoteObject();
    if (obj == nullptr) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection: ReadRemoteObject err.");
        return nullptr;
    }
    sptr<RSIClientToRenderConnection> newConn = iface_cast<RSIClientToRenderConnection>(obj);
    if (newConn == nullptr) {
        ROSEN_LOGE("RSConnectToRenderProcessProxy::CreateRenderConnection: RSIClientToRenderConnection err.");
        return nullptr;
    }
    return newConn;
}

int32_t RSConnectToRenderProcessProxy::SendRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}
} // namespace Rosen
} // namespace OHOS