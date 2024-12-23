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

#include "hgm_core.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_hardware_thread.h"
#include "platform/common/rs_log.h"
#include "vsync_sampler.h"
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "common/rs_optional_trace.h"
#include "rs_trace.h"
#include <mutex>

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
    constexpr uint32_t MAX_VIRTUAL_SCREEN_NUM = 64;
    constexpr uint32_t MAX_VIRTUAL_SCREEN_WIDTH = 65536;
    constexpr uint32_t MAX_VIRTUAL_SCREEN_HEIGHT = 65536;
    constexpr uint32_t MAX_VIRTUAL_SCREEN_REFRESH_RATE = 60;
    void SensorPostureDataCallback(SensorEvent *event)
    {
        OHOS::Rosen::CreateOrGetScreenManager()->HandlePostureData(event);
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

RSScreenManager::RSScreenManager()
{
}

RSScreenManager::~RSScreenManager() noexcept
{
}

bool RSScreenManager::Init() noexcept
{
    composer_ = HdiBackend::GetInstance();
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    isFoldScreenFlag_ = system::GetParameter("const.window.foldscreen.type", "") != "";
#endif
    if (composer_ == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to get composer.", __func__);
        return false;
    }

    if (composer_->RegScreenHotplug(&RSScreenManager::OnHotPlug, this) != 0) {
        RS_LOGE("RSScreenManager %{public}s: Failed to register OnHotPlug Func to composer.", __func__);
        return false;
    }

    if (composer_->RegScreenRefresh(&RSScreenManager::OnRefresh, this) != 0) {
        RS_LOGE("RSScreenManager %{public}s: Failed to register OnRefresh Func to composer.", __func__);
    }

    if (composer_->RegHwcDeadListener(&RSScreenManager::OnHwcDead, this) != 0) {
        RS_LOGE("RSScreenManager %{public}s: Failed to register OnHwcDead Func to composer.", __func__);
        return false;
    }

    if (composer_->RegScreenVBlankIdleCallback(&RSScreenManager::OnScreenVBlankIdle, this) != 0) {
        RS_LOGW("RSScreenManager %{public}s: Not support register OnScreenVBlankIdle Func to composer.", __func__);
    }

    // call ProcessScreenHotPlugEvents() for primary screen immediately in main thread.
    ProcessScreenHotPlugEvents();

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    if (isFoldScreenFlag_) {
        RS_LOGI("RSScreenManager %{public}s: FoldScreen need to RegisterSensorCallback.", __func__);
        RegisterSensorCallback();
    }
#endif
    RS_LOGI("RSScreenManager Init succeed");
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
        RS_LOGI("RSScreenManager RegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
        setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &user, POSTURE_INTERVAL, POSTURE_INTERVAL);
        RS_LOGI("RSScreenManager RegisterSensorCallback, setBatchRet: %{public}d", setBatchRet);
        activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
        RS_LOGI("RSScreenManager RegisterSensorCallback, activateRet: %{public}d", activateRet);
        if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
            RS_LOGE("RSScreenManager RegisterSensorCallback failed.");
            usleep(1000); // wait 1000 us for next try
            tryCnt++;
        }
    } while (tryCnt <= tryLimit && (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS ||
        activateRet != SENSOR_SUCCESS));
    if (tryCnt <= tryLimit) {
        RS_LOGI("RSScreenManager RegisterSensorCallback success.");
    }
}

void RSScreenManager::UnRegisterSensorCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        RS_LOGI("RSScreenManager UnRegisterSensorCallback success.");
    }
}

void RSScreenManager::HandlePostureData(const SensorEvent * const event)
{
    if (event == nullptr) {
        RS_LOGI("SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        RS_LOGI("SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        RS_LOGI("SensorEvent dataLen less than posture data size.");
        return;
    }
    PostureData *postureData = reinterpret_cast<PostureData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    float angle = (*postureData).angle;
    if (std::isless(angle, ANGLE_MIN_VAL) || std::isgreater(angle, ANGLE_MAX_VAL)) {
        RS_LOGD("Invalid angle value, angle is %{public}f.", angle);
        return;
    }
    RS_LOGD("angle vlaue in PostureData is: %{public}f.", angle);
    HandleSensorData(angle);
}

void RSScreenManager::HandleSensorData(float angle)
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    FoldState foldState = TransferAngleToScreenState(angle);
    if (foldState == FoldState::FOLDED) {
        RS_LOGI("RSScreenManager %{public}s: foldState == FoldState::FOLDED.", __func__);
        activeScreenId_ = externalScreenId_;
    } else {
        RS_LOGI("RSScreenManager %{public}s: foldState != FoldState::FOLDED.", __func__);
        activeScreenId_ = innerScreenId_;
    }
    isPostureSensorDataHandled_ = true;
    HgmCore::Instance().SetActiveScreenId(activeScreenId_);
    activeScreenIdAssignedCV_.notify_one();
}

FoldState RSScreenManager::TransferAngleToScreenState(float angle)
{
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        RS_LOGI("RSScreenManager %{public}s: angle isless ANGLE_MIN_VAL.", __func__);
        return FoldState::FOLDED;
    }
    if (std::isgreaterequal(angle, HALF_FOLDED_MAX_THRESHOLD)) {
        RS_LOGI("RSScreenManager %{public}s: angle isgreaterequal HALF_FOLDED_MAX_THRESHOLD.", __func__);
        return FoldState::EXPAND;
    }
    FoldState state;
    if (std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD)) {
        RS_LOGI("RSScreenManager %{public}s: angle islessequal OPEN_HALF_FOLDED_MIN_THRESHOLD.", __func__);
        state = FoldState::FOLDED;
    } else {
        RS_LOGI("RSScreenManager %{public}s: angle isgreater HALF_FOLDED_MAX_THRESHOLD.", __func__);
        state = FoldState::EXPAND;
    }
    return state;
}

ScreenId RSScreenManager::GetActiveScreenId()
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    if (!isFoldScreenFlag_) {
        RS_LOGI("RSScreenManager %{public}s: !isFoldScreenFlag_.", __func__);
        return INVALID_SCREEN_ID;
    }
    if (isPostureSensorDataHandled_) {
        isFirstTimeToGetActiveScreenId_ = false;
        UnRegisterSensorCallback();
        RS_LOGI("RSScreenManager activeScreenId: %{public}" PRIu64 " ", activeScreenId_);
        return activeScreenId_;
    }
    activeScreenIdAssignedCV_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT), [this]() {
            return isPostureSensorDataHandled_;
        });
    if (isFirstTimeToGetActiveScreenId_) {
        RS_LOGI("RSScreenManager %{public}s: isFirstTimeToGetActiveScreenId_.", __func__);
        isFirstTimeToGetActiveScreenId_ = false;
        UnRegisterSensorCallback();
    }
    RS_LOGI("RSScreenManager activeScreenId: %{public}" PRIu64 " ", activeScreenId_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenPowerStatus_.empty()) {
        RS_LOGE("RSScreenManager %{public}s: screenPowerStatus_ is empty.", __func__);
        return false;
    }
    for (const auto &[id, powerStatus] : screenPowerStatus_) {
        auto iter = screens_.find(id);
        if (iter != screens_.end() && iter->second->IsVirtual()) {
            continue;
        }
        // we also need to consider the AOD mode(POWER_STATUS_SUSPEND)
        if (powerStatus != ScreenPowerStatus::POWER_STATUS_OFF &&
            powerStatus != ScreenPowerStatus::POWER_STATUS_SUSPEND) {
            RS_LOGE("RSScreenManager %{public}s: powerStatus is not off or suspend.", __func__);
            return false;
        }
    }
    return true;
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
float RSScreenManager::GetScreenBrightnessNits(ScreenId id) const
{
    constexpr float DEFAULT_SCREEN_LIGHT_NITS = 500.0;
    constexpr float DEFAULT_SCREEN_LIGHT_MAX_NITS = 1200.0;
    constexpr int32_t DEFAULT_SCREEN_LIGHT_MAX_LEVEL = 255;

    float screenBrightnessNits = DEFAULT_SCREEN_LIGHT_NITS;

    RSScreenType screenType;
    if (GetScreenType(id, screenType) != SUCCESS) {
        RS_LOGW("RSScreenManager::GetScreenBrightnessNits GetScreenType fail.");
        return screenBrightnessNits;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        RS_LOGE("RSScreenManager %{public}s: screenType is VIRTUAL_TYPE_SCREEN.", __func__);
        return screenBrightnessNits;
    }

    int32_t backLightLevel = GetScreenBacklight(id);
    if (backLightLevel <= 0) {
        RS_LOGE("RSScreenManager %{public}s: backLightLevel <= zero.", __func__);
        return screenBrightnessNits;
    }

    return DEFAULT_SCREEN_LIGHT_MAX_NITS * backLightLevel / DEFAULT_SCREEN_LIGHT_MAX_LEVEL;
}
#endif

