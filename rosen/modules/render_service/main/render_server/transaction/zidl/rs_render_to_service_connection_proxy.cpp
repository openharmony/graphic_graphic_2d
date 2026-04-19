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

#include "rs_render_to_service_connection_proxy.h"
#include "gfx/dump/rs_dump_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToServiceConnectionProxy"

namespace OHOS {
namespace Rosen {
RSRenderToServiceConnectionProxy::RSRenderToServiceConnectionProxy(
    const sptr<IRemoteObject>& impl) : IRemoteProxy<RSIRenderToServiceConnection>(impl) {}

sptr<HgmServiceToProcessInfo> RSRenderToServiceConnectionProxy::NotifyRpHgmFrameRate(uint64_t timestamp,
    uint64_t vsyncId, const sptr<HgmProcessToServiceInfo>& processToServiceInfo)
{
    return nullptr;
}

void RSRenderToServiceConnectionProxy::NotifyScreenSwitchFinished(ScreenId screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIRenderToServiceConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCH_FINISHED);
    if (!data.WriteUint64(screenId)) {
        RS_LOGE("%{public}s: WriteUint64 failed", __func__);
        return;
    }
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return;
    }
}

} // namespace Rosen
} // namespace OHOS