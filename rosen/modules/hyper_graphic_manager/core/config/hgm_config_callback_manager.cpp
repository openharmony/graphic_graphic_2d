/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_config_callback_manager.h"

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_log.h"

namespace OHOS::Rosen {
std::once_flag HgmConfigCallbackManager::createFlag_;
sptr<HgmConfigCallbackManager> HgmConfigCallbackManager::instance_ = nullptr;
namespace {
constexpr int32_t DESTROYED_XCOMPONENT_FRAMERATE = -1;
constexpr int32_t MAX_XCOMPONENT_ID_NUMS = 50;
}

sptr<HgmConfigCallbackManager> HgmConfigCallbackManager::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new HgmConfigCallbackManager();
    });
    return instance_;
}

HgmConfigCallbackManager::HgmConfigCallbackManager()
{
}

HgmConfigCallbackManager::~HgmConfigCallbackManager() noexcept
{
    animDynamicCfgCallbacks_.clear();
    pendingAnimDynamicCfgCallbacks_.clear();
    refreshRateModeCallbacks_.clear();
}

void HgmConfigCallbackManager::RegisterHgmConfigChangeCallback(
    pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback)
{
    if (callback == nullptr) {
        HILOG_COMM_ERROR("HgmConfigCallbackManager %{public}s : callback is null.", __func__);
        return;
    }
    animDynamicCfgCallbacks_[pid] = callback;
    HGM_LOGD("HgmConfigCallbackManager %{public}s : add a remote callback succeed.", __func__);

    auto& hgmCore = HgmCore::Instance();
    auto data = std::make_shared<RSHgmConfigData>();

    data->SetIsSyncConfig(true);
    auto screenType = hgmCore.GetFrameRateMgr()->GetCurScreenStrategyId();
    auto settingMode = std::to_string(hgmCore.GetCurrentRefreshRateMode());
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        auto dynamicSettingMap = configData->GetAceSceneDynamicSettingMap(screenType, settingMode);
        for (auto& [animType, dynamicSetting] : dynamicSettingMap) {
            for (auto& [animName, dynamicConfig] : dynamicSetting) {
                data->AddAnimDynamicItem({
                    animType, animName, dynamicConfig.min, dynamicConfig.max, dynamicConfig.preferredFps});
            }
        }
    }

    if (configData != nullptr) {
        for (auto item : configData->pageNameList_) {
            data->AddPageName(item);
        }
    }

    auto screen = hgmCore.GetActiveScreen();
    if (screen != nullptr) {
        data->SetPpi(screen->GetPpi());
        data->SetXDpi(screen->GetXDpi());
        data->SetYDpi(screen->GetYDpi());
    }
    callback->OnHgmConfigChanged(data);
}

void HgmConfigCallbackManager::RegisterHgmRefreshRateModeChangeCallback(
    pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback)
{
    if (callback == nullptr) {
        HILOG_COMM_ERROR("HgmRefreshRateModeCallbackManager %{public}s : callback is null.", __func__);
        return;
    }
    refreshRateModeCallbacks_[pid] = callback;
    HGM_LOGD("HgmRefreshRateModeCallbackManager %{public}s : add a remote callback succeed.", __func__);

    int32_t currentRefreshRateModeName = HgmCore::Instance().GetCurrentRefreshRateModeName();
    callback->OnHgmRefreshRateModeChanged(currentRefreshRateModeName);
}

void HgmConfigCallbackManager::RegisterHgmRefreshRateUpdateCallback(
    pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback)
{
    if (callback == nullptr) {
        if (refreshRateUpdateCallbacks_.find(pid) != refreshRateUpdateCallbacks_.end()) {
            refreshRateUpdateCallbacks_.erase(pid);
            HGM_LOGD("refreshRateUpdateCallbacks unregister succ, remove pid %{public}d", pid);
        }
        return;
    }
    refreshRateUpdateCallbacks_[pid] = callback;
    uint32_t currentRefreshRate =
        HgmCore::Instance().GetScreenCurrentRefreshRate(HgmCore::Instance().GetActiveScreenId());
    HGM_LOGD("%{public}s : currentRefreshRate = %{public}u", __func__, currentRefreshRate);
    callback->OnHgmRefreshRateUpdate(currentRefreshRate);
}

void HgmConfigCallbackManager::RegisterXComponentExpectedFrameRateCallback(pid_t listenerPid, pid_t dstPid,
    const sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback>& callback)
{
    HGM_LOGI("%{public}d:[%{public}d,%{public}d]", listenerPid, dstPid, callback == nullptr);
    if (callback == nullptr) {
        if (auto iter = xcomponentExpectedFrameRateCallbacks_.find(dstPid);
            iter != xcomponentExpectedFrameRateCallbacks_.end()) {
            iter->second.erase(listenerPid);
            HGM_LOGI("refreshRateUpdateCallbacks unregister succ, remove pid %{public}d", listenerPid);
        }
        return;
    }
    xcomponentExpectedFrameRateCallbacks_[dstPid][listenerPid] = callback;
    if (auto iter = xcomponentExpectedFrameRate_.find(dstPid); iter != xcomponentExpectedFrameRate_.end()) {
        for (auto& [xcomponentId, frameRate] : iter->second) {
            HGM_LOGI("%{public}d:[%{public}s,%{public}d]", dstPid, xcomponentId.c_str(), frameRate);
            callback->OnFrameRateLinkerExpectedFpsUpdate(dstPid, xcomponentId, frameRate);
        }
    }
}

void HgmConfigCallbackManager::SyncHgmConfigChangeCallback(pid_t extraSyncPid)
{
    pendingAnimDynamicCfgCallbacks_ = animDynamicCfgCallbacks_;
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    SyncHgmConfigChangeCallback(frameRateMgr->GetMultiAppStrategy().GetForegroundPidApp(), extraSyncPid);
}

void HgmConfigCallbackManager::SyncHgmConfigChangeCallback(
    const std::unordered_map<pid_t, std::pair<int32_t, std::string>>& pids, pid_t extraSyncPid)
{
    if (pendingAnimDynamicCfgCallbacks_.empty()) {
        return;
    }
    decltype(pendingAnimDynamicCfgCallbacks_) callbacks;
    for (const auto& [pid, _] : pids) {
        if (auto iter = pendingAnimDynamicCfgCallbacks_.find(pid);
            iter != pendingAnimDynamicCfgCallbacks_.end()) {
            callbacks.insert_or_assign(pid, std::move(iter->second));
            pendingAnimDynamicCfgCallbacks_.erase(iter);
        }
    }
    if (extraSyncPid > DEFAULT_PID) {
        if (auto iter = pendingAnimDynamicCfgCallbacks_.find(extraSyncPid);
            iter != pendingAnimDynamicCfgCallbacks_.end()) {
            callbacks.insert_or_assign(extraSyncPid, std::move(iter->second));
            pendingAnimDynamicCfgCallbacks_.erase(iter);
        }
    }

    if (callbacks.empty()) {
        return;
    }

    auto& hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    auto data = std::make_shared<RSHgmConfigData>();

    data->SetIsSyncConfig(true);
    auto screenType = frameRateMgr->GetCurScreenStrategyId();
    auto settingMode = std::to_string(hgmCore.GetCurrentRefreshRateMode());
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        auto dynamicSettingMap = configData->GetAceSceneDynamicSettingMap(screenType, settingMode);
        for (auto& [animType, dynamicSetting] : dynamicSettingMap) {
            for (auto& [animName, dynamicConfig] : dynamicSetting) {
                data->AddAnimDynamicItem({
                    animType, animName, dynamicConfig.min, dynamicConfig.max, dynamicConfig.preferredFps});
            }
        }
    }

    if (configData != nullptr) {
        for (auto item : configData->pageNameList_) {
            data->AddPageName(item);
        }
    }

    auto screen = hgmCore.GetActiveScreen();
    if (screen != nullptr) {
        data->SetPpi(screen->GetPpi());
        data->SetXDpi(screen->GetXDpi());
        data->SetYDpi(screen->GetYDpi());
    }

    for (auto& [pid, callback] : callbacks) {
        HGM_LOGD("pid:%{public}d", pid);
        callback->OnHgmConfigChanged(data);
    }
}

void HgmConfigCallbackManager::SyncRefreshRateModeChangeCallback(int32_t refreshRateMode)
{
    for (const auto& [_, callback] : refreshRateModeCallbacks_) {
        if (callback) {
            callback->OnHgmRefreshRateModeChanged(refreshRateMode);
        }
    }
}

void HgmConfigCallbackManager::SyncRefreshRateUpdateCallback(int32_t refreshRate)
{
    for (const auto& callback : refreshRateUpdateCallbacks_) {
        if (callback.second != nullptr) {
            callback.second->OnHgmRefreshRateUpdate(refreshRate);
        }
    }
}

