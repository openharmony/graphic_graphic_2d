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

#include "rs_render_multi_process_manager.h"

#include <csignal>
#include <fcntl.h>
#include <future>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rs_render_composer_manager.h"
#include "rs_render_service.h"

#undef LOG_TAG
#define LOG_TAG "RSMultiRenderProcessManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_RETRIES = 5;
constexpr int32_t DEFAULT_MAX_FD = 1024;
constexpr int32_t SUBPROCESS_DEATH_THRESHOLD_COUNT = 2;
constexpr int32_t SUBPROCESS_DEATH_TIME_WINDOW_SECS = 180;

int32_t ForkAndExec(GroupId groupId)
{
    pid_t pid = fork();
    RS_LOGW("%{public}s: Forked done %{public}d", __func__, pid);
    if (pid < 0) {
        RS_LOGE("%{public}s: Fork failed, errorno:%{public}d, errormsg:%{public}s", __func__, errno, strerror(errno));
        return -1;
    }
    if (pid == 0) {
        long maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd < 0) {
            maxfd = DEFAULT_MAX_FD;
        }
        for (int fd = 3; fd < maxfd; fd++) {
            close(fd);
        }
        RS_LOGW("%{public}s: Forked success self %{public}d, parent %{public}d", __func__, getpid(), getppid());
        if (prctl(PR_SET_PDEATHSIG, SIGKILL) == -1) {
            RS_LOGE("%{public}s: prctl(PR_SET_PDEATHSIG, SIGKILL), errorno:%{public}d, errormsg:%{public}s", __func__,
                errno, strerror(errno));
            _exit(-1);
        }
        std::string processName = "render_service:" + std::to_string(groupId);
        char* argv[] = { const_cast<char*>(processName.c_str()), nullptr };
        if (execv("/system/bin/render_process", argv) == -1) {
            RS_LOGE("%{public}s: Fork execv command failed, errorno:%{public}d, errormsg:%{public}s", __func__, errno,
                strerror(errno));
            _exit(-1);
        }
        return 0;
    } else {
        RS_LOGW("%{public}s: Forked success parent %{public}d, child %{public}d", __func__, getpid(), pid);
        return pid;
    }
}
} // namespace

RSMultiRenderProcessManager::RSMultiRenderProcessManager(RSRenderService& renderService)
    : RSRenderProcessManager(renderService), ipcPersistenceManager_(std::make_shared<RSIpcPersistenceManager>())
{
    renderProcessDeathCallback_ = [&screenManager = renderService_.screenManager_](
        std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>& screenOutputs) {
        if (screenManager != nullptr) {
            screenManager->OnProcessDisconnected(screenOutputs);
        }
    };
}

void RSMultiRenderProcessManager::RecordComposerToRenderConnection(
    pid_t pid, sptr<IRSComposerToRenderConnection> composerToRenderConnection)
{
    std::lock_guard<std::mutex> lock(mutex_);
    composerToRenderConnections_.insert_or_assign(pid, composerToRenderConnection);
}

std::optional<pid_t> RSMultiRenderProcessManager::GetRenderProcessPidByGroupId(GroupId groupId) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetRenderProcessPidByGroupIdLocked(groupId);
}

std::optional<pid_t> RSMultiRenderProcessManager::GetRenderProcessPidByGroupIdLocked(GroupId groupId) const
{
    // without lock version
    if (auto iter = groupIdToRenderProcessPid_.find(groupId); iter != groupIdToRenderProcessPid_.end()) {
        return iter->second;
    }
    return std::nullopt;
}

sptr<RSScreenProperty> RSMultiRenderProcessManager::GetPendingScreenProperty(pid_t pid)
{
    std::unique_lock<std::mutex> lock(renderProcessReadyPromiseMutex_);
    bool isFound = renderProcessReadyPromiseCv_.wait_for(lock, std::chrono::seconds(5),
        [this, pid]() { return renderProcessReadyPromises_.find(pid) != renderProcessReadyPromises_.end(); });
    if (!isFound) {
        RS_LOGE("%{public}s: Cannot find renderProcessReadyPromises in 5s for pid: %{public}u", __func__, pid);
        return nullptr;
    }

    const auto node = pendingScreenConnectInfos_.extract(pid);
    if (!node) {
        RS_LOGE("%{public}s: Cannot find pendingScreenConnectInfo for pid", __func__);
        return nullptr;
    }
    RS_LOGI("%{public}s: Found pendingScreenConnectInfo for pid: %{public}u", __func__, pid);
    return node.mapped().property;
}

bool RSMultiRenderProcessManager::SetRenderProcessReadyPromise(pid_t pid,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
    const sptr<RSIConnectToRenderProcess>& connectToRenderConnection)
{
    RS_LOGI("render_process %{public}u DeathRecipient registered", pid);
    RegisterDeathRecipient(pid, serviceToRenderConnection->AsObject());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        serviceToRenderConnections_.insert_or_assign(pid, serviceToRenderConnection);
        connectToRenderConnections_.insert_or_assign(pid, connectToRenderConnection);
    }

    {
        std::lock_guard<std::mutex> lock(renderProcessReadyPromiseMutex_);
        auto promiseIt = renderProcessReadyPromises_.find(pid);
        if (promiseIt != renderProcessReadyPromises_.end()) {
            promiseIt->second.set_value(true);
            renderProcessReadyPromises_.erase(promiseIt);
            return true;
        } else {
            RS_LOGE("%{public}s: promise not found for pid", __func__);
            return false;
        }
    }
}

GroupId RSMultiRenderProcessManager::GetGroupIdByScreenId(ScreenId screenId) const
{
    const auto& screenIdToGroupId = renderService_.renderModeConfig_->GetScreenIdToGroupId();
    auto iter = screenIdToGroupId.find(screenId);
    return (iter != screenIdToGroupId.end()) ? iter->second
                                             : renderService_.renderModeConfig_->GetDefaultRenderProcess();
}

std::optional<GroupId> RSMultiRenderProcessManager::CheckGroupIdByScreenId(ScreenId screenId) const
{
    const auto& screenIdToGroupId = renderService_.renderModeConfig_->GetScreenIdToGroupId();
    auto iter = screenIdToGroupId.find(screenId);
    return (iter != screenIdToGroupId.end()) ? std::make_optional<GroupId>(iter->second) : std::nullopt;
}

void RSMultiRenderProcessManager::UpdateGroupIdToRenderProcessPid(GroupId groupId, pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    groupIdToRenderProcessPid_.insert_or_assign(groupId, pid);
}