void RSScreenManager::ForceRefreshOneFrameIfNoRNV()
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr && !mainThread->IsRequestedNextVSync()) {
        RS_TRACE_NAME("No RNV, ForceRefreshOneFrame");
        mainThread->PostTask([mainThread]() {
            mainThread->SetDirtyFlag();
        });
        mainThread->RequestNextVSync();
    }
}

void RSScreenManager::OnHotPlug(std::shared_ptr<HdiOutput> &output, bool connected, void *data)
{
    if (output == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: output is nullptr.", __func__);
        return;
    }

    RSScreenManager *screenManager = nullptr;
    if (data != nullptr) {
        RS_LOGI("RSScreenManager %{public}s: data is not nullptr.", __func__);
        screenManager = static_cast<RSScreenManager *>(data);
    } else {
        RS_LOGI("RSScreenManager %{public}s: data is nullptr.", __func__);
        screenManager = static_cast<RSScreenManager *>(RSScreenManager::GetInstance().GetRefPtr());
    }

    if (screenManager == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }

    screenManager->OnHotPlugEvent(output, connected);
}

void RSScreenManager::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        ScreenId id = ToScreenId(output->GetScreenId());
        if (pendingHotPlugEvents_.find(id) != pendingHotPlugEvents_.end()) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 "is covered.", __func__, id);
        }
        pendingHotPlugEvents_[id] = ScreenHotPlugEvent{output, connected};
    }

    // This func would be called in main thread first time immediately after calling composer_->RegScreenHotplug,
    // but at this time the RSMainThread object would not be ready to handle this, so we need to call
    // ProcessScreenHotPlugEvents() after this func in RSScreenManager::Init().

    // Normally, this func would be called in hdi's hw-ipc threads(but sometimes in main thread, maybe),
    // so we should notify the RSMainThread to postTask to call ProcessScreenHotPlugEvents().
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        RS_LOGI("RSScreenManager %{public}s: mainThread is nullptr.", __func__);
        return;
    }
    mainThread->RequestNextVSync();
}

void RSScreenManager::OnRefresh(ScreenId id, void *data)
{
    RSScreenManager *screenManager = nullptr;
    if (data != nullptr) {
        screenManager = static_cast<RSScreenManager *>(data);
    } else {
        RS_LOGI("RSScreenManager %{public}s: data is nullptr.", __func__);
        screenManager = static_cast<RSScreenManager *>(RSScreenManager::GetInstance().GetRefPtr());
    }

    if (screenManager == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }
    screenManager->OnRefreshEvent(id);
}

void RSScreenManager::OnRefreshEvent(ScreenId id)
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        RS_LOGI("RSScreenManager %{public}s: mainThread is nullptr.", __func__);
        return;
    }
    mainThread->PostTask([mainThread]() {
        mainThread->SetForceUpdateUniRenderFlag(true);
        mainThread->RequestNextVSync();
    });
}

void RSScreenManager::OnHwcDead(void *data)
{
    RS_LOGW("RSScreenManager %{public}s: The composer_host is already dead.", __func__);
    RSScreenManager *screenManager = static_cast<RSScreenManager *>(RSScreenManager::GetInstance().GetRefPtr());
    if (screenManager == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }

    // Automatically recover when composer host dies.
    screenManager->CleanAndReinit();
}

void RSScreenManager::OnHwcDeadEvent()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &[id, screen] : screens_) {
        if (screen) {
            // In sceneboard, we should not notify the WMS to remove node from RSTree
            if (screen->IsVirtual()) {
                continue;
            } else {
#ifdef RS_ENABLE_GPU
                RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
#endif
            }
        }
    }
    isHwcDead_ = true;
    screens_.clear();
    defaultScreenId_ = INVALID_SCREEN_ID;
}

void RSScreenManager::OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void *data)
{
    CreateVSyncSampler()->StartSample(true);
    RSScreenManager *screenManager = static_cast<RSScreenManager *>(RSScreenManager::GetInstance().GetRefPtr());
    if (screenManager == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }
    screenManager->OnScreenVBlankIdleEvent(devId, ns);
}

void RSScreenManager::OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ScreenId screenId = ToScreenId(devId);
    if (screens_.count(screenId) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, screenId);
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
    RSScreenManager *screenManager = static_cast<RSScreenManager *>(RSScreenManager::GetInstance().GetRefPtr());
    if (screenManager == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: Failed to find RSScreenManager instance.", __func__);
        return;
    }

    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: Reinit failed, get RSMainThread failed.", __func__);
            return;
        }
        mainThread->PostTask([screenManager, this]() {
            screenManager->OnHwcDeadEvent();
            if (!composer_) {
                RS_LOGE("RSScreenManager %{public}s: Failed to get composer.", __func__);
                return;
            }
            composer_->ResetDevice();
            if (!screenManager->Init()) {
                RS_LOGE("RSScreenManager %{public}s: Reinit failed, screenManager init failed in mainThread.",
                    __func__);
                return;
            }
        });
    } else {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().PostTask([screenManager, this]() {
            RS_LOGW("RSScreenManager %{public}s: clean and reinit in hardware thread.", __func__);
            screenManager->OnHwcDeadEvent();
            if (!composer_) {
                RS_LOGE("RSScreenManager %{public}s: Failed to get composer.", __func__);
                return;
            }
            composer_->ResetDevice();
            if (!screenManager->Init()) {
                RS_LOGE("RSScreenManager %{public}s: Reinit failed, screenManager init failed in HardwareThread.",
                    __func__);
                return;
            }
        });
#endif
    }
}

bool RSScreenManager::TrySimpleProcessHotPlugEvents()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isHwcDead_ && pendingHotPlugEvents_.empty() && connectedIds_.empty()) {
        mipiCheckInFirstHotPlugEvent_ = true;
        return true;
    }
    return false;
}

void RSScreenManager::ProcessScreenHotPlugEvents()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [_, event] : pendingHotPlugEvents_) {
        if (event.connected) {
            ProcessScreenConnectedLocked(event.output);
            AddScreenToHgm(event.output);
        } else {
            ProcessScreenDisConnectedLocked(event.output);
            RemoveScreenFromHgm(event.output);
        }
    }
    for (auto id : connectedIds_) {
        for (auto &cb : screenChangeCallbacks_) {
            if (!isHwcDead_) {
                cb->OnScreenChanged(id, ScreenEvent::CONNECTED);
                continue;
            }
        }
        auto screenIt = screens_.find(id);
        if (screenIt == screens_.end() || screenIt->second == nullptr) {
            continue;
        }
        auto screenCorrectionIt = screenCorrection_.find(id);
        auto screenBacklightIt = screenBacklight_.find(id);
        auto screenPowerStatusIt = screenPowerStatus_.find(id);
        if (screenCorrectionIt != screenCorrection_.end()) {
            screenIt->second->SetScreenCorrection(screenCorrectionIt->second);
        }
        if (screenBacklightIt != screenBacklight_.end() && (screenPowerStatusIt == screenPowerStatus_.end() ||
            screenPowerStatusIt->second == ScreenPowerStatus::POWER_STATUS_ON)) {
            screenIt->second->SetScreenBacklight(screenBacklightIt->second);
            auto mainThread = RSMainThread::Instance();
            mainThread->PostTask([mainThread]() {
                mainThread->SetDirtyFlag();
            });
            mainThread->ForceRefreshForUni();
        }
    }
    isHwcDead_ = false;
    mipiCheckInFirstHotPlugEvent_ = true;
    pendingHotPlugEvents_.clear();
    connectedIds_.clear();
}

void RSScreenManager::AddScreenToHgm(std::shared_ptr<HdiOutput> &output)
{
    if (output == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: output is nullptr.", __func__);
        return;
    }
    RS_LOGI("RSScreenManager AddScreenToHgm");
    HgmTaskHandleThread::Instance().PostSyncTask([this, &output] () {
        auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
        ScreenId thisId = ToScreenId(output->GetScreenId());
        auto screensIt = screens_.find(thisId);
        if (screensIt == screens_.end()) {
            RS_LOGE("RSScreenManager invalid screen id, screen not found : %{public}" PRIu64 "", thisId);
            return;
        }

        int32_t initModeId = 0;
        if (screensIt->second == nullptr) {
            RS_LOGW("AddScreenToHgm:screen %{public}" PRIu64 " not found", thisId);
            return;
        }
        const auto &screen = screensIt->second;
        auto initMode = screen->GetActiveMode();
        if (!initMode) {
            RS_LOGE("RSScreenManager failed to get initial mode");
        } else {
            initModeId = initMode->id;
        }
        const auto &capability = screen->GetCapability();
        ScreenSize screenSize = {screen->Width(), screen->Height(), capability.phyWidth, capability.phyHeight};
        RS_LOGD_IF(DEBUG_SCREEN, "RSScreenManager add screen: w * h: [%{public}u * %{public}u], capability w * h: "
            "[%{public}u * %{public}u]", screen->Width(), screen->Height(), capability.phyWidth, capability.phyHeight);
        if (hgmCore.AddScreen(thisId, initModeId, screenSize)) {
            RS_LOGW("RSScreenManager failed to add screen : %{public}" PRIu64 "", thisId);
            return;
        }

        // for each supported mode, use the index as modeId to add the detailed mode to hgm
        int32_t modeId = 0;
        auto supportedModes = screen->GetSupportedModes();
        for (auto mode = supportedModes.begin(); mode != supportedModes.end(); ++mode) {
            if (hgmCore.AddScreenInfo(thisId, (*mode).width, (*mode).height,
                (*mode).freshRate, modeId)) {
                RS_LOGW("RSScreenManager failed to add a screen profile to the screen : %{public}" PRIu64 "", thisId);
            }
            modeId++;
        }
    });
}

void RSScreenManager::RemoveScreenFromHgm(std::shared_ptr<HdiOutput> &output)
{
    if (output == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: output is nullptr.", __func__);
        return;
    }

    RS_LOGI("RSScreenManager RemoveScreenFromHgm");
    HgmTaskHandleThread::Instance().PostTask([id = ToScreenId(output->GetScreenId())] () {
        auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
        RS_LOGD_IF(DEBUG_SCREEN, "RSScreenManager remove screen, id: %{public}" PRIu64 "", id);
        if (hgmCore.RemoveScreen(id)) {
            RS_LOGW("RSScreenManager failed to remove screen : %{public}" PRIu64 "", id);
        }
    });
}

void RSScreenManager::ProcessScreenConnectedLocked(std::shared_ptr<HdiOutput> &output)
{
    if (output == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: output is nullptr.", __func__);
        return;
    }

    bool isVirtual = false;
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGI("RSScreenManager %{public}s The screen for id %{public}" PRIu64 " connected.", __func__, id);

    auto it = screens_.find(id);
    if (it != screens_.end() && it->second != nullptr) {
        RS_LOGW("RSScreenManager %{public}s The screen for id %{public}" PRIu64 " already existed.", __func__, id);

        // [PLANNING]: should we erase it and create a new one?
        for (auto &cb : screenChangeCallbacks_) {
            cb->OnScreenChanged(id, ScreenEvent::DISCONNECTED);
        }
        screens_.erase(it);
        RS_LOGI("The screen for id %{public}" PRIu64 " already existed, remove the orignal one.", id);
    }

    screens_[id] = std::make_unique<RSScreen>(id, isVirtual, output, nullptr);

    if (screens_[id]->GetCapability().type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        if (!mipiCheckInFirstHotPlugEvent_) {
            defaultScreenId_ = id;
        }
        mipiCheckInFirstHotPlugEvent_ = true;
    } else if (defaultScreenId_ == INVALID_SCREEN_ID) {
        defaultScreenId_ = id;
    }

    ScreenId vsyncEnabledScreenId = id;
    if (RSSystemProperties::IsPcType() || RSSystemProperties::IsTabletType()) {
        vsyncEnabledScreenId = defaultScreenId_;
        if (defaultScreenId_ == id) {
            screens_[id]->SetScreenVsyncEnabled(true);
        }
        screens_[id]->SetDisplayPropertyForHardCursor();
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RegSetScreenVsyncEnabledCallbackForMainThread(vsyncEnabledScreenId);
    } else {
        RegSetScreenVsyncEnabledCallbackForHardwareThread(vsyncEnabledScreenId);
    }

    RS_LOGI("RSScreenManager %{public}s: A new screen(id %{public}" PRIu64 ") connected.", __func__, id);
    connectedIds_.emplace_back(id);
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    if (isFoldScreenFlag_ && id != 0) {
        externalScreenId_ = id;
    }
#endif
}

void RSScreenManager::ProcessScreenDisConnectedLocked(std::shared_ptr<HdiOutput> &output)
{
    ScreenId id = ToScreenId(output->GetScreenId());
    RS_LOGD_IF(DEBUG_SCREEN, "RSScreenManager process screen disconnected, id: %{public}" PRIu64 "", id);
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
    } else {
        for (auto &cb : screenChangeCallbacks_) {
            cb->OnScreenChanged(id, ScreenEvent::DISCONNECTED);
        }
        screens_.erase(screensIt);
        RS_LOGI("RSScreenManager %{public}s: Screen(id %{public}" PRIu64 ") disconnected.", __func__, id);
    }
    screenPowerStatus_.erase(id);
    screenBacklight_.erase(id);
    screenCorrection_.erase(id);
    if (id == defaultScreenId_) {
        HandleDefaultScreenDisConnectedLocked();
    }

    ScreenId vsyncEnabledScreenId = INVALID_SCREEN_ID;
    if (RSSystemProperties::IsPcType() || RSSystemProperties::IsTabletType()) {
        vsyncEnabledScreenId = defaultScreenId_;
    } else if (screens_.size() != 0) {
        vsyncEnabledScreenId = screens_.rbegin()->first;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RegSetScreenVsyncEnabledCallbackForMainThread(vsyncEnabledScreenId);
    } else {
        RegSetScreenVsyncEnabledCallbackForHardwareThread(vsyncEnabledScreenId);
    }
}

