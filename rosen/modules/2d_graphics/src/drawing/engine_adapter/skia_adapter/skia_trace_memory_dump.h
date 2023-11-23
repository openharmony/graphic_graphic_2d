/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_TRACE_MEMORY_DUMP_H
#define SKIA_TRACE_MEMORY_DUMP_H

#include "impl_interface/trace_memory_dump_impl.h"
#include "image/trace_memory_dump.h"
#include "include/core/SkTraceMemoryDump.h"
#include "rs_skia_memory_tracer.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTraceMemoryDump : public TraceMemoryDumpImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaTraceMemoryDump(const char* categoryKey, bool itemizeType);
    virtual ~SkiaTraceMemoryDump() = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void DumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value) override;

    void DumpStringValue(const char* dumpName, const char* valueName, const char* value) override;

    void LogOutput(OHOS::Rosen::DfxString& log) override;

    void LogTotals(OHOS::Rosen::DfxString& log) override;

    float GetGpuMemorySizeInMB() override;

    float GetGLMemorySize() override;

    std::shared_ptr<SkiaMemoryTracer> GetTraceMemoryDump() const;

protected:
    std::shared_ptr<SkiaMemoryTracer> skiaMemoryTrace_;
};
}
}
}

#endif // SKIA_TRACE_MEMORY_DUMP_H
