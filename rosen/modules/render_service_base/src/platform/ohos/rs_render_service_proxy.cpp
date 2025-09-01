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

#include "rs_render_service_proxy.h"

#include <message_option.h>
#include <message_parcel.h>
#include <platform/common/rs_log.h>
#include "ipc_callbacks/rs_ipc_callbacks_check.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderServiceProxy"

namespace OHOS {
namespace Rosen {

RSRenderServiceProxy::RSRenderServiceProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<RSIRenderService>(impl) {}

sptr<RSIRenderServiceConnection> RSRenderServiceProxy::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    if (token == nullptr) {
        ROSEN_LOGE("CreateConnection(): token is null.");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        ROSEN_LOGE("CreateConnection(): WriteInterfaceToken failed.");
        return nullptr;
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        ROSEN_LOGE("CreateConnection(): WriteRemoteObject failed.");
        return nullptr;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("CreateConnection(): SendRequest failed, err is %{public}d.", err);
        return nullptr;
    }
    bool isReadRemoteObj{0};
    if (!reply.ReadBool(isReadRemoteObj)) {
        ROSEN_LOGE("CreateConnection Read isReadRemoteObj failed, connection is nullptr.");
        return nullptr;
    }
    if (!isReadRemoteObj) {
        ROSEN_LOGE("CreateConnection(): ReadBool failed, connection is nullptr.");
        return nullptr;
    }
    auto remoteObj = reply.ReadRemoteObject();
    if (remoteObj == nullptr || !remoteObj->IsProxyObject()) {
        ROSEN_LOGE("CreateConnection(): Reply is not valid.");
        return nullptr;
    }

    return iface_cast<RSIRenderServiceConnection>(remoteObj);
}

bool RSRenderServiceProxy::RemoveConnection(const sptr<RSIConnectionToken>& token)
{
    if (token == nullptr) {
        ROSEN_LOGE("RemoveConnection(): token is null.");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        ROSEN_LOGE("RemoveConnection(): WriteInterfaceToken failed.");
        return false;
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        ROSEN_LOGE("RemoveConnection(): WriteRemoteObject failed.");
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::REMOVE_CONNECTION);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RemoveConnection(): SendRequest failed, err is %{public}d.", err);
        return false;
    }

    return reply.ReadBool();
}
} // namespace Rosen
} // namespace OHOS
