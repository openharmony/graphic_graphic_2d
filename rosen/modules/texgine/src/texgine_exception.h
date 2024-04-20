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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_EXCEPTION_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_EXCEPTION_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define TEXGINE_EXCEPTION(name) te##name

enum class ExceptionType {
    CUSTOM = -1,
    SUCCESS,
    INVALID_ARGUMENT,
    API_FAILED,
    ERROR_STATUS,
    OUT_OF_RANGE,
    INVALID_CHAR_GROUPS,
    NULLPTR,
};

struct TexgineException {
    ExceptionType code = ExceptionType::CUSTOM;
    std::string message = "";
};

static inline const TexgineException TEXGINE_EXCEPTION(INVALID_ARGUMENT) = {
    .code = ExceptionType::INVALID_ARGUMENT,
    .message = "Invalid Argument"
};
static inline const TexgineException TEXGINE_EXCEPTION(API_FAILED) = {
    .code = ExceptionType::API_FAILED,
    .message = "API Failed"
};
static inline const TexgineException TEXGINE_EXCEPTION(ERROR_STATUS) = {
    .code = ExceptionType::ERROR_STATUS,
    .message = "Error Status"
};
static inline const TexgineException TEXGINE_EXCEPTION(OUT_OF_RANGE) = {
    .code = ExceptionType::OUT_OF_RANGE,
    .message = "Out Of Range"
};
static inline const TexgineException TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS) = {
    .code = ExceptionType::INVALID_CHAR_GROUPS,
    .message = "Invalid CharGroups"
};
static inline const TexgineException TEXGINE_EXCEPTION(NULLPTR) = {
    .code = ExceptionType::NULLPTR,
    .message = "Null Pointer"
};

TexgineException CustomException(const char *msg);
TexgineException APIFailedException(const char *msg);
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_EXCEPTION_H
