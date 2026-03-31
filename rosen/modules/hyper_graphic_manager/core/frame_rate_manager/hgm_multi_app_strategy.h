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

#include <algorithm>
#include <functional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "hgm_command.h"
#include "hgm_lru_cache.h"
#include "hgm_touch_manager.h"

namespace OHOS {
namespace Rosen {
class HgmMultiAppStrategy final {
public:
    using StrategyChangeCallback = std::function<void(const PolicyConfigData::StrategyConfig&)>;

    struct TouchInfo {
        std::string pkgName;
        TouchState touchState;
        int32_t upExpectFps;
    };

    static std::tuple<std::string, pid_t, int32_t> AnalyzePkgParam(const std::string& param);

    HgmMultiAppStrategy();
    ~HgmMultiAppStrategy() noexcept = default;

    HgmErrCode HandlePkgsEvent(const std::vector<std::string>& pkgs);
    void HandleTouchInfo(const TouchInfo& touchInfo);
    void HandleLightFactorStatus(int32_t state);
    void HandleLowAmbientStatus(bool isEffect);
    void SetScreenType(bool isLtpo);

    void CalcVote();
    // Test ONLY
    HgmErrCode GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes) const;

    void RegisterStrategyChangeCallback(StrategyChangeCallback callback);
    bool CheckPidValid(pid_t pid, bool onlyCheckForegroundApp = false);

    std::string GetGameNodeName(const std::string& pkgName);
    std::string GetAppStrategyConfigName(const std::string& pkgName);
    HgmErrCode GetFocusAppStrategyConfig(PolicyConfigData::StrategyConfig& strategyRes);
    const std::unordered_map<pid_t, std::pair<int32_t, std::string>>& GetForegroundPidApp() const;
    // Test ONLY
    HgmLRUCache<pid_t> GetBackgroundPid() const { return backgroundPid_; }
    std::vector<std::string> GetPackages() const { return pkgs_; }
    PolicyConfigData::ScreenSetting GetScreenSetting() const { return screenSettingCache_; }
    // Test ONLY
    PolicyConfigData::StrategyConfigMap GetStrategyConfigs() const { return strategyConfigMapCache_; }
    HgmErrCode GetStrategyConfig(const std::string& strategyName, PolicyConfigData::StrategyConfig& strategyRes);
    HgmErrCode GetAppStrategyConfig(const std::string& pkgName, PolicyConfigData::StrategyConfig& strategyRes);

    // Test ONLY
    void SetScreenSetting(const PolicyConfigData::ScreenSetting& screenSetting) { screenSettingCache_ = screenSetting; }
    // Test ONLY
    void SetStrategyConfigs(const PolicyConfigData::StrategyConfigMap& strategyConfigs);
    void SetDisableSafeVoteValue(bool disableSafeVote) { disableSafeVote_ = disableSafeVote; }

    void CleanApp(pid_t pid);
    void UpdateXmlConfigCache();

    pid_t GetSceneBoardPid() const { return sceneBoardPid_; }

private:
    void UseStrategyNum();
    void FollowFocus();
    void UseMax();

    void OnLightFactor(PolicyConfigData::StrategyConfig& strategyRes) const;
    void UpdateStrategyByTouch(
        PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate = false);
    void OnStrategyChange();

    std::vector<std::string> pkgs_;
    std::unordered_map<std::string, std::pair<pid_t, int32_t>> pidAppTypeMap_;
    std::unordered_map<pid_t, std::pair<int32_t, std::string>> foregroundPidAppMap_;
    HgmLRUCache<pid_t> backgroundPid_{ 100 }; // max nums of pkgs that can be stored is 100

    std::pair<HgmErrCode, PolicyConfigData::StrategyConfig> voteRes_ = { HGM_ERROR, {
        .min = OledRefreshRate::OLED_NULL_HZ,
        .max = OledRefreshRate::OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .idleFps = OledRefreshRate::OLED_NULL_HZ,
        .drawMin = OledRefreshRate::OLED_NULL_HZ,
        .drawMax = OledRefreshRate::OLED_120_HZ,
        .down = OledRefreshRate::OLED_120_HZ,
    }};

    TouchInfo touchInfo_ = { "", TouchState::IDLE_STATE, OLED_120_HZ }; // pkgName, touchState
    std::unique_ptr<TouchInfo> uniqueTouchInfo_ = nullptr;

    std::atomic<int32_t> lightFactorStatus_{ 0 };
    bool lowAmbientStatus_ = false;

    bool isLtpo_ = true;
    std::vector<StrategyChangeCallback> strategyChangeCallbacks_;

    PolicyConfigData::ScreenSetting& screenSettingCache_;
    PolicyConfigData::StrategyConfigMap& strategyConfigMapCache_;
    bool disableSafeVote_ = false;
    pid_t sceneBoardPid_ = DEFAULT_PID;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_MULTI_APP_STRATEGY_H