/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "rs_screen_preprocessor.h"

#include "parameter.h"
#include <parameters.h>
#include "common/rs_optional_trace.h"
#include "graphic_feature_param_manager.h"
#include "param/sys_param.h"

#include <screen_manager/rs_screen_manager.h>

namespace OHOS {
namespace Rosen {
RSScreenPreprocessor::RSScreenPreprocessor(RSScreenManager& screenManager,
    RSScreenCallbackManager& callbackMgr, std::shared_ptr<AppExecFwk::EventHandler> handler,
    bool isFoldScreen)
    : screenManager_(screenManager), callbackMgr_(callbackMgr), mainHandler_(handler),
      isFoldScreenFlag_(isFoldScreen) {}

void RSScreenPreprocessor::OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data)
{
    if (output == nullptr) {
        RS_LOGE("%{public}s: output is nullptr", __func__);
        return;
    }

    RSScreenPreprocessor* processor = static_cast<RSScreenPreprocessor*>(data);
    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }

    processor->OnHotPlugEvent(output, connected);
}

void RSScreenPreprocessor::OnRefresh(ScreenId id, void* data)
{
    RSScreenPreprocessor* processor = static_cast<RSScreenPreprocessor*>(data);
    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }
    processor->OnRefreshEvent(id);
}

void RSScreenPreprocessor::OnHwcDead(void* data)
{
    RS_LOGW("%{public}s: The composer_host is already dead.", __func__);
    RSScreenPreprocessor* processor = static_cast<RSScreenPreprocessor*>(data);
    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }

    processor->OnHwcDeadEvent();
}

void RSScreenPreprocessor::OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData, void* data)
{
    RS_LOGI("%{public}s: deviceId:%{public}" PRIu32 ", eventId:%{public}" PRIu32 "",
        __func__, deviceId, eventId);
    RSScreenPreprocessor* processor = static_cast<RSScreenPreprocessor*>(data);
    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }

    processor->OnHwcEventCallback(deviceId, eventId, eventData);
}

void RSScreenPreprocessor::OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void* data)
{
    RS_LOGI("%{public}s: devid:%{public}u, ns:" RSPUBU64, __func__, devId, ns);
    RS_TRACE_NAME_FMT("OnScreenVBlankIdle devId:%u, ns:%u", devId, ns);
    CreateVSyncSampler()->StartSample(true);
    RSScreenPreprocessor* processor = static_cast<RSScreenPreprocessor*>(data);
    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }

    processor->OnScreenVBlankIdleEvent(devId, ns);
}

