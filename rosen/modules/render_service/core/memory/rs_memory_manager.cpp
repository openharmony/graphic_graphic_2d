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

#include <fstream>
#include <malloc.h>
#include <sstream>
#include <string>
#include "include/core/SkGraphics.h"
#include "rs_trace.h"
#include "third_party/cJSON/cJSON.h"

#include "memory/rs_dfx_string.h"
#include "skia_adapter/rs_skia_memory_tracer.h"
#include "skia_adapter/skia_graphics.h"
#include "memory/rs_memory_graphic.h"
#include "include/gpu/GrDirectContext.h"
#include "src/gpu/GrDirectContextPriv.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_singleton.h"
#include "memory/rs_tag_tracker.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#include "app_mgr_client.h"
#include "hisysevent.h"
#include "image/gpu_context.h"

#ifdef RS_ENABLE_VK
#include "pipeline/rs_vk_image_manager.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_sched_kill_reason.h"
#endif

namespace OHOS::Rosen {
namespace {
const std::string KERNEL_CONFIG_PATH = "/system/etc/hiview/kernel_leak_config.json";
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr uint32_t MEMORY_REPORT_INTERVAL = 24 * 60 * 60 * 1000; // Each process can report at most once a day.
constexpr uint32_t FRAME_NUMBER = 10; // Check memory every ten frames.
constexpr const char* MEM_RS_TYPE = "renderservice";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_JEMALLOC_TYPE = "jemalloc";
constexpr const char* MEM_SNAPSHOT = "snapshot";
constexpr int DUPM_STRING_BUF_SIZE = 4000;
}

std::mutex MemoryManager::mutex_;
std::unordered_map<pid_t, uint64_t> MemoryManager::pidInfo_;
uint32_t MemoryManager::frameCount_ = 0;
uint64_t MemoryManager::memoryWarning_ = UINT64_MAX;
uint64_t MemoryManager::gpuMemoryControl_ = UINT64_MAX;
uint64_t MemoryManager::totalMemoryReportTime_ = 0;

void MemoryManager::DumpMemoryUsage(DfxString& log, std::string& type)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        RSUniRenderThread::Instance().DumpMem(log);
    }
    if (type.empty() || type == MEM_JEMALLOC_TYPE) {
        std::string out;
        DumpMallocStat(out);
        log.AppendFormat("%s\n... detail dump at hilog\n", out.c_str());
    }
    if (type.empty() || type == MEM_SNAPSHOT) {
        DumpMemorySnapshot(log);
    }
}

void MemoryManager::ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseAllGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    gpuContext->ReleaseByTag(tag);
#endif
}

void MemoryManager::ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(pid, 0, 0, 0, "ReleaseAllGpuResource");
    ReleaseAllGpuResource(gpuContext, tag);
#endif
}

void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    gpuContext->PurgeUnlockedResourcesByTag(false, tag);
#endif
}

void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, std::set<pid_t> exitedPidSet)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByPid fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource exitedPidSet size: %d", exitedPidSet.size());
    gpuContext->PurgeUnlockedResourcesByPid(false, exitedPidSet);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPidSet);
    ErasePidInfo(exitedPidSet);
#endif
}

void MemoryManager::PurgeCacheBetweenFrames(Drawing::GPUContext* gpuContext, bool scratchResourceOnly,
    std::set<pid_t>& exitedPidSet, std::set<pid_t>& protectedPidSet)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("PurgeCacheBetweenFrames fail, gpuContext is nullptr");
        return;
    }
    gpuContext->PurgeCacheBetweenFrames(scratchResourceOnly, exitedPidSet, protectedPidSet);
#endif
}

void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, NodeId surfaceNodeId)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(ExtractPid(surfaceNodeId), 0, 0, 0, "ReleaseUnlockGpuResource");
    ReleaseUnlockGpuResource(grContext, tag);
#endif
}

void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(pid, 0, 0, 0, "ReleaseUnlockGpuResource");
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}

void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, bool scratchResourcesOnly)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource scratchResourcesOnly:%d", scratchResourcesOnly);
    gpuContext->PurgeUnlockedResources(scratchResourcesOnly);
#endif
}

void MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(Drawing::GPUContext* gpuContext)
{
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockAndSafeCacheGpuResource");
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
#endif
}

void MemoryManager::SetGpuCacheSuppressWindowSwitch(Drawing::GPUContext* gpuContext, bool enabled)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("SetGpuCacheSuppressWindowSwitch fail, gpuContext is nullptr");
        return;
    }
    gpuContext->SetGpuCacheSuppressWindowSwitch(enabled);
#endif
}

void MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(Drawing::GPUContext* gpuContext, bool enabled)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr");
        return;
    }
    gpuContext->SetGpuMemoryAsyncReclaimerSwitch(enabled);
#endif
}

void MemoryManager::FlushGpuMemoryInWaitQueue(Drawing::GPUContext* gpuContext)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("FlushGpuMemoryInWaitQueue fail, gpuContext is nullptr");
        return;
    }
    gpuContext->FlushGpuMemoryInWaitQueue();
#endif
}

void MemoryManager::SuppressGpuCacheBelowCertainRatio(
    Drawing::GPUContext* gpuContext, const std::function<bool(void)>& nextFrameHasArrived)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("SuppressGpuCacheBelowCertainRatio fail, gpuContext is nullptr");
        return;
    }
    gpuContext->SuppressGpuCacheBelowCertainRatio(nextFrameHasArrived);
#endif
}

float MemoryManager::GetAppGpuMemoryInMB(Drawing::GPUContext* gpuContext)
{
    if (!gpuContext) {
        return 0.f;
    }
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TraceMemoryDump trace("category", true);
    gpuContext->DumpMemoryStatistics(&trace);
    auto total = trace.GetGpuMemorySizeInMB();
    float rsMemSize = 0.f;
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        Drawing::GPUResourceTag resourceTag(0, 0, 0, tagtype,
            RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype)));
        Drawing::TraceMemoryDump gpuTrace("category", true);
        gpuContext->DumpMemoryStatisticsByTag(&gpuTrace, resourceTag);
        rsMemSize += gpuTrace.GetGpuMemorySizeInMB();
    }
    return total - rsMemSize;
#else
    return 0.f;
#endif
}