sptr<IRemoteObject> RSMultiRenderProcessManager::OnScreenConnected(
    ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    // Find the corresponding groupId for the screenId
    GroupId groupId = GetGroupIdByScreenId(screenId);
    auto optionalPid = GetRenderProcessPidByGroupId(groupId);
    auto renderConnProxy = optionalPid.has_value() ?
        HandleExistingGroup(optionalPid.value(), screenId, output, property) :
        HandleNewGroup(groupId, screenId, output, property);
    return renderConnProxy;
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleExistingGroup(
    pid_t pid, ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: GroupId has connected already, screenId is %{public}" PRIu64, __func__, screenId);
    auto serviceToRenderConnection = GetServiceToRenderConnByPid(pid);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return nullptr;
    }
    auto composerToRenderConn = GetComposerToRenderConnByPid(pid);
    if (!composerToRenderConn) {
        RS_LOGE("%{public}s: composerToRenderConn is nullptr", __func__);
        return nullptr;
    }

    AddScreenOutputToProcess(pid, screenId, output);
    auto renderToComposerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(screenId);
    if (!serviceToRenderConnection->NotifyScreenConnectInfoToRender(property,
        renderToComposerConn, composerToRenderConn)) {
        RS_LOGE("%{public}s: NotifyScreenConnectInfoToRender failed, screenId=%{public}" PRIu64, __func__, screenId);
        return nullptr;
    }

    auto connectToRender = GetConnectToRenderConnByPid(pid);
    if (!connectToRender) {
        RS_LOGE("%{public}s: connectToRender is nullptr", __func__);
        return nullptr;
    }
    return connectToRender->AsObject();
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleNewGroup(GroupId groupId, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: GroupId first time connect, screenId is %{public}" PRIu64, __func__, screenId);
    std::promise<bool> renderProcessReadyPromise;
    std::future<bool> renderProcessReadyFuture = renderProcessReadyPromise.get_future();

    // Fork Subprocess for the screenId
    int32_t pid = -1;
    for (int32_t retryCount = MAX_RETRIES; retryCount > 0; --retryCount) {
        pid = ForkAndExec(groupId);
        if (pid > 0) {
            UpdateGroupIdToRenderProcessPid(groupId, pid);
            AddScreenOutputToProcess(pid, screenId, output);
            PendingScreenConnectInfo screenConnectInfo { .screenId = screenId, .property = property };
            {
                std::lock_guard<std::mutex> lock(renderProcessReadyPromiseMutex_);
                pendingScreenConnectInfos_.insert_or_assign(pid, screenConnectInfo);
                renderProcessReadyPromises_[pid] = std::move(renderProcessReadyPromise);
                renderProcessReadyPromiseCv_.notify_one();
            }
            break;
        }
        RS_LOGE("%{public}s: Fork failed, retrying...", __func__);
        sleep(1);
    }
    if (pid <= 0) {
        RS_LOGE("%{public}s: Fork failed after %{public}d retries", __func__, MAX_RETRIES);
        exit(-1);
    }

    // Block and wait for the clientToRenderConnectionProxy to be ready
    renderProcessReadyFuture.wait();
    RS_LOGI("%{public}s: render process is ready", __func__);
    auto renderConnProxy = GotConnectToRenderConnByPid(pid)->AsObject();
    return renderConnProxy;
}

void RSMultiRenderProcessManager::OnScreenDisconnected(ScreenId screenId)
{
    auto serviceToRenderConnection = GetServiceToRenderConn(screenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    if (!serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId)) {
        RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64, __func__, screenId);
    }
}

