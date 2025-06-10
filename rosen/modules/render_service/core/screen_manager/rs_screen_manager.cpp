/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_screen_manager.h"

#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"

#include "common/rs_optional_trace.h"
#include "common/rs_switching_thread.h"
#include "display_engine/rs_color_temperature.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "graphic_feature_param_manager.h"
#include "hgm_core.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_screen.h"
#include "rs_trace.h"
#include "vsync_sampler.h"

#undef LOG_TAG
#define LOG_TAG "RSScreenManager"

namespace OHOS {
namespace Rosen {
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
namespace {
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float ANGLE_MAX_VAL = 180.0F;
constexpr int32_t SENSOR_SUCCESS = 0;
constexpr int32_t POSTURE_INTERVAL = 4000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
constexpr float HALF_FOLDED_MAX_THRESHOLD = 140.0F;
constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
constexpr uint32_t WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT = 1000;
constexpr uint32_t WAIT_FOR_STATUS_TASK_TIMEOUT = 1000; // 1000ms
constexpr uint32_t MAX_VIRTUAL_SCREEN_NUM = 64;
constexpr uint32_t MAX_VIRTUAL_SCREEN_WIDTH = 65536;
constexpr uint32_t MAX_VIRTUAL_SCREEN_HEIGHT = 65536;
constexpr uint32_t MAX_VIRTUAL_SCREEN_REFRESH_RATE = 120;
constexpr uint32_t ORIGINAL_FOLD_SCREEN_AMOUNT = 2;
const std::string FORCE_REFRESH_ONE_FRAME_TASK_NAME = "ForceRefreshOneFrameIfNoRNV";
void SensorPostureDataCallback(SensorEvent* event)
{
    CreateOrGetScreenManager()->HandlePostureData(event);
}
} // namespace
#endif
using namespace HiviewDFX;
namespace impl {
std::once_flag RSScreenManager::createFlag_;
sptr<OHOS::Rosen::RSScreenManager> RSScreenManager::instance_ = nullptr;

sptr<OHOS::Rosen::RSScreenManager> RSScreenManager::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new RSScreenManager();
    });

    return instance_;
}

bool RSScreenManager::Init() noexcept
{
    composer_ = HdiBackend::GetInstance();
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    isFoldScreenFlag_ = system::GetParameter("const.window.foldscreen.type", "") != "";
#endif
    if (composer_ == nullptr) {
        RS_LOGE("%{public}s: Failed to get composer.", __func__);
        return false;
    }

    composer_->InitLoadOptParams(loadOptParamsForScreen_.loadOptParamsForHdiBackend);

    if (composer_->RegScreenHotplug(&RSScreenManager::OnHotPlug, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnHotPlug Func to composer.", __func__);
        return false;
    }

    if (composer_->RegScreenRefresh(&RSScreenManager::OnRefresh, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnRefresh Func to composer.", __func__);
    }

    if (composer_->RegHwcDeadListener(&RSScreenManager::OnHwcDead, this) != 0) {
        RS_LOGE("%{public}s: Failed to register OnHwcDead Func to composer.", __func__);
        return false;
    }

    if (composer_->RegScreenVBlankIdleCallback(&RSScreenManager::OnScreenVBlankIdle, this) != 0) {
        RS_LOGW("%{public}s: Not support register OnScreenVBlankIdle Func to composer.", __func__);
    }

    // call ProcessScreenHotPlugEvents() for primary screen immediately in main thread.
    ProcessScreenHotPlugEvents();

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    if (isFoldScreenFlag_) {
        RS_LOGI("%{public}s: FoldScreen need to RegisterSensorCallback.", __func__);
        RegisterSensorCallback();
    }
#endif
    RS_LOGI("Init succeed");
    return true;
}

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
void RSScreenManager::RegisterSensorCallback()
{
    user.callback = SensorPostureDataCallback;
    int32_t subscribeRet;
    int32_t setBatchRet;
    int32_t activateRet;
    int tryCnt = 0;
    constexpr int tryLimit = 5; // 5 times failure limit
    do {
        subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
        RS_LOGI("%{public}s: subscribeRet: %{public}d", __func__, subscribeRet);
        setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &user, POSTURE_INTERVAL, POSTURE_INTERVAL);
        RS_LOGI("%{public}s: setBatchRet: %{public}d", __func__, setBatchRet);
        activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
        RS_LOGI("%{public}s: activateRet: %{public}d", __func__, activateRet);
        if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
            RS_LOGE("%{public}s failed.", __func__);
            usleep(1000); // wait 1000 us for next try
            tryCnt++;
        }
    } while (tryCnt <= tryLimit && (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS ||
        activateRet != SENSOR_SUCCESS));
    if (tryCnt <= tryLimit) {
        RS_LOGI("%{public}s success.", __func__);
    }
}

void RSScreenManager::UnRegisterSensorCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        RS_LOGI("%{public}s success.", __func__);
    }
}

void RSScreenManager::HandlePostureData(const SensorEvent* const event)
{
    if (event == nullptr) {
        RS_LOGW("SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        RS_LOGW("SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        RS_LOGW("SensorEvent dataLen less than posture data size.");
        return;
    }
    PostureData* postureData = reinterpret_cast<PostureData*>(event[SENSOR_EVENT_FIRST_DATA].data);
    float angle = (*postureData).angle;
    if (std::isless(angle, ANGLE_MIN_VAL) || std::isgreater(angle, ANGLE_MAX_VAL)) {
        RS_LOGW("Invalid angle value, angle is %{public}f.", angle);
        return;
    }
    RS_LOGI("angle vlaue in PostureData is: %{public}f.", angle);
    HandleSensorData(angle);
}

void RSScreenManager::HandleSensorData(float angle)
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    FoldState foldState = TransferAngleToScreenState(angle);
    if (foldState == FoldState::FOLDED) {
        if (activeScreenId_ != externalScreenId_) {
            activeScreenId_ = externalScreenId_;
            RS_LOGI("%{public}s: foldState is FoldState::FOLDED.", __func__);
        }
    } else if (activeScreenId_ != innerScreenId_) {
        activeScreenId_ = innerScreenId_;
        RS_LOGI("%{public}s: foldState is not FoldState::FOLDED.", __func__);
    }
    isPostureSensorDataHandled_ = true;
    HgmCore::Instance().SetActiveScreenId(activeScreenId_);
    activeScreenIdAssignedCV_.notify_one();
}

FoldState RSScreenManager::TransferAngleToScreenState(float angle)
{
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        RS_LOGI("%{public}s: angle isless ANGLE_MIN_VAL.", __func__);
        return FoldState::FOLDED;
    }
    if (std::isgreaterequal(angle, HALF_FOLDED_MAX_THRESHOLD)) {
        RS_LOGI("%{public}s: angle isgreaterequal HALF_FOLDED_MAX_THRESHOLD.", __func__);
        return FoldState::EXPAND;
    }
    FoldState state;
    if (std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD)) {
        RS_LOGI("%{public}s: angle islessequal OPEN_HALF_FOLDED_MIN_THRESHOLD.", __func__);
        state = FoldState::FOLDED;
    } else {
        RS_LOGI("%{public}s: angle isgreater HALF_FOLDED_MAX_THRESHOLD.", __func__);
        state = FoldState::EXPAND;
    }
    return state;
}

ScreenId RSScreenManager::GetActiveScreenId()
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    if (!isFoldScreenFlag_) {
        RS_LOGW("%{public}s: !isFoldScreenFlag_.", __func__);
        return INVALID_SCREEN_ID;
    }
    if (isPostureSensorDataHandled_) {
        isFirstTimeToGetActiveScreenId_ = false;
        UnRegisterSensorCallback();
        RS_LOGW("%{public}s: activeScreenId: %{public}" PRIu64, __func__, activeScreenId_);
        return activeScreenId_;
    }
    activeScreenIdAssignedCV_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT), [this]() {
            return isPostureSensorDataHandled_;
        });
    if (isFirstTimeToGetActiveScreenId_) {
        RS_LOGI("%{public}s: isFirstTimeToGetActiveScreenId_.", __func__);
        isFirstTimeToGetActiveScreenId_ = false;
        UnRegisterSensorCallback();
    }
    RS_LOGI("%{public}s: activeScreenId: %{public}" PRIu64, __func__, activeScreenId_);
    return activeScreenId_;
}
#else
ScreenId RSScreenManager::GetActiveScreenId()
{
    return INVALID_SCREEN_ID;
}
#endif

bool RSScreenManager::IsAllScreensPowerOff() const
{
    std::scoped_lock lock(screenMapMutex_, powerStatusMutex_);
    if (screenPowerStatus_.empty()) {
        RS_LOGE("%{public}s: screenPowerStatus_ is empty.", __func__);
        return false;
    }
    for (const auto& [id, powerStatus] : screenPowerStatus_) {
        auto iter = screens_.find(id);
        if (iter != screens_.end() && iter->second != nullptr && iter->second->IsVirtual()) {
            continue;
        }
        // we also need to consider the AOD mode(POWER_STATUS_SUSPEND)
        if (powerStatus != ScreenPowerStatus::POWER_STATUS_OFF &&
            powerStatus != ScreenPowerStatus::POWER_STATUS_SUSPEND) {
            return false;
        }
    }
    return true;
}

