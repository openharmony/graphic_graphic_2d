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

#include "screen_supported_hdr_formats_callback_stub.h"

#include "ipc_callbacks/screen_supported_hdr_formats_callback_ipc_interface_code.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSScreenSupportedHDRFormatsCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIScreenSupportedHdrFormatsCallback::GetDescriptor()) {
        ROSEN_LOGE("RSScreenSupportedHDRFormatsCallbackStub::OnRemoteRequest WriteInterfaceToken failed");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIScreenSupportedHdrFormatsCallbackInterfaceCode::ON_SCREEN_SUPPORTED_HDR_FORMATS_UPDATE): {
            ScreenId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSScreenSupportedHDRFormatsCallbackStub::ON_SCREEN_SUPPORTED_HDR_FORMATS_UPDATE read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<uint32_t> hdrFormatsRecv;
            if (!data.ReadUInt32Vector(&hdrFormatsRecv)) {
                RS_LOGE("RSScreenSupportedHDRFormatsCallbackStub::ON_SCREEN_SUPPORTED_HDR_FORMATS_UPDATE ReadUInt32Vector failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<ScreenHDRFormat> hdrFormats;
            std::transform(hdrFormatsRecv.begin(), hdrFormatsRecv.end(), back_inserter(hdrFormats),
                [](uint32_t i) -> ScreenHDRFormat {return static_cast<ScreenHDRFormat>(i);});
            OnScreenSupportedHDRFormatsUpdate(id, hdrFormats);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS