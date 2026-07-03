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

uint64_t GetProcessUniqueIdTimeStamp()
{
    return std::chrono::steady_clock::now().time_since_epoch().count();
}

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
    auto tokenOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!tokenOpt.has_value()) {
        RS_LOGE("%{public}s: token not found for pid %{public}u", __func__, pid);
        return;
    }
    composerToRenderConnections_.insert_or_assign(tokenOpt.value(), composerToRenderConnection);
}

std::optional<ProcessUniqueId> RSMultiRenderProcessManager::GetRenderProcessUniqueIdByGroupId(GroupId groupId) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetRenderProcessUniqueIdByGroupIdLocked(groupId);
}

std::optional<ProcessUniqueId> RSMultiRenderProcessManager::GetRenderProcessUniqueIdByGroupIdLocked(
    GroupId groupId) const
{
    // without lock version
    if (auto iter = groupIdToRenderProcessUniqueId_.find(groupId); iter != groupIdToRenderProcessUniqueId_.end()) {
        return iter->second;
    }
    return std::nullopt;
}

void RSMultiRenderProcessManager::UpdateGroupIdToRenderProcessUniqueId(GroupId groupId, ProcessUniqueId token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    UpdateGroupIdToRenderProcessUniqueIdLocked(groupId, token);
}

void RSMultiRenderProcessManager::UpdateGroupIdToRenderProcessUniqueIdLocked(GroupId groupId, ProcessUniqueId token)
{
    groupIdToRenderProcessUniqueId_.insert_or_assign(groupId, token);
}

void RSMultiRenderProcessManager::RemoveGroupIdByRenderProcessUniqueId(ProcessUniqueId token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RemoveGroupIdByRenderProcessUniqueIdLocked(token);
}

void RSMultiRenderProcessManager::RemoveGroupIdByRenderProcessUniqueIdLocked(ProcessUniqueId token)
{
    for (auto iter = groupIdToRenderProcessUniqueId_.begin(); iter != groupIdToRenderProcessUniqueId_.end();) {
        if (iter->second == token) {
            iter = groupIdToRenderProcessUniqueId_.erase(iter);
        } else {
            ++iter;
        }
    }
}

std::optional<ProcessUniqueId> RSMultiRenderProcessManager::GetValidRenderProcessUniqueIdByPid(pid_t pid) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetValidRenderProcessUniqueIdByPidLocked(pid);
}

std::optional<ProcessUniqueId> RSMultiRenderProcessManager::GetValidRenderProcessUniqueIdByPidLocked(pid_t pid) const
{
    // without lock version
    for (const auto& token : validRenderProcessUniqueIds_) {
        if (token.GetPid() == pid) {
            return token;
        }
    }
    return std::nullopt;
}

sptr<RSScreenProperty> RSMultiRenderProcessManager::GetPendingScreenProperty(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto tokenOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!tokenOpt.has_value()) {
        RS_LOGE("%{public}s: token not found for pid %{public}u", __func__, pid);
        return nullptr;
    }
    ProcessUniqueId token = tokenOpt.value();

    const auto node = pendingScreenConnectInfos_.extract(token);
    if (!node) {
        RS_LOGE("%{public}s: Cannot find pendingScreenConnectInfo for token", __func__);
        return nullptr;
    }
    RS_LOGI("%{public}s: Found pendingScreenConnectInfo for pid: %{public}u, timeStamp: %{public}" PRIu64,
        __func__, pid, token.GetTimestamp());
    return node.mapped().property;
}

bool RSMultiRenderProcessManager::SetRenderProcessReadyPromise(pid_t pid,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
    const sptr<RSIConnectToRenderProcess>& connectToRenderConnection)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto tokenOpt = GetValidRenderProcessUniqueIdByPidLocked(pid);
    if (!tokenOpt.has_value()) {
        RS_LOGE("%{public}s: token not found for pid %{public}u", __func__, pid);
        return false;
    }
    ProcessUniqueId token = tokenOpt.value();

    RegisterDeathRecipientLocked(token, serviceToRenderConnection->AsObject());
    serviceToRenderConnections_.insert_or_assign(token, serviceToRenderConnection);
    connectToRenderConnections_.insert_or_assign(token, connectToRenderConnection);

    auto promiseIt = renderProcessReadyPromises_.find(token);
    if (promiseIt != renderProcessReadyPromises_.end()) {
        promiseIt->second.set_value(true);
        renderProcessReadyPromises_.erase(promiseIt);
        return true;
    } else {
        RS_LOGE("%{public}s: promise not found for pid", __func__);
        return false;
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

bool RSMultiRenderProcessManager::IsValidRenderProcessPid(pid_t pid) const
{
    std::unique_lock<std::mutex> lock(mutex_);
    bool result = validPidCv_.wait_for(lock, std::chrono::seconds(5),
        [this, pid]() {
            return std::any_of(validRenderProcessUniqueIds_.begin(), validRenderProcessUniqueIds_.end(),
                [pid](const ProcessUniqueId& t) { return t.GetPid() == pid; });
        });
    if (!result) {
        RS_LOGE("%{public}s: pid %{public}u not found in validRenderProcessUniqueIds after 5s", __func__, pid);
    }
    return result;
}

sptr<IRemoteObject> RSMultiRenderProcessManager::OnScreenConnected(
    ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    // Find the corresponding groupId for the screenId
    GroupId groupId = GetGroupIdByScreenId(screenId);
    auto optionalToken = GetRenderProcessUniqueIdByGroupId(groupId);
    auto renderConnProxy = optionalToken.has_value() ?
        HandleExistingGroup(optionalToken.value(), screenId, output, property) :
        HandleNewGroup(groupId, screenId, output, property);
    return renderConnProxy;
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleExistingGroup(ProcessUniqueId token, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: GroupId has connected already, screenId is %{public}" PRIu64, __func__, screenId);
    auto serviceToRenderConnection = GetServiceToRenderConnByUniqueId(token);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return nullptr;
    }
    auto composerToRenderConn = GetComposerToRenderConnByUniqueId(token);
    if (!composerToRenderConn) {
        RS_LOGE("%{public}s: composerToRenderConn is nullptr", __func__);
        return nullptr;
    }

    AddScreenOutputToProcess(token, screenId, output);
    auto renderToComposerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(screenId);
    if (!serviceToRenderConnection->NotifyScreenConnectInfoToRender(property,
        renderToComposerConn, composerToRenderConn)) {
        RS_LOGE("%{public}s: NotifyScreenConnectInfoToRender failed, screenId=%{public}" PRIu64, __func__, screenId);
        return nullptr;
    }

    auto connectToRender = GetConnectToRenderConnByUniqueId(token);
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
    uint64_t timeStamp = GetProcessUniqueIdTimeStamp();
    for (int32_t retryCount = MAX_RETRIES; retryCount > 0; --retryCount) {
        pid = ForkAndExec(groupId);
        if (pid > 0) {
            ProcessUniqueId token = {pid, timeStamp};
            PendingScreenConnectInfo screenConnectInfo { .screenId = screenId, .property = property };
            {
                std::lock_guard<std::mutex> lock(mutex_);
                UpdateGroupIdToRenderProcessUniqueIdLocked(groupId, token);
                AddValidRenderProcessUniqueIdLocked(token);
                AddScreenOutputToProcessLocked(token, screenId, output);
                pendingScreenConnectInfos_.insert_or_assign(token, screenConnectInfo);
                renderProcessReadyPromises_[token] = std::move(renderProcessReadyPromise);
            }
            validPidCv_.notify_all();
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
    auto renderConnProxy = GotConnectToRenderConnByUniqueId(ProcessUniqueId{pid, timeStamp});
    if (!renderConnProxy) {
        RS_LOGE("%{public}s: token invalidated by death cleanup", __func__);
        return nullptr;
    }
    return renderConnProxy->AsObject();
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
    auto optionalToken = GetRenderProcessUniqueIdByGroupId(groupId);
    if (optionalToken.has_value()) {
        AddScreenOutputToProcess(optionalToken.value(), screenId, nullptr);
        RS_LOGI("%{public}s: Added virtualScreenId %{public}" PRIu64 " to render_process %{public}u",
            __func__, screenId, optionalToken.value().GetPid());
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
    auto optionalToken = GetRenderProcessUniqueIdByGroupId(groupId);
    if (optionalToken.has_value()) {
        RemoveScreenOutputFromProcess(optionalToken.value(), screenId);
        RS_LOGI("%{public}s: Removed virtualScreenId %{public}" PRIu64 " from render_process %{public}u",
            __func__, screenId, optionalToken.value().GetPid());
    }
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConn(ScreenId screenId) const
{
    auto groupId = GetGroupIdByScreenId(screenId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto optionalToken = GetRenderProcessUniqueIdByGroupIdLocked(groupId);
    if (!optionalToken.has_value()) {
        RS_LOGE("%{public}s: get render process token failed", __func__);
        return nullptr;
    }
    return GetServiceToRenderConnByUniqueIdLocked(optionalToken.value());
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
    auto optionalToken = GetRenderProcessUniqueIdByGroupIdLocked(groupId);
    if (!optionalToken.has_value()) {
        RS_LOGE("%{public}s: get render process token failed", __func__);
        return nullptr;
    }
    return GetConnectToRenderConnByUniqueIdLocked(optionalToken.value());
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetServiceToRenderConnByUniqueIdLocked(token);
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GotServiceToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // Must success, or gonna throw an exception if failed. Caller must ensure token is valid.
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsValidRenderProcessUniqueIdLocked(token)) {
        RS_LOGE("%{public}s: token invalidated by death cleanup", __func__);
        return nullptr;
    }
    return serviceToRenderConnections_.at(token);
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConnByUniqueIdLocked(
    ProcessUniqueId token) const
{
    // without lock version
    if (auto iter = serviceToRenderConnections_.find(token); iter != serviceToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetConnectToRenderConnByUniqueIdLocked(token);
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GotConnectToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // Must success, or gonna throw an exception if failed. Caller must ensure token is valid.
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsValidRenderProcessUniqueIdLocked(token)) {
        RS_LOGE("%{public}s: token invalidated by death cleanup", __func__);
        return nullptr;
    }
    return connectToRenderConnections_.at(token);
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnByUniqueIdLocked(
    ProcessUniqueId token) const
{
    // without lock version
    if (auto iter = connectToRenderConnections_.find(token); iter != connectToRenderConnections_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GotComposerToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // Must success, or gonna throw an exception if failed. Caller must ensure token is valid.
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsValidRenderProcessUniqueIdLocked(token)) {
        RS_LOGE("%{public}s: token invalidated by death cleanup", __func__);
        return nullptr;
    }
    return composerToRenderConnections_.at(token);
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GetComposerToRenderConnByUniqueId(
    ProcessUniqueId token) const
{
    // lock version
    std::lock_guard<std::mutex> lock(mutex_);
    return GetComposerToRenderConnByUniqueIdLocked(token);
}

sptr<IRSComposerToRenderConnection> RSMultiRenderProcessManager::GetComposerToRenderConnByUniqueIdLocked(
    ProcessUniqueId token) const
{
    // without lock version
    if (auto iter = composerToRenderConnections_.find(token); iter != composerToRenderConnections_.end()) {
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
            __func__, token_.GetPid());
        return;
    }
    
    RS_LOGE("%{public}s: render_process died, DeathRecipient triggered for pid %{public}u timeStamp %{public}" PRIu64,
        __func__, token_.GetPid(), token_.GetTimestamp());
    managerSptr->HandleRenderProcessDeath(token_);
}

void RSMultiRenderProcessManager::HandleRenderProcessDeath(ProcessUniqueId token)
{
    RS_LOGI("Handling render_process death, pid=%{public}u, timeStamp=%{public}" PRIu64,
        token.GetPid(), token.GetTimestamp());
    CheckAndHandleSubprocessDeathOverflow();

    std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>> affectedScreenOutputs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        RemoveValidRenderProcessUniqueIdLocked(token);
        RemoveGroupIdByRenderProcessUniqueIdLocked(token);

        auto node = processToScreenOutputMap_.extract(token);
        if (node) {
            affectedScreenOutputs = std::move(node.mapped());
        }

        serviceToRenderConnections_.erase(token);
        connectToRenderConnections_.erase(token);
        composerToRenderConnections_.erase(token);
        UnregisterDeathRecipientLocked(token);
        pendingScreenConnectInfos_.erase(token);
        auto promiseIt = renderProcessReadyPromises_.find(token);
        if (promiseIt != renderProcessReadyPromises_.end()) {
            promiseIt->second.set_value(false);
            renderProcessReadyPromises_.erase(promiseIt);
        }
    }

    RS_LOGI("%{public}s: ProcessManager screen disconnection for pid=%{public}u, screenIds count=%{public}zu",
        __func__, token.GetPid(), affectedScreenOutputs.size());
    for (const auto& [screenId, output] : affectedScreenOutputs) {
        RS_LOGI("%{public}s: ScreenId=%{public}" PRIu64 " disconnected due to process death", __func__, screenId);
    }
    if (renderProcessDeathCallback_ && !affectedScreenOutputs.empty()) {
        renderProcessDeathCallback_(affectedScreenOutputs);
    }
}

void RSMultiRenderProcessManager::RegisterDeathRecipient(ProcessUniqueId token, const sptr<IRemoteObject>& binderObject)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RegisterDeathRecipientLocked(token, binderObject);
}

void RSMultiRenderProcessManager::RegisterDeathRecipientLocked(
    ProcessUniqueId token, const sptr<IRemoteObject>& binderObject)
{
    if (deathRecipients_.find(token) != deathRecipients_.end()) {
        RS_LOGW("%{public}s: DeathRecipient already registered for token(pid=%{public}u), replacing.",
            __func__, token.GetPid());
    }

    auto deathRecipient = sptr<RenderProcessDeathRecipient>::MakeSptr(token, wptr<RSMultiRenderProcessManager>(this));
    if (!binderObject->AddDeathRecipient(deathRecipient)) {
        RS_LOGE("%{public}s: Failed to add DeathRecipient for render_process %{public}u", __func__, token.GetPid());
        return;
    }

    deathRecipients_[token] = deathRecipient;
    RS_LOGI("%{public}s: Registered DeathRecipient for render_process %{public}u timeStamp %{public}" PRIu64,
        __func__, token.GetPid(), token.GetTimestamp());
}

void RSMultiRenderProcessManager::UnregisterDeathRecipient(ProcessUniqueId token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterDeathRecipientLocked(token);
}

void RSMultiRenderProcessManager::UnregisterDeathRecipientLocked(ProcessUniqueId token)
{
    auto it = deathRecipients_.find(token);
    if (it != deathRecipients_.end()) {
        deathRecipients_.erase(it);
        RS_LOGI("%{public}s: Unregistered DeathRecipient for render_process %{public}u timeStamp %{public}" PRIu64,
            __func__, token.GetPid(), token.GetTimestamp());
    }
}

void RSMultiRenderProcessManager::AddValidRenderProcessUniqueId(ProcessUniqueId token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    AddValidRenderProcessUniqueIdLocked(token);
}

void RSMultiRenderProcessManager::AddValidRenderProcessUniqueIdLocked(ProcessUniqueId token)
{
    validRenderProcessUniqueIds_.insert(token);
}

void RSMultiRenderProcessManager::RemoveValidRenderProcessUniqueId(ProcessUniqueId token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RemoveValidRenderProcessUniqueIdLocked(token);
}

void RSMultiRenderProcessManager::RemoveValidRenderProcessUniqueIdLocked(ProcessUniqueId token)
{
    validRenderProcessUniqueIds_.erase(token);
}

bool RSMultiRenderProcessManager::IsValidRenderProcessUniqueIdLocked(ProcessUniqueId token) const
{
    return validRenderProcessUniqueIds_.find(token) != validRenderProcessUniqueIds_.end();
}

void RSMultiRenderProcessManager::AddScreenOutputToProcess(ProcessUniqueId token, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output)
{
    std::lock_guard<std::mutex> lock(mutex_);
    AddScreenOutputToProcessLocked(token, screenId, output);
}

void RSMultiRenderProcessManager::AddScreenOutputToProcessLocked(ProcessUniqueId token, ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output)
{
    if (!IsValidRenderProcessUniqueIdLocked(token)) {
        RS_LOGW("%{public}s: Token no longer valid, skip adding screen %{public}" PRIu64,
            __func__, screenId);
        return;
    }
    auto& screenOutputs = processToScreenOutputMap_[token];
    auto existing = std::find_if(screenOutputs.begin(), screenOutputs.end(),
        [screenId](const auto& pair) { return pair.first == screenId; });
    if (existing != screenOutputs.end()) {
        existing->second = output;
    } else {
        screenOutputs.emplace_back(screenId, output);
    }
    RS_LOGI("AddScreenOutputToProcess screenId %{public}" PRIu64 " token %{public}u timeStamp %{public}" PRIu64
        "replaced=%{public}d", screenId, token.GetPid(), token.GetTimestamp(), existing != screenOutputs.end());
}
 
void RSMultiRenderProcessManager::RemoveScreenOutputFromProcess(ProcessUniqueId token, ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = processToScreenOutputMap_.find(token);
    if (it == processToScreenOutputMap_.end()) {
        RS_LOGW("%{public}s: Cannot find token in processToScreenOutputMap", __func__);
        return;
    }
    
    auto& screenOutputs = it->second;
    auto outputIt = std::find_if(screenOutputs.begin(), screenOutputs.end(), [screenId](const auto& pair) {
        return pair.first == screenId;
    });
    if (outputIt != screenOutputs.end()) {
        screenOutputs.erase(outputIt);
        RS_LOGI("Removed screenId %{public}" PRIu64 " from render_process %{public}u", screenId, token.GetPid());
        if (screenOutputs.empty()) {
            processToScreenOutputMap_.erase(it);
        }
    } else {
        RS_LOGW("%{public}s: Cannot find screenId %{public}" PRIu64, __func__, screenId);
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
