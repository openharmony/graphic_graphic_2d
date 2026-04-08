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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H

#include <memory>
#include <mutex>
#include <optional>
#include <refbase.h>
#include <unordered_map>

#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#include "rs_render_mode_config.h"
#include "rs_render_process_manager.h"

namespace OHOS {
namespace Rosen {
class RSRenderComposerManager;
class RSRenderComposerAgent;
class RSMultiRenderProcessManager : public RSRenderProcessManager {
public:
    explicit RSMultiRenderProcessManager(RSRenderService& renderService);
    ~RSMultiRenderProcessManager() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(
        ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property) override;
    void OnScreenDisconnected(ScreenId screenId) override;
    void OnScreenPropertyChanged(
        ScreenId screenId, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property) override;
    void OnScreenRefresh(ScreenId screenId) override;
    void OnVirtualScreenConnected(
        ScreenId screenId, ScreenId associatedScreenId, const sptr<RSScreenProperty>& property) override;
    void OnVirtualScreenDisconnected(ScreenId screenId) override;

    void RecordComposerToRenderConnection(pid_t pid, sptr<IRSComposerToRenderConnection> composerToRenderConnection);
    sptr<RSScreenProperty> GetPendingScreenProperty(pid_t pid) const;
    void SetRenderProcessReadyPromise(pid_t pid, const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
        const sptr<RSIConnectToRenderProcess>& connectToRenderConnection);

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override;
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override;

    std::shared_ptr<RSIpcReplayManager> GetIpcReplayManager() const override;

private:
    sptr<IRemoteObject> HandleExistingGroup(pid_t pid, ScreenId screenId, const sptr<RSScreenProperty>& property);
    sptr<IRemoteObject> HandleNewGroup(GroupId groupId, ScreenId screenId, const sptr<RSScreenProperty>& property);
    GroupId GetGroupIdByScreenId(ScreenId screenId) const;
    std::optional<GroupId> CheckGroupIdByScreenId(ScreenId screenId) const;

    std::optional<pid_t> GetRenderProcessPidByGroupId(GroupId groupId) const;
    std::optional<pid_t> GetRenderProcessPidByGroupIdLocked(GroupId groupId) const;
    void UpdateGroupIdToRenderProcessPid(GroupId groupId, pid_t pid);

    // GetServiceToRenderProcessConnByPid is with lock and returns nullptr when no connection is found for the given pid
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByPid(pid_t pid) const;
    // GotServiceToRenderConnByPid is locked and throws an exception when no connection is found for the give pid
    sptr<RSIServiceToRenderConnection> GotServiceToRenderConnByPid(pid_t pid) const;
    // GetServiceToRenderProcessConnByPid is without lock and returns nullptr
    // when no connection is found for the given pid
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByPidLocked(pid_t pid) const;

    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByPid(pid_t pid) const;
    sptr<RSIConnectToRenderProcess> GotConnectToRenderConnByPid(pid_t pid) const;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByPidLocked(pid_t pid) const;

    sptr<IRSComposerToRenderConnection> GotComposerToRenderConnByPid(pid_t pid) const;

    ScreenId InsertVirtualToPhysicalScreenMap(ScreenId screenId, ScreenId associatedScreenId);
    std::optional<ScreenId> DeleteVirtualToPhysicalScreenMap(ScreenId id);
    ScreenId FindVirtualToPhysicalScreenMap(ScreenId screenId);

    std::unordered_map<pid_t, std::promise<bool>> renderProcessReadyPromises_;

    mutable std::mutex mutex_;
    std::unordered_map<GroupId, pid_t> groupIdToRenderProcessPid_;
    std::unordered_map<pid_t, sptr<IRSComposerToRenderConnection>> composerToRenderConnections_;
    std::unordered_map<pid_t, sptr<RSIServiceToRenderConnection>> serviceToRenderConnections_;
    std::unordered_map<pid_t, sptr<RSIConnectToRenderProcess>> connectToRenderConnections_;

    mutable std::mutex virtualScreenMutex_;
    std::map<ScreenId, ScreenId> virtualToPhysicalScreenMap_;

    struct PendingScreenConnectInfo {
        ScreenId screenId = INVALID_SCREEN_ID;
        sptr<RSScreenProperty> property = nullptr;
    };
    std::unordered_map<pid_t, PendingScreenConnectInfo> pendingScreenConnectInfos_;

    const std::shared_ptr<RSIpcReplayManager> ipcReplayManager_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H