void RSScreenManager::RegSetScreenVsyncEnabledCallbackForMainThread(ScreenId vsyncEnabledScreenId)
{
    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("RegSetScreenVsyncEnabledCallbackForMainThread failed, vsyncSampler is null");
        return;
    }
    vsyncSampler->RegSetScreenVsyncEnabledCallback([this, vsyncEnabledScreenId](bool enabled) {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, get RSMainThread failed", enabled);
            return;
        }
        mainThread->PostTask([this, vsyncEnabledScreenId, enabled]() {
            std::lock_guard<std::mutex> lock(mutex_);
            auto screensIt = screens_.find(vsyncEnabledScreenId);
            if (screensIt == screens_.end() || screensIt->second == nullptr) {
                RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                enabled, vsyncEnabledScreenId);
                return;
            }
            screensIt->second->SetScreenVsyncEnabled(enabled);
        });
    });
}

void RSScreenManager::RegSetScreenVsyncEnabledCallbackForHardwareThread(ScreenId vsyncEnabledScreenId)
{
    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler == nullptr) {
        RS_LOGE("RegSetScreenVsyncEnabledCallbackForHardwareThread failed, vsyncSampler is null");
        return;
    }
#ifdef RS_ENABLE_GPU
    vsyncSampler->RegSetScreenVsyncEnabledCallback([this, vsyncEnabledScreenId](bool enabled) {
        RSHardwareThread::Instance().PostTask([this, vsyncEnabledScreenId, enabled]() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto screensIt = screens_.find(vsyncEnabledScreenId);
            if (screensIt == screens_.end() || screensIt->second == nullptr) {
                RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                    enabled, vsyncEnabledScreenId);
                return;
            }
            screensIt->second->SetScreenVsyncEnabled(enabled);
        });
    });
#endif
}

// If the previous primary screen disconnected, we traversal the left screens
// and find the first physical screen to be the default screen.
// If there was no physical screen left, we set the first screen as default, no matter what type it is.
// At last, if no screen left, we set Default Screen Id to INVALID_SCREEN_ID.
void RSScreenManager::HandleDefaultScreenDisConnectedLocked()
{
    defaultScreenId_ = INVALID_SCREEN_ID;
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGW("HandleDefaultScreenDisConnectedLocked:screen %{public}" PRIu64 " not found", id);
            continue;
        }
        if (!screen->IsVirtual()) {
            defaultScreenId_ = id;
            break;
        }
    }

    if (defaultScreenId_ == INVALID_SCREEN_ID) {
        if (!screens_.empty()) {
            defaultScreenId_ = screens_.cbegin()->first;
        }
    }
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
    std::lock_guard<std::mutex> lock(mutex_);
    defaultScreenId_ = id;
}

void RSScreenManager::SetScreenMirror(ScreenId id, ScreenId toMirror)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screensIt->second->SetMirror(toMirror);
}

ScreenId RSScreenManager::GenerateVirtualScreenIdLocked()
{
    if (!freeVirtualScreenIds_.empty()) {
        ScreenId id = freeVirtualScreenIds_.front();
        freeVirtualScreenIds_.pop();
        RS_LOGI("RSScreenManager %{public}s: VirtualScreenId is %{public}" PRIu64 " .", __func__, id);
        return id;
    }

    RS_LOGI("RSScreenManager %{public}s: freeVirtualScreenIds_ is empty.", __func__);
    // The left 32 bits is for virtual screen id.
    return (static_cast<ScreenId>(virtualScreenCount_++) << 32) | 0xffffffffu;
}

void RSScreenManager::ReuseVirtualScreenIdLocked(ScreenId id)
{
    freeVirtualScreenIds_.push(id);
}

void RSScreenManager::GetVirtualScreenResolutionLocked(ScreenId id,
    RSVirtualScreenResolution& virtualScreenResolution) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    const auto& screen = screensIt->second;
    virtualScreenResolution.SetVirtualScreenWidth(static_cast<uint32_t>(screen->Width()));
    virtualScreenResolution.SetVirtualScreenHeight(static_cast<uint32_t>(screen->Height()));
}

void RSScreenManager::GetScreenActiveModeLocked(ScreenId id, RSScreenModeInfo& screenModeInfo) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    const auto& screen = screensIt->second;
    auto modeInfo = screen->GetActiveMode();
    if (!modeInfo) {
        RS_LOGE("RSScreenManager %{public}s: Failed to get active mode for screen %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screenModeInfo.SetScreenWidth(modeInfo->width);
    screenModeInfo.SetScreenHeight(modeInfo->height);
    screenModeInfo.SetScreenRefreshRate(modeInfo->freshRate);
    screenModeInfo.SetScreenModeId(screen->GetActiveModePosByModeId(modeInfo->id));
}

std::vector<RSScreenModeInfo> RSScreenManager::GetScreenSupportedModesLocked(ScreenId id) const
{
    std::vector<RSScreenModeInfo> screenSupportedModes;
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return screenSupportedModes;
    }

    const auto& displaySupportedModes = screensIt->second->GetSupportedModes();
    screenSupportedModes.resize(displaySupportedModes.size());
    for (decltype(displaySupportedModes.size()) idx = 0; idx < displaySupportedModes.size(); ++idx) {
        screenSupportedModes[idx].SetScreenWidth(displaySupportedModes[idx].width);
        screenSupportedModes[idx].SetScreenHeight(displaySupportedModes[idx].height);
        screenSupportedModes[idx].SetScreenRefreshRate(displaySupportedModes[idx].freshRate);
        screenSupportedModes[idx].SetScreenModeId(displaySupportedModes[idx].id);
    }
    return screenSupportedModes;
}

RSScreenCapability RSScreenManager::GetScreenCapabilityLocked(ScreenId id) const
{
    RSScreenCapability screenCapability;
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return screenCapability;
    }
    const auto& screen = screensIt->second;
    if (screen->IsVirtual()) {
        RS_LOGW("RSScreenManager %{public}s: only name attribute is valid for virtual screen.", __func__);
        screenCapability.SetName(screen->Name());
        return screenCapability;
    }

    const auto& capability = screen->GetCapability();
    std::vector<RSScreenProps> props;
    uint32_t propCount = capability.propertyCount;
    props.resize(propCount);
    for (uint32_t propIndex = 0; propIndex < propCount; propIndex++) {
        props[propIndex] = RSScreenProps(capability.props[propIndex].name, capability.props[propIndex].propId,
            capability.props[propIndex].value);
    }
    screenCapability.SetName(capability.name);
    screenCapability.SetType(static_cast<ScreenInterfaceType>(capability.type));
    screenCapability.SetPhyWidth(capability.phyWidth);
    screenCapability.SetPhyHeight(capability.phyHeight);
    screenCapability.SetSupportLayers(capability.supportLayers);
    screenCapability.SetVirtualDispCount(capability.virtualDispCount);
    screenCapability.SetSupportWriteBack(capability.supportWriteBack);
    screenCapability.SetProps(props);
    return screenCapability;
}

ScreenPowerStatus RSScreenManager::GetScreenPowerStatusLocked(ScreenId id) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return INVALID_POWER_STATUS;
    }

    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(screensIt->second->GetPowerStatus());
    return status;
}

ScreenRotation RSScreenManager::GetScreenCorrectionLocked(ScreenId id) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return ScreenRotation::INVALID_SCREEN_ROTATION;
    }

    ScreenRotation screenRotation = screensIt->second->GetScreenCorrection();
    return screenRotation;
}

std::vector<ScreenId> RSScreenManager::GetAllScreenIds() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ScreenId> ids;
    for (auto iter = screens_.begin(); iter != screens_.end(); ++iter) {
        ids.emplace_back(iter->first);
    }
    return ids;
}

