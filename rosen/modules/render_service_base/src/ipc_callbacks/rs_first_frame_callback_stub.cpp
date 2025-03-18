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

#include "rs_first_frame_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSFirstFrameCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIFirstFrameCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON): {
            uint32_t screenId{0};
            int64_t timestamp{0};
            if (!data.ReadUint32(screenId)) {
                RS_LOGE("RSFirstFrameCallbackStub::OnRemoteRequest read screenId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!data.ReadInt64(timestamp)) {
                RS_LOGE("RSFirstFrameCallbackStub::OnRemoteRequest read timestamp failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnPowerOnFirstFrame(screenId, timestamp);
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
