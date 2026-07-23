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
#include "rs_render_service_agent.h"
#include "rs_render_process_manager_agent.h"
#include "screen_manager/public/rs_screen_manager_agent.h"
#include "transaction/rs_render_to_service_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSMultiRenderProcessManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_RETRIES = 5;
constexpr int32_t DEFAULT_MAX_FD = 1024;

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
        for (long fd = 3; fd < maxfd; fd++) {
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
    : RSRenderProcessManager(renderService),
      ipcPersistenceManager_(std::make_shared<RSIpcPersistenceManager>())
{}

void RSMultiRenderProcessManager::RecordComposerToRenderConnection(
    pid_t pid, sptr<IRSComposerToRenderConnection> composerToRenderConnection)
{
    stateStore_.RecordComposerToRenderConnection(pid, std::move(composerToRenderConnection));
}

sptr<RSScreenProperty> RSMultiRenderProcessManager::GetPendingScreenProperty(pid_t pid)
{
    return stateStore_.GetPendingScreenProperty(pid);
}

bool RSMultiRenderProcessManager::SetRenderProcessReadyPromise(pid_t pid,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
    const sptr<RSIConnectToRenderProcess>& connectToRenderConnection)
{
    auto factory = [weakManager = wptr<RSMultiRenderProcessManager>(this)](
        ProcessUniqueId uid) -> sptr<IRemoteObject::DeathRecipient> {
        return sptr<RenderProcessDeathRecipient>::MakeSptr(uid, weakManager);
    };
    return stateStore_.SetRenderProcessReadyPromise(pid, serviceToRenderConnection,
        connectToRenderConnection, factory, serviceToRenderConnection->AsObject());
}

GroupId RSMultiRenderProcessManager::GetGroupIdByScreenId(ScreenId screenId) const
{
    const auto& screenIdToGroupId = renderService_.renderModeConfig_->GetScreenIdToGroupId();
    auto iter = screenIdToGroupId.find(screenId);
    return (iter != screenIdToGroupId.end()) ? iter->second
                                             : renderService_.renderModeConfig_->GetDefaultRenderProcess();
}

sptr<IRemoteObject> RSMultiRenderProcessManager::CreateRenderToServiceConnection(pid_t callingPid)
{
    if (!stateStore_.IsValidRenderProcessPid(callingPid)) {
        RS_LOGE("%{public}s: Invalid render_process pid %{public}u, not forked by render_service",
            __func__, callingPid);
        return nullptr;
    }
    auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService_);
    auto renderProcessManagerAgent = sptr<RSRenderProcessManagerAgent>::MakeSptr(this);
    auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService_.screenManager_);
    auto renderToServiceConnection =
        sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent, screenManagerAgent);
    return renderToServiceConnection->AsObject();
}

sptr<IRemoteObject> RSMultiRenderProcessManager::OnScreenConnected(
    ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    GroupId groupId = GetGroupIdByScreenId(screenId);
    auto optionalProcessUniqueId = stateStore_.GetRenderProcessUniqueIdByGroupId(groupId);
    auto renderConnProxy = optionalProcessUniqueId.has_value() ?
        HandleExistingGroup(optionalProcessUniqueId.value(), screenId, output, property) :
        HandleNewGroup(groupId, screenId, output, property);
    return renderConnProxy;
}

sptr<IRemoteObject> RSMultiRenderProcessManager::HandleExistingGroup(ProcessUniqueId processUniqueId,
    ScreenId screenId, const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: GroupId has connected already, screenId is %{public}" PRIu64, __func__, screenId);
    auto serviceToRenderConnection = stateStore_.GetServiceToRenderConnByUniqueId(processUniqueId);
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return nullptr;
    }
    auto composerToRenderConn = stateStore_.GetComposerToRenderConnByUniqueId(processUniqueId);
    if (!composerToRenderConn) {
        RS_LOGE("%{public}s: composerToRenderConn is nullptr", __func__);
        return nullptr;
    }

    stateStore_.AddScreenOutputToProcess(processUniqueId, screenId, output);
    auto renderToComposerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(screenId);
    if (!serviceToRenderConnection->NotifyScreenConnectInfoToRender(property,
        renderToComposerConn, composerToRenderConn)) {
        RS_LOGE("%{public}s: NotifyScreenConnectInfoToRender failed, screenId=%{public}" PRIu64, __func__, screenId);
        return nullptr;
    }

    auto connectToRender = stateStore_.GetConnectToRenderConnByUniqueId(processUniqueId);
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

    int32_t pid = -1;
    std::optional<ProcessUniqueId> processUniqueIdOpt;
    for (int32_t retryCount = MAX_RETRIES; retryCount > 0; --retryCount) {
        pid = ForkAndExec(groupId);
        if (pid > 0) {
            processUniqueIdOpt.emplace(
                stateStore_.RegisterNewProcess(groupId, pid,
                    { screenId, output, property }, std::move(renderProcessReadyPromise)));
            break;
        }
        RS_LOGE("%{public}s: Fork failed, retrying...", __func__);
        sleep(1);
    }
    if (pid <= 0) {
        RS_LOGE("%{public}s: Fork failed after %{public}d retries", __func__, MAX_RETRIES);
        _exit(-1);
    }

    renderProcessReadyFuture.wait();
    RS_LOGI("%{public}s: render process is ready", __func__);
    auto renderConnProxy = stateStore_.GotConnectToRenderConnByUniqueId(processUniqueIdOpt.value());
    if (!renderConnProxy) {
        RS_LOGE("%{public}s: processUniqueId invalidated by death cleanup", __func__);
        return nullptr;
    }
    return renderConnProxy->AsObject();
}

void RSMultiRenderProcessManager::OnScreenDisconnected(ScreenId screenId)
{
    auto serviceToRenderConnection =
        stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(screenId));
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    if (!serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId)) {
        RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64, __func__, screenId);
    }
    auto optionalProcessUniqueId =
        stateStore_.GetRenderProcessUniqueIdByGroupId(GetGroupIdByScreenId(screenId));
    if (optionalProcessUniqueId.has_value()) {
        stateStore_.RemoveScreenOutputFromProcess(optionalProcessUniqueId.value(), screenId);
        RS_LOGI("%{public}s: Removed screenId %{public}" PRIu64 " from render_process %{public}u",
            __func__, screenId, optionalProcessUniqueId.value().GetPid());
    }
}

void RSMultiRenderProcessManager::OnScreenPropertyChanged(
    ScreenId screenId, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
{
    if (!property) {
        RS_LOGE("%{public}s: property is null, screenId: %{public}" PRIu64, __func__, screenId);
        return;
    }
    ScreenId connScreenId = stateStore_.FindVirtualToPhysicalScreenMap(screenId);
    auto serviceToRenderConnection =
        stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(connScreenId));
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
    auto serviceToRenderConnection =
        stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(screenId));
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    serviceToRenderConnection->NotifyScreenRefresh(screenId);
}

void RSMultiRenderProcessManager::OnVirtualScreenConnected(
    ScreenId screenId, ScreenId associatedScreenId, const sptr<RSScreenProperty>& property)
{
    auto serviceToRenderConnection =
        stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(associatedScreenId));
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr, associatedScreenId: %{public}" PRIu64, __func__,
            associatedScreenId);
        return;
    }

    ScreenId oldPhysicalScreenId = stateStore_.InsertVirtualToPhysicalScreenMap(screenId, associatedScreenId);
    if (oldPhysicalScreenId != INVALID_SCREEN_ID) {
        if (auto oldScreenServiceToRenderConn =
            stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(oldPhysicalScreenId))) {
            if (!oldScreenServiceToRenderConn->NotifyScreenDisconnectInfoToRender(screenId)) {
                RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64,
                    __func__, screenId);
            }
        }
    }

    if (!serviceToRenderConnection->NotifyScreenConnectInfoToRender(property, nullptr, nullptr)) {
        RS_LOGE("%{public}s: NotifyScreenConnectInfoToRender failed, screenId=%{public}" PRIu64,
            __func__, associatedScreenId);
    }
    auto optionalProcessUniqueId =
        stateStore_.GetRenderProcessUniqueIdByGroupId(GetGroupIdByScreenId(associatedScreenId));
    if (optionalProcessUniqueId.has_value()) {
        stateStore_.AddScreenOutputToProcess(optionalProcessUniqueId.value(), screenId, nullptr);
        RS_LOGI("%{public}s: Added virtualScreenId %{public}" PRIu64 " to render_process %{public}u",
            __func__, screenId, optionalProcessUniqueId.value().GetPid());
    } else {
        RS_LOGW("%{public}s: Cannot find pid for associatedScreenId %{public}" PRIu64,
            __func__, associatedScreenId);
    }
}

