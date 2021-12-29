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
int RSLog::Output(RSLog::Level level, const char* format, ...)
{
    std::string levelStr;
    switch (level) {
        case LEVEL_INFO:
            levelStr = "INFO";
            break;
        case LEVEL_DEBUG:
            levelStr = "DEBUG";
            break;
        case LEVEL_WARN:
            levelStr = "WARN";
            break;
        case LEVEL_ERROR:
            levelStr = "ERROR";
            break;
        case LEVEL_FATAL:
            levelStr = "FATAL";
            break;
        default:
            break;
    }
    printf("[%s][%s](%s)(%d)", levelStr.c_str(), tag_.c_str(), __FUNCTION__, __LINE__);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    return 0;
}
} // namespace Rosen
} // namespace OHOS
