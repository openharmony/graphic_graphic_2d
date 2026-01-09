/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "rs_pipline_dumper.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <memory>
#include <parameters.h>
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#include <string>
#include <system_ability_definition.h>
#include <unistd.h>

#include "hgm_core.h"
#include "memory/rs_memory_manager.h"
#include "parameter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_profiler.h"
#include "vsync_generator.h"
#include "rs_trace.h"
#include "ge_mesa_blur_shader_filter.h"

#include "pipeline/rs_surface_render_node.h"
#include "system/rs_system_parameters.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "dfx/rs_pipline_dump_manager.h"
#include "graphic_feature_param_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr size_t CLIENT_DUMP_TREE_TIMEOUT = 2000; // 2000ms
#ifdef RS_ENABLE_GPU
static const int INT_INIT_VAL = 0;
static const int CREAT_NUM_ONE = 1;
static const int INIT_EGL_VERSION = 3;
static EGLDisplay g_tmpDisplay = EGL_NO_DISPLAY;
static EGLContext g_tmpContext = EGL_NO_CONTEXT;
#endif
}

RSPiplineDumper::RSPiplineDumper(std::shared_ptr<AppExecFwk::EventHandler> mainHandler)
    : mainHandler_(mainHandler) {}

static bool IsNumber(const std::string& type)
{
    auto number = static_cast<uint32_t>(std::count_if(type.begin(), type.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == type.length();
}

static bool ExtractDumpInfo(std::unordered_set<std::u16string>& argSets, std::string& dumpInfo, std::u16string title)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpInfo.append("\n---------------\n Not in UniRender and no information");
        return false;
    }
    if (argSets.size() < 2) {
        dumpInfo.append("\n---------------\n no need extract info");
        return false;
    }
    argSets.erase(title);
    if (!argSets.empty()) {
        dumpInfo = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
    return true;
}

#ifdef RS_ENABLE_GPU
static void InitGLES()
{
    g_tmpDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_tmpDisplay == EGL_NO_DISPLAY) {
        RS_LOGE("Failed to get default display.");
        return;
    }
    EGLint major, minor;
    if (eglInitialize(g_tmpDisplay, &major, &minor) == EGL_FALSE) {
        RS_LOGE("Failed to initialize EGL.");
        return;
    }

    EGLint numConfigs = INT_INIT_VAL;
    EGLConfig config;
    if (eglChooseConfig(g_tmpDisplay, nullptr, &config, CREAT_NUM_ONE, &numConfigs) == EGL_FALSE || numConfigs < 1) {
        RS_LOGE("Failed to choose EGL config.");
        eglTerminate(g_tmpDisplay);
        return;
    }
    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, INIT_EGL_VERSION, EGL_NONE };
    g_tmpContext = eglCreateContext(g_tmpDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (g_tmpContext == EGL_NO_CONTEXT) {
        RS_LOGE("Failed to create EGL context.");
        eglTerminate(g_tmpDisplay);
        return;
    }
    if (eglMakeCurrent(g_tmpDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, g_tmpContext) == EGL_FALSE) {
        RS_LOGE("Failed to make EGL context current.");
        eglDestroyContext(g_tmpDisplay, g_tmpContext);
        eglTerminate(g_tmpDisplay);
        return;
    }
}

static void DestroyGLES()
{
    eglDestroyContext(g_tmpDisplay, g_tmpContext);
    g_tmpContext = EGL_NO_CONTEXT;
    eglTerminate(g_tmpDisplay);
    g_tmpDisplay = EGL_NO_DISPLAY;
}
#endif

uint32_t RSPiplineDumper::GenerateTaskId()
{
    static std::atomic<uint32_t> id;
    return id.fetch_add(1, std::memory_order::memory_order_relaxed);
}

void RSPiplineDumper::RpDumpInit(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    // 用于renderProcess进程Dump初始化
    RegisterRSTreeFuncs(rpDumpManager);
    RegisterGpuFuncs(rpDumpManager);
    RegisterRSGfxFuncs(rpDumpManager);
    RegisterMemFuncs(rpDumpManager);
    RegisterBufferFuncs(rpDumpManager);
    RegisterSurfaceInfoFuncs(rpDumpManager);
}

