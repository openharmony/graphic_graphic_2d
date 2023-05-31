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

#include "memory/rs_memory_manager.h"

#include <malloc.h>
#include <SkGraphics.h>
#include "rs_trace.h"

#include "rs_skia_memory_tracer.h"
#include "memory/rs_memory_graphic.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#include "src/gpu/GrDirectContextPriv.h"
#else
#include "include/gpu/GrContext.h"
#include "src/gpu/GrContextPriv.h"
#endif

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr const char* MEM_RS_TYPE = "renderservice";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_JEMALLOC_TYPE = "jemalloc";
}

#ifdef NEW_SKIA
void MemoryManager::DumpMemoryUsage(DfxString& log, const GrDirectContext* grContext, std::string& type)
#else
void MemoryManager::DumpMemoryUsage(DfxString& log, const GrContext* grContext, std::string& type)
#endif
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

#ifdef NEW_SKIA
void MemoryManager::ReleaseAllGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag)
#else
void MemoryManager::ReleaseAllGpuResource(GrContext* grContext, GrGpuResourceTag& tag)
#endif
{
#ifdef RS_ENABLE_GL
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
    }
    RS_TRACE_NAME_FMT("ReleaseAllGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext->releaseByTag(tag);
#endif
}

#ifdef NEW_SKIA
void MemoryManager::ReleaseAllGpuResource(GrDirectContext* grContext, pid_t pid)
#else
void MemoryManager::ReleaseAllGpuResource(GrContext* grContext, pid_t pid)
#endif
{
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(pid, 0, 0, 0);
    ReleaseAllGpuResource(grContext, tag);
#endif
}

#ifdef NEW_SKIA
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag)
#else
void MemoryManager::ReleaseUnlockGpuResource(GrContext* grContext, GrGpuResourceTag& tag)
#endif
{
#ifdef RS_ENABLE_GL
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext->purgeUnlockedResourcesByTag(false, tag);
#endif
}

#ifdef NEW_SKIA
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, NodeId surfaceNodeId)
#else
void MemoryManager::ReleaseUnlockGpuResource(GrContext* grContext, NodeId surfaceNodeId)
#endif
{
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(ExtractPid(surfaceNodeId), 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}

#ifdef NEW_SKIA
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, pid_t pid)
#else
void MemoryManager::ReleaseUnlockGpuResource(GrContext* grContext, pid_t pid)
#endif
{
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(pid, 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}

#ifdef NEW_SKIA
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, bool scratchResourcesOnly)
#else
void MemoryManager::ReleaseUnlockGpuResource(GrContext* grContext, bool scratchResourcesOnly)
#endif
{
#ifdef RS_ENABLE_GL
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource scratchResourcesOnly:%d", scratchResourcesOnly);
    grContext->purgeUnlockedResources(scratchResourcesOnly);
#endif
}

#ifdef NEW_SKIA
void MemoryManager::DumpPidMemory(DfxString& log, int pid, const GrDirectContext* grContext)
#else
void MemoryManager::DumpPidMemory(DfxString& log, int pid, const GrContext* grContext)
#endif
{
    //MemoryTrack::Instance().DumpMemoryStatistics(log, pid);
    MemoryGraphic mem = CountPidMemory(pid, grContext);
    log.AppendFormat("GPU Mem(MB):%f\n", mem.GetGpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("CPU Mem(MB):%f\n", mem.GetCpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("Total Mem(MB):%f\n", mem.GetTotalMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
}

#ifdef NEW_SKIA
MemoryGraphic MemoryManager::CountPidMemory(int pid, const GrDirectContext* grContext)
#else
MemoryGraphic MemoryManager::CountPidMemory(int pid, const GrContext* grContext)
#endif
{
    MemoryGraphic totalMemGraphic;

    // Count mem of RS
    totalMemGraphic.SetPid(pid);
    MemoryGraphic rsMemGraphic = MemoryTrack::Instance().CountRSMemory(pid);
    totalMemGraphic += rsMemGraphic;

#ifdef RS_ENABLE_GL
    // Count mem of Skia GPU
    if (grContext) {
        SkiaMemoryTracer gpuTracer("category", true);
        GrGpuResourceTag tag(pid, 0, 0, 0);
        grContext->dumpMemoryStatisticsByTag(&gpuTracer, tag);
        float gpuMem = gpuTracer.GetGLMemorySize();
        totalMemGraphic.IncreaseGpuMemory(gpuMem);
    }
#endif

    return totalMemGraphic;
}

#ifdef NEW_SKIA
void MemoryManager::CountMemory(std::vector<pid_t> pids, const GrDirectContext* grContext,
    std::vector<MemoryGraphic>& mems)
#else
void MemoryManager::CountMemory(std::vector<pid_t> pids, const GrContext* grContext, std::vector<MemoryGraphic>& mems)
#endif
{
    auto countMem = [&grContext, &mems] (pid_t pid) {
        mems.emplace_back(CountPidMemory(pid, grContext));
    };
    // Count mem of Skia GPU
    std::for_each(pids.begin(), pids.end(), countMem);
}

static std::tuple<uint64_t, std::string, RectI> FindGeoById(uint64_t nodeId)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    uint64_t windowId = nodeId;
    std::string windowName = "NONE";
    RectI nodeFrameRect;
    if (!node) {
        return { windowId, windowName, nodeFrameRect };
    }
    nodeFrameRect =
        std::static_pointer_cast<RSObjAbsGeometry>(node->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    // Obtain the window according to childId
    auto parent = node->GetParent().lock();
    while (parent) {
        if (parent->IsInstanceOf<RSSurfaceRenderNode>()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parent);
            windowName = surfaceNode->GetName();
            windowId = surfaceNode->GetId();
            break;
        }
        parent = parent->GetParent().lock();
    }
    return { windowId, windowName, nodeFrameRect };
}

void MemoryManager::DumpRenderServiceMemory(DfxString& log)
{
    log.AppendFormat("\n----------\nRenderService caches:\n");
    MemoryTrack::Instance().DumpMemoryStatistics(log, FindGeoById);
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

#ifdef NEW_SKIA
void MemoryManager::DumpGpuCache(
    DfxString& log, const GrDirectContext* grContext, GrGpuResourceTag* tag, std::string& name)
#else
void MemoryManager::DumpGpuCache(DfxString& log, const GrContext* grContext, GrGpuResourceTag* tag, std::string& name)
#endif
{
    if (!grContext) {
        log.AppendFormat("grContext is nullptr.\n");
        return;
    }
    /////////////////////////////GPU/////////////////////////
#ifdef RS_ENABLE_GL
    log.AppendFormat("\n---------------\nSkia GPU Caches:%s\n", name.c_str());
    SkiaMemoryTracer gpuTracer("category", true);
    if (tag) {
        grContext->dumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        grContext->dumpMemoryStatistics(&gpuTracer);
    }
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);
#endif
}

#ifdef NEW_SKIA
void MemoryManager::DumpAllGpuInfo(DfxString& log, const GrDirectContext* grContext)
#else
void MemoryManager::DumpAllGpuInfo(DfxString& log, const GrContext* grContext)
#endif
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#ifdef RS_ENABLE_GL
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&log, &grContext](const std::shared_ptr<RSSurfaceRenderNode> node) {
        GrGpuResourceTag tag(ExtractPid(node->GetId()), 0, node->GetId(), 0);
        std::string name = node->GetName() + " " + std::to_string(node->GetId());
        DumpGpuCache(log, grContext, &tag, name);
    });
#endif
}

#ifdef NEW_SKIA
void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const GrDirectContext* grContext)
#else
void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const GrContext* grContext)
#endif
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    std::string gpuInfo;
    /////////////////////////////GPU/////////////////////////
#ifdef RS_ENABLE_GL
    // total
    DumpGpuCache(log, grContext, nullptr, gpuInfo);
    // Get memory of window by tag
    DumpAllGpuInfo(log, grContext);
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        GrGpuResourceTag tag(0, 0, 0, tagtype);
        std::string tagType = RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype));
        DumpGpuCache(log, grContext, &tag, tagType);
    }
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
    malloc_stats_print(
        [](void* fp, const char* str) {
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
        },
        &log, nullptr);
}
} // namespace OHOS::Rosen