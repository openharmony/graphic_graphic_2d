/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <ctime>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <malloc.h>
#include <sstream>
#include <string>
#include <sys/prctl.h>
#include "include/core/SkGraphics.h"
#include "rs_trace.h"
#include "cJSON.h"

#include "memory/rs_dfx_string.h"
#include "skia_adapter/rs_skia_memory_tracer.h"
#include "skia_adapter/skia_graphics.h"
#include "memory/rs_memory_graphic.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#include "src/gpu/ganesh/GrDirectContextPriv.h"
#else
#include "include/gpu/GrDirectContext.h"
#include "src/gpu/GrDirectContextPriv.h"
#endif
#include "include/gpu/vk/GrVulkanTrackerInterface.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_singleton.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature_cfg/feature_param/extend_feature/mem_param.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "memory/rs_tag_tracker.h"
#include "render/rs_typeface_cache.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#include "app_mgr_client.h"
#include "hisysevent.h"
#include "image/gpu_context.h"
#include "platform/common/rs_hisysevent.h"

#ifdef RS_ENABLE_UNI_RENDER
#include "ability_manager_client.h"
#endif

#ifdef RS_ENABLE_VK
#include "feature/gpuComposition/rs_vk_image_manager.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
static inline const char* GetThreadName()
{
    static constexpr int nameLen = 16;
    static thread_local char threadName[nameLen + 1] = "";
    if (threadName[0] == 0) {
        prctl(PR_GET_NAME, threadName);
        threadName[nameLen] = 0;
    }
    return threadName;
}

