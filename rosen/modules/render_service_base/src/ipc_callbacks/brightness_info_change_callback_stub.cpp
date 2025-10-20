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

#include "brightness_info_change_callback_stub.h"

namespace OHOS {
namespace Rosen {
int RSBrightnessInfoChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != GetDescriptor()) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackStub::OnRemoteRequest ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE): {
            ScreenId screenId = 0;
            if (!data.ReadUint64(screenId)) {
                RS_LOGE("RSBrightnessInfoChangeCallbackStub::ON_BRIGHTNESS_INFO_CHANGE read screenId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            BrightnessInfo brightnessInfo;
            if (!ReadBrightnessInfo(brightnessInfo, data)) {
                RS_LOGE("RSBrightnessInfoChangeCallbackStub::ReadBrightnessInfo ReadBrightnessInfo failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnBrightnessInfoChange(screenId, brightnessInfo);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

bool RSBrightnessInfoChangeCallbackStub::ReadBrightnessInfo(BrightnessInfo& brightnessInfo, MessageParcel& data)
{
    if (!data.ReadFloat(brightnessInfo.currentHeadroom) ||
        !data.ReadFloat(brightnessInfo.maxHeadroom) ||
        !data.ReadFloat(brightnessInfo.sdrNits)) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackStub::ReadBrightnessInfo read brightnessInfo failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
