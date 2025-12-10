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

#include "rs_render_pipeline.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <parameters.h>
#include <string>
#include <unistd.h>

#include "common/rs_singleton.h"
#include "common/rs_background_thread.h"
#include "command/rs_surface_node_command.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#ifdef RS_ENABLE_GPU
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/capture/rs_ui_capture.h"
#endif
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif

#include "ge_mesa_blur_shader_filter.h"
#include "graphic_feature_param_manager.h"
#include "main/render_process/dfx/rs_process_dump_manager.h"
#include "main/render_process/dfx/rs_process_dumper.h"
#include "parameter.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pixel_map_from_surface.h"
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_property.h"
#include "system/rs_system_parameters.h"
#include "text/font_mgr.h"
#include "pipeline/hwc/rs_hwc_context.h"
#include "command/rs_command_verify_helper.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "render/rs_typeface_cache.h"
#include "monitor/self_drawing_node_monitor.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderPipeline"

namespace OHOS{
namespace Rosen {
namespace {
constexpr int SLEEP_TIME_US = 1000;
const std::string REGISTER_NODE = "RegisterNode";
constexpr uint32_t MEM_BYTE_TO_MB = 1024 * 1024;
constexpr uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;
constexpr uint32_t PIDLIST_SIZE_MAX = 128;
}

std::shared_ptr<RSRenderPipeline> RSRenderPipeline::Create(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver)
{
    std::shared_ptr<RSRenderPipeline> render = std::make_shared<RSRenderPipeline>();
    render->Init(handler, receiver);
    return render;
}

void RSRenderPipeline::Init(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver)
{
    InitEnvironment();

    InitUniRenderConfig();

    // init CCM config
    InitCCMConfig();

    InitUniRenderThread();

    // todo
    RegisterRcdMsg();

    InitMainThread(handler, receiver);

    // Gfx init
    InitDumper();

    // todo
    // RS_PROFILER_INIT(this);

    RS_LOGI("render pipeline int success.");
}

void RSRenderPipeline::OnScreenConnected(const sptr<RSScreenProperty>& rsScreenProperty,
    const std::shared_ptr<RSRenderComposerClient>& composerClient, const std::shared_ptr<HgmClient>& hgmClient)
{
    RS_LOGI("RSRenderPipeline %{public}s, screen id: %{public}" PRIu64, __func__,
        rsScreenProperty ? rsScreenProperty->GetScreenId() : INVALID_SCREEN_ID);
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenConnected(rsScreenProperty, hgmClient);

    if (rsScreenProperty->IsVirtual()) {
        RS_LOGI("dmulti_process RSRenderPipeline %{public}s, virtual screen connected.", __func__);
        return;
    }
    if (!uniRenderThread_) {
        RS_LOGE("%{public}s uniRenderThread_ is nullptr, return", __func__);
        return;
    }
    uniRenderThread_->OnScreenConnected(rsScreenProperty->GetScreenId(), composerClient);
}

void RSRenderPipeline::OnScreenDisconnected(ScreenId screenId)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenDisconnected(screenId);
    if (!uniRenderThread_) {
        RS_LOGE("%{public}s uniRenderThread_ is nullptr, return", __func__);
        return;
    }
    uniRenderThread_->OnScreenDisconnected(screenId);
}

void RSRenderPipeline::OnScreenPropertyChanged(const sptr<RSScreenProperty>& rsScreenProperty)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenPropertyChanged(rsScreenProperty);
    RS_LOGD("RSRenderPipeline %{public}s, screen id: %{public}" PRIu64, __func__,
        rsScreenProperty ? rsScreenProperty->GetScreenId() : INVALID_SCREEN_ID);
}

void RSRenderPipeline::OnScreenRefresh(ScreenId screenId)
{
    if (mainThread_) {
        mainThread_->PostTask([mainThread = mainThread_]() {
            mainThread->SetForceUpdateUniRenderFlag(true);
            mainThread->RequestNextVSync();
        });
    }
}

