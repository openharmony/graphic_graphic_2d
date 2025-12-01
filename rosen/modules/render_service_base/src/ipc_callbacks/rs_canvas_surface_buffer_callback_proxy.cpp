/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_canvas_surface_buffer_callback_proxy.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include <message_option.h>
#include <message_parcel.h>

#include "ipc_callbacks/rs_ipc_callbacks_check.h"
#include "platform/common/rs_log.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {

RSCanvasSurfaceBufferCallbackProxy::RSCanvasSurfaceBufferCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSICanvasSurfaceBufferCallback>(impl)
{}

void RSCanvasSurfaceBufferCallbackProxy::OnCanvasSurfaceBufferChanged(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSICanvasSurfaceBufferCallback::GetDescriptor())) {
        ROSEN_LOGE("OnCanvasSurfaceBufferChanged WriteInterfaceToken failed, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("OnCanvasSurfaceBufferChanged write nodeId failed, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    if (!data.WriteUint32(resetSurfaceIndex)) {
        ROSEN_LOGE("OnCanvasSurfaceBufferChanged write resetSurfaceIndex failed, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    if (buffer != nullptr) {
        if (!data.WriteBool(true)) {
            ROSEN_LOGE(
                "OnCanvasSurfaceBufferChanged write hasBuffer flag[true] failed, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
        GSError ret = buffer->WriteToMessageParcel(data);
        if (ret != GSERROR_OK) {
            ROSEN_LOGE("OnCanvasSurfaceBufferChanged WriteToMessageParcel failed, ret=%{public}d, "
                "nodeId=%{public}" PRIu64, ret, nodeId);
            return;
        }
    } else if (!data.WriteBool(false)) {
        ROSEN_LOGE("OnCanvasSurfaceBufferChanged write hasBuffer flag[false] failed, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    uint32_t code =
        static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::ON_CANVAS_SURFACE_BUFFER_CHANGED);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE(
            "OnCanvasSurfaceBufferChanged SendRequest failed, err=%{public}d, nodeId=%{public}" PRIu64, err, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK