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

#include "skia_trace_memory_dump.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTraceMemoryDump::SkiaTraceMemoryDump(const char* categoryKey, bool itemizeType)
    : TraceMemoryDumpImpl(categoryKey, itemizeType)
{
    skiaMemoryTrace_ = std::make_shared<SkiaMemoryTracer>(categoryKey, itemizeType);
}

void SkiaTraceMemoryDump::DumpNumericValue(const char* dumpName,
    const char* valueName, const char* units, uint64_t value)
{
    if (skiaMemoryTrace_ == nullptr) {
        LOGE("SkiaTraceMemoryDump::DumpNumericValue, skiaMemoryTrace_ is nullptr");
        return;
    }

    skiaMemoryTrace_->dumpNumericValue(dumpName, valueName, units, value);
}

void SkiaTraceMemoryDump::DumpStringValue(const char* dumpName, const char* valueName, const char* value)
{
    if (skiaMemoryTrace_ == nullptr) {
        LOGE("SkiaTraceMemoryDump::DumpStringValue, skiaMemoryTrace_ is nullptr");
        return;
    }

    skiaMemoryTrace_->dumpStringValue(dumpName, valueName, value);
}

void SkiaTraceMemoryDump::LogOutput(OHOS::Rosen::DfxString& log)
{
    if (skiaMemoryTrace_ == nullptr) {
        LOGE("SkiaTraceMemoryDump::LogOutput, skiaMemoryTrace_ is nullptr");
        return;
    }

    skiaMemoryTrace_->LogOutput(log);
}

void SkiaTraceMemoryDump::LogTotals(OHOS::Rosen::DfxString& log)
{
    if (skiaMemoryTrace_ == nullptr) {
        LOGE("SkiaTraceMemoryDump::LogTotals, skiaMemoryTrace_ is nullptr");
        return;
    }

    skiaMemoryTrace_->LogTotals(log);
}

float SkiaTraceMemoryDump::GetGLMemorySize()
{
    if (skiaMemoryTrace_ == nullptr) {
        LOGE("SkiaTraceMemoryDump::GetGLMemorySize, skiaMemoryTrace_ is nullptr");
        return 0.0f;
    }

    return skiaMemoryTrace_->GetGLMemorySize();
}

std::shared_ptr<SkiaMemoryTracer> SkiaTraceMemoryDump::GetTraceMemoryDump() const
{
    return skiaMemoryTrace_;
}
}
}
}