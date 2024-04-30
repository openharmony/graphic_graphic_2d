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
#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "hgm_command.h"
#include "hgm_touch_manager.h"
#include "pipeline/rs_render_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
class HgmMultiAppStrategy final {
public:
    using StrategyChangeCallback = std::function<void(const PolicyConfigData::StrategyConfig&)>;
    using TouchInfo = std::pair<std::string, TouchState>;

    HgmMultiAppStrategy();
    ~HgmMultiAppStrategy() = default;

    HgmErrCode HandlePkgsEvent(const std::vector<std::string>& pkgs);
    void HandleTouchInfo(const std::string& pkgName, TouchState touchState);

    void CalcVote();
    HgmErrCode GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes);

    void RegisterStrategyChangeCallback(const StrategyChangeCallback& callback);

    const std::vector<std::string>& GetPackages() const { return pkgs_; }
    void UpdateXmlConfigCache();
    PolicyConfigData::ScreenSetting& GetScreenSetting() const { return screenSettingCache_; }
    PolicyConfigData::StrategyConfigMap& GetStrategyConfigs() const { return strategyConfigMapCache_; }

private:
    void UseStrategyNum();
    void FollowFocus();
    void UseMax();

    HgmErrCode GetPkgNameByPid(pid_t pid, std::string& pkgNameRes) const;
    static std::pair<std::string, pid_t> AnalyzePkgParam(const std::string& param);
    void UpdateStrategyByTouch(
        PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate = false);
    void OnStrategyChange();

    std::vector<std::string> pkgs_;
    std::unordered_map<pid_t, std::string> pidPkgNameMap_;
    std::pair<HgmErrCode, PolicyConfigData::StrategyConfig> voteRes_ = { HGM_ERROR, {
        .min = OledRefreshRate::OLED_NULL_HZ,
        .max = OledRefreshRate::OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .drawMin = OledRefreshRate::OLED_NULL_HZ,
        .drawMax = OledRefreshRate::OLED_120_HZ,
        .down = OledRefreshRate::OLED_120_HZ,
    }};
    TouchInfo touchInfo_ = { "", TouchState::IDLE }; // pkgName, touchState
    std::unique_ptr<TouchInfo> uniqueTouchInfo_ = nullptr;
    std::vector<StrategyChangeCallback> strategyChangeCallbacks_;

    PolicyConfigData::ScreenSetting& screenSettingCache_;
    PolicyConfigData::StrategyConfigMap& strategyConfigMapCache_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_MULTI_APP_STRATEGY_H