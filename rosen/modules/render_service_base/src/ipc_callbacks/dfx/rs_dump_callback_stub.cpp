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

 #include "rs_dump_callback_stub.h"
 #include "gfx/dump/rs_dump_manager.h"
 #include "ipc_callbacks/dfx/rs_dump_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
int RSDumpCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIDumpCallback::GetDescriptor()) {
        RS_LOGE("RSDumpCallbackStub: token ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE): {
            int32_t size = 0;
            if (!data.ReadInt32(size)) {
                RS_LOGE("%{public}s::REPLY_DUMP_RESULT_TO_SERVICE ReadInt32 failed.", __func__);
                return ERR_INVALID_STATE;
            }
            char *dataPtr = RSDumpManager::ReadAshmemDataFromParcel(data, size);
            std::string dumpString = dataPtr != nullptr ? 
                std::string(static_cast<sonst char*>(dataPtr), size) : "";
            OnDumpResult(dumpString);
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