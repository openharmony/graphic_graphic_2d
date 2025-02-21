/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_render_service.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <parameters.h>
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#include <string>
#include <system_ability_definition.h>
#include <unistd.h>

#include "hgm_core.h"
#include "parameter.h"
#include "rs_main_thread.h"
#include "rs_profiler.h"
#include "rs_render_service_connection.h"
#include "vsync_generator.h"

#include "common/rs_singleton.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_message_bus.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "system/rs_system_parameters.h"

#include "text/font_mgr.h"

#ifdef TP_FEATURE_ENABLE
#include "touch_screen/touch_screen.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns
const std::string BOOTEVENT_RENDER_SERVICE_READY = "bootevent.renderservice.ready";
constexpr size_t CLIENT_DUMP_TREE_TIMEOUT = 2000; // 2000ms
static const int INT_INIT_VAL = 0;
static const int RGB_SIZE = 8;
static const int CREAT_NUM_ONE = 1;
static const int INIT_WIDTH = 480;
static const int INIT_HEIGHT = 800;
static const int INIT_EGL_VERSION = 3;
static EGLDisplay g_tmpDisplay = EGL_NO_DISPLAY;
static EGLContext g_tmpContext = EGL_NO_CONTEXT;
static EGLSurface g_tmpSurface = EGL_NO_SURFACE;

uint32_t GenerateTaskId()
{
    static std::atomic<uint32_t> id;
    return id.fetch_add(1, std::memory_order::memory_order_relaxed);
}
}
RSRenderService::RSRenderService() {}

RSRenderService::~RSRenderService() noexcept {}

bool RSRenderService::Init()
{
    system::SetParameter(BOOTEVENT_RENDER_SERVICE_READY.c_str(), "false");
    std::thread preLoadSysTTFThread([]() {
        Drawing::FontMgr::CreateDefaultFontMgr();
    });
    preLoadSysTTFThread.detach();

    if (RSSystemParameters::GetTcacheEnabled()) {
        // enable cache
        mallopt(M_OHOS_CONFIG, M_TCACHE_NORMAL_MODE);
        mallopt(M_OHOS_CONFIG, M_ENABLE_OPT_TCACHE);
        mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
        mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);
    }

    RSMainThread::Instance();
    RSUniRenderJudgement::InitUniRenderConfig();
#ifdef TP_FEATURE_ENABLE
    TOUCH_SCREEN->InitTouchScreen();
#endif
    screenManager_ = CreateOrGetScreenManager();
    if (RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        // screenManager initializtion executes in RSHHardwareThread under UNI_RENDER mode
        if (screenManager_ == nullptr || !screenManager_->Init()) {
            RS_LOGE("RSRenderService CreateOrGetScreenManager fail.");
            return false;
        }
    } else {
        RSUniRenderThread::Instance().Start();
        RSHardwareThread::Instance().Start();
        RegisterRcdMsg();
    }

    auto generator = CreateVSyncGenerator();

    // The offset needs to be set
    int64_t offset = 0;
    if (!HgmCore::Instance().GetLtpoEnabled()) {
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            offset = HgmCore::Instance().IsDelayMode() ? UNI_RENDER_VSYNC_OFFSET_DELAY_MODE : UNI_RENDER_VSYNC_OFFSET;
        }
        rsVSyncController_ = new VSyncController(generator, offset);
        appVSyncController_ = new VSyncController(generator, offset);
    } else {
        rsVSyncController_ = new VSyncController(generator, 0);
        appVSyncController_ = new VSyncController(generator, 0);
        generator->SetVSyncMode(VSYNC_MODE_LTPO);
    }
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs");
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app");

    generator->SetRSDistributor(rsVSyncDistributor_);
    generator->SetAppDistributor(appVSyncDistributor_);

    mainThread_ = RSMainThread::Instance();
    if (mainThread_ == nullptr) {
        return false;
    }
    mainThread_->rsVSyncDistributor_ = rsVSyncDistributor_;
    mainThread_->rsVSyncController_ = rsVSyncController_;
    mainThread_->appVSyncController_ = appVSyncController_;
    mainThread_->vsyncGenerator_ = generator;
    mainThread_->Init();
    mainThread_->SetAppVSyncDistributor(appVSyncDistributor_);
    mainThread_->PostTask([]() {
        system::SetParameter(BOOTEVENT_RENDER_SERVICE_READY.c_str(), "true");
        RS_LOGI("Set boot render service started true");
        }, "BOOTEVENT_RENDER_SERVICE_READY", 0, AppExecFwk::EventQueue::Priority::VIP);

    // Wait samgr ready for up to 5 second to ensure adding service to samgr.
    int status = WaitParameter("bootevent.samgr.ready", "true", 5);
    if (status != 0) {
        RS_LOGE("RSRenderService wait SAMGR error, return value [%d].", status);
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        RS_LOGE("RSRenderService GetSystemAbilityManager fail.");
        return false;
    }
    samgr->AddSystemAbility(RENDER_SERVICE, this);

    RS_PROFILER_INIT(this);

    return true;
}

