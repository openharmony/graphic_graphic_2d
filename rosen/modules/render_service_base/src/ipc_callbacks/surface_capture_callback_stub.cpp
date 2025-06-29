/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "surface_capture_callback_stub.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSSurfaceCaptureCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSISurfaceCaptureCallback::GetDescriptor()) {
        ROSEN_LOGE("RSSurfaceCaptureCallbackStub: token ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSISurfaceCaptureCallbackInterfaceCode::ON_SURFACE_CAPTURE): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSSurfaceCaptureCallbackStub::ON_SURFACE_CAPTURE read nodeId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSSurfaceCaptureCallbackStub: ReadSurfaceCaptureConfig failed");
                break;
            }
            auto pixelmap = data.ReadParcelable<OHOS::Media::PixelMap>();
            OnSurfaceCapture(id, captureConfig, pixelmap);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}

bool RSSurfaceCaptureCallbackStub::ReadSurfaceCaptureConfig(RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    // read mainScreenRect only to reduce ipc data size
    if (!data.ReadFloat(captureConfig.mainScreenRect.left_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.top_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.right_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.ReadUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.ReadBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.left_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.top_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.right_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.bottom_)) {
        RS_LOGE("RSSurfaceCaptureCallbackStub::ReadSurfaceCaptureConfig read parcel failed!");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
