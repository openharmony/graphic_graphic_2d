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

#include "rs_render_multi_process_manager_repository.h"

#include <algorithm>
#include <chrono>

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderMultiProcessManagerRepository"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SUBPROCESS_DEATH_THRESHOLD_COUNT = 2;
constexpr int32_t SUBPROCESS_DEATH_TIME_WINDOW_SECS = 180;
} // namespace

ProcessUniqueId RSRenderMultiProcessManagerRepository::RegisterNewProcess(GroupId groupId, pid_t pid,
    InitialScreenInfo screenInfo, std::promise<bool> promise)
{
    ProcessUniqueId processUniqueId(pid);
    PendingScreenConnectInfo screenConnectInfo { .screenId = screenInfo.screenId,
        .property = screenInfo.property };
    {
        std::lock_guard<std::mutex> lock(mutex_);
        groupIdToRenderProcessUniqueId_.insert_or_assign(groupId, processUniqueId);
        AddScreenOutputToProcessLocked(processUniqueId, screenInfo.screenId, screenInfo.output);
        pendingScreenConnectInfos_.insert_or_assign(processUniqueId, screenConnectInfo);
        renderProcessReadyPromises_[processUniqueId] = std::move(promise);
    }
    validPidCv_.notify_all();
    return processUniqueId;
}

bool RSRenderMultiProcessManagerRepository::SetRenderProcessReadyPromise(pid_t pid,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
    const sptr<RSIConnectToRenderProcess>& connectToRenderConnection,
    const DeathRecipientFactory& deathRecipientFactory,
    const sptr<IRemoteObject>& binderObject)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto processUniqueIdOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!processUniqueIdOpt.has_value()) {
        RS_LOGE("%{public}s: processUniqueId not found for pid %{public}u", __func__, pid);
        return false;
    }
    ProcessUniqueId processUniqueId = processUniqueIdOpt.value();

    if (deathRecipientFactory && binderObject) {
        auto deathRecipient = deathRecipientFactory(processUniqueId);
        if (deathRecipients_.find(processUniqueId) != deathRecipients_.end()) {
            RS_LOGW("%{public}s: DeathRecipient already registered for uid(pid=%{public}u), replacing.",
                __func__, processUniqueId.GetPid());
        }
        if (!binderObject->AddDeathRecipient(deathRecipient)) {
            RS_LOGE("%{public}s: Failed to add DeathRecipient for render_process %{public}u",
                __func__, processUniqueId.GetPid());
        } else {
            deathRecipients_[processUniqueId] = deathRecipient;
            RS_LOGI("%{public}s: Registered DeathRecipient for render_process %{public}u",
                __func__, processUniqueId.GetPid());
        }
    }

    serviceToRenderConnections_.insert_or_assign(processUniqueId, serviceToRenderConnection);
    connectToRenderConnections_.insert_or_assign(processUniqueId, connectToRenderConnection);

    auto promiseIt = renderProcessReadyPromises_.find(processUniqueId);
    if (promiseIt != renderProcessReadyPromises_.end()) {
        promiseIt->second.set_value(true);
        renderProcessReadyPromises_.erase(promiseIt);
        return true;
    } else {
        RS_LOGE("%{public}s: promise not found for pid", __func__);
        return false;
    }
}

RSRenderMultiProcessManagerRepository::ScreenOutputList RSRenderMultiProcessManagerRepository::HandleRenderProcessDeath(
    ProcessUniqueId processUniqueId)
{
    ScreenOutputList affectedScreenOutputs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        RemoveGroupIdByRenderProcessUniqueIdLocked(processUniqueId);

        auto node = processToScreenOutputMap_.extract(processUniqueId);
        if (node) {
            affectedScreenOutputs = std::move(node.mapped());
        }

        serviceToRenderConnections_.erase(processUniqueId);
        connectToRenderConnections_.erase(processUniqueId);
        composerToRenderConnections_.erase(processUniqueId);
        pendingScreenConnectInfos_.erase(processUniqueId);
        deathRecipients_.erase(processUniqueId);
        auto promiseIt = renderProcessReadyPromises_.find(processUniqueId);
        if (promiseIt != renderProcessReadyPromises_.end()) {
            promiseIt->second.set_value(false);
            renderProcessReadyPromises_.erase(promiseIt);
        }
    }
    return affectedScreenOutputs;
}

void RSRenderMultiProcessManagerRepository::CheckAndHandleSubprocessDeathOverflow()
{
    std::lock_guard<std::mutex> lock(subprocessDeathTimesMutex_);
    auto now = std::chrono::steady_clock::now();
    auto windowStart = now - std::chrono::seconds(SUBPROCESS_DEATH_TIME_WINDOW_SECS);

    while (!subprocessDeathTimes_.empty() && subprocessDeathTimes_.front() < windowStart) {
        subprocessDeathTimes_.pop_front();
    }
    subprocessDeathTimes_.push_back(now);

    if (static_cast<int32_t>(subprocessDeathTimes_.size()) > SUBPROCESS_DEATH_THRESHOLD_COUNT) {
        RS_LOGE("%{public}s Subprocess died more than %{public}d times in %{public}d seconds,"
            " waiting render_service", __func__, SUBPROCESS_DEATH_THRESHOLD_COUNT,
            SUBPROCESS_DEATH_TIME_WINDOW_SECS);
        _exit(-1);
    }
}