void RSMultiRenderProcessManager::OnVirtualScreenDisconnected(ScreenId screenId)
{
    auto optionalConnScreenId = stateStore_.DeleteVirtualToPhysicalScreenMap(screenId);
    if (!optionalConnScreenId.has_value()) {
        RS_LOGW("%{public}s: Cannot find physicalScreenId for virtualScreenId %{public}" PRIu64,
            __func__, screenId);
        return;
    }
    ScreenId associatedScreenId = optionalConnScreenId.value();
    auto serviceToRenderConnection =
        stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(associatedScreenId));
    if (!serviceToRenderConnection) {
        RS_LOGE("%{public}s: serviceToRenderConnection is nullptr", __func__);
        return;
    }
    if (!serviceToRenderConnection->NotifyScreenDisconnectInfoToRender(screenId)) {
        RS_LOGE("%{public}s: NotifyScreenDisconnectInfoToRender failed, screenId=%{public}" PRIu64,
            __func__, screenId);
    }
    auto optionalProcessUniqueId =
        stateStore_.GetRenderProcessUniqueIdByGroupId(GetGroupIdByScreenId(associatedScreenId));
    if (optionalProcessUniqueId.has_value()) {
        stateStore_.RemoveScreenOutputFromProcess(optionalProcessUniqueId.value(), screenId);
        RS_LOGI("%{public}s: Removed virtualScreenId %{public}" PRIu64 " from render_process %{public}u",
            __func__, screenId, optionalProcessUniqueId.value().GetPid());
    }
}

sptr<RSIServiceToRenderConnection> RSMultiRenderProcessManager::GetServiceToRenderConn(ScreenId screenId) const
{
    return stateStore_.GetServiceToRenderConnByGroupId(GetGroupIdByScreenId(screenId));
}

std::vector<sptr<RSIServiceToRenderConnection>> RSMultiRenderProcessManager::GetServiceToRenderConns() const
{
    return stateStore_.GetServiceToRenderConns();
}

sptr<RSIConnectToRenderProcess> RSMultiRenderProcessManager::GetConnectToRenderConnection(ScreenId screenId) const
{
    auto optionalProcessUniqueId =
        stateStore_.GetRenderProcessUniqueIdByGroupId(GetGroupIdByScreenId(screenId));
    if (!optionalProcessUniqueId.has_value()) {
        RS_LOGE("%{public}s: get render process processUniqueId failed", __func__);
        return nullptr;
    }
    return stateStore_.GetConnectToRenderConnByUniqueId(optionalProcessUniqueId.value());
}

std::shared_ptr<RSIpcPersistenceManager> RSMultiRenderProcessManager::GetIpcPersistenceManager() const
{
    return ipcPersistenceManager_;
}

void RSMultiRenderProcessManager::RenderProcessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    auto remoteObjectSptr = remoteObject.promote();
    if (remoteObjectSptr == nullptr) {
        RS_LOGW("%{public}s: can't promote remote object", __func__);
        return;
    }

    auto managerSptr = manager_.promote();
    if (managerSptr == nullptr) {
        RS_LOGW("%{public}s: manager has been destroyed, skip death handling for pid %{public}u",
            __func__, processUniqueId_.GetPid());
        return;
    }

    RS_LOGE("%{public}s: render_process died, DeathRecipient triggered for pid %{public}u",
        __func__, processUniqueId_.GetPid());
    managerSptr->HandleRenderProcessDeath(processUniqueId_);
}

void RSMultiRenderProcessManager::HandleRenderProcessDeath(ProcessUniqueId processUniqueId)
{
    RS_LOGI("Handling render_process death, pid=%{public}u", processUniqueId.GetPid());
    stateStore_.CheckAndHandleSubprocessDeathOverflow();

    auto affectedScreenOutputs = stateStore_.HandleRenderProcessDeath(processUniqueId);

    RS_LOGI("%{public}s: ProcessManager screen disconnection for pid=%{public}u, screenIds count=%{public}zu",
        __func__, processUniqueId.GetPid(), affectedScreenOutputs.size());
    for (const auto& [screenId, output] : affectedScreenOutputs) {
        RS_LOGI("%{public}s: ScreenId=%{public}" PRIu64 " disconnected due to process death", __func__, screenId);
    }
    if (!affectedScreenOutputs.empty() && renderService_.screenManager_ != nullptr) {
        renderService_.screenManager_->OnProcessDisconnected(affectedScreenOutputs);
    }
}

} // namespace Rosen
} // namespace OHOS
