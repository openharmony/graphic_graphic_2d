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

#include <vector>

#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#else
#include "image/gpu_context.h"
#endif

#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {

class MemoryManager {
public:
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
    static void DumpMemoryUsage(DfxString& log, const GrDirectContext* grContext, std::string& type);
    static void DumpPidMemory(DfxString& log, int pid, const GrDirectContext* grContext);
    // Count memory for hidumper
    static MemoryGraphic CountPidMemory(int pid, const GrDirectContext* grContext);
    static void CountMemory(std::vector<pid_t> pids, const GrDirectContext* grContext,
        std::vector<MemoryGraphic>& mems);
    static void ReleaseUnlockGpuResource(GrDirectContext* grContext, NodeId surfaceNodeId);
    static void ReleaseUnlockGpuResource(GrDirectContext* grContext, pid_t pid);
    static void ReleaseUnlockGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag);
    static void ReleaseAllGpuResource(GrDirectContext* grContext, pid_t pid);
    static void ReleaseAllGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag);
    static void ReleaseUnlockGpuResource(GrDirectContext* grContext, bool scratchResourcesOnly = true);
#else
    static void DumpMemoryUsage(DfxString& log, const GrContext* grContext, std::string& type);
    static void DumpPidMemory(DfxString& log, int pid, const GrContext* grContext);
    // Count memory for hidumper
    static MemoryGraphic CountPidMemory(int pid, const GrContext* grContext);
    static void CountMemory(std::vector<pid_t> pids, const GrContext* grContext, std::vector<MemoryGraphic>& mems);
    static void ReleaseUnlockGpuResource(GrContext* grContext, GrGpuResourceTag& tag);
    static void ReleaseUnlockGpuResource(GrContext* grContext, NodeId surfaceNodeId);
    static void ReleaseUnlockGpuResource(GrContext* grContext, pid_t pid);
    static void ReleaseAllGpuResource(GrContext* grContext, GrGpuResourceTag& tag);
    static void ReleaseAllGpuResource(GrContext* grContext, pid_t pid);
    static void ReleaseUnlockGpuResource(GrContext* grContext, bool scratchResourcesOnly = true);
#endif
#else
    static void DumpMemoryUsage(DfxString& log, const Drawing::GPUContext* gpuContext, std::string& type);
    static void DumpPidMemory(DfxString& log, int pid, const Drawing::GPUContext* gpuContext);
    // Count memory for hidumper
    static MemoryGraphic CountPidMemory(int pid, const Drawing::GPUContext* gpuContext);
    static void CountMemory(std::vector<pid_t> pids,
        const Drawing::GPUContext* gpuContext, std::vector<MemoryGraphic>& mems);
    // TODO Drawing static void ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, GrGpuResourceTag& tag);
    static void ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, NodeId surfaceNodeId);
    static void ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, pid_t pid);
    // TODO Drawing static void ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, GrGpuResourceTag& tag);
    static void ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, pid_t pid);
    static void ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, bool scratchResourcesOnly = true);
#endif

private:
    // rs memory = rs + skia cpu + skia gpu
    static void DumpRenderServiceMemory(DfxString& log);
    static void DumpDrawingCpuMemory(DfxString& log);
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
    static void DumpDrawingGpuMemory(DfxString& log, const GrDirectContext* grContext);
    static void DumpAllGpuInfo(DfxString& log, const GrDirectContext* grContext);
    static void DumpGpuCache(
        DfxString& log, const GrDirectContext* grContext, GrGpuResourceTag* tag, std::string& name);
#else
    static void DumpDrawingGpuMemory(DfxString& log, const GrContext* grContext);
    static void DumpGpuCache(DfxString& log, const GrContext* grContext, GrGpuResourceTag* tag, std::string& name);
    static void DumpAllGpuInfo(DfxString& log, const GrContext* grContext);
#endif
#else
    static void DumpDrawingGpuMemory(DfxString& log, const Drawing::GPUContext* grContext);
    //static void DumpGpuCache(DfxString& log, const Drawing::GPUContext* grContext, GrGpuResourceTag* tag, std::string& name);
    static void DumpAllGpuInfo(DfxString& log, const Drawing::GPUContext* grContext);
#endif
    //jemalloc info
    static void DumpMallocStat(std::string& log);
};
} // namespace OHOS::Rosen