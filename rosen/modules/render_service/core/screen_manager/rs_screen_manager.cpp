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

    if (composer_->RegHwcDeadListener(&RSScreenManager::OnHwcDead, this) != 0) {
        RS_LOGE("RSScreenManager %{public}s: Failed to register OnHwcDead Func to composer.", __func__);
        return false;
    }

    // call ProcessScreenHotPlugEvents() for primary screen immediately in main thread.
    ProcessScreenHotPlugEvents();

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    if (isFoldScreenFlag_) {
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
    const int tryLimit = 5; // 5 times failure limit
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
        activeScreenId_ = externalScreenId_;
    } else {
        activeScreenId_ = innerScreenId_;
    }
    isPostureSensorDataHandled_ = true;
    HgmCore::Instance().SetActiveScreenId(activeScreenId_);
    activeScreenIdAssignedCV_.notify_one();
}

FoldState RSScreenManager::TransferAngleToScreenState(float angle)
{
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        return FoldState::FOLDED;
    }
    if (std::isgreaterequal(angle, HALF_FOLDED_MAX_THRESHOLD)) {
        return FoldState::EXPAND;
    }
    FoldState state;
    if (std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD)) {
        state = FoldState::FOLDED;
    } else {
        state = FoldState::EXPAND;
    }
    return state;
}

ScreenId RSScreenManager::GetActiveScreenId()
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    if (!isFoldScreenFlag_) {
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
            return false;
        }
    }
    return true;
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
float RSScreenManager::GetScreenBrightnessNits(ScreenId id)
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
        return screenBrightnessNits;
    }

    int32_t backLightLevel = GetScreenBacklight(id);
    if (backLightLevel <= 0) {
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
        mainThread->ForceRefreshForUni();
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
        screenManager = static_cast<RSScreenManager *>(data);
    } else {
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
        pendingHotPlugEvents_.emplace_back(ScreenHotPlugEvent{output, connected});
    }

    // This func would be called in main thread first time immediately after calling composer_->RegScreenHotplug,
    // but at this time the RSMainThread object would not be ready to handle this, so we need to call
    // ProcessScreenHotPlugEvents() after this func in RSScreenManager::Init().

    // Normally, this func would be called in hdi's hw-ipc threads(but sometimes in main thread, maybe),
    // so we should notify the RSMainThread to postTask to call ProcessScreenHotPlugEvents().
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        return;
    }
    mainThread->RequestNextVSync();
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
                RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
            }
        }
    }
    isHwcDead_ = true;
    screens_.clear();
    defaultScreenId_ = INVALID_SCREEN_ID;
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
            composer_->ResetDevice();
            if (!screenManager->Init()) {
                RS_LOGE("RSScreenManager %{public}s: Reinit failed, screenManager init failed in mainThread.",
                    __func__);
                return;
            }
        });
    } else {
        RSHardwareThread::Instance().PostTask([screenManager, this]() {
            RS_LOGW("RSScreenManager %{public}s: clean and reinit in hardware thread.", __func__);
            screenManager->OnHwcDeadEvent();
            composer_->ResetDevice();
            if (!screenManager->Init()) {
                RS_LOGE("RSScreenManager %{public}s: Reinit failed, screenManager init failed in HardwareThread.",
                    __func__);
                return;
            }
        });
    }
}

void RSScreenManager::ProcessScreenHotPlugEvents()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &event : pendingHotPlugEvents_) {
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
            if (screens_.count(id) != 0 && screenBacklight_.count(id) != 0 &&
                (screenPowerStatus_.count(id) == 0 || screenPowerStatus_[id] == ScreenPowerStatus::POWER_STATUS_ON)) {
                screens_[id]->SetScreenBacklight(screenBacklight_[id]);
                auto mainThread = RSMainThread::Instance();
                mainThread->PostTask([mainThread]() {
                    mainThread->SetDirtyFlag();
                });
                mainThread->ForceRefreshForUni();
            }
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
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    ScreenId thisId = ToScreenId(output->GetScreenId());
    if (screens_.find(thisId) == screens_.end()) {
        RS_LOGE("RSScreenManager invalid screen id, screen not found : %{public}" PRIu64 "", thisId);
        return;
    }

    int32_t initModeId = 0;
    auto initMode = screens_[thisId]->GetActiveMode();
    if (!initMode) {
        RS_LOGE("RSScreenManager failed to get initial mode");
    } else {
        initModeId = initMode->id;
    }
    const auto &screen = screens_.at(thisId);
    const auto &capability = screens_.at(thisId)->GetCapability();
    ScreenSize screenSize = {screen->Width(), screen->Height(), capability.phyWidth, capability.phyHeight};
    if (hgmCore.AddScreen(thisId, initModeId, screenSize)) {
        RS_LOGW("RSScreenManager failed to add screen : %{public}" PRIu64 "", thisId);
        return;
    }
    hgmCore.SetActiveScreenId(thisId);

    // for each supported mode, use the index as modeId to add the detailed mode to hgm
    int32_t modeId = 0;
    auto supportedModes = screens_[thisId]->GetSupportedModes();
    for (auto mode = supportedModes.begin(); mode != supportedModes.end(); ++mode) {
        if (hgmCore.AddScreenInfo(thisId, (*mode).width, (*mode).height,
            (*mode).freshRate, modeId)) {
            RS_LOGW("RSScreenManager failed to add a screen profile to the screen : %{public}" PRIu64 "", thisId);
        }
        modeId++;
    }
}

void RSScreenManager::RemoveScreenFromHgm(std::shared_ptr<HdiOutput> &output)
{
    if (output == nullptr) {
        RS_LOGE("RSScreenManager %{public}s: output is nullptr.", __func__);
        return;
    }

    RS_LOGI("RSScreenManager RemoveScreenFromHgm");
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    ScreenId id = ToScreenId(output->GetScreenId());
    if (hgmCore.RemoveScreen(id)) {
        RS_LOGW("RSScreenManager failed to remove screen : %{public}" PRIu64 "", id);
    }
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

    if (screens_.count(id) == 1) {
        RS_LOGW("RSScreenManager %{public}s The screen for id %{public}" PRIu64 " already existed.", __func__, id);

        // [PLANNING]: should we erase it and create a new one?
        for (auto &cb : screenChangeCallbacks_) {
            cb->OnScreenChanged(id, ScreenEvent::DISCONNECTED);
        }
        screens_.erase(id);
        RS_LOGI("The screen for id %{public}" PRIu64 " already existed, remove the orignal one.", id);
    }

    screens_[id] = std::make_unique<RSScreen>(id, isVirtual, output, nullptr);

    auto vsyncSampler = CreateVSyncSampler();
    if (vsyncSampler != nullptr) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            vsyncSampler->RegSetScreenVsyncEnabledCallback([this, id](bool enabled) {
                auto mainThread = RSMainThread::Instance();
                if (mainThread == nullptr) {
                    RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, get RSMainThread failed", enabled);
                    return;
                }
                mainThread->PostTask([this, id, enabled]() {
                    if (screens_[id] == nullptr) {
                        RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                            enabled, id);
                        return;
                    }
                    screens_[id]->SetScreenVsyncEnabled(enabled);
                });
            });
        } else {
            vsyncSampler->RegSetScreenVsyncEnabledCallback([this, id](bool enabled) {
                RSHardwareThread::Instance().PostTask([this, id, enabled]() {
                    if (screens_[id] == nullptr) {
                        RS_LOGE("SetScreenVsyncEnabled:%{public}d failed, screen %{public}" PRIu64 " not found",
                            enabled, id);
                        return;
                    }
                    screens_[id]->SetScreenVsyncEnabled(enabled);
                });
            });
        }
    } else {
        RS_LOGE("RegSetScreenVsyncEnabledCallback failed, vsyncSampler is null");
    }

    if (screens_[id]->GetCapability().type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        if (!mipiCheckInFirstHotPlugEvent_) {
            defaultScreenId_ = id;
        }
        mipiCheckInFirstHotPlugEvent_ = true;
    } else if (defaultScreenId_ == INVALID_SCREEN_ID) {
        defaultScreenId_ = id;
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
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
    } else {
        for (auto &cb : screenChangeCallbacks_) {
            cb->OnScreenChanged(id, ScreenEvent::DISCONNECTED);
        }
        screens_.erase(id);
        RS_LOGI("RSScreenManager %{public}s: Screen(id %{public}" PRIu64 ") disconnected.", __func__, id);
    }
    if (screenPowerStatus_.count(id) != 0) {
        screenPowerStatus_.erase(id);
    }
    if (screenBacklight_.count(id) != 0) {
        screenBacklight_.erase(id);
    }
    if (id == defaultScreenId_) {
        HandleDefaultScreenDisConnectedLocked();
    }
}

// If the previous primary screen disconnected, we traversal the left screens
// and find the first physical screen to be the default screen.
// If there was no physical screen left, we set the first screen as default, no matter what type it is.
// At last, if no screen left, we set Default Screen Id to INVALID_SCREEN_ID.
void RSScreenManager::HandleDefaultScreenDisConnectedLocked()
{
    defaultScreenId_ = INVALID_SCREEN_ID;
    for (const auto &[id, screen] : screens_) {
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

    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screens_[id]->SetMirror(toMirror);
}

ScreenId RSScreenManager::GenerateVirtualScreenIdLocked()
{
    if (!freeVirtualScreenIds_.empty()) {
        ScreenId id = freeVirtualScreenIds_.front();
        freeVirtualScreenIds_.pop();
        return id;
    }

    // The left 32 bits is for virtual screen id.
    return (static_cast<ScreenId>(maxVirtualScreenNum_++) << 32) | 0xffffffffu;
}

void RSScreenManager::ReuseVirtualScreenIdLocked(ScreenId id)
{
    freeVirtualScreenIds_.push(id);
}

void RSScreenManager::GetVirtualScreenResolutionLocked(ScreenId id,
    RSVirtualScreenResolution& virtualScreenResolution) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    virtualScreenResolution.SetVirtualScreenWidth(static_cast<uint32_t>(screens_.at(id)->Width()));
    virtualScreenResolution.SetVirtualScreenHeight(static_cast<uint32_t>(screens_.at(id)->Height()));
}

void RSScreenManager::GetScreenActiveModeLocked(ScreenId id, RSScreenModeInfo& screenModeInfo) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    auto modeInfo = screens_.at(id)->GetActiveMode();
    if (!modeInfo) {
        RS_LOGE("RSScreenManager %{public}s: Failed to get active mode for screen %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screenModeInfo.SetScreenWidth(modeInfo->width);
    screenModeInfo.SetScreenHeight(modeInfo->height);
    screenModeInfo.SetScreenRefreshRate(modeInfo->freshRate);
    screenModeInfo.SetScreenModeId(screens_.at(id)->GetActiveModePosByModeId(modeInfo->id));
}

std::vector<RSScreenModeInfo> RSScreenManager::GetScreenSupportedModesLocked(ScreenId id) const
{
    std::vector<RSScreenModeInfo> screenSupportedModes;
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return screenSupportedModes;
    }

    const auto& displaySupportedModes = screens_.at(id)->GetSupportedModes();
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
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return screenCapability;
    }
    if (screens_.at(id)->IsVirtual()) {
        RS_LOGW("RSScreenManager %{public}s: only name attribute is valid for virtual screen.", __func__);
        screenCapability.SetName(screens_.at(id)->Name());
        return screenCapability;
    }

    const auto& capability = screens_.at(id)->GetCapability();
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
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return INVALID_POWER_STATUS;
    }

    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(screens_.at(id)->GetPowerStatus());
    return status;
}

ScreenRotation RSScreenManager::GetScreenCorrectionLocked(ScreenId id) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return ScreenRotation::INVALID_SCREEN_ROTATION;
    }

    ScreenRotation screenRotation = screens_.at(id)->GetScreenCorrection();
    return screenRotation;
}

