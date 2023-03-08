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

#include "MemoryManager.h"

#include <malloc.h>
#include <SkGraphics.h>

#include "SkiaMemoryTracer.h"
#include "include/gpu/GrContext.h"
#include "src/gpu/GrContextPriv.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr const char* MEM_RS_TYPE = "rs";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_JEMALLOC_TYPE = "jemalloc";
}


void MemoryManager::DumpMemoryUsage(DfxString& log, const GrContext* grContext, std::string& type)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        DumpDrawingGpuMemory(log, grContext);
    }
    if (type.empty() || type == MEM_JEMALLOC_TYPE) {
        std::string out;
        DumpMallocStat(out);
        log.AppendFormat("%s\n... detail dump at hilog\n", out.c_str());
    }
}

void MemoryManager::DumpPidMemory(DfxString& log, int pid)
{
    MemoryTrack::Instance().DumpMemoryStatistics(log, pid);
}

MemoryGraphic MemoryManager::DumpPidMemory(int pid)
{
    return MemoryTrack::Instance().DumpMemoryStatistics(pid);
}

void MemoryManager::DumpRenderServiceMemory(DfxString& log)
{
    log.AppendFormat("\n----------\nRenderService caches:\n");
    MemoryTrack::Instance().DumpMemoryStatistics(log);
}

void MemoryManager::DumpDrawingCpuMemory(DfxString& log)
{
    // CPU
    log.AppendFormat("\n----------\nSkia CPU caches:\n");
    log.AppendFormat("Font Cache (CPU):\n");
    log.AppendFormat("  Size: %.2f kB \n", SkGraphics::GetFontCacheUsed() / MEMUNIT_RATE);
    log.AppendFormat("  Glyph Count: %d \n", SkGraphics::GetFontCacheCountUsed());

    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
        { "skia/sk_resource_cache/rects-blur_", "Masks" },
        { "skia/sk_resource_cache/tessellated", "Shadows" },
        { "skia/sk_resource_cache/yuv-planes_", "YUVPlanes" },
        { "skia/sk_resource_cache/budget_glyph_count", "Bitmaps" },
    };
    SkiaMemoryTracer cpuTracer(cpuResourceMap, true);
    SkGraphics::DumpMemoryStatistics(&cpuTracer);
    log.AppendFormat("CPU Cachesxx:\n");
    cpuTracer.LogOutput(log);
    log.AppendFormat("Total CPU memory usage:\n");
    cpuTracer.LogTotals(log);

    // cache limit
    size_t cacheLimit = SkGraphics::GetResourceCacheTotalByteLimit();
    size_t fontCacheLimit = SkGraphics::GetFontCacheLimit();
    log.AppendFormat("\ncpu cache limit = %zu ( fontcache = %zu ):\n", cacheLimit, fontCacheLimit);
}

void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const GrContext* grContext)
{
    if (!grContext) {
        log.AppendFormat("No valid cache instance.\n");
        return;
    }
    /////////////////////////////GPU/////////////////////////
#ifdef RS_ENABLE_GL
    log.AppendFormat("\n---------------\nSkia GPU Caches:\n");
    SkiaMemoryTracer gpuTracer("category", true);
    grContext->dumpMemoryStatistics(&gpuTracer);
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);

    // cache limit
    size_t cacheLimit = 0;
    size_t cacheUsed = 0;
    grContext->getResourceCacheLimits(nullptr, &cacheLimit);
    grContext->getResourceCacheUsage(nullptr, &cacheUsed);
    log.AppendFormat("\ngpu limit = %zu ( used = %zu ):\n", cacheLimit, cacheUsed);

    //////////////////////////ShaderCache///////////////////
    log.AppendFormat("\n---------------\nShader Caches:\n");
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());

    // gpu stat
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    SkString stat;
    grContext->priv().dumpGpuStats(&stat);
    log.AppendFormat("%s\n", stat.c_str());
#endif
}

void MemoryManager::DumpMallocStat(std::string& log)
{
    malloc_stats_print([](void *fp, const char* str) {
        if (!fp) {
            RS_LOGE("DumpMallocStat fp is nullptr");
            return;
        }
        std::string* sp = static_cast<std::string*>(fp);
        if (str) {
            // cause log only support 2096 len. we need to only output critical log
            // and only put total log in RSLOG
            // get allocated string
            if (strncmp(str, "Allocated", strlen("Allocated")) == 0) {
                sp->append(str);
            }
            RS_LOGW("[mallocstat]:%s", str);
        }
    }, &log, nullptr);
}
} // namespace OHOS::Rosen