void RSPiplineDumper::RegisterRSGfxFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    // Event Param List
    RSDumpFunc rsEventParamFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                         std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpRSEvenParam(dumpString); });
    };

    // rs log flag
    RSDumpFunc rsLogFlagFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                      std::string &dumpString) -> void {
        argSets.erase(cmd);
        if (!argSets.empty()) {
            std::string logFlag =
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(*argSets.begin());
            if (RSLogManager::GetInstance().SetRSLogFlag(logFlag)) {
                dumpString.append("Successed to set flag: " + logFlag + "\n");
            } else {
                dumpString.append("Failed to set flag: " + logFlag + "\n");
            }
        }
    };

    // flush Jank Stats
    RSDumpFunc flushJankStatsRsFunc = [this](const std::u16string &cmd,
                                             std::unordered_set<std::u16string> &argSets,
                                             std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpJankStatsRs(dumpString); });
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::EVENT_PARAM_LIST, rsEventParamFunc },
        { RSDumpID::RS_LOG_FLAG, rsLogFlagFunc },
        { RSDumpID::RS_FLUSH_JANK_STATS, flushJankStatsRsFunc },
    };
    rpDumpManager->Register(handers);
}

void RSPiplineDumper::RegisterRSTreeFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    // RS not on Tree
    RSDumpFunc rsNotOnTreeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                        std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpNodesNotOnTheTree(dumpString); });
    };

    // RS Tree
    RSDumpFunc rsTreeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                   std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpRenderServiceTree(dumpString); });
    };

    // RS SurfaceNode
    RSDumpFunc surfaceNodeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                        std::string &dumpString) -> void {
        argSets.erase(cmd);
        if (!argSets.empty()) {
            NodeId id =
                static_cast<NodeId>(std::atoll(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}
                                                   .to_bytes(*argSets.begin())
                                                   .c_str()));
            ScheduleTask([this, &dumpString, &id]() { return DumpSurfaceNode(dumpString, id); });
        }
    };

    // Multi RS Trees
    RSDumpFunc multiRSTreesFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                         std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpRenderServiceTree(dumpString, false); });
    };

    // client tree
    RSDumpFunc rsClientFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                     std::string &dumpString) -> void {
        auto taskId = GenerateTaskId();
        ScheduleTask([this, taskId]() { RSMainThread::Instance()->SendClientDumpNodeTreeCommands(taskId); });
        RSMainThread::Instance()->CollectClientNodeTreeResult(taskId, dumpString, CLIENT_DUMP_TREE_TIMEOUT);
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::RS_NOT_ON_TREE_INFO, rsNotOnTreeFunc },
        { RSDumpID::RENDER_NODE_INFO, rsTreeFunc, RS_MAIN_THREAD_TAG },
        { RSDumpID::SURFACENODE_INFO, surfaceNodeFunc },
        { RSDumpID::MULTI_RSTREES_INFO, multiRSTreesFunc },
        { RSDumpID::CLIENT_INFO, rsClientFunc, RS_CLIENT_TAG },
    };

    rpDumpManager->Register(handers);
}

void RSPiplineDumper::RegisterMemFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    // surface mem
    RSDumpFunc surfaceMemFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                       std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpAllNodesMemSize(dumpString); });
    };

    // Mem
    RSDumpFunc memDumpFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                    std::string &dumpString) -> void {
        DumpMem(argSets, dumpString);
    };

    // pid mem
    RSDumpFunc existPidMemFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                        std::string &dumpString) -> void {
        argSets.erase(cmd);
        DumpExistPidMem(argSets, dumpString);
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::SURFACE_MEM_INFO, surfaceMemFunc },
        { RSDumpID::MEM_INFO, memDumpFunc },
        { RSDumpID::EXIST_PID_MEM_INFO, existPidMemFunc },
    };

    rpDumpManager->Register(handers);
}

void RSPiplineDumper::RegisterGpuFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
#ifdef RS_ENABLE_VK
    // vk texture Limit
    RSDumpFunc vktextureLimitFunc = [this](const std::u16string &cmd,
                                           std::unordered_set<std::u16string> &argSets,
                                           std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpVkTextureLimit(dumpString); });
    };
#endif

    std::vector<RSDumpHander> handers = {
#ifdef RS_ENABLE_VK
        { RSDumpID::VK_TEXTURE_LIMIT, vktextureLimitFunc },
#endif
    };

    rpDumpManager->Register(handers);
}

void RSPiplineDumper::RegisterBufferFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    RSDumpFunc currentFrameBufferFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                               std::string &dumpString) -> void {
        // todo : 分离渲染适配
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RS_TRACE_NAME("RSProcessDumper dump current frame buffer in UniRenderThread");
            RS_LOGD("dump current frame buffer in UniRenderThread");
            RSUniRenderThread::Instance().DumpCurrentFrameLayers();
        } else {
// #ifdef RS_ENABLE_GPU
//             RSHardwareThread::Instance().ScheduleTask([this]() {
//                 RS_TRACE_NAME("RSRenderService dump current frame buffer in HardwareThread");
//                 RS_LOGD("dump current frame buffer in HardwareThread");
//                 return screenManager_->DumpCurrentFrameLayers();
//             });
// #endif
        }
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::CURRENT_FRAME_BUFFER, currentFrameBufferFunc },
    };

    rpDumpManager->Register(handers);
}

void RSPiplineDumper::RegisterSurfaceInfoFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager)
{
    // todo : 分离渲染适配
    // surface info
    RSDumpFunc surfaceInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                        std::string &dumpString) -> void {
        RSUniRenderThread::Instance().DumpSurfaceInfo(dumpString);
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::SURFACE_INFO, surfaceInfoFunc, RS_HW_THREAD_TAG },
    };

    rpDumpManager->Register(handers);
}

void RSPiplineDumper::DumpNodesNotOnTheTree(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- Node Not On Tree\n");

    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraversalNodes([&dumpString](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }

        if (node->IsInstanceOf<RSSurfaceRenderNode>() && !node->IsOnTheTree()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
            dumpString += "\n node Id[" + std::to_string(node->GetId()) + "]:\n";
            const auto& surfaceConsumer = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
            if (surfaceConsumer == nullptr) {
                return;
            }
            surfaceConsumer->Dump(dumpString);
        }
    });
}

void RSPiplineDumper::DumpRenderServiceTree(std::string& dumpString, bool forceDumpSingleFrame) const
{
    dumpString.append("\n");
    dumpString.append("-- RenderServiceTreeDump: \n");
#ifdef RS_ENABLE_GPU
    RSMainThread::Instance()->RenderServiceTreeDump(dumpString, forceDumpSingleFrame, true);
#endif
}

void RSPiplineDumper::DumpAllNodesMemSize(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- All Surfaces Memory Size\n");
    dumpString.append("the memory size of all surfaces buffer is : dumpend");

    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraversalNodes([&dumpString](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr || !node->IsInstanceOf<RSSurfaceRenderNode>()) {
            return;
        }

        const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        const auto& surfaceConsumer = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
        if (surfaceConsumer == nullptr) {
            return;
        }

        surfaceConsumer->Dump(dumpString);
    });
}

void RSPiplineDumper::DumpRSEvenParam(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- EventParamListDump: \n");
    RSMainThread::Instance()->RsEventParamDump(dumpString);
}

void RSPiplineDumper::DumpJankStatsRs(std::string& dumpString) const
{
    dumpString.append("\n");
    RSJankStats::GetInstance().ReportJankStats();
    dumpString.append("flush done\n");
}

#ifdef RS_ENABLE_VK
void RSPiplineDumper::DumpVkTextureLimit(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- vktextureLimit:\n");
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t maxTextureWidth = deviceProperties.limits.maxImageDimension2D;
    uint32_t maxTextureHeight = deviceProperties.limits.maxImageDimension2D;
    dumpString.append(
        "width: " + std::to_string(maxTextureWidth) + " height: " + std::to_string(maxTextureHeight) + "\n");
}
#endif

