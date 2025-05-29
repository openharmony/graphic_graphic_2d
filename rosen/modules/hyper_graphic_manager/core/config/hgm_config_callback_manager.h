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

#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "refbase.h"

namespace OHOS::Rosen {
class HgmConfigCallbackManager : public RefBase {
public:
    static sptr<HgmConfigCallbackManager> GetInstance() noexcept;

    // non-copyable
    HgmConfigCallbackManager(const HgmConfigCallbackManager &) = delete;
    HgmConfigCallbackManager &operator=(const HgmConfigCallbackManager &) = delete;

    void RegisterHgmConfigChangeCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void RegisterHgmRefreshRateModeChangeCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void RegisterHgmRefreshRateUpdateCallback(pid_t pid, const sptr<RSIHgmConfigChangeCallback>& callback);
    void RegisterXComponentExpectedFrameRateCallback(pid_t listenerPid, pid_t dstPid,
        const sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback>& callback);
    void SyncHgmConfigChangeCallback();
    void SyncHgmConfigChangeCallback(const std::unordered_map<pid_t, std::pair<int32_t, std::string>>& pids);
    void SyncRefreshRateModeChangeCallback(int32_t refreshRateMode);
    void SyncRefreshRateUpdateCallback(int32_t refreshRate);
    void SyncXComponentExpectedFrameRateCallback(pid_t pid, const std::string& id, int32_t expectedFrameRate);
    void UnRegisterHgmConfigChangeCallback(pid_t pid);

private:
    HgmConfigCallbackManager();
    ~HgmConfigCallbackManager() noexcept override;
    static std::once_flag createFlag_;
    static sptr<HgmConfigCallbackManager> instance_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> animDynamicCfgCallbacks_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> pendingAnimDynamicCfgCallbacks_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> refreshRateModeCallbacks_;
    std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> refreshRateUpdateCallbacks_;
    // <dstPid, <listenerPid, cb>>
    std::unordered_map<pid_t, std::unordered_map<pid_t,
        sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback>>> xcomponentExpectedFrameRateCallbacks_;
    // <dstPid, fps>
    std::unordered_map<pid_t, int32_t> xcomponentExpectedFrameRate_;
};
} // namespace OHOS::Rosen
#endif // HGM_CONFIG_CALLBACK_MANAGER_H