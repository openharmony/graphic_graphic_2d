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

#include "rs_render_multi_process_manager.h"

#include <future>
#include <csignal>
#include <sys/prctl.h>
#include <sys/wait.h>

#include "rs_render_composer_manager.h"
#include "rs_render_service.h"

#undef LOG_TAG
#define LOG_TAG "RSMultiRenderProcessManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_RETRIES = 5;

void sigchld_handler(int sig)
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        exit(-1);
    }
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
        RS_LOGW("%{public}s: Forked success self %{public}d, parent %{public}d", __func__, getpid(), getppid());
        if (prctl(PR_SET_PDEATHSIG, SIGKILL) == -1) {
            RS_LOGE("%{public}s: prctl(PR_SET_PDEATHSIG, SIGKILL), errorno:%{public}d, errormsg:%{public}s", __func__,
                errno, strerror(errno));
            exit(-1);
        }
        std::string processName = "render_process_" + std::to_string(groupId);
        const char* binName = processName.c_str();
        char* argv[] = { const_cast<char*>(binName), NULL };
        if (execv("/system/bin/render_process", argv) == -1) {
            RS_LOGE("%{public}s: Fork execv command failed, errorno:%{public}d, errormsg:%{public}s", __func__, errno,
                strerror(errno));
            exit(-1);
        }
        return 0;
    } else {
        RS_LOGW("%{public}s: Forked success parent %{public}d, child %{public}d", __func__, getpid(), pid);
        return pid;
    }
}
} // namespace

RSMultiRenderProcessManager::RSMultiRenderProcessManager(RSRenderService& renderService) :
    RSRenderProcessManager(renderService)
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        RS_LOGE("Error setting SIGCHLD handler, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        exit(-1);
    }
}

void RSMultiRenderProcessManager::RecordRenderProcessConnection(pid_t pid,
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection,
    sptr<IRSComposerToRenderConnection> composerToRenderConnection,
    sptr<RSIConnectToRenderProcess> connectToRenderConnection)
{
    std::lock_guard<std::mutex> lock(mutex_);
    composerToRenderConnections_.insert_or_assign(pid, composerToRenderConnection);
    serviceToRenderConnections_.insert_or_assign(pid, serviceToRenderConnection);
    connectToRenderConnections_.insert_or_assign(pid, connectToRenderConnection);
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

sptr<RSScreenProperty> RSMultiRenderProcessManager::GetPendingScreenProperty(pid_t pid) const
{
    if (auto iter = pendingScreenConnectInfos_.find(pid); iter != pendingScreenConnectInfos_.end()) {
        return iter->second.property;
    }
    return nullptr;
}

void RSMultiRenderProcessManager::SetRenderProcessReadyPromise(pid_t pid)
{
    renderProcessReadyPromises_.at(pid).set_value(true);
    renderProcessReadyPromises_.erase(pid);
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
    auto renderConnProxy = optionalPid.has_value() ? HandleExistingGroup(optionalPid.value(), screenId, property)
                                                   : HandleNewGroup(groupId, screenId, property);
    return renderConnProxy;
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleExistingGroup(
    pid_t pid, ScreenId screenId, const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: GroupId has connected already, screenId is %{public}" PRIu64, __func__, screenId);
    auto renderToComposerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(screenId);
    auto composerToRenderConn = GotComposerToRenderConnByPid(pid);

    auto serviceToRenderConnection = GotServiceToRenderConnByPid(pid);
    serviceToRenderConnection->NotifyScreenConnectInfoToRender(property, renderToComposerConn, composerToRenderConn);

    auto connectToRender = GotConnectToRenderConnByPid(pid);
    return connectToRender->AsObject();
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleNewGroup(
    GroupId groupId, ScreenId screenId, const sptr<RSScreenProperty>& property)
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
            PendingScreenConnectInfo screenConnectInfo { .screenId = screenId, .property = property };
            pendingScreenConnectInfos_.insert_or_assign(pid, screenConnectInfo);
            renderProcessReadyPromises_[pid] = std::move(renderProcessReadyPromise);
            if (const auto& hgmContext = renderService_.GetHgmContext()) {
                hgmContext->AddRenderProcessPid(pid);
            }
            break;
        }
        RS_LOGE("%{public}s: Fork failed, retrying...", __func__);
        sleep(1);
    }
    if (pid <= 0) {
        RS_LOGE("%{public}s: Fork failed after 5 retries", __func__);
        exit(-1);
    }

    // Block and wait for the clientToRenderConnectionProxy to be ready
    renderProcessReadyFuture.wait();
    auto renderConnProxy = GotConnectToRenderConnByPid(pid)->AsObject();

    // TODO: 字体需要适配
    // auto conn = GotServiceToRenderConnByPid(pid);
    // auto cachedTypefaces = RSTypefaceCache::Instance().GetCachedTypeface();
    // for (auto& cachedTypeface : cachedTypefaces) {
    //     conn->RegisterTypeface(cachedTypeface.first, cachedTypeface.second);
    // }
    return renderConnProxy;
}

void RSMultiRenderProcessManager::OnScreenDisconnected(ScreenId screenId)
{
    auto serviceToRenderConnection = GetServiceToRenderConn(screenId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId);
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
    serviceToRenderConnection->NotifyScreenPropertyChangedInfoToRender(screenId, type, property);
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
            oldScreenServiceToRenderConn->NotifyScreenDisconnectInfoToRender(screenId);
        }
    }

    serviceToRenderConnection->NotifyScreenConnectInfoToRender(property, nullptr, nullptr);
}

void RSMultiRenderProcessManager::OnVirtualScreenDisconnected(ScreenId screenId)
{
    auto optionalConnScreenId = DeleteVirtualToPhysicalScreenMap(screenId);
    if (!optionalConnScreenId.has_value()) {
        return;
    }
    auto serviceToRenderConnection = GetServiceToRenderConn(optionalConnScreenId.value());
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId);
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConn(ScreenId screenId) const
{
    auto optionalGroupId = CheckGroupIdByScreenId(screenId);
    if (!optionalGroupId.has_value()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto optionalPid = GetRenderProcessPidByGroupIdLocked(optionalGroupId.value());
    if (!optionalPid.has_value()) {
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
    auto optionalGroupId = CheckGroupIdByScreenId(screenId);
    if (!optionalGroupId.has_value()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto optionalPid = GetRenderProcessPidByGroupIdLocked(optionalGroupId.value());
    if (!optionalPid.has_value()) {
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
        RS_LOGW("%{public}s: cannot found render_process by virtual screenId: %{public}" PRIu64, __func__, screenId);
        return iter->second;
    }
    return screenId;
}
} // namespace Rosen
} // namespace OHOS
