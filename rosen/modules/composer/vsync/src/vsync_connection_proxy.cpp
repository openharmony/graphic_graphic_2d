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

#include "vsync_connection_proxy.h"
#include "graphic_common.h"
#include "vsync_log.h"

constexpr int MAX_RETRY = 3;
constexpr int RETRY_INTERVAL = 1000; // 1000us = 1ms
namespace OHOS {
namespace Rosen {
VSyncConnectionProxy::VSyncConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IVSyncConnection>(impl)
{
}

VsyncError VSyncConnectionProxy::RequestNextVSync()
{
    return RequestNextVSync("unknown", 0);
}

VsyncError VSyncConnectionProxy::RequestNextVSync(
    const std::string& fromWhom, int64_t lastVSyncTS, const int64_t& requestVsyncTime)
{
    MessageOption opt(MessageOption::TF_ASYNC);
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }

    int sendCount = 0;
    int res = NO_ERROR;
    do {
        res = remote->SendRequest(IVSYNC_CONNECTION_REQUEST_NEXT_VSYNC, arg, ret, opt);
        if (res != NO_ERROR && sendCount < MAX_RETRY) {
            sendCount++;
            VLOGE("ipc send fail, error:%{public}d, try again, times:%{public}d", res, sendCount);
            usleep(RETRY_INTERVAL * sendCount);
        } else if (res != NO_ERROR) {
            return VSYNC_ERROR_BINDER_ERROR;
        }
    } while (res != NO_ERROR);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncConnectionProxy::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    MessageOption opt(MessageOption::TF_ASYNC);
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    if (!arg.WriteBool(dvsyncSwitch)) {
        VLOGE("Failed to write dvsyncSwitch:%{public}d", dvsyncSwitch);
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_SET_UI_DVSYNC_SWITCH, arg, ret, opt);
    if (res != NO_ERROR) {
        VLOGE("ipc send fail, error:%{public}d", res);
        return VSYNC_ERROR_UNKOWN;
    }
    return static_cast<VsyncError>(ret.ReadInt32());
}

VsyncError VSyncConnectionProxy::SetNativeDVSyncSwitch(bool dvsyncSwitch)
{
    MessageOption opt(MessageOption::TF_ASYNC);
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    if (!arg.WriteBool(dvsyncSwitch)) {
        VLOGE("Failed to write dvsyncSwitch:%{public}d", dvsyncSwitch);
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_SET_NATIVE_DVSYNC_SWITCH, arg, ret, opt);
    if (res != NO_ERROR) {
        VLOGE("ipc send fail, error:%{public}d", res);
        return VSYNC_ERROR_UNKOWN;
    }
    return static_cast<VsyncError>(ret.ReadInt32());
}

VsyncError VSyncConnectionProxy::SetUiDvsyncConfig(int32_t bufferCount, bool delayEnable, bool nativeDelayEnable)
{
    MessageOption opt(MessageOption::TF_ASYNC);
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    if (!arg.WriteInt32(bufferCount)) {
        VLOGE("SetUiDvsyncConfig bufferCount error");
        return VSYNC_ERROR_UNKOWN;
    }
    if (!arg.WriteBool(delayEnable)) {
        VLOGE("SetUiDvsyncConfig delayEnable error");
        return VSYNC_ERROR_UNKOWN;
    }
    if (!arg.WriteBool(nativeDelayEnable)) {
        VLOGE("SetUiDvsyncConfig nativeDelayEnable error");
        return VSYNC_ERROR_UNKOWN;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG, arg, ret, opt);
    if (res != NO_ERROR) {
        return VSYNC_ERROR_UNKOWN;
    }
    return static_cast<VsyncError>(ret.ReadInt32());
}

VsyncError VSyncConnectionProxy::GetReceiveFd(int32_t &fd)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_GET_RECEIVE_FD, arg, ret, opt);
    if (res != NO_ERROR) {
        VLOGE("GetReceiveFd Failed, res = %{public}d", res);
        return VSYNC_ERROR_BINDER_ERROR;
    }
    res = ret.ReadInt32();
    if (res != VSYNC_ERROR_OK) {
        return static_cast<VsyncError>(res);
    }
    fd = ret.ReadFileDescriptor();
    if (fd < 0) {
        VLOGE("GetReceiveFd Invalid fd:%{public}d", fd);
        return VSYNC_ERROR_API_FAILED;
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncConnectionProxy::SetVSyncRate(int32_t rate)
{
    if (rate < -1) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    if (!arg.WriteInt32(rate)) {
        VLOGE("Failed to write rate:%{public}d", rate);
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_SET_RATE, arg, ret, opt);
    if (res != NO_ERROR) {
        return VSYNC_ERROR_BINDER_ERROR;
    }
    return static_cast<VsyncError>(ret.ReadInt32());
}

VsyncError VSyncConnectionProxy::Destroy()
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("Failed to write interface token");
        return VSYNC_ERROR_API_FAILED;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        VLOGE("remote is null");
        return VSYNC_ERROR_API_FAILED;
    }
    int res = remote->SendRequest(IVSYNC_CONNECTION_DESTROY, arg, ret, opt);
    if (res != NO_ERROR) {
        return VSYNC_ERROR_BINDER_ERROR;
    }
    return static_cast<VsyncError>(ret.ReadInt32());
}
} // namespace Vsync
} // namespace OHOS
