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
#include "rs_service_dumper.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <parameters.h>
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#include <string>
#include <system_ability_definition.h>
#include <unistd.h>

#include "hgm_core.h"
#include "memory/rs_memory_manager.h"
#include "parameter.h"
#include "rs_profiler.h"
#include "vsync_generator.h"
#include "rs_trace.h"
#include "ge_mesa_blur_shader_filter.h"

#include "feature/uifirst/rs_sub_thread_manager.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "dfx/rs_service_dump_manager.h"
#include "rs_render_composer_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
#ifdef RS_ENABLE_GPU
static const int INT_INIT_VAL = 0;
static const int CREAT_NUM_ONE = 1;
static const int INIT_EGL_VERSION = 3;
static EGLDisplay g_tmpDisplay = EGL_NO_DISPLAY;
static EGLContext g_tmpContext = EGL_NO_CONTEXT;
#endif
}

RSServiceDumper::RSServiceDumper(std::shared_ptr<AppExecFwk::EventHandler> mainHandler,
    sptr<RSScreenManager> screenManager, std::shared_ptr<RSRenderComposerManager> rsRenderComposerManager)
    : mainHandler_(mainHandler), screenManager_(screenManager), rsRenderComposerManager_(rsRenderComposerManager) {}

static bool IsNumber(const std::string& type)
{
    auto number = static_cast<uint32_t>(std::count_if(type.begin(), type.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == type.length();
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

void RSServiceDumper::RsDumpInit(std::shared_ptr<RSServiceDumpManager> rsDumpManager)
{
    // for renderService dump init
    RegisterGpuFuncs(rsDumpManager);
    RegisterMemFuncs(rsDumpManager);
    RegisterFpsFuncs(rsDumpManager);
    RegisterRSGfxFuncs(rsDumpManager);
}

void RSServiceDumper::RegisterRSGfxFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager)
{
    // screen info
    RSDumpFunc screenInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                       std::string &dumpString) -> void {
        RS_LOGE("hwj RSServiceDumper::RegisterRSGfxFuncs screenInfoFunc");
        ScheduleTask([this, &dumpString]() { screenManager_->DisplayDump(dumpString); });
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

    std::vector<RSDumpHander> handers = {
        { RSDumpID::SCREEN_INFO, screenInfoFunc },
        { RSDumpID::RS_LOG_FLAG, rsLogFlagFunc },
    };
    rsDumpManager->Register(handers);
}

void RSServiceDumper::DumpAllScreenPowerStatus(std::string& dumpString)
{
    std::vector<ScreenId> allScreen = screenManager_->GetAllScreenIds();
    bool powerStatus = false;
    dumpString += "\n-- ScreenPowerStatus --\n";
    for (auto screenId : allScreen) {
        powerStatus = (screenManager_->GetScreenPowerStatus(screenId) == ScreenPowerStatus::POWER_STATUS_SUSPEND ||
                       screenManager_->GetScreenPowerStatus(screenId) == ScreenPowerStatus::POWER_STATUS_OFF);
        dumpString += "ScreenId: " + std::to_string(screenId) + "; IsPowerOff: " + std::to_string(powerStatus) + "\n";
    }
}

void RSServiceDumper::RegisterMemFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager)
{
    // surface info
    RSDumpFunc surfaceInfoFunc = [this](const std::u16string& cmd, std::unordered_set<std::u16string>& argSets,
                                        std::string& dumpString) -> void {
        ScheduleTask([this, &dumpString]() {
            rsRenderComposerManager_->SurfaceDump(dumpString);
            DumpAllScreenPowerStatus(dumpString);
        });
    };

    // surface mem
    RSDumpFunc surfaceMemFunc = [this](const std::u16string& cmd, std::unordered_set<std::u16string>& argSets,
                                       std::string& dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpAllNodesMemSize(dumpString); });
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::SURFACE_INFO, surfaceInfoFunc, RS_HW_THREAD_TAG },
        { RSDumpID::SURFACE_MEM_INFO, surfaceMemFunc },
    };

    rsDumpManager->Register(handers);
}

void RSServiceDumper::RegisterFpsFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager)
{
    // fps info cmd, [windowname]/composer fps
    RSDumpFunc fpsInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                    std::string &dumpString) -> void {
        FPSDumpProcess(argSets, dumpString, cmd);
    };

    // fps clear cmd : [surface name]/composer fpsClear
    RSDumpFunc fpsClearFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                     std::string &dumpString) -> void {
        FPSDumpClearProcess(argSets, dumpString, cmd);
    };

    // fps count
    RSDumpFunc fpsCountFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                     std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpRefreshRateCounts(dumpString); });
    };

    // clear fps Count
    RSDumpFunc clearFpsCountFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                          std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpClearRefreshRateCounts(dumpString); });
    };

    // [windowname] hitchs
    RSDumpFunc hitchsFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                   std::string &dumpString) -> void {
        WindowHitchsDump(argSets, dumpString, cmd);
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::FPS_INFO, fpsInfoFunc },   { RSDumpID::FPS_CLEAR, fpsClearFunc },
        { RSDumpID::FPS_COUNT, fpsCountFunc }, { RSDumpID::CLEAR_FPS_COUNT, clearFpsCountFunc },
        { RSDumpID::HITCHS, hitchsFunc },
    };

    rsDumpManager->Register(handers);
}

