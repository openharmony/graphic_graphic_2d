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
#ifndef RS_SKIA_MEMORY_TRACER_H
#define RS_SKIA_MEMORY_TRACER_H
#include <unordered_map>
#include <vector>

#include "include/core/SkString.h"
#include "include/core/SkTraceMemoryDump.h"
#include "memory/rs_dfx_string.h"

namespace OHOS::Rosen {
typedef std::pair<const char*, const char*> ResourcePair;

class SkiaMemoryTracer : public SkTraceMemoryDump {
public:
    SkiaMemoryTracer(const char* categoryKey, bool itemizeType);
    SkiaMemoryTracer(const std::vector<ResourcePair>& resourceMap, bool itemizeType);
    ~SkiaMemoryTracer() override {}

    void LogOutput(DfxString& log);
    void LogTotals(DfxString& log);

    float GetGpuMemorySizeInMB();
    void dumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value) override;

    void dumpStringValue(const char* dumpName, const char* valueName, const char* value) override
    {
        dumpNumericValue(dumpName, valueName, value, 0);
    }

    float GetGLMemorySize();

    void setMemoryBacking(const char*, const char*, const char*) override {}
    void setDiscardableMemoryBacking(const char*, const SkDiscardableMemory&) override {}

    LevelOfDetail getRequestedDetails() const override
    {
        return SkTraceMemoryDump::kLight_LevelOfDetail;
    }

    bool shouldDumpWrappedObjects() const override
    {
        return true;
    }
private:
    struct TraceValue {
        TraceValue(const char* units, uint64_t value) : units(units), value(value), count(1) {}
        TraceValue(const TraceValue& v) : units(v.units), value(v.value), count(v.count) {}
        TraceValue& operator=(const TraceValue& v)
        {
            units = v.units;
            value = v.value;
            count = v.count;
            return *this;
        }
        SkString units;
        float value;
        int count;
    };

    const char* MapName(const char* resourceName);
    void ProcessElement();
    TraceValue ConvertUnits(const TraceValue& value);
    float ConvertToMB(const TraceValue& value);

    const std::vector<ResourcePair> resourceMap_;
    const char* categoryKey_ = nullptr;
    const bool itemizeType_;

    TraceValue totalSize_;
    TraceValue purgeableSize_;
    std::string currentElement_;
    std::unordered_map<std::string, TraceValue> currentValues_;
    std::unordered_map<std::string, std::unordered_map<std::string, TraceValue>> results_;
};

} // namespace OHOS::Rosen
#endif
