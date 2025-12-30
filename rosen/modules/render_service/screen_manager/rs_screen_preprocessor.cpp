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
namespace Rosen{
constexpr int32_t SENSOR_SUCCESS = 0;
constexpr int32_t POSTURE_INTERVAL = 4000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
const std::string BOOTEVENT_BOOT_COMPLETED = "bootevent.boot.completed";
std::function<void(SensorEvent*)> sensorCallback = nullptr;
RSScreenPreprocessor::RSScreenPreprocessor(wptr<RSScreenManager> screenManager,
    std::shared_ptr<RSScreenCallbackManager> callbackMgr, std::shared_ptr<AppExecFwk::EventHandler> handler,
    bool isFoldScreen)
    : screenManager_(screenManager), callbackMgrWeak_(callbackMgr), mainHandler_(handler),
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
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    InitFoldSensor();
#endif
    RS_LOGI("Init RSScreenPreprocessor succeed");
    return true;
}

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
void RSScreenPreprocessor::InitFoldSensor()
{
    if (!isFoldScreenFlag_) {
        RS_LOGI("%{public}s not FoldScreen no need to InitFoldSensor.", __func__);
        return;
    }
    RS_LOGI("%{public}s FoldScreen need to RegisterSensorCallback.", __func__);
    RegisterSensorCallback();
    RSSystemProperties::WatchSystemProperty(BOOTEVENT_BOOT_COMPLETED.c_str(), OnBootComplete, nullptr);
    bool bootCompleted = RSSystemProperties::GetBootCompleted();
    if (UNLIKELY(bootCompleted)) {
        RS_LOGW("%{public}s boot completed.", __func__);
        UnRegisterSensorCallback();
        return;
    }
}

void RSScreenPreprocessor::RegisterSensorCallback()
{
    std::unique_lock<std::mutex> lock(registerSensorMutex_);
    if (hasRegisterSensorCallback_) {
        RS_LOGE("%{public}s hasRegisterSensorCallback_ is true", __func__);
        return;
    }
    hasRegisterSensorCallback_ = true;
    sensorCallback = std::bind(&RSScreenPreprocessor::HandlePostureData, this, std::placeholders::_1);
    sensorUser_.callback = [](SensorEvent* event) { sensorCallback(event); };
    int32_t subscribeRet;
    int32_t setBatchRet;
    int32_t activateRet;
    int tryCnt = 0;
    constexpr int tryLimit = 5; // 5 times failure limit
    do {
        subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
        RS_LOGI("%{public}s: subscribeRet: %{public}d", __func__, subscribeRet);
        setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &sensorUser_, POSTURE_INTERVAL, POSTURE_INTERVAL);
        RS_LOGI("%{public}s: setBatchRet: %{public}d", __func__, setBatchRet);
        activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
        RS_LOGI("%{public}s: activateRet: %{public}d", __func__, activateRet);
        if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
            RS_LOGE("%{public}s failed subscribeRet:%{public}d, setBatchRet:%{public}d, activateRet:%{public}d",
                    __func__, subscribeRet, setBatchRet, activateRet);
            usleep(1000); // wait 1000 us for next try
            tryCnt++;
        }
    } while (tryCnt <= tryLimit && (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS ||
        activateRet != SENSOR_SUCCESS));
    if (tryCnt <= tryLimit) {
        RS_LOGI("%{public}s success.", __func__);
    }
}

void RSScreenPreprocessor::UnRegisterSensorCallback()
{
    std::unique_lock<std::mutex> lock(registerSensorMutex_);
    if (!hasRegisterSensorCallback_) {
        RS_LOGE("%{public}s hasRegisterSensorCallback_ is false", __func__);
        return;
    }
    hasRegisterSensorCallback_ = false;
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        RS_LOGI("%{public}s success.", __func__);
    } else {
        RS_LOGE("%{public}s failed, deactivateRet:%{public}d, unsubscribeRet:%{public}d",
                __func__, deactivateRet, unsubscribeRet);
    }
}

void RSScreenPreprocessor::OnBootComplete(const char* key, const char* value, void *context)
{
    if (strcmp(key, BOOTEVENT_BOOT_COMPLETED.c_str()) == 0 && strcmp(value, "true") == 0) {
        RSScreenPreprocessor* processor = nullptr;
        if (context != nullptr) {
            RS_LOGI("%{public}s: data is not nullptr", __func__);
            processor = static_cast<RSScreenPreprocessor*>(context);
        } else {
            RS_LOGI("%{public}s: data is nullptr", __func__);
        }

        if (!processor) {
            RS_LOGE("%{public}s: processor is nullptr", __func__);
            return;
        }
    
        processor->OnBootCompleteEvent();
    } else {
        RS_LOGE("%{public}s key:%{public}s, value:%{public}s", __func__, key, value);
    }
}

void RSScreenPreprocessor::OnBootCompleteEvent()
{
    if (isFoldScreenFlag_) {
        RS_LOGI("%{public}s: UnRegisterSensorCallback", __func__);
        UnRegisterSensorCallback();
    }
}

void RSScreenPreprocessor::HandlePostureData(const SensorEvent* const event)
{
    if (event == nullptr) {
        RS_LOGW("%{public}s SensorEvent is nullptr.", __func__);
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        RS_LOGW("%{public}s SensorEvent[0].data is nullptr.", __func__);
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        RS_LOGW("%{public}s SensorEvent dataLen less than posture data size.", __func__);
        return;
    }
    PostureData* postureData = reinterpret_cast<PostureData*>(event[SENSOR_EVENT_FIRST_DATA].data);
    float angle = (*postureData).angle;
    RS_LOGD("%{public}s angle value in PostureData is: %{public}f.", __func__, angle);
    screenManager_->HandleSensorData(angle);
}
#endif

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
    if (auto screenManager = screenManager_.promote()) {
        screenManager->ProcessPendingConnections();
    } else {
        RS_LOGE("%{public}s: screenManager is nullptr.", __func__);
    }
    isHwcDead_ = false;
}

void RSScreenPreprocessor::ConfigureScreenConnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s The screen for id %{public}" PRIu64 ", connected.", __func__, id);

    if (auto screenManager = screenManager_.promote()) {
        // 第一步：检查物理屏是否已经连接，已经连接先通知屏幕断开消息
        if (screenManager->GetScreen(id)) {
            RS_LOGW("%{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
            if (auto callbackMgr = callbackMgrWeak_.lock()) {
                callbackMgr->NotifyScreenDisconnected(id);
            }
        }
        // 第二步: 配置连接消息
        screenManager->ProcessScreenConnected(id);
        // 第三步: 通知屏幕连接消息
        if (auto callbackMgr = callbackMgrWeak_.lock()) {
            ScreenPresenceEvent event = {.id = id, .output = output,
                                         .property = screenManager->QueryScreenProperty(id)};
            if (isHwcDead_) {
                callbackMgr->NotifyHwcRestored(event);
            } else {
                callbackMgr->NotifyScreenConnected(event);
            }
        }
    }
}

void RSScreenPreprocessor::ConfigureScreenDisconnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s: The screen for id %{public}" PRIu64 "disconnected.", __func__, id);

    if (auto screenManager = screenManager_.promote()) {
        // 第一步：检查物理屏是否已经连接，已经连接通知屏幕断开消息
        if (screenManager->GetScreen(id)) {
            RS_LOGW("%{public}s: The screen for id %{public}" PRIu64 "already existed.", __func__, id);
            if (auto callbackMgr = callbackMgrWeak_.lock()) {
                callbackMgr->NotifyScreenDisconnected(id);
            }
        }
        // 第二步: 配置断开屏幕
        screenManager->ProcessScreenDisConnected(id);
    }
}

void RSScreenPreprocessor::NotifyVirtualScreenConnected(ScreenId newId,
    ScreenId associatedScreenId, sptr<RSScreenProperty> property)
{
    if (auto callbackMgr = callbackMgrWeak_.lock()) {
        callbackMgr->NotifyVirtualScreenConnected(newId, associatedScreenId, property);
    }
}

void RSScreenPreprocessor::NotifyVirtualScreenDisconnected(ScreenId id)
{
    if (auto callbackMgr = callbackMgrWeak_.lock()) {
        callbackMgr->NotifyVirtualScreenDisconnected(id);
    }
}

void RSScreenPreprocessor::OnRefreshEvent(ScreenId id)
{
    if (auto callbackMgr = callbackMgrWeak_.lock()) {
        callbackMgr->NotifyScreenRefresh(id);
    }
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
        if (auto screenManager = screenManager_.promote()) {
            std::map<ScreenId, std::shared_ptr<RSScreen>> retScreens;
            screenManager->OnHwcDeadEvent(retScreens);
            #ifdef RS_ENABLE_GPU
            // The `NotifyHwcDead` method synchronously calls the composition cleanup-related resources.
            // It may take a long time due to task accumulation, resulting in holding screenMapMutex lock for too long.
            // That is why it is necessary to handle this notification independently.
            if (auto callbackMgr = callbackMgrWeak_.lock()) {
                for (const auto& [id, _] : retScreens) {
                    callbackMgr->NotifyHwcDead(id);
                }
            }
            #endif
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
    if (auto callbackMgr = callbackMgrWeak_.lock()) {
        callbackMgr->NotifyHwcEvent(deviceId, eventId, eventData);
    }
}

void RSScreenPreprocessor::OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns)
{
#ifdef RS_ENABLE_GPU
    if (auto callbackMgr = callbackMgrWeak_.lock()) {
        ScreenId id = ToScreenId(devId);
        callbackMgr->NotifyVBlankIdle(id, ns);
    }
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