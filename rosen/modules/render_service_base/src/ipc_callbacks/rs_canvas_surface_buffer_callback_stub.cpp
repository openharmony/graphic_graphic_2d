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

#include "rs_canvas_surface_buffer_callback_stub.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/common/rs_log.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {

int RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSICanvasSurfaceBufferCallback::GetDescriptor()) {
        ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest token ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    if (code == static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::ON_CANVAS_SURFACE_BUFFER_CHANGED)) {
        NodeId nodeId = 0;
        if (!data.ReadUint64(nodeId)) {
            ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest read nodeId failed");
            return ERR_INVALID_DATA;
        }

        uint32_t resetSurfaceIndex = 0;
        if (!data.ReadUint32(resetSurfaceIndex)) {
            ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest read resetSurfaceIndex failed");
            return ERR_INVALID_DATA;
        }

        bool hasBuffer = false;
        if (!data.ReadBool(hasBuffer)) {
            ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest read hasBuffer failed");
            return ERR_INVALID_DATA;
        }

        sptr<SurfaceBuffer> buffer = nullptr;
        if (hasBuffer) {
            buffer = SurfaceBuffer::Create();
            if (buffer == nullptr) {
                ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest SurfaceBuffer::Create failed");
                return ERR_INVALID_DATA;
            }
            GSError gsRet = buffer->ReadFromMessageParcel(data);
            if (gsRet != GSERROR_OK) {
                ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest ReadFromMessageParcel failed, "
                    "ret=%{public}d", gsRet);
                return ERR_INVALID_DATA;
            }
        }
        OnCanvasSurfaceBufferChanged(nodeId, buffer, resetSurfaceIndex);
    } else {
        ROSEN_LOGE("RSCanvasSurfaceBufferCallbackStub::OnRemoteRequest unhandled code=%{public}u", code);
        ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK