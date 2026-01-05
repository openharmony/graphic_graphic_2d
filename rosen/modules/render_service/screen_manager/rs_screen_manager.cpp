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
#include <sstream>

#include "common/rs_optional_trace.h"
#include "display_engine/rs_color_temperature.h"
#include "display_engine/rs_luminance_control.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "param/sys_param.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "product_feature/fold/rs_foldable_screen_manager.h"
#include "rs_screen.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSScreenManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t WAIT_FOR_STATUS_TASK_TIMEOUT = 1000; // 1000ms
constexpr uint32_t MAX_VIRTUAL_SCREEN_NUM = 64;
constexpr uint32_t MAX_VIRTUAL_SCREEN_WIDTH = 65536;
constexpr uint32_t MAX_VIRTUAL_SCREEN_HEIGHT = 65536;
constexpr uint32_t MAX_VIRTUAL_SCREEN_REFRESH_RATE = 120;
constexpr uint32_t ORIGINAL_FOLD_SCREEN_AMOUNT = 2;
constexpr ScreenId BUILT_IN_MAIN_SCREEN_ID = 0;
constexpr ScreenId BUILT_IN_EXTERNAL_SCREEN_ID = 5;
} // namespace
using namespace HiviewDFX;

void RSScreenManager::RegisterCoreListener(const sptr<RSIScreenManagerListener>& listener)
{
    if (!callbackMgr_) {
        RS_LOGE("%{public}s: callbackMgr is nullptr", __func__);
        return;
    }
    callbackMgr_->SetCoreListener(listener);
}

void RSScreenManager::RegisterAgentListener(const sptr<RSIScreenManagerAgentListener>& listener)
{
    if (!callbackMgr_) {
        RS_LOGE("%{public}s: callbackMgr is nullptr", __func__);
        return;
    }
    callbackMgr_->AddAgentListener(listener);
}

void RSScreenManager::UnRegisterAgentListener(const sptr<RSIScreenManagerAgentListener>& listener)
{
    if (!callbackMgr_) {
        RS_LOGE("%{public}s: callbackMgr is nullptr", __func__);
        return;
    }
    callbackMgr_->RemoveAgentListener(listener);
}

bool RSScreenManager::Init(const std::shared_ptr<AppExecFwk::EventHandler>& mainHandler) noexcept
{
    isFoldScreenFlag_ = system::GetParameter("const.window.foldscreen.type", "") != "";
    preprocessor_ = std::make_shared<RSScreenPreprocessor>(
        wptr<RSScreenManager>(this), callbackMgr_, mainHandler, isFoldScreenFlag_);
    if (isFoldScreenFlag_) {
        foldScreenManager_ = std::make_unique<RSFoldableScreenManager>(preprocessor_);
    }
    if (!preprocessor_->Init()) {
        return false;
    }
    if (foldScreenManager_) {
        foldScreenManager_->Init();
    }
    return true;
}

ScreenId RSScreenManager::GetActiveScreenId()
{
    return foldScreenManager_ ? foldScreenManager_->GetActiveScreenId() : INVALID_SCREEN_ID;
}

void RSScreenManager::OnHwcDeadEvent(std::map<ScreenId, std::shared_ptr<RSScreen>>& retScreens)
{
    RS_TRACE_FUNC();
    RS_LOGW("%{public}s.", __func__);
    // Only clean the physical screen when hwc dead
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        for (auto it = screens_.begin(); it != screens_.end();) {
            if (it->second && !it->second->IsVirtual()) {
                auto node = screens_.extract(it++);
                retScreens.insert(std::move(node));
            } else {
                ++it;
            }
        }
    }
    defaultScreenId_ = INVALID_SCREEN_ID;
}

void RSScreenManager::ProcessPendingConnections()
{
    std::vector<ScreenId> pendingConnectedIds;
    {
        std::lock_guard<std::mutex> lock(hotPlugAndConnectMutex_);
        pendingConnectedIds.swap(pendingConnectedIds_);
    }
    for (auto id : pendingConnectedIds) {
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
        if (backLightLevel != INVALID_BACKLIGHT_VALUE) {
            screen->SetScreenBacklight(backLightLevel);
        }
    }
}

bool RSScreenManager::CheckFoldScreenIdBuiltIn(ScreenId id)
{
    if (id == BUILT_IN_MAIN_SCREEN_ID || id == BUILT_IN_EXTERNAL_SCREEN_ID) {
        return true;
    }
    return false; // ??? todo
}

void RSScreenManager::ProcessScreenConnected(ScreenId id)
{
    auto screen = std::make_shared<RSScreen>(id);
    screen->SetOnPropertyChangedCallback(
        std::bind(&RSScreenManager::OnScreenPropertyChanged, this, std::placeholders::_1));
    screen->SetOnBacklightChangedCallback(
        std::bind(&RSScreenManager::OnScreenBacklightChanged, this, std::placeholders::_1, std::placeholders::_2));

    std::unique_lock<std::mutex> lock(screenMapMutex_);
    screens_[id] = screen;
    if (isFoldScreenFlag_ && CheckFoldScreenIdBuiltIn(id) && foldScreenIds_.size() < ORIGINAL_FOLD_SCREEN_AMOUNT) {
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
    if (foldScreenManager_ && id != 0) {
        foldScreenManager_->SetExternalScreenId(id);
    }
    std::lock_guard<std::mutex> connectLock(hotPlugAndConnectMutex_);
    pendingConnectedIds_.emplace_back(id);
    mipiCheckInFirstHotPlugEvent_ = true;
}

void RSScreenManager::ProcessScreenDisConnected(ScreenId id)
{
    if (auto screen = GetScreen(id)) {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        screens_.erase(id);
        RS_LOGI("%{public}s: Screen(id %{public}" PRIu64 ") disconnected.", __func__, id);
    } else {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
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

    mipiCheckInFirstHotPlugEvent_ = true;
}

bool RSScreenManager::UpdateVsyncEnabledScreenId(ScreenId screenId)
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
            return false;
        }
    }
    lock.unlock();
    return true;
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

void RSScreenManager::SetScreenVsyncEnableById(ScreenId vsyncEnabledScreenId, ScreenId screenId, bool enabled)
{
    if (vsyncEnabledScreenId != INVALID_SCREEN_ID) {
        if (vsyncEnabledScreenId == screenId && screens_.find(screenId) != screens_.end()) {
            screens_[screenId]->SetScreenVsyncEnabled(true);
        }
    }
    else {
        auto screen = GetScreen(screenId);
        if (screen == nullptr) {
            RS_LOGE("SetScreenVsyncEnableById:%{public}d failed, screen %{public}" PRIu64 " not found",
                enabled, screenId);
            return;
        }
        screen->SetScreenVsyncEnabled(enabled);
    }
}

uint64_t RSScreenManager::GetScreenVsyncEnableById(ScreenId vsyncEnabledScreenId)
{
    auto iter = std::find_if(screens_.cbegin(), screens_.cend(), [](const auto& node) {
        const auto& screen = node.second;
        return screen && !screen->IsVirtual();
    });
    if (iter != screens_.end()) {
        vsyncEnabledScreenId = iter->first;
    }
    return vsyncEnabledScreenId;
}

bool RSScreenManager::GetIsFoldScreenFlag()
{
    return isFoldScreenFlag_;
}

uint64_t RSScreenManager::JudgeVSyncEnabledScreenWhilePowerStatusChanged(ScreenId screenId, ScreenPowerStatus status, uint64_t enabledScreenId)
{
    std::unique_lock<std::mutex> lock(screenMapMutex_);
    auto it = foldScreenIds_.find(screenId);
    if (it == foldScreenIds_.end()) {
        return enabledScreenId;
    }

    if (status == ScreenPowerStatus::POWER_STATUS_ON) {
        it->second.isPowerOn = true;
        auto vsyncScreenIt = foldScreenIds_.find(enabledScreenId);
        if (vsyncScreenIt == foldScreenIds_.end() || vsyncScreenIt->second.isPowerOn == false) {
            return screenId;
        }
    } else if (status == ScreenPowerStatus::POWER_STATUS_OFF) {
        it->second.isPowerOn = false;
        if (screenId != enabledScreenId) {
            return enabledScreenId;
        }
        for (auto& [foldScreenId, status] : foldScreenIds_) {
            if (status.isConnected && status.isPowerOn) {
                return foldScreenId;
            }
        }
    }
    return enabledScreenId;
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    auto modeInfo = screen->GetActiveMode();
    if (!modeInfo) {
        RS_LOGE("%{public}s: Failed to get active mode for screen %{public}" PRIu64, __func__, id);
        return;
    }

    HILOG_COMM_INFO("GetScreenActiveMode: screen[%{public}" PRIu64 "] pixel[%{public}d * %{public}d],"
        "freshRate[%{public}d]", id, modeInfo->width, modeInfo->height, modeInfo->freshRate);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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

    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return INVALID_POWER_STATUS;
    }

    ScreenPowerStatus status = screen->GetPowerStatus();
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
    ScreenId associatedScreenId,
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
    configs.associatedScreenId = associatedScreenId;
    configs.name = name;
    configs.width = width;
    configs.height = height;
    configs.surface = surface;
    configs.flags = flags;
    configs.whiteList = std::unordered_set<NodeId>(whiteList.begin(), whiteList.end());

    auto screen = std::make_shared<RSScreen>(configs);
    screen->SetOnPropertyChangedCallback(std::bind(&RSScreenManager::OnScreenPropertyChanged, this,
        std::placeholders::_1));
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        screens_[newId] = screen;
    }
    ++currentVirtualScreenNum_;
    preprocessor_->NotifyVirtualScreenConnected(newId, associatedScreenId, screen->GetProperty());
    RS_LOGI("%{public}s: create virtual screen(id %{public}" PRIu64 "), width %{public}u, height %{public}u."
        "associatedScreenId %{public}" PRIu64,
        __func__, newId, width, height, associatedScreenId);
    return newId;
}

int32_t RSScreenManager::SetVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    std::unordered_set<NodeId> screenBlackList(blackList.begin(), blackList.end());
    if (id == INVALID_SCREEN_ID) {
        return SetGlobalBlackList(screenBlackList);
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    virtualScreen->SetBlackList(screenBlackList);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(__func__, SpecialLayerType::IS_BLACK_LIST, id, screenBlackList);
    return SUCCESS;
}

int32_t RSScreenManager::SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList)
{
    std::unordered_set<NodeType> screenTypeBlackList(typeBlackList.begin(), typeBlackList.end());
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: Record typeblacklist for id %{public}" PRIu64, __func__, id);
    virtualScreen->SetTypeBlackList(screenTypeBlackList);
    return SUCCESS;
}

int32_t RSScreenManager::AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    if (id == INVALID_SCREEN_ID) {
        return AddGlobalBlackList(blackList);
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    if (virtualScreen->GetBlackList().size() + blackList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: blacklist is over max size!", __func__);
        return INVALID_ARGUMENTS;
    }
    virtualScreen->AddBlackList(blackList);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        __func__, SpecialLayerType::IS_BLACK_LIST, id, virtualScreen->GetBlackList());
    return SUCCESS;
}

int32_t RSScreenManager::RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    if (id == INVALID_SCREEN_ID) {
        return RemoveGlobalBlackList(blackList);
    }
    auto virtualScreen = GetScreen(id);
    if (virtualScreen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    virtualScreen->RemoveBlackList(blackList);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        __func__, SpecialLayerType::IS_BLACK_LIST, id, virtualScreen->GetBlackList());
    return SUCCESS;
}

int32_t RSScreenManager::SetGlobalBlackList(const std::unordered_set<NodeId>& blackList)
{
    std::lock_guard<std::mutex> lock(globalBlackListMutex_);
    if (blackList == globalBlackList_) {
        RS_LOGI("%{public}s: The global blacklist has not changed!", __func__);
        return SUCCESS;
    }
    globalBlackList_ = blackList;
    callbackMgr_->NotifyGlobalBlacklistChanged(globalBlackList_);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        __func__, SpecialLayerType::IS_BLACK_LIST, INVALID_SCREEN_ID, globalBlackList_);
    return SUCCESS;
}

int32_t RSScreenManager::AddGlobalBlackList(const std::vector<NodeId>& blackList)
{
    std::lock_guard<std::mutex> lock(globalBlackListMutex_);
    if (globalBlackList_.size() + blackList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: global blacklist is over max size!", __func__);
        return INVALID_ARGUMENTS;
    }
    auto oldGlobalBlackList = globalBlackList_;
    globalBlackList_.insert(blackList.cbegin(), blackList.cend());
    if (oldGlobalBlackList == globalBlackList_) {
        RS_LOGI("%{public}s: The global blacklist has not changed!", __func__);
        return SUCCESS;
    }
    callbackMgr_->NotifyGlobalBlacklistChanged(globalBlackList_);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        __func__, SpecialLayerType::IS_BLACK_LIST, INVALID_SCREEN_ID, globalBlackList_);
    return SUCCESS;
}

int32_t RSScreenManager::RemoveGlobalBlackList(const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(globalBlackListMutex_);
    auto oldGlobalBlackList = globalBlackList_;
    for (const auto& nodeId : blackList) {
        globalBlackList_.erase(nodeId);
    }
    if (oldGlobalBlackList == globalBlackList_) {
        RS_LOGI("%{public}s: The global blacklist has not changed!", __func__);
        return SUCCESS;
    }
    callbackMgr_->NotifyGlobalBlacklistChanged(globalBlackList_);
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        __func__, SpecialLayerType::IS_BLACK_LIST, INVALID_SCREEN_ID, globalBlackList_);
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
    return SUCCESS;
}

// only used in dirtyRegion
bool RSScreenManager::CheckVirtualScreenStatusChanged(ScreenId id)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }
    if (!screen->IsVirtual()) {
        return false;
    }
    return screen->GetAndResetPSurfaceChange() || screen->GetAndResetVirtualScreenPlay();
}

void RSScreenManager::RemoveVirtualScreen(ScreenId id)
{
    {
        std::lock_guard<std::mutex> lock(screenMapMutex_);
        auto iter = screens_.find(id);
        if (iter == screens_.end() || iter->second == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            return;
        }
        if (!iter->second->IsVirtual()) {
            RS_LOGW("%{public}s: The screen is not virtual, id %{public}" PRIu64, __func__, id);
            return;
        }

        screens_.erase(iter);
        --currentVirtualScreenNum_;
        RS_LOGI("%{public}s: remove virtual screen(id %{public}" PRIu64 ").", __func__, id);
    }
    {
        std::lock_guard<std::mutex> lock(virtualScreenIdMutex_);
        freeVirtualScreenIds_.push(id);
    }
    preprocessor_->NotifyVirtualScreenDisconnected(id);

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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "], modeId:[%" PRIu32 "]", __func__, id, modeId);
    RS_LOGI("%{public}s, id:[%{public}" PRIu64 "], modeId:[%{public}" PRIu32 "]", __func__, id, modeId);
    return screen->SetActiveMode(modeId);
}

uint32_t RSScreenManager::SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screen->SetScreenActiveRect(activeRect);
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
    return SUCCESS;
}

int32_t RSScreenManager::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return SCREEN_NOT_FOUND;
    }
    RS_LOGI("%{public}s: set rog screen resolution success", __func__);
    screen->SetResolution(width, height);
    return SUCCESS;
}

void RSScreenManager::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("[UL_POWER] %{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }

    auto ret = screen->SetPowerStatus(static_cast<uint32_t>(status));
    if (ret != static_cast<int32_t>(StatusCode::SUCCESS)) {
        RS_LOGE("[UL_POWER] %{public}s: Failed to set power status of id %{public}" PRIu64, __func__, id);
        return;
    }

    /*
     * If app adds the first frame when power on the screen, delete the code
     */
    if (status == ScreenPowerStatus::POWER_STATUS_ON ||
        status == ScreenPowerStatus::POWER_STATUS_ON_ADVANCED) {
        RSFirstFrameNotifier::GetInstance().AddFirstFrameCommitScreen(id);
    }
    RSLuminanceControl::Get().UpdateScreenStatus(id, status);
    RSColorTemperature::Get().UpdateScreenStatus(id, status);
    std::lock_guard<std::shared_mutex> lock(powerStatusMutex_);
    screenPowerStatus_[id] = status;
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

int32_t RSScreenManager::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    RS_LOGI("%{public}s: screenId: %{public}" PRIu64 " isAutoRotation: %{public}d", __func__, id, isAutoRotation);
    return screen->SetVirtualScreenAutoRotation(isAutoRotation);
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

/* only used for mock tests */
void RSScreenManager::MockHdiScreenConnected(std::shared_ptr<RSScreen> rsScreen)
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::GetScreenData id:[%" PRIu64 "].", id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
        RS_LOGD("%{public}s: repeat backlight screenId: %{public}" PRIu64 " newLevel: %u", __func__, id, level);
        return;
    }
    screenBacklight_[id] = level;
}

uint32_t RSScreenManager::GetScreenActiveRefreshRate(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return 0;
    }

    return screen->GetActiveRefreshRate();
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

bool RSScreenManager::GetVirtualScreenAutoRotation(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return false;
    }
    return screen->GetVirtualScreenAutoRotation();
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

int32_t RSScreenManager::SetScreenSwitchingNotifyCallback(const sptr<RSIScreenSwitchingNotifyCallback>& callback)
{
    std::lock_guard<std::shared_mutex> lock(screenSwitchingNotifyCallbackMutex_);
    screenSwitchingNotifyCallback_ = callback;
    RS_LOGI("%{public}s: set screen switching notify callback succeed.", __func__);
    return SUCCESS;
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

int32_t RSScreenManager::SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type)
{
    frameId_++;
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_TRACE_NAME_FMT("SetScreenConstraint frameId:%lu timestamp:%lu type:%d", frameId_.load(), timestamp, type);
    return screen->SetScreenConstraint(frameId_, timestamp, type);
}

int32_t RSScreenManager::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenSupportedColorGamuts(mode);
}

int32_t RSScreenManager::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenSupportedMetaDataKeys(keys);
}

int32_t RSScreenManager::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenColorGamut(mode);
}

int32_t RSScreenManager::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->SetScreenColorGamut(modeIdx);
}

int32_t RSScreenManager::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, ForceRefreshOneFrame, id:[%" PRIu64 "].", __func__, id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    screen->SetScreenSkipFrameInterval(skipFrameInterval);
    RS_LOGI("%{public}s: screen(id %{public}" PRIu64 "), skipFrameInterval(%{public}u).",
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
    RS_LOGI("%{public}s: screen(id %{public}" PRIu64 "), maxRefreshRate(%{public}u).",
        __func__, id, maxRefreshRate);
    actualRefreshRate = maxRefreshRate;
    return StatusCode::SUCCESS;
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

ScreenConnectionType RSScreenManager::GetScreenConnectionType(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return ScreenConnectionType::INVALID_DISPLAY_CONNECTION_TYPE;
    }
    return screen->GetConnectionType();
}

int32_t RSScreenManager::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->SetPixelFormat(pixelFormat);
}

int32_t RSScreenManager::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenSupportedHDRFormats(hdrFormats);
}

int32_t RSScreenManager::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenHDRFormat(hdrFormat);
}

int32_t RSScreenManager::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenSupportedColorSpaces(colorSpaces);
}

int32_t RSScreenManager::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->GetScreenColorSpace(colorSpace);
}

