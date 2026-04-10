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

#include "active_screen_id_changed_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {

void RSActiveScreenIdChangedCallbackProxy::OnActiveScreenIdChanged(ScreenId activeScreenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIActiveScreenIdChangedCallback::GetDescriptor())) {
        ROSEN_LOGE("RSActiveScreenIdChangedCallbackProxy::OnActiveScreenIdChanged WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(activeScreenId)) {
        ROSEN_LOGE("RSActiveScreenIdChangedCallbackProxy::OnActiveScreenIdChanged WriteUint64 failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIActiveScreenIdChangedCallbackInterfaceCode::ON_ACTIVE_SCREEN_ID_CHANGED);

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSActiveScreenIdChangedCallbackProxy::OnActiveScreenIdChanged: remote is null!");
        return;
    }

    int32_t ret = { remote->SendRequest(code, data, reply, option) };
    if (ret != NO_ERROR) {
        ROSEN_LOGE("RSActiveScreenIdChangedCallbackProxy::OnActiveScreenIdChanged error = %{public}d", ret);
    }
}

} // namespace Rosen
} // namespace OHOS