void RSRenderService::Run()
{
    if (!mainThread_) {
        RS_LOGE("RSRenderService::Run failed, mainThread is nullptr");
        return;
    }
    RS_LOGE("RSRenderService::Run");
    mainThread_->Start();
}

void RSRenderService::RegisterRcdMsg()
{
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RS_LOGD("RSSubThreadManager::RegisterRcdMsg");
        if (!isRcdServiceRegister_) {
            auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
            auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
            msgBus.RegisterTopic<NodeId, uint32_t, uint32_t>(
                TOPIC_RCD_DISPLAY_SIZE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateDisplayParameter);
            msgBus.RegisterTopic<NodeId, ScreenRotation>(
                TOPIC_RCD_DISPLAY_ROTATION, &rcdInstance,
                &RoundCornerDisplayManager::UpdateOrientationStatus);
            msgBus.RegisterTopic<NodeId, int>(
                TOPIC_RCD_DISPLAY_NOTCH, &rcdInstance,
                &RoundCornerDisplayManager::UpdateNotchStatus);
            msgBus.RegisterTopic<NodeId, bool>(
                TOPIC_RCD_DISPLAY_HWRESOURCE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateHardwareResourcePrepared);
            isRcdServiceRegister_ = true;
            RS_LOGD("RSSubThreadManager::RegisterRcdMsg Registed rcd renderservice end");
            return;
        }
        RS_LOGD("RSSubThreadManager::RegisterRcdMsg Registed rcd renderservice already.");
    }
}

sptr<RSIRenderServiceConnection> RSRenderService::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    if (!mainThread_ || !token) {
        RS_LOGE("RSRenderService::CreateConnection failed, mainThread or token is nullptr");
        return nullptr;
    }
    pid_t remotePid = GetCallingPid();
    RS_PROFILER_ON_CREATE_CONNECTION(remotePid);

    auto tokenObj = token->AsObject();
    sptr<RSIRenderServiceConnection> newConn(
        new RSRenderServiceConnection(remotePid, this, mainThread_, screenManager_, tokenObj, appVSyncDistributor_));

    sptr<RSIRenderServiceConnection> tmp;
    std::unique_lock<std::mutex> lock(mutex_);
    // if connections_ has the same token one, replace it.
    auto it = connections_.find(tokenObj);
    if (it != connections_.end()) {
        tmp = it->second;
    }
    connections_[tokenObj] = newConn;
    lock.unlock();
    mainThread_->AddTransactionDataPidInfo(remotePid);
    return newConn;
}

void RSRenderService::RemoveConnection(sptr<IRemoteObject> token)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connections_.count(token) == 0) {
        return;
    }

    auto tmp = connections_.at(token);
    connections_.erase(token);
    lock.unlock();
}