std::optional<ProcessUniqueId> RSRenderMultiProcessManagerRepository::GetRenderProcessUniqueIdByGroupId(
    GroupId groupId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetRenderProcessUniqueIdByGroupIdLocked(groupId);
}

std::optional<ProcessUniqueId> RSRenderMultiProcessManagerRepository::GetValidRenderProcessUniqueIdByPid(
    pid_t pid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetValidRenderProcessUniqueIdByPidLocked(pid);
}

bool RSRenderMultiProcessManagerRepository::IsValidRenderProcessPid(pid_t pid) const
{
    std::unique_lock<std::mutex> lock(mutex_);
    bool result = validPidCv_.wait_for(lock, std::chrono::seconds(5),
        [this, pid]() {
            return std::any_of(processToScreenOutputMap_.begin(), processToScreenOutputMap_.end(),
                [pid](const auto& pair) { return pair.first.GetPid() == pid; });
        });
    if (!result) {
        RS_LOGE("%{public}s: pid %{public}u not found in processToScreenOutputMap after 5s",
            __func__, pid);
    }
    return result;
}

sptr<RSIServiceToRenderConnection> RSRenderMultiProcessManagerRepository::GetServiceToRenderConnByGroupId(
    GroupId groupId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto processUniqueIdOpt = GetRenderProcessUniqueIdByGroupIdLocked(groupId);
    if (!processUniqueIdOpt.has_value()) {
        RS_LOGE("%{public}s: get render process processUniqueId failed", __func__);
        return nullptr;
    }
    return GetServiceToRenderConnByUniqueIdLocked(processUniqueIdOpt.value());
}

sptr<RSIServiceToRenderConnection> RSRenderMultiProcessManagerRepository::GetServiceToRenderConnByUniqueId(
    ProcessUniqueId uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetServiceToRenderConnByUniqueIdLocked(uid);
}

sptr<RSIConnectToRenderProcess> RSRenderMultiProcessManagerRepository::GetConnectToRenderConnByUniqueId(
    ProcessUniqueId uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetConnectToRenderConnByUniqueIdLocked(uid);
}

sptr<RSIConnectToRenderProcess> RSRenderMultiProcessManagerRepository::GotConnectToRenderConnByUniqueId(
    ProcessUniqueId uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsValidRenderProcessUniqueIdLocked(uid)) {
        RS_LOGE("%{public}s: processUniqueId invalidated by death cleanup", __func__);
        return nullptr;
    }
    return connectToRenderConnections_.at(uid);
}

sptr<IRSComposerToRenderConnection> RSRenderMultiProcessManagerRepository::GetComposerToRenderConnByUniqueId(
    ProcessUniqueId uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetComposerToRenderConnByUniqueIdLocked(uid);
}

std::vector<sptr<RSIServiceToRenderConnection>> RSRenderMultiProcessManagerRepository::GetServiceToRenderConns() const
{
    std::vector<sptr<RSIServiceToRenderConnection>> connections;
    std::lock_guard<std::mutex> lock(mutex_);
    connections.reserve(serviceToRenderConnections_.size());
    for (const auto& [_, conn] : serviceToRenderConnections_) {
        connections.emplace_back(conn);
    }
    return connections;
}

void RSRenderMultiProcessManagerRepository::RecordComposerToRenderConnection(pid_t pid,
    sptr<IRSComposerToRenderConnection> conn)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto processUniqueIdOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!processUniqueIdOpt.has_value()) {
        RS_LOGE("%{public}s: processUniqueId not found for pid %{public}u", __func__, pid);
        return;
    }
    composerToRenderConnections_.insert_or_assign(processUniqueIdOpt.value(), conn);
}

sptr<RSScreenProperty> RSRenderMultiProcessManagerRepository::GetPendingScreenProperty(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto processUniqueIdOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!processUniqueIdOpt.has_value()) {
        RS_LOGE("%{public}s: processUniqueId not found for pid %{public}u", __func__, pid);
        return nullptr;
    }
    ProcessUniqueId processUniqueId = processUniqueIdOpt.value();

    const auto node = pendingScreenConnectInfos_.extract(processUniqueId);
    if (!node) {
        RS_LOGE("%{public}s: Cannot find pendingScreenConnectInfo for processUniqueId", __func__);
        return nullptr;
    }
    RS_LOGI("%{public}s: Found pendingScreenConnectInfo for pid: %{public}u", __func__, pid);
    return node.mapped().property;
}

void RSRenderMultiProcessManagerRepository::AddScreenOutputToProcess(ProcessUniqueId uid, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output)
{
    std::lock_guard<std::mutex> lock(mutex_);
    AddScreenOutputToProcessLocked(uid, screenId, output);
}