void MemoryManager::DumpPidMemory(DfxString& log, int pid, const Drawing::GPUContext* gpuContext)
{
    MemoryGraphic mem = CountPidMemory(pid, gpuContext);
    log.AppendFormat("GPU Mem(MB):%f\n", mem.GetGpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("CPU Mem(KB):%f\n", mem.GetCpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("Total Mem(MB):%f\n", mem.GetTotalMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
}

MemoryGraphic MemoryManager::CountPidMemory(int pid, const Drawing::GPUContext* gpuContext)
{
    MemoryGraphic totalMemGraphic;

    // Count mem of RS
    totalMemGraphic.SetPid(pid);

#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Count mem of Skia GPU
    if (gpuContext) {
        Drawing::TraceMemoryDump gpuTracer("category", true);
        Drawing::GPUResourceTag tag(pid, 0, 0, 0, "ReleaseUnlockGpuResource");
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, tag);
        float gpuMem = gpuTracer.GetGLMemorySize();
        totalMemGraphic.IncreaseGpuMemory(gpuMem);
    }
#endif

    return totalMemGraphic;
}

void MemoryManager::CountMemory(
    std::vector<pid_t> pids, const Drawing::GPUContext* gpuContext, std::vector<MemoryGraphic>& mems)
{
    auto countMem = [&gpuContext, &mems] (pid_t pid) {
        mems.emplace_back(CountPidMemory(pid, gpuContext));
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
        (node->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    // Obtain the window according to childId
    auto parent = node->GetParent().lock();
    bool windowsNameFlag = false;
    while (parent) {
        if (parent->IsInstanceOf<RSSurfaceRenderNode>()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parent);
            windowName = surfaceNode->GetName();
            windowId = surfaceNode->GetId();
            windowsNameFlag = true;
            break;
        }
        parent = parent->GetParent().lock();
    }
    if (!windowsNameFlag) {
        windowName = "EXISTS-BUT-NO-SURFACE";
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
    std::string cpuInfo = "Skia CPU caches : pid:" + std::to_string(getpid()) +
        ", threadId:" + std::to_string(gettid());
#ifdef ROSEN_OHOS
    char threadName[16]; // thread name is restricted to 16 bytes
    auto result = pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
    if (result == 0) {
        cpuInfo = cpuInfo + ", threadName: " + threadName;
    }
#endif
    log.AppendFormat("\n----------\n%s\n", cpuInfo.c_str());
    log.AppendFormat("Font Cache (CPU):\n");
    log.AppendFormat("  Size: %.2f kB \n", Drawing::SkiaGraphics::GetFontCacheUsed() / MEMUNIT_RATE);
    log.AppendFormat("  Glyph Count: %d \n", Drawing::SkiaGraphics::GetFontCacheCountUsed());

    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
        { "skia/sk_resource_cache/rects-blur_", "Masks" },
        { "skia/sk_resource_cache/tessellated", "Shadows" },
        { "skia/sk_resource_cache/yuv-planes_", "YUVPlanes" },
        { "skia/sk_resource_cache/budget_glyph_count", "Bitmaps" },
    };
    SkiaMemoryTracer cpuTracer(cpuResourceMap, true);
    Drawing::SkiaGraphics::DumpMemoryStatistics(&cpuTracer);
    log.AppendFormat("CPU Cachesxx:\n");
    cpuTracer.LogOutput(log);
    log.AppendFormat("Total CPU memory usage:\n");
    cpuTracer.LogTotals(log);

    // cache limit
    size_t cacheLimit = Drawing::SkiaGraphics::GetResourceCacheTotalByteLimit();
    size_t fontCacheLimit = Drawing::SkiaGraphics::GetFontCacheLimit();
    log.AppendFormat("\ncpu cache limit = %zu ( fontcache = %zu ):\n", cacheLimit, fontCacheLimit);
}

void MemoryManager::DumpGpuCache(
    DfxString& log, const Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag* tag, std::string& name)
{
    if (!gpuContext) {
        log.AppendFormat("gpuContext is nullptr.\n");
        return;
    }
    /* GPU */
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    log.AppendFormat("\n---------------\nSkia GPU Caches:%s\n", name.c_str());
    Drawing::TraceMemoryDump gpuTracer("category", true);
    if (tag) {
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        gpuContext->DumpMemoryStatistics(&gpuTracer);
#ifdef RS_ENABLE_VK
        RsVulkanMemStat& memStat = RsVulkanContext::GetSingleton().GetRsVkMemStat();
        memStat.DumpMemoryStatistics(&gpuTracer);
#endif
    }
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);
#endif
}

void MemoryManager::DumpAllGpuInfo(DfxString& log, const Drawing::GPUContext* gpuContext,
    std::vector<std::pair<NodeId, std::string>>& nodeTags)
{
    if (!gpuContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    for (auto& nodeTag : nodeTags) {
        Drawing::GPUResourceTag tag(ExtractPid(nodeTag.first), 0, nodeTag.first, 0, nodeTag.second);
        DumpGpuCache(log, gpuContext, &tag, nodeTag.second);
    }
#endif
}

void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const Drawing::GPUContext* gpuContext,
    std::vector<std::pair<NodeId, std::string>>& nodeTags)
{
    if (!gpuContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    /* GPU */
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    std::string gpuInfo = "pid:" + std::to_string(getpid()) + ", threadId:" + std::to_string(gettid());
#ifdef ROSEN_OHOS
    char threadName[16]; // thread name is restricted to 16 bytes
    auto result = pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
    if (result == 0) {
        gpuInfo = gpuInfo + ", threadName: " + threadName;
    }
#endif
    // total
    DumpGpuCache(log, gpuContext, nullptr, gpuInfo);
    // Get memory of window by tag
    DumpAllGpuInfo(log, gpuContext, nodeTags);
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        std::string tagTypeName = RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype));
        Drawing::GPUResourceTag tag(0, 0, 0, tagtype, tagTypeName);
        DumpGpuCache(log, gpuContext, &tag, tagTypeName);
    }
    // cache limit
    size_t cacheLimit = 0;
    size_t cacheUsed = 0;
    gpuContext->GetResourceCacheLimits(nullptr, &cacheLimit);
    gpuContext->GetResourceCacheUsage(nullptr, &cacheUsed);
    log.AppendFormat("\ngpu limit = %zu ( used = %zu ):\n", cacheLimit, cacheUsed);

    /* ShaderCache */
    log.AppendFormat("\n---------------\nShader Caches:\n");
#ifdef NEW_RENDER_CONTEXT
    log.AppendFormat(MemoryHandler::QuerryShader().c_str());
#else
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());
#endif
    // gpu stat
    DumpGpuStats(log, gpuContext);
#endif
}

void MemoryManager::DumpGpuStats(DfxString& log, const Drawing::GPUContext* gpuContext)
{
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    std::string stat;
    gpuContext->DumpGpuStats(stat);

    int statIndex = 0;
    int statLength = stat.length();
    while (statIndex < statLength) {
        std::string statSubStr;
        if (statLength - statIndex > DUPM_STRING_BUF_SIZE) {
            statSubStr = stat.substr(statIndex, DUPM_STRING_BUF_SIZE);
            statIndex += DUPM_STRING_BUF_SIZE;
        } else {
            statSubStr = stat.substr(statIndex, statLength - statIndex);
            statIndex = statLength;
        }
        log.AppendFormat("%s", statSubStr.c_str());
    }
    log.AppendFormat("\ndumpGpuStats end\n---------------\n");
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
                RS_LOGW("[mallocstat]:%{public}s", str);
            }
        },
        &log, nullptr);
}

void MemoryManager::DumpMemorySnapshot(DfxString& log)
{
    size_t totalMemory = MemorySnapshot::Instance().GetTotalMemory();
    log.AppendFormat("\n---------------\nmemorySnapshots, totalMemory %zuKB\n", totalMemory / MEMUNIT_RATE);
    std::unordered_map<pid_t, MemorySnapshotInfo> memorySnapshotInfo;
    MemorySnapshot::Instance().GetMemorySnapshot(memorySnapshotInfo);
    for (auto& [pid, snapshotInfo] : memorySnapshotInfo) {
        std::string infoStr = "pid: " + std::to_string(pid) + " " + snapshotInfo.bundleName +
            ", cpu: " + std::to_string(snapshotInfo.cpuMemory / MEMUNIT_RATE) +
            "KB, gpu: " + std::to_string(snapshotInfo.gpuMemory / MEMUNIT_RATE) + "KB";
        log.AppendFormat("%s\n", infoStr.c_str());
    }
}

uint64_t ParseMemoryLimit(const cJSON* json, const char* name)
{
    cJSON* jsonItem = cJSON_GetObjectItem(json, name);
    if (jsonItem != nullptr && cJSON_IsNumber(jsonItem)) {
        return static_cast<uint64_t>(jsonItem->valueint) * MEMUNIT_RATE * MEMUNIT_RATE;
    }
    return UINT64_MAX;
}

void MemoryManager::InitMemoryLimit()
{
    std::ifstream configFile;
    configFile.open(KERNEL_CONFIG_PATH);
    std::stringstream filterParamsStream;
    filterParamsStream << configFile.rdbuf();
    configFile.close();
    std::string paramsString = filterParamsStream.str();

    cJSON* root = cJSON_Parse(paramsString.c_str());
    if (root == nullptr) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not parse config to json");
        return;
    }
    cJSON* kernelLeak = cJSON_GetObjectItem(root, "KernelLeak");
    if (kernelLeak == nullptr) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not find kernelLeak");
        cJSON_Delete(root);
        return;
    }
    cJSON* version = cJSON_GetObjectItem(kernelLeak, RSSystemProperties::GetVersionType().c_str());
    if (version == nullptr) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not find version");
        cJSON_Delete(root);
        return;
    }
    cJSON* rsWatchPoint = cJSON_GetObjectItem(version, "rs_watchpoint");
    if (rsWatchPoint == nullptr) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not find rsWatchPoint");
        cJSON_Delete(root);
        return;
    }
    // warning threshold for total memory of a single process
    memoryWarning_ = ParseMemoryLimit(rsWatchPoint, "process_warning_threshold");
    // error threshold for cpu memory of a single process
    uint64_t cpuMemoryControl = ParseMemoryLimit(rsWatchPoint, "process_cpu_control_threshold");
    // error threshold for gpu memory of a single process
    gpuMemoryControl_ = ParseMemoryLimit(rsWatchPoint, "process_gpu_control_threshold");
    // threshold for the total memory of all processes in renderservice
    uint64_t totalMemoryWarning = ParseMemoryLimit(rsWatchPoint, "total_threshold");
    cJSON_Delete(root);

    MemorySnapshot::Instance().InitMemoryLimit(MemoryOverflow, memoryWarning_, cpuMemoryControl, totalMemoryWarning);
}

