/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_COLOR_SPACE_UTILS_H
#define OHOS_JS_COLOR_SPACE_UTILS_H

#include <map>
#include <hilog/log.h>

#include "color_space.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace ColorManager {
#ifndef TITLE
#define TITLE __func__
#endif

constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD001400, "JsColorSpace"};
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
    CUSTOM = 5,
    TYPE_END
};

const std::map<ColorSpaceName, ApiColorSpaceType> NATIVE_JS_TO_COLOR_SPACE_TYPE_MAP {
    { ColorSpaceName::NONE, ApiColorSpaceType::UNKNOWN },
    { ColorSpaceName::ADOBE_RGB, ApiColorSpaceType::ADOBE_RGB_1998 },
    { ColorSpaceName::DCI_P3, ApiColorSpaceType::DCI_P3 },
    { ColorSpaceName::DISPLAY_P3, ApiColorSpaceType::DISPLAY_P3 },
    { ColorSpaceName::SRGB, ApiColorSpaceType::SRGB },
    { ColorSpaceName::CUSTOM, ApiColorSpaceType::CUSTOM },
};

const std::map<ApiColorSpaceType, ColorSpaceName> JS_TO_NATIVE_COLOR_SPACE_NAME_MAP {
    { ApiColorSpaceType::UNKNOWN, ColorSpaceName::NONE },
    { ApiColorSpaceType::ADOBE_RGB_1998, ColorSpaceName::ADOBE_RGB },
    { ApiColorSpaceType::DCI_P3, ColorSpaceName::DCI_P3 },
    { ApiColorSpaceType::DISPLAY_P3, ColorSpaceName::DISPLAY_P3 },
    { ApiColorSpaceType::SRGB, ColorSpaceName::SRGB },
    { ApiColorSpaceType::CUSTOM, ColorSpaceName::CUSTOM },
};

    NativeValue* ColorSpaceTypeInit(NativeEngine* engine);
    bool ParseJsDoubleValue(NativeObject* jsObject, NativeEngine& engine, const std::string& name, double& data);
}  // namespace ColorManager
}  // namespace OHOS
#endif // OHOS_JS_COLOR_SPACE_UTILS_H