ScreenId RSScreenManager::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (currentVirtualScreenNum_ >= MAX_VIRTUAL_SCREEN_NUM) {
        RS_LOGW("RSScreenManager %{public}s: virtual screens num %{public}" PRIu32" has reached the maximum limit!",
            __func__, currentVirtualScreenNum_);
        return INVALID_SCREEN_ID;
    }
    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("RSScreenManager %{public}s: width %{public}" PRIu32" or height %{public}" PRIu32" has reached"
            " the maximum limit!", __func__, width, height);
        return INVALID_SCREEN_ID;
    }
    if (surface != nullptr) {
        uint64_t surfaceId = surface->GetUniqueId();
        for (auto &[_, screen] : screens_) {
            if (screen == nullptr || !screen->IsVirtual()) {
                continue;
            }
            auto screenSurface = screen->GetProducerSurface();
            if (screenSurface == nullptr) {
                continue;
            }
            if (screenSurface->GetUniqueId() == surfaceId) {
                RS_LOGW("RSScreenManager %{public}s: surface %{public}" PRIu64 " is used, create virtual"
                    " screen failed!", __func__, surfaceId);
                return INVALID_SCREEN_ID;
            }
        }
    } else {
        RS_LOGD("RSScreenManager %{public}s: surface is nullptr.", __func__);
    }

    VirtualScreenConfigs configs;
    ScreenId newId = GenerateVirtualScreenIdLocked();
    configs.id = newId;
    configs.mirrorId = mirrorId;
    configs.name = name;
    configs.width = width;
    configs.height = height;
    configs.surface = surface;
    configs.flags = flags;
    configs.whiteList = std::unordered_set<NodeId>(whiteList.begin(), whiteList.end());

    screens_[newId] = std::make_unique<RSScreen>(configs);
    ++currentVirtualScreenNum_;
    RS_LOGD("RSScreenManager %{public}s: create virtual screen(id %{public}" PRIu64 ").", __func__, newId);
    return newId;
}

int32_t RSScreenManager::SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_set<NodeId> screenBlackList(blackList.begin(), blackList.end());
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("RSScreenManager %{public}s: Cast screen blacklists for id %{public}" PRIu64 ".", __func__, id);
        castScreenBlackLists_ = screenBlackList;
        return SUCCESS;
    }
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    } else {
        if (virtualScreen->second != nullptr) {
            RS_LOGI("RSScreenManager %{public}s: Record screen blacklists for id %{public}" PRIu64 ".", __func__, id);
            virtualScreen->second->SetBlackList(screenBlackList);
        } else {
            RS_LOGW("RSScreenManager %{public}s: BlackLists are null for id %{public}" PRIu64 ".", __func__, id);
            return SCREEN_NOT_FOUND;
        }
    }
    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = screens_.find(mainId);
        if (mainScreen == screens_.end()) {
            RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, mainId);
            return SCREEN_NOT_FOUND;
        } else {
            if (mainScreen->second != nullptr) {
                mainScreen->second->SetBlackList(screenBlackList);
            } else {
                RS_LOGW("RSScreenManager %{public}s: BlackLists are null for id %{public}" PRIu64 ".",
                    __func__, mainId);
                return SCREEN_NOT_FOUND;
            }
        }
    }
    return SUCCESS;
}

int32_t RSScreenManager::AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("RSScreenManager %{public}s: Cast screen blacklists", __func__);
        for (auto& list : blackList) {
            castScreenBlackLists_.emplace(list);
        }
        return SUCCESS;
    }
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end() || virtualScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("RSScreenManager %{public}s: Record screen blacklists for id %{public}" PRIu64 ".", __func__, id);
    virtualScreen->second->AddBlackList(blackList);

    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = screens_.find(mainId);
        if (mainScreen == screens_.end() || mainScreen->second == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, mainId);
            return SCREEN_NOT_FOUND;
        }
        mainScreen->second->AddBlackList(blackList);
    }
    return SUCCESS;
}

int32_t RSScreenManager::RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (id == INVALID_SCREEN_ID) {
        RS_LOGI("RSScreenManager %{public}s: Cast screen blacklists", __func__);
        for (auto& list : blackList) {
            auto it = castScreenBlackLists_.find(list);
            if (it == castScreenBlackLists_.end()) {
                continue;
            }
            castScreenBlackLists_.erase(it);
        }
        return SUCCESS;
    }
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end() || virtualScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("RSScreenManager %{public}s: Record screen blacklists for id %{public}" PRIu64 ".", __func__, id);
    virtualScreen->second->RemoveBlackList(blackList);

    ScreenId mainId = GetDefaultScreenId();
    if (mainId != id) {
        auto mainScreen = screens_.find(mainId);
        if (mainScreen == screens_.end() || mainScreen->second == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, mainId);
            return SCREEN_NOT_FOUND;
        }
        mainScreen->second->RemoveBlackList(blackList);
    }
    return SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<uint64_t>& securityExemptionList)
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGD("RSScreenManager %{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }

    if (virtualScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    if (!(virtualScreen->second->IsVirtual())) {
        RS_LOGW("RSScreenManager %{public}s: not virtual screen for id %{public}" PRIu64 ".", __func__, id);
        return INVALID_ARGUMENTS;
    }
    virtualScreen->second->SetSecurityExemptionList(securityExemptionList);
    for (const auto& exemption : securityExemptionList) {
        RS_LOGD("RSScreenManager %{public}s: virtual screen(id %{public}" PRIu64 "), nodeId %{public}" PRIu64 ".",
            __func__, id, exemption);
    }
    return SUCCESS;
}

const std::vector<uint64_t> RSScreenManager::GetVirtualScreenSecurityExemptionList(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return {};
    }

    if (virtualScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
        return {};
    }
    return virtualScreen->second->GetSecurityExemptionList();
}

int32_t RSScreenManager::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect)
{
    if (id == INVALID_SCREEN_ID) {
        RS_LOGD("RSScreenManager %{public}s: INVALID_SCREEN_ID.", __func__);
        return INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto mirrorScreen = screens_.find(id);
    if (mirrorScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }

    if (mirrorScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    mirrorScreen->second->SetEnableVisibleRect(true);
    mirrorScreen->second->SetMainScreenVisibleRect(mainScreenRect);
    RS_LOGD("RSScreenManager %{public}s: mirror screen(id %{public}" PRIu64 "), "
        "visible rect[%{public}d, %{public}d, %{public}d, %{public}d].",
        __func__, id, mainScreenRect.x, mainScreenRect.y, mainScreenRect.w, mainScreenRect.h);
    return SUCCESS;
}

Rect RSScreenManager::GetMirrorScreenVisibleRect(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto mirrorScreen = screens_.find(id);
    if (mirrorScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return {};
    }

    if (mirrorScreen->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
        return {};
    }
    return mirrorScreen->second->GetMainScreenVisibleRect();
}

const std::unordered_set<NodeId> RSScreenManager::GetVirtualScreenBlackList(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return {};
    }
    if (GetCastScreenEnableSkipWindow(id)) {
        RS_LOGD("RSScreenManager %{public}s: Cast screen blacklists for id %{public}" PRIu64 ".", __func__, id);
        return castScreenBlackLists_;
    }
    if (virtualScreen->second != nullptr) {
        RS_LOGD("RSScreenManager %{public}s: Record screen blacklists for id %{public}" PRIu64 ".", __func__, id);
        return virtualScreen->second->GetBlackList();
    }
    RS_LOGD("RSScreenManager %{public}s: BlackLists are null for id %{public}" PRIu64 ".", __func__, id);
    return {};
}

int32_t RSScreenManager::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    } else {
        if (virtualScreen->second != nullptr) {
            virtualScreen->second->SetCastScreenEnableSkipWindow(enable);
            RS_LOGW("RSScreenManager %{public}s: screen id %{public}" PRIu64 " set %{public}d success.",
                __func__, id, enable);
            return SUCCESS;
        } else {
            RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
            return SCREEN_NOT_FOUND;
        }
    }
}

