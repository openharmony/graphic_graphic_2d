/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
 
#include "pointer_luminance_callback_stub.h"
 
namespace OHOS {
namespace Rosen {
int RSPointerLuminanceChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIPointerLuminanceChangeCallback::GetDescriptor()) {
        ROSEN_LOGE("RSPointerLuminanceChangeCallbackStub::OnRemoteRequest WriteInterfaceToken failed");
        return ERR_INVALID_STATE;
    }
 
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIPointerLuminanceChangeCallbackInterfaceCode::ON_POINTER_LUMINANCE_CHANGED): {
            int32_t brightness{0};
            if (!data.ReadInt32(brightness)) {
                RS_LOGE("RSPointerLuminanceChangeCallbackStub::ON_POINTER_LUMINANCE_CHANGED read brightness failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnPointerLuminanceChanged(brightness);
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