int RSRenderService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::unordered_set<std::u16string> argSets;
    for (decltype(args.size()) index = 0; index < args.size(); ++index) {
        argSets.insert(args[index]);
    }
    if (screenManager_ == nullptr) {
        return OHOS::INVALID_OPERATION;
    }
    std::string dumpString;
    DoDump(argSets, dumpString);
    if (dumpString.size() == 0) {
        return OHOS::INVALID_OPERATION;
    }
    if (write(fd, dumpString.c_str(), dumpString.size()) < 0) {
        RS_LOGE("RSRenderService::DumpNodesNotOnTheTree write failed");
        return UNKNOWN_ERROR;
    }
    return OHOS::NO_ERROR;
}

void RSRenderService::DumpNodesNotOnTheTree(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- Node Not On Tree\n");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
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

void RSRenderService::DumpAllNodesMemSize(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- All Surfaces Memory Size\n");
    dumpString.append("the memory size of all surfaces buffer is : dumpend");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
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

void RSRenderService::DumpHelpInfo(std::string& dumpString) const
{
    dumpString.append("------Graphic2D--RenderSerice ------\n")
        .append("Usage:\n")
        .append(" h                             ")
        .append("|help text for the tool\n")
        .append("screen                         ")
        .append("|dump all screen infomation in the system\n")
        .append("surface                        ")
        .append("|dump all surface information\n")
        .append("composer fps                   ")
        .append("|dump the fps info of composer\n")
        .append("[surface name] fps             ")
        .append("|dump the fps info of surface\n")
        .append("composer fpsClear              ")
        .append("|clear the fps info of composer\n")
        .append("[windowname] fps               ")
        .append("|dump the fps info of window\n")
        .append("[windowname] hitchs            ")
        .append("|dump the hitchs info of window\n")
        .append("[surface name] fpsClear        ")
        .append("|clear the fps info of surface\n")
        .append("nodeNotOnTree                  ")
        .append("|dump nodeNotOnTree info\n")
        .append("allSurfacesMem                 ")
        .append("|dump surface mem info\n")
        .append("RSTree                         ")
        .append("|dump RSTree info\n")
        .append("EventParamList                 ")
        .append("|dump EventParamList info\n")
        .append("allInfo                        ")
        .append("|dump all info\n")
        .append("client                         ")
        .append("|dump client ui node trees\n")
        .append("dumpMem                        ")
        .append("|dump Cache\n")
        .append("trimMem cpu/gpu/shader         ")
        .append("|release Cache\n")
        .append("surfacenode [id]               ")
        .append("|dump node info\n")
        .append("fpsCount                       ")
        .append("|dump the refresh rate counts info\n")
        .append("clearFpsCount                  ")
        .append("|clear the refresh rate counts info\n")
#ifdef RS_ENABLE_VK
        .append("vktextureLimit                 ")
        .append("|dump vk texture limit info\n")
#endif
        .append("flushJankStatsRs")
        .append("|flush rs jank stats hisysevent\n")
        .append("gles                           ")
        .append("|inquire gpu info\n");
}

void RSRenderService::FPSDUMPProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
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
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->FpsDump(dumpString, layerArg); }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->FpsDump(dumpString, layerArg); }).wait();
        }
    }
}

void RSRenderService::FPSDUMPClearProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter != argSets.end()) {
        std::string layerArg;
        argSets.erase(iter);
        if (!argSets.empty()) {
            layerArg = std::wstring_convert<
            std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
        }
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() {
                    return screenManager_->ClearFpsDump(dumpString, layerArg);
                }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString, &layerArg]() {
                    return screenManager_->ClearFpsDump(dumpString, layerArg);
                }).wait();
        }
    }
}

void RSRenderService::DumpRSEvenParam(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- EventParamListDump: \n");
    mainThread_->RsEventParamDump(dumpString);
}