void RSRenderPipeline::OnScreenBacklightChanged(ScreenId screenId, uint32_t level)
{
    RSLuminanceControl::Get().SetSdrLuminance(screenId, level);
    if (RSLuminanceControl::Get().IsHdrOn(screenId) && level > 0 && mainThread_ != nullptr) {
        mainThread_->PostTask([mainThread = mainThread_, screenId]() {
            mainThread->SetForceUpdateUniRenderFlag(true);
            mainThread->SetLuminanceChangingStatus(screenId, true);
            mainThread->SetDirtyFlag();
            mainThread->RequestNextVSync();
        });
        return;
    }
    if (uniRenderThread_ != nullptr) {
        auto client = uniRenderThread_->GetRSRenderComposerClient(screenId);
        if (client != nullptr) {
            client->SetScreenBacklight(level);
        }
    }
}

void RSRenderPipeline::InitEnvironment()
{
    std::thread preLoadSysTTFThread([]() {
        Drawing::FontMgr::CreateDefaultFontMgr();
    });
    preLoadSysTTFThread.detach();

#ifdef RS_ENABLE_VK
if (Drawing::SystemProperties::IsUseVulkan()) {
    RsVulkanContext::SetRecyclable(false);
}
#endif
}

void RSRenderPipeline::InitUniRenderConfig()
{
    RSUniRenderJudgement::InitUniRenderConfig();
}

void RSRenderPipeline::InitCCMConfig()
{
    // feature param parse
    GraphicFeatureParamManager::GetInstance().Init();

    // need called after GraphicFeatureParamManager::GetInstance().Init();
    FilterCCMInit();

    // hwcContext_ = std::make_shared<RSHwcContext>();
    imageEnhanceManager_ = std::make_shared<ImageEnhanceManager>();
}

void RSRenderPipeline::FilterCCMInit()
{
    RSFilterCacheManager::isCCMFilterCacheEnable_ = FilterParam::IsFilterCacheEnable();
    RSFilterCacheManager::isCCMEffectMergeEnable_ = FilterParam::IsEffectMergeEnable();
    RSProperties::SetFilterCacheEnabledByCCM(RSFilterCacheManager::isCCMFilterCacheEnable_);
    RSProperties::SetBlurAdaptiveAdjustEnabledByCCM(FilterParam::IsBlurAdaptiveAdjust());
    RSKawaseBlurShaderFilter::SetMesablurAllEnabledByCCM(FilterParam::IsMesablurAllEnable());
    GEMESABlurShaderFilter::SetMesaModeByCCM(FilterParam::GetSimplifiedMesaMode());
}

void RSRenderPipeline::RegisterRcdMsg()
{
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RS_LOGD("RSRenderPipeline::RegisterRcdMsg");
        if (isRcdServiceRegister_) {
            auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
            auto& rcdHardManager = RSRcdRenderManager::GetInstance();
            auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
            msgBus.RegisterTopic<NodeId, uint32_t, uint32_t, uint32_t, uint32_t>(
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
            RS_LOGD("RSRenderPipeline::RegisterRcdMsg Registed rcd renderservice end.");
            return;
        }
        RS_LOGD("RSRenderPipeline::RegisterRcdMsg Registed rcd renderservice already.");
    }
}

void RSRenderPipeline::InitMainThread(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver)
{
    mainThread_ = RSMainThread::Instance();
    mainThread_->Init(handler, receiver);
}

void RSRenderPipeline::InitUniRenderThread()
{
    uniRenderThread_ = &(RSUniRenderThread::Instance());
    uniRenderThread_->Start();


    uniBufferThread_ = &(RSBufferThread::Instance());
    uniBufferThread_->Start();
}

void RSRenderPipeline::InitDumper()
{
    auto rpDumper_ = std::make_shared<RSProcessDumper>();
    rpDumper_->RpDumpInit();
}

bool RSRenderPipeline::GetHighContrastTextState()
{
    return RSBaseRenderEngine::IsHighContrastEnabled();
}

