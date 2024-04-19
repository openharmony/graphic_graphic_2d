/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_CJ_COLOR_MGR_UTILS_H
#define OHOS_CJ_COLOR_MGR_UTILS_H

#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <hilog/log.h>
#include "color_space.h"

#define FFI_EXPORT __attribute__((visibility("default")))

namespace OHOS {
namespace ColorManager {
#ifndef TITLE
#define TITLE __func__
#endif

constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD001400, "CjColorSpace"};
#define CMLOGE(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)
#define CMLOGI(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)

enum class ApiColorSpaceType : uint32_t {
    UNKNOWN = 0,
    ADOBE_RGB_1998 = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_1998_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    CUSTOM = 5,
    TYPE_END = 26
};

const std::map<ColorSpaceName, ApiColorSpaceType> NATIVE_TO_CJ_COLOR_SPACE_TYPE_MAP {
    { ColorSpaceName::NONE, ApiColorSpaceType::UNKNOWN },
    { ColorSpaceName::ADOBE_RGB, ApiColorSpaceType::ADOBE_RGB_1998 },
    { ColorSpaceName::DCI_P3, ApiColorSpaceType::DCI_P3 },
    { ColorSpaceName::DISPLAY_P3, ApiColorSpaceType::DISPLAY_P3 },
    { ColorSpaceName::SRGB, ApiColorSpaceType::SRGB },
    { ColorSpaceName::BT709, ApiColorSpaceType::BT709 },
    { ColorSpaceName::BT601_EBU, ApiColorSpaceType::BT601_EBU },
    { ColorSpaceName::BT601_SMPTE_C, ApiColorSpaceType::BT601_SMPTE_C },
    { ColorSpaceName::BT2020_HLG, ApiColorSpaceType::BT2020_HLG },
    { ColorSpaceName::BT2020_PQ, ApiColorSpaceType::BT2020_PQ },
    { ColorSpaceName::P3_HLG, ApiColorSpaceType::P3_HLG },
    { ColorSpaceName::P3_PQ, ApiColorSpaceType::P3_PQ },
    { ColorSpaceName::ADOBE_RGB_LIMIT, ApiColorSpaceType::ADOBE_RGB_1998_LIMIT },
    { ColorSpaceName::DISPLAY_P3_LIMIT, ApiColorSpaceType::DISPLAY_P3_LIMIT },
    { ColorSpaceName::SRGB_LIMIT, ApiColorSpaceType::SRGB_LIMIT },
    { ColorSpaceName::BT709_LIMIT, ApiColorSpaceType::BT709_LIMIT },
    { ColorSpaceName::BT601_EBU_LIMIT, ApiColorSpaceType::BT601_EBU_LIMIT },
    { ColorSpaceName::BT601_SMPTE_C_LIMIT, ApiColorSpaceType::BT601_SMPTE_C_LIMIT },
    { ColorSpaceName::BT2020_HLG_LIMIT, ApiColorSpaceType::BT2020_HLG_LIMIT },
    { ColorSpaceName::BT2020_PQ_LIMIT, ApiColorSpaceType::BT2020_PQ_LIMIT },
    { ColorSpaceName::P3_HLG_LIMIT, ApiColorSpaceType::P3_HLG_LIMIT },
    { ColorSpaceName::P3_PQ_LIMIT, ApiColorSpaceType::P3_PQ_LIMIT },
    { ColorSpaceName::LINEAR_P3, ApiColorSpaceType::LINEAR_P3 },
    { ColorSpaceName::LINEAR_SRGB, ApiColorSpaceType::LINEAR_SRGB },
    { ColorSpaceName::LINEAR_BT709, ApiColorSpaceType::LINEAR_BT709 },
    { ColorSpaceName::LINEAR_BT2020, ApiColorSpaceType::LINEAR_BT2020 },
    { ColorSpaceName::DISPLAY_SRGB, ApiColorSpaceType::DISPLAY_SRGB },
    { ColorSpaceName::DISPLAY_P3_SRGB, ApiColorSpaceType::DISPLAY_P3_SRGB },
    { ColorSpaceName::DISPLAY_P3_HLG, ApiColorSpaceType::DISPLAY_P3_HLG },
    { ColorSpaceName::DISPLAY_P3_PQ, ApiColorSpaceType::DISPLAY_P3_PQ },
    { ColorSpaceName::CUSTOM, ApiColorSpaceType::CUSTOM },
};

const std::map<ApiColorSpaceType, ColorSpaceName> CJ_TO_NATIVE_COLOR_SPACE_NAME_MAP {
    { ApiColorSpaceType::UNKNOWN, ColorSpaceName::NONE },
    { ApiColorSpaceType::ADOBE_RGB_1998, ColorSpaceName::ADOBE_RGB },
    { ApiColorSpaceType::DCI_P3, ColorSpaceName::DCI_P3 },
    { ApiColorSpaceType::DISPLAY_P3, ColorSpaceName::DISPLAY_P3 },
    { ApiColorSpaceType::SRGB, ColorSpaceName::SRGB },
    { ApiColorSpaceType::BT709, ColorSpaceName::BT709 },
    { ApiColorSpaceType::BT601_EBU, ColorSpaceName::BT601_EBU },
    { ApiColorSpaceType::BT601_SMPTE_C, ColorSpaceName::BT601_SMPTE_C },
    { ApiColorSpaceType::BT2020_HLG, ColorSpaceName::BT2020_HLG },
    { ApiColorSpaceType::BT2020_PQ, ColorSpaceName::BT2020_PQ },
    { ApiColorSpaceType::P3_HLG, ColorSpaceName::P3_HLG },
    { ApiColorSpaceType::P3_PQ, ColorSpaceName::P3_PQ },
    { ApiColorSpaceType::ADOBE_RGB_1998_LIMIT, ColorSpaceName::ADOBE_RGB_LIMIT },
    { ApiColorSpaceType::DISPLAY_P3_LIMIT, ColorSpaceName::DISPLAY_P3_LIMIT },
    { ApiColorSpaceType::SRGB_LIMIT, ColorSpaceName::SRGB_LIMIT },
    { ApiColorSpaceType::BT709_LIMIT, ColorSpaceName::BT709_LIMIT },
    { ApiColorSpaceType::BT601_EBU_LIMIT, ColorSpaceName::BT601_EBU_LIMIT },
    { ApiColorSpaceType::BT601_SMPTE_C_LIMIT, ColorSpaceName::BT601_SMPTE_C_LIMIT },
    { ApiColorSpaceType::BT2020_HLG_LIMIT, ColorSpaceName::BT2020_HLG_LIMIT },
    { ApiColorSpaceType::BT2020_PQ_LIMIT, ColorSpaceName::BT2020_PQ_LIMIT },
    { ApiColorSpaceType::P3_HLG_LIMIT, ColorSpaceName::P3_HLG_LIMIT },
    { ApiColorSpaceType::P3_PQ_LIMIT, ColorSpaceName::P3_PQ_LIMIT },
    { ApiColorSpaceType::LINEAR_P3, ColorSpaceName::LINEAR_P3 },
    { ApiColorSpaceType::LINEAR_SRGB, ColorSpaceName::LINEAR_SRGB },
    { ApiColorSpaceType::LINEAR_BT709, ColorSpaceName::LINEAR_BT709 },
    { ApiColorSpaceType::LINEAR_BT2020, ColorSpaceName::LINEAR_BT2020 },
    { ApiColorSpaceType::DISPLAY_SRGB, ColorSpaceName::DISPLAY_SRGB },
    { ApiColorSpaceType::DISPLAY_P3_SRGB, ColorSpaceName::DISPLAY_P3_SRGB },
    { ApiColorSpaceType::DISPLAY_P3_HLG, ColorSpaceName::DISPLAY_P3_HLG },
    { ApiColorSpaceType::DISPLAY_P3_PQ, ColorSpaceName::DISPLAY_P3_PQ },
    { ApiColorSpaceType::CUSTOM, ColorSpaceName::CUSTOM },
};

enum class CMError : int32_t {
    CM_OK = 0,
    CM_ERROR_NULLPTR,
    CM_ERROR_INVALID_PARAM,
    CM_ERROR_INVALID_ENUM_USAGE,
};

enum class CMErrorCode : int32_t {
    CM_OK = 0,
    CM_ERROR_NO_PERMISSION = 201, // the value do not change. It is defined on all system
    CM_ERROR_INVALID_PARAM = 401, // the value do not change. It is defined on all system
    CM_ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change. It is defined on all system
    CM_ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
};

const std::map<CMError, CMErrorCode> CJ_TO_ERROR_CODE_MAP {
    { CMError::CM_OK, CMErrorCode::CM_OK },
    { CMError::CM_ERROR_NULLPTR, CMErrorCode::CM_ERROR_INVALID_PARAM },
    { CMError::CM_ERROR_INVALID_PARAM, CMErrorCode::CM_ERROR_INVALID_PARAM },
    { CMError::CM_ERROR_INVALID_ENUM_USAGE, CMErrorCode::CM_ERROR_ABNORMAL_PARAM_VALUE },
};
}
}

class FFI_EXPORT Utils {
public:
    static char* MallocCString(const std::string& origin);
};
#endif