bool RSScreenManager::GetCastScreenEnableSkipWindow(ScreenId id) const
{
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    } else {
        if (virtualScreen->second != nullptr) {
            return virtualScreen->second->GetCastScreenEnableSkipWindow();
        } else {
            RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
            return false;
        }
    }
}

int32_t RSScreenManager::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr || surface == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: screen not found.", __func__);
        return SCREEN_NOT_FOUND;
    }
    uint64_t surfaceId = surface->GetUniqueId();
    for (auto &[screenId, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found", __func__, screenId);
            continue;
        }
        if (!screen->IsVirtual() || screenId == id) {
            RS_LOGW("RSScreenManager %{public}s: screen %{public}" PRIu64 " is not virtual or is the same screen.",
                __func__, screenId);
            continue;
        }
        auto screenSurface = screen->GetProducerSurface();
        if (screenSurface == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: screen %{public}" PRIu64 " is nullptr.",
                __func__, screenId);
            continue;
        }
        if (screenSurface->GetUniqueId() == surface->GetUniqueId()) {
            RS_LOGE("RSScreenManager %{public}s: surface %{public}" PRIu64 " is used, set surface failed!",
                __func__, surfaceId);
            return SURFACE_NOT_UNIQUE;
        }
    }
    screensIt->second->SetProducerSurface(surface);
    RS_LOGD("RSScreenManager %{public}s: set virtual screen surface success!", __func__);
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetVirtualScreenSurface, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

bool RSScreenManager::GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto virtualScreen = screens_.find(id);
    if (virtualScreen == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }
    if (virtualScreen->second != nullptr) {
        RS_LOGI("RSScreenManager %{public}s: screen %{public}" PRIu64 "get and reset virtualSurface.", __func__, id);
        return virtualScreen->second->GetAndResetVirtualSurfaceUpdateFlag();
    } else {
        RS_LOGW("RSScreenManager %{public}s: Null screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }
}

void RSScreenManager::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    RemoveVirtualScreenLocked(id);
}

void RSScreenManager::RemoveVirtualScreenLocked(ScreenId id)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end()) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screens_.erase(screensIt);
    --currentVirtualScreenNum_;

    // Update other screens' mirrorId.
    for (auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGW("RemoveVirtualScreenLocked:screen %{public}" PRIu64 " not found", id);
            continue;
        }
        if (screen->MirrorId() == id) {
            screen->SetMirror(INVALID_SCREEN_ID);
        }
    }
    RS_LOGD("RSScreenManager %{public}s: remove virtual screen(id %{public}" PRIu64 ").", __func__, id);

    ReuseVirtualScreenIdLocked(id);

    disableRenderControlScreens_.erase(id);
}

void RSScreenManager::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    screensIt->second->SetActiveMode(modeId);
}

uint32_t RSScreenManager::SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    return screensIt->second->SetScreenActiveRect(activeRect);
}

int32_t RSScreenManager::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("RSScreenManager %{public}s: width %{public}" PRIu32" or height %{public}" PRIu32" has reached"
            " the maximum limit!", __func__, width, height);
        return INVALID_ARGUMENTS;
    }
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screensIt->second->SetResolution(width, height);
    RS_LOGD("RSScreenManager %{public}s: set virtual screen resolution success", __func__);
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetVirtualScreenResolution, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

int32_t RSScreenManager::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screensIt->second->SetRogResolution(width, height);
    return SUCCESS;
}

void RSScreenManager::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("[UL_POWER]RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    screensIt->second->SetPowerStatus(static_cast<uint32_t>(status));

    /*
     * If app adds the first frame when power on the screen, delete the code
     */
    if (status == ScreenPowerStatus::POWER_STATUS_ON ||
        status == ScreenPowerStatus::POWER_STATUS_ON_ADVANCED) {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            return;
        }
        mainThread->PostTask([mainThread]() {
            mainThread->SetDirtyFlag();
            mainThread->SetScreenPowerOnChanged(true);
        });
        if (screenPowerStatus_.count(id) == 0 ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF_FAKE ||
            screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED) {
            mainThread->ForceRefreshForUni();
        } else {
            mainThread->RequestNextVSync();
        }

        RS_LOGD("[UL_POWER]RSScreenManager %{public}s: PowerStatus %{public}d, request a frame", __func__, status);
    }
    screenPowerStatus_[id] = status;
}

bool RSScreenManager::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }

    RS_LOGD("RSScreenManager %{public}s: canvasRotation: %{public}d", __func__, canvasRotation);

    return screensIt->second->SetVirtualMirrorScreenCanvasRotation(canvasRotation);
}

bool RSScreenManager::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }

    RS_LOGD("RSScreenManager %{public}s: scaleMode: %{public}d", __func__, scaleMode);

    return screensIt->second->SetVirtualMirrorScreenScaleMode(scaleMode);
}

void RSScreenManager::GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    GetVirtualScreenResolutionLocked(id, virtualScreenResolution);
}

void RSScreenManager::GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    GetScreenActiveModeLocked(id, screenModeInfo);
}

void RSScreenManager::GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    GetScreenActiveModeLocked(defaultScreenId_, screenModeInfo);
}

void RSScreenManager::ReleaseScreenDmaBuffer(uint64_t screenId)
{
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().PostTask([screenId]() {
        RS_TRACE_NAME("RSScreenManager ReleaseScreenDmaBuffer");
        auto screenManager = CreateOrGetScreenManager();
        if (screenManager == nullptr) {
            RS_LOGE("RSScreenManager::ReleaseScreenDmaBuffer RSScreenManager is nullptr!");
            return;
        }
        auto output = screenManager->GetOutput(screenId);
        if (output == nullptr) {
            RS_LOGE("RSScreenManager::ReleaseScreenDmaBuffer HdiOutput is nullptr!");
            return;
        }
        std::vector<LayerInfoPtr> layer;
        output->SetLayerInfo(layer);
    });
#endif
}

std::vector<RSScreenModeInfo> RSScreenManager::GetScreenSupportedModes(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return GetScreenSupportedModesLocked(id);
}

RSScreenCapability RSScreenManager::GetScreenCapability(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return GetScreenCapabilityLocked(id);
}

ScreenPowerStatus RSScreenManager::GetScreenPowerStatus(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return GetScreenPowerStatusLocked(id);
}

ScreenRotation RSScreenManager::GetScreenCorrection(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return GetScreenCorrectionLocked(id);
}

RSScreenData RSScreenManager::GetScreenData(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    RSScreenData screenData;
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return screenData;
    }
    RSScreenCapability capability = GetScreenCapabilityLocked(id);
    RSScreenModeInfo activeMode;
    GetScreenActiveModeLocked(id, activeMode);
    std::vector<RSScreenModeInfo> supportModes = GetScreenSupportedModesLocked(id);
    ScreenPowerStatus powerStatus = GetScreenPowerStatusLocked(id);
    screenData.SetCapability(capability);
    screenData.SetActivityModeInfo(activeMode);
    screenData.SetSupportModeInfo(supportModes);
    screenData.SetPowerStatus(powerStatus);
    return screenData;
}

