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
namespace {
bool IsInvalidConnectInfo(const sptr<ConnectToServiceInfo>& result)
{
    return !result ||
           !result->composerToRenderConnection_ ||
           !result->vsyncToken_;
}
} // namespace

RSRenderToServiceConnection::RSRenderToServiceConnection(sptr<RSRenderServiceAgent> renderServiceAgent,
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent, sptr<RSScreenManagerAgent> screenManagerAgent)
    : renderServiceAgent_(renderServiceAgent),
      renderProcessManagerAgent_(renderProcessManagerAgent),
      screenManagerAgent_(screenManagerAgent) {}

bool RSRenderToServiceConnection::NotifyRenderProcessInitFinished(
    const sptr<IRemoteObject>& serviceToRenderConnection, const sptr<IRemoteObject>& connectToRenderConnection)
{
    auto serviceToRenderConn = iface_cast<RSIServiceToRenderConnection>(serviceToRenderConnection);
    auto connectToRenderConn = iface_cast<RSIConnectToRenderProcess>(connectToRenderConnection);
    if (!serviceToRenderConn || !connectToRenderConn) {
        RS_LOGE("%{public}s: iface_cast failed", __func__);
        return false;
    }
    renderProcessManagerAgent_->SetRenderProcessReadyPromise(GetCallingPid(), serviceToRenderConn, connectToRenderConn);
    return true;
}

sptr<ReplyToRenderInfo> RSRenderToServiceConnection::SendProcessInfo(
    const sptr<ConnectToServiceInfo>& connectToServiceInfo)
{
    if (IsInvalidConnectInfo(connectToServiceInfo)) {
        RS_LOGE("%{public}s: connectToServiceInfo is invalid", __func__);
        return nullptr;
    }
    const auto remotePid = GetCallingPid();
    auto composerToRenderConn =
        iface_cast<IRSComposerToRenderConnection>(connectToServiceInfo->composerToRenderConnection_);
    if (!composerToRenderConn) {
        RS_LOGE("%{public}s: composerToRenderConn is nullptr", __func__);
        return nullptr;
    }
    // preparing required infos
    RS_LOGI("%{public}s: Preparing Required Infos", __func__);
    auto [rsScreenProperty, replayData] = renderProcessManagerAgent_->GetProcessInfo(remotePid, composerToRenderConn);
    if (!rsScreenProperty) {
        RS_LOGE("%{public}s: rsScreenProperty is nullptr", __func__);
        return nullptr;
    }
    auto [renderToComposerConnection, vsyncConnection] =
        renderServiceAgent_->GetProcessInfo(rsScreenProperty->GetScreenId(), connectToServiceInfo->vsyncToken_);
    if (!renderToComposerConnection || !vsyncConnection) {
        RS_LOGE("%{public}s: renderToComposerConnection or vsyncConnection is nullptr", __func__);
        return nullptr;
    }
    return sptr<ReplyToRenderInfo>::MakeSptr(
        renderToComposerConnection->AsObject(), rsScreenProperty, vsyncConnection->AsObject(), replayData);
}

sptr<HgmServiceToProcessInfo> RSRenderToServiceConnection::NotifyRpHgmFrameRate(uint64_t timestamp,
    uint64_t vsyncId, const sptr<HgmProcessToServiceInfo>& processToServiceInfo)
{
    auto serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    if (auto hgmProcessCallback = renderServiceAgent_->GetHgmProcessCallback()) {
        hgmProcessCallback(timestamp, vsyncId, processToServiceInfo, serviceToProcessInfo);
    }
    return serviceToProcessInfo;
}

void RSRenderToServiceConnection::NotifyScreenSwitchFinished(ScreenId screenId)
{
    screenManagerAgent_->SetScreenSwitchStatus(false);
}
} // namespace Rosen
} // namespace OHOS