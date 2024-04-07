/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_COMMON_H
#define ROSEN_MODULES_SPTEXT_COMMON_H

#ifndef TXT_TRACE_DISABLE
#include <hitrace_meter.h>
#include <parameters.h>
#endif

namespace OHOS {
#ifdef TXT_TRACE_DISABLE
#define DRAWING_TRACE_NAME_FMT_LEVEL(Level, fmt, ...)                       \
    do {                                                                    \
        (void)Level;                                                        \
    } while (0)
#else
#define DRAWING_TRACE_NAME_FMT_LEVEL(Level, fmt, ...)                       \
    do {                                                                    \
        if (GetDebugTraceLevel() >= Level) {                                \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)

int GetDebugTraceLevel()
{
    static int openDebugTraceLevel =
        std::atoi((OHOS::system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str());
    return openDebugTraceLevel;
}
#endif
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_COMMON_H