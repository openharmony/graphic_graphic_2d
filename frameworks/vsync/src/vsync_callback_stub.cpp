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

#include "vsync_callback_stub.h"
#include "vsync_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncCallbackStub" };
}

int32_t VsyncCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
                                           MessageParcel& reply, MessageOption& option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case IVSYNC_CALLBACK_ON_VSYNC: {
            int64_t timestamp;

            bool ret = data.ReadInt64(timestamp);
            if (!ret) {
                VLOG_FAILURE("need param");
                return 1;
            }

            OnVsync(timestamp);
        } break;
        default: {
            VLOG_FAILURE("code %{public}d cannot process", code);
            return 1;
        } break;
    }
    return 0;
}
} // namespace OHOS
