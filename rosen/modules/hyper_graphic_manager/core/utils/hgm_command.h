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

#ifndef HGM_COMMAND_H
#define HGM_COMMAND_H

#include <inttypes.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "screen_manager/screen_types.h"
#include "animation/rs_frame_rate_range.h"

namespace OHOS::Rosen {

constexpr int UNI_APP_PID = -1;
constexpr int32_t HGM_REFRESHRATE_MODE_AUTO = -1;
constexpr pid_t DEFAULT_PID = 0;

enum OledRefreshRate {
    OLED_NULL_HZ = 0,
    OLED_MIN_HZ = 1,
    OLED_1_HZ = 1,
    OLED_10_HZ = 10,
    OLED_20_HZ = 20,
    OLED_30_HZ = 30,
    OLED_40_HZ = 40,
    OLED_60_HZ = 60,
    OLED_90_HZ = 90,
    OLED_120_HZ = 120,
    OLED_144_HZ = 144,
    OLED_MAX_HZ = 1000,
};

enum HgmErrCode {
    HGM_ERROR = -1,

    EXEC_SUCCESS = 0,

    HGM_NO_SCREEN = 100,
    HGM_BASE_REMOVE_FAILED,

    XML_PARSER_CREATE_FAIL = 200,
    XML_FILE_LOAD_FAIL,
    XML_GET_ROOT_FAIL,
    XML_PARSE_INTERNAL_FAIL,

    HGM_SCREEN_MODE_EXIST,
    HGM_SCREEN_PARAM_ERROR,
    FINAL_RANGE_NOT_VALID,
};

enum HgmXmlNode {
    HGM_XML_UNDEFINED = 0,
    HGM_XML_PARAM,
    HGM_XML_PARAMS,
};

enum class SceneType {
    SCREEN_RECORD,
};

enum DynamicModeType : int32_t {
    TOUCH_DISENABLED = 0,
    TOUCH_ENABLED = 1,
};

enum MultiAppStrategyType {
    USE_MAX,
    FOLLOW_FOCUS,
    USE_STRATEGY_NUM,
};

class PolicyConfigData {
public:
    PolicyConfigData() = default;
    ~PolicyConfigData() = default;

    struct StrategyConfig {
        int32_t min;
        int32_t max;
        DynamicModeType dynamicMode;
        int32_t drawMin;
        int32_t drawMax;
        int32_t down;
    };
    // <"1", StrategyConfig>
    using StrategyConfigMap = std::unordered_map<std::string, StrategyConfig>;

    struct SceneConfig {
        std::string strategy;
        std::string priority;
    };
    // <"SCENE_APP_START_ANIMATION", SceneConfig>
    using SceneConfigMap = std::unordered_map<std::string, SceneConfig>;

    struct DynamicConfig {
        int32_t min;
        int32_t max;
        int32_t preferred_fps;
    };
    // <"1", DynamicConfig>
    using DynamicSetting = std::unordered_map<std::string, DynamicConfig>;
    // <"translate", DynamicSetting>
    using DynamicSettingMap = std::unordered_map<std::string, DynamicSetting>;

    struct ScreenSetting {
        std::string strategy;
        // <"switch", "1">
        std::unordered_map<std::string, std::string> ltpoConfig;
        // <"pkgName", "4">
        std::unordered_map<std::string, std::string> appList;
        MultiAppStrategyType multiAppStrategyType;
        std::string multiAppStrategyName;
        SceneConfigMap sceneList;
        DynamicSettingMap animationDynamicSettings;
        DynamicSettingMap aceSceneDynamicSettings;
    };
    // <"-1", ScreenSetting>
    using ScreenConfig = std::unordered_map<std::string, ScreenSetting>;
    // <"LTPO-DEFAULT", ScreenConfig>
    using ScreenConfigMap = std::unordered_map<std::string, ScreenConfig>;

    std::string defaultRefreshRateMode_ = "-1";
    // <"120", "1">
    std::vector<std::pair<int32_t, int32_t>> refreshRateForSettings_;
    bool xmlCompatibleMode_ = false;
    // <"VIRTUAL_AXX", "4">
    std::unordered_map<std::string, std::string> virtualDisplayConfigs_;
    bool virtualDisplaySwitch_;
    // <"screen0_LTPO", "LTPO-DEFAULT">
    std::unordered_map<std::string, std::string> screenStrategyConfigs_;
    StrategyConfigMap strategyConfigs_;
    ScreenConfigMap screenConfigs_;

    DynamicSettingMap GetAceSceneDynamicSettingMap(const std::string& screenType, const std::string& settingMode)
    {
        if (screenConfigs_.count(screenType) && screenConfigs_[screenType].count(settingMode)) {
            return screenConfigs_[screenType][settingMode].aceSceneDynamicSettings;
        } else {
            return {};
        }
    }

    int32_t SettingModeId2XmlModeId(int32_t settingModeId)
    {
        if (settingModeId < 0 || settingModeId >= static_cast<int32_t>(refreshRateForSettings_.size())) {
            return 0;
        }
        return refreshRateForSettings_[settingModeId].second;
    }

    int32_t XmlModeId2SettingModeId(int32_t xmlModeId)
    {
        auto iter = std::find_if(refreshRateForSettings_.begin(), refreshRateForSettings_.end(),
            [=] (auto nameModeId) { return nameModeId.second == xmlModeId; });
        if (iter != refreshRateForSettings_.end()) {
            return static_cast<int32_t>(iter - refreshRateForSettings_.begin());
        }
        return 0;
    }

    int32_t GetRefreshRateModeName(int32_t refreshRateModeId)
    {
        auto iter = std::find_if(refreshRateForSettings_.begin(), refreshRateForSettings_.end(),
            [=] (auto nameModeId) { return nameModeId.second == refreshRateModeId; });
        if (iter != refreshRateForSettings_.end()) {
            return iter->first;
        }
        return 0;
    }
};
} // namespace OHOS
#endif // HGM_COMMAND_H