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

#include "image/trace_memory_dump.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
TraceMemoryDump::TraceMemoryDump(const char* categoryKey, bool itemizeType)
    : impl_(ImplFactory::CreateTraceMemoryDumpImpl(categoryKey, itemizeType)) {}

void TraceMemoryDump::DumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value)
{
    impl_->DumpNumericValue(dumpName, valueName, units, value);
}

void TraceMemoryDump::DumpStringValue(const char* dumpName, const char* valueName, const char* value)
{
    impl_->DumpStringValue(dumpName, valueName, value);
}

void TraceMemoryDump::LogOutput(OHOS::Rosen::DfxString& log)
{
    impl_->LogOutput(log);
}

void TraceMemoryDump::LogTotals(OHOS::Rosen::DfxString& log)
{
    impl_->LogTotals(log);
}

float TraceMemoryDump::GetGpuMemorySizeInMB() const
{
    return impl_->GetGpuMemorySizeInMB();
}

float TraceMemoryDump::GetGLMemorySize() const
{
    return impl_->GetGLMemorySize();
}
}
}
}