void RSRenderMultiProcessManagerRepository::RemoveScreenOutputFromProcess(ProcessUniqueId uid, ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = processToScreenOutputMap_.find(uid);
    if (it == processToScreenOutputMap_.end()) {
        RS_LOGW("%{public}s: Cannot find processUniqueId in processToScreenOutputMap", __func__);
        return;
    }

    auto& screenOutputs = it->second;
    auto outputIt = std::find_if(screenOutputs.begin(), screenOutputs.end(),
        [screenId](const auto& pair) { return pair.first == screenId; });
    if (outputIt != screenOutputs.end()) {
        screenOutputs.erase(outputIt);
        RS_LOGI("Removed screenId %{public}" PRIu64 " from render_process %{public}u",
            screenId, uid.GetPid());
        if (screenOutputs.empty()) {
            processToScreenOutputMap_.erase(it);
        }
    } else {
        RS_LOGW("%{public}s: Cannot find screenId %{public}" PRIu64, __func__, screenId);
    }
}

ScreenId RSRenderMultiProcessManagerRepository::InsertVirtualToPhysicalScreenMap(
    ScreenId screenId, ScreenId associatedScreenId)
{
    ScreenId oldPhysicalScreenId = INVALID_SCREEN_ID;
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto iter = virtualToPhysicalScreenMap_.find(screenId);
        iter != virtualToPhysicalScreenMap_.end()) {
        if (iter->second != associatedScreenId) {
            oldPhysicalScreenId = iter->second;
            iter->second = associatedScreenId;
        }
    } else {
        virtualToPhysicalScreenMap_.emplace(screenId, associatedScreenId);
    }
    return oldPhysicalScreenId;
}

std::optional<ScreenId> RSRenderMultiProcessManagerRepository::DeleteVirtualToPhysicalScreenMap(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto node = virtualToPhysicalScreenMap_.extract(screenId)) {
        return node.mapped();
    }
    return std::nullopt;
}

ScreenId RSRenderMultiProcessManagerRepository::FindVirtualToPhysicalScreenMap(ScreenId screenId) const
{
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto iter = virtualToPhysicalScreenMap_.find(screenId);
        iter != virtualToPhysicalScreenMap_.end()) {
        return iter->second;
    }
    return screenId;
}

// === Private locked helpers ===

std::optional<ProcessUniqueId> RSRenderMultiProcessManagerRepository::GetRenderProcessUniqueIdByGroupIdLocked(
    GroupId groupId) const
{
    if (auto iter = groupIdToRenderProcessUniqueId_.find(groupId);
        iter != groupIdToRenderProcessUniqueId_.end()) {
        return iter->second;
    }
    return std::nullopt;
}

std::optional<ProcessUniqueId> RSRenderMultiProcessManagerRepository::GetValidRenderProcessUniqueIdByPidLocked(
    pid_t pid) const
{
    for (const auto& [processUniqueId, _] : processToScreenOutputMap_) {
        if (processUniqueId.GetPid() == pid) {
            return processUniqueId;
        }
    }
    return std::nullopt;
}

bool RSRenderMultiProcessManagerRepository::IsValidRenderProcessUniqueIdLocked(ProcessUniqueId uid) const
{
    return processToScreenOutputMap_.find(uid) != processToScreenOutputMap_.end();
}

sptr<RSIServiceToRenderConnection> RSRenderMultiProcessManagerRepository::GetServiceToRenderConnByUniqueIdLocked(
    ProcessUniqueId uid) const
{
    if (auto iter = serviceToRenderConnections_.find(uid); iter != serviceToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<RSIConnectToRenderProcess> RSRenderMultiProcessManagerRepository::GetConnectToRenderConnByUniqueIdLocked(
    ProcessUniqueId uid) const
{
    if (auto iter = connectToRenderConnections_.find(uid); iter != connectToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<IRSComposerToRenderConnection> RSRenderMultiProcessManagerRepository::GetComposerToRenderConnByUniqueIdLocked(
    ProcessUniqueId uid) const
{
    if (auto iter = composerToRenderConnections_.find(uid); iter != composerToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

void RSRenderMultiProcessManagerRepository::AddScreenOutputToProcessLocked(ProcessUniqueId uid, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output)
{
    auto& screenOutputs = processToScreenOutputMap_[uid];
    auto existing = std::find_if(screenOutputs.begin(), screenOutputs.end(),
        [screenId](const auto& pair) { return pair.first == screenId; });
    if (existing != screenOutputs.end()) {
        existing->second = output;
    } else {
        screenOutputs.emplace_back(screenId, output);
    }
    RS_LOGI("AddScreenOutputToProcess screenId %{public}" PRIu64 " pid %{public}u replaced=%{public}d",
        screenId, uid.GetPid(), existing != screenOutputs.end());
}

void RSRenderMultiProcessManagerRepository::RemoveGroupIdByRenderProcessUniqueIdLocked(ProcessUniqueId uid)
{
    for (auto iter = groupIdToRenderProcessUniqueId_.begin();
        iter != groupIdToRenderProcessUniqueId_.end();) {
        if (iter->second == uid) {
            iter = groupIdToRenderProcessUniqueId_.erase(iter);
        } else {
            ++iter;
        }
    }
}

} // namespace Rosen
} // namespace OHOS
