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
 
#include <string>


#define ROSEN_TRACE_BEGIN(tag, name) OHOS::Rosen::RosenTraceBegin(name)
#define RS_TRACE_BEGIN(name) OHOS::Rosen::RosenTraceBegin(name)
#define ROSEN_TRACE_END(tag) OHOS::Rosen::RosenTraceEnd()
#define RS_TRACE_END() OHOS::Rosen::RosenTraceEnd()
#define RS_TRACE_NAME(name) OHOS::Rosen::ScopedTrace ___tracer(name)
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__FUNCTION__)

namespace OHOS {
namespace Rosen {

void RosenTraceBegin(const char* name);
void RosenTraceBegin(std::string name);
void RosenTraceEnd();
class ScopedTrace {
    public:
      explicit ScopedTrace(const char *name);

      explicit ScopedTrace(std::string name);

      ~ScopedTrace();
};
} // namespace Rosen
} // namespace OHOS