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

#ifndef GRAPHIC_RS_TRACE_H
#define GRAPHIC_RS_TRACE_H

#ifndef ROSEN_TRACE_DISABLE
#include "hitrace_meter.h"
#define ROSEN_TRACE_BEGIN(tag, name) StartTrace(tag, name)
#define RS_TRACE_BEGIN(name) ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, name)
#define ROSEN_TRACE_END(tag) FinishTrace(tag)
#define RS_TRACE_END() ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME(name) HITRACE_METER_NAME(HITRACE_TAG_GRAPHIC_AGP, name)
#define RS_TRACE_NAME_FMT(fmt, ...) HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN(name, value) StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_ASYNC_TRACE_END(name, value) FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT(name, value) CountTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__func__)
#else

#ifdef ROSEN_ANDROID
#include <android/trace.h>
#include "platform/common/rs_log.h"
inline void RosenTraceBegin(const char* name)
{
    if (name != nullptr) {
        ATrace_beginSection(name);
    }
}

inline void RosenTraceBegin(std::string name)
{
    ATrace_beginSection(name.c_str());
}
#define ROSEN_TRACE_BEGIN(tag, name) RosenTraceBegin(name)
#define RS_TRACE_BEGIN(name) RosenTraceBegin(name)
#define ROSEN_TRACE_END(tag) ATrace_endSection()
#define RS_TRACE_END() ATrace_endSection()
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)

class ScopedTrace {
    public:
      inline ScopedTrace(const char *name) {
        if (name != nullptr) {
            ATrace_beginSection(name);
        }
      }

      inline ScopedTrace(std::string name) {
          ATrace_beginSection(name.c_str());
      }

      inline ~ScopedTrace() {
          ATrace_endSection();
      }
};

#define RS_TRACE_NAME(name) ScopedTrace ___tracer(name)

// RS_TRACE_FUNC() is an RS_TRACE_NAME that uses the current function name.
#define RS_TRACE_FUNC() RS_TRACE_NAME(__FUNCTION__)
#else

#define ROSEN_TRACE_BEGIN(tag, name)
#define RS_TRACE_BEGIN(name)
#define ROSEN_TRACE_END(tag)
#define RS_TRACE_END()
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_NAME(name)
#define RS_TRACE_FUNC()
#endif //ROSEN_ANDROID
#endif //ROSEN_TRACE_DISABLE

#endif // GRAPHIC_RS_TRACE_H