void HgmConfigCallbackManager::SyncXComponentExpectedFrameRateCallback(
    pid_t pid, const std::string& xcomponentId, int32_t expectedFrameRate)
{
    HGM_LOGI("%{public}d:[%{public}s,%{public}d]", pid, xcomponentId.c_str(), expectedFrameRate);
    if (expectedFrameRate == DESTROYED_XCOMPONENT_FRAMERATE) {
        DestroyXComponent(pid, xcomponentId);
        return;
    }

    if (expectedFrameRate < OLED_NULL_HZ) {
        // invalid framerate
        return;
    }

    // return if lastExpectedFrameRate == expectedFrameRate
    if (auto iter = xcomponentExpectedFrameRate_.find(pid); iter != xcomponentExpectedFrameRate_.end()) {
        auto& xcomponentIdFps = iter->second;
        auto frameRateIter = xcomponentIdFps.find(xcomponentId);
        if (frameRateIter != xcomponentIdFps.end() && frameRateIter->second == expectedFrameRate) {
            return;
        }
        if (xcomponentIdFps.size() >= MAX_XCOMPONENT_ID_NUMS) {
            HGM_LOGE("HgmConfigCallbackManager xcomponentIdNums is largest");
            return;
        }
    }

    // store framerate
    xcomponentExpectedFrameRate_[pid][xcomponentId] = expectedFrameRate;

    // cb
    if (auto iter = xcomponentExpectedFrameRateCallbacks_.find(pid);
        iter != xcomponentExpectedFrameRateCallbacks_.end()) {
        for (auto& [listenerPid, cb] : iter->second) {
            if (cb == nullptr) {
                continue;
            }
            HGM_LOGI("notify %{public}d: %{public}d:[%{public}s,%{public}d]",
                listenerPid, pid, xcomponentId.c_str(), expectedFrameRate);
            cb->OnFrameRateLinkerExpectedFpsUpdate(pid, xcomponentId, expectedFrameRate);
        }
    }
}

void HgmConfigCallbackManager::UnRegisterHgmConfigChangeCallback(pid_t pid)
{
    if (animDynamicCfgCallbacks_.find(pid) != animDynamicCfgCallbacks_.end()) {
        animDynamicCfgCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove animDynamicCfgCallback", __func__);
    }
    if (pendingAnimDynamicCfgCallbacks_.find(pid) != pendingAnimDynamicCfgCallbacks_.end()) {
        pendingAnimDynamicCfgCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove pendingAnimDynamicCfgCallback", __func__);
    }

    if (refreshRateModeCallbacks_.find(pid) != refreshRateModeCallbacks_.end()) {
        refreshRateModeCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove refreshRateModeCallback", __func__);
    }

    if (refreshRateUpdateCallbacks_.find(pid) != refreshRateUpdateCallbacks_.end()) {
        refreshRateUpdateCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove refreshRateUpdateCallback", __func__);
    }

    // clean xcomponent framerate cb
    // app exit
    if (auto iter = xcomponentExpectedFrameRate_.find(pid); iter != xcomponentExpectedFrameRate_.end()) {
        for (auto& [xcomponentId, _] : iter->second) {
            SyncXComponentExpectedFrameRateCallback(pid, xcomponentId, OLED_NULL_HZ);
        }
        xcomponentExpectedFrameRate_.erase(pid);
    }

    // game server exit
    for (auto iter = xcomponentExpectedFrameRateCallbacks_.begin();
        iter != xcomponentExpectedFrameRateCallbacks_.end();) {
        auto& listenerPidCb = iter->second;
        listenerPidCb.erase(pid);
        if (listenerPidCb.empty()) {
            iter = xcomponentExpectedFrameRateCallbacks_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void HgmConfigCallbackManager::SyncEnergyDataCallback(const EnergyInfo& energyInfo)
{
    HGM_LOGD("syncEnergyInfo pid: %d", energyInfo.componentPid);
    pid_t receivePid = (energyInfo.componentPid == DEFAULT_PID) ? energyInfo_.componentPid : energyInfo.componentPid;
    energyInfo_ = energyInfo;
    if (receivePid == DEFAULT_PID) {
        return;
    }
    auto iter = animDynamicCfgCallbacks_.find(receivePid);
    if (iter == animDynamicCfgCallbacks_.end() || iter->second == nullptr) {
        return;
    }
    auto data = std::make_shared<RSHgmConfigData>();
    data->SetEnergyInfo(energyInfo_);
    iter->second->OnHgmConfigChanged(data);
}

void HgmConfigCallbackManager::DestroyXComponent(pid_t pid, const std::string& xcomponentId)
{
    HGM_LOGI("Destroy XComponent: %{public}d:%{public}s", pid, xcomponentId.c_str());
    SyncXComponentExpectedFrameRateCallback(pid, xcomponentId, OLED_NULL_HZ);

    if (auto iter = xcomponentExpectedFrameRate_.find(pid); iter != xcomponentExpectedFrameRate_.end()) {
        iter->second.erase(xcomponentId);
    }
}
} // namespace OHOS::Rosen
