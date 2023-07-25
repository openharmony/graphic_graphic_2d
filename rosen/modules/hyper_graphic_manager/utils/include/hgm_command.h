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

#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {

enum OledRefreshRate {
    OLED_NULL_HZ = 0,
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
};

enum HgmXmlNode {
    HGM_XML_UNDEFINED = 0,
    HGM_XML_PARAM,
    HGM_XML_PARAMS,
};

enum Animation {
    HGM_ANIMATION_ZOOM = 0,
    HGM_ANIMAION_TRANS,
    HGM_ANIMAION_UI,
    HGM_ANIMAION_OTHERS,
};

enum RefreshRateMode {
    HGM_REFRESHRATE_MODE_AUTO = -1,
    HGM_REFRESHRATE_MODE_NULL = 0,
    HGM_REFRESHRATE_MODE_LOW = 1,
    HGM_REFRESHRATE_MODE_MEDIUM,
    HGM_REFRESHRATE_MODE_HIGH,
};

class ParsedConfigData {
public:
    ParsedConfigData() = default;
    ~ParsedConfigData() = default;

    std::string isDynamicFrameRateEnable_ = "0";
    std::string defaultRefreshRateMode_ = "-1";
    struct detailedStrat {
        std::string name;
        std::string isDynamic;
        std::string min;
        std::string max;
    };
    std::unordered_map<std::string, std::string> customerSettingConfig_;
    std::unordered_map<std::string, detailedStrat> detailedStrategies_;
    std::unordered_map<std::string, std::string> animationDynamicStrats_;
    std::unordered_map<std::string, std::string> bundle_black_list_;
    std::unordered_map<std::string, std::string> bundle_white_list_;
};
} // namespace OHOS
#endif // HGM_COMMAND_H