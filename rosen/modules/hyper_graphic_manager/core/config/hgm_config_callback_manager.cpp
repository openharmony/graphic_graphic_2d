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
    for (auto& [animType, dynamicSettingMap] : hgmCore.GetParsedConfigData()->aceSceneDynamicSetting_) {
        for (auto& [animName, dynamicSetting] : dynamicSettingMap) {
            data->AddAnimDynamicItem({animType, animName, dynamicSetting.min, dynamicSetting.max, dynamicSetting.preferred_fps});
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

void HgmConfigCallbackManager::UnRegisterHgmConfigChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (animDynamicCfgCallbacks_.find(pid) != animDynamicCfgCallbacks_.end()) {
        animDynamicCfgCallbacks_.erase(pid);
        HGM_LOGD("HgmConfigCallbackManager %{public}s : remove a remote callback succeed.", __func__);
        return;
    }
    HGM_LOGD("HgmConfigCallbackManager %{public}s : initialization or do not find callback(pid = %d)",
        __func__, static_cast<int>(pid));
}
} // namespace OHOS::Rosen