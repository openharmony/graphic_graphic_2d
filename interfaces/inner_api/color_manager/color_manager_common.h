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

#ifndef OHOS_ROSEN_COLOR_MANAGER_COMMON_H
#define OHOS_ROSEN_COLOR_MANAGER_COMMON_H

#include <cstdint>

namespace OHOS {
namespace ColorManager {
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

const std::map<CMError, CMErrorCode> JS_TO_ERROR_CODE_MAP {
    { CMError::CM_OK, CMErrorCode::CM_OK },
    { CMError::CM_ERROR_NULLPTR, CMErrorCode::CM_ERROR_INVALID_PARAM },
    { CMError::CM_ERROR_INVALID_PARAM, CMErrorCode::CM_ERROR_INVALID_PARAM },
    { CMError::CM_ERROR_INVALID_ENUM_USAGE, CMErrorCode::CM_ERROR_ABNORMAL_PARAM_VALUE },
};
} // namespace ColorManager
} // namespace OHOS

#endif // OHOS_ROSEN_COLOR_MANAGER_COMMON_H