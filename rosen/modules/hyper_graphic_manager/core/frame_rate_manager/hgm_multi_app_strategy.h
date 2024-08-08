/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef HGM_MULTI_APP_STRATEGY_H
#define HGM_MULTI_APP_STRATEGY_H

#include <functional>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/rs_common_def.h"
#include "hgm_command.h"
#include "hgm_lru_cache.h"
#include "hgm_touch_manager.h"
#include "pipeline/rs_render_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
constexpr int32_t DEFAULT_APP_TYPE = -1;

class HgmMultiAppStrategy final {
public:
    using StrategyChangeCallback = std::function<void(const PolicyConfigData::StrategyConfig&)>;

    HgmMultiAppStrategy();
    ~HgmMultiAppStrategy() = default;

    struct TouchInfo {
        std::string pkgName;
        TouchState touchState;
        int32_t upExpectFps;
    };

    HgmErrCode HandlePkgsEvent(const std::vector<std::string>& pkgs);
    void HandleTouchInfo(const TouchInfo& touchInfo);
    void HandleLightFactorStatus(bool isSafe);

    void CalcVote();
    HgmErrCode GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes) const;

    void RegisterStrategyChangeCallback(const StrategyChangeCallback& callback);
    bool CheckPidValid(pid_t pid);

    std::string GetAppStrategyConfigName(const std::string& pkgName);
    HgmErrCode GetFocusAppStrategyConfig(PolicyConfigData::StrategyConfig& strategyRes);
    std::unordered_map<std::string, std::pair<pid_t, int32_t>> GetPidAppType();
    std::unordered_map<pid_t, std::pair<int32_t, std::string>> GetForegroundPidApp();
    HgmLRUCache<pid_t> GetBackgroundPid();
    std::vector<std::string> GetPackages();
    void CleanApp(pid_t pid);
    void UpdateXmlConfigCache();
    PolicyConfigData::ScreenSetting GetScreenSetting();
    void SetScreenSetting(const PolicyConfigData::ScreenSetting& screenSetting);
    PolicyConfigData::StrategyConfigMap GetStrategyConfigs();
    void SetStrategyConfigs(const PolicyConfigData::StrategyConfigMap& strategyConfigs);
    HgmErrCode GetStrategyConfig(const std::string& strategyName, PolicyConfigData::StrategyConfig& strategyRes);
    HgmErrCode GetAppStrategyConfig(const std::string& pkgName, PolicyConfigData::StrategyConfig& strategyRes);

    static std::tuple<std::string, pid_t, int32_t> AnalyzePkgParam(const std::string& param);

    // use in temporary scheme with background alpha
    void CheckPackageInConfigList(const std::vector<std::string>& pkgs);
private:
    void UseStrategyNum();
    void FollowFocus();
    void UseMax();

    void OnLightFactor(PolicyConfigData::StrategyConfig& strategyRes) const;
    void UpdateStrategyByTouch(
        PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate = false);
    void OnStrategyChange();

    std::mutex pkgsMutex_;
    std::vector<std::string> pkgs_;
    std::unordered_map<std::string, std::pair<pid_t, int32_t>> pidAppTypeMap_;
    std::unordered_map<pid_t, std::pair<int32_t, std::string>> foregroundPidAppMap_;
    HgmLRUCache<pid_t> backgroundPid_{ 100 }; // max nums of pkgs that can be stored is 100
    std::mutex pidAppTypeMutex_;
    std::pair<HgmErrCode, PolicyConfigData::StrategyConfig> voteRes_ = { HGM_ERROR, {
        .min = OledRefreshRate::OLED_NULL_HZ,
        .max = OledRefreshRate::OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .drawMin = OledRefreshRate::OLED_NULL_HZ,
        .drawMax = OledRefreshRate::OLED_120_HZ,
        .down = OledRefreshRate::OLED_120_HZ,
    }};
    std::mutex touchInfoMutex_;
    TouchInfo touchInfo_ = { "", TouchState::IDLE_STATE, OLED_120_HZ }; // pkgName, touchState
    std::unique_ptr<TouchInfo> uniqueTouchInfo_ = nullptr;
    std::atomic<bool> lightFactorStatus_{false};
    std::vector<StrategyChangeCallback> strategyChangeCallbacks_;

    std::mutex updateCacheMutex_;
    PolicyConfigData::ScreenSetting& screenSettingCache_;
    PolicyConfigData::StrategyConfigMap& strategyConfigMapCache_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_MULTI_APP_STRATEGY_H