int32_t RSScreenManager::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenManager::%s, id:[%" PRIu64 "].", __func__, id);
    return screen->SetScreenColorSpace(colorSpace);
}

void RSScreenManager::MarkPowerOffNeedProcessOneFrame()
{
    powerOffNeedProcessOneFrame_ = true;
}

void RSScreenManager::DisablePowerOffRenderControl(ScreenId id)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    screen->SetDisablePowerOffRenderControl(true);
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

void RSScreenManager::SetScreenSwitchStatus(ScreenId id, bool status)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }

    screen->SetScreenSwitchStatus(status);
    RS_LOGI("%{public}s: set isScreenSwitching_ = %{public}d", __func__, status);
    if (!status) {
        NotifySwitchingCallback(status);
    }
}

// 移出去
int32_t RSScreenManager::SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix)
{
    auto task = [this, id, matrix]() -> void {
        RS_OPTIONAL_TRACE_NAME_FMT("SetScreenLinearMatrix, id:[%" PRIu64 "].", id);
        auto screen = GetScreen(id);
        if (screen == nullptr) {
            RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
            return;
        }
        screen->SetScreenLinearMatrix(matrix);
    };
    // SetScreenLinearMatrix is SMQ API, which can only be executed in RSHardwareThread.
    // RSHardwareThread::Instance().PostTask(task);
    return StatusCode::SUCCESS;
}

void RSScreenManager::SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGW("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return;
    }
    screen->SetScreenOffset(offsetX, offsetY);
}

bool RSScreenManager::AnyScreenFits(std::function<bool(const ScreenNode&)> func) const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    return std::any_of(screens_.cbegin(), screens_.cend(), func);
}

void RSScreenManager::NotifySwitchingCallback(bool status) const
{
    std::shared_lock<std::shared_mutex> lock(screenSwitchingNotifyCallbackMutex_);
    if (screenSwitchingNotifyCallback_ == nullptr) {
        RS_LOGE("%{public}s: screenSwitchingNotifyCallback_ is nullptr! status: %{public}d", __func__, status);
        return;
    }

    RS_LOGI("%{public}s: status: %{public}d", __func__, status);
    screenSwitchingNotifyCallback_->OnScreenSwitchingNotify(status);
}

std::shared_ptr<RSScreen> RSScreenManager::GetScreen(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    auto iter = screens_.find(id);
    if (iter == screens_.end() || iter->second == nullptr) {
        return nullptr;
    }
    return iter->second;
}
void RSScreenManager::ExecuteCallback(const sptr<RSIScreenChangeCallback>& callback) const
{
    std::lock_guard<std::mutex> lock(screenMapMutex_);
    for (const auto& [id, screen] : screens_) {
        if (screen == nullptr) {
            RS_LOGW("%{public}s: screen %{public}" PRIu64 " not found", __func__, id);
            continue;
        }
        if (screen->IsVirtual()) {
            continue;
        }
        sptr<IRemoteObject> conn = callbackMgr_ ? callbackMgr_->GetClientToRenderConnection(id) : nullptr;
        callback->OnScreenChanged(id, ScreenEvent::CONNECTED, ScreenChangeReason::DEFAULT, conn);
    }
}

sptr<RSScreenProperty> RSScreenManager::QueryScreenProperty(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (screen == nullptr) {
        RS_LOGE("%{public}s: There is no screen for id %{public}" PRIu64, __func__, id);
        return nullptr;
    }
    return screen->GetProperty();
}

void RSScreenManager::OnScreenPropertyChanged(const sptr<RSScreenProperty>& property)
{
    if (property == nullptr) {
        return;
    }
    callbackMgr_->NotifyScreenPropertyUpdated(property->id_, property);
}

void RSScreenManager::OnScreenBacklightChanged(ScreenId id, uint32_t level)
{
    callbackMgr_->NotifyScreenBacklightChanged(id, level);
}
} // namespace Rosen
} // namespace OHOS
