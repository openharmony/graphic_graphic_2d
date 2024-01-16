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

#ifndef TRACE_MEMORY_DUMP_H
#define TRACE_MEMORY_DUMP_H

#include "impl_interface/trace_memory_dump_impl.h"
#include "memory/rs_dfx_string.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TraceMemoryDump {
public:
    TraceMemoryDump(const char* categoryKey, bool itemizeType);
    ~TraceMemoryDump() {}

    void DumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value);

    void DumpStringValue(const char* dumpName, const char* valueName, const char* value);

    void LogOutput(OHOS::Rosen::DfxString& log);

    void LogTotals(OHOS::Rosen::DfxString& log);

    float GetGpuMemorySizeInMB() const;

    float GetGLMemorySize() const;

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
private:
    std::shared_ptr<TraceMemoryDumpImpl> impl_;
};
}
}
}
#endif // TRACE_MEMORY_DUMP_H