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

#ifdef ROSEN_OHOS
#include "buffer_available_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSBufferAvailableCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIBufferAvailableCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case ON_BUFFER_AVAILABLE: {
            OnBufferAvailable();
            break;
        }
        default: {
            ret = ERR_UNKNOWN_TRANSACTION;
            ROSEN_LOGE("RSBufferAvailableCallbackStub::OnRemoteRequest error");
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS