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

#include "rs_occlusion_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSOcclusionChangeCallbackProxy::RSOcclusionChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIOcclusionChangeCallback>(impl)
{
}

void RSOcclusionChangeCallbackProxy::OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIOcclusionChangeCallback::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteParcelable(occlusionData.get());
    int32_t err = Remote()->SendRequest(RSIOcclusionChangeCallback::ON_OCCLUSION_VISIBLE_CHANGED, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderOcclusionChangeCallbackProxy::OnOcclusionVisibleChanged error = %d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
