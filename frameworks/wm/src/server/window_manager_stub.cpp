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

#include "window_manager_stub.h"

#include "window_manager_hilog.h"

namespace OHOS {
int32_t WindowManagerStub::OnRemoteRequest(uint32_t code,
                                           MessageParcel& data,
                                           MessageParcel& reply,
                                           MessageOption& option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    int32_t ret = 0;
    switch (code) {
        case WM_CREATE_WINDOW: {
            WMLOG_I("WindowManagerStub::OnRemoteRequest WM_CREATE_WINDOW");
            WindowConfig* config = (WindowConfig*)data.ReadRawData(sizeof(WindowConfig));
            if (config) {
                int32_t result = CreateWindow(*config);
                WMLOG_I("WindowManagerStub::OnRemoteRequest WM_CREATE_WINDOW result=%{public}d", result);
                reply.WriteInt32(result);
            }
            break;
        }
        case WM_SWITCH_TOP: {
            WMLOG_I("WindowManagerStub::OnRemoteRequest WM_SWITCH_TOP");
            int32_t layerid = data.ReadInt32();
            SwitchTop(layerid);
            break;
        }
        case WM_DESTROY_WINDOW: {
            WMLOG_I("WindowManagerStub::OnRemoteRequest WM_DESTROY_WINDOW");
            int32_t windowid = data.ReadInt32();
            DestroyWindow(windowid);
            break;
        }
        default: {
            WMLOG_I("WindowManagerStub::OnRemoteRequest default");
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}
} // namespace OHOS