ErrCode RSRenderPipeline::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [this, isCurtainScreenOn]() -> void {
        mainThread_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
    RSMainThread* mainThread = mainThread_;
#ifdef RS_ENABLE_GPU
    RSUniRenderThread* renderThread = uniRenderThread_;
    auto getBitmapTask = [id, &bitmap, mainThread, renderThread, &result]() {
        auto node = mainThread->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
        if (node == nullptr) {
            RS_LOGE("GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", id);
            result.set_value(false);
            return;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            result.set_value(false);
            return;
        }
        auto grContext = renderThread->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
        auto getDrawableBitmapTask = [drawableNode, &bitmap, grContext, &result]() {
            bitmap = std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)
                ->GetBitmap(grContext);
            result.set_value(!bitmap.IsEmpty());
        };
        renderThread->PostTask(getDrawableBitmapTask);
    };
    mainThread_->PostTask(getBitmapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipeline::SetDiscardJankFrames(bool discardJankFrames)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    mainThread_->SetDiscardJankFrames(discardJankFrames);
    RSJankStatsRenderFrameHelper::GetInstance().SetDiscardJankFrames(discardJankFrames);
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipeline::ReportJankStats()
{
    if (!uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = []() -> void { RSJankStats::GetInstance().ReportJankStats(); };
    uniRenderThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::ReportEventResponse(DataBaseRs info)
{
    if (!mainThread_ || !uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [this, info]() -> void {
        RSJankStats::GetInstance().SetReportEventResponse(info);
        mainThread_->SetForceRsDVsync(info.sceneId);
    };
    uniRenderThread_->PostTask(task);
    RSUifirstManager::Instance().OnProcessEventResponse(info);
    RSUifirstFrameRateControl::Instance().SetAnimationStartInfo(info);
    return ERR_OK;
}

ErrCode RSRenderPipeline::ReportEventComplete(DataBaseRs info)
{
    if (!uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportEventComplete(info);
    };
    uniRenderThread_->PostTask(task);
    RSUifirstManager::Instance().OnProcessEventComplete(info);
    return ERR_OK;
}

ErrCode RSRenderPipeline::ReportEventJankFrame(DataBaseRs info)
{
    if (!uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    bool isReportTaskDelayed = uniRenderThread_->IsMainLooping();
    auto task = [info, isReportTaskDelayed]() -> void {
        RSJankStats::GetInstance().SetReportEventJankFrame(info, isReportTaskDelayed);
    };
    uniRenderThread_->PostTask(task);
    RSUifirstFrameRateControl::Instance().SetAnimationEndInfo(info);
    return ERR_OK;
}

ErrCode RSRenderPipeline::ReportRsSceneJankStart(AppInfo info)
{
    if (!uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankStart(info);
    };
    uniRenderThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::ReportRsSceneJankEnd(AppInfo info)
{
    if (!uniRenderThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankEnd(info);
    };
    uniRenderThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [uniqueIdList, surfaceNameList, fps, reportTime]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [uniqueIdList, surfaceNameList, fps]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    sptr<Surface>& sfc, bool unobscured)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, mainThread_->GetContext(), unobscured);
    if (node == nullptr) {
        RS_LOGE("RSRenderPipeline::CreateNodeAndSurface CreateNode fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    if (surface == nullptr) {
        RS_LOGE("RSRenderPipeline::CreateNodeAndSurface get consumer surface fail");
        return ERR_INVALID_VALUE;
    }
    const std::string& surfaceName = surface->GetName();
    RS_LOGI("RsDebug RSRenderPipeline::CreateNodeAndSurface node"
        "id:%{public}" PRIu64 " name:%{public}s surface id:%{public}" PRIu64 " name:%{public}s",
        node->GetId(), node->GetName().c_str(),
        surface->GetUniqueId(), surfaceName.c_str());
    auto defaultUsage = surface->GetDefaultUsage();
    auto nodeId = node->GetId();
    bool isUseSelfDrawBufferUsage = RSSystemProperties::GetSelfDrawingDirtyRegionEnabled() &&
        RSGpuDirtyCollector::GetInstance().IsGpuDirtyEnable(nodeId) &&
        config.nodeType == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isUseSelfDrawBufferUsage) {
        defaultUsage |= BUFFER_USAGE_GPU_RENDER_DIRTY;
    }
    surface->SetDefaultUsage(defaultUsage | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER);
    node->GetRSSurfaceHandler()->SetConsumer(surface);
    RSMainThread* mainThread = mainThread_;
    std::function<void()> registerNode = [node, mainThread]() -> void {
        if (auto preNode = mainThread->GetContext().GetNodeMap().GetRenderNode(node->GetId())) {
            if (auto preSurfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                RS_LOGE("CreateNodeAndSurface same id node:%{public}" PRIu64 ", name:%{public}s, type:%{public}d",
                    node->GetId(), preSurfaceNode->GetName().c_str(), preNode->GetType());
            } else {
                RS_LOGE("CreateNodeAndSurface same id node:%{public}" PRIu64 ", type:%{public}d", node->GetId(),
                    preNode->GetType());
            }
            usleep(SLEEP_TIME_US);
        }
        RS_LOGI("CreateNodeAndSurface RegisterRenderNode id:%{public}" PRIu64 ", name:%{public}s", node->GetId(),
            node->GetName().c_str());
        mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    if (config.isSync) {
        mainThread_->PostSyncTask(registerNode);
    } else {
        mainThread_->PostTask(registerNode, REGISTER_NODE, 0, AppExecFwk::EventQueue::Priority::VIP);
    }
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode, uniRenderThread_);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    sfc = Surface::CreateSurfaceAsProducer(producer);
    return ERR_OK;
}

void RSRenderPipeline::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    if (mainThread_ != nullptr) {
        mainThread_->RegisterOcclusionChangeCallback(pid, callback);
    }
}

void RSRenderPipeline::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (mainThread_ != nullptr) {
        mainThread_->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    }
}

void RSRenderPipeline::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (mainThread_ != nullptr) {
        mainThread_->UnRegisterSurfaceOcclusionChangeCallback(id);
    }
}

ErrCode RSRenderPipeline::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (!mainThread_ || !mainThread_->GetContext().GetNodeMap().ContainPid(pid)) {
        return ERR_INVALID_VALUE;
    }
    if (uniRenderThread_ == nullptr || uniRenderThread_->GetRenderEngine() == nullptr ||
        uniRenderThread_->GetRenderEngine()->GetRenderContext() == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto context = uniRenderThread_->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
    uniRenderThread_->PostSyncTask(
        [&context, &memoryGraphic, &pid] { memoryGraphic = MemoryManager::CountPidMemory(pid, context); });
    return ERR_OK;
}

void RSRenderPipeline::DoDump(std::unordered_set<std::u16string> &argSets)
{
    std::string dumpString;
    RSProcessDumpManager::GetInstance().CmdExec(argSets, dumpString);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSRenderPipeline::SetOverlayDisplayMode(int32_t mode)
{
    RS_LOGI("RSRenderPipeline::SetOverlayDisplayMode: mode: [%{public}d]", mode);
    return RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(mode) == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif

void RSRenderPipeline::HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    RSUniHwcPrevalidateUtil::GetInstance().HandleHwcEvent(deviceId, eventId, eventData);
}

ErrCode RSRenderPipeline::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    OHOS::Media::Rect rect = {
        .left = srcRect.x,
        .top = srcRect.y,
        .width = srcRect.w,
        .height = srcRect.h,
    };
    RSBackgroundThread::Instance().PostSyncTask([surface, rect, &pixelMap]() {
        pixelMap = Rosen::CreatePixelMapFromSurface(surface, rect);
    });
    return ERR_OK;
}

void RSRenderPipeline::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    mainThread_.PostTask([this]() {
        this->mainThread_.CheckPackageInConfigList(packageList);
    });
    HandleDisplayPackageEvent(listSize, packageList);
}

