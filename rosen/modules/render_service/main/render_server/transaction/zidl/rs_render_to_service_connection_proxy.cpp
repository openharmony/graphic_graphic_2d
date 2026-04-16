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

#include "rs_render_to_service_connection_proxy.h"
#include "gfx/dump/rs_dump_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToServiceConnectionProxy"

namespace OHOS {
namespace Rosen {
RSRenderToServiceConnectionProxy::RSRenderToServiceConnectionProxy(const sptr<IRemoteObject>& impl) :
    IRemoteProxy<RSIRenderToServiceConnection>(impl) {}

bool RSRenderToServiceConnectionProxy::NotifyRenderProcessInitFinished(
    const sptr<IRemoteObject>& serviceToRenderConnection, const sptr<IRemoteObject>& connectToRenderConnection)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return false;
    }
    if (!data.WriteRemoteObject(serviceToRenderConnection)) {
        RS_LOGE("%{public}s: Write serviceToRenderConnection failed", __func__);
        return false;
    }
    if (!data.WriteRemoteObject(connectToRenderConnection)) {
        RS_LOGE("%{public}s: Write connectToRenderConnection failed", __func__);
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_RENDER_PROCESS_READY);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRquest failed, err is %{public}d", __func__, err);
        return false;
    }
    bool result = false;
    if (!reply.ReadBool(result)) {
        RS_LOGE("%{public}s: ReadBool failed", __func__);
        return false;
    }
    return result;
}

sptr<ReplyToRenderInfo> RSRenderToServiceConnectionProxy::SendProcessInfo(
    const sptr<ConnectToServiceInfo>& connectToServiceInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return nullptr;
    }
    if (!data.WriteParcelable(connectToServiceInfo.GetRefPtr())) {
        RS_LOGE("%{public}s: WriteParcelable failed", __func__);
        return nullptr;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::SEND_PROCESS_INFO);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRquest failed, err is %{public}d", __func__, err);
        return nullptr;
    }
    return sptr<ReplyToRenderInfo>(reply.ReadParcelable<ReplyToRenderInfo>());
}

sptr<HgmServiceToProcessInfo> RSRenderToServiceConnectionProxy::NotifyRpHgmFrameRate(
    uint64_t timestamp, uint64_t vsyncId, const sptr<HgmProcessToServiceInfo>& processToServiceInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return nullptr;
    }
    if (!data.WriteUint64(timestamp) || !data.WriteUint64(vsyncId)) {
        RS_LOGE("%{public}s: WriteTimestamp or WriteVsyncId failed", __func__);
        return nullptr;
    }
    if (!data.WriteParcelable(processToServiceInfo.GetRefPtr())) {
        RS_LOGE("%{public}s: WriteParcelable failed", __func__);
        return nullptr;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_PROCESS_FRAME_RATE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return nullptr;
    }
    auto serviceToProcessInfo = sptr<HgmServiceToProcessInfo>(reply.ReadParcelable<HgmServiceToProcessInfo>());
    return serviceToProcessInfo;
}

void RSRenderToServiceConnectionProxy::NotifyScreenSwitchFinished(ScreenId screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCH_FINISHED);
    if (!data.WriteUint64(screenId)) {
        RS_LOGE("%{public}s: WriteUint64 failed", __func__);
        return;
    }
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return;
    }
}

} // namespace Rosen
} // namespace OHOS