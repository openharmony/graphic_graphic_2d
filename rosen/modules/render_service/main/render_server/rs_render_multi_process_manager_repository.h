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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_REPOSITORY_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_REPOSITORY_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <refbase.h>
#include <unordered_map>
#include <vector>

#include <hdi_output.h>

#include "common/rs_macros.h"
#include "irs_composer_to_render_connection.h"
#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#include "render_process/transaction/zidl/rs_iservice_to_render_connection.h"
#include "rs_render_mode_config.h"
#include "screen_manager/rs_screen_property.h"

namespace OHOS {
namespace Rosen {

class ProcessUniqueId {
public:
    explicit ProcessUniqueId(pid_t pid)
        : id_((static_cast<uint64_t>(static_cast<uint32_t>(pid)) << uniqueIdPidOffset) |
            counter_.fetch_add(1, std::memory_order_relaxed))
    {}
    bool operator==(const ProcessUniqueId& other) const { return id_ == other.id_; }
    struct Hash {
        size_t operator()(const ProcessUniqueId& processUniqueId) const
        {
            return std::hash<uint64_t>()(processUniqueId.id_);
        }
    };
    pid_t GetPid() const { return static_cast<pid_t>(id_ >> uniqueIdPidOffset); }

private:
    static constexpr uint32_t uniqueIdPidOffset = 32;
    static inline std::atomic<uint32_t> counter_{0};
    uint64_t id_;
};

class RSRenderMultiProcessManagerRepository {
public:
    using ScreenOutputList = std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>;

    struct InitialScreenInfo {
        ScreenId screenId = INVALID_SCREEN_ID;
        std::shared_ptr<HdiOutput> output;
        sptr<RSScreenProperty> property;
    };

    RSRenderMultiProcessManagerRepository() = default;
    ~RSRenderMultiProcessManagerRepository() noexcept = default;

    ProcessUniqueId RegisterNewProcess(GroupId groupId, pid_t pid,
        InitialScreenInfo screenInfo, std::promise<bool> promise);

    using DeathRecipientFactory = std::function<sptr<IRemoteObject::DeathRecipient>(ProcessUniqueId)>;
    bool SetRenderProcessReadyPromise(pid_t pid,
        const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
        const sptr<RSIConnectToRenderProcess>& connectToRenderConnection,
        const DeathRecipientFactory& deathRecipientFactory,
        const sptr<IRemoteObject>& binderObject);

    ScreenOutputList HandleRenderProcessDeath(ProcessUniqueId processUniqueId);

    void CheckAndHandleSubprocessDeathOverflow();

    std::optional<ProcessUniqueId> GetRenderProcessUniqueIdByGroupId(GroupId groupId) const;
    std::optional<ProcessUniqueId> GetValidRenderProcessUniqueIdByPid(pid_t pid) const;
    bool IsValidRenderProcessPid(pid_t pid) const;

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByGroupId(GroupId groupId) const;
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByUniqueId(ProcessUniqueId uid) const;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByUniqueId(ProcessUniqueId uid) const;
    sptr<RSIConnectToRenderProcess> GotConnectToRenderConnByUniqueId(ProcessUniqueId uid) const;
    sptr<IRSComposerToRenderConnection> GetComposerToRenderConnByUniqueId(ProcessUniqueId uid) const;
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const;

    void RecordComposerToRenderConnection(pid_t pid, sptr<IRSComposerToRenderConnection> conn);
    sptr<RSScreenProperty> GetPendingScreenProperty(pid_t pid);

    void AddScreenOutputToProcess(ProcessUniqueId uid, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output);
    void RemoveScreenOutputFromProcess(ProcessUniqueId uid, ScreenId screenId);

    ScreenId InsertVirtualToPhysicalScreenMap(ScreenId screenId, ScreenId associatedScreenId);
    std::optional<ScreenId> DeleteVirtualToPhysicalScreenMap(ScreenId screenId);
    ScreenId FindVirtualToPhysicalScreenMap(ScreenId screenId) const;

private:
    // Locked helpers (caller must hold mutex_)
    std::optional<ProcessUniqueId> GetRenderProcessUniqueIdByGroupIdLocked(GroupId groupId) const;
    std::optional<ProcessUniqueId> GetValidRenderProcessUniqueIdByPidLocked(pid_t pid) const;
    bool IsValidRenderProcessUniqueIdLocked(ProcessUniqueId uid) const;
    sptr<RSIServiceToRenderConnection> GetServiceToRenderConnByUniqueIdLocked(ProcessUniqueId uid) const;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnByUniqueIdLocked(ProcessUniqueId uid) const;
    sptr<IRSComposerToRenderConnection> GetComposerToRenderConnByUniqueIdLocked(ProcessUniqueId uid) const;
    void AddScreenOutputToProcessLocked(ProcessUniqueId uid, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output);
    void RemoveGroupIdByRenderProcessUniqueIdLocked(ProcessUniqueId uid);

    mutable std::mutex mutex_;
    mutable std::condition_variable validPidCv_;
    std::unordered_map<GroupId, ProcessUniqueId> groupIdToRenderProcessUniqueId_;
    std::unordered_map<ProcessUniqueId, std::promise<bool>, ProcessUniqueId::Hash> renderProcessReadyPromises_;
    std::unordered_map<ProcessUniqueId, sptr<IRSComposerToRenderConnection>, ProcessUniqueId::Hash>
        composerToRenderConnections_;
    std::unordered_map<ProcessUniqueId, sptr<RSIServiceToRenderConnection>, ProcessUniqueId::Hash>
        serviceToRenderConnections_;
    std::unordered_map<ProcessUniqueId, sptr<RSIConnectToRenderProcess>, ProcessUniqueId::Hash>
        connectToRenderConnections_;
    std::unordered_map<ProcessUniqueId, ScreenOutputList, ProcessUniqueId::Hash>
        processToScreenOutputMap_;
    std::unordered_map<ProcessUniqueId, sptr<IRemoteObject::DeathRecipient>, ProcessUniqueId::Hash>
        deathRecipients_;

    struct PendingScreenConnectInfo {
        ScreenId screenId = INVALID_SCREEN_ID;
        sptr<RSScreenProperty> property = nullptr;
    };
    std::unordered_map<ProcessUniqueId, PendingScreenConnectInfo, ProcessUniqueId::Hash>
        pendingScreenConnectInfos_;

    mutable std::mutex virtualScreenMutex_;
    std::map<ScreenId, ScreenId> virtualToPhysicalScreenMap_;

    std::deque<std::chrono::steady_clock::time_point> subprocessDeathTimes_;
    std::mutex subprocessDeathTimesMutex_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_REPOSITORY_H
