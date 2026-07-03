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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H

#include <chrono>
#include <condition_variable>
#include <csignal>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <refbase.h>
#include <unordered_map>
#include <unordered_set>

#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#include "rs_render_mode_config.h"
#include "rs_render_process_manager.h"

namespace OHOS {
namespace Rosen {
class ProcessUniqueId {
public:
    ProcessUniqueId(pid_t pid, uint64_t timeStamp) : pid_(pid), timestamp_(timeStamp) {}
    bool operator==(const ProcessUniqueId& other) const
    {
        return GetPid() == other.GetPid() && GetTimestamp() == other.GetTimestamp();
    }
    struct Hash {
        size_t operator()(const ProcessUniqueId& token) const
        {
            return std::hash<pid_t>()(token.GetPid()) ^ (std::hash<uint64_t>()(token.GetTimestamp()) << 1);
        }
    };
    pid_t GetPid() const { return pid_; }
    uint64_t GetTimestamp() const { return timestamp_; }

private:
    pid_t pid_ = -1;
    uint64_t timestamp_ = 0;
};

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
    sptr<RSScreenProperty> GetPendingScreenProperty(pid_t pid);
    bool SetRenderProcessReadyPromise(pid_t pid, const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
        const sptr<RSIConnectToRenderProcess>& connectToRenderConnection);

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override;
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override;

    std::shared_ptr<RSIpcPersistenceManager> GetIpcPersistenceManager() const override;

    bool IsValidRenderProcessPid(pid_t pid) const override;

private:

