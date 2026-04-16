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

#include "rs_render_process_manager_agent.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManagerAgent"

namespace OHOS {
namespace Rosen {
RSRenderProcessManagerAgent::RSRenderProcessManagerAgent(sptr<RSRenderProcessManager> renderProcessManager)
    : renderProcessManager_(renderProcessManager) {}

void RSRenderProcessManagerAgent::SetRenderProcessReadyPromise(pid_t pid,
    const sptr<RSIServiceToRenderConnection>& serviceToRenderConnection,
    const sptr<RSIConnectToRenderProcess>& connectToRenderConnection)
{
    auto renderProcessManager = static_cast<RSMultiRenderProcessManager*>(renderProcessManager_.GetRefPtr());
    renderProcessManager->SetRenderProcessReadyPromise(pid, serviceToRenderConnection, connectToRenderConnection);
}

sptr<RSIServiceToRenderConnection> RSRenderProcessManagerAgent::GetServiceToRenderConn(ScreenId screenId) const
{
    return renderProcessManager_->GetServiceToRenderConn(screenId);
}

std::vector<sptr<RSIServiceToRenderConnection>> RSRenderProcessManagerAgent::GetServiceToRenderConns() const
{
    return renderProcessManager_->GetServiceToRenderConns();
}

std::shared_ptr<RSIpcPersistenceManager> RSRenderProcessManagerAgent::GetIpcPersistenceManager() const
{
    return renderProcessManager_->GetIpcPersistenceManager();
}

std::pair<sptr<RSScreenProperty>, std::shared_ptr<IpcPersistenceTypeToDataMap>>
RSRenderProcessManagerAgent::GetProcessInfo(pid_t pid, sptr<IRSComposerToRenderConnection> composerToRenderConnection)
{
    // TODO: 异常判断
    auto renderMultiProcessManager = static_cast<RSMultiRenderProcessManager*>(renderProcessManager_.GetRefPtr());
    renderMultiProcessManager->RecordComposerToRenderConnection(pid, composerToRenderConnection);

    auto rsScreenProperty = renderMultiProcessManager->GetPendingScreenProperty(pid);
    // TODO: 找个位置打印这个，这里可能为nullptr
    // RS_LOGI("%{public}s rsScreenProperty.id[%{public}" PRIu64 "] .width[%{public}d] .height[%{public}d]", __func__,
    //     rsScreenProperty->GetScreenId(), rsScreenProperty->GetWidth(), rsScreenProperty->GetHeight());

    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>(
        renderProcessManager_->GetIpcPersistenceManager()->GetReplayData());
    return { rsScreenProperty, replayData };
}
} // namespace Rosen
} // namespace OHOS