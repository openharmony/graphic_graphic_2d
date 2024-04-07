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

#ifndef HGM_CONFIG_CALLBACK_MANAGER_H
#define HGM_CONFIG_CALLBACK_MANAGER_H

#include <list>
#include <mutex>
#include <unordered_map>

#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "refbase.h"

namespace OHOS::Rosen {
using PidToRefreshRateModeCallback = std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>>;
class HgmConfigCallbackManager : public RefBase {
public:
    static sptr<HgmConfigCallbackManager> GetInstance() noexcept;

    // non-copyable
    HgmConfigCallbackManager(const HgmConfigCallbackManager &) = delete;
    HgmConfigCallbackManager &operator=(const HgmConfigCallbackManager &) = delete;

    void RegisterHgmConfigChangeCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void RegisterHgmRefreshRateModeChangeCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void RegisterHgmRefreshRateUpdateCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void SyncHgmConfigChangeCallback();
    void SyncRefreshRateModeChangeCallback(int32_t refreshRateMode);
    void SyncRefreshRateUpdateCallback(int32_t refreshRate);
    void UnRegisterHgmConfigChangeCallback(pid_t pid);

private:
    HgmConfigCallbackManager();
    ~HgmConfigCallbackManager() noexcept override;
    std::mutex mtx_;
    static std::once_flag createFlag_;
    static sptr<HgmConfigCallbackManager> instance_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> animDynamicCfgCallbacks_;
    PidToRefreshRateModeCallback refreshRateModeCallbacks_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> refreshRateUpdateCallbacks_;
};
} // namespace OHOS::Rosen
#endif // HGM_CONFIG_CALLBACK_MANAGER_H