void RSRenderService::DumpRenderServiceTree(std::string& dumpString, bool forceDumpSingleFrame) const
{
    dumpString.append("\n");
    dumpString.append("-- RenderServiceTreeDump: \n");
    mainThread_->RenderServiceTreeDump(dumpString, forceDumpSingleFrame);
}

void RSRenderService::DumpRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- RefreshRateCounts: \n");
    RSHardwareThread::Instance().RefreshRateCounts(dumpString);
}

void RSRenderService::DumpClearRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- ClearRefreshRateCounts: \n");
    RSHardwareThread::Instance().ClearRefreshRateCounts(dumpString);
}

void RSRenderService::WindowHitchsDump(
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
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->HitchsDump(dumpString, layerArg); }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->HitchsDump(dumpString, layerArg); }).wait();
        }
    }
}

void RSRenderService::DumpSurfaceNode(std::string& dumpString, NodeId id) const
{
    dumpString.append("\n");
    dumpString.append("-- SurfaceNode\n");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
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

static bool IsNumber(const std::string& type)
{
    auto number = static_cast<uint32_t>(std::count_if(type.begin(), type.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == type.length();
}

void RSRenderService::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\nNot in UniRender and no information");
    } else {
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
        mainThread_->ScheduleTask(
            [this, &argSets, &dumpString, &type, &pid]() {
                return mainThread_->DumpMem(argSets, dumpString, type, pid);
            }).wait();
        return;
    }
}

void RSRenderService::DumpJankStatsRs(std::string& dumpString) const
{
    dumpString.append("\n");
    RSJankStats::GetInstance().ReportJankStats();
    dumpString.append("flush done\n");
}

static void InitGLES()
{
    g_tmpDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major;
    EGLint minor;
    eglInitialize(g_tmpDisplay, &major, &minor);
    EGLint numConfigs = INT_INIT_VAL;
    const EGLint configAttribs[] = {
        EGL_RED_SIZE, RGB_SIZE,
        EGL_GREEN_SIZE, RGB_SIZE,
        EGL_BLUE_SIZE, RGB_SIZE,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    EGLConfig config;
    eglChooseConfig(g_tmpDisplay, configAttribs, &config, CREAT_NUM_ONE, &numConfigs);
    const EGLint surfaceAttribs[] = { EGL_WIDTH, INIT_WIDTH, EGL_HEIGHT, INIT_HEIGHT, EGL_NONE };
    g_tmpSurface = eglCreatePbufferSurface(g_tmpDisplay, config, surfaceAttribs);
    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, INIT_EGL_VERSION, EGL_NONE };
    g_tmpContext = eglCreateContext(g_tmpDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(g_tmpDisplay, g_tmpSurface, g_tmpSurface, g_tmpContext);
}

static void DestroyGLES()
{
    eglDestroySurface(g_tmpDisplay, g_tmpSurface);
    g_tmpSurface = EGL_NO_SURFACE;
    eglDestroyContext(g_tmpDisplay, g_tmpContext);
    g_tmpContext = EGL_NO_CONTEXT;
    eglTerminate(g_tmpDisplay);
    g_tmpDisplay = EGL_NO_DISPLAY;
}

void RSRenderService::DumpGpuInfo(std::string& dumpString) const
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

#ifdef RS_ENABLE_VK
void RSRenderService::DumpVkTextureLimit(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- vktextureLimit:\n");
    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t maxTextureWidth = deviceProperties.limits.maxImageDimension2D;
    uint32_t maxTextureHeight = deviceProperties.limits.maxImageDimension2D;
    dumpString.append(
        "width: " + std::to_string(maxTextureWidth) + " height: " + std::to_string(maxTextureHeight) + "\n");
}
#endif

void RSRenderService::DoDump(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    if (!mainThread_ || !screenManager_) {
        RS_LOGE("RSRenderService::DoDump failed, mainThread or screenManager is nullptr");
        return;
    }
    std::u16string arg1(u"screen");
    std::u16string arg2(u"surface");
    std::u16string arg3(u"fps");
    std::u16string arg4(u"nodeNotOnTree");
    std::u16string arg5(u"allSurfacesMem");
    std::u16string arg6(u"RSTree");
    std::u16string arg6_1(u"MultiRSTrees");
    std::u16string arg7(u"EventParamList");
    std::u16string arg8(u"h");
    std::u16string arg9(u"allInfo");
    std::u16string arg10(u"trimMem");
    std::u16string arg11(u"dumpMem");
    std::u16string arg12(u"surfacenode");
    std::u16string arg13(u"fpsClear");
    std::u16string arg15(u"fpsCount");
    std::u16string arg16(u"clearFpsCount");
    std::u16string arg17(u"hitchs");
    std::u16string arg18(u"rsLogFlag");
    std::u16string arg19(u"flushJankStatsRs");
    std::u16string arg20(u"client");
#ifdef RS_ENABLE_VK
    std::u16string arg22(u"vktextureLimit");
#endif
    std::u16string arg23(u"gles");
    if (argSets.count(arg9) || argSets.count(arg1) != 0) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString]() { screenManager_->DisplayDump(dumpString); }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString]() { screenManager_->DisplayDump(dumpString); }).wait();
        }
    }
    if (argSets.count(arg9) || argSets.count(arg2) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { return screenManager_->SurfaceDump(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg4) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpNodesNotOnTheTree(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg5) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpAllNodesMemSize(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg6) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRenderServiceTree(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg6_1) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() {DumpRenderServiceTree(dumpString, false); }).wait();
    }
    if (argSets.count(arg9) ||argSets.count(arg7) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRSEvenParam(dumpString); }).wait();
    }
    if (argSets.count(arg10)) {
        mainThread_->ScheduleTask(
            [this, &argSets, &dumpString]() { return mainThread_->TrimMem(argSets, dumpString); }).wait();
    }
    if (argSets.count(arg11)) {
        DumpMem(argSets, dumpString);
    }
    if (auto iter = argSets.find(arg12) != argSets.end()) {
        argSets.erase(arg12);
        if (!argSets.empty()) {
            NodeId id = static_cast<NodeId>(std::atoll(std::wstring_convert<
                std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(*argSets.begin()).c_str()));
            mainThread_->ScheduleTask(
                [this, &dumpString, &id]() { return DumpSurfaceNode(dumpString, id); }).wait();
        }
    }
    FPSDUMPProcess(argSets, dumpString, arg3);
    FPSDUMPClearProcess(argSets, dumpString, arg13);
    WindowHitchsDump(argSets, dumpString, arg17);
    if (auto iter = argSets.find(arg18) != argSets.end()) {
        argSets.erase(arg18);
        if (!argSets.empty()) {
            std::string logFlag = std::wstring_convert<
                std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(*argSets.begin());
            if (RSLogManager::GetInstance().SetRSLogFlag(logFlag)) {
                dumpString.append("Successed to set flag: " + logFlag + "\n");
            } else {
                dumpString.append("Failed to set flag: " + logFlag + "\n");
            }
        }
    }
    if (argSets.size() == 0 || argSets.count(arg8) != 0 || dumpString.empty()) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpHelpInfo(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg15) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRefreshRateCounts(dumpString); }).wait();
    }
    if (argSets.count(arg16) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpClearRefreshRateCounts(dumpString); }).wait();
    }
    if (argSets.count(arg19) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpJankStatsRs(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg20)) {
        auto taskId = GenerateTaskId();
        mainThread_->ScheduleTask(
            [this, taskId]() {
                mainThread_->SendClientDumpNodeTreeCommands(taskId);
            }).wait();
        mainThread_->CollectClientNodeTreeResult(taskId, dumpString, CLIENT_DUMP_TREE_TIMEOUT);
    }
    if (argSets.count(arg23) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpGpuInfo(dumpString); }).wait();
    }
#ifdef RS_ENABLE_VK
    if (argSets.count(arg22) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpVkTextureLimit(dumpString); }).wait();
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