void RSRenderPipeline::HandleDisplayPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    RS_LOGD("ImageEnhanceManager RSRenderPipeline::HandleDisplayPackageEvent start");
    imageEnhanceManager_->CheckPackageInConfigList(packageList);
}

ErrCode RSRenderPipeline::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [this, nodeIdStr, isTop]() -> void {
        if (this->mainThread_ == nullptr) {
            return;
        }
        auto& context = this->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isTop](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                surfaceNode->SetLayerTop(isTop);
                return;
            }
        });
        // It can be displayed immediately after layer-top changed.
        this->mainThread_->SetDirtyFlag();
        this->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
#ifdef RS_ENABLE_GPU
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    mainThread_->GetAppMemoryInMB(cpuMemSize, gpuMemSize);
    gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
#endif
    return ERR_OK;
}

ErrCode RSRenderPipeline::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    
    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
    std::vector<pid_t> pids;
    nodeMap.TraverseSurfaceNodes([&pids] (const std::shared_ptr<RSSurfaceRenderNode>& node) {
        auto pid = ExtractPid(node->GetId());
        if (std::find(pids.begin(), pids.end(), pid) == pids.end()) {
            pids.emplace_back(pid);
        }
    });

    bool enable = RSUniRenderJudgement::IsUniRender();
    if (uniRenderThread_->GetRenderEngine() == nullptr || uniRenderThread_->GetRenderEngine()->GetRenderContext() == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto context = uniRenderThread_->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
    if (enable) {
        uniRenderThread_->PostSyncTask(
            [&context, &memoryGraphics, &pids] {
                MemoryManager::CountMemory(pids, context, memoryGraphics);
            });
        return ERR_OK;
    } else {
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSRenderPipeline::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (mainThread_ == nullptr) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>> sfBufferInfoVector;
    auto selfDrawingNodeVector = mainThread_->GetContext().GetMutableNodeMap().GetSelfDrawingNodeInProcess(pid);
    std::function<void()> getSurfaceBufferByPidTask = [this, &selfDrawingNodeVector, &sfBufferInfoVector, pid]() -> void {
        RS_TRACE_NAME_FMT("RSRenderPipeline::GetPixelMapByProcessId getSurfaceBufferByPidTask pid: %d", pid);
        for (auto iter = selfDrawingNodeVector.rbegin(); iter != selfDrawingNodeVector.rend(); ++iter) {
            auto node = mainThread_->GetContext().GetNodeMap().GetRenderNode(*iter);
            if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                auto surfaceBuffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
                auto surfaceBufferInfo = std::make_tuple(surfaceBuffer, surfaceNode->GetName(),
                    surfaceNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect());
                sfBufferInfoVector.push_back(surfaceBufferInfo);
            }
        }
    };
    mainThread_->PostSyncTask(getSurfaceBufferByPidTask);

    for (uint32_t i = 0; i < sfBufferInfoVector.size(); i++) {
        auto surfaceBuffer = std::get<0>(sfBufferInfoVector[i]);
        const auto surfaceName = std::get<1>(sfBufferInfoVector[i]);
        const auto& absRect = std::get<2>(sfBufferInfoVector[i]);
        if (surfaceBuffer) {
            OHOS::Media::Rect rect = { 0, 0, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight() };
            std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
            RSBackgroundThread::Instance().PostSyncTask([&surfaceBuffer, rect, &pixelmap]() {
                pixelmap = Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, rect);
            });
            if (pixelmap) {
                pixelMapInfoVector.emplace_back(PixelMapInfo { pixelmap,
                    { absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight(), i },
                    surfaceName,
                    GetRotationInfoFromSurfaceBuffer(surfaceBuffer) });
            } else {
                RS_LOGE("dmulti_process RSRenderPipeline::CreatePixelMapFromSurfaceBuffer pixelmap is null, nodeName:%{public}s", surfaceName.c_str());
            }
        } else {
            RS_LOGE("dmulti_process RSRenderPipeline::CreatePixelMapFromSurface surfaceBuffer is null, nodeName:%{public}s, rect:%{public}s",
                surfaceName.c_str(), absRect.ToString().c_str());
        }
    }
    repCode = SUCCESS;
    return ERR_OK;
}