int32_t RSScreenManager::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (width > MAX_VIRTUAL_SCREEN_WIDTH || height > MAX_VIRTUAL_SCREEN_HEIGHT) {
        RS_LOGW("RSScreenManager %{public}s: width %{public}" PRIu32" or height %{public}" PRIu32" has reached"
            " the maximum limit!", __func__, width, height);
        return INVALID_ARGUMENTS;
    }
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screensIt->second->ResizeVirtualScreen(width, height);
    RS_LOGI("RSScreenManager %{public}s: resize virtual screen success, width:%{public}u, height:%{public}u",
        __func__, width, height);

    return SUCCESS;
}

int32_t RSScreenManager::GetScreenBacklight(ScreenId id) const
{
    return GetScreenBacklightLocked(id);
}

int32_t RSScreenManager::GetScreenBacklightLocked(ScreenId id) const
{
    std::shared_ptr<OHOS::Rosen::RSScreen> screen = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto screensIt = screens_.find(id);
        if (screensIt == screens_.end() || screensIt->second == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
            return INVALID_BACKLIGHT_VALUE;
        }
        screen = screensIt->second;
    }
    int32_t level = screen->GetScreenBacklight();
    return level;
}

void RSScreenManager::SetScreenBacklight(ScreenId id, uint32_t level)
{
    std::shared_ptr<OHOS::Rosen::RSScreen> screen = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto screensIt = screens_.find(id);
        if (screensIt == screens_.end() || screensIt->second == nullptr) {
            RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
            return;
        }
        screenBacklight_[id] = level;
        screen = screensIt->second;
    }
    screen->SetScreenBacklight(level);
}

ScreenInfo RSScreenManager::QueryDefaultScreenInfo() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QueryScreenInfoLocked(defaultScreenId_);
}

ScreenInfo RSScreenManager::QueryScreenInfo(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QueryScreenInfoLocked(id);
}

ScreenInfo RSScreenManager::QueryScreenInfoLocked(ScreenId id) const
{
    ScreenInfo info;
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end()) {
        RS_LOGD("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return info;
    }

    const auto &screen = screensIt->second;
    if (!screen) {
        RS_LOGE("RSScreenManager::QueryScreenInfo screen %{public}" PRIu64 " has no info.", id);
        return info;
    }
    info.id = id;
    info.width = screen->Width();
    info.height = screen->Height();
    info.phyWidth = screen->PhyWidth() ? screen->PhyWidth() : screen->Width();
    info.phyHeight = screen->PhyHeight() ? screen->PhyHeight() : screen->Height();
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
    screen->GetPixelFormat(info.pixelFormat);
    screen->GetScreenHDRFormat(info.hdrFormat);
    info.whiteList = screen->GetWhiteList();
    info.enableVisibleRect = screen->GetEnableVisibleRect();
    info.activeRect = screen->GetActiveRect();
    return info;
}

bool RSScreenManager::GetCanvasRotation(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGD("RSScreenManager::GetCanvasRotation: There is no screen for id %{public}" PRIu64 ".", id);
        return false;
    }
    return screensIt->second->GetCanvasRotation();
}

ScreenScaleMode RSScreenManager::GetScaleMode(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGD("RSScreenManager::GetScaleMode: There is no screen for id %{public}" PRIu64 ".", id);
        return ScreenScaleMode::INVALID_MODE;
    }
    auto scaleModeDFX = static_cast<ScreenScaleMode>(
        RSSystemProperties::GetVirtualScreenScaleModeDFX());
    // Support mode can be configured for maintenance and testing before
    // upper layer application adaptation
    const auto& scaleMode = (scaleModeDFX == ScreenScaleMode::INVALID_MODE) ?
        screensIt->second->GetScaleMode() : scaleModeDFX;
    return scaleMode;
}

sptr<Surface> RSScreenManager::GetProducerSurface(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGD("RSScreenManager::GetProducerSurface: There is no screen for id %{public}" PRIu64 ".", id);
        return nullptr;
    }
    return screensIt->second->GetProducerSurface();
}

std::shared_ptr<HdiOutput> RSScreenManager::GetOutput(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager::GetOutput: There is no screen for id %{public}" PRIu64 ".", id);
        return nullptr;
    }
    return screensIt->second->GetOutput();
}

int32_t RSScreenManager::AddScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback)
{
    if (callback == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: callback is NULL.", __func__);
        return INVALID_ARGUMENTS;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    // when the callback first registered, maybe there were some physical screens already connected,
    // so notify to remote immediately.
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGW("AddScreenChangeCallback:screen %{public}" PRIu64 " not found", id);
            continue;
        }
        if (!screen->IsVirtual()) {
            callback->OnScreenChanged(id, ScreenEvent::CONNECTED);
        }
    }
    screenChangeCallbacks_.push_back(callback);
    RS_LOGD("RSScreenManager %{public}s: add a remote callback succeed.", __func__);
    return SUCCESS;
}

void RSScreenManager::RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = screenChangeCallbacks_.begin(); it != screenChangeCallbacks_.end(); it++) {
        if (*it == callback) {
            screenChangeCallbacks_.erase(it);
            RS_LOGD("RSScreenManager %{public}s: remove a remote callback succeed.", __func__);
            break;
        }
    }
}

void RSScreenManager::DisplayDump(std::string& dumpString)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t index = 0;
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->DisplayDump(index, dumpString);
        index++;
    }
}

void RSScreenManager::SurfaceDump(std::string& dumpString)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t index = 0;
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->SurfaceDump(index, dumpString);
        index++;
    }
}

void RSScreenManager::FpsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t index = 0;
    dumpString += "\n-- The recently fps records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->FpsDump(index, dumpString, arg);
        index++;
    }
}

void RSScreenManager::ClearFpsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t index = 0;
    dumpString += "\n-- Clear fps records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
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
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, screen] : screens_) {
            if (!screen || !screen->GetOutput()) {
                RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
                continue;
            }
            if (screen->GetOutput()->GetBufferCacheSize() > 0) {
                RS_LOGI("RSScreenManager %{public}s: screen %{public}" PRIu64 " ClearFrameBuffers.", __func__, id);
                RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
            }
        }
    });
#endif
}

int32_t RSScreenManager::SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type)
{
    frameId_++;
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_TRACE_NAME_FMT("SetScreenConstraint frameId:%lu timestamp:%lu type:%d", frameId_, timestamp, type);
    return screensIt->second->SetScreenConstraint(frameId_, timestamp, type);
}

void RSScreenManager::HitchsDump(std::string& dumpString, std::string& arg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t index = 0;
    dumpString += "\n-- The recently window hitchs records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        screen->HitchsDump(index, dumpString, arg);
        index++;
    }
}

int32_t RSScreenManager::GetScreenSupportedColorGamutsLocked(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenSupportedColorGamuts(mode);
}

int32_t RSScreenManager::GetScreenSupportedMetaDataKeysLocked(
    ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenSupportedMetaDataKeys(keys);
}

int32_t RSScreenManager::GetScreenColorGamutLocked(ScreenId id, ScreenColorGamut& mode) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenColorGamut(mode);
}

int32_t RSScreenManager::SetScreenColorGamutLocked(ScreenId id, int32_t modeIdx)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->SetScreenColorGamut(modeIdx);
}

int32_t RSScreenManager::SetScreenGamutMapLocked(ScreenId id, ScreenGamutMap mode)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetScreenGamutMapLocked, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return screensIt->second->SetScreenGamutMap(mode);
}

int32_t RSScreenManager::SetScreenCorrectionLocked(ScreenId id, ScreenRotation screenRotation)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    screensIt->second->SetScreenCorrection(screenRotation);
    screenCorrection_[id] = screenRotation;
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetScreenGamutMapLocked(ScreenId id, ScreenGamutMap &mode) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenGamutMap(mode);
}

