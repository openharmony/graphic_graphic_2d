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
    refreshRateModeCallbacks_.clear();
    instance_ = nullptr;
}

void HgmConfigCallbackManager::RegisterHgmConfigChangeCallback(
    pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback)
{
    if (callback == nullptr) {
        HGM_LOGE("HgmConfigCallbackManager %{public}s : callback is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
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
    std::lock_guard<std::mutex> lock(mtx_);
    refreshRateModeCallbacks_[pid] = callback;
    HGM_LOGD("HgmRefreshRateModeCallbackManager %{public}s : add a remote callback succeed.", __func__);

    int32_t currentRefreshRateModeName = HgmCore::Instance().GetCurrentRefreshRateModeName();
    callback->OnHgmRefreshRateModeChanged(currentRefreshRateModeName);
}

void HgmConfigCallbackManager::RegisterHgmRefreshRateUpdateCallback(
    pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback)
{
    std::lock_guard<std::mutex> lock(mtx_);
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

void HgmConfigCallbackManager::SyncHgmConfigChangeCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (animDynamicCfgCallbacks_.empty()) {
        return;
    }

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

    auto screen = hgmCore.GetActiveScreen();
    if (screen != nullptr) {
        data->SetPpi(screen->GetPpi());
        data->SetXDpi(screen->GetXDpi());
        data->SetYDpi(screen->GetYDpi());
    }

    for (auto& callback : animDynamicCfgCallbacks_) {
        callback.second->OnHgmConfigChanged(data);
    }
}

void HgmConfigCallbackManager::SyncRefreshRateModeChangeCallback(int32_t refreshRateMode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& [_, callback] : refreshRateModeCallbacks_) {
        if (callback) {
            callback->OnHgmRefreshRateModeChanged(refreshRateMode);
        }
    }
}

void HgmConfigCallbackManager::SyncRefreshRateUpdateCallback(int32_t refreshRate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& callback : refreshRateUpdateCallbacks_) {
        if (callback.second != nullptr) {
            callback.second->OnHgmRefreshRateUpdate(refreshRate);
        }
    }
}

void HgmConfigCallbackManager::UnRegisterHgmConfigChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (animDynamicCfgCallbacks_.find(pid) != animDynamicCfgCallbacks_.end()) {
        animDynamicCfgCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove a remote callback succeed.", __func__);
    }

    if (refreshRateModeCallbacks_.find(pid) != refreshRateModeCallbacks_.end()) {
        refreshRateModeCallbacks_.erase(pid);
        HGM_LOGD("HgmRefreshRateModeCallbackManager %{public}s : remove a remote callback succeed.", __func__);
    }
}
} // namespace OHOS::Rosen