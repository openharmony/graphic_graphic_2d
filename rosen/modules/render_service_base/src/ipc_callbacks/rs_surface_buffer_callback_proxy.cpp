/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_surface_buffer_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceBufferCallbackProxy::RSSurfaceBufferCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSISurfaceBufferCallback>(impl)
{
}

void RSSurfaceBufferCallbackProxy::OnFinish(uint64_t uid, const std::vector<uint32_t>& surfaceBufferIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSISurfaceBufferCallback::GetDescriptor())) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy: data.WriteInterfaceToken error");
        return;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write uid error");
        return;
    }
    if (!data.WriteUInt32Vector(surfaceBufferIds)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write surfaceBufferIds error");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy: Remote()->SendRequest() error");
    }
}
} // namespace Rosen
} // namespace OHOS