float RSRenderPipeline::GetRotationInfoFromSurfaceBuffer(const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        RS_LOGE("GetRotationInfoFromSurfaceBuffer buffer is null");
        return 0.0f;
    }
    auto transformType = buffer->GetSurfaceBufferTransform();
    if (transformType == GRAPHIC_ROTATE_90) {
        return 90.0f;
    } else if (transformType == GRAPHIC_ROTATE_180) {
        return 180.0f;
    } else if (transformType == GRAPHIC_ROTATE_270) {
        return 270.0f;
    }
    return 0.0f;
}


ErrCode RSRenderPipeline::SetWatermark(pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark,
    bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    mainThread_->SetWatermark(callingPid, name, watermark);
    success = true;
    return ERR_OK;
}

void RSRenderPipeline::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (!mainThread_) {
        return;
    }
    auto task = [this, watermarkImg, isShow]() -> void {
        mainThread_->ShowWatermark(watermarkImg, isShow);
    };
    mainThread_->PostTask(task);
}

ErrCode RSRenderPipeline::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [this, nodeIdStr, isForceRefresh]() -> void {
        if (this->mainThread_ == nullptr) {
            return;
        }
        auto& context = this->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isForceRefresh](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                surfaceNode->SetForceRefresh(isForceRefresh);
                return;
            }
        });
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

