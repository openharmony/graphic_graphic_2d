/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rsmemorymanager003_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "memory/rs_memory_manager.h"
#include "memory/rs_dfx_string.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {

std::unique_ptr<OHOS::Rosen::Drawing::GPUContext> g_gpuContext;

namespace {
const uint8_t DO_DUMP_MEM = 0;
const uint8_t DO_DUMP_GPU_MEM = 1;
const uint8_t DO_DUMP_MEMORY_USAGE = 2;
const uint8_t DO_DUMP_DRAWING_GPU_MEMORY = 3;
const uint8_t DO_DUMP_ALL_GPU_INFO_NEW = 4;
const uint8_t DO_DUMP_NODES_INFO_FOR_REPORT = 5;
const uint8_t TARGET_SIZE = 6;
constexpr uint8_t MAX_FUZZ_NODE_TAG_COUNT = 8;
constexpr uint8_t MAX_FUZZ_U16_ARG_COUNT = 4;

OHOS::Rosen::Drawing::GPUContext* GetFuzzedGpuContext(FuzzedDataProvider& fdp)
{
    bool useNull = fdp.ConsumeBool();
    return useNull ? nullptr : g_gpuContext.get();
}

std::vector<std::pair<NodeId, std::string>> CreateFuzzedNodeTags(FuzzedDataProvider& fdp, uint8_t maxCount)
{
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    uint8_t count = fdp.ConsumeIntegralInRange<uint8_t>(0, maxCount);
    for (uint8_t i = 0; i < count; i++) {
        NodeId nodeId = fdp.ConsumeIntegral<uint64_t>();
        std::string tagName = fdp.ConsumeRandomLengthString(32);
        nodeTags.push_back({nodeId, tagName});
    }
    return nodeTags;
}

std::unordered_set<std::u16string> CreateFuzzedU16Args(FuzzedDataProvider& fdp, uint8_t maxCount)
{
    std::unordered_set<std::u16string> argSets;
    uint8_t count = fdp.ConsumeIntegralInRange<uint8_t>(0, maxCount);
    for (uint8_t i = 0; i < count; i++) {
        std::string arg = fdp.ConsumeRandomLengthString(16);
        std::u16string u16Arg(arg.begin(), arg.end());
        argSets.insert(u16Arg);
    }
    return argSets;
}

void DoDumpMem(FuzzedDataProvider& fdp)
{
    std::unordered_set<std::u16string> argSets = CreateFuzzedU16Args(fdp, MAX_FUZZ_U16_ARG_COUNT);
    std::string result;
    std::string type = fdp.ConsumeRandomLengthString(64);
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    bool isLite = fdp.ConsumeBool();
    MemoryManager::DumpMem(argSets, result, type, pid, isLite);
}

void DoDumpGpuMem(FuzzedDataProvider& fdp)
{
    std::unordered_set<std::u16string> argSets = CreateFuzzedU16Args(fdp, MAX_FUZZ_U16_ARG_COUNT);
    std::string dumpString;
    std::string type = fdp.ConsumeRandomLengthString(64);
    MemoryManager::DumpGpuMem(argSets, dumpString, type);
}

void DoDumpMemoryUsage(FuzzedDataProvider& fdp)
{
    DfxString log;
    std::string type = fdp.ConsumeRandomLengthString(64);
    bool isLite = fdp.ConsumeBool();
    MemoryManager::DumpMemoryUsage(log, type, isLite);
}

void DoDumpDrawingGpuMemory(FuzzedDataProvider& fdp)
{
    DfxString log;
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    std::vector<std::pair<NodeId, std::string>> nodeTags = CreateFuzzedNodeTags(fdp, MAX_FUZZ_NODE_TAG_COUNT);
    bool isLite = fdp.ConsumeBool();
    MemoryManager::DumpDrawingGpuMemory(log, gpuContext, nodeTags, isLite);
}

void DoDumpAllGpuInfoNew(FuzzedDataProvider& fdp)
{
    DfxString log;
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    std::vector<std::pair<NodeId, std::string>> nodeTags = CreateFuzzedNodeTags(fdp, MAX_FUZZ_NODE_TAG_COUNT);
    MemoryManager::DumpAllGpuInfoNew(log, gpuContext, nodeTags);
}

void DoDumpNodesInfoForReport(FuzzedDataProvider& fdp)
{
    std::string log;
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    MemoryManager::DumpNodesInfoForReport(log, pid);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_gpuContext = std::make_unique<OHOS::Rosen::Drawing::GPUContext>();
    if (!OHOS::Rosen::g_gpuContext) {
        return -1;
    }
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_DUMP_MEM:
            OHOS::Rosen::DoDumpMem(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_GPU_MEM:
            OHOS::Rosen::DoDumpGpuMem(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_MEMORY_USAGE:
            OHOS::Rosen::DoDumpMemoryUsage(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_DRAWING_GPU_MEMORY:
            OHOS::Rosen::DoDumpDrawingGpuMemory(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_ALL_GPU_INFO_NEW:
            OHOS::Rosen::DoDumpAllGpuInfoNew(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_NODES_INFO_FOR_REPORT:
            OHOS::Rosen::DoDumpNodesInfoForReport(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
