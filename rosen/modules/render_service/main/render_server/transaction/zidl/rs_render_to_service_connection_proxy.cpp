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

void RSRenderToServiceConnectionProxy::ReplyDumpResultToService(std::string& dumpString)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return false;
    }
    int32_t dumpStringLength = static_cast<int32_t>(dumpString.length());
    const char* dumpData = dumpString.c_str();
    if (!data.WriteInt32(dumpStringLength)) {
        ROSEN_LOGE("dmulti_process RSRenderToServiceConnectionProxy::REPLY_DUMP_RESULT_TO_SERVICE: WriteSize failed.");
        return;
    }
    if (!RSDumpManager::GetInstance().WriteAshmemDataToParcel(data, dumpStringLength, dumpData)) {
        ROSEN_LOGE("dmulti_process RSRenderToServiceConnectionProxy::REPLY_DUMP_RESULT_TO_SERVICE: WriteDumpData failed.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("dmulti_process RSRenderToServiceConnectionProxy::REPLY_DUMP_RESULT_TO_SERVICE:"
                    "SendRquest failed, err is %{public}d", err);
        return;
    }
}

sptr<HgmServiceToProcessInfo> RSRenderToServiceConnectionProxy::NotifyRpHgmFrameRate(uint64_t timestamp,
    uint64_t vsyncId, const std::unordered_set<ScreenId>& screenIds,
    const sptr<HgmProcessToServiceInfo>& processToServiceInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return false;
    }
    if (!data.WriteUint64(timestamp)) {
        RS_LOGI("dmulti_process RSRenderToServiceConnectionProxy::NotifyRenderServiceProcessHgmFrameRate writeuint64 failed");
        return nullptr;
    }
    if (!data.WriteUint64(vsyncId)) {
        RS_LOGI("dmulti_process RSRenderToServiceConnectionProxy::NotifyRenderServiceProcessHgmFrameRate writeuint64 failed");
        return nullptr;
    }
    if (!data.WriteParcelable(processToServiceInfo.GetRefPtr())) {
        RS_LOGI("dmulti_process RSRenderToServiceConnectionProxy::NotifyRenderServiceProcessHgmFrameRate writeParcelable failed");
        return nullptr;
    }
    auto size = screenIds.size();
    if (!data.WriteUint32(size)) {
        RS_LOGI("dmulti_process RSRenderToServiceConnectionProxy::NotifyRenderServiceProcessHgmFrameRate write uint32 failed");
        return nullptr;
    }
    for (auto screenId : screenIds) {
        if (!data.WriteUint64(screenId)) {
            RS_LOGI("dmulti_process RSRenderToServiceConnectionProxy::NotifyRenderServiceProcessHgmFrameRate write uint64 failed");
            return nullptr;
        }
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_PROCESS_FRAME_RATE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("dmulti_process %{public}s SendRequest failed, err is %{public}d.", __func__, err);
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
        RS_LOGI("RSRenderToServiceConnectionProxy::NotifyScreenSwitchFinished WriteUint64 failed");
        return;
    }
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("dmulti_process %{public}s SendRequest failed, err is %{public}d.", __func__, err);
    }
}

} // namespace Rosen
} // namespace OHOS