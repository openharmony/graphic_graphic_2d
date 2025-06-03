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
#include "memory/rs_memory_manager.h"
#include "parameter.h"
#include "rs_main_thread.h"
#include "rs_profiler.h"
#include "rs_render_service_connection.h"
#include "vsync_generator.h"
#include "rs_trace.h"
#include "ge_mesa_blur_shader_filter.h"

#include "common/rs_singleton.h"
#ifdef RS_ENABLE_GPU
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#endif
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "system/rs_system_parameters.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/dump/rs_dump_manager.h"
#include "graphic_feature_param_manager.h"

#include "text/font_mgr.h"


namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns
const std::string BOOTEVENT_RENDER_SERVICE_READY = "bootevent.renderservice.ready";
constexpr size_t CLIENT_DUMP_TREE_TIMEOUT = 2000; // 2000ms
#ifdef RS_ENABLE_GPU
static const int INT_INIT_VAL = 0;
static const int CREAT_NUM_ONE = 1;
static const int INIT_EGL_VERSION = 3;
static EGLDisplay g_tmpDisplay = EGL_NO_DISPLAY;
static EGLContext g_tmpContext = EGL_NO_CONTEXT;
#endif

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
#ifdef RS_ENABLE_VK
if (Drawing::SystemProperties::IsUseVulkan()) {
    RsVulkanContext::SetRecyclable(false);
}
#endif
    RSMainThread::Instance();
    RSUniRenderJudgement::InitUniRenderConfig();

    // feature param parse
    GraphicFeatureParamManager::GetInstance().Init();

    // need called after GraphicFeatureParamManager::GetInstance().Init();
    FilterCCMInit();

    // load optimization params init
    LoadOptParams loadOptParams;
    InitLoadOptParams(loadOptParams);

    screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ != nullptr) {
        screenManager_->InitLoadOptParams(loadOptParams.loadOptParamsForScreen);
    }
    if (RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        // screenManager initializtion executes in RSHHardwareThread under UNI_RENDER mode
        if (screenManager_ == nullptr || !screenManager_->Init()) {
            RS_LOGE("RSRenderService CreateOrGetScreenManager fail.");
            return false;
        }
    } else {
#ifdef RS_ENABLE_GPU
        RSUniRenderThread::Instance().Start();
        RSHardwareThread::Instance().Start();
        RSSingleton<RoundCornerDisplayManager>::GetInstance().RegisterRcdMsg();
#endif
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
    DVSyncFeatureParam dvsyncParam;
    InitDVSyncParams(dvsyncParam);
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs", dvsyncParam);
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app", dvsyncParam);

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
    mainThread_->SetAppVSyncDistributor(appVSyncDistributor_);
    mainThread_->Init();
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
	
    RSGfxDumpInit(); // Gfx Init

    RS_PROFILER_INIT(this);

    return true;
}

void RSRenderService::InitDVSyncParams(DVSyncFeatureParam &dvsyncParam)
{
    std::vector<bool> switchParams = {};
    std::vector<uint32_t> bufferCountParams = {};
    std::unordered_map<std::string, std::string> adaptiveConfigs;
    switchParams = {
        DVSyncParam::IsDVSyncEnable(),
        DVSyncParam::IsUiDVSyncEnable(),
        DVSyncParam::IsNativeDVSyncEnable(),
        DVSyncParam::IsAdaptiveDVSyncEnable(),
    };
    bufferCountParams = {
        DVSyncParam::GetUiBufferCount(),
        DVSyncParam::GetRsBufferCount(),
        DVSyncParam::GetNativeBufferCount(),
        DVSyncParam::GetWebBufferCount(),
    };
    adaptiveConfigs = DVSyncParam::GetAdaptiveConfig();
    dvsyncParam = { switchParams, bufferCountParams, adaptiveConfigs };
}

void RSRenderService::InitLoadOptParams(LoadOptParams& loadOptParams)
{
    std::unordered_map<std::string, bool> tempSwitchParams = {};
    auto& paramsForScreen = loadOptParams.loadOptParamsForScreen;
    auto& paramsForHdiBackend = paramsForScreen.loadOptParamsForHdiBackend;
    auto& paramsForHdiOutput = paramsForHdiBackend.loadOptParamsForHdiOutput;

    tempSwitchParams[IS_MERGE_FENCE_SKIPPED] = LoadOptimizationParam::IsMergeFenceSkipped();
    paramsForHdiOutput.switchParams = { tempSwitchParams };
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
    // temporarily extending the life cycle
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = connections_.find(token);
    if (iter == connections_.end()) {
        RS_LOGE("RSRenderService::RemoveConnection: connections_ cannot find token");
        return;
    }
    auto tmp = iter->second;
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
        RS_LOGE("RSRenderService::DumpNodesNotOnTheTree write failed, string size: %{public}zu", dumpString.size());
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

void RSRenderService::FPSDumpProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter == argSets.end()) {
        return ;
    }
    argSets.erase(iter);
    if (argSets.empty()) {
        RS_LOGE("RSRenderService::FPSDumpProcess layer name is not specified");
        return ;
    }
    RS_TRACE_NAME("RSRenderService::FPSDumpProcess");
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

void RSRenderService::DumpFps(std::string& dumpString, std::string& layerName) const
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().ScheduleTask(
            [this, &dumpString, &layerName]() { return screenManager_->FpsDump(dumpString, layerName); }).wait();
#endif
    } else {
        mainThread_->ScheduleTask(
            [this, &dumpString, &layerName]() { return screenManager_->FpsDump(dumpString, layerName); }).wait();
    }
}

void RSRenderService::FPSDumpClearProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter == argSets.end()) {
        return ;
    }
    argSets.erase(iter);
    if (argSets.empty()) {
        RS_LOGE("RSRenderService::FPSDumpClearProcess layer name is not specified");
        return ;
    }
    RS_TRACE_NAME("RSRenderService::FPSDumpClearProcess");
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

void RSRenderService::ClearFps(std::string& dumpString, std::string& layerName) const
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().ScheduleTask(
            [this, &dumpString, &layerName]() {
                return screenManager_->ClearFpsDump(dumpString, layerName);
            }).wait();
#endif
    } else {
        mainThread_->ScheduleTask(
            [this, &dumpString, &layerName]() {
                return screenManager_->ClearFpsDump(dumpString, layerName);
            }).wait();
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
#ifdef RS_ENABLE_GPU
    mainThread_->RenderServiceTreeDump(dumpString, forceDumpSingleFrame, true);
#endif
}

void RSRenderService::DumpRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- RefreshRateCounts: \n");
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().RefreshRateCounts(dumpString);
#endif
}

void RSRenderService::DumpClearRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- ClearRefreshRateCounts: \n");
#ifdef RS_ENABLE_GPU
    RSHardwareThread::Instance().ClearRefreshRateCounts(dumpString);
#endif
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
#ifdef RS_ENABLE_GPU
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->HitchsDump(dumpString, layerArg); }).wait();
#endif
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

void RSRenderService::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    std::string type;
    bool isSuccess = ExtractDumpInfo(argSets, type, u"dumpMem");
    if (!isSuccess) {
        return;
    }
    int pid = 0;
    if (!type.empty() && IsNumber(type) && type.length() < 10) {
        pid = std::atoi(type.c_str());
    }
    mainThread_->ScheduleTask(
        [this, &argSets, &dumpString, &type, &pid]() {
            return mainThread_->DumpMem(argSets, dumpString, type, pid);
        }).wait();
}

void RSRenderService::DumpNode(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    std::string type;
    bool isSuccess = ExtractDumpInfo(argSets, type, u"dumpNode");
    if (!isSuccess) {
        return;
    }
    uint64_t nodeId = 0;
    if (!type.empty() && IsNumber(type) && type.length() < 20) {
        nodeId = std::stoull(type);
    }
    mainThread_->ScheduleTask(
        [this, &dumpString, &nodeId]() {
            return mainThread_->DumpNode(dumpString, nodeId);
        }).wait();
    
}

void RSRenderService::DumpJankStatsRs(std::string& dumpString) const
{
    dumpString.append("\n");
    RSJankStats::GetInstance().ReportJankStats();
    dumpString.append("flush done\n");
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
#endif

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

void RSRenderService::DumpExistPidMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
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

void RSRenderService::DoDump(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    if (!mainThread_ || !screenManager_) {
        RS_LOGE("RSRenderService::DoDump failed, mainThread, screenManager is nullptr");
        return;
    }

    std::string cmdStr;
    for (const std::u16string &cmd : argSets) {
        cmdStr += std::string(cmd.begin(), cmd.end()) + " ";
    }
    RS_TRACE_NAME("RSRenderService::DoDump args is [ " + cmdStr + " ]");

    RSDumpManager::GetInstance().CmdExec(argSets, dumpString);
}

void RSRenderService::RSGfxDumpInit()
{
    RegisterRSGfxFuncs();
    RegisterRSTreeFuncs();
    RegisterMemFuncs();
    RegisterFpsFuncs();
    RegisterGpuFuncs();
    RegisterBufferFuncs();
}

void RSRenderService::RegisterRSGfxFuncs()
{
    // screen info
    RSDumpFunc screenInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                              std::string &dumpString) -> void {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
            RSHardwareThread::Instance()
                .ScheduleTask([this, &dumpString]() { screenManager_->DisplayDump(dumpString); })
                .wait();
#endif
        } else {
            mainThread_->ScheduleTask([this, &dumpString]() { screenManager_->DisplayDump(dumpString); }).wait();
        }
    };

    // Event Param List
    RSDumpFunc rsEventParamFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpRSEvenParam(dumpString); }).wait();
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
        mainThread_->ScheduleTask([this, &dumpString]() { DumpJankStatsRs(dumpString); }).wait();
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::SCREEN_INFO, screenInfoFunc },
        { RSDumpID::EVENT_PARAM_LIST, rsEventParamFunc },
        { RSDumpID::RS_LOG_FLAG, rsLogFlagFunc },
        { RSDumpID::RS_FLUSH_JANK_STATS, flushJankStatsRsFunc },
    };
    RSDumpManager::GetInstance().Register(handers);
}

void RSRenderService::RegisterRSTreeFuncs()
{
    // RS not on Tree
    RSDumpFunc rsNotOnTreeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                               std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpNodesNotOnTheTree(dumpString); }).wait();
    };

    // RS Tree
    RSDumpFunc rsTreeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                          std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpRenderServiceTree(dumpString); }).wait();
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
            mainThread_->ScheduleTask([this, &dumpString, &id]() { return DumpSurfaceNode(dumpString, id); }).wait();
        }
    };

    // Multi RS Trees
    RSDumpFunc multiRSTreesFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpRenderServiceTree(dumpString, false); }).wait();
    };

    // client tree
    RSDumpFunc rsClientFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                            std::string &dumpString) -> void {
        auto taskId = GenerateTaskId();
        mainThread_->ScheduleTask([this, taskId]() { mainThread_->SendClientDumpNodeTreeCommands(taskId); }).wait();
        mainThread_->CollectClientNodeTreeResult(taskId, dumpString, CLIENT_DUMP_TREE_TIMEOUT);
    };

    // Dump Node
    RSDumpFunc dumpNodeFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                std::string &dumpString) -> void {
        DumpNode(argSets, dumpString);
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::RS_NOT_ON_TREE_INFO, rsNotOnTreeFunc },
        { RSDumpID::RENDER_NODE_INFO, rsTreeFunc, RS_MAIN_THREAD_TAG },
        { RSDumpID::SURFACENODE_INFO, surfaceNodeFunc },
        { RSDumpID::MULTI_RSTREES_INFO, multiRSTreesFunc },
        { RSDumpID::CLIENT_INFO, rsClientFunc, RS_CLIENT_TAG },
        { RSDumpID::RS_RENDER_NODE_INFO, dumpNodeFunc },
    };

    RSDumpManager::GetInstance().Register(handers);
}

void RSRenderService::RegisterMemFuncs()
{
    // surface info
    RSDumpFunc surfaceInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                               std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { return screenManager_->SurfaceDump(dumpString); }).wait();
    };

    // surface mem
    RSDumpFunc surfaceMemFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                              std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpAllNodesMemSize(dumpString); }).wait();
    };

    // trim mem
    RSDumpFunc trimMemFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                           std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &argSets, &dumpString]() { return mainThread_->TrimMem(argSets, dumpString); })
            .wait();
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
        { RSDumpID::SURFACE_INFO, surfaceInfoFunc, RS_HW_THREAD_TAG },
        { RSDumpID::SURFACE_MEM_INFO, surfaceMemFunc },
        { RSDumpID::TRIM_MEM_INFO, trimMemFunc },
        { RSDumpID::MEM_INFO, memDumpFunc },
        { RSDumpID::EXIST_PID_MEM_INFO, existPidMemFunc },
    };

    RSDumpManager::GetInstance().Register(handers);
}

void RSRenderService::RegisterFpsFuncs()
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
        mainThread_->ScheduleTask([this, &dumpString]() { DumpRefreshRateCounts(dumpString); }).wait();
    };

    // clear fps Count
    RSDumpFunc clearFpsCountFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                 std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpClearRefreshRateCounts(dumpString); }).wait();
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

    RSDumpManager::GetInstance().Register(handers);
}

void RSRenderService::RegisterGpuFuncs()
{
    // gpu info
    RSDumpFunc gpuInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                           std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpGpuInfo(dumpString); }).wait();
    };

#ifdef RS_ENABLE_VK
    // vk texture Limit
    RSDumpFunc vktextureLimitFunc = [this](const std::u16string &cmd,
                                                  std::unordered_set<std::u16string> &argSets,
                                                  std::string &dumpString) -> void {
        mainThread_->ScheduleTask([this, &dumpString]() { DumpVkTextureLimit(dumpString); }).wait();
    };
#endif

    std::vector<RSDumpHander> handers = {
        { RSDumpID::GPU_INFO, gpuInfoFunc },
#ifdef RS_ENABLE_VK
        { RSDumpID::VK_TEXTURE_LIMIT, vktextureLimitFunc },
#endif
    };

    RSDumpManager::GetInstance().Register(handers);
}

void RSRenderService::RegisterBufferFuncs()
{
    RSDumpFunc currentFrameBufferFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                               std::string &dumpString) -> void {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
            RSHardwareThread::Instance().ScheduleTask(
                [this]() {
                RS_TRACE_NAME("RSRenderService dump current frame buffer in HardwareThread");
                RS_LOGD("RSRenderService dump current frame buffer in HardwareThread");
                return screenManager_->DumpCurrentFrameLayers();
                }).wait();
#endif
        } else {
            mainThread_->ScheduleTask(
                [this]() {
                RS_TRACE_NAME("RSRenderService dump current frame buffer in MainThread");
                RS_LOGD("RSRenderService dump current frame buffer in MainThread");
                return screenManager_->DumpCurrentFrameLayers();
                }).wait();
        }
    };

    std::vector<RSDumpHander> handers = {
        { RSDumpID::CURRENT_FRAME_BUFFER, currentFrameBufferFunc },
    };

    RSDumpManager::GetInstance().Register(handers);
}

// need called after GraphicFeatureParamManager::GetInstance().Init();
void RSRenderService::FilterCCMInit()
{
    RSFilterCacheManager::isCCMFilterCacheEnable_ = FilterParam::IsFilterCacheEnable();
    RSFilterCacheManager::isCCMEffectMergeEnable_ = FilterParam::IsEffectMergeEnable();
    RSProperties::SetFilterCacheEnabledByCCM(RSFilterCacheManager::isCCMFilterCacheEnable_);
    RSProperties::SetBlurAdaptiveAdjustEnabledByCCM(FilterParam::IsBlurAdaptiveAdjust());
    RSKawaseBlurShaderFilter::SetMesablurAllEnabledByCCM(FilterParam::IsMesablurAllEnable());
    GEMESABlurShaderFilter::SetMesaModeByCCM(FilterParam::GetSimplifiedMesaMode());
}
} // namespace Rosen
} // namespace OHOS
