/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "screen_change_callback_stub.h"

namespace OHOS {
namespace Rosen {
int RSScreenChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIScreenChangeCallback::GetDescriptor()) {
        ROSEN_LOGE("RSScreenChangeCallbackStub::OnRemoteRequest WriteInterfaceToken failed");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED): {
            ScreenId id{0};
            uint8_t event{0};
            uint8_t reason{0};
            if (!data.ReadUint64(id) || !data.ReadUint8(event) || !data.ReadUint8(reason)) {
                RS_LOGE("RSScreenChangeCallbackStub::ON_SCREEN_CHANGED read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnScreenChanged(id, static_cast<ScreenEvent>(event), static_cast<ScreenChangeReason>(reason));
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
