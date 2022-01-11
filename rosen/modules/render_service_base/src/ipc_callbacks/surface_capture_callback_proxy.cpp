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
#include "surface_capture_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceCaptureCallbackProxy::RSSurfaceCaptureCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSISurfaceCaptureCallback>(impl)
{
}

void RSSurfaceCaptureCallbackProxy::OnSurfaceCapture(NodeId id, Media::PixelMap* pixelmap)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSISurfaceCaptureCallback::GetDescriptor())) {
        ROSEN_LOGE("SurfaceCaptureCallbackProxy: data.WriteInterfaceToken error");
        return;
    }
    data.WriteUint64(id);
    data.WriteParcelable(pixelmap);
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(RSISurfaceCaptureCallback::ON_SURFACE_CAPTURE, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SurfaceCaptureCallbackProxy: Remote()->SendRequest() error");
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
