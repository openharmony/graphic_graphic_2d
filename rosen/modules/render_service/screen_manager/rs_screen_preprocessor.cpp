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

#include "common/rs_optional_trace.h"
#include "graphic_feature_param_manager.h"
#include <screen_manager/rs_screen_manager.h>

namespace OHOS {
namespace Rosen{
RSScreenPreprocessor::RSScreenPreprocessor(wptr<RSScreenManager> screenManager,
    std::shared_ptr<RSScreenCallbackManager> callbackMgr, std::shared_ptr<AppExecFwk::EventHandler> handler)
    : screenManager_(screenManager), callbackMgrWeak_(callbackMgr), mainHandler_(handler) {}

void RSScreenPreprocessor::OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data)
{
    if (output == nullptr) {
        RS_LOGE("%{public}s: output is nullptr", __func__);
        return;
    }

    RSScreenPreprocessor* processor = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr", __func__);
        processor = static_cast<RSScreenPreprocessor*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr", __func__);
    }

    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }

    processor->OnHotPlugEvent(output, connected);
}

void RSScreenPreprocessor::OnRefresh(ScreenId id, void* data)
{
    RSScreenPreprocessor* processor = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr", __func__);
        processor = static_cast<RSScreenPreprocessor*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr", __func__);
    }

    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }
    processor->OnRefreshEvent(id);
}

void RSScreenPreprocessor::OnHwcDead(void* data)
{
    RS_LOGW("%{public}s: The composer_host is already dead.", __func__);
    RSScreenPreprocessor* processor = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr", __func__);
        processor = static_cast<RSScreenPreprocessor*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr", __func__);
    }

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
    RSScreenPreprocessor* processor = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr", __func__);
        processor = static_cast<RSScreenPreprocessor*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr", __func__);
    }

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
    RSScreenPreprocessor* processor = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr", __func__);
        processor = static_cast<RSScreenPreprocessor*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr", __func__);
    }

    if (processor == nullptr) {
        RS_LOGE("%{public}s: processor is nullptr", __func__);
        return;
    }
    processor->OnScreenVBlankIdleEvent(devId, ns);
}

bool RSScreenPreprocessor::Init() noexcept
{
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
        RS_LOGE("%{public}s: Failed to register OnHwcDead Func to composer", __func__);
        return false;
    }

    // this feature attribute is non-core for startup; do not return false.
    if (composer_->RegScreenVBlankIdleCallback(&RSScreenPreprocessor::OnScreenVBlankIdle, this) != 0) {
        RS_LOGW("%{public}s: Not support register OnScreenVBlankIdle Func to composer", __func__);
    }
    RS_LOGI("Init RSScreenPreprocessor succeed");
    return true;
}

void RSScreenPreprocessor::OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected)
{
    {
        std::lock_guard<std::mutex> lock(hotPlugMutex_);
        ScreenId id = ToScreenId(output->GetScreenId());
        if (pendingHotPlugEvents_.find(id) != pendingHotPlugEvents_.end()) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 "is covered.", __func__, id);
        }
        pendingHotPlugEvents_[id] = ScreenHotPlugEvent{output, connected};
        RS_LOGE("%{public}s: screen %{public}" PRIu64 "is %{public}s, event has been saved", __func__, id,
            connected ? "connected" : "disconnected");
    }

    auto task = [this]() {
        std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents;
        {
            std::lock_guard<std::mutex> lock(hotPlugMutex_);
            pendingHotPlugEvents.swap(pendingHotPlugEvents_);
        }
        for (auto& [_, event] : pendingHotPlugEvents) {
            if (event.output == nullptr) {
                RS_LOGE("%{public}s: output is nullptr", __func__);
                continue;
            }
            if (event.connected) {
                ConfigureScreenConnected(event.output);
            } else {
                ConfigureScreenDisconnected(event.output);
            }
        }
        if (auto screenManager = screenManager_.promote()) {
            screenManager->ProcessPendingConnections();
        }
        isHwcDead_ = false;
    };
    ScheduleTask(task);
}

void RSScreenPreprocessor::ConfigureScreenConnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s: The screen for id %{public}" PRIu64 "connected.", __func__, id);

    ScreenChangeReason reason = ScreenChangeReason::DEFAULT;
    if (isHwcDead_ && id != 0 && MultiScreenParam::IsRsReportHwcDead()) {
        reason = ScreenChangeReason::HWCDEAD;
    }
    if (auto screenManager = screenManager_.promote()) {
        // 第一步：检查物理屏是否已经连接，已经连接先通知屏幕断开消息
        if (screenManager->GetScreen(id)) {
            RS_LOGW("dmulti_process %{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
            if (auto callbackMgr = callbackMgrWeak_.lock()) {
                ScreenPresenceEvent event = {.id = id, .connected = false, .reason = reason, .output = output};
                callbackMgr->NotifyScreenPresenceChanged(event);
            }
        }
        // 第二步: 配置连接消息
        screenManager->ProcessScreenConnected(id, output);
        // 第三步: 通知屏幕连接消息
        if (auto callbackMgr = callbackMgrWeak_.lock()) {
            ScreenPresenceEvent event = {.id = id, .connected = true, .reason = reason,
                .output = output, .property = screenManager->QueryScreenProperty(id)};
            callbackMgr->NotifyScreenPresenceChanged(event);
        }
    }
}

void RSScreenPreprocessor::ConfigureScreenDisconnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s: The screen for id %{public}" PRIu64 "disconnected.", __func__, id);

    ScreenChangeReason reason = ScreenChangeReason::DEFAULT;
    if (isHwcDead_ && id != 0 && MultiScreenParam::IsRsReportHwcDead()) {
        reason = ScreenChangeReason::HWCDEAD;
    }
    if (auto screenManager = screenManager_.promote()) {
        // 第一步：检查物理屏是否已经连接，已经连接通知屏幕断开消息
        if (screenManager->GetScreen(id)) {
            RS_LOGW("dmulti_process %{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
            if (auto callbackMgr = callbackMgrWeak_.lock()) {
                ScreenPresenceEvent event = {.id = id, .connected = false, .reason = reason, .output = output};
                callbackMgr->NotifyScreenPresenceChanged(event);
            }
        }
        // 第二步: 配置断开屏幕
        screenManager->ProcessScreenDisConnected(id, output);
    }
}

void RSScreenPreprocessor::OnRefreshEvent(ScreenId id)
{
    if (auto screenManager = screenManager_.promote()) {
        screenManager->OnRefreshEvent(id);
    }
}

// 统一改成直接通知屏幕断开 加上hwcdead标记位 ,内部的
// 异常断开 合成模块清理合成管线，保证不再使用hdi相关接口，
// 异常处理：渲染正常送合成 合成模块在hdi为空的时候要异常处理。
// 先通知异常断开，再做恢复重新初始化流程 ?task是不是可以用move优化下？
void RSScreenPreprocessor::OnHwcDeadEvent()
{
    auto task = [this] () {
        isHwcDead_ = true;
        if (auto screenManager = screenManager_.promote()) {
            screenManager->OnHwcDeadEvent();
        }
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
    if (auto screenManager = screenManager_.promote()) {
        screenManager->OnHwcEvent(deviceId, eventId, eventData);
    }
}

void RSScreenPreprocessor::OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns)
{
    if (auto screenManager = screenManager_.promote()) {
        screenManager->OnScreenVBlankIdleEvent(devId, ns);
    }
}

void RSScreenPreprocessor::ScheduleTask(std::function<void()> task)
{
    if (mainHandler_) {
        mainHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

} // namespace OHOS
} // namespace Rosen