void RSScreenManager::PostForceRefreshTask()
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr && !mainThread->IsRequestedNextVSync()) {
        mainThread->PostTask([mainThread]() {
            RS_TRACE_NAME("No RNV, ForceRefreshOneFrame");
            mainThread->SetDirtyFlag();
            mainThread->RequestNextVSync();
        }, FORCE_REFRESH_ONE_FRAME_TASK_NAME, 20); // delay 20ms
    }
}

void RSScreenManager::RemoveForceRefreshTask()
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        mainThread->RemoveTask(FORCE_REFRESH_ONE_FRAME_TASK_NAME);
    }
}

void RSScreenManager::OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data)
{
    if (output == nullptr) {
        RS_LOGE("%{public}s: output is nullptr.", __func__);
        return;
    }

    RSScreenManager* screenManager = nullptr;
    if (data != nullptr) {
        RS_LOGI("%{public}s: data is not nullptr.", __func__);
        screenManager = static_cast<RSScreenManager*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr.", __func__);
        screenManager = static_cast<RSScreenManager*>(RSScreenManager::GetInstance().GetRefPtr());
    }

    if (screenManager == nullptr) {
        RS_LOGE("%{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }

    screenManager->OnHotPlugEvent(output, connected);
}

void RSScreenManager::OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected)
{
    {
        std::lock_guard<std::mutex> lock(hotPlugAndConnectMutex_);

        ScreenId id = ToScreenId(output->GetScreenId());
        if (pendingHotPlugEvents_.find(id) != pendingHotPlugEvents_.end()) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 "is covered.", __func__, id);
        }
        pendingHotPlugEvents_[id] = ScreenHotPlugEvent{output, connected};
        RS_LOGI("%{public}s: screen %{public}" PRIu64 "is %{public}s, event has been saved", __func__, id,
                connected ? "connected" : "disconnected");
    }

    // This func would be called in main thread first time immediately after calling composer_->RegScreenHotplug,
    // but at this time the RSMainThread object would not be ready to handle this, so we need to call
    // ProcessScreenHotPlugEvents() after this func in RSScreenManager::Init().

    // Normally, this func would be called in hdi's hw-ipc threads(but sometimes in main thread, maybe),
    // so we should notify the RSMainThread to postTask to call ProcessScreenHotPlugEvents().
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        RS_LOGE("%{public}s: mainThread is nullptr.", __func__);
        return;
    }
    RS_LOGI("%{public}s: mainThread->RequestNextVSync()", __func__);
    mainThread->RequestNextVSync();
}

void RSScreenManager::OnRefresh(ScreenId id, void* data)
{
    RSScreenManager* screenManager = nullptr;
    if (data != nullptr) {
        screenManager = static_cast<RSScreenManager*>(data);
    } else {
        RS_LOGI("%{public}s: data is nullptr.", __func__);
        screenManager = static_cast<RSScreenManager*>(RSScreenManager::GetInstance().GetRefPtr());
    }

    if (screenManager == nullptr) {
        RS_LOGE("%{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }
    screenManager->OnRefreshEvent(id);
}

void RSScreenManager::OnRefreshEvent(ScreenId id)
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        RS_LOGE("%{public}s: mainThread is nullptr.", __func__);
        return;
    }
    mainThread->PostTask([mainThread]() {
        mainThread->SetForceUpdateUniRenderFlag(true);
        mainThread->RequestNextVSync();
    });
}

void RSScreenManager::OnHwcDead(void* data)
{
    RS_LOGW("%{public}s: The composer_host is already dead.", __func__);
    RSScreenManager* screenManager = static_cast<RSScreenManager*>(RSScreenManager::GetInstance().GetRefPtr());
    if (screenManager == nullptr) {
        RS_LOGE("%{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }

    // Automatically recover when composer host dies.
    screenManager->CleanAndReinit();
}

void RSScreenManager::OnHwcDeadEvent()
{
    // Only clean the physical screen when hwc dead
    std::map<ScreenId, std::shared_ptr<OHOS::Rosen::RSScreen>> screens;
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        for (auto it = screens_.begin(); it != screens_.end();) {
            if (it->second && !it->second->IsVirtual()) {
                auto node = screens_.extract(it++);
                screens.insert(std::move(node));
            } else {
                ++it;
            }
        }
    }
#ifdef RS_ENABLE_GPU
    for (const auto& [_, screen] : screens) {
        RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
    }
#endif
    isHwcDead_ = true;
    defaultScreenId_ = INVALID_SCREEN_ID;
}

void RSScreenManager::OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void* data)
{
    RS_LOGI("%{public}s: devId:%{public}u, ns:" RSPUBU64, __func__, devId, ns);
    RS_TRACE_NAME_FMT("OnScreenVBlankIdle devId:%u, ns:%lu", devId, ns);
    CreateVSyncSampler()->StartSample(true);
    RSScreenManager* screenManager = static_cast<RSScreenManager*>(RSScreenManager::GetInstance().GetRefPtr());
    if (screenManager == nullptr) {
        RS_LOGE("%{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }
    screenManager->OnScreenVBlankIdleEvent(devId, ns);
}

void RSScreenManager::OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns)
{
    ScreenId screenId = ToScreenId(devId);
    if (GetScreen(screenId) == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, screenId);
        return;
    }
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().PostTask([screenId, ns]() {
        RSHardwareThread::Instance().OnScreenVBlankIdleCallback(screenId, ns);
    });
#endif
}

void RSScreenManager::CleanAndReinit()
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            RS_LOGE("%{public}s: Reinit failed, get RSMainThread failed.", __func__);
            return;
        }
        mainThread->PostTask([this]() {
            OnHwcDeadEvent();
            if (!composer_) {
                RS_LOGE("CleanAndReinit: Failed to get composer.");
                return;
            }
            composer_->ResetDevice();
            if (!Init()) {
                RS_LOGE("CleanAndReinit: Reinit failed, screenManager init failed in mainThread.");
                return;
            }
        });
    } else {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().PostTask([this]() {
            RS_LOGW("CleanAndReinit: clean and reinit in hardware thread.");
            OnHwcDeadEvent();
            if (!composer_) {
                RS_LOGE("CleanAndReinit: Failed to get composer.");
                return;
            }
            composer_->ResetDevice();
            if (!Init()) {
                RS_LOGE("CleanAndReinit: Reinit failed, screenManager init failed in HardwareThread.");
                return;
            }
        });
#endif
    }
}

bool RSScreenManager::TrySimpleProcessHotPlugEvents()
{
    std::lock_guard<std::mutex> lock(hotPlugAndConnectMutex_);
    if (!isHwcDead_ && pendingHotPlugEvents_.empty() && pendingConnectedIds_.empty()) {
        mipiCheckInFirstHotPlugEvent_ = true;
        return true;
    }
    RS_LOGI("%{public}s: isHwcDead_:%{public}d, pendingHotPlugEventsSize:%{public}zu, "
            "pendingConnectedIdsSize:%{public}zu",
            __func__, isHwcDead_.load(), pendingHotPlugEvents_.size(), pendingConnectedIds_.size());
    return false;
}

void RSScreenManager::ProcessScreenHotPlugEvents()
{
    std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents;
    {
        std::lock_guard<std::mutex> lock(hotPlugAndConnectMutex_);
        pendingHotPlugEvents.swap(pendingHotPlugEvents_);
    }
    for (auto& [_, event] : pendingHotPlugEvents) {
        if (event.output == nullptr) {
            RS_LOGE("%{public}s: output is nullptr.", __func__);
            continue;
        }
        if (event.connected) {
            ProcessScreenConnected(event.output);
            AddScreenToHgm(event.output);
        } else {
            ProcessScreenDisConnected(event.output);
            RemoveScreenFromHgm(event.output);
        }
    }
    ProcessPendingConnections();
    isHwcDead_ = false;
    mipiCheckInFirstHotPlugEvent_ = true;
}

void RSScreenManager::ProcessPendingConnections()
{
    std::vector<ScreenId> pendingConnectedIds;
    {
        std::lock_guard<std::mutex> lock(hotPlugAndConnectMutex_);
        pendingConnectedIds.swap(pendingConnectedIds_);
    }
    for (auto id : pendingConnectedIds) {
        if (!isHwcDead_) {
            TriggerCallbacks(id, ScreenEvent::CONNECTED);
        } else if (id != 0 && MultiScreenParam::IsRsReportHwcDead()) {
            TriggerCallbacks(id, ScreenEvent::CONNECTED, ScreenChangeReason::HWCDEAD);
        }
        auto screen = GetScreen(id);
        if (screen == nullptr) {
            continue;
        }

        ScreenRotation rotation = ScreenRotation::INVALID_SCREEN_ROTATION;
        int32_t backLightLevel = INVALID_BACKLIGHT_VALUE;
        {
            std::shared_lock<std::shared_mutex> lock(backLightAndCorrectionMutex_);
            if (auto iter = screenCorrection_.find(id); iter != screenCorrection_.end()) {
                rotation = iter->second;
            }
            if (auto iter = screenBacklight_.find(id); iter != screenBacklight_.end()) {
                backLightLevel = static_cast<int32_t>(iter->second);
            }
        }
        if (rotation != ScreenRotation::INVALID_SCREEN_ROTATION) {
            screen->SetScreenCorrection(rotation);
        }

        bool screenPowerOn = false;
        {
            std::shared_lock<std::shared_mutex> lock(powerStatusMutex_);
            auto iter = screenPowerStatus_.find(id);
            screenPowerOn = (iter == screenPowerStatus_.end() || iter->second == ScreenPowerStatus::POWER_STATUS_ON);
        }
        if (backLightLevel != INVALID_BACKLIGHT_VALUE && screenPowerOn) {
            screen->SetScreenBacklight(static_cast<uint32_t>(backLightLevel));
            auto mainThread = RSMainThread::Instance();
            mainThread->PostTask([mainThread]() {
                mainThread->SetDirtyFlag();
            });
            mainThread->ForceRefreshForUni();
        }
    }
}

void RSScreenManager::AddScreenToHgm(std::shared_ptr<HdiOutput>& output)
{
    RS_LOGI("%{public}s in", __func__);
    HgmTaskHandleThread::Instance().PostSyncTask([this, &output] () {
        auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
        ScreenId thisId = ToScreenId(output->GetScreenId());
        auto screen = GetScreen(thisId);
        if (screen == nullptr) {
            RS_LOGE("invalid screen id, screen not found : %{public}" PRIu64, thisId);
            return;
        }

        int32_t initModeId = 0;
        auto initMode = screen->GetActiveMode();
        if (!initMode) {
            RS_LOGE("%{public}s failed to get initial mode", __func__);
        } else {
            initModeId = initMode->id;
        }
        const auto& capability = screen->GetCapability();
        ScreenSize screenSize = {screen->Width(), screen->Height(), capability.phyWidth, capability.phyHeight};
        RS_LOGI("%{public}s: add screen: w * h: [%{public}u * %{public}u], capability w * h: "
            "[%{public}u * %{public}u]", __func__, screen->Width(), screen->Height(),
            capability.phyWidth, capability.phyHeight);
        if (hgmCore.AddScreen(thisId, initModeId, screenSize)) {
            RS_LOGE("%{public}s failed to add screen : %{public}" PRIu64, __func__, thisId);
            return;
        }

        // for each supported mode, use the index as modeId to add the detailed mode to hgm
        int32_t modeId = 0;
        auto supportedModes = screen->GetSupportedModes();
        for (const auto& mode : supportedModes) {
            if (hgmCore.AddScreenInfo(thisId, mode.width, mode.height, mode.freshRate, modeId)) {
                RS_LOGW("failed to add a screen profile to the screen : %{public}" PRIu64, thisId);
            }
            modeId++;
        }
    });
}

void RSScreenManager::RemoveScreenFromHgm(std::shared_ptr<HdiOutput>& output)
{
    RS_LOGI("%{public}s in", __func__);
    HgmTaskHandleThread::Instance().PostTask([id = ToScreenId(output->GetScreenId())] () {
        auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
        RS_LOGI("remove screen, id: %{public}" PRIu64, id);
        if (hgmCore.RemoveScreen(id)) {
            RS_LOGW("failed to remove screen : %{public}" PRIu64, id);
        }
    });
}

void RSScreenManager::ProcessScreenConnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("%{public}s The screen for id %{public}" PRIu64 " connected.", __func__, id);

    if (GetScreen(id)) {
        TriggerCallbacks(id, ScreenEvent::DISCONNECTED);
        RS_LOGW("%{public}s The screen for id %{public}" PRIu64 " already existed.", __func__, id);
    }
    auto screen = std::make_shared<RSScreen>(id, false, output, nullptr);

    std::unique_lock<std::mutex> lock(screenMapMutex_);
    screens_[id] = screen;
    if (isFoldScreenFlag_ && foldScreenIds_.size() < ORIGINAL_FOLD_SCREEN_AMOUNT) {
        foldScreenIds_[id] = {true, false};
    }
    lock.unlock();

    ScreenId defaultScreenId = defaultScreenId_;
    if (screen->GetCapability().type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        if (!mipiCheckInFirstHotPlugEvent_.exchange(true)) {
            defaultScreenId = id;
        }
    } else if (defaultScreenId == INVALID_SCREEN_ID) {
        RS_LOGI("%{public}s The screen id %{public}" PRIu64
                " is set to defaultScreenId. last defaultScreenId is %{public}" PRIu64 ".",
                __func__, id, defaultScreenId);
        defaultScreenId = id;
    }
    defaultScreenId_ = defaultScreenId;

    uint64_t vsyncEnabledScreenId = JudgeVSyncEnabledScreenWhileHotPlug(id, true);
    UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
    if (vsyncEnabledScreenId == id) {
        screen->SetScreenVsyncEnabled(true);
    }

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    if (isFoldScreenFlag_ && id != 0) {
        RS_LOGI("%{public}s The externalScreenId_ is set %{public}" PRIu64 ".", __func__, id);
        externalScreenId_ = id;
    }
#endif
    std::lock_guard<std::mutex> connectLock(hotPlugAndConnectMutex_);
    pendingConnectedIds_.emplace_back(id);
}

void RSScreenManager::ProcessScreenDisConnected(std::shared_ptr<HdiOutput>& output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGW("%{public}s process screen disconnected, id: %{public}" PRIu64, __func__, id);
    if (GetScreen(id) == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
    } else {
        TriggerCallbacks(id, ScreenEvent::DISCONNECTED);
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        screens_.erase(id);
        RS_LOGI("%{public}s: Screen(id %{public}" PRIu64 ") disconnected.", __func__, id);
    }
    {
        std::lock_guard<std::shared_mutex> lock(powerStatusMutex_);
        screenPowerStatus_.erase(id);
    }
    {
        std::lock_guard<std::shared_mutex> lock(backLightAndCorrectionMutex_);
        screenBacklight_.erase(id);
        screenCorrection_.erase(id);
    }
    if (id == defaultScreenId_) {
        HandleDefaultScreenDisConnected();
    }

    uint64_t vsyncEnabledScreenId = JudgeVSyncEnabledScreenWhileHotPlug(id, false);
    UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
}

void RSScreenManager::UpdateVsyncEnabledScreenId(ScreenId screenId)
{
    std::unique_lock<std::mutex> lock(screenMapMutex_);
    if (isFoldScreenFlag_ && foldScreenIds_.size() == ORIGINAL_FOLD_SCREEN_AMOUNT) {
        bool isAllFoldScreenDisconnected = true;
        for (const auto& [foldScreenId, foldScreenStatus] : foldScreenIds_) {
            if (foldScreenStatus.isConnected) {
                isAllFoldScreenDisconnected = false;
                break;
            }
        }
        auto it = foldScreenIds_.find(screenId);
        if (it == foldScreenIds_.end() && !isAllFoldScreenDisconnected) {
            return;
        }
    }
    lock.unlock();

    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RegSetScreenVsyncEnabledCallbackForMainThread(screenId);
    } else {
        RegSetScreenVsyncEnabledCallbackForHardwareThread(screenId);
    }
}

void RSScreenManager::RegSetScreenVsyncEnabledCallbackForMainThread(ScreenId vsyncEnabledScreenId)
{
    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("%{public}s failed, vsyncSampler is null", __func__);
        return;
    }
    vsyncSampler->SetVsyncEnabledScreenId(vsyncEnabledScreenId);
    vsyncSampler->RegSetScreenVsyncEnabledCallback([this](uint64_t screenId, bool enabled) {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            RS_LOGE("%{public}s screenVsyncEnabled:%{public}d set failed, "
                "get RSMainThread failed", __func__, enabled);
            return;
        }
        mainThread->PostTask([this, screenId, enabled]() {
            auto screen = GetScreen(screenId);
            if (screen == nullptr) {
                RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                    enabled, screenId);
                return;
            }
            screen->SetScreenVsyncEnabled(enabled);
        });
    });
}

void RSScreenManager::RegSetScreenVsyncEnabledCallbackForHardwareThread(ScreenId vsyncEnabledScreenId)
{
    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("%{public}s failed, vsyncSampler is null", __func__);
        return;
    }
    vsyncSampler->SetVsyncEnabledScreenId(vsyncEnabledScreenId);
#ifdef RS_ENABLE_GPU
    vsyncSampler->RegSetScreenVsyncEnabledCallback([this](uint64_t screenId, bool enabled) {
        RSHardwareThread::Instance().PostTask([this, screenId, enabled]() {
            auto screen = GetScreen(screenId);
            if (screen == nullptr) {
                RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                    enabled, screenId);
                return;
            }
            screen->SetScreenVsyncEnabled(enabled);
        });
    });
#endif
}

// If the previous primary screen disconnected, we traversal the left screens
// and find the first physical screen to be the default screen.
// If there was no physical screen left, we set the first screen as default, no matter what type it is.
// At last, if no screen left, we set Default Screen Id to INVALID_SCREEN_ID.
void RSScreenManager::HandleDefaultScreenDisConnected()
{
    ScreenId defaultScreenId = INVALID_SCREEN_ID;
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    auto iter = std::find_if(screens_.cbegin(), screens_.cend(), [](const auto& node) {
        const auto& screen = node.second;
        return screen && !screen->IsVirtual();
    });
    if (iter != screens_.cend()) {
        defaultScreenId = iter->first;
    }

    if (defaultScreenId == INVALID_SCREEN_ID && !screens_.empty()) {
        defaultScreenId = screens_.cbegin()->first;
    }
    defaultScreenId_ = defaultScreenId;
}

void RSScreenManager::UpdateFoldScreenConnectStatusLocked(ScreenId screenId, bool connected)
{
    if (isFoldScreenFlag_) {
        auto it = foldScreenIds_.find(screenId);
        if (it != foldScreenIds_.end()) {
            it->second.isConnected = connected;
        }
    }
}

uint64_t RSScreenManager::JudgeVSyncEnabledScreenWhileHotPlug(ScreenId screenId, bool connected)
{
    std::unique_lock<std::mutex> lock(screenMapMutex_);
    UpdateFoldScreenConnectStatusLocked(screenId, connected);

    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("%{public}s failed, vsyncSampler is null", __func__);
        return screenId;
    }
    uint64_t vsyncEnabledScreenId = vsyncSampler->GetVsyncEnabledScreenId();
    if (connected) { // screen connected
        if (vsyncEnabledScreenId == INVALID_SCREEN_ID) {
            return screenId;
        }
    } else { // screen disconnected
        if (vsyncEnabledScreenId != screenId) {
            return vsyncEnabledScreenId;
        }
        vsyncEnabledScreenId = INVALID_SCREEN_ID;
        auto iter = std::find_if(screens_.cbegin(), screens_.cend(), [](const auto& node) {
            const auto& screen = node.second;
            return screen && !screen->IsVirtual();
        });
        if (iter != screens_.end()) {
            vsyncEnabledScreenId = iter->first;
        }
    }
    return vsyncEnabledScreenId;
}

uint64_t RSScreenManager::JudgeVSyncEnabledScreenWhilePowerStatusChanged(ScreenId screenId, ScreenPowerStatus status)
{
    std::unique_lock<std::mutex> lock(screenMapMutex_);
    uint64_t vsyncEnabledScreenId = CreateVSyncSampler()->GetVsyncEnabledScreenId();
    auto it = foldScreenIds_.find(screenId);
    if (it == foldScreenIds_.end()) {
        return vsyncEnabledScreenId;
    }

    if (status == ScreenPowerStatus::POWER_STATUS_ON) {
        it->second.isPowerOn = true;
        auto vsyncScreenIt = foldScreenIds_.find(vsyncEnabledScreenId);
        if (vsyncScreenIt == foldScreenIds_.end() || vsyncScreenIt->second.isPowerOn == false) {
            return screenId;
        }
    } else if (status == ScreenPowerStatus::POWER_STATUS_OFF) {
        it->second.isPowerOn = false;
        if (screenId != vsyncEnabledScreenId) {
            return vsyncEnabledScreenId;
        }
        for (auto& [foldScreenId, status] : foldScreenIds_) {
            if (status.isConnected && status.isPowerOn) {
                return foldScreenId;
            }
        }
    }
    return vsyncEnabledScreenId;
}

// if SetVirtualScreenSurface success, force a refresh of one frame, avoiding prolong black screen
void RSScreenManager::ForceRefreshOneFrame() const
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        mainThread->PostTask([mainThread]() {
            mainThread->SetDirtyFlag();
        });
        mainThread->ForceRefreshForUni();
    }
}

void RSScreenManager::SetDefaultScreenId(ScreenId id)
{
    defaultScreenId_ = id;
}

ScreenId RSScreenManager::GenerateVirtualScreenId()
{
    std::lock_guard<std::mutex> lock(virtualScreenIdMutex_);
    if (!freeVirtualScreenIds_.empty()) {
        ScreenId id = freeVirtualScreenIds_.front();
        freeVirtualScreenIds_.pop();
        RS_LOGI("%{public}s: VirtualScreenId is %{public}" PRIu64, __func__, id);
        return id;
    }

    RS_LOGI("%{public}s: freeVirtualScreenIds_ is empty.", __func__);
    // The left 32 bits is for virtual screen id.
    return (static_cast<ScreenId>(virtualScreenCount_++) << 32) | 0xffffffffu;
}

void RSScreenManager::GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }

    virtualScreenResolution.SetVirtualScreenWidth(static_cast<uint32_t>(screen->Width()));
    virtualScreenResolution.SetVirtualScreenHeight(static_cast<uint32_t>(screen->Height()));
}

void RSScreenManager::GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    auto modeInfo = screen->GetActiveMode();
    if (!modeInfo) {
        RS_LOGE("%{public}s: Failed to get active mode for screen %{public}" PRIu64, __func__, id);
        return;
    }

    RS_LOGI("%{public}s: screen[%{public}" PRIu64 "] pixel[%{public}d * %{public}d],"
        "freshRate[%{public}d]", __func__, id, modeInfo->width, modeInfo->height, modeInfo->freshRate);
    screenModeInfo.SetScreenWidth(modeInfo->width);
    screenModeInfo.SetScreenHeight(modeInfo->height);
    screenModeInfo.SetScreenRefreshRate(modeInfo->freshRate);
    screenModeInfo.SetScreenModeId(screen->GetActiveModePosByModeId(modeInfo->id));
}

std::vector<RSScreenModeInfo> RSScreenManager::GetScreenSupportedModes(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return {};
    }

    const auto& displaySupportedModes = screen->GetSupportedModes();
    std::vector<RSScreenModeInfo> screenSupportedModes(displaySupportedModes.size());
    std::transform(displaySupportedModes.cbegin(), displaySupportedModes.cend(), screenSupportedModes.begin(),
        [](const auto& node) { return RSScreenModeInfo(node.width, node.height, node.freshRate, node.id); });
    return screenSupportedModes;
}

RSScreenCapability RSScreenManager::GetScreenCapability(ScreenId id) const
{
    RSScreenCapability screenCapability;
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return screenCapability;
    }

    if (screen->IsVirtual()) {
        RS_LOGW("%{public}s: only name attribute is valid for virtual screen.", __func__);
        screenCapability.SetName(screen->Name());
        return screenCapability;
    }

    const auto& capability = screen->GetCapability();
    std::vector<RSScreenProps> props(capability.props.size());
    std::transform(capability.props.cbegin(), capability.props.cend(), props.begin(), [](const auto& node) {
        return RSScreenProps(node.name, node.propId, node.value);
    });
    screenCapability.SetName(capability.name);
    screenCapability.SetType(static_cast<ScreenInterfaceType>(capability.type));
    screenCapability.SetPhyWidth(capability.phyWidth);
    screenCapability.SetPhyHeight(capability.phyHeight);
    screenCapability.SetSupportLayers(capability.supportLayers);
    screenCapability.SetVirtualDispCount(capability.virtualDispCount);
    screenCapability.SetSupportWriteBack(capability.supportWriteBack);
    screenCapability.SetProps(std::move(props));
    return screenCapability;
}

ScreenPowerStatus RSScreenManager::GetScreenPowerStatus(ScreenId id) const
{
    if (!RSSystemProperties::IsSmallFoldDevice()) {
        auto screen = GetScreen(id);
        if (screen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            return INVALID_POWER_STATUS;
        }

        ScreenPowerStatus status = static_cast<ScreenPowerStatus>(screen->GetPowerStatus());
        return status;
    }

    ScreenPowerStatus status = ScreenPowerStatus::INVALID_POWER_STATUS;
    RSSwitchingThread::Instance().PostSyncTask([id, &status, this]() {
        auto screen = GetScreen(id);
        if (screen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            status = INVALID_POWER_STATUS;
            return;
        }

        status = static_cast<ScreenPowerStatus>(screen->GetPowerStatus());
    });

    return status;
}

ScreenRotation RSScreenManager::GetScreenCorrection(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return ScreenRotation::INVALID_SCREEN_ROTATION;
    }

    return screen->GetScreenCorrection();
}

ScreenId RSScreenManager::GetDefaultScreenId() const
{
    return defaultScreenId_;
}

std::vector<ScreenId> RSScreenManager::GetAllScreenIds() const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    std::vector<ScreenId> ids;
    for (auto iter = screens_.begin(); iter != screens_.end(); ++iter) {
        ids.emplace_back(iter->first);
    }
    return ids;
}

ScreenId RSScreenManager::CreateVirtualScreen(
    const std::string& name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    if (currentVirtualScreenNum_ >= MAX_VIRTUAL_SCREEN_NUM) {
        RS_LOGW("%{public}s: virtual screens num %{public}" PRIu32 " has reached the maximum limit!",
            __func__, currentVirtualScreenNum_.load());
        return INVALID_SCREEN_ID;
    }
    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("%{public}s: width %{public}" PRIu32 " or height %{public}" PRIu32 " has reached"
            " the maximum limit!", __func__, width, height);
        return INVALID_SCREEN_ID;
    }
    if (surface != nullptr) {
        uint64_t surfaceId = surface->GetUniqueId();
        auto func = [surfaceId](const ScreenNode& node) {
            const auto& screen = node.second;
            return screen && screen->IsVirtual() && screen->GetProducerSurface() &&
                screen->GetProducerSurface()->GetUniqueId() == surfaceId;
        };
        if (AnyScreenFits(func)) {
            RS_LOGW("%{public}s: surface %{public}" PRIu64 " is used, create virtual"
                " screen failed!", __func__, surfaceId);
            return INVALID_SCREEN_ID;
        }
    } else {
        RS_LOGW("%{public}s: surface is nullptr.", __func__);
    }

    VirtualScreenConfigs configs;
    ScreenId newId = GenerateVirtualScreenId();
    configs.id = newId;
    configs.mirrorId = mirrorId;
    configs.name = name;
    configs.width = width;
    configs.height = height;
    configs.surface = surface;
    configs.flags = flags;
    configs.whiteList = std::unordered_set<NodeId>(whiteList.begin(), whiteList.end());

    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        screens_[newId] = std::make_shared<RSScreen>(configs);
    }
    ++currentVirtualScreenNum_;
    RS_LOGI("%{public}s: create virtual screen(id %{public}" PRIu64 ").", __func__, newId);
    return newId;
}

std::unordered_set<uint64_t> RSScreenManager::GetBlackListVirtualScreenByNode(uint64_t nodeId)
{
    std::unordered_set<uint64_t> virtualScreens = {};
    std::lock_guard<std::mutex> lock(blackListScreenMutex_);
    if (blackListInVirtualScreen_.find(nodeId) != blackListInVirtualScreen_.end()) {
        virtualScreens = blackListInVirtualScreen_[nodeId];
    }
    return virtualScreens;
}

int32_t RSScreenManager::SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    std::unordered_set<NodeId> screenBlackList(blackList.begin(), blackList.end());
    {
        std::lock_guard<std::mutex> lock(blackListScreenMutex_);
        for (const auto& [nodeId, screenIdSet] : blackListInVirtualScreen_) {
            if (screenIdSet.size() > 0 && screenIdSet.find(id) != screenIdSet.end()) {
                blackListInVirtualScreen_[nodeId].erase(id);
            }
        }
        for (const auto& nodeId : screenBlackList) {
            blackListInVirtualScreen_[nodeId].insert(id);
        }
    }
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("%{public}s: Cast screen blacklists for id %{public}" PRIu64, __func__, id);
        std::lock_guard<std::mutex> lock(blackListMutex_);
        castScreenBlackList_ = std::move(screenBlackList);
        return SUCCESS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: Record screen blacklists for id %{public}" PRIu64, __func__, id);
    virtualScreen->SetBlackList(screenBlackList);

    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = GetScreen(mainId);
        if (mainScreen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, mainId);
            return SCREEN_NOT_FOUND;
        }
        mainScreen->SetBlackList(screenBlackList);
    }
    return SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList)
{
    std::unordered_set<NodeType> screenTypeBlackList(typeBlackList.begin(), typeBlackList.end());
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("%{public}s: Cast screen typeblacklists for id %{public}" PRIu64, __func__, id);
        std::lock_guard<std::mutex> lock(typeBlackListMutex_);
        castScreenTypeBlackList_ = std::move(screenTypeBlackList);
        return SUCCESS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: Record screen typeblacklists for id %{public}" PRIu64, __func__, id);
    virtualScreen->SetTypeBlackList(screenTypeBlackList);
    return SUCCESS;
}

int32_t RSScreenManager::AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    {
        std::lock_guard<std::mutex> lock(blackListScreenMutex_);
        for (const auto& nodeId : blackList) {
            blackListInVirtualScreen_[nodeId].insert(id);
        }
    }
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("%{public}s: Cast screen blacklists", __func__);
        std::lock_guard<std::mutex> lock(blackListMutex_);
        castScreenBlackList_.insert(blackList.cbegin(), blackList.cend());
        return SUCCESS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: Record screen blacklists for id %{public}" PRIu64, __func__, id);
    virtualScreen->AddBlackList(blackList);

    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = GetScreen(mainId);
        if (mainScreen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, mainId);
            return SCREEN_NOT_FOUND;
        }
        mainScreen->AddBlackList(blackList);
    }
    return SUCCESS;
}

int32_t RSScreenManager::RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    {
        std::lock_guard<std::mutex> lock(blackListScreenMutex_);
        for (const auto& nodeId : blackList) {
            blackListInVirtualScreen_[nodeId].erase(id);
        }
    }
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("%{public}s: Cast screen blacklists", __func__);
        std::lock_guard<std::mutex> lock(blackListMutex_);
        for (const auto& list : blackList) {
            castScreenBlackList_.erase(list);
        }
        return SUCCESS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: Record screen blacklists for id %{public}" PRIu64, __func__, id);
    virtualScreen->RemoveBlackList(blackList);

    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = GetScreen(mainId);
        if (mainScreen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, mainId);
            return SCREEN_NOT_FOUND;
        }
        mainScreen->RemoveBlackList(blackList);
    }
    return SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<uint64_t>& securityExemptionList)
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGW("%{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }

    if (!virtualScreen->IsVirtual()) {
        RS_LOGW("%{public}s: not virtual screen for id %{public}" PRIu64, __func__, id);
        return INVALID_ARGUMENTS;
    }
    virtualScreen->SetSecurityExemptionList(securityExemptionList);
    for (const auto& exemption : securityExemptionList) {
        RS_LOGW("%{public}s: virtual screen(id %{public}" PRIu64 "), nodeId %{public}" PRIu64,
            __func__, id, exemption);
    }
    return SUCCESS;
}

const std::vector<uint64_t> RSScreenManager::GetVirtualScreenSecurityExemptionList(ScreenId id) const
{
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return {};
    }

    return virtualScreen->GetSecurityExemptionList();
}

int32_t RSScreenManager::SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask)
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGW("%{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }

    return screen->SetSecurityMask(securityMask);
}

std::shared_ptr<Media::PixelMap> RSScreenManager::GetScreenSecurityMask(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return nullptr;
    }
    return screen->GetSecurityMask();
}

int32_t RSScreenManager::GetVirtualScreenSecLayerOption(ScreenId id) const
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGW("%{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr || !virtualScreen->IsVirtual()) {
        RS_LOGW("%{public}s: There is no virtual screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    return virtualScreen->GetVirtualSecLayerOption();
}

int32_t RSScreenManager::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation)
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGW("%{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    auto mainScreen = GetScreen(id);
    if (mainScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }

    // zero Rect means disable visible rect
    static Rect ZERO = {0, 0, 0, 0};
    mainScreen->SetEnableVisibleRect(mainScreenRect != ZERO);
    mainScreen->SetMainScreenVisibleRect(mainScreenRect);
    mainScreen->SetVisibleRectSupportRotation(supportRotation);
    RS_LOGI("%{public}s: mirror screen(id %{public}" PRIu64 "), "
        "visible rect[%{public}d, %{public}d, %{public}d, %{public}d], supportRotation: %{public}d",
        __func__, id, mainScreenRect.x, mainScreenRect.y, mainScreenRect.w, mainScreenRect.h, supportRotation);
    return SUCCESS;
}

Rect RSScreenManager::GetMirrorScreenVisibleRect(ScreenId id) const
{
    auto mirrorScreen = GetScreen(id);
    if (mirrorScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return {};
    }

    return mirrorScreen->GetMainScreenVisibleRect();
}

int32_t RSScreenManager::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    virtualScreen->SetCastScreenEnableSkipWindow(enable);
    RS_LOGW("%{public}s: screen id %{public}" PRIu64 " set %{public}d success.", __func__, id, enable);
    return SUCCESS;
}

const std::unordered_set<NodeId> RSScreenManager::GetVirtualScreenBlackList(ScreenId id) const
{
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return {};
    }
    std::unordered_set<uint64_t> virtualScreenBlackList = virtualScreen->GetBlackList();
    // if the skipWindow flag is enabled, the global blacklist takes effect in addition
    if (virtualScreen->GetCastScreenEnableSkipWindow()) {
        RS_LOGD("%{public}s: Cast screen blacklists for id %{public}" PRIu64, __func__, id);
        std::lock_guard<std::mutex> lock(blackListMutex_);
        virtualScreenBlackList.insert(castScreenBlackList_.begin(), castScreenBlackList_.end());
    }
    RS_LOGD("%{public}s: Record screen blacklists for id %{public}" PRIu64, __func__, id);
    return virtualScreenBlackList;
}

const std::unordered_set<NodeType> RSScreenManager::GetVirtualScreenTypeBlackList(ScreenId id) const
{
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return {};
    }
    if (virtualScreen->GetCastScreenEnableSkipWindow()) {
        RS_LOGD("%{public}s: Cast screen typeblacklists for id %{public}" PRIu64, __func__, id);
        std::lock_guard<std::mutex> lock(typeBlackListMutex_);
        return castScreenTypeBlackList_;
    }
    RS_LOGD("%{public}s: Record screen typeblacklists for id %{public}" PRIu64, __func__, id);
    return virtualScreen->GetTypeBlackList();
}

std::unordered_set<uint64_t> RSScreenManager::GetAllBlackList() const
{
    std::unordered_set<uint64_t> allBlackList;
    std::scoped_lock lock(screenMapMutex_, blackListMutex_);
    for (const auto& [_, screen] : screens_) {
        if (screen == nullptr) {
            continue;
        }
        if (screen->GetCastScreenEnableSkipWindow()) {
            allBlackList.insert(castScreenBlackList_.cbegin(), castScreenBlackList_.cend());
        } else {
            const auto blackList = screen->GetBlackList();
            allBlackList.insert(blackList.cbegin(), blackList.cend());
        }
    }
    return allBlackList;
}

std::unordered_set<uint64_t> RSScreenManager::GetAllWhiteList() const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    std::unordered_set<uint64_t> allWhiteList;
    for (const auto& [_, screen] : screens_) {
        if (screen != nullptr) {
            const auto& whiteList = screen->GetWhiteList();
            allWhiteList.insert(whiteList.begin(), whiteList.end());
        }
    }
    return allWhiteList;
}