void MemoryManager::SetGpuMemoryLimit(Drawing::GPUContext* gpuContext)
{
    if (gpuContext == nullptr || gpuMemoryControl_ == UINT64_MAX) {
        RS_LOGW("MemoryManager::SetGpuMemoryLimit gpuContext is nullptr or gpuMemoryControl_ is uninitialized");
        return;
    }
    gpuContext->InitGpuMemoryLimit(MemoryOverflow, gpuMemoryControl_);
}

void MemoryManager::MemoryOverCheck(Drawing::GPUContext* gpuContext)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    frameCount_++;
    if (!gpuContext || frameCount_ < FRAME_NUMBER) {
        return;
    }
    frameCount_ = 0;

    FillMemorySnapshot();
    std::unordered_map<pid_t, size_t> gpuMemory;
    gpuContext->GetUpdatedMemoryMap(gpuMemory);

    auto task = [gpuMemory = std::move(gpuMemory)]() {
        std::unordered_map<pid_t, MemorySnapshotInfo> infoMap;
        bool isTotalOver = false;
        MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuMemory, infoMap, isTotalOver);
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        // total memory overflow of all processes in renderservice
        if (isTotalOver && currentTime > totalMemoryReportTime_) {
            TotalMemoryOverReport(infoMap);
            totalMemoryReportTime_ = currentTime + MEMORY_REPORT_INTERVAL;
        }

        bool needReport = false;
        for (const auto& [pid, memoryInfo] : infoMap) {
            if (memoryInfo.TotalMemory() <= memoryWarning_) {
                continue;
            }
            needReport = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = pidInfo_.find(pid);
                if (it == pidInfo_.end()) {
                    pidInfo_.emplace(pid, currentTime + MEMORY_REPORT_INTERVAL);
                    needReport = true;
                } else if (currentTime > it->second) {
                    it->second = currentTime + MEMORY_REPORT_INTERVAL;
                    needReport = true;
                }
            }
            if (needReport) {
                MemoryOverReport(pid, memoryInfo, "RENDER_MEMORY_OVER_WARNING");
            }
        }
    };
    RSBackgroundThread::Instance().PostTask(task);
#endif
}
 
void MemoryManager::FillMemorySnapshot()
{
    std::vector<pid_t> pidList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(pidList);
    if (pidList.size() == 0) {
        return;
    }

    std::unordered_map<pid_t, MemorySnapshotInfo> infoMap;
    for (auto& pid : pidList) {
        MemorySnapshotInfo& mInfo = infoMap[pid];
        int32_t uid;
        auto& appMgrClient = RSSingleton<AppExecFwk::AppMgrClient>::GetInstance();
        appMgrClient.GetBundleNameByPid(pid, mInfo.bundleName, uid);
    }
    MemorySnapshot::Instance().FillMemorySnapshot(infoMap);
}

