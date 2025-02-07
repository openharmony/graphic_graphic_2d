/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_frame_rate_linker_expected_fps_update_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::RSFrameRateLinkerExpectedFpsUpdateCallbackProxy(
    const sptr<IRemoteObject>& impl) : IRemoteProxy<RSIFrameRateLinkerExpectedFpsUpdateCallback>(impl)
{
}

void RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::OnFrameRateLinkerExpectedFpsUpdate(
    pid_t dstPid, int32_t expectedFps)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIFrameRateLinkerExpectedFpsUpdateCallback::GetDescriptor())) {
        ROSEN_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::OnFrameRateLinkerExpectedFpsUpdate"
            "WriteInterfaceToken failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(dstPid)) {
        ROSEN_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::OnFrameRateLinkerExpectedFpsUpdate"
            "WriteInt32 dstPid failed");
        return;
    }
    if (!data.WriteInt32(expectedFps)) {
        ROSEN_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::OnFrameRateLinkerExpectedFpsUpdate"
            "WriteInt32 expectedFps failed");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIFrameRateLinkerExpectedFpsUpdateCallbackInterfaceCode::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE);

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackProxy::OnFrameRateLinkerExpectedFpsUpdate"
            " error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