bool RSScreenPreprocessor::Init() noexcept
{
    RS_TRACE_FUNC();
    composer_ = HdiBackend::GetInstance();
    if (composer_ == nullptr) {
        RS_LOGE("%{public}s: Failed to get composer.", __func__);
        return false;
    }

    if (composer_->RegScreenHotplug(&RSScreenPreprocessor::OnHotPlug, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnHotPlug Func to composer", __func__);
        return false;
    }

    if (composer_->RegScreenRefresh(&RSScreenPreprocessor::OnRefresh, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnRefresh Func to composer", __func__);
    }

    if (composer_->RegHwcDeadListener(&RSScreenPreprocessor::OnHwcDead, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnHwcDead Func to composer", __func__);
        return false;
    }

    if (composer_->RegHwcEventCallback(&RSScreenPreprocessor::OnHwcEvent, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnHwcEvent Func to composer", __func__);
        return false;
    }

    // this feature attribute is non-core for startup; do not return false.
    if (composer_->RegScreenVBlankIdleCallback(&RSScreenPreprocessor::OnScreenVBlankIdle, this) != 0) {
        RS_LOGW("%{public}s: Not support register OnScreenVBlankIdle Func to composer", __func__);
    }
    ProcessScreenHotPlugEvents();
    RS_LOGI("Init RSScreenPreprocessor succeed");
    return true;
}

void RSScreenPreprocessor::OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected)
{
    {
        std::lock_guard<std::mutex> lock(hotPlugMutex_);
        ScreenId id = ToScreenId(output->GetScreenId());
        if (auto iter = pendingHotPlugEvents_.find(id);
            iter != pendingHotPlugEvents_.end() && iter->second.connected == connected) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 "is covered.", __func__, id);
        }
        pendingHotPlugEvents_[id] = ScreenHotPlugEvent{output, connected};
        RS_LOGI("%{public}s: screen %{public}" PRIu64 "is %{public}s, event has been saved", __func__, id,
            connected ? "connected" : "disconnected");
    }
    if (isHwcDead_) {
        RS_LOGE("%{public}s: hotPlugEvent should be processed after Init() on hwc dead.", __func__);
        return;
    }
    ScheduleTask([this]() {
        ProcessScreenHotPlugEvents();
    });
}
void RSScreenPreprocessor::ProcessScreenHotPlugEvents()
{
    std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents;
    {
        std::lock_guard<std::mutex> lock(hotPlugMutex_);
        pendingHotPlugEvents.swap(pendingHotPlugEvents_);
    }
    for (auto& [_, event] : pendingHotPlugEvents) {
        if (event.output == nullptr) {
            RS_LOGE("%{public}s: output is nullptr.", __func__);
            continue;
        }
        if (event.connected) {
            ConfigureScreenConnected(event.output);
        } else {
            ConfigureScreenDisconnected(event.output);
        }
    }
    screenManager_.ProcessPendingConnections();
    isHwcDead_ = false;
}

void RSScreenPreprocessor::ConfigureScreenConnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s The screen for id %{public}" PRIu64 ", connected.", __func__, id);

    if (screenManager_.GetScreen(id)) {
        RS_LOGW("%{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
        callbackMgr_.NotifyScreenDisconnected(id);
    }
    screenManager_.ProcessScreenConnected(id);
    ScreenPresenceEvent event = { .id = id, .output = output, .property = screenManager_.QueryScreenProperty(id) };
    if (isHwcDead_) {
        callbackMgr_.NotifyHwcRestored(event);
    } else {
        callbackMgr_.NotifyScreenConnected(event);
    }
}

void RSScreenPreprocessor::ConfigureScreenDisconnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s: The screen for id %{public}" PRIu64 "disconnected.", __func__, id);

    if (screenManager_.GetScreen(id)) {
        RS_LOGW("%{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
        callbackMgr_.NotifyScreenDisconnected(id);
    }
    screenManager_.ProcessScreenDisConnected(id);
}

void RSScreenPreprocessor::NotifyVirtualScreenConnected(ScreenId newId,
    ScreenId associatedScreenId, sptr<RSScreenProperty> property)
{
    callbackMgr_.NotifyVirtualScreenConnected(newId, associatedScreenId, property);
}

void RSScreenPreprocessor::NotifyVirtualScreenDisconnected(ScreenId id)
{
    callbackMgr_.NotifyVirtualScreenDisconnected(id);
}

void RSScreenPreprocessor::NotifyActiveScreenIdChanged(ScreenId activeScreenId)
{
    callbackMgr_.NotifyActiveScreenIdChanged(activeScreenId);
}

void RSScreenPreprocessor::OnRefreshEvent(ScreenId id)
{
    callbackMgr_.NotifyScreenRefresh(id);
}

/**
 * @brief Composer exception handling:
 * The composer module reset composer resource to ensure that hdi interfaces are no longer used.
 * Render pipeline exception handling: Rendering proceeds normally to Composer.
 */
void RSScreenPreprocessor::OnHwcDeadEvent()
{
    auto task = [this] () {
        RS_TRACE_NAME("RSScreenPreprocessor::OnHwcDeadEvent");
        RS_LOGI("RSScreenPreprocessor::OnHwcDeadEvent.");
        isHwcDead_ = true;
        std::map<ScreenId, std::shared_ptr<RSScreen>> retScreens;
        screenManager_.OnHwcDeadEvent(retScreens);
        #ifdef RS_ENABLE_GPU
        // The `NotifyHwcDead` method synchronously calls the composition cleanup-related resources.
        // It may take a long time due to task accumulation, resulting in holding screenMapMutex lock for too long.
        // That is why it is necessary to handle this notification independently.
        for (const auto& [id, _] : retScreens) {
            callbackMgr_.NotifyHwcDead(id);
        }
        #endif
        if (!composer_) {
            RS_LOGE("CleanAndReinit: Failed to get composer.");
            return;
        }
        composer_->ResetDevice();
        if (!Init()) {
            RS_LOGE("CleanAndReinit: Reinit failed, RSScreenPreprocessor init failed.");
            return;
        }
    };
    ScheduleTask(task);
}

void RSScreenPreprocessor::OnHwcEventCallback(
    uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    callbackMgr_.NotifyHwcEvent(deviceId, eventId, eventData);
}

void RSScreenPreprocessor::OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns)
{
#ifdef RS_ENABLE_GPU
    ScreenId id = ToScreenId(devId);
    callbackMgr_.NotifyVBlankIdle(id, ns);
#endif
}

void RSScreenPreprocessor::ScheduleTask(std::function<void()> task)
{
    if (mainHandler_) {
        mainHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
} // namespace OHOS
} // namespace Rosen