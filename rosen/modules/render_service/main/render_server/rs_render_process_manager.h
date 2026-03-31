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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_PROCESS_MANAGER_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_PROCESS_MANAGER_H

#include <vector>

#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#include "render_process/transaction/zidl/rs_iservice_to_render_connection.h"
#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"
#include "screen_manager/public/rs_iscreen_manager_listener.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSRenderService;
class RSRenderProcessManager : public RSIScreenManagerListener {
public:
    static sptr<RSRenderProcessManager> Create(RSRenderService& renderService);

    explicit RSRenderProcessManager(RSRenderService& renderService) : renderService_(renderService) {}
    virtual ~RSRenderProcessManager() noexcept = default;

    void OnVBlankIdle(ScreenId id, uint64_t ns) override;
    void OnScreenBacklightChanged(ScreenId id, uint32_t level) override;
    void OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList) override;
    void OnActiveScreenIdChanged(ScreenId activeScreenId) override;
    void OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) override;

    void OnHwcRestored(ScreenId id, const std::shared_ptr<HdiOutput>& output,
        const sptr<RSScreenProperty>& property) override;
    void OnHwcDead(ScreenId id) override;

    virtual sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const = 0;
    virtual std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const = 0;
    virtual sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const = 0;

protected:
    RSRenderService& renderService_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_PROCESS_MANAGER_H