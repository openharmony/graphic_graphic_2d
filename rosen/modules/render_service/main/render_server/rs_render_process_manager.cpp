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

#include "rs_ipc_persistence_data.h"
#include "rs_render_service.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManager"

namespace OHOS {
namespace Rosen {
sptr<RSRenderProcessManager> RSRenderProcessManager::Create(RSRenderService& renderService)
{
    sptr<RSRenderProcessManager> renderProcessManager;
    if (renderService.renderModeConfig_->GetIsMultiProcessModeEnabled()) {
        renderProcessManager = sptr<RSMultiRenderProcessManager>::MakeSptr(renderService);
    } else {
        renderProcessManager = sptr<RSSingleRenderProcessManager>::MakeSptr(renderService);
    }
    return renderProcessManager;
}

void RSRenderProcessManager::OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (auto ipcPersistenceManager = GetIpcPersistenceManager()) {
        auto data = std::make_shared<OnHwcEventPersistenceData>(deviceId, eventId, eventData);
        ipcPersistenceManager->RegisterWithoutCallingPid(data);
    }
    auto serviceToRenderConns = GetServiceToRenderConns();
    for (const auto& conn : serviceToRenderConns) {
        RS_LOGI("%{public}s: OnHwcEvent start", __func__);
        conn->HandleHwcEvent(deviceId, eventId, eventData);
    }
}

void RSRenderProcessManager::OnScreenBacklightChanged(ScreenId id, uint32_t level)
{
    auto conn = GetServiceToRenderConn(id);
    if (conn == nullptr) {
        RS_LOGE("%{public}s conn is nullptr", __func__);
        return;
    }
    conn->OnScreenBacklightChanged(id, level);
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