void RSServiceDumper::RegisterGpuFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager)
{
    // gpu info
    RSDumpFunc gpuInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                    std::string &dumpString) -> void {
        ScheduleTask([this, &dumpString]() { DumpGpuInfo(dumpString); });
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::GPU_INFO, gpuInfoFunc },
    };

    rsDumpManager->Register(handers);
}

void RSServiceDumper::FPSDumpProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter == argSets.end()) {
        return ;
    }
    argSets.erase(iter);
    if (argSets.empty()) {
        RS_LOGE("FPSDumpProcess layer name is not specified");
        return ;
    }
    RS_TRACE_NAME("RSServiceDumper::FPSDumpProcess");
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    std::string option("");
    std::string argStr("");
    RSSurfaceFpsManager::GetInstance().ProcessParam(argSets, option, argStr);
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL
        && RSSurfaceFpsManager::GetInstance().IsSurface(option, argStr)) {
        RSSurfaceFpsManager::GetInstance().DumpSurfaceNodeFps(dumpString, option, argStr);
    } else {
        DumpFps(dumpString, argStr);
    }
}

void RSServiceDumper::DumpRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- RefreshRateCounts: \n");
#ifdef RS_ENABLE_GPU
    rsRenderComposerManager_->RefreshRateCounts(dumpString);
#endif
}

void RSServiceDumper::DumpFps(std::string& dumpString, std::string& layerName) const
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        ScheduleTask([this, &dumpString, &layerName]() { rsRenderComposerManager_->FpsDump(dumpString, layerName); });
#endif
    } else {
        // ScheduleTask(
        //     [this, &dumpString, &layerName]() { return screenManager_->FpsDump(dumpString, layerName); }).wait();
    }
}

void RSServiceDumper::FPSDumpClearProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter == argSets.end()) {
        return ;
    }
    argSets.erase(iter);
    if (argSets.empty()) {
        RS_LOGE("FPSDumpClearProcess layer name is not specified");
        return ;
    }
    RS_TRACE_NAME("RSServiceDumper::FPSDumpClearProcess");
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    std::string option("");
    std::string argStr("");
    RSSurfaceFpsManager::GetInstance().ProcessParam(argSets, option, argStr);
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL
        && RSSurfaceFpsManager::GetInstance().IsSurface(option, argStr)) {
        RSSurfaceFpsManager::GetInstance().ClearSurfaceNodeFps(dumpString, option, argStr);
    } else {
        ClearFps(dumpString, argStr);
    }
}

void RSServiceDumper::ClearFps(std::string& dumpString, std::string& layerName) const
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        ScheduleTask([this, &dumpString, &layerName]() { rsRenderComposerManager_->ClearFpsDump(dumpString, layerName); });
#endif
    } else {
        // RSMainThread::Instance()->ScheduleTask(
        //     [this, &dumpString, &layerName]() {
        //         return screenManager_->ClearFpsDump(dumpString, layerName);
        //     }).wait();
    }
}

void RSServiceDumper::DumpAllNodesMemSize(std::string& dumpString) const
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

#ifdef RS_ENABLE_GPU
void RSServiceDumper::DumpGpuInfo(std::string& dumpString) const
{
    InitGLES(); // This is necessary because you cannot query GPU information without initialization.
    dumpString.append("\n");
    dumpString.append("-- DumpGpuInfo: \n");
    auto glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    auto glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto glShadingLanguageVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    dumpString.append("GL_VENDOR: ");
    dumpString.append(glVendor ? glVendor : "Unknown");
    dumpString.append("\n");
    dumpString.append("GL_RENDERER: ");
    dumpString.append(glRenderer ? glRenderer : "Unknown");
    dumpString.append("\n");
    dumpString.append("GL_VERSION: ");
    dumpString.append(glesVersion ? glesVersion : "Unknown");
    dumpString.append("\n");
    dumpString.append("GL_SHADING_LANGUAGE_VERSION: ");
    dumpString.append(glShadingLanguageVersion ? glShadingLanguageVersion : "Unknown");
    dumpString.append("\n");
    DestroyGLES();
}
#endif

void RSServiceDumper::DumpClearRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- ClearRefreshRateCounts: \n");
#ifdef RS_ENABLE_GPU
    rsRenderComposerManager_->ClearRefreshRateCounts(dumpString);
#endif
}

void RSServiceDumper::WindowHitchsDump(
    std::unordered_set<std::u16string>& argSets, std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter != argSets.end()) {
        std::string layerArg;
        argSets.erase(iter);
        if (!argSets.empty()) {
            layerArg = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
        }
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
            ScheduleTask([this, &dumpString, &layerArg]() { return rsRenderComposerManager_->HitchsDump(dumpString, layerArg); });
#endif
        } else {
            // RSMainThread::Instance()->ScheduleTask(
            //     [this, &dumpString, &layerArg]() { return screenManager_->HitchsDump(dumpString, layerArg); }).wait();
        }
    }
}

void RSServiceDumper::ScheduleTask(std::function<void()> task) const
{
    if (mainHandler_) {
        mainHandler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return;
    }
    RS_LOGE("RSServiceDumper::ScheduleTask mainHandler_ is null");
}
} // Rosen
} // OHOS