    sptr<IRemoteObject> HandleExistingGroup(ProcessUniqueId token, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    sptr<IRemoteObject> HandleNewGroup(GroupId groupId, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    GroupId GetGroupIdByScreenId(ScreenId screenId) const;
    std::optional<GroupId> CheckGroupIdByScreenId(ScreenId screenId) const;

    std::optional<ProcessUniqueId> GetRenderProcessUniqueIdByGroupId(GroupId groupId) const;
    std::optional<ProcessUniqueId> GetRenderProcessUniqueIdByGroupIdLocked(GroupId groupId) const;
    void UpdateGroupIdToRenderProcessUniqueId(GroupId groupId, ProcessUniqueId token);
    void UpdateGroupIdToRenderProcessUniqueIdLocked(GroupId groupId, ProcessUniqueId token);
    void RemoveGroupIdByRenderProcessUniqueId(ProcessUniqueId token);
    void RemoveGroupIdByRenderProcessUniqueIdLocked(ProcessUniqueId token);

    std::optional<ProcessUniqueId> GetValidRenderProcessUniqueIdByPid(pid_t pid) const;
    std::optional<ProcessUniqueId> GetValidRenderProcessUniqueIdByPidLocked(pid_t pid) const;

    // GetServiceToRenderConnByUniqueId is with lock and returns nullptr when no connection is found for the given token
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByUniqueId(ProcessUniqueId token) const;
    // GotServiceToRenderConnByUniqueId is with lock and throws std::out_of_range when no connection is found
    sptr<RSIServiceToRenderConnection> GotServiceToRenderConnByUniqueId(ProcessUniqueId token) const;
    // GetServiceToRenderConnByUniqueIdLocked is without lock and returns nullptr when no connection is found
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByUniqueIdLocked(ProcessUniqueId token) const;

    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByUniqueId(ProcessUniqueId token) const;
    sptr<RSIConnectToRenderProcess> GotConnectToRenderConnByUniqueId(ProcessUniqueId token) const;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByUniqueIdLocked(ProcessUniqueId token) const;

    sptr<IRSComposerToRenderConnection> GotComposerToRenderConnByUniqueId(ProcessUniqueId token) const;
    sptr<IRSComposerToRenderConnection> GetComposerToRenderConnByUniqueId(ProcessUniqueId token) const;
    sptr<IRSComposerToRenderConnection> GetComposerToRenderConnByUniqueIdLocked(ProcessUniqueId token) const;

    ScreenId InsertVirtualToPhysicalScreenMap(ScreenId screenId, ScreenId associatedScreenId);
    std::optional<ScreenId> DeleteVirtualToPhysicalScreenMap(ScreenId id);
    ScreenId FindVirtualToPhysicalScreenMap(ScreenId screenId);

    class RenderProcessDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RenderProcessDeathRecipient(ProcessUniqueId token, wptr<RSMultiRenderProcessManager> manager)
            : token_(token), manager_(manager) {}
        ~RenderProcessDeathRecipient() noexcept override = default;
        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

    private:
        ProcessUniqueId token_;
        wptr<RSMultiRenderProcessManager> manager_;
    };

    void RegisterDeathRecipient(ProcessUniqueId token, const sptr<IRemoteObject>& binderObject);
    void RegisterDeathRecipientLocked(ProcessUniqueId token, const sptr<IRemoteObject>& binderObject);
    void UnregisterDeathRecipient(ProcessUniqueId token);
    void UnregisterDeathRecipientLocked(ProcessUniqueId token);
    void HandleRenderProcessDeath(ProcessUniqueId token);
    std::unordered_map<ProcessUniqueId, sptr<RenderProcessDeathRecipient>, ProcessUniqueId::Hash> deathRecipients_;

    void AddValidRenderProcessUniqueId(ProcessUniqueId token);
    void AddValidRenderProcessUniqueIdLocked(ProcessUniqueId token);
    void RemoveValidRenderProcessUniqueId(ProcessUniqueId token);
    void RemoveValidRenderProcessUniqueIdLocked(ProcessUniqueId token);
    bool IsValidRenderProcessUniqueIdLocked(ProcessUniqueId token) const;

    void AddScreenOutputToProcess(ProcessUniqueId token, ScreenId screenId, const std::shared_ptr<HdiOutput>& output);
    void AddScreenOutputToProcessLocked(ProcessUniqueId token, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output);
    void RemoveScreenOutputFromProcess(ProcessUniqueId token, ScreenId screenId);

    mutable std::mutex mutex_;
    mutable std::condition_variable validPidCv_;
    std::unordered_set<ProcessUniqueId, ProcessUniqueId::Hash> validRenderProcessUniqueIds_;
    std::unordered_map<GroupId, ProcessUniqueId> groupIdToRenderProcessUniqueId_;
    std::unordered_map<ProcessUniqueId, std::promise<bool>, ProcessUniqueId::Hash> renderProcessReadyPromises_;
    std::unordered_map<ProcessUniqueId, sptr<IRSComposerToRenderConnection>, ProcessUniqueId::Hash>
        composerToRenderConnections_;
    std::unordered_map<ProcessUniqueId, sptr<RSIServiceToRenderConnection>, ProcessUniqueId::Hash>
        serviceToRenderConnections_;
    std::unordered_map<ProcessUniqueId, sptr<RSIConnectToRenderProcess>, ProcessUniqueId::Hash>
        connectToRenderConnections_;
    std::unordered_map<ProcessUniqueId,
        std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>, ProcessUniqueId::Hash>
        processToScreenOutputMap_;
    std::function<void(std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>&)> renderProcessDeathCallback_;

    mutable std::mutex virtualScreenMutex_;
    std::map<ScreenId, ScreenId> virtualToPhysicalScreenMap_;

    struct PendingScreenConnectInfo {
        ScreenId screenId = INVALID_SCREEN_ID;
        sptr<RSScreenProperty> property = nullptr;
    };
    std::unordered_map<ProcessUniqueId, PendingScreenConnectInfo, ProcessUniqueId::Hash> pendingScreenConnectInfos_;

    const std::shared_ptr<RSIpcPersistenceManager> ipcPersistenceManager_;

    std::deque<std::chrono::steady_clock::time_point> subprocessDeathTimes_;
    std::mutex subprocessDeathTimesMutex_;
    void CheckAndHandleSubprocessDeathOverflow();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H
