/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"

#include <cstdarg>
#include <cstdio>

namespace OHOS {
namespace Rosen {
void RSLogOutput(RSLog::Tag tag, RSLog::Level level, const char* format, ...)
{
    std::string levelStr;
    switch (level) {
        case RSLog::LEVEL_INFO:
            levelStr = "INFO";
            break;
        case RSLog::LEVEL_DEBUG:
            levelStr = "DEBUG";
            break;
        case RSLog::LEVEL_WARN:
            levelStr = "WARN";
            break;
        case RSLog::LEVEL_ERROR:
            levelStr = "ERROR";
            break;
        case RSLog::LEVEL_FATAL:
            levelStr = "FATAL";
            break;
        default:
            break;
    }
    printf("[%s][%s]", levelStr.c_str(), (tag == RSLog::Tag::RS) ? "OHOS::RS" : "OHOS::ROSEN");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
} // namespace Rosen
} // namespace OHOS