namespace OHOS::Rosen {
namespace {
const std::string KERNEL_CONFIG_PATH = "/system/etc/hiview/kernel_leak_config.json";
const std::string GPUMEM_INFO_PATH = "/proc/gpumem_process_info";
const std::string GPUMEM_NODEINFO_PATH = "/proc/gpu_memory";
const std::string EVENT_ENTER_RECENTS = "GESTURE_TO_RECENTS";
const std::string GPU_RS_LEAK = "ResourceLeak(GpuRsLeak)";
const std::string SCB_BUNDLE_NAME = "com.ohos.sceneboard";
const std::string GRAPHIC_MEM_DIR = "/data/service/el0/render_service/";
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr uint32_t MEMORY_REPORT_INTERVAL = 24 * 60 * 60 * 1000; // Each process can report at most once a day.
constexpr uint32_t FRAME_NUMBER = 10; // Check memory every ten frames.
constexpr uint32_t CLEAR_TWO_APPS_TIME = 1000; // 1000ms
constexpr const char* MEM_RS_TYPE = "renderservice";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_SNAPSHOT = "snapshot";
constexpr int DUPM_STRING_BUF_SIZE = 4000;
constexpr int KILL_PROCESS_TYPE = 301;
const bool IS_BETA = RSSystemProperties::GetVersionType() == "beta";
constexpr size_t GPU_REPORT_INTERVAL = 60;   // 60s
}

std::mutex MemoryManager::mutex_;
std::unordered_map<pid_t, uint64_t> MemoryManager::pidInfo_;
uint32_t MemoryManager::frameCount_ = 0;
uint64_t MemoryManager::memoryWarning_ = UINT64_MAX;
uint64_t MemoryManager::gpuMemoryControl_ = UINT64_MAX;
uint64_t MemoryManager::totalMemoryReportTime_ = 0;

static std::string Data2String(std::string data, uint32_t targetNumber)
{
    if (data.length() < targetNumber) {
        return std::string(targetNumber - data.length(), ' ') + data;
    } else {
        return data;
    }
}

void MemoryManager::GetNodeInfo(std::unordered_map<int, std::pair<int, int>>& node_info,
    std::unordered_map<int, int>& nullnode_info, std::unordered_map<pid_t, size_t>& modifierSize)
{
    RS_TRACE_NAME_FMT("MemoryManager::GetNodeInfo MemNodeMap size:%zu", MemoryTrack::Instance().GetMemNodeMap().size());
    for (auto& [nodeId, info] : MemoryTrack::Instance().GetMemNodeMap()) {
        auto node = RSMainThread::Instance()->GetContext().GetMutableNodeMap().GetRenderNode(nodeId);
        int pid = info.pid;
        if (node) {
            if (node_info.count(pid)) {
                node_info[pid].first++;
                node_info[pid].second += node->IsOnTheTree() ? 1 : 0;
            } else {
                node_info[pid].first = 1;
                node_info[pid].second = node->IsOnTheTree() ? 1 : 0;
            }
            modifierSize[pid] += node->GetAllModifierSize();
        } else {
            if (nullnode_info.count(pid)) {
                nullnode_info[pid]++;
            } else {
                nullnode_info[pid] = 1;
            }
        }
    }
}

void MemoryManager::RenderServiceAllNodeDump(DfxString& log)
{
    RS_TRACE_NAME("MemoryManager::RenderServiceAllNodeDump");
    std::unordered_map<int, std::pair<int, int>> node_info; // [pid, [count, ontreecount]]
    std::unordered_map<int, int> nullnode_info; // [pid, count]
    std::unordered_map<pid_t, size_t> modifierSize; // [pid, modifiersize]
    constexpr uint32_t NODE_DUMP_STRING_LEN = 8;

    GetNodeInfo(node_info, nullnode_info, modifierSize);
    std::string log_str = Data2String("Pid", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("Count", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("OnTree", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("NodeMemSize", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("DrawableMem", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("ModifierMem", NODE_DUMP_STRING_LEN);
    log.AppendFormat("%s\n", log_str.c_str());
    size_t totalNodeSize = 0;
    for (auto& [pid, info]: node_info) {
        size_t renderNodeSize = MemoryTrack::Instance().GetNodeMemoryOfPid(pid, MEMORY_TYPE::MEM_RENDER_NODE);
        size_t drawableNodeSize = MemoryTrack::Instance().GetNodeMemoryOfPid(pid,
            MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
        size_t modifierNodeSize = modifierSize[pid] / BYTE_CONVERT;
        log_str = Data2String(std::to_string(pid), NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(info.first), NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(info.second), NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(renderNodeSize), NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(drawableNodeSize), NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(modifierNodeSize), NODE_DUMP_STRING_LEN);
        log.AppendFormat("%s\n", log_str.c_str());
        totalNodeSize += renderNodeSize + drawableNodeSize + modifierNodeSize;
    }
    log_str = Data2String("Total Node Size(RenderNode/Drawable/Modifier)", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String(std::to_string(totalNodeSize), NODE_DUMP_STRING_LEN);
    log.AppendFormat("%s\n", log_str.c_str());
    if (!nullnode_info.empty()) {
        log_str = "Purgeable node: \n" +
            Data2String("Pid", NODE_DUMP_STRING_LEN) + "\t" +
            Data2String("Count", NODE_DUMP_STRING_LEN);
        log.AppendFormat("%s\n", log_str.c_str());
        for (auto& [pid, info]: nullnode_info) {
            log_str = Data2String(std::to_string(pid), NODE_DUMP_STRING_LEN) + "\t" +
                Data2String(std::to_string(info), NODE_DUMP_STRING_LEN);
            log.AppendFormat("%s\n", log_str.c_str());
        }
    }
    node_info.clear();
    nullnode_info.clear();
}

void MemoryManager::RenderServiceAllSurfaceDump(DfxString& log)
{
    RS_TRACE_NAME("MemoryManager::RenderServiceAllSurfaceDump");
    log.AppendFormat("%s\n", "the memory of size of all surfaces buffer: ");
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&log](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
        const auto& surfaceHandler = node->GetRSSurfaceHandler();
        const auto& surfaceConsumer = surfaceHandler->GetConsumer();
        std::string dumpSurfaceInfo;
        if (surfaceConsumer != nullptr) {
            surfaceConsumer->Dump(dumpSurfaceInfo);
            if (dumpSurfaceInfo.find("sequence") == std::string::npos) {
                dumpSurfaceInfo = "";
            }
        }
        if (dumpSurfaceInfo.empty()) {
            auto buffer = surfaceHandler->GetBuffer();
            if (buffer) {
                dumpSurfaceInfo += "\nseqNum = " + std::to_string(buffer->GetSeqNum()) +
                    " , bufferWidth = " + std::to_string(buffer->GetWidth()) +
                    " , bufferHeight = " + std::to_string(buffer->GetHeight()) +
                    " , bufferSize = " + std::to_string(buffer->GetSize()) + "\n";
            }
            auto preBuffer = surfaceHandler->GetPreBuffer();
            if (preBuffer) {
                dumpSurfaceInfo += "preSeqNum = " + std::to_string(preBuffer->GetSeqNum()) +
                    " , preBufferWidth = " + std::to_string(preBuffer->GetWidth()) +
                    " , preBufferHeight = " + std::to_string(preBuffer->GetHeight()) +
                    " , preBufferSize = " + std::to_string(preBuffer->GetSize()) + "\n";
            }
        }
        if (!dumpSurfaceInfo.empty()) {
            log.AppendFormat("pid = %d nodeId:%llu ", ExtractPid(node->GetId()), node->GetId());
            log.AppendFormat("%s\n", dumpSurfaceInfo.c_str());
        }
    });
}

void MemoryManager::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
    std::string& type, pid_t pid, bool isLite)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    DfxString log;
    if (pid != 0) {
        RSUniRenderThread::Instance().PostSyncTask([&log, pid] {
            RS_TRACE_NAME_FMT("Dumping memory of pid[%d]", pid);
            DumpPidMemory(log, pid,
                RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        });
    } else {
        DumpMemoryUsage(log, type, isLite);
    }

    if ((type.empty() || type == MEM_GPU_TYPE) && RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        auto subThreadManager = RSSubThreadManager::Instance();
        if (subThreadManager) {
            subThreadManager->DumpMem(log, isLite);
        }
    }
    dumpString.append("dumpMem: " + type + "\n");
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("DumpMem screenManager is nullptr");
        return;
    }
    auto maxScreenInfo = screenManager->GetActualScreenMaxResolution();
    dumpString.append("ScreenResolution = " + std::to_string(maxScreenInfo.phyWidth) +
        "x" + std::to_string(maxScreenInfo.phyHeight) + "\n");
    dumpString.append(log.GetString());
    if (!isLite) {
        RSUniRenderThread::Instance().DumpVkImageInfo(dumpString);
        RSRenderComposerManager::GetInstance().DumpVkImageInfo(dumpString);
    }
#else
    dumpString.append("No GPU in this device");
#endif
}

void MemoryManager::DumpGpuMem(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::string& type)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&nodeTags](const std::shared_ptr<RSSurfaceRenderNode> node) {
        nodeTags.push_back({node->GetId(), ""});
    });
    if (type.empty() || type == MEM_GPU_TYPE) {
        RSUniRenderThread::Instance().DumpGpuMem(log, nodeTags);
        if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
            auto subThreadManager = RSSubThreadManager::Instance();
                if (subThreadManager) {
                    subThreadManager->DumpGpuMem(log, nodeTags);
                }
        }
    }
    DumpGpuNodeMemory(log);
    dumpString.append(log.GetString());
#else
    dumpString.append("No GPU in this device");
#endif
}