int32_t RSScreenManager::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    if (surface == nullptr) {
        RS_LOGW("%{public}s: screenId:%{public}" PRIu64 " surface is null.", __func__, id);
        return INVALID_ARGUMENTS;
    }
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    if (!screen->IsVirtual()) {
        RS_LOGW("%{public}s: The screen is not virtual, id %{public}" PRIu64, __func__, id);
        return INVALID_ARGUMENTS;
    }
    uint64_t surfaceId = surface->GetUniqueId();

    auto func = [id, surfaceId](const ScreenNode& node) -> bool {
        const auto& [screenId, screen] = node;
        return screen && screen->IsVirtual() && screenId != id && screen->GetProducerSurface() &&
            screen->GetProducerSurface()->GetUniqueId() == surfaceId;
    };
    if (AnyScreenFits(func)) {
        RS_LOGE("%{public}s: surface %{public}" PRIu64 " is used, set surface failed!", __func__, surfaceId);
        return SURFACE_NOT_UNIQUE;
    }

    screen->SetProducerSurface(surface);
    RS_LOGI("%{public}s: set virtual screen surface success!", __func__);
    RS_TRACE_NAME("RSScreenManager::SetVirtualScreenSurface, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

bool RSScreenManager::GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const
{
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }
    return virtualScreen->GetAndResetVirtualSurfaceUpdateFlag();
}

void RSScreenManager::RemoveVirtualScreen(ScreenId id)
{
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        auto screensIt = screens_.find(id);
        if (screensIt == screens_.end() || screensIt->second == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            return;
        }
        if (!screensIt->second->IsVirtual()) {
            RS_LOGW("%{public}s: The screen is not virtual, id %{public}" PRIu64, __func__, id);
            return;
        }

        screens_.erase(screensIt);
        --currentVirtualScreenNum_;

        // Update other screens' mirrorId.
        for (auto& [id, screen] : screens_) {
            if (screen == nullptr) {
                RS_LOGW("%{public}s: screen %{public}" PRIu64 " not found", __func__, id);
                continue;
            }
            if (screen->MirroredId() == id) {
                screen->SetMirror(INVALID_SCREEN_ID);
            }
        }
        RS_LOGI("%{public}s: remove virtual screen(id %{public}" PRIu64 ").", __func__, id);
    }
    {
        std::lock_guard<std::mutex> lock(virtualScreenIdMutex_);
        freeVirtualScreenIds_.push(id);
    }

    // when virtual screen doesn't exist no more, render control can be recovered.
    {
        std::lock_guard<std::mutex> lock(renderControlMutex_);
        disableRenderControlScreens_.erase(id);
    }
}

uint32_t RSScreenManager::GetCurrentVirtualScreenNum()
{
    return currentVirtualScreenNum_;
}

uint32_t RSScreenManager::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetActiveMode(modeId);
}

uint32_t RSScreenManager::SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    RSHardwareThread::Instance().ScheduleTask([screen, activeRect, id]() {
        if (screen->SetScreenActiveRect(activeRect) != StatusCode::SUCCESS) {
            RS_LOGW("%{public}s: Invalid param", __func__);
            return;
        }
        auto screenManager = CreateOrGetScreenManager();
        if (screenManager) {
            auto output = screenManager->GetOutput(id);
            if (output) {
                output->SetActiveRectSwitchStatus(true);
            }
        }
    }).wait();
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    return screen->SetResolution(width, height);
}

int32_t RSScreenManager::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("%{public}s: width %{public}" PRIu32 " or height %{public}" PRIu32 " has reached "
            "the maximum limit!", __func__, width, height);
        return INVALID_ARGUMENTS;
    }
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screen->SetResolution(width, height);
    RS_LOGI("%{public}s: set virtual screen resolution success", __func__);
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetVirtualScreenResolution, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

int32_t RSScreenManager::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screen->SetRogResolution(width, height);
    return SUCCESS;
}

void RSScreenManager::ProcessVSyncScreenIdWhilePowerStatusChanged(ScreenId id, ScreenPowerStatus status)
{
    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("%{public}s failed, vsyncSampler is null.", __func__);
        return;
    }
    RS_TRACE_NAME_FMT("%s, id:%lu, status:%d, isPowerOff:%d, isSuspend:%d", __func__, id, status,
        (status == ScreenPowerStatus::POWER_STATUS_OFF), (status == ScreenPowerStatus::POWER_STATUS_SUSPEND));
    if (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) {
        vsyncSampler->SetScreenVsyncEnabledInRSMainThread(id, false);
    }
    if (isFoldScreenFlag_) {
        uint64_t vsyncEnabledScreenId = JudgeVSyncEnabledScreenWhilePowerStatusChanged(id, status);
        uint64_t lastVsyncEnabledScreenId = vsyncSampler->GetVsyncEnabledScreenId();
        if (vsyncEnabledScreenId != lastVsyncEnabledScreenId) {
            RS_TRACE_NAME_FMT("vsyncEnabledScreenId has changed, need disable lastVsyncEnabledScreenId vsync, "
                "vsyncEnabledScreenId:%lu, lastVsyncEnabledScreenId:%lu",
                vsyncEnabledScreenId, lastVsyncEnabledScreenId);
            vsyncSampler->SetScreenVsyncEnabledInRSMainThread(lastVsyncEnabledScreenId, false);
        }
        UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
    }
}

void RSScreenManager::UpdateScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("[UL_POWER] %{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }

    isScreenPoweringOn_ =
        (status == ScreenPowerStatus::POWER_STATUS_ON || status == ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    auto ret = screen->SetPowerStatus(static_cast<uint32_t>(status));
    isScreenPoweringOn_ = false;
    if (ret != static_cast<int32_t>(StatusCode::SUCCESS)) {
        RS_LOGE("[UL_POWER] %{public}s: Failed to set power status of id %{public}" PRIu64, __func__, id);
        return;
    }

    ProcessVSyncScreenIdWhilePowerStatusChanged(id, status);

    /*
     * If app adds the first frame when power on the screen, delete the code
     */
    if (status == ScreenPowerStatus::POWER_STATUS_ON ||
        status == ScreenPowerStatus::POWER_STATUS_ON_ADVANCED) {
        RSFirstFrameNotifier::GetInstance().AddFirstFrameCommitScreen(id);
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            RS_LOGE("[UL_POWER] %{public}s: mainThread is nullptr", __func__);
            return;
        }
        mainThread->PostTask([mainThread]() {
            mainThread->SetDirtyFlag();
            mainThread->SetScreenPowerOnChanged(true);
        });
        std::shared_lock<std::shared_mutex> lock(powerStatusMutex_);
        if (screenPowerStatus_.count(id) == 0 ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF_FAKE ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED) {
            mainThread->ForceRefreshForUni();
        } else {
            mainThread->RequestNextVSync();
        }

        RS_LOGI("[UL_POWER] %{public}s: PowerStatus %{public}d, request a frame", __func__, status);
    }
    RSColorTemperature::Get().UpdateScreenStatus(id, status);
    std::lock_guard<std::shared_mutex> lock(powerStatusMutex_);
    screenPowerStatus_[id] = status;
}

void RSScreenManager::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (!RSSystemProperties::IsSmallFoldDevice()) {
        UpdateScreenPowerStatus(id, status);
        return;
    }

    ResetScreenPowerStatusTask();
    RSSwitchingThread::Instance().PostTask([id, status, this]() {
        if (status == ScreenPowerStatus::POWER_STATUS_OFF) {
            std::lock_guard<std::shared_mutex> lock(powerStatusMutex_);
            isScreenPoweringOff_.insert(id);
        }

        UpdateScreenPowerStatus(id, status);

        {
            std::lock_guard<std::shared_mutex> lock(powerStatusMutex_);
            isScreenPoweringOff_.erase(id);
        }

        std::unique_lock<std::mutex> taskLock(syncTaskMutex_);
        statusTaskEndFlag_ = true;
        statusTaskCV_.notify_all();
    });
}

void RSScreenManager::ResetScreenPowerStatusTask()
{
    if (!RSSystemProperties::IsSmallFoldDevice()) {
        return;
    }

    std::unique_lock<std::mutex> taskLock(syncTaskMutex_);
    statusTaskEndFlag_ = false;
}

void RSScreenManager::WaitScreenPowerStatusTask()
{
    if (!RSSystemProperties::IsSmallFoldDevice()) {
        return;
    }

    std::unique_lock<std::mutex> taskLock(syncTaskMutex_);
    if (!statusTaskCV_.wait_for(taskLock, std::chrono::milliseconds(WAIT_FOR_STATUS_TASK_TIMEOUT), [this]() {
        return statusTaskEndFlag_;
    })) {
        RS_LOGW("[UL_POWER] %{public}s: wait screen power status task timeout", __func__);
    }
}

bool RSScreenManager::IsScreenPoweringOn() const
{
    return isScreenPoweringOn_;
}

bool RSScreenManager::IsScreenPoweringOff(ScreenId id) const
{
    std::shared_lock<std::shared_mutex> lock(powerStatusMutex_);
    return isScreenPoweringOff_.count(id) != 0;
}

bool RSScreenManager::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }

    RS_LOGI("%{public}s: canvasRotation: %{public}d", __func__, canvasRotation);
    return screen->SetVirtualMirrorScreenCanvasRotation(canvasRotation);
}

bool RSScreenManager::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }

    RS_LOGI("%{public}s: scaleMode: %{public}d", __func__, scaleMode);
    return screen->SetVirtualMirrorScreenScaleMode(scaleMode);
}

void RSScreenManager::GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const
{
    GetScreenActiveMode(defaultScreenId_, screenModeInfo);
}

void RSScreenManager::ReleaseScreenDmaBuffer(ScreenId screenId)
{
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().PostTask([this, screenId]() {
        RS_TRACE_NAME("RSScreenManager ReleaseScreenDmaBuffer");
        auto output = GetOutput(screenId);
        if (output == nullptr) {
            RS_LOGE("ReleaseScreenDmaBuffer: HdiOutput is nullptr!");
            return;
        }
        std::vector<LayerInfoPtr> layer;
        output->SetLayerInfo(layer);
    });
#endif
}

/* only used for mock tests */
void RSScreenManager::MockHdiScreenConnected(std::shared_ptr<OHOS::Rosen::RSScreen> rsScreen)
{
    if (rsScreen == nullptr) {
        return;
    }
    screens_[rsScreen->Id()] = rsScreen;
}

RSScreenData RSScreenManager::GetScreenData(ScreenId id) const
{
    RSScreenData screenData;
    if (GetScreen(id) == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return screenData;
    }
    RSScreenCapability capability = GetScreenCapability(id);
    RSScreenModeInfo activeMode;
    GetScreenActiveMode(id, activeMode);
    std::vector<RSScreenModeInfo> supportModes = GetScreenSupportedModes(id);
    ScreenPowerStatus powerStatus = GetScreenPowerStatus(id);
    screenData.SetCapability(capability);
    screenData.SetActivityModeInfo(activeMode);
    screenData.SetSupportModeInfo(supportModes);
    screenData.SetPowerStatus(powerStatus);
    return screenData;
}

int32_t RSScreenManager::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("%{public}s: width %{public}" PRIu32 " or height %{public}" PRIu32 " has reached"
            " the maximum limit!", __func__, width, height);
        return INVALID_ARGUMENTS;
    }
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screen->ResizeVirtualScreen(width, height);
    RS_LOGI("%{public}s: resize virtual screen success, width:%{public}u, height:%{public}u",
        __func__, width, height);

    return SUCCESS;
}

int32_t RSScreenManager::GetScreenBacklight(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return INVALID_BACKLIGHT_VALUE;
    }
    return screen->GetScreenBacklight();
}

void RSScreenManager::SetScreenBacklight(ScreenId id, uint32_t level)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    screen->SetScreenBacklight(level);

    std::lock_guard<std::shared_mutex> lock(backLightAndCorrectionMutex_);
    if (screenBacklight_[id] == level) {
        RS_LOGD("%{public}s: repeat backlight screenId: %{public}" PRIu64 " newLevel: %d", __func__, id, level);
        return;
    }
    screenBacklight_[id] = level;
}

ScreenInfo RSScreenManager::QueryDefaultScreenInfo() const
{
    return QueryScreenInfo(defaultScreenId_);
}

ScreenInfo RSScreenManager::QueryScreenInfo(ScreenId id) const
{
    ScreenInfo info;
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return info;
    }

    info.id = id;
    info.width = screen->Width();
    info.height = screen->Height();
    info.phyWidth = screen->PhyWidth() ? screen->PhyWidth() : screen->Width();
    info.phyHeight = screen->PhyHeight() ? screen->PhyHeight() : screen->Height();
    info.isSamplingOn = screen->IsSamplingOn();
    info.samplingTranslateX = screen->GetSamplingTranslateX();
    info.samplingTranslateY = screen->GetSamplingTranslateY();
    info.samplingScale = screen->GetSamplingScale();
    auto ret = screen->GetScreenColorGamut(info.colorGamut);
    if (ret != StatusCode::SUCCESS) {
        info.colorGamut = COLOR_GAMUT_SRGB;
    }

    if (!screen->IsEnable()) {
        info.state = ScreenState::DISABLED;
    } else if (!screen->IsVirtual()) {
        info.state = ScreenState::HDI_OUTPUT_ENABLE;
    } else {
        info.state = ScreenState::SOFTWARE_OUTPUT_ENABLE;
    }
    info.skipFrameInterval = screen->GetScreenSkipFrameInterval();
    info.expectedRefreshRate = screen->GetScreenExpectedRefreshRate();
    info.skipFrameStrategy = screen->GetScreenSkipFrameStrategy();
    info.isEqualVsyncPeriod = screen->GetEqualVsyncPeriod();
    screen->GetPixelFormat(info.pixelFormat);
    screen->GetScreenHDRFormat(info.hdrFormat);
    info.whiteList = screen->GetWhiteList();
    info.enableVisibleRect = screen->GetEnableVisibleRect();
    info.activeRect = screen->GetActiveRect();
    info.maskRect = screen->GetMaskRect();
    info.reviseRect = screen->GetReviseRect();
    return info;
}

bool RSScreenManager::GetCanvasRotation(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }
    return screen->GetCanvasRotation();
}

ScreenScaleMode RSScreenManager::GetScaleMode(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return ScreenScaleMode::INVALID_MODE;
    }
    auto scaleModeDFX = static_cast<ScreenScaleMode>(
        RSSystemProperties::GetVirtualScreenScaleModeDFX());
    // Support mode can be configured for maintenance and testing before
    // upper layer application adaptation
    return (scaleModeDFX == ScreenScaleMode::INVALID_MODE) ? screen->GetScaleMode() : scaleModeDFX;
}

sptr<Surface> RSScreenManager::GetProducerSurface(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return nullptr;
    }
    return screen->GetProducerSurface();
}

std::shared_ptr<HdiOutput> RSScreenManager::GetOutput(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return nullptr;
    }
    return screen->GetOutput();
}

int32_t RSScreenManager::AddScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback)
{
    if (callback == nullptr) {
        RS_LOGE("%{public}s: callback is NULL.", __func__);
        return INVALID_ARGUMENTS;
    }

    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        // when the callback first registered, maybe there were some physical screens already connected,
        // so notify to remote immediately.
        for (const auto& [id, screen] : screens_) {
            if (screen == nullptr) {
                RS_LOGW("%{public}s: screen %{public}" PRIu64 " not found", __func__, id);
                continue;
            }
            if (!screen->IsVirtual()) {
                callback->OnScreenChanged(id, ScreenEvent::CONNECTED);
            }
        }
    }

    std::lock_guard<std::shared_mutex> lock(screenChangeCallbackMutex_);
    screenChangeCallbacks_.push_back(callback);
    RS_LOGI("%{public}s: add a remote callback succeed.", __func__);
    return SUCCESS;
}

void RSScreenManager::RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback)
{
    std::lock_guard<std::shared_mutex> lock(screenChangeCallbackMutex_);
    auto iter = std::find(screenChangeCallbacks_.cbegin(), screenChangeCallbacks_.cend(), callback);
    if (iter == screenChangeCallbacks_.cend()) {
        RS_LOGW("%{public}s: The remote callback is not in screenChangeCallbacks_", __func__);
        return;
    }
    screenChangeCallbacks_.erase(iter);
    RS_LOGI("%{public}s: remove a remote callback succeed.", __func__);
}

void RSScreenManager::DisplayDump(std::string& dumpString)
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    int32_t index = 0;
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->DisplayDump(index, dumpString);
        index++;
    }
    if (isFoldScreenFlag_) {
        dumpString += "===================\n";
        dumpString += "foldScreenIds_ size is " + std::to_string(foldScreenIds_.size()) + "\n";
        for (auto& [screenId, status] : foldScreenIds_) {
            dumpString += "foldScreenId:" + std::to_string(screenId) +
                ", isConnected:" + std::to_string(status.isConnected) +
                ", isPowerOn:" + std::to_string(status.isPowerOn) + "\n";
        }
    }
}

void RSScreenManager::SurfaceDump(std::string& dumpString)
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    int32_t index = 0;
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->SurfaceDump(index, dumpString);
        index++;
    }
}

void RSScreenManager::DumpCurrentFrameLayers()
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->DumpCurrentFrameLayers();
    }
}

void RSScreenManager::FpsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    int32_t index = 0;
    dumpString += "\n-- The recently fps records info of screens:\n";
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->FpsDump(index, dumpString, arg);
        index++;
    }
}

void RSScreenManager::ClearFpsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    int32_t index = 0;
    dumpString += "\n-- Clear fps records info of screens:\n";
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->ClearFpsDump(index, dumpString, arg);
        index++;
    }
}

void RSScreenManager::ClearFrameBufferIfNeed()
{
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().PostTask([this]() {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        for (const auto& [id, screen] : screens_) {
            if (!screen || !screen->GetOutput()) {
                RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
                continue;
            }
            if (screen->GetHasProtectedLayer()) {
                RS_TRACE_NAME_FMT("screen Id:%lu has protected layer.", id);
                continue;
            }
            if (screen->GetOutput()->GetBufferCacheSize() > 0) {
                RS_LOGI("%{public}s: screen %{public}" PRIu64 " ClearFrameBuffers.", __func__, id);
                RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
            }
        }
    });
#endif
}

