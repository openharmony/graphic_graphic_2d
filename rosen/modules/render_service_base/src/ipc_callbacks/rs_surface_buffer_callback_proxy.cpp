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

void RSSurfaceBufferCallbackProxy::OnFinish(const FinishCallbackRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSISurfaceBufferCallback::GetDescriptor())) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish data.WriteInterfaceToken error");
        return;
    }
    if (!data.WriteUint64(ret.uid)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write uid error");
        return;
    }
    if (!data.WriteUInt32Vector(ret.surfaceBufferIds)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write surfaceBufferIds error");
        return;
    }
    if (!data.WriteUInt8Vector(ret.isRenderedFlags)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write isRenderedFlags error");
        return;
    }
    if (!data.WriteBool(ret.isUniRender)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write isUniRender error");
        return;
    }
#ifdef ROSEN_OHOS
    if (!data.WriteUint64(static_cast<uint64_t>(ret.releaseFences.size()))) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnFinish write releaseFences Size error");
        return;
    }
    for (auto& releaseFence : ret.releaseFences) {
        releaseFence->WriteToMessageParcel(data);
    }
#endif
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy: SendRequest() error");
    }
}

void RSSurfaceBufferCallbackProxy::OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSISurfaceBufferCallback::GetDescriptor())) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnAfterAcquireBuffer data.WriteInterfaceToken error");
        return;
    }
    if (!data.WriteUint64(ret.uid)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnAfterAcquireBuffer write uid error");
        return;
    }
    if (!data.WriteBool(ret.isUniRender)) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy::OnAfterAcquireBuffer write isUniRender error");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_AFTER_ACQUIRE_BUFFER);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSSurfaceBufferCallbackProxy: SendRequest() error");
    }
}

} // namespace Rosen
} // namespace OHOS
