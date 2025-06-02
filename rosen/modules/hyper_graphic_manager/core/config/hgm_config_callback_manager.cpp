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
        HGM_LOGE("HgmConfigCallbackManager %{public}s : callback is null.", __func__);
        return;
    }
    animDynamicCfgCallbacks_[pid] = callback;
    HGM_LOGD("HgmConfigCallbackManager %{public}s : add a remote callback succeed.", __func__);

    auto& hgmCore = HgmCore::Instance();
    auto data = std::make_shared<RSHgmConfigData>();

    auto screenType = hgmCore.GetFrameRateMgr()->GetCurScreenStrategyId();
    auto settingMode = std::to_string(hgmCore.GetCurrentRefreshRateMode());
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        auto dynamicSettingMap = configData->GetAceSceneDynamicSettingMap(screenType, settingMode);
        for (auto& [animType, dynamicSetting] : dynamicSettingMap) {
            for (auto& [animName, dynamicConfig] : dynamicSetting) {
                data->AddAnimDynamicItem({
                    animType, animName, dynamicConfig.min, dynamicConfig.max, dynamicConfig.preferred_fps});
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
        HGM_LOGE("HgmRefreshRateModeCallbackManager %{public}s : callback is null.", __func__);
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
            HGM_LOGD("refreshRateUpdateCallbacks unregister succ, remove pid %{public}u", pid);
        }
        return;
    }
    refreshRateUpdateCallbacks_[pid] = callback;
    uint32_t currentRefreshRate =
        HgmCore::Instance().GetScreenCurrentRefreshRate(HgmCore::Instance().GetActiveScreenId());
    HGM_LOGD("%{public}s : currentRefreshRate = %{public}d", __func__, currentRefreshRate);
    callback->OnHgmRefreshRateUpdate(currentRefreshRate);
}

void HgmConfigCallbackManager::RegisterXComponentExpectedFrameRateCallback(pid_t listenerPid, pid_t dstPid,
    const sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback>& callback)
{
    if (callback == nullptr) {
        if (auto iter = xcomponentExpectedFrameRateCallbacks_.find(dstPid);
            iter != xcomponentExpectedFrameRateCallbacks_.end()) {
            iter->second.erase(listenerPid);
            HGM_LOGD("refreshRateUpdateCallbacks unregister succ, remove pid %{public}u", listenerPid);
        }
        return;
    }
    xcomponentExpectedFrameRateCallbacks_[dstPid][listenerPid] = callback;
    if (auto iter = xcomponentExpectedFrameRate_.find(dstPid); iter != xcomponentExpectedFrameRate_.end()) {
        HGM_LOGD("%{public}d: %{public}d", dstPid, iter->second);
        callback->OnFrameRateLinkerExpectedFpsUpdate(dstPid, iter->second);
    }
}

void HgmConfigCallbackManager::SyncHgmConfigChangeCallback()
{
    pendingAnimDynamicCfgCallbacks_ = animDynamicCfgCallbacks_;
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    SyncHgmConfigChangeCallback(frameRateMgr->GetMultiAppStrategy().GetForegroundPidApp());
}

void HgmConfigCallbackManager::SyncHgmConfigChangeCallback(
    const std::unordered_map<pid_t, std::pair<int32_t, std::string>>& pids)
{
    if (pendingAnimDynamicCfgCallbacks_.empty()) {
        return;
    }
    decltype(pendingAnimDynamicCfgCallbacks_) callbacks;
    for (const auto &[pid, _] : pids) {
        if (pendingAnimDynamicCfgCallbacks_.find(pid) == pendingAnimDynamicCfgCallbacks_.end()) {
            continue;
        }
        callbacks[pid] = pendingAnimDynamicCfgCallbacks_[pid];
        pendingAnimDynamicCfgCallbacks_.erase(pid);
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

    auto screenType = frameRateMgr->GetCurScreenStrategyId();
    auto settingMode = std::to_string(hgmCore.GetCurrentRefreshRateMode());
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        auto dynamicSettingMap = configData->GetAceSceneDynamicSettingMap(screenType, settingMode);
        for (auto& [animType, dynamicSetting] : dynamicSettingMap) {
            for (auto& [animName, dynamicConfig] : dynamicSetting) {
                data->AddAnimDynamicItem({
                    animType, animName, dynamicConfig.min, dynamicConfig.max, dynamicConfig.preferred_fps});
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

    for (auto &[pid, callback] : callbacks) {
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
    pid_t pid, const std::string& id, int32_t expectedFrameRate)
{
    if (auto iter = xcomponentExpectedFrameRateCallbacks_.find(pid);
        iter != xcomponentExpectedFrameRateCallbacks_.end()) {
        for (auto &[listenerPid, cb] : iter->second) {
            if (cb == nullptr) {
                continue;
            }
            HGM_LOGD(" -> %{public}d: %{public}d, %{public}d", listenerPid, pid, expectedFrameRate);
            cb->OnFrameRateLinkerExpectedFpsUpdate(pid, expectedFrameRate);
        }
    }
    xcomponentExpectedFrameRate_[pid] = expectedFrameRate;
}

void HgmConfigCallbackManager::UnRegisterHgmConfigChangeCallback(pid_t pid)
{
    if (animDynamicCfgCallbacks_.find(pid) != animDynamicCfgCallbacks_.end()) {
        animDynamicCfgCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove a remote callback succeed.", __func__);
    }
    if (pendingAnimDynamicCfgCallbacks_.find(pid) != pendingAnimDynamicCfgCallbacks_.end()) {
        pendingAnimDynamicCfgCallbacks_.erase(pid);
    }

    if (refreshRateModeCallbacks_.find(pid) != refreshRateModeCallbacks_.end()) {
        refreshRateModeCallbacks_.erase(pid);
        HGM_LOGD("HgmRefreshRateModeCallbackManager %{public}s : remove a remote callback succeed.", __func__);
    }

    for (auto &[_, listenerPidCb] : xcomponentExpectedFrameRateCallbacks_) {
        listenerPidCb.erase(pid);
    }
    SyncXComponentExpectedFrameRateCallback(pid, "", 0);
    xcomponentExpectedFrameRateCallbacks_.erase(pid);
    xcomponentExpectedFrameRate_.erase(pid);
}
} // namespace OHOS::Rosen

