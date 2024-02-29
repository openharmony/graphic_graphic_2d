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

#ifndef OHOS_JS_HDR_FORMAT_UTILS_H
#define OHOS_JS_HDR_FORMAT_UTILS_H

#include <map>
#include <string>
#include <hilog/log.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace HdrCapability {
#ifndef TITLE
#define TITLE __func__
#endif

constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD001410, "JsHdrCapability"};
#define HCLOGE(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)
#define HCLOGI(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)

enum class ApiHDRFormat : uint32_t {
    NONE = 0,
    VIDEO_HLG = 1,
    VIDEO_HDR10 = 2,
    VIDEO_HDR_VIVID = 3,
    IMAGE_HDR_VIVID_DUAL = 4,
    IMAGE_HDR_VIVID_SINGLE = 5,
    IMAGE_HDR_ISO_DUAL = 6,
    IMAGE_HDR_ISO_SINGLE = 7,
};

const std::map<std::string, ApiHDRFormat> STRING_TO_JS_MAP = {
    { "NONE", ApiHDRFormat::NONE },
    { "VIDEO_HLG", ApiHDRFormat::VIDEO_HLG },
    { "VIDEO_HDR10", ApiHDRFormat::VIDEO_HDR10 },
    { "VIDEO_HDR_VIVID", ApiHDRFormat::VIDEO_HDR_VIVID },
    { "IMAGE_HDR_VIVID_DUAL", ApiHDRFormat::IMAGE_HDR_VIVID_DUAL },
    { "IMAGE_HDR_VIVID_SINGLE", ApiHDRFormat::IMAGE_HDR_VIVID_SINGLE },
    { "IMAGE_HDR_ISO_DUAL", ApiHDRFormat::IMAGE_HDR_ISO_DUAL },
    { "IMAGE_HDR_ISO_SINGLE", ApiHDRFormat::IMAGE_HDR_ISO_SINGLE },
};

napi_value HDRFormatInit(napi_env env);
}  // namespace HdrCapability
}  // namespace OHOS
#endif // OHOS_JS_HDR_FORMAT_UTILS_H
