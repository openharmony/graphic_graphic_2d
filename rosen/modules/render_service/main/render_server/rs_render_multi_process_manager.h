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

#include <refbase.h>

#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#include "rs_render_mode_config.h"
#include "rs_render_process_manager.h"
#include "rs_render_multi_process_manager_repository.h"

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
    sptr<RSScreenProperty> GetPendingScreenProperty(pid_t pid);
    bool SetRenderProcessReadyPromise(pid_t pid, const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
        const sptr<RSIConnectToRenderProcess>& connectToRenderConnection);

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override;
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override;

    std::shared_ptr<RSIpcPersistenceManager> GetIpcPersistenceManager() const override;

    sptr<IRemoteObject> CreateRenderToServiceConnection(pid_t callingPid) override;

private:
    sptr<IRemoteObject> HandleExistingGroup(ProcessUniqueId processUniqueId, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    sptr<IRemoteObject> HandleNewGroup(GroupId groupId, ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    GroupId GetGroupIdByScreenId(ScreenId screenId) const;

    void HandleRenderProcessDeath(ProcessUniqueId processUniqueId);

    class RenderProcessDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RenderProcessDeathRecipient(ProcessUniqueId processUniqueId, wptr<RSMultiRenderProcessManager> manager)
            : processUniqueId_(processUniqueId), manager_(manager) {}
        ~RenderProcessDeathRecipient() noexcept override = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remoteObject) override;

    private:
        ProcessUniqueId processUniqueId_;
        wptr<RSMultiRenderProcessManager> manager_;
    };

    RSRenderMultiProcessManagerRepository stateStore_;
    const std::shared_ptr<RSIpcPersistenceManager> ipcPersistenceManager_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_MULTI_PROCESS_MANAGER_H