int32_t RSScreenManager::GetScreenHDRCapabilityLocked(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    GraphicHDRCapability hdrCapability = screensIt->second->GetHDRCapability();
    std::vector<ScreenHDRFormat> hdrFormats;
    uint32_t formatCount = hdrCapability.formatCount;
    hdrFormats.resize(formatCount);
    for (uint32_t index = 0; index < formatCount; index++) {
        hdrFormats[index] = static_cast<ScreenHDRFormat>(hdrCapability.formats[index]);
    }
    screenHdrCapability.SetMaxLum(hdrCapability.maxLum);
    screenHdrCapability.SetMaxAverageLum(hdrCapability.maxAverageLum);
    screenHdrCapability.SetMinLum(hdrCapability.minLum);
    screenHdrCapability.SetHdrFormats(hdrFormats);
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetScreenTypeLocked(ScreenId id, RSScreenType& type) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGD("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    type = screensIt->second->GetScreenType();
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetScreenSkipFrameIntervalLocked(ScreenId id, uint32_t skipFrameInterval)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RSScreenModeInfo screenModeInfo;
    // use the refresh rate of the physical screen as the maximum refresh rate
    GetScreenActiveModeLocked(defaultScreenId_, screenModeInfo);
    // guaranteed screen refresh rate at least 1
    if (skipFrameInterval == 0 || (skipFrameInterval > screenModeInfo.GetScreenRefreshRate())) {
        RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " is INVALID_ARGUMENTS.", __func__, id);
        return INVALID_ARGUMENTS;
    }
    screensIt->second->SetScreenSkipFrameInterval(skipFrameInterval);
    RS_LOGD("RSScreenManager %{public}s: screen(id %" PRIu64 "), skipFrameInterval(%d).",
        __func__, id, skipFrameInterval);
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    if (!screensIt->second->IsVirtual()) {
        RS_LOGW("RSScreenManager %{public}s: Not Support for screen:%{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    if (maxRefreshRate == 0) {
        RS_LOGW("RSScreenManager %{public}s: Invalid maxRefreshRate:%{public}u.", __func__, maxRefreshRate);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (maxRefreshRate > MAX_VIRTUAL_SCREEN_REFRESH_RATE) {
        maxRefreshRate = MAX_VIRTUAL_SCREEN_REFRESH_RATE;
    }
    while (MAX_VIRTUAL_SCREEN_REFRESH_RATE % maxRefreshRate != 0) { // maxRefreshRate is greater than 0
        maxRefreshRate--;
    }
    screensIt->second->SetScreenExpectedRefreshRate(maxRefreshRate);
    RS_LOGI("RSScreenManager %{public}s: screen(id %" PRIu64 "), maxRefreshRate(%d).",
        __func__, id, maxRefreshRate);
    actualRefreshRate = maxRefreshRate;
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetPixelFormatLocked(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::SetPixelFormatLocked(ScreenId id, GraphicPixelFormat pixelFormat)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->SetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::GetScreenSupportedHDRFormatsLocked(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenSupportedHDRFormats(hdrFormats);
}

int32_t RSScreenManager::GetScreenHDRFormatLocked(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenHDRFormat(hdrFormat);
}

int32_t RSScreenManager::SetScreenHDRFormatLocked(ScreenId id, int32_t modeIdx)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->SetScreenHDRFormat(modeIdx);
}

int32_t RSScreenManager::GetScreenSupportedColorSpacesLocked(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenSupportedColorSpaces(colorSpaces);
}

int32_t RSScreenManager::GetScreenColorSpaceLocked(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->GetScreenColorSpace(colorSpace);
}

int32_t RSScreenManager::SetScreenColorSpaceLocked(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screensIt->second->SetScreenColorSpace(colorSpace);
}

int32_t RSScreenManager::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenSupportedColorGamutsLocked(id, mode);
}

int32_t RSScreenManager::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenSupportedMetaDataKeysLocked(id, keys);
}

uint32_t RSScreenManager::GetActualScreensNum() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t num = 0;
    for (const auto &[id, screen] : screens_) {
        if (!screen) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        if (!screen->IsVirtual()) {
            num += 1;
        } else {
            if (screen->GetProducerSurface()) {
                num += 1;
            }
        }
    }
    return num;
}

ScreenInfo RSScreenManager::GetActualScreenMaxResolution() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t maxResolution = 0;
    ScreenId maxScreenId = INVALID_SCREEN_ID;
    for (const auto &[id, screen] : screens_) {
        if (!screen || screen->IsVirtual()) {
            RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        uint32_t resolution = screen->PhyWidth() * screen->PhyHeight();
        if (resolution > maxResolution) {
            maxScreenId = id;
            maxResolution = resolution;
        }
    }
    return QueryScreenInfoLocked(maxScreenId);
}

int32_t RSScreenManager::GetScreenColorGamut(ScreenId id, ScreenColorGamut &mode) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenColorGamutLocked(id, mode);
}

int32_t RSScreenManager::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenColorGamutLocked(id, modeIdx);
}

int32_t RSScreenManager::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenGamutMapLocked(id, mode);
}

int32_t RSScreenManager::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenCorrectionLocked(id, screenRotation);
}

int32_t RSScreenManager::GetScreenGamutMap(ScreenId id, ScreenGamutMap &mode) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenGamutMapLocked(id, mode);
}

int32_t RSScreenManager::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenHDRCapabilityLocked(id, screenHdrCapability);
}

int32_t RSScreenManager::GetScreenType(ScreenId id, RSScreenType& type) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenTypeLocked(id, type);
}

int32_t RSScreenManager::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenSkipFrameIntervalLocked(id, skipFrameInterval);
}

int32_t RSScreenManager::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetPixelFormatLocked(id, pixelFormat);
}

int32_t RSScreenManager::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetPixelFormatLocked(id, pixelFormat);
}

int32_t RSScreenManager::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenSupportedHDRFormatsLocked(id, hdrFormats);
}

int32_t RSScreenManager::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenHDRFormatLocked(id, hdrFormat);
}

int32_t RSScreenManager::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenHDRFormatLocked(id, modeIdx);
}

int32_t RSScreenManager::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenSupportedColorSpacesLocked(id, colorSpaces);
}

int32_t RSScreenManager::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenColorSpaceLocked(id, colorSpace);
}

int32_t RSScreenManager::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return SetScreenColorSpaceLocked(id, colorSpace);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenPowerStatus_.count(id) == 0) {
        RS_LOGE("RSScreenManager %{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
        return false;
    }
    return screenPowerStatus_.at(id) == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_SUSPEND ||
        screenPowerStatus_.at(id) == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF;
}

void RSScreenManager::DisablePowerOffRenderControl(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_LOGD("RSScreenManager Add Screen_%{public}" PRIu64 " for disable power-off render control.", id);
    disableRenderControlScreens_.insert(id);
}

int RSScreenManager::GetDisableRenderControlScreensCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return disableRenderControlScreens_.size();
}

bool RSScreenManager::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }
    return screensIt->second->SetVirtualScreenStatus(screenStatus);
}

VirtualScreenStatus RSScreenManager::GetVirtualScreenStatus(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto screensIt = screens_.find(id);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS;
    }
    return screensIt->second->GetVirtualScreenStatus();
}

bool RSScreenManager::GetDisplayPropertyForHardCursor(uint32_t screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto screensIt = screens_.find(screenId);
    if (screensIt == screens_.end() || screensIt->second == nullptr) {
        RS_LOGD("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu32 ".", __func__, screenId);
        return false;
    }
    return screensIt->second->GetDisplayPropertyForHardCursor();
}
} // namespace impl

sptr<RSScreenManager> CreateOrGetScreenManager()
{
    return impl::RSScreenManager::GetInstance();
}
} // namespace Rosen
} // namespace OHOS