void MemoryManager::DumpMemoryUsage(DfxString& log, std::string& type, bool isLite)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log, isLite);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        RSUniRenderThread::Instance().DumpMem(log, isLite);
    }
    if ((type.empty() || type == MEM_SNAPSHOT) && !isLite) {
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

void MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(
    Drawing::GPUContext* gpuContext, bool enabled, const std::function<void()>& setThreadPriority)
{
#if defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr");
        return;
    }
    gpuContext->SetGpuMemoryAsyncReclaimerSwitch(enabled, setThreadPriority);
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
        float gpuMem = gpuTracer.GetGLMemorySizeExcludeDMA();
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
    float totalMem = 0.0f;
    for (const auto& mem : mems) {
        totalMem += mem.GetGpuMemorySize();
    }
    const float maxTotalMem = 4.0f * MEMUNIT_RATE * MEMUNIT_RATE * MEMUNIT_RATE;
    if (totalMem >= maxTotalMem) {
        RS_LOGE("MemoryManager::CountMemoryTotal may overflow: %.2f GB",
                totalMem / (MEMUNIT_RATE * MEMUNIT_RATE * MEMUNIT_RATE));
    }
}

static std::tuple<uint64_t, std::string, RectI, bool> FindGeoById(uint64_t nodeId)
{
    constexpr int maxTreeDepth = 256;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    uint64_t windowId = nodeId;
    std::string windowName = "NONE";
    RectI nodeFrameRect;
    if (!node) {
        return { windowId, windowName, nodeFrameRect, true };
    }
    nodeFrameRect =
        (node->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    // Obtain the window according to childId
    auto parent = node->GetParent().lock();
    bool windowsNameFlag = false;
    int seekDepth = 0;
    while (parent && seekDepth < maxTreeDepth) {
        if (parent->IsInstanceOf<RSSurfaceRenderNode>()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parent);
            windowName = surfaceNode->GetName();
            windowId = surfaceNode->GetId();
            windowsNameFlag = true;
            break;
        }
        parent = parent->GetParent().lock();
        seekDepth++;
    }
    if (!windowsNameFlag) {
        windowName = "EXISTS-BUT-NO-SURFACE";
    }
    return { windowId, windowName, nodeFrameRect, false };
}

static std::tuple<uint64_t, std::string, RectI, bool> FindGeoByIdLite(uint64_t nodeId)
{
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    return { 0ULL, "", RectI(), !node };
}

void MemoryManager::DumpRenderServiceMemory(DfxString& log, bool isLite)
{
    log.AppendFormat("\n----------\nRenderService caches:\n");
    if (isLite) {
        MemoryTrack::Instance().DumpMemoryStatistics(log, FindGeoByIdLite, isLite);
        RenderServiceAllSurfaceDump(log);
    } else {
        MemoryTrack::Instance().DumpMemoryStatistics(log, FindGeoById);
        RenderServiceAllNodeDump(log);
        RenderServiceAllSurfaceDump(log);
    }
#ifdef RS_ENABLE_VK
    RsVulkanMemStat& memStat = RsVulkanContext::GetSingleton().GetRsVkMemStat();
    memStat.DumpMemoryStatistics(&gpuTracer);
#endif
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
    log.AppendFormat("\n---------------\nGPU Caches:%s\n", name.c_str());
    Drawing::TraceMemoryDump gpuTracer("category", true);
    if (tag) {
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        gpuContext->DumpMemoryStatistics(&gpuTracer);
    }
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);
#endif
}

float MemoryManager::DumpGpuCacheNew(
    DfxString& log, const Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag* tag)
{
    if (!gpuContext) {
        log.AppendFormat("gpuContext is nullptr.\n");
        return 0.0f;
    }
    /* GPU */
    float ret = 0.0f;
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TraceMemoryDump gpuTracer("category", true);
    if (tag) {
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        gpuContext->DumpMemoryStatistics(&gpuTracer);
    }
    ret = gpuTracer.GetGLMemorySizeExcludeDMA();
#endif
    return ret;
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

void MemoryManager::DumpGpuNodeMemory(DfxString& log)
{
    std::string gpuMemInfo;
    std::ifstream gpuMemInfoFile;
    gpuMemInfoFile.open(GPUMEM_NODEINFO_PATH);
    if (gpuMemInfoFile.is_open()) {
        std::stringstream gpuMemInfoStream;
        gpuMemInfoStream << gpuMemInfoFile.rdbuf();
        gpuMemInfo = gpuMemInfoStream.str();
        gpuMemInfoFile.close();
    } else {
        RS_LOGE("MemoryManager::DumpGpuNodeMemory can not open gpumem info");
    }
    log.AppendFormat("\n---------------\nGPU Memory:\n");
    log.AppendFormat("%s", gpuMemInfo.c_str());
    log.AppendFormat("\n---------------\n");
}

static int32_t MemoryTrackerGetGLByPid(int32_t pid)
{
    return pid;
}

void MemoryManager::DumpAllGpuInfoNew(DfxString& log, const Drawing::GPUContext* gpuContext,
    const std::vector<std::pair<NodeId, std::string>>& nodeTags)
{
    if (!gpuContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    std::unordered_map<pid_t, float> memoryMap;
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    for (auto& nodeTag : nodeTags) {
        pid_t pid = ExtractPid(nodeTag.first);
        memoryMap[pid] = 0.0f;
    }
    log.AppendFormat("\n---------------\nGPU Memory Data:\n");
    float totalMemoryKB = 0.0f;
    int32_t rsMemory = MemoryTrackerGetGLByPid(getpid());
    for (auto& [pid, memory] : memoryMap) {
        Drawing::GPUResourceTag tag(pid, 0, 0, 0, "ReleaseUnlockGpuResource");
        memory = DumpGpuCacheNew(log, gpuContext, &tag);
        float memoryKB = memory / MEMUNIT_RATE;
        totalMemoryKB += memoryKB;
        if (memoryKB >= 0.1f) {
            log.AppendFormat("pid: %d, gpu memory: %.2fKB\n", pid, memoryKB);
        }
    }
    rsMemory -= static_cast<int32_t>(round(totalMemoryKB));
    log.AppendFormat("Render Service GPU memory: %d KB\n", rsMemory);
#endif
}

void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const Drawing::GPUContext* gpuContext,
    std::vector<std::pair<NodeId, std::string>>& nodeTags, bool isLite)
{
    RS_TRACE_NAME_FMT("MemoryManager::DumpDrawingGpuMemory nodeTags size:%zu", nodeTags.size());
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
    DumpAllGpuInfoNew(log, gpuContext, nodeTags);
    for (uint32_t tagtype = RSTagTracker::TAG_DRAW_SURFACENODE + 1;
        tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
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
    std::shared_ptr<RenderContext> rendercontext = RenderContext::Create();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());
    // gpu stat
    if (!isLite) {
        DumpGpuStats(log, gpuContext);
    }
#endif
}

void MemoryManager::DumpGpuStats(DfxString& log, const Drawing::GPUContext* gpuContext)
{
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    std::string stat;
    gpuContext->DumpGpuStats(stat);

    size_t statIndex = 0;
    size_t statLength = stat.length();
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
    {
        static thread_local int tid = gettid();
        log.AppendFormat("\n------------------\n[%s:%d] dumpAllResource:\n", GetThreadName(), tid);
        std::stringstream allResources;
        gpuContext->DumpAllResource(allResources);
        std::string s;
        while (std::getline(allResources, s, '\n')) {
            log.AppendFormat("%s\n", s.c_str());
        }
    }
}

void ProcessJemallocString(std::string* sp, const char* str)
{
    sp->append("strbuf size = " + std::to_string(strlen(str)) + "\n");
    // split ///////////////////////////////
    std::vector<std::string> lines;
    std::string currentLine;

    for (int i = 0; str[i] != '\0' && i < INT_MAX; ++i) {
        if (str[i] == '\n') {
            lines.push_back(currentLine);
            currentLine.clear();
        } else {
            currentLine += str[i];
        }
    }
    // last line
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    // compute tcache and decay free ///////////////////////
    // tcache_bytes:                     784
    // decaying:  time       npages       sweeps     madvises       purged
    //   dirty:   N/A           94         5084        55957       295998
    //   muzzy:   N/A            0         3812        39219       178519
    const char* strArray[] = {"tcache_bytes:", "decaying:", "   dirty:", "   muzzy:"};
    size_t size = sizeof(strArray) / sizeof(strArray[0]);
    size_t total = 0;
    for (const auto& line : lines) {
        for (size_t i = 0; i < size; ++i) {
            if (strncmp(line.c_str(), strArray[i], strlen(strArray[i])) == 0) {
                sp->append(line + "\n");
                total ++;
            }
        }

        // get first one: (the total one, others are separated by threads)
        if (total >= size) {
            break;
        }
    }
}

void MemoryManager::DumpMemorySnapshot(DfxString& log)
{
    size_t totalMemory = MemorySnapshot::Instance().GetTotalMemory();
    log.AppendFormat("\n---------------\nmemorySnapshots, totalMemory %zuKB\n", totalMemory / MEMUNIT_RATE);
    std::unordered_map<pid_t, MemorySnapshotInfo> memorySnapshotInfo;
    MemorySnapshot::Instance().GetMemorySnapshot(memorySnapshotInfo);
    for (auto& [pid, snapshotInfo] : memorySnapshotInfo) {
        std::string infoStr = "pid: " + std::to_string(pid) +
            ", uid: " + std::to_string(snapshotInfo.uid) + ", bundleName: " + snapshotInfo.bundleName +
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
    auto featureParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[MEM]);
    if (!featureParam) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not get mem featureParam");
        return;
    }
    std::string rsWatchPointParamName = std::static_pointer_cast<MEMParam>(featureParam)->GetRSWatchPoint();
    if (rsWatchPointParamName.empty()) {
        RS_LOGI("MemoryManager::InitMemoryLimit can not find rsWatchPoint");
        return;
    }

    std::ifstream configFile;
    configFile.open(KERNEL_CONFIG_PATH);
    if (!configFile.is_open()) {
        RS_LOGE("MemoryManager::InitMemoryLimit can not open config file");
        return;
    }
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
    cJSON* rsWatchPoint = cJSON_GetObjectItem(version, rsWatchPointParamName.c_str());
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
    gpuContext->InitGpuMemoryReportLimit(GpuMemoryOverReport, GPU_REPORT_INTERVAL, gpuMemoryControl_);
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
                std::string filePath = GRAPHIC_MEM_DIR + "renderservice_mem_warning_report.txt";
                MemoryOverReport(pid, memoryInfo, RSEventName::RENDER_MEMORY_OVER_WARNING, "", filePath);
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
        auto& appMgrClient = RSSingleton<AppExecFwk::AppMgrClient>::GetInstance();
        appMgrClient.GetBundleNameByPid(pid, mInfo.bundleName, mInfo.uid);
    }
    MemorySnapshot::Instance().FillMemorySnapshot(infoMap);
}

static void KillProcessByPid(const pid_t pid, const MemorySnapshotInfo& info, const std::string& reason)
{
#ifdef RS_ENABLE_UNI_RENDER
    if (pid > 0) {
        int32_t eventWriteStatus = -1;
        AAFwk::ExitReason killReason{AAFwk::Reason::REASON_RESOURCE_CONTROL, KILL_PROCESS_TYPE, reason};
        int32_t ret = (int32_t)AAFwk::AbilityManagerClient::GetInstance()->KillProcessWithReason(pid, killReason);
        if (ret == ERR_OK) {
            RS_TRACE_NAME("KillProcessByPid HiSysEventWrite");
            eventWriteStatus = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::FRAMEWORK, "PROCESS_KILL",
                HiviewDFX::HiSysEvent::EventType::FAULT, "PID", pid, "PROCESS_NAME", info.bundleName,
                "MSG", reason, "FOREGROUND", true, "UID", info.uid, "BUNDLE_NAME", info.bundleName,
                "REASON", GPU_RS_LEAK);
        }
        // To prevent the print from being filtered, use RS_LOGE.
        std::string logInfo = "KillProcessByPid, pid: " + std::to_string(pid) + ", process name: " + info.bundleName +
            "killStatus: " + std::to_string(ret) + ", eventWriteStatus: " + std::to_string(eventWriteStatus) +
            ", reason: " + reason;
        RS_LOGE("%{public}s", logInfo.c_str());
    }
