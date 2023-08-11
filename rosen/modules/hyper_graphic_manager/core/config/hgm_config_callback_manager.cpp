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

#include "anim_dynamic_configs.h"
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
}

void HgmConfigCallbackManager::RegisterHgmConfigChangeCallback(
    const sptr<RSIHgmConfigChangeCallback> &callback)
{
    if (callback == nullptr) {
        HGM_LOGE("HgmConfigCallbackManager %{public}s : callback is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    animDynamicCfgCallbacks_.push_back(callback);
    HGM_LOGD("HgmConfigCallbackManager %{public}s : add a remote callback succeed.", __func__);

    auto data = std::make_shared<RSHgmConfigData>();
    auto configs = AnimDynamicConfigs::GetInstance()->GetAnimDynamicConfigs();
    for (auto config : configs) {
        data->AddConfigItem({config.GetAnimType(), config.GetAnimName(), config.GetMinSpeed(),
            config.GetMaxSpeed(), config.GetPreferredFps()});
    }
    callback->OnHgmConfigChanged(data);
}

void HgmConfigCallbackManager::UnRegisterHgmConfigChangeCallback(
    const sptr<RSIHgmConfigChangeCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto iter = animDynamicCfgCallbacks_.begin(); iter != animDynamicCfgCallbacks_.end(); iter++) {
        if (*iter == callback) {
            animDynamicCfgCallbacks_.erase(iter);
            HGM_LOGD("HgmConfigCallbackManager %s : remove a remote callback succeed.", __func__);
            break;
        }
    }
}
} // namespace OHOS::Rosen