int32_t RSScreenManager::SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type)
{
    frameId_++;
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_TRACE_NAME_FMT("SetScreenConstraint frameId:%lu timestamp:%lu type:%d", frameId_, timestamp, type);
    return screen->SetScreenConstraint(frameId_, timestamp, type);
}

void RSScreenManager::HitchsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    int32_t index = 0;
    dumpString += "\n-- The recently window hitchs records info of screens:\n";
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->HitchsDump(index, dumpString, arg);
        index++;
    }
}

int32_t RSScreenManager::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenSupportedColorGamuts(mode);
}

int32_t RSScreenManager::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenSupportedMetaDataKeys(keys);
}

int32_t RSScreenManager::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenColorGamut(mode);
}

int32_t RSScreenManager::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetScreenColorGamut(modeIdx);
}

int32_t RSScreenManager::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetScreenGamutMapLocked, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return screen->SetScreenGamutMap(mode);
}

int32_t RSScreenManager::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    screen->SetScreenCorrection(screenRotation);

    std::lock_guard<std::shared_mutex> lock(backLightAndCorrectionMutex_);
    screenCorrection_[id] = screenRotation;
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenGamutMap(mode);
}

int32_t RSScreenManager::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    GraphicHDRCapability hdrCapability = screen->GetHDRCapability();
    std::vector<ScreenHDRFormat> hdrFormats(hdrCapability.formats.size());
    std::transform(hdrCapability.formats.cbegin(), hdrCapability.formats.cend(), hdrFormats.begin(),
        [](const auto& node) { return static_cast<ScreenHDRFormat>(node); }
    );

    screenHdrCapability.SetMaxLum(hdrCapability.maxLum);
    screenHdrCapability.SetMaxAverageLum(hdrCapability.maxAverageLum);
    screenHdrCapability.SetMinLum(hdrCapability.minLum);
    screenHdrCapability.SetHdrFormats(std::move(hdrFormats));
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetScreenType(ScreenId id, RSScreenType& type) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    type = screen->GetScreenType();
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RSScreenModeInfo screenModeInfo;
    // use the refresh rate of the physical screen as the maximum refresh rate
    GetDefaultScreenActiveMode(screenModeInfo);
    // guaranteed screen refresh rate at least 1
    if (skipFrameInterval == 0 || (skipFrameInterval > screenModeInfo.GetScreenRefreshRate())) {
        RS_LOGE("%{public}s: screen %{public}" PRIu64 " is INVALID_ARGUMENTS.", __func__, id);
        return INVALID_ARGUMENTS;
    }
    screen->SetScreenSkipFrameInterval(skipFrameInterval);
    screen->SetEqualVsyncPeriod(skipFrameInterval == 1);
    RS_LOGI("%{public}s: screen(id %" PRIu64 "), skipFrameInterval(%d).",
        __func__, id, skipFrameInterval);
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    if (!screen->IsVirtual()) {
        RS_LOGW("%{public}s: Not Support for screen:%{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    if (maxRefreshRate == 0) {
        RS_LOGW("%{public}s: Invalid maxRefreshRate:%{public}u.", __func__, maxRefreshRate);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (maxRefreshRate > MAX_VIRTUAL_SCREEN_REFRESH_RATE) {
        maxRefreshRate = MAX_VIRTUAL_SCREEN_REFRESH_RATE;
    }
    screen->SetScreenExpectedRefreshRate(maxRefreshRate);
    RS_LOGI("%{public}s: screen(id %" PRIu64 "), maxRefreshRate(%d).",
        __func__, id, maxRefreshRate);
    actualRefreshRate = maxRefreshRate;
    return StatusCode::SUCCESS;
}

void RSScreenManager::SetEqualVsyncPeriod(ScreenId id, bool isEqualVsyncPeriod)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    screen->SetEqualVsyncPeriod(isEqualVsyncPeriod);
    return;
}

int32_t RSScreenManager::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }

    return screen->GetDisplayIdentificationData(outPort, edidData);
}

int32_t RSScreenManager::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenSupportedHDRFormats(hdrFormats);
}

int32_t RSScreenManager::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenHDRFormat(hdrFormat);
}

int32_t RSScreenManager::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetScreenHDRFormat(modeIdx);
}

int32_t RSScreenManager::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenSupportedColorSpaces(colorSpaces);
}

int32_t RSScreenManager::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->GetScreenColorSpace(colorSpace);
}

int32_t RSScreenManager::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetScreenColorSpace(colorSpace);
}

ScreenInfo RSScreenManager::GetActualScreenMaxResolution() const
{
    ScreenId maxScreenId = INVALID_SCREEN_ID;
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        uint32_t maxResolution = 0;
        for (const auto& [id, screen] : screens_) {
            if (!screen || screen->IsVirtual()) {
                RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
                continue;
            }
            uint32_t resolution = screen->PhyWidth() * screen->PhyHeight();
            if (resolution > maxResolution) {
                maxScreenId = id;
                maxResolution = resolution;
            }
        }
    }

    return QueryScreenInfo(maxScreenId);
}

void RSScreenManager::MarkPowerOffNeedProcessOneFrame()
{
    powerOffNeedProcessOneFrame_ = true;
}

void RSScreenManager::ResetPowerOffNeedProcessOneFrame()
{
    powerOffNeedProcessOneFrame_ = false;
}

bool RSScreenManager::GetPowerOffNeedProcessOneFrame() const
{
    return powerOffNeedProcessOneFrame_;
}

bool RSScreenManager::IsScreenPowerOff(ScreenId id) const
{
    std::shared_lock<std::shared_mutex> lock(powerStatusMutex_);
    if (screenPowerStatus_.count(id) == 0) {
        RS_LOGD("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
        return false;
    }
    return screenPowerStatus_.at(id) == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_SUSPEND ||
        screenPowerStatus_.at(id) == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF;
}

void RSScreenManager::DisablePowerOffRenderControl(ScreenId id)
{
    std::lock_guard<std::mutex> lock(renderControlMutex_);
    RS_LOGI("%{public}s: Add Screen_%{public}" PRIu64 " for disable power-off render control.", __func__, id);
    disableRenderControlScreens_.insert(id);
}

int RSScreenManager::GetDisableRenderControlScreensCount() const
{
    std::lock_guard<std::mutex> lock(renderControlMutex_);
    return disableRenderControlScreens_.size();
}

bool RSScreenManager::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }
    RS_LOGW("%{public}s: id %{public}" PRIu64 " status:%{public}d", __func__, id, screenStatus);
    return screen->SetVirtualScreenStatus(screenStatus);
}

VirtualScreenStatus RSScreenManager::GetVirtualScreenStatus(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS;
    }
    return screen->GetVirtualScreenStatus();
}

bool RSScreenManager::GetDisplayPropertyForHardCursor(uint32_t screenId)
{
    auto screen = GetScreen(screenId);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu32, __func__, screenId);
        return false;
    }
    return screen->GetDisplayPropertyForHardCursor();
}

void RSScreenManager::SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    screen->SetHasProtectedLayer(hasProtectedLayer);
}

void RSScreenManager::SetScreenSwitchStatus(bool flag)
{
    RS_LOGI("%{public}s: set isScreenSwitching_ = %{public}d", __func__, flag);
    isScreenSwitching_ = flag;
}

bool RSScreenManager::IsScreenSwitching() const
{
    return isScreenSwitching_;
}

int32_t RSScreenManager::SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix)
{
    auto task = [this, id, matrix]() -> void {
        auto screen = GetScreen(id);
        if (screen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            return;
        }
        screen->SetScreenLinearMatrix(matrix);
    };
    // SetScreenLinearMatrix is SMQ API, which can only be executed in RSHardwareThread.
    RSHardwareThread::Instance().PostTask(task);
    return StatusCode::SUCCESS;
}

bool RSScreenManager::IsVisibleRectSupportRotation(ScreenId id)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }

    return screen->GetVisibleRectSupportRotation();
}

bool RSScreenManager::AnyScreenFits(std::function<bool(const ScreenNode&)> func) const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    return std::any_of(screens_.cbegin(), screens_.cend(), func);
}

void RSScreenManager::TriggerCallbacks(ScreenId id, ScreenEvent event, ScreenChangeReason reason) const
{
    std::shared_lock<std::shared_mutex> lock(screenChangeCallbackMutex_);
    RS_LOGI("%{public}s: id %{public}" PRIu64
            "event %{public}u reason %{public}u screenChangeCallbacks_.size() %{public}zu",
            __func__, id, static_cast<uint8_t>(event), static_cast<uint8_t>(reason), screenChangeCallbacks_.size());
    for (const auto& cb : screenChangeCallbacks_) {
        cb->OnScreenChanged(id, event, reason);
    }
}

std::shared_ptr<OHOS::Rosen::RSScreen> RSScreenManager::GetScreen(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    auto iter = screens_.find(id);
    if (iter == screens_.end() || iter->second == nullptr) {
        return nullptr;
    }
    return iter->second;
}

void RSScreenManager::InitLoadOptParams(LoadOptParamsForScreen& loadOptParamsForScreen)
{
    loadOptParamsForScreen_ = loadOptParamsForScreen;
}
} // namespace impl

sptr<RSScreenManager> CreateOrGetScreenManager()
{
    return impl::RSScreenManager::GetInstance();
}
} // namespace Rosen
} // namespace OHOS
