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

#ifndef SCOPED_TRACE_FMT_H
#define SCOPED_TRACE_FMT_H

#include <hitrace_meter.h>
#include "parameters.h"

static bool g_debugTraceEnabled =
    std::atoi((OHOS::system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;

#define SCOPED_DEBUG_TRACE_FMT(fmt, ...)                                    \
    do {                                                                    \
        if (g_debugTraceEnabled) {                                          \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)

#endif // SCOPED_TRACE_FMT_H
