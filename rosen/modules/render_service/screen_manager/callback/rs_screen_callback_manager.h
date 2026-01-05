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

#ifndef RS_SCREEN_CALLBACK_MANAGER
#define RS_SCREEN_CALLBACK_MANAGER

#include <memory>
#include <vector>

#include "hdi_backend.h"
#include "../public/rs_iscreen_manager_listener.h"
#include "../public/rs_iscreen_manager_agent_listener.h"
#include "screen_manager/rs_screen_info.h"

namespace OHOS {
namespace Rosen {
struct ScreenPresenceEvent {
    ScreenId id;
    std::shared_ptr<HdiOutput> output;
    const sptr<RSScreenProperty> property;
};

class RSScreenCallbackManager {
public:
    void SetCoreListener(const sptr<RSIScreenManagerListener>& listener);
    sptr<RSIScreenManagerListener> GetCoreListener() const;

    void AddAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);
    void RemoveAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);

    void NotifyScreenConnected(const ScreenPresenceEvent& event);
    void NotifyScreenDisconnected(ScreenId id);
    void NotifyScreenPropertyUpdated(ScreenId id, const sptr<RSScreenProperty>& property);
    void NotifyScreenRefresh(ScreenId id);
    void NotifyHwcRestored(const ScreenPresenceEvent& event);
    void NotifyHwcDead(ScreenId id);
    void NotifyHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData);
    void NotifyVBlankIdle(ScreenId id, uint64_t ns);
    void NotifyVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId, sptr<RSScreenProperty> property);
    void NotifyVirtualScreenDisconnected(ScreenId id);
    void NotifyActiveScreenIdChanged(ScreenId activeScreenId);
    void NotifyScreenBacklightChanged(ScreenId id, uint32_t level);
    void NotifyGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList);

    sptr<IRemoteObject> GetClientToRenderConnection(ScreenId id) const;

private:
    void NotifyScreenConnectedToAgentListeners(ScreenId id, ScreenChangeReason reason, sptr<IRemoteObject> remoteConn);
    void NotifyScreenDisconnectedToAgentListeners(ScreenId id, ScreenChangeReason reason);

    std::unordered_map<ScreenId, sptr<IRemoteObject>> clientToRenderConns_;
    sptr<RSIScreenManagerListener> coreListener_;
    std::vector<sptr<RSIScreenManagerAgentListener>> agentListeners_;
    mutable std::mutex agentMtx_;
    mutable std::mutex clientToRenderMtx_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_SCREEN_CALLBACK_MANAGER