void RSRenderPipeline::GetSurfaceRootNodeId(NodeId& windowNodeId)
{
    if (!mainThread_) {
        RS_LOGE("mainThread is null");
        return;
    }
    mainThread_->ScheduleTask([&windowNodeId]() {
        RS_TRACE_NAME("TraverseSurfaceNodes");
        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
        nodeMap.TraverseSurfaceNodes(
            [&windowNodeId](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if (surfaceNode == nullptr || surfaceNode->GetRSSurfaceHandler() == nullptr ||
                    surfaceNode->GetRSSurfaceHandler()->GetConsumer() == nullptr) {
                    return;
                }
                if (surfaceNode->GetRSSurfaceHandler()->GetConsumer()->GetUniqueId() == windowNodeId) {
                    windowNodeId = surfaceNode->GetInstanceRootNodeId();
                    return;
                }
            }
        );
    }).wait();
}

void RSRenderPipeline::SetBehindWindowFilterEnabled(bool enabled)
{
    if (!mainThread_) {
        return;
    }
    auto task = [this, enabled] () {  
        if (RSSystemProperties::GetBehindWindowFilterEnabled() == enabled) {
            return;
        }  
        RSSystemProperties::SetBehindWindowFilterEnabled(enabled);
        auto& nodeMap = mainThread_->GetContext().GetNodeMap();
        bool needRequestNextVSync = false;
        nodeMap.TraverseSurfaceNodes(
            [&needRequestNextVSync](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if (!surfaceNode) {
                    return;
                }
                if (!surfaceNode->NeedUpdateDrawableBehindWindow()) {
                    return;
                }
                surfaceNode->SetContentDirty();
                needRequestNextVSync = true;
            });
        if (needRequestNextVSync) {
            mainThread_->RequestNextVSync();
            mainThread_->SetForceUpdateUniRenderFlag(true);
        }
    };
    mainThread_->PostTask(task);
}

void RSRenderPipeline::SetVmaCacheStatus(bool flag)
{
#ifdef RS_ENABLE_GPU
    uniRenderThread_->SetVmaCacheStatus(flag);
#endif
}

bool RSRenderPipeline::GetBehindWindowFilterEnabled()
{
    bool enabled = RSSystemProperties::GetBehindWindowFilterEnabled();
    return enabled;
}

int32_t RSRenderPipeline::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterUIExtensionCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    return StatusCode::SUCCESS;
}

bool RSRenderPipeline::RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface)
{
    RS_LOGI("RSRenderPipeline::RegisterTypeface, pid[%{public}d], familyname:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSRenderPipeline::UnRegisterTypeface(uint64_t globalUniqueId)
{
    RS_LOGW("RSRenderPipeline::UnRegisterTypeface: pid[%{public}d], uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);
    return true;
}

void RSRenderPipeline::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    if (mainThread_ == nullptr) {
        RS_LOGE("%{public}s: mainThread_ is nullptr", __func__);
        return;
    }
    mainThread_->SetForceUpdateUniRenderFlag(flag);
    mainThread_->RequestNextVSync("ltpoForceUpdate");
}

int32_t RSRenderPipeline::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    MemorySnapshotInfo memorySnapshotInfo;

    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, memorySnapshotInfo);
    if (!ret) {
        RS_LOGD("RSClientToRenderConnection::GetPidGpuMemoryInMB fail to find pid!");
        return ERR_INVALID_VALUE;
    }
    gpuMemInMB = static_cast<float>(memorySnapshotInfo.gpuMemory) / MEM_BYTE_TO_MB;
    RS_LOGD("RSClientToRenderConnection::GetPidGpuMemoryInMB called succ");
    return ERR_OK;
}

ErrCode RSRenderPipeline::RepaintEverything() const
{
    RS_LOGI("RepaintEverything start");
    if (mainThread_ == nullptr) {
        RS_LOGE("RepaintEverything mainThread_ is null, return");
        return ERR_INVALID_VALUE;
    }
    auto task = [this]() -> void {
        RS_LOGI("RepaintEverything, SetDirtyFlag ForceRefreshForUni");
        mainThread_->SetDirtyFlag();
        mainThread_->ForceRefreshForUni();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipeline::CleanResources(pid_t pid)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    RS_LOGD("CleanAll() start.");
    RS_TRACE_NAME("RSRenderPipeline::CleanResources begin, remotePid: " + std::to_string(pid));
    RsCommandVerifyHelper::GetInstance().RemoveCntWithPid(pid);
    mainThread_->ScheduleTask(
        [mainThread = mainThread_, pid]() {
            RS_TRACE_NAME_FMT("CleanRenderNodes %d", pid);
            if (mainThread == nullptr) {
                return;
            }
            auto& context = mainThread->GetContext();
            auto& nodeMap = context.GetMutableNodeMap();
            MemoryTrack::Instance().RemovePidRecord(pid);

            nodeMap.FilterNodeByPid(pid);
        }).wait();
    mainThread_->ScheduleTask(
        [mainThread = mainThread_, pid]() {
            RS_TRACE_NAME_FMT("ClearTransactionDataPidInfo %d", pid);
            mainThread->ClearTransactionDataPidInfo(pid);
        }).wait();
    mainThread_->ScheduleTask(
        [mainThread = mainThread_, pid]() {
            RS_TRACE_NAME_FMT("UnRegisterCallback %d", pid);
            if (mainThread == nullptr) {
                return;
            }
            mainThread->UnRegisterOcclusionChangeCallback(pid);
            mainThread->ClearSurfaceOcclusionChangeCallback(pid);
            mainThread->UnRegisterUIExtensionCallback(pid);
        }).wait();

    mainThread_->ScheduleTask(
        [mainThread = mainThread_, pid]() {
            if (mainThread == nullptr) {
                return;
            }
            mainThread->ClearSurfaceWatermark(pid);
        }).wait();
    if (SelfDrawingNodeMonitor::GetInstance().IsListeningEnabled()) {
        mainThread_->ScheduleTask(
            [mainThread = mainThread_, pid]() {
                if (mainThread == nullptr) {
                    return;
                }
                auto &monitor = SelfDrawingNodeMonitor::GetInstance();
                monitor.UnRegisterRectChangeCallback(pid);
            }).wait();
    }
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid);
    return ERR_OK;
}

ErrCode RSRenderPipeline::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [this, nodeIdStr, isColorFollow]() -> void {
        if (this->mainThread_ == nullptr) {
            return;
        }
        auto& context = this->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isColorFollow](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                    (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                    surfaceNode->SetColorFollow(isColorFollow);
                    return;
                }
            });
        this->mainThread_->SetDirtyFlag();
        this->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

void RSRenderPipeline::SetFreeMultiWindowStatus(bool enable)
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto task = [enable]() -> void {
        RSUifirstManager::Instance().SetFreeMultiWindowStatus(enable);
    };
    mainThread_->PostTask(task);
}

int32_t RSRenderPipeline::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSelfDrawingNodeRectChangeCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }

    auto task = [pid = remotePid, constraint, callback]() {
        SelfDrawingNodeMonitor::GetInstance().RegisterRectChangeCallback(pid, constraint, callback);
    };
    mainThread_->PostTask(task);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipeline::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [pid = remotePid]() { SelfDrawingNodeMonitor::GetInstance().UnRegisterRectChangeCallback(pid); };
    mainThread_->PostTask(task);
    return StatusCode::SUCCESS;
}

uint32_t RSRenderPipeline::GetRealtimeRefreshRate(ScreenId screenId)
{
    return RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRateByScreenId(screenId);
}

void RSRenderPipeline::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enabled, type);
}

ErrCode RSRenderPipeline::GetShowRefreshRateEnabled(bool& enable)
{
    enable = RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled();
    return ERR_OK;
}

ErrCode RSRenderPipeline::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [pidList]() -> void {
        RSGpuDirtyCollector::GetInstance().SetSelfDrawingGpuDirtyPidList(pidList);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

std::vector<ActiveDirtyRegionInfo> RSRenderPipeline::GetActiveDirtyRegionInfo()
{
    const auto& activeDirtyRegionInfos = GpuDirtyRegionCollection::GetInstance().GetActiveDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetActiveDirtyRegionInfo();
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSRenderPipeline::GetGlobalDirtyRegionInfo()
{
    const auto& globalDirtyRegionInfo = GpuDirtyRegionCollection::GetInstance().GetGlobalDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetGlobalDirtyRegionInfo();
    return globalDirtyRegionInfo;
}

LayerComposeInfo RSRenderPipeline::GetLayerComposeInfo()
{
    const auto& layerComposeInfo = LayerComposeCollection::GetInstance().GetLayerComposeInfo();
    LayerComposeCollection::GetInstance().ResetLayerComposeInfo();
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSRenderPipeline::GetHwcDisabledReasonInfo()
{
    const auto& hwcDisabledReasonInfos = HwcDisabledReasonCollection::GetInstance().GetHwcDisabledReasonInfo();
    return hwcDisabledReasonInfos;
}

ErrCode RSRenderPipeline::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    auto rsHdrCollection = RsHdrCollection::GetInstance();
    if (rsHdrCollection == nullptr) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = rsHdrCollection->GetHdrOnDuration();
    rsHdrCollection->ResetHdrOnDuration();
    return ERR_OK;
}

ErrCode RSRenderPipeline::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    if (pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    return ERR_OK;
}

void RSRenderPipeline::SetScreenFrameGravity(ScreenId id, Gravity gravity)
{
    if (!mainThread_) {
        RS_LOGE("RSRenderPipeline::%{public}s, mainThread_ is null.", __func__);
        return;
    }
    mainThread_->SetScreenFrameGravity(id, gravity);
}

void RSRenderPipeline::InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    if (!mainThread_) {
        RS_LOGE("RSRenderPipeline::%{public}s, mainThread_ is null.", __func__);
        return;
    }
    mainThread_->rsVsyncManagerAgent_ = rsVsyncManagerAgent;
}

void RSRenderPipeline::RegisterScreenSwitchFinishCallback(const sptr<RSIRenderToServiceConnection>& conn)
{
    if (!mainThread_) {
        RS_LOGE("RSRenderPipeline::%{public}s, mainThread_ is null.", __func__);
        return;
    }
    mainThread_->RegisterScreenSwitchFinishCallback(conn);
}
} // namespace Rosen
} // namespace OHOS