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

#include "rs_render_service.h"
#include "transaction/rs_service_to_render_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManager"

namespace OHOS {
namespace Rosen {
sptr<RSRenderProcessManager> RSRenderProcessManager::Create(RSRenderService& renderService)
{
    auto renderProcessManager = sptr<RSSingleRenderProcessManager>::MakeSptr(renderService);
    return renderProcessManager;
}

void RSRenderProcessManager::OnVBlankIdle(ScreenId id, uint64_t ns)
{
}

void RSRenderProcessManager::OnActiveScreenIdChanged(ScreenId activeScreenId)
{
}

void RSRenderProcessManager::OnHwcRestored(ScreenId id, const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
}

void RSRenderProcessManager::OnHwcDead(ScreenId id)
{
}

void RSRenderProcessManager::OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
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