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

#include "screen_supported_hdr_formats_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "ipc_callbacks/screen_supported_hdr_formats_callback_ipc_interface_code.h"
#include "platform/common/rs_log.h"


namespace OHOS {
namespace Rosen {
RSScreenSupportedHdrFormatsCallbackProxy::RSScreenSupportedHdrFormatsCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIScreenSupportedHdrFormatsCallback>(impl)
{
}

void RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIScreenSupportedHdrFormatsCallback::GetDescriptor())) {
        ROSEN_LOGE("RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate"
            " WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate WriteUint64 failed");
        return;
    }
    std::vector<uint32_t> hdrFormatsSend;
    std::copy(hdrFormats.begin(), hdrFormats.end(), std::back_inserter(hdrFormatsSend));

    if (!data.WriteUInt32Vector(hdrFormatsSend)) {
        ROSEN_LOGE("RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate"
            " WriteUInt32Vector failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>
        (RSIScreenSupportedHdrFormatsCallbackInterfaceCode::ON_SCREEN_SUPPORTED_HDR_FORMATS_UPDATE);

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate: remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSScreenSupportedHdrFormatsCallbackProxy::OnScreenSupportedHDRFormatsUpdate"
            " error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS