/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_occlusion_change_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSOcclusionChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIOcclusionChangeCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case ON_OCCLUSION_VISIBLE_CHANGED: {
            std::shared_ptr<RSOcclusionData> occlusionData(data.ReadParcelable<RSOcclusionData>());
            OnOcclusionVisibleChanged(occlusionData);
            break;
        }
        default: {
            ret = ERR_UNKNOWN_REASON;
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
