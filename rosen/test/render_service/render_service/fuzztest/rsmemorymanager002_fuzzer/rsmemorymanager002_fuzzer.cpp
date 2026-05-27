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

#include "rsmemorymanager002_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "memory/rs_memory_manager.h"
#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_graphic.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {

// Global GPU context, initialized in LLVMFuzzerInitialize, managed by unique_ptr (RAII)
std::unique_ptr<OHOS::Rosen::Drawing::GPUContext> g_gpuContext;

namespace {
const uint8_t DO_SET_GPU_MEMORY_ASYNC_RECLAIMER_SWITCH = 0;
const uint8_t DO_INTERRUPT_RECLAIM_TASK = 1;
const uint8_t DO_SET_RECLAIM_INTERRUPT = 2;
const uint8_t DO_DUMP_EXIT_PID_MEM = 3;
const uint8_t DO_COUNT_PID_MEMORY = 4;
const uint8_t DO_COUNT_MEMORY = 5;
const uint8_t DO_DUMP_PID_MEMORY = 6;
const uint8_t TARGET_SIZE = 7;
constexpr uint8_t MAX_FUZZ_PID_SET_SIZE = 8;

OHOS::Rosen::Drawing::GPUContext* GetFuzzedGpuContext(FuzzedDataProvider& fdp)
{
    bool useNull = fdp.ConsumeBool();
    return useNull ? nullptr : g_gpuContext.get();
}

std::vector<pid_t> CreateFuzzedPidVector(FuzzedDataProvider& fdp, uint8_t maxCount)
{
    std::vector<pid_t> pids;
    uint8_t count = fdp.ConsumeIntegralInRange<uint8_t>(0, maxCount);
    for (uint8_t i = 0; i < count; i++) {
        pids.push_back(static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>()));
    }
    return pids;
}

void DoSetGpuMemoryAsyncReclaimerSwitch(FuzzedDataProvider& fdp)
{
    bool enabled = fdp.ConsumeBool();
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    std::function<void()> setThreadPriority = []() {};
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(gpuContext, enabled, setThreadPriority);
}

void DoInterruptReclaimTask(FuzzedDataProvider& fdp)
{
    std::string sceneId = fdp.ConsumeRandomLengthString(64);
    RSReclaimMemoryManager::Instance().InterruptReclaimTask(sceneId);
}

void DoSetReclaimInterrupt(FuzzedDataProvider& fdp)
{
    bool isInterrupt = fdp.ConsumeBool();
    RSReclaimMemoryManager::Instance().SetReclaimInterrupt(isInterrupt);
}

void DoDumpExitPidMem(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    std::string log;
    MemoryManager::DumpExitPidMem(log, pid);
}

void DoCountPidMemory(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::CountPidMemory(pid, gpuContext);
}

void DoCountMemory(FuzzedDataProvider& fdp)
{
    std::vector<pid_t> pids = CreateFuzzedPidVector(fdp, MAX_FUZZ_PID_SET_SIZE);
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    std::vector<MemoryGraphic> mems;
    MemoryManager::CountMemory(pids, gpuContext, mems);
}

void DoDumpPidMemory(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    DfxString log;
    MemoryManager::DumpPidMemory(log, pid, gpuContext);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_gpuContext = std::make_unique<OHOS::Rosen::Drawing::GPUContext>();
    if (!OHOS::Rosen::g_gpuContext) {
        return -1;
    }
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_GPU_MEMORY_ASYNC_RECLAIMER_SWITCH:
            OHOS::Rosen::DoSetGpuMemoryAsyncReclaimerSwitch(fdp);
            break;
        case OHOS::Rosen::DO_INTERRUPT_RECLAIM_TASK:
            OHOS::Rosen::DoInterruptReclaimTask(fdp);
            break;
        case OHOS::Rosen::DO_SET_RECLAIM_INTERRUPT:
            OHOS::Rosen::DoSetReclaimInterrupt(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_EXIT_PID_MEM:
            OHOS::Rosen::DoDumpExitPidMem(fdp);
            break;
        case OHOS::Rosen::DO_COUNT_PID_MEMORY:
            OHOS::Rosen::DoCountPidMemory(fdp);
            break;
        case OHOS::Rosen::DO_COUNT_MEMORY:
            OHOS::Rosen::DoCountMemory(fdp);
            break;
        case OHOS::Rosen::DO_DUMP_PID_MEMORY:
            OHOS::Rosen::DoDumpPidMemory(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
