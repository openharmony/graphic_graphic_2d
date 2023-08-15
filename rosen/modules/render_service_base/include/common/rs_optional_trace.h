/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_OPTIONAL_TRACE
#define RENDER_SERVICE_BASE_COMMON_OPTIONAL_TRACE

#include "rs_trace.h"
#include "platform/common/rs_system_properties.h"

#define RS_OPTIONAL_TRACE_BEGIN(name)                            \
    do {                                                         \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) { \
            RS_TRACE_BEGIN(name);                                \
        }                                                        \
    } while (0)

#define RS_OPTIONAL_TRACE_END()                                  \
    do {                                                         \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) { \
            RS_TRACE_END();                                      \
        }                                                        \
    } while (0)

#define RS_OPTIONAL_TRACE_FUNC_BEGIN()                           \
    do {                                                         \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) { \
            RS_TRACE_BEGIN(__func__);                            \
        }                                                        \
    } while (0)

#define RS_OPTIONAL_TRACE_FUNC_END() RS_OPTIONAL_TRACE_END()

#define RS_OPTIONAL_TRACE_NAME(name)                             \
    do {                                                         \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) { \
            RS_TRACE_NAME(name);                                 \
        }                                                        \
    } while (0)

#define RS_OPTIONAL_TRACE_NAME_FMT(fmt, ...)                                \
    do {                                                                    \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) {            \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)

#endif // RENDER_SERVICE_BASE_COMMON_OPTIONAL_TRACE