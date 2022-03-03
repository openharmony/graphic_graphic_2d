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
#include "screen_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {
RSScreenChangeCallbackProxy::RSScreenChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIScreenChangeCallback>(impl)
{
}

void RSScreenChangeCallbackProxy::OnScreenChanged(ScreenId id, ScreenEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIScreenChangeCallback::GetDescriptor())) {
        return;
    }

    data.WriteUint64(id);
    data.WriteUint8(ECast(event));

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(RSIScreenChangeCallback::ON_SCREEN_CHANGED, data, reply, option);
    if (err != NO_ERROR) {
        // [PLANNING]: Error log
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