#endif
}

bool MemoryManager::MemoryOverflow(pid_t pid, size_t overflowMemory, bool isGpu)
{
    if (pid == 0) {
        RS_LOGD("MemoryManager::MemoryOverflow pid = 0");
        return false;
    }
    MemorySnapshotInfo info;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    if (isGpu) {
        info.gpuMemory = overflowMemory;
    }
    if (info.bundleName.empty()) {
        auto& appMgrClient = RSSingleton<AppExecFwk::AppMgrClient>::GetInstance();
        appMgrClient.GetBundleNameByPid(pid, info.bundleName, info.uid);
    }
    if (info.bundleName == SCB_BUNDLE_NAME && !MEMParam::IsKillScbEnabled()) {
        return false;
    }
    if (IS_BETA) {
        RSMainThread::Instance()->PostTask([pid, info]() mutable {
            RS_TRACE_NAME_FMT("RSMem Dump Task");
            std::unordered_set<std::u16string> argSets;
            std::string dumpString = "";
            std::string type = MEM_GPU_TYPE;
            DumpMem(argSets, dumpString, type, 0);
            std::string filePath = GRAPHIC_MEM_DIR + "renderservice_mem_kill_report.txt";
            MemoryOverReport(pid, info, RSEventName::RENDER_MEMORY_OVER_ERROR, dumpString, filePath);
        });
    } else {
        std::string filePath = GRAPHIC_MEM_DIR + "renderservice_mem_kill_report.txt";
        MemoryOverReport(pid, info, RSEventName::RENDER_MEMORY_OVER_ERROR, "", filePath);
    }
    std::string reason = "RENDER_MEMORY_OVER_ERROR: cpu[" + std::to_string(info.cpuMemory)
        + "], gpu[" + std::to_string(info.gpuMemory) + "], total["
        + std::to_string(info.TotalMemory()) + "]";

    KillProcessByPid(pid, info, reason);
    RS_LOGE("RSMemoryOverflow pid[%{public}d] cpu[%{public}zu] gpu[%{public}zu]", pid, info.cpuMemory, info.gpuMemory);
    return true;
}

bool MemoryManager::GpuMemoryOverReport(pid_t pid, size_t overflowMemory,
    std::unordered_map<std::string, std::pair<size_t, size_t>> typeInfo, std::unordered_map<pid_t, size_t> pidInfo)
{
    RS_TRACE_NAME("MemoryManager::GpuMemoryOverReport");
    std::string gpuInfo = "total gpu info:\n";
    for (const auto &[type, info] : typeInfo) {
        gpuInfo += type + ": " + std::to_string(info.first / MEMUNIT_RATE) + "KB (" + std::to_string(info.second) +
            " entries)\n";
    }
    gpuInfo += "\ngpu memory data of pid:\n";
    for (const auto &[pid, memSize] : pidInfo) {
        MemorySnapshotInfo info;
        MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
        info.gpuMemory = memSize;
        gpuInfo += "pid: " + std::to_string(pid) + ", bundleName: " + info.bundleName +
            ", gpu: " +  std::to_string(memSize / MEMUNIT_RATE) + "KB\n";
    }
    MemorySnapshotInfo maxInfo;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, maxInfo);
    maxInfo.gpuMemory = overflowMemory;
    gpuInfo += "\nRENDER_MEMORY_OVER_ERROR: cpu[" + std::to_string(maxInfo.cpuMemory) +
        "], gpu[" + std::to_string(maxInfo.gpuMemory) + "], total[" + std::to_string(maxInfo.TotalMemory()) + "]" +
        " KillProcessByPid, pid: " + std::to_string(pid) + ", process name: " + maxInfo.bundleName;
    std::string filePath = GRAPHIC_MEM_DIR + "renderservice_killProcessByPid.txt";
    MemoryOverReport(pid, maxInfo, RSEventName::RENDER_MEMORY_OVER_ERROR, gpuInfo, filePath);
    return true;
}

void MemoryManager::MemoryOverReport(const pid_t pid, const MemorySnapshotInfo& info, const std::string& reportName,
    const std::string& hidumperReport, const std::string& filePath)
{
    std::string gpuMemInfo;
    std::ifstream gpuMemInfoFile;
    gpuMemInfoFile.open(GPUMEM_INFO_PATH);
    if (gpuMemInfoFile.is_open()) {
        std::stringstream gpuMemInfoStream;
        gpuMemInfoStream << gpuMemInfoFile.rdbuf();
        gpuMemInfo = gpuMemInfoStream.str();
        gpuMemInfoFile.close();
    } else {
        gpuMemInfo = reportName;
        RS_LOGE("MemoryManager::MemoryOverReport can not open gpumem info");
    }

    RS_TRACE_NAME("MemoryManager::MemoryOverReport HiSysEventWrite");
    WriteInfoToFile(filePath, gpuMemInfo, hidumperReport);

    int ret = RSHiSysEvent::EventWrite(reportName, RSEventType::RS_STATISTIC,
        "PID", pid,
        "TYPE", "GPU",
        "BUNDLE_NAME", info.bundleName,
        "CPU_MEMORY", info.cpuMemory,
        "GPU_MEMORY", info.gpuMemory,
        "TOTAL_MEMORY", info.TotalMemory(),
        "FILEPATH", filePath);
    RS_LOGW("hisysevent writ result=%{public}d, send event [FRAMEWORK,PROCESS_KILL], "
        "pid[%{public}d] bundleName[%{public}s] cpu[%{public}zu] gpu[%{public}zu] total[%{public}zu]",
        ret, pid, info.bundleName.c_str(), info.cpuMemory, info.gpuMemory, info.TotalMemory());
}

void MemoryManager::WriteInfoToFile(
    const std::string& filePath, std::string& gpuMemInfo, const std::string& hidumperReport)
{
    std::ofstream tempFile(filePath);
    if (tempFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm* localTime = std::localtime(&nowTime);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
        tempFile << "time: " << timeStr << "\n";
        if (!hidumperReport.empty()) {
            tempFile << "\n******************************\n";
            tempFile << "LOGGER_RENDER_SERVICE_MEM\n";
            tempFile << hidumperReport;
        }
        tempFile << "\n******************************\n";
        tempFile << gpuMemInfo;
        tempFile << "\n************ endl ************\n";
    } else {
        RS_LOGE("MemoryOverReport::file open fail!");
    }
    tempFile.close();
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

void MemoryManager::DumpExitPidMem(std::string& log, int pid)
{
    RS_TRACE_NAME_FMT("DumpExitPidMem");
    DfxString dfxlog;
    auto mem = MemoryTrack::Instance().CountRSMemory(pid);
    size_t allNodeAndPixelmapSize = mem.GetTotalMemorySize();
    dfxlog.AppendFormat("allNodeAndPixelmapSize: %zu \n", allNodeAndPixelmapSize);

    size_t allModifySize = 0;
    RSMainThread::Instance()->ScheduleTask([pid, &allModifySize] () {
        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
        nodeMap.TraversalNodesByPid(pid, [&allModifySize] (const std::shared_ptr<RSBaseRenderNode>& node) {
            allModifySize += node->GetAllModifierSize();
        });
    }).wait();
    dfxlog.AppendFormat("allModifySize: %zu \n", allModifySize);

    size_t allGpuSize = 0;
    RSUniRenderThread::Instance().PostSyncTask([&allGpuSize, pid] {
        MemoryGraphic mem = CountPidMemory(pid,
            RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        allGpuSize += static_cast<size_t>(mem.GetGpuMemorySize());
    });
    dfxlog.AppendFormat("allGpuSize: %zu \n", allGpuSize);
    dfxlog.AppendFormat("pid: %d totalSize: %zu \n", pid, (allNodeAndPixelmapSize + allModifySize + allGpuSize));
    log.append(dfxlog.GetString());
}

RSReclaimMemoryManager& RSReclaimMemoryManager::Instance()
{
    static RSReclaimMemoryManager instance;
    return instance;
}

void RSReclaimMemoryManager::TriggerReclaimTask()
{
    // Clear two Applications in one second, post task to reclaim.
    auto& unirenderThread = RSUniRenderThread::Instance();
    if (!unirenderThread.IsTimeToReclaim()) {
        static std::chrono::steady_clock::time_point lastClearAppTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        bool isTimeToReclaim = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastClearAppTime).count() < CLEAR_TWO_APPS_TIME;
        if (isTimeToReclaim) {
            unirenderThread.ReclaimMemory();
            unirenderThread.SetTimeToReclaim(true);
            isReclaimInterrupt_.store(false);
        }
        lastClearAppTime = currentTime;
    }
}

void RSReclaimMemoryManager::InterruptReclaimTask(const std::string& sceneId)
{
    // When operate in launcher, interrupt reclaim task.
    if (!isReclaimInterrupt_.load() && sceneId != EVENT_ENTER_RECENTS) {
        isReclaimInterrupt_.store(true);
    }
}

} // namespace OHOS::Rosen