void RSPiplineDumper::DumpExistPidMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\n Not in UniRender and no information");
        return;
    }

    std::string type;
    if (!argSets.empty()) {
        type = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
    int pid = 0;
    if (!type.empty() && IsNumber(type)) {
        pid = std::atoi(type.c_str());
        MemoryManager::DumpExitPidMem(dumpString, pid);
    } else {
        dumpString.append("\n---------------\n Pid is error \n" + type);
    }
}

void RSPiplineDumper::DumpSurfaceNode(std::string& dumpString, NodeId id) const
{
    dumpString.append("\n");
    dumpString.append("-- SurfaceNode\n");

    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(id);
    if (node == nullptr) {
        dumpString +=  std::to_string(id) + " is invalid ID\n";
        return;
    }
    dumpString += "ID: " + std::to_string(node->GetId()) + "\n";
    dumpString += "Name: " + node->GetName() + "\n";
    dumpString += "SrcRect: [" + std::to_string(node->GetSrcRect().left_) + "," +
        std::to_string(node->GetSrcRect().top_) + "," +
        std::to_string(node->GetSrcRect().width_) + "," +
        std::to_string(node->GetSrcRect().height_) + "]\n";
    dumpString += "DstRect: [" + std::to_string(node->GetDstRect().left_) + "," +
        std::to_string(node->GetDstRect().top_) + "," +
        std::to_string(node->GetDstRect().width_) + "," +
        std::to_string(node->GetDstRect().height_) + "]\n";
    dumpString += "CornerRadius: [" + std::to_string(node->GetRenderProperties().GetCornerRadius().x_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().y_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().z_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().w_) + "]\n";
    dumpString += "Bounds: [" + std::to_string(node->GetRenderProperties().GetBoundsWidth()) + "," +
        std::to_string(node->GetRenderProperties().GetBoundsHeight()) + "]\n";
    if (auto& contextClipRegion = node->contextClipRect_) {
        dumpString += "ContextClipRegion: [" + std::to_string(contextClipRegion->GetWidth()) + "," +
                      std::to_string(contextClipRegion->GetHeight()) + "]\n";
    } else {
        dumpString += "ContextClipRegion: [ empty ]\n";
    }
    dumpString += "Zorder: " + std::to_string(node->GetRenderProperties().GetPositionZ()) + "\n";
    dumpString += "IsOnTheTree: " + std::to_string(node->IsOnTheTree()) + "\n";
    dumpString += "Visible: " + std::to_string(node->GetRenderProperties().GetVisible()) + "\n";
    dumpString += "OcclusionBg: " + std::to_string(node->GetAbilityBgAlpha())+ "\n";
    dumpString += "Alpha: " + std::to_string(node->GetRenderProperties().GetAlpha()) +
                  "(include ContextAlpha: " + std::to_string(node->contextAlpha_) + ")\n";
    dumpString += "GlobalAlpha: " + std::to_string(node->GetGlobalAlpha()) + "\n";
    dumpString += node->GetVisibleRegion().GetRegionInfo() + "\n";
    const auto consumer = node->GetRSSurfaceHandler()->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    dumpString += "Consumer Info: \n";
    consumer->Dump(dumpString);
}

void RSPiplineDumper::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\nNot in UniRender and no information");
        return;
    }
    std::string type;
    if (argSets.size() > 1) {
        argSets.erase(u"dumpMem");
        if (!argSets.empty()) {
            type = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
        }
    }
    int pid = 0;
    if (!type.empty() && IsNumber(type)) {
        pid = std::atoi(type.c_str());
    }
    ScheduleTask([this, &argSets, &dumpString, &type, &pid]() {
            return RSMainThread::Instance()->DumpMem(argSets, dumpString, type, pid);
        });
}

void RSPiplineDumper::ScheduleTask(std::function<void()> task) const
{
    if (mainHandler_) {
        mainHandler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return;
    }
    RS_LOGE("RSPiplineDumper::ScheduleTask mainHandler_ is null");
}
} // Rosen
} // OHOS