std::vector<ScreenId> RSScreenManager::GetAllScreenIds()
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
    std::vector<NodeId> filteredAppVector)
{
    std::lock_guard<std::mutex> lock(mutex_);

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

    std::unordered_set<NodeId> filteredAppSet(filteredAppVector.begin(), filteredAppVector.end());
    VirtualScreenConfigs configs;
    ScreenId newId = GenerateVirtualScreenIdLocked();
    configs.id = newId;
    configs.mirrorId = mirrorId;
    configs.name = name;
    configs.width = width;
    configs.height = height;
    configs.surface = surface;
    configs.flags = flags;
    configs.filteredAppSet= filteredAppSet;

    screens_[newId] = std::make_unique<RSScreen>(configs);
    RS_LOGD("RSScreenManager %{public}s: create virtual screen(id %{public}" PRIu64 ").", __func__, newId);
    return newId;
}

int32_t RSScreenManager::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (screens_.find(id) == screens_.end()) {
        return SCREEN_NOT_FOUND;
    }
    uint64_t surfaceId = surface->GetUniqueId();
    for (auto &[screenId, screen] : screens_) {
        if (!screen->IsVirtual() || screenId == id) {
            continue;
        }
        auto screenSurface = screen->GetProducerSurface();
        if (screenSurface == nullptr) {
            continue;
        }
        if (screenSurface->GetUniqueId() == surface->GetUniqueId()) {
            RS_LOGE("RSScreenManager %{public}s: surface %{public}" PRIu64 " is used, set surface failed!",
                __func__, surfaceId);
            return SURFACE_NOT_UNIQUE;
        }
    }
    screens_.at(id)->SetProducerSurface(surface);
    RS_LOGD("RSScreenManager %{public}s: set virtual screen surface success!", __func__);
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetVirtualScreenSurface, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

void RSScreenManager::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    RemoveVirtualScreenLocked(id);
}

void RSScreenManager::RemoveVirtualScreenLocked(ScreenId id)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }

    screens_.erase(id);

    // Update other screens' mirrorId.
    for (auto &[id, screen] : screens_) {
        if (screen->MirrorId() == id) {
            screen->SetMirror(INVALID_SCREEN_ID);
        }
    }
    RS_LOGD("RSScreenManager %{public}s: remove virtual screen(id %{public}" PRIu64 ").", __func__, id);

    ReuseVirtualScreenIdLocked(id);
}

void RSScreenManager::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    screens_.at(id)->SetActiveMode(modeId);
}

int32_t RSScreenManager::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screens_.at(id)->SetResolution(width, height);
    RS_LOGD("RSScreenManager %{public}s: set virtual screen resolution success", __func__);
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetVirtualScreenResolution, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return SUCCESS;
}

int32_t RSScreenManager::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screens_.at(id)->SetRogResolution(width, height);
    return SUCCESS;
}

void RSScreenManager::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("[UL_POWER]RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    screens_.at(id)->SetPowerStatus(static_cast<uint32_t>(status));

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

    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }

    RS_LOGD("RSScreenManager %{public}s: canvasRotation: %{public}d", __func__, canvasRotation);
    
    return screens_.at(id)->SetVirtualMirrorScreenCanvasRotation(canvasRotation);
}

bool RSScreenManager::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return false;
    }

    RS_LOGD("RSScreenManager %{public}s: scaleMode: %{public}d", __func__, scaleMode);
    
    return screens_.at(id)->SetVirtualMirrorScreenScaleMode(scaleMode);
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
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return SCREEN_NOT_FOUND;
    }
    screens_.at(id)->ResizeVirtualScreen(width, height);
    RS_LOGI("RSScreenManager %{public}s: resize virtual screen success, width:%{public}u, height:%{public}u",
        __func__, width, height);

    return SUCCESS;
}

int32_t RSScreenManager::GetScreenBacklight(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetScreenBacklightLocked(id);
}

int32_t RSScreenManager::GetScreenBacklightLocked(ScreenId id) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return INVALID_BACKLIGHT_VALUE;
    }

    int32_t level = screens_.at(id)->GetScreenBacklight();
    return level;
}

void RSScreenManager::SetScreenBacklight(ScreenId id, uint32_t level)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return;
    }
    screenBacklight_[id] = level;
    screens_.at(id)->SetScreenBacklight(level);
}

ScreenInfo RSScreenManager::QueryScreenInfo(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    ScreenInfo info;
    if (screens_.count(id) == 0) {
        RS_LOGD("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return info;
    }

    const auto &screen = screens_.at(id);
    if (!screen) {
        RS_LOGE("RSScreenManager::QueryScreenInfo screen %{public}" PRIu64 " has no info.", id);
        return info;
    }
    info.id = id;
    info.width = screen->Width();
    info.height = screen->Height();
    info.phyWidth = screen->PhyWidth();
    info.phyHeight = screen->PhyHeight();
    auto ret = screen->GetScreenColorGamut(info.colorGamut);
    if (ret != StatusCode::SUCCESS) {
        info.colorGamut = COLOR_GAMUT_SRGB;
    }

    if (!screen->IsEnable()) {
        info.state = ScreenState::DISABLED;
    } else if (!screen->IsVirtual()) {
        info.state = ScreenState::HDI_OUTPUT_ENABLE;
    } else {
        info.state = ScreenState::PRODUCER_SURFACE_ENABLE;
    }
    info.skipFrameInterval = screen->GetScreenSkipFrameInterval();
    screen->GetPixelFormat(info.pixelFormat);
    screen->GetScreenHDRFormat(info.hdrFormat);
    info.filteredAppSet = screen->GetFilteredAppSet();
    return info;
}

bool RSScreenManager::GetCanvasRotation(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("RSScreenManager::GetCanvasRotation: There is no screen for id %{public}" PRIu64 ".", id);
        return false;
    }
    return screens_.at(id)->GetCanvasRotation();
}

ScreenScaleMode RSScreenManager::GetScaleMode(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("RSScreenManager::GetScaleMode: There is no screen for id %{public}" PRIu64 ".", id);
        return ScreenScaleMode::INVALID_MODE;
    }
    auto scaleModeDFX = static_cast<ScreenScaleMode>(
        RSSystemProperties::GetVirtualScreenScaleModeDFX());
    // Support mode can be configured for maintenance and testing before
    // upper layer application adaptation
    const auto& scaleMode = (scaleModeDFX == ScreenScaleMode::INVALID_MODE) ?
        screens_.at(id)->GetScaleMode() : scaleModeDFX;
    return scaleMode;
}

sptr<Surface> RSScreenManager::GetProducerSurface(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    // assert screens_.count(id) == 1
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager::GetProducerSurface: There is no screen for id %{public}" PRIu64 ".", id);
        return nullptr;
    }
    return screens_.at(id)->GetProducerSurface();
}

std::shared_ptr<HdiOutput> RSScreenManager::GetOutput(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    // assert screens_.count(id) == 1
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager::GetOutput: There is no screen for id %{public}" PRIu64 ".", id);
        return nullptr;
    }
    return screens_.at(id)->GetOutput();
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
    int32_t index = 0;
    for (const auto &[id, screen] : screens_) {
        if (screen == nullptr) {
            return;
        }
        screen->DisplayDump(index, dumpString);
        index++;
    }
}

void RSScreenManager::SurfaceDump(std::string& dumpString)
{
    int32_t index = 0;
    for (const auto &[id, screen] : screens_) {
        screen->SurfaceDump(index, dumpString);
        index++;
    }
}

void RSScreenManager::FpsDump(std::string& dumpString, std::string& arg)
{
    int32_t index = 0;
    dumpString += "\n-- The recently fps records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        screen->FpsDump(index, dumpString, arg);
        index++;
    }
}

void RSScreenManager::ClearFpsDump(std::string& dumpString, std::string& arg)
{
    int32_t index = 0;
    dumpString += "\n-- Clear fps records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        screen->ClearFpsDump(index, dumpString, arg);
        index++;
    }
}

void RSScreenManager::ClearFrameBufferIfNeed()
{
    RSHardwareThread::Instance().PostTask([this]() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, screen] : screens_) {
            if (!screen) {
                continue;
            }
            if (screen->GetOutput()->GetBufferCacheSize() > 0) {
                RSHardwareThread::Instance().ClearFrameBuffers(screen->GetOutput());
            }
        }
    });
}

void RSScreenManager::HitchsDump(std::string& dumpString, std::string& arg)
{
    int32_t index = 0;
    dumpString += "\n-- The recently window hitchs records info of screens:\n";
    for (const auto &[id, screen] : screens_) {
        screen->HitchsDump(index, dumpString, arg);
        index++;
    }
}

int32_t RSScreenManager::GetScreenSupportedColorGamutsLocked(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenSupportedColorGamuts(mode);
}

int32_t RSScreenManager::GetScreenSupportedMetaDataKeysLocked(
    ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenSupportedMetaDataKeys(keys);
}

int32_t RSScreenManager::GetScreenColorGamutLocked(ScreenId id, ScreenColorGamut& mode) const
{
    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenColorGamut(mode);
}

int32_t RSScreenManager::SetScreenColorGamutLocked(ScreenId id, int32_t modeIdx)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->SetScreenColorGamut(modeIdx);
}

int32_t RSScreenManager::SetScreenGamutMapLocked(ScreenId id, ScreenGamutMap mode)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME("RSScreenManager::SetScreenGamutMapLocked, ForceRefreshOneFrame.");
    ForceRefreshOneFrame();
    return screens_.at(id)->SetScreenGamutMap(mode);
}

int32_t RSScreenManager::SetScreenCorrectionLocked(ScreenId id, ScreenRotation screenRotation)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    screens_.at(id)->SetScreenCorrection(screenRotation);
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetScreenGamutMapLocked(ScreenId id, ScreenGamutMap &mode) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenGamutMap(mode);
}

int32_t RSScreenManager::GetScreenHDRCapabilityLocked(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    GraphicHDRCapability hdrCapability = screens_.at(id)->GetHDRCapability();
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
    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGD("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    type = screens_.at(id)->GetScreenType();
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::SetScreenSkipFrameIntervalLocked(ScreenId id, uint32_t skipFrameInterval)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RSScreenModeInfo screenModeInfo;
    // use the refresh rate of the physical screen as the maximum refresh rate
    GetScreenActiveModeLocked(defaultScreenId_, screenModeInfo);
    // guaranteed screen refresh rate at least 1
    if (skipFrameInterval == 0 || (skipFrameInterval > screenModeInfo.GetScreenRefreshRate())) {
        return INVALID_ARGUMENTS;
    }
    screens_.at(id)->SetScreenSkipFrameInterval(skipFrameInterval);
    RS_LOGD("RSScreenManager %{public}s: screen(id %" PRIu64 "), skipFrameInterval(%d).",
        __func__, id, skipFrameInterval);
    return StatusCode::SUCCESS;
}

int32_t RSScreenManager::GetPixelFormatLocked(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    if (screens_.count(id) == 0 || !screens_.at(id)) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::SetPixelFormatLocked(ScreenId id, GraphicPixelFormat pixelFormat)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->SetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::GetScreenSupportedHDRFormatsLocked(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenSupportedHDRFormats(hdrFormats);
}

int32_t RSScreenManager::GetScreenHDRFormatLocked(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenHDRFormat(hdrFormat);
}

int32_t RSScreenManager::SetScreenHDRFormatLocked(ScreenId id, int32_t modeIdx)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->SetScreenHDRFormat(modeIdx);
}

int32_t RSScreenManager::GetScreenSupportedColorSpacesLocked(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenSupportedColorSpaces(colorSpaces);
}

int32_t RSScreenManager::GetScreenColorSpaceLocked(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->GetScreenColorSpace(colorSpace);
}

int32_t RSScreenManager::SetScreenColorSpaceLocked(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    if (screens_.count(id) == 0) {
        RS_LOGW("RSScreenManager %{public}s: There is no screen for id %{public}" PRIu64 ".", __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screens_.at(id)->SetScreenColorSpace(colorSpace);
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

} // namespace impl

sptr<RSScreenManager> CreateOrGetScreenManager()
{
    return impl::RSScreenManager::GetInstance();
}
} // namespace Rosen
} // namespace OHOS