static void KillProcessByPid(const pid_t pid, const std::string& processName, const std::string& reason)
{
#ifdef RES_SCHED_ENABLE
    std::unordered_map<std::string, std::string> killInfo;
    killInfo["pid"] = std::to_string(pid);
    killInfo["processName"] = processName;
    killInfo["killReason"] = reason;
    if (pid > 0) {
        int32_t eventWriteStatus = -1;
        int32_t killStatus = ResourceSchedule::ResSchedClient::GetInstance().KillProcess(killInfo);
        if (killStatus == 0) {
            eventWriteStatus = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::FRAMEWORK, "PROCESS_KILL",
                HiviewDFX::HiSysEvent::EventType::FAULT, "PID", pid, "PROCESS_NAME", processName,
                "MSG", reason, "FOREGROUND", false);
        }
        // To prevent the print from being filtered, use RS_LOGE.
        RS_LOGE("KillProcessByPid, pid: %{public}d, process name: %{public}s, "
            "killStatus: %{public}d, eventWriteStatus: %{public}d, reason: %{public}s",
            static_cast<int32_t>(pid), processName.c_str(), killStatus, eventWriteStatus, reason.c_str());
    }
#endif
}

void MemoryManager::MemoryOverflow(pid_t pid, size_t overflowMemory, bool isGpu)
{
    MemorySnapshotInfo info;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    if (isGpu) {
        info.gpuMemory = overflowMemory;
    }
    RSMainThread::Instance()->PostTask([]() {
        RS_TRACE_NAME_FMT("RSMem Dump Task");
        std::unordered_set<std::u16string> argSets;
        std::string dumpString = "";
        std::string type = MEM_SNAPSHOT;
        RSMainThread::Instance()->DumpMem(argSets, dumpString, type, 0);
        RS_LOGI("=======================RSMem Dump Info=======================");
        std::istringstream stream(dumpString);
        std::string line;
        while (std::getline(stream, line)) {
            RS_LOGI("%{public}s", line.c_str());
        }
        RS_LOGI("=============================================================");
    });
    std::string reason = "RENDER_MEMORY_OVER_ERROR: cpu[" + std::to_string(info.cpuMemory)
        + "], gpu[" + std::to_string(info.gpuMemory) + "], total["
        + std::to_string(info.TotalMemory()) + "]";

    if (info.bundleName.empty()) {
        int32_t uid;
        auto& appMgrClient = RSSingleton<AppExecFwk::AppMgrClient>::GetInstance();
        appMgrClient.GetBundleNameByPid(pid, info.bundleName, uid);
    }
    MemoryOverReport(pid, info, "RENDER_MEMORY_OVER_ERROR");
    KillProcessByPid(pid, info.bundleName, reason);
    RS_LOGE("RSMemoryOverflow pid[%{public}d] cpu[%{public}zu] gpu[%{public}zu]", pid, info.cpuMemory, info.gpuMemory);
}

void MemoryManager::MemoryOverReport(const pid_t pid, const MemorySnapshotInfo& info, const std::string& reportName)
{
    int ret = HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid,
        "BUNDLE_NAME", info.bundleName,
        "CPU_MEMORY", info.cpuMemory,
        "GPU_MEMORY", info.gpuMemory,
        "TOTAL_MEMORY", info.TotalMemory());
    RS_LOGW("RSMemoryOverReport pid[%{public}d] bundleName[%{public}s] cpu[%{public}zu] "
        "gpu[%{public}zu] total[%{public}zu] ret[%{public}d]",
        pid, info.bundleName.c_str(), info.cpuMemory, info.gpuMemory, info.TotalMemory(), ret);
}

void MemoryManager::TotalMemoryOverReport(const std::unordered_map<pid_t, MemorySnapshotInfo>& infoMap)
{
    std::ostringstream oss;
    for (const auto& info : infoMap) {
        oss << info.first << '_' << info.second.TotalMemory() << ' ';
    }
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RENDER_MEMORY_OVER_TOTAL_ERROR",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "MEMORY_MSG", oss.str());
}

void MemoryManager::ErasePidInfo(const std::set<pid_t>& exitedPidSet)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto pid : exitedPidSet) {
        pidInfo_.erase(pid);
    }
}

void MemoryManager::VmaDefragment(Drawing::GPUContext* gpuContext)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("VmaDefragment fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("VmaDefragment");
    gpuContext->VmaDefragment();
#endif
}
} // namespace OHOS::Rosen