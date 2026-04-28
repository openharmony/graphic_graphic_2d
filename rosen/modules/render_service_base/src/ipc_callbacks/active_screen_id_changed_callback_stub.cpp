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

#include "active_screen_id_changed_callback_stub.h"

namespace OHOS {
namespace Rosen {
int RSActiveScreenIdChangedCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIActiveScreenIdChangedCallback::GetDescriptor()) {
        ROSEN_LOGE("RSActiveScreenIdChangedCallbackStub::OnRemoteRequest ReadInterfaceToken failed");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIActiveScreenIdChangedCallbackInterfaceCode::ON_ACTIVE_SCREEN_ID_CHANGED): {
            ScreenId activeScreenId = INVALID_SCREEN_ID;
            if (!data.ReadUint64(activeScreenId)) {
                RS_LOGE("RSActiveScreenIdChangedCallbackStub::ON_ACTIVE_SCREEN_ID_CHANGED read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnActiveScreenIdChanged(activeScreenId);
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
