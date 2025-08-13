/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_surface_occlusion_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceOcclusionChangeCallbackProxy::RSSurfaceOcclusionChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSISurfaceOcclusionChangeCallback>(impl)
{
}

void RSSurfaceOcclusionChangeCallbackProxy::OnSurfaceOcclusionVisibleChanged(float visibleAreaRatio)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSISurfaceOcclusionChangeCallback::GetDescriptor())) {
        ROSEN_LOGE("RSSurfaceOcclusionChangeCallbackProxy::OnSurfaceOcclusionVisibleChanged "
                   "WriteInterfaceToken failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteFloat(visibleAreaRatio)) {
        ROSEN_LOGE("RSSurfaceOcclusionChangeCallbackProxy::OnSurfaceOcclusionVisibleChanged WriteFloat failed");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSISurfaceOcclusionChangeCallbackInterfaceCode::ON_SURFACE_OCCLUSION_VISIBLE_CHANGED);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSSurfaceOcclusionChangeCallbackProxy::OnSurfaceOcclusionVisibleChanged error = %d", err);
    }
}

} // namespace Rosen
} // namespace OHOS
