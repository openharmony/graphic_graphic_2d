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

#include "display_engine/transaction/zidl/rs_display_engine_control_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDisplayEngineControlProxy::RSDisplayEngineControlProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIDisplayEngineControl>(impl)
{
}

int32_t RSDisplayEngineControlProxy::SendRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int32_t ret = Remote()->SendRequest(code, data, reply, option);
    if (ret != NO_ERROR) {
        RS_LOGE("RSDisplayEngineControlProxy::SendRequest code=%{public}u err=%{public}d", code, ret);
    }
    return ret;
}

int32_t RSDisplayEngineControlProxy::NotifyDEStatusChange(const uint32_t sceneKey, const std::vector<uint8_t>& values)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor())) {
        RS_LOGE("RSDisplayEngineControlProxy::NotifyDEStatusChange WriteInterfaceToken failed");
        return -1;
    }
    if (!data.WriteUint32(sceneKey)) {
        RS_LOGE("RSDisplayEngineControlProxy::NotifyDEStatusChange WriteUint32 failed");
        return -1;
    }
    if (!data.WriteUInt8Vector(values)) {
        RS_LOGE("RSDisplayEngineControlProxy::NotifyDEStatusChange WriteUInt8Vector failed");
        return -1;
    }

    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    int32_t ret = SendRequest(code, data, reply, option);
    if (ret != ERR_NONE) {
        RS_LOGE("RSDisplayEngineControlProxy::NotifyDEStatusChange SendRequest failed, ret=%{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t RSDisplayEngineControlProxy::RegisterDEStatusChangeCallback(const sptr<RSIDEStatusChangeCallback>& callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor())) {
        RS_LOGE("RSDisplayEngineControlProxy::RegisterDEStatusChangeCallback WriteInterfaceToken failed");
        return -1;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        RS_LOGE("RSDisplayEngineControlProxy::RegisterDEStatusChangeCallback WriteRemoteObject failed");
        return -1;
    }

    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK);
    int32_t ret = SendRequest(code, data, reply, option);
    if (ret != ERR_NONE) {
        RS_LOGE("RSDisplayEngineControlProxy::RegisterDEStatusChangeCallback SendRequest failed, ret=%{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t RSDisplayEngineControlProxy::UnregisterDEStatusChangeCallback()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor())) {
        RS_LOGE("RSDisplayEngineControlProxy::UnregisterDEStatusChangeCallback WriteInterfaceToken failed");
        return -1;
    }

    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::UNREGISTER_DE_STATUS_CHANGE_CALLBACK);
    int32_t ret = SendRequest(code, data, reply, option);
    if (ret != ERR_NONE) {
        RS_LOGE("RSDisplayEngineControlProxy::UnregisterDEStatusChangeCallback SendRequest failed, ret=%{public}d",
            ret);
        return ret;
    }
    return reply.ReadInt32();
}
} // namespace Rosen
} // namespace OHOS