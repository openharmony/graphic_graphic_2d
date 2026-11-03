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

#include "rs_render_process_manager.h"

#include "render_process/transaction/ipc_persistence/rs_ipc_persistence_manager.h"
#include "render_process/transaction/zidl/transfers/rs_on_hwc_event_transfer.h"
#include "rs_render_service.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManager"

namespace OHOS {
namespace Rosen {
sptr<RSRenderProcessManager> RSRenderProcessManager::Create(
    RSRenderService& renderService, HgmProcessCallback hgmProcessCallback)
{
    sptr<RSRenderProcessManager> renderProcessManager;
    if (renderService.renderModeConfig_->GetIsMultiProcessModeEnabled()) {
        renderProcessManager = sptr<RSMultiRenderProcessManager>::MakeSptr(renderService);
    } else {
        renderProcessManager =
            sptr<RSSingleRenderProcessManager>::MakeSptr(renderService, std::move(hgmProcessCallback));
    }
    return renderProcessManager;
}

void RSRenderProcessManager::OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    RS_LOGD("%{public}s: deviceId:%{public}u, eventId:%{public}u, eventDataSize:%{public}zu",
        __func__, deviceId, eventId, eventData.size());
    auto transfer = std::make_shared<OnHwcEventTransfer>(
        std::make_shared<OnHwcEventInput>(deviceId, eventId, eventData));
    if (SendTransfer(transfer) != StatusCode::SUCCESS) {
        RS_LOGD("%{public}s: SendTransfer failed", __func__);
    }
}

void RSRenderProcessManager::OnScreenBacklightChanged(const RsScreenBrightnessData& brightnessData)
{
    auto conn = GetServiceToRenderConn(brightnessData.screenId);
    if (conn == nullptr) {
        RS_LOGE("%{public}s conn is nullptr", __func__);
        return;
    }
    conn->OnScreenBacklightChanged(brightnessData);
}

void RSRenderProcessManager::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    auto serviceToRenderConns = GetServiceToRenderConns();
    for (const auto& conn : serviceToRenderConns) {
        if (conn == nullptr) {
            continue;
        }
        conn->OnGlobalBlacklistChanged(globalBlackList);
    }
}
} // namespace Rosen
} // namespace OHOS
