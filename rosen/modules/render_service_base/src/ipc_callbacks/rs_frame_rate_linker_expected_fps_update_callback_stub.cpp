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

#include "rs_frame_rate_linker_expected_fps_update_callback_stub.h"

#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

int RSFrameRateLinkerExpectedFpsUpdateCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIFrameRateLinkerExpectedFpsUpdateCallback::GetDescriptor()) {
        RS_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackStub::OnRemoteRequest read token failed!");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(
            RSIFrameRateLinkerExpectedFpsUpdateCallbackInterfaceCode::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE): {
            int32_t dstPid{0};
            int32_t expectedFps{0};
            if (!data.ReadInt32(dstPid) || !data.ReadInt32(expectedFps)) {
                RS_LOGE("RSFrameRateLinkerExpectedFpsUpdateCallbackStub::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE read "
                        "parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnFrameRateLinkerExpectedFpsUpdate(dstPid, expectedFps);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
