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

#define EXCEPTION_LIST(MACRO) \
    MACRO(INVALID_ARGUMENT, "Invalid Argument") \
    MACRO(API_FAILED, "API Failed") \
    MACRO(ERROR_STATUS, "Error Status") \
    MACRO(OUT_OF_RANGE, "Out Of Range") \
    MACRO(INVALID_CHAR_GROUPS, "Invalid CharGroups") \
    MACRO(NULLPTR, "Null Pointer")

enum class ExceptionType {
    CUSTOM = -1,
    SUCCESS,
#define DEFINE_ENUM_MACRO(name, str) name,
    EXCEPTION_LIST(DEFINE_ENUM_MACRO)
#undef DEFINE_ENUM_MACRO
};

struct TexgineException {
    ExceptionType code = ExceptionType::CUSTOM;
    std::string message = "";
};

#define DEFINE_CONSTVAR_MACRO(name, str) \
    static inline const TexgineException TEXGINE_EXCEPTION(name) = {.code = ExceptionType::name, .message = str};
EXCEPTION_LIST(DEFINE_CONSTVAR_MACRO);
#undef DEFINE_CONSTVAR_MACRO
#undef EXCEPTION_LIST

TexgineException CustomException(const char *msg);
TexgineException APIFailedException(const char *msg);
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_EXCEPTION_H
