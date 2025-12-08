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

#include "rs_render_process_manager.h"

#include "hgm_core.h"
#include "rs_render_composer_manager.h"
#include "rs_render_mode_config.h"
#include "rs_render_pipeline.h"
#include "rs_render_service.h"
#include "transaction/rs_service_to_render_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManager"

namespace OHOS {
namespace Rosen {
sptr<RSRenderProcessManager> RSRenderProcessManager::Create(RSRenderService& renderService)
{
    sptr<RSRenderProcessManager> renderProcessManager = new RSSingleRenderProcessManager(renderService);
    return renderProcessManager;
}

sptr<IRemoteObject> RSRenderProcessManager::OnScreenConnected(ScreenId id, const ScreenEventData& data,
    const sptr<RSScreenProperty>& property)
{
    RS_LOGD("%{public}s: rsScreenProperty.id[%{public}" PRIu64 "] .width[%{public}d] .height[%{public}d]",
        __func__, property->GetScreenId(), property->GetWidth(), property->GetHeight());
    renderService_.rsRenderComposerManager_->OnScreenConnected(data.output, property);
    renderService_.GetHgmContext()->AddScreenToHgm(id);
    uint64_t vsyncEnabledScreenId = renderService_.vsyncSampler_->JudgeVSyncEnabledScreenWhileHotPlug(id, true);
    renderService_.vsyncSampler_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId, renderService_.handler_);
    renderService_.screenManager_->SetScreenVsyncEnableById(vsyncEnabledScreenId, id, false);
    return nullptr;
}

void RSRenderProcessManager::OnScreenDisconnected(ScreenId id)
{
    RS_LOGD("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    renderService_.rsRenderComposerManager_->OnScreenDisconnected(id);
    renderService_.GetHgmContext()->RemoveScreenFromHgm(id);
    uint64_t vsyncEnabledScreenId = renderService_.vsyncSampler_->JudgeVSyncEnabledScreenWhileHotPlug(id, false);
    renderService_.vsyncSampler_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId, renderService_.handler_);
}

void RSRenderProcessManager::OnScreenPropertyChanged(ScreenId id, const sptr<RSScreenProperty>& property,
    std::optional<ScreenId> associatedScreenId)
{
    RS_LOGD("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    if (!property->IsVirtual()) {
        auto status = property->GetScreenPowerStatus();
        renderService_.vsyncSampler_->ProcessVSyncScreenIdWhilePowerStatusChanged(id, status, renderService_.handler_, renderService_.screenManager_->GetIsFoldScreenFlag());
    }
}

void RSRenderProcessManager::ONVBlankIdle(ScreenId id, uint64_t ns)
{
    if (auto composerConn = renderService_.rsRenderProcessManager_->GetRSComposerConnection(id)) {
        composerConn->OnScreenVBlankIdleCallback(id, ns);
    }
}

void RSRenderProcessManager::OnActiveScreenIdChanged(ScreenId activeScreenId)
{
    HgmCore::Instance().SetActiveScreenId(activeScreenId);
}

void RSRenderProcessManager::OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    auto serviceToRenderConns = GetServiceToRenderConns();
    for (const auto& conn : serviceToRenderConns) {
        RS_LOGI("dmulti_process %{public}s: OnHwcEvent start", __func__);
        conn->HandleHwcEvent(deviceId, eventId, eventData);
    }
}

void RSRenderProcessManager::OnScreenBacklightChanged(ScreenId id, uint32_t level)
{
    auto conn = GetServiceToRenderConn(id);
    if (conn == nullptr) {
        RS_LOGE("dmulti_process %{public}s conn is nullptr", __func__);
        return;
    }
    conn->OnScreenBacklightChanged(id, level);
}
} // namespace Rosen
} // namespace OHOS