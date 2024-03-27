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

#define RS_OPTIONAL_TRACE_NAME_FMT(fmt, ...)                                \
    do {                                                                    \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled()) {            \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)

#ifdef ROSEN_TRACE_DISABLE
#define RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(Level, fmt, ...)                   \
    do {                                                                    \
        (void)TRACE_LEVEL_TWO;                                              \
    } while (0)
#else
#define RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(Level, fmt, ...)                   \
    do {                                                                    \
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= Level) {     \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)
#endif

#define RS_APPOINTED_TRACE_BEGIN(node, name)                           \
    do {                                                             \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled() ||     \
            Rosen::RSSystemProperties::FindNodeInTargetList(node)) { \
            RS_TRACE_BEGIN(name);                                         \
        }                                                            \
    } while (0)

#define RS_APPOINTED_TRACE_END(node)                                  \
    do {                                                             \
        if (Rosen::RSSystemProperties::GetDebugTraceEnabled() ||     \
            Rosen::RSSystemProperties::FindNodeInTargetList(node)) { \
            RS_TRACE_END();                                          \
        }                                                            \
    } while (0)

#define RS_OPTIONAL_TRACE_NAME(name) RSOptionalTrace optionalTrace(name)

#define RS_OPTIONAL_TRACE_FUNC() RSOptionalTrace optionalTrace(__func__)

#define RS_PROCESS_TRACE(forceEnable, name) RSProcessTrace processTrace(forceEnable, name)

class RSOptionalTrace {
public:
    RSOptionalTrace(const std::string& traceStr)
    {
        debugTraceEnable_ = OHOS::Rosen::RSSystemProperties::GetDebugTraceEnabled();
        if (debugTraceEnable_) {
            RS_TRACE_BEGIN(traceStr);
        }
    }
    ~RSOptionalTrace()
    {
        if (debugTraceEnable_) {
            RS_TRACE_END();
        }
    }

private:
    bool debugTraceEnable_ = false;
};

class RSProcessTrace {
public:
    RSProcessTrace(bool forceEnable, const std::string& traceStr)
    {
        debugTraceEnable_ = OHOS::Rosen::RSSystemProperties::GetDebugTraceEnabled();
        forceEnable_ = forceEnable;
        if (debugTraceEnable_ || forceEnable_) {
            RS_TRACE_BEGIN(traceStr);
        }
    }
    ~RSProcessTrace()
    {
        if (debugTraceEnable_ || forceEnable_) {
            RS_TRACE_END();
        }
    }
private:
    bool debugTraceEnable_ = false;
    bool forceEnable_ = false;
};
#endif // RENDER_SERVICE_BASE_COMMON_OPTIONAL_TRACE