void RSMultiRenderProcessManager::OnScreenPropertyChanged(
    ScreenId screenId, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
{
    if (!property) {
        RS_LOGE("%{public}s: property is null, screenId: %{public}" PRIu64, __func__, screenId);
        return;
    }
    ScreenId connScreenId = FindVirtualToPhysicalScreenMap(screenId);
    auto serviceToRenderConnection = GetServiceToRenderConn(connScreenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    if (!serviceToRenderConnection->NotifyScreenPropertyChangedInfoToRender(screenId, type, property)) {
        RS_LOGE("%{public}s: NotifyScreenPropertyChangedInfoToRender failed, screenId=%{public}" PRIu64,
            __func__, screenId);
    }
}

void RSMultiRenderProcessManager::OnScreenRefresh(ScreenId screenId)
{
    RS_LOGW("%{public}s: screenId: %{public}" PRIu64, __func__, screenId);
    auto serviceToRenderConnection = GetServiceToRenderConn(screenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    serviceToRenderConnection->NotifyScreenRefresh(screenId);
}

void RSMultiRenderProcessManager::OnVirtualScreenConnected(
    ScreenId screenId, ScreenId associatedScreenId, const sptr<RSScreenProperty>& property)
{
    auto serviceToRenderConnection = GetServiceToRenderConn(associatedScreenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr, associatedScreenId: %{public}" PRIu64, __func__,
            associatedScreenId);
        return;
    }
    ScreenId oldPhysicalScreenId = InsertVirtualToPhysicalScreenMap(screenId, associatedScreenId);
    if (oldPhysicalScreenId != INVALID_SCREEN_ID) {
        if (auto oldScreenServiceToRenderConn = GetServiceToRenderConn(oldPhysicalScreenId)) {
            if (!oldScreenServiceToRenderConn->NotifyScreenDisconnectInfoToRender(screenId)) {
                RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64,
                    __func__, screenId);
            }
        }
    }
    if (!serviceToRenderConnection->NotifyScreenConnectInfoToRender(property, nullptr, nullptr)) {
        RS_LOGE("%{public}s: NotifyScreenConnectInfoToRender failed, screenId=%{public}" PRIu64,
            __func__, screenId);
    }
    auto groupId = GetGroupIdByScreenId(associatedScreenId);
    auto optionalPid = GetRenderProcessPidByGroupId(groupId);
    if (optionalPid.has_value()) {
        pid_t pid = optionalPid.value();
        AddScreenOutputToProcess(pid, screenId, nullptr);
        RS_LOGI("%{public}s: Added virtualScreenId %{public}" PRIu64 " to render_process %{public}u",
            __func__, screenId, pid);
    } else {
        RS_LOGW("%{public}s: Cannot find pid for associatedScreenId %{public}" PRIu64,
            __func__, associatedScreenId);
    }
}

void RSMultiRenderProcessManager::OnVirtualScreenDisconnected(ScreenId screenId)
{
    auto optionalConnScreenId = DeleteVirtualToPhysicalScreenMap(screenId);
    if (!optionalConnScreenId.has_value()) {
        RS_LOGW("%{public}s: Cannot find physicalScreenId for virtualScreenId %{public}" PRIu64,
            __func__, screenId);
        return;
    }

    ScreenId associatedScreenId = optionalConnScreenId.value();
    auto serviceToRenderConnection = GetServiceToRenderConn(associatedScreenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    if (!serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId)) {
        RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64,
            __func__, screenId);
    }

    auto groupId = GetGroupIdByScreenId(associatedScreenId);
    auto optionalPid = GetRenderProcessPidByGroupId(groupId);
    if (optionalPid.has_value()) {
        pid_t pid = optionalPid.value();
        RemoveScreenOutputFromProcess(pid, screenId);
        RS_LOGI("%{public}s: Removed virtualScreenId %{public}" PRIu64 " from render_process %{public}u",
            __func__, screenId, pid);
    }
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConn(ScreenId screenId) const
{
    auto groupId = GetGroupIdByScreenId(screenId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto optionalPid = GetRenderProcessPidByGroupIdLocked(groupId);
    if (!optionalPid.has_value()) {
        RS_LOGE("%{public}s: get render process pid failed", __func__);
        return nullptr;
    }
    return GetServiceToRenderConnByPidLocked(optionalPid.value());
}

std::vector<sptr<RSIServiceToRenderConnection>> RSMultiRenderProcessManager::GetServiceToRenderConns() const
{
    std::vector<sptr<RSIServiceToRenderConnection>> connections;
    std::lock_guard<std::mutex> lock(mutex_);
    connections.reserve(serviceToRenderConnections_.size());
    for (const auto& [_, conn] : serviceToRenderConnections_) {
        connections.emplace_back(conn);
    }
    return connections;
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnection(ScreenId screenId) const
{
    auto groupId = GetGroupIdByScreenId(screenId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto optionalPid = GetRenderProcessPidByGroupIdLocked(groupId);
    if (!optionalPid.has_value()) {
        RS_LOGE("%{public}s: get render process pid failed", __func__);
        return nullptr;
    }
    return GetConnectToRenderConnByPidLocked(optionalPid.value());
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConnByPid(pid_t pid) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetServiceToRenderConnByPidLocked(pid);
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GotServiceToRenderConnByPid(pid_t pid) const
{
    // Must success, or gonna throw an exception if failed
    std::lock_guard<std::mutex> lock(mutex_);
    return serviceToRenderConnections_.at(pid);
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConnByPidLocked(pid_t pid) const
{
    // without lock version
    if (auto iter = serviceToRenderConnections_.find(pid); iter != serviceToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnByPid(pid_t pid) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetConnectToRenderConnByPidLocked(pid);
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GotConnectToRenderConnByPid(pid_t pid) const
{
    // Must success, or gonna throw an exception if failed
    std::lock_guard<std::mutex> lock(mutex_);
    return connectToRenderConnections_.at(pid);
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnByPidLocked(pid_t pid) const
{
    // without lock version
    if (auto iter = connectToRenderConnections_.find(pid); iter != connectToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GotComposerToRenderConnByPid(pid_t pid) const
{
    // Must success, or gonna throw an exception if failed
    std::lock_guard<std::mutex> lock(mutex_);
    return composerToRenderConnections_.at(pid);
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GetComposerToRenderConnByPid(pid_t pid) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetComposerToRenderConnByPidLocked(pid);
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GetComposerToRenderConnByPidLocked(pid_t pid) const
{
    // without lock version
    if (auto iter = composerToRenderConnections_.find(pid); iter != composerToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

ScreenId RSMultiRenderProcessManager::InsertVirtualToPhysicalScreenMap(ScreenId screenId, ScreenId associatedScreenId)
{
    ScreenId oldPhysicalScreenId = INVALID_SCREEN_ID;
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto iter = virtualToPhysicalScreenMap_.find(screenId); iter != virtualToPhysicalScreenMap_.end()) {
        if (iter->second != associatedScreenId) {
            oldPhysicalScreenId = iter->second;
            iter->second = associatedScreenId;
        }
    } else {
        virtualToPhysicalScreenMap_.emplace(screenId, associatedScreenId);
    }
    return oldPhysicalScreenId;
}

std::optional<ScreenId> RSMultiRenderProcessManager::DeleteVirtualToPhysicalScreenMap(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto node = virtualToPhysicalScreenMap_.extract(screenId)) {
        return node.mapped();
    }
    return std::nullopt;
}

ScreenId RSMultiRenderProcessManager::FindVirtualToPhysicalScreenMap(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(virtualScreenMutex_);
    if (auto iter = virtualToPhysicalScreenMap_.find(screenId); iter != virtualToPhysicalScreenMap_.end()) {
        return iter->second;
    }
    return screenId;
}

std::shared_ptr<RSIpcPersistenceManager> RSMultiRenderProcessManager::GetIpcPersistenceManager() const
{
    return ipcPersistenceManager_;
}

void RSMultiRenderProcessManager::RenderProcessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("%{public}s: can't promote remote object", __func__);
        return;
    }

    auto managerSptr = manager_.promote();
    if (managerSptr == nullptr) {
        RS_LOGW("%{public}s: manager has been destroyed, skip death handling for pid %{public}u",
            __func__, pid_);
        return;
    }
    
    RS_LOGE("%{public}s: render_process died, DeathRecipient triggered for pid %{public}u",
        __func__, pid_);
    managerSptr->HandleRenderProcessDeath(pid_);
}

void RSMultiRenderProcessManager::HandleRenderProcessDeath(pid_t pid)
{
    RS_LOGI("Handling render_process %{public}u death", pid);
    CheckAndHandleSubprocessDeathOverflow();
    std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>> affectedScreenOutputs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        serviceToRenderConnections_.erase(pid);
        connectToRenderConnections_.erase(pid);
        composerToRenderConnections_.erase(pid);

        auto node = pidToScreenOutputMap_.extract(pid);
        if (node) {
            affectedScreenOutputs = std::move(node.mapped());
        }

        auto groupIter = std::find_if(groupIdToRenderProcessPid_.begin(), groupIdToRenderProcessPid_.end(),
            [pid](const auto& pair) { return pair.second == pid; });
        if (groupIter != groupIdToRenderProcessPid_.end()) {
            groupIdToRenderProcessPid_.erase(groupIter);
        }
    }
    {
        std::lock_guard<std::mutex> lock(renderProcessReadyPromiseMutex_);
        pendingScreenConnectInfos_.erase(pid);
        auto promiseIt = renderProcessReadyPromises_.find(pid);
        if (promiseIt != renderProcessReadyPromises_.end()) {
            promiseIt->second.set_value(false);
            renderProcessReadyPromises_.erase(promiseIt);
        }
    }
    RS_LOGI("%{public}s: ProcessManager executing screen disconnection for pid=%{public}u, screenIds count=%{public}zu",
        __func__, pid, affectedScreenOutputs.size());
    for (const auto& [screenId, output] : affectedScreenOutputs) {
        RS_LOGI("%{public}s: ScreenId=%{public}" PRIu64 " disconnected due to process death", __func__, screenId);
    }
    if (renderProcessDeathCallback_ && !affectedScreenOutputs.empty()) {
        renderProcessDeathCallback_(affectedScreenOutputs);
    }
    UnregisterDeathRecipient(pid);
}

void RSMultiRenderProcessManager::RegisterDeathRecipient(pid_t pid, const sptr<IRemoteObject>& binderObject)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (deathRecipients_.find(pid) != deathRecipients_.end()) {
        RS_LOGW("%{public}s: DeathRecipient already registered for pid %{public}u, replacing.", __func__, pid);
    }

    auto deathRecipient = sptr<RenderProcessDeathRecipient>::MakeSptr(pid, wptr<RSMultiRenderProcessManager>(this));
    if (!binderObject->AddDeathRecipient(deathRecipient)) {
        RS_LOGE("%{public}s: Failed to add DeathRecipient for render_process %{public}u", __func__, pid);
        return;
    }

    deathRecipients_[pid] = deathRecipient;
    RS_LOGI("%{public}s: Registered DeathRecipient for render_process %{public}u", __func__, pid);
}

void RSMultiRenderProcessManager::UnregisterDeathRecipient(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = deathRecipients_.find(pid);
    if (it != deathRecipients_.end()) {
        deathRecipients_.erase(it);
        RS_LOGI("%{public}s: Unregistered DeathRecipient for render_process %{public}u", __func__, pid);
    }
}

void RSMultiRenderProcessManager::AddScreenOutputToProcess(pid_t pid, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_LOGI("Added screenId %{public}" PRIu64 " to render_process %{public}u", screenId, pid);
    pidToScreenOutputMap_[pid].emplace_back(screenId, output);
}

void RSMultiRenderProcessManager::RemoveScreenOutputFromProcess(pid_t pid, ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pidToScreenOutputMap_.find(pid);
    if (it == pidToScreenOutputMap_.end()) {
        RS_LOGW("%{public}s: Cannot find pid %{public}u in pidToScreenOutputMap", __func__, pid);
        return;
    }
    
    auto& screenOutputs = it->second;
    auto outputIt = std::find_if(screenOutputs.begin(), screenOutputs.end(), [screenId](const auto& pair) {
        return pair.first == screenId;
    });
    if (outputIt != screenOutputs.end()) {
        screenOutputs.erase(outputIt);
        RS_LOGI("Removed screenId %{public}" PRIu64 " from render_process %{public}u", screenId, pid);
        if (screenOutputs.empty()) {
            pidToScreenOutputMap_.erase(it);
        }
    } else {
        RS_LOGW("%{public}s: Cannot find screenId %{public}" PRIu64 " in pid %{public}u",
            __func__, screenId, pid);
    }
}

void RSMultiRenderProcessManager::CheckAndHandleSubprocessDeathOverflow()
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
            " waiting render_service", __func__, SUBPROCESS_DEATH_THRESHOLD_COUNT, SUBPROCESS_DEATH_TIME_WINDOW_SECS);
        exit(-1);
    }
}
} // namespace Rosen
} // namespace OHOS
