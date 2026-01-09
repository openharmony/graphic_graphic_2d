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

#include "transaction/rs_render_to_service_connection.h"

#include "dfx/rs_service_dump_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToServiceConnection"

namespace OHOS {
namespace Rosen {
RSRenderToServiceConnection::RSRenderToServiceConnection(sptr<RSRenderServiceAgent> renderServiceAgent,
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent, sptr<RSScreenManagerAgent> screenManagerAgent)
    : renderServiceAgent_(renderServiceAgent),
      renderProcessManagerAgent_(renderProcessManagerAgent),
      screenManagerAgent_(screenManagerAgent) {}

sptr<HgmServiceToProcessInfo> RSRenderToServiceConnection::NotifyRpHgmFrameRate(uint64_t timestamp,
    uint64_t vsyncId, const sptr<HgmProcessToServiceInfo>& processToServiceInfo)
{
    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    renderServiceAgent_->ProcessHgmFrameRate(timestamp, vsyncId, processToServiceInfo, serviceToProcessInfo);
    return serviceToProcessInfo;
}

void RSRenderToServiceConnection::NotifyScreenSwitchFinished(ScreenId screenId)
{
    screenManagerAgent_->SetScreenSwitchStatus(screenId, false);
}
} // namespace Rosen
} // namespace OHOS