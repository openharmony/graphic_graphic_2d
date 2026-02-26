/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_profiler.h"
#include "rs_client_to_service_connection.h"

#include "frame_report.h"
#include "hgm_command.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
//blur predict
#include "rs_frame_blur_predict.h"
#include "rs_frame_report.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

#include "command/rs_command_verify_helper.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_background_thread.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "display_engine/rs_luminance_control.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/capture/rs_ui_capture.h"
#include "feature/capture/rs_uni_ui_capture.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "feature/capture/rs_ui_capture_solo_task_parallel.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "info_collection/rs_hdr_collection.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "memory/rs_canvas_dma_buffer_cache.h"
#endif
#include "memory/rs_memory_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#include "node_mem_release_param.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_render_node_gc.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pixel_map_from_surface.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats_helper.h"
#include "render/rs_typeface_cache.h"
#include "transaction/rs_unmarshal_thread.h"
#include "transaction/rs_transaction_data_callback_manager.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"

#ifdef TP_FEATURE_ENABLE
#include "screen_manager/touch_screen.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#undef LOG_TAG
#define LOG_TAG "RSClientToServiceConnection"

#include "app_mgr_client.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int SLEEP_TIME_US = 1000;
const std::string REGISTER_NODE = "RegisterNode";
const std::string APS_SET_VSYNC = "APS_SET_VSYNC";
constexpr uint32_t MEM_BYTE_TO_MB = 1024 * 1024;
constexpr uint32_t PIDLIST_SIZE_MAX = 128;
constexpr uint64_t MAX_TIME_OUT_NS = 1e9;
constexpr int64_t MAX_FREEZE_SCREEN_TIME = 3000;
const std::string UNFREEZE_SCREEN_TASK_NAME = "UNFREEZE_SCREEN_TASK";
}
// we guarantee that when constructing this object,
// all these pointers are valid, so will not check them.
RSClientToServiceConnection::RSClientToServiceConnection(
    pid_t remotePid,
    wptr<RSRenderService> renderService,
    RSMainThread* mainThread,
    sptr<RSScreenManager> screenManager,
    sptr<IRemoteObject> token,
    sptr<VSyncDistributor> distributor)
    : remotePid_(remotePid),
      renderService_(renderService),
      mainThread_(mainThread),
#ifdef RS_ENABLE_GPU
      renderThread_(RSUniRenderThread::Instance()),
#endif
      screenManager_(screenManager),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      applicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this)),
      appVSyncDistributor_(distributor)
{
    if (token_ == nullptr || !token_->AddDeathRecipient(connDeathRecipient_)) {
        RS_LOGW("RSClientToServiceConnection: Failed to set death recipient.");
    }
    if (renderService_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: renderService_ is nullptr");
    }
    if (mainThread_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: mainThread_ is nullptr");
    }
    if (screenManager_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: screenManager_ is nullptr");
    }
    if (appVSyncDistributor_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: appVSyncDistributor_ is nullptr");
    }
}

RSClientToServiceConnection::~RSClientToServiceConnection() noexcept
{
    if (token_ && connDeathRecipient_) {
        token_->RemoveDeathRecipient(connDeathRecipient_);
    }
    CleanAll();
}

void RSClientToServiceConnection::CleanVirtualScreens() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (screenManager_ != nullptr) {
        for (const auto id : virtualScreenIds_) {
            screenManager_->RemoveVirtualScreen(id);
        }
    }
    virtualScreenIds_.clear();

    if (screenChangeCallback_ != nullptr && screenManager_ != nullptr) {
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
        screenChangeCallback_ = nullptr;
    }
}

void RSClientToServiceConnection::CleanRenderNodes() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();
    MemoryTrack::Instance().RemovePidRecord(remotePid_);

    RS_PROFILER_KILL_PID(remotePid_);
    nodeMap.FilterNodeByPid(remotePid_);
    RS_PROFILER_KILL_PID_END();

    if (NodeMemReleaseParam::IsRsRenderNodeGCMemReleaseEnabled()) {
        RSRenderNodeGC::Instance().ReleaseFromTree(AppExecFwk::EventQueue::Priority::HIGH);
    }
}

void RSClientToServiceConnection::CleanFrameRateLinkers() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();

    frameRateLinkerMap.FilterFrameRateLinkerByPid(remotePid_);
}

void RSClientToServiceConnection::CleanAll(bool toDelete) noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (cleanDone_) {
            return;
        }
    }
    if (!mainThread_) {
        return;
    }
    RS_LOGD("CleanAll() start.");
    RS_TRACE_NAME("RSClientToServiceConnection CleanAll begin, remotePid: " + std::to_string(remotePid_));
    RsCommandVerifyHelper::GetInstance().RemoveCntWithPid(remotePid_);
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (!connection) {
                return;
            }
            RS_TRACE_NAME_FMT("CleanVirtualScreens %d", connection->remotePid_);
            connection->CleanVirtualScreens();
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (!connection) {
                return;
            }
            RS_TRACE_NAME_FMT("CleanRenderNodes %d", connection->remotePid_);
            connection->CleanRenderNodes();
            connection->CleanFrameRateLinkers();
            connection->CleanBrightnessInfoChangeCallbacks();
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
            connection->CleanCanvasCallbacksAndPendingBuffer();
#endif
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            RS_TRACE_NAME_FMT("ClearTransactionDataPidInfo %d", connection->remotePid_);
            connection->mainThread_->ClearTransactionDataPidInfo(connection->remotePid_);
            connection->mainThread_->RemoveDropFramePid(connection->remotePid_);
            if (connection->mainThread_->IsRequestedNextVSync()) {
                connection->mainThread_->SetDirtyFlag();
            }
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            RS_TRACE_NAME_FMT("UnRegisterCallback %d", connection->remotePid_);
            connection->mainThread_->UnRegisterOcclusionChangeCallback(connection->remotePid_);
            connection->mainThread_->ClearSurfaceOcclusionChangeCallback(connection->remotePid_);
            connection->mainThread_->UnRegisterUIExtensionCallback(connection->remotePid_);
        }).wait();

    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            connection->mainThread_->ClearSurfaceWatermark(connection->remotePid_);
        }).wait();
    if (SelfDrawingNodeMonitor::GetInstance().IsListeningEnabled()) {
        mainThread_->ScheduleTask(
            [weakThis = wptr<RSClientToServiceConnection>(this)]() {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr) {
                    return;
                }
                auto &monitor = SelfDrawingNodeMonitor::GetInstance();
                monitor.UnRegisterRectChangeCallback(connection->remotePid_);
            }).wait();
    }
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(remotePid_);
    HgmTaskHandleThread::Instance().ScheduleTask([pid = remotePid_] () {
        RS_TRACE_NAME_FMT("CleanHgmEvent %d", pid);
        HgmConfigCallbackManager::GetInstance()->UnRegisterHgmConfigChangeCallback(pid);
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->CleanVote(pid);
        }
    }).wait();
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(remotePid_);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cleanDone_ = true;
    }

    if (toDelete) {
        if (auto renderService = renderService_.promote()) {
            auto token = iface_cast<RSIConnectionToken>(GetToken());
            renderService->RemoveConnection(token);
        } else {
            RS_LOGW("CleanAll() RenderService is dead.");
        }
    }

    RS_LOGD("CleanAll() end.");
    RS_TRACE_NAME("RSClientToServiceConnection CleanAll end, remotePid: " + std::to_string(remotePid_));
    {
        std::lock_guard<std::mutex> lock(pidToBundleMutex_);
        pidToBundleName_.clear();
    }
}

RSClientToServiceConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSClientToServiceConnection> conn) : conn_(conn)
{
}

void RSClientToServiceConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: RSClientToServiceConnection was dead, do nothing.");
        return;
    }

    if (rsConn->GetToken() != tokenSptr) {
        RS_LOGI("RSConnectionDeathRecipient::OnRemoteDied: token doesn't match, ignore it.");
        return;
    }

    rsConn->CleanAll(true);
}

RSClientToServiceConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSClientToServiceConnection> conn) : conn_(conn)
{}

void RSClientToServiceConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: "
            "RSClientToServiceConnection was dead, do nothing.");
        return;
    }

    RS_LOGD("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: Unregister.");
    auto app = iface_cast<IApplicationAgent>(tokenSptr);
    rsConn->UnRegisterApplicationAgent(app);
}

ErrCode RSClientToServiceConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    pid_t callingPid = GetCallingPid();
    bool isTokenTypeValid = true;
    bool isNonSystemAppCalling = false;
    RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
    bool shouldDrop = RSUnmarshalThread::Instance().ReportTransactionDataStatistics(
        callingPid, transactionData.get(), isNonSystemAppCalling);
    if (shouldDrop) {
        RS_LOGW("CommitTransaction data droped");
        return ERR_INVALID_VALUE;
    }
    if (transactionData && transactionData->GetDVSyncUpdate()) {
        mainThread_->DVSyncUpdate(transactionData->GetDVSyncTime(), transactionData->GetTimestamp());
    }
    bool isProcessBySingleFrame = mainThread_->IsNeedProcessBySingleFrameComposer(transactionData);
    if (isProcessBySingleFrame) {
        mainThread_->ProcessDataBySingleFrameComposer(transactionData);
    } else {
        mainThread_->RecvRSTransactionData(transactionData);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr || mainThread_ == nullptr) {
        RS_LOGW("ExecuteSynchronousTask, task or main thread is null!");
        return ERR_INVALID_VALUE;
    }
    // After a synchronous task times out, it will no longer be executed.
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    std::chrono::nanoseconds span(std::min(task->GetTimeout(), MAX_TIME_OUT_NS));
    mainThread_->ScheduleTask([task, mainThread = mainThread_, isTimeoutWeak] {
        if (task == nullptr || mainThread == nullptr || isTimeoutWeak.expired()) {
            return;
        }
        task->Process(mainThread->GetContext());
    }).wait_for(span);
    isTimeout.reset();
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetUniRenderEnabled(bool& res)
{
    res = RSUniRenderJudgement::IsUniRender();
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    auto node = DisplayNodeCommandHelper::CreateWithConfigInRS(mainThread_->GetContext(), nodeId, displayNodeConfig);
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateDisplayNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [this, nodeId, node, &displayNodeConfig]() {
        RS_TRACE_NAME_FMT("RSClientToServiceConnection::CreateNode, nodeId[%" PRIu64 "], screenId[%" PRIu64 "]",
            nodeId, displayNodeConfig.screenId);
        if (mainThread_ == nullptr) {
            return;
        }
        RS_LOGI("On registerNode scheduled by mainThread. NodeID: %{public}" PRIu64 ", screenID: %{public}" PRIu64,
                nodeId, displayNodeConfig.screenId);

        auto& context = mainThread_->GetContext();
        auto& nodeMap = context.GetMutableNodeMap();
        nodeMap.RegisterRenderNode(node);

        DisplayNodeCommandHelper::AddDisplayNodeToTree(context, nodeId);
        DisplayNodeCommandHelper::SetDisplayMode(context, nodeId, displayNodeConfig);
    };
    // When the event runner has not started to rotate, synchronous tasks will not be executed,
    // so asynchronous tasks need to be dispatched at this time.
    if (!mainThread_->IsReadyForSyncTask() || !mainThread_->PostSyncTask(registerNode)) {
        RS_LOGW("Post async tasks instead. Sync task processor ready? %{public}d",
                static_cast<int>(mainThread_->IsReadyForSyncTask()));
        mainThread_->PostTask(registerNode);
    }
    success = true;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, mainThread_->GetContext());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [node, weakThis = wptr<RSClientToServiceConnection>(this)]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    success = true;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    sptr<Surface>& sfc, bool unobscured)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, mainThread_->GetContext(), unobscured);
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    if (surface == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return ERR_INVALID_VALUE;
    }
    const std::string& surfaceName = surface->GetName();
    RS_LOGI("CreateNodeAndSurface node id:%{public}" PRIu64 " name:%{public}s surface id:%{public}" PRIu64
        " name:%{public}s", node->GetId(), node->GetName().c_str(), surface->GetUniqueId(), surfaceName.c_str());
    auto defaultUsage = surface->GetDefaultUsage();
    auto nodeId = node->GetId();
    bool isUseSelfDrawBufferUsage = RSSystemProperties::GetSelfDrawingDirtyRegionEnabled() &&
        RSGpuDirtyCollector::GetInstance().IsGpuDirtyEnable(nodeId) &&
        config.nodeType == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isUseSelfDrawBufferUsage) {
        defaultUsage |= BUFFER_USAGE_AUXILLARY_BUFFER0;
    }
    surface->SetDefaultUsage(defaultUsage | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER);
    node->GetRSSurfaceHandler()->SetConsumer(surface);
    RSMainThread* mainThread = mainThread_;
    std::function<void()> registerNode = [node, mainThread]() -> void {
        if (auto preNode = mainThread->GetContext().GetNodeMap().GetRenderNode(node->GetId())) {
            if (auto preSurfaceNode = preNode->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                RS_LOGE("CreateNodeAndSurface same id node:%{public}" PRIu64 ", name:%{public}s, type:%{public}d",
                    node->GetId(), preSurfaceNode->GetName().c_str(), preNode->GetType());
            } else {
                RS_LOGE("CreateNodeAndSurface same id node:%{public}" PRIu64 ", type:%{public}d", node->GetId(),
                    preNode->GetType());
            }
            usleep(SLEEP_TIME_US); // When nodeid is same, sleep 1ms
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    sfc = Surface::CreateSurfaceAsProducer(producer);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn, const std::string& name,
    const sptr<VSyncIConnectionToken>& token, VSyncConnParam vsyncConnParam)
{
    if (mainThread_ == nullptr || appVSyncDistributor_ == nullptr) {
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    uint64_t id = vsyncConnParam.id;
    NodeId windowNodeId = vsyncConnParam.windowNodeId;
    if (vsyncConnParam.fromXcomponent) {
        mainThread_->ScheduleTask([&windowNodeId]() {
            auto& node = RSMainThread::Instance()->GetContext().GetNodeMap()
                        .GetRenderNode<RSRenderNode>(windowNodeId);
            if (node == nullptr) {
                RS_LOGE("CreateVSyncConnection:node is nullptr");
                return;
            }
            windowNodeId = node->GetInstanceRootNodeId();
        }).wait();
    }
    sptr<VSyncConnection> conn = new VSyncConnection(appVSyncDistributor_, name, token->AsObject(), 0, windowNodeId);
    if (ExtractPid(id) == remotePid_) {
        auto observer = [] (const RSRenderFrameRateLinker& linker) {
            if (auto mainThread = RSMainThread::Instance(); mainThread != nullptr) {
                HgmCore::Instance().SetHgmTaskFlag(true);
            }
        };
        mainThread_->ScheduleTask([weakThis = wptr<RSClientToServiceConnection>(this),
            id, observer, name, windowNodeId]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            auto linker = std::make_shared<RSRenderFrameRateLinker>(id, observer);
            linker->SetVsyncName(name);
            linker->SetWindowNodeId(windowNodeId);
            auto& context = connection->mainThread_->GetContext();
            auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();
            frameRateLinkerMap.RegisterFrameRateLinker(linker);
        }).wait();
        conn->id_ = id;
        RS_LOGD("CreateVSyncConnection connect id: %{public}" PRIu64, id);
    }
    auto ret = appVSyncDistributor_->AddConnection(conn, windowNodeId);
    if (ret != VSYNC_ERROR_OK) {
        UnregisterFrameRateLinker(conn->id_);
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    vsyncConn = conn;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (mainThread_ == nullptr) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>> sfBufferInfoVector;
    std::function<void()> getSurfaceBufferByPidTask = [weakThis = wptr<RSClientToServiceConnection>(this),
                                                          &sfBufferInfoVector, pid]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        RS_TRACE_NAME_FMT("RSClientToServiceConnection::GetPixelMapByProcessId getSurfaceBufferByPidTask pid: %d", pid);
        auto selfDrawingNodeVector =
            connection->mainThread_->GetContext().GetMutableNodeMap().GetSelfDrawingNodeInProcess(pid);
        for (auto iter = selfDrawingNodeVector.rbegin(); iter != selfDrawingNodeVector.rend(); ++iter) {
            auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(*iter);
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
        const auto& surfaceBuffer = std::get<0>(sfBufferInfoVector[i]);
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
                RS_LOGE("CreatePixelMapFromSurfaceBuffer pixelmap is null, nodeName:%{public}s", surfaceName.c_str());
            }
        } else {
            RS_LOGE("CreatePixelMapFromSurface surfaceBuffer is null, nodeName:%{public}s, rect:%{public}s",
                surfaceName.c_str(), absRect.ToString().c_str());
        }
    }
    repCode = SUCCESS;
    return ERR_OK;
}

float RSClientToServiceConnection::GetRotationInfoFromSurfaceBuffer(const sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        RS_LOGE("GetRotationInfoFromSurfaceBuffer buffer is null");
        return 0.0f;
    }
    auto transformType = buffer->GetSurfaceBufferTransform();
    if (transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
        return 90.0f;
    } else if (transformType == GraphicTransformType::GRAPHIC_ROTATE_180) {
        return 180.0f;
    } else if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        return 270.0f;
    }
    return 0.0f;
}

ErrCode RSClientToServiceConnection::CreatePixelMapFromSurface(sptr<Surface> surface,
    const Rect &srcRect, std::shared_ptr<Media::PixelMap> &pixelmap, bool transformEnabled)
{
    OHOS::Media::Rect rect = {
        .left = srcRect.x,
        .top = srcRect.y,
        .width = srcRect.w,
        .height = srcRect.h,
    };
    RS_LOGD("RSClientToServiceConnection::CreatePixelMapFromSurface: transformEnabled:%{public}d", transformEnabled);
    RSBackgroundThread::Instance().PostSyncTask([surface, rect, transformEnabled, &pixelmap]() {
        pixelmap = Rosen::CreatePixelMapFromSurface(surface, rect, transformEnabled);
    });
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetWatermark(
    const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    pid_t callingPid = GetCallingPid();
    std::function<void()> task = [weakThis = wptr<RSClientToServiceConnection>(this),
        callingPid, name, watermark]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->SetWatermark(callingPid, name, watermark);
    };
    mainThread_->PostTask(task);
    success = true;
    return ERR_OK;
}

uint32_t RSClientToServiceConnection::SetSurfaceWatermark(pid_t pid, const std::string& name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId>& nodeIdList, SurfaceWatermarkType watermarkType)
{
    if (!mainThread_) {
        return WATER_MARK_IPC_ERROR;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::SET_SURFACE_WATERMARK");
    uint32_t res = SurfaceWatermarkStatusCode::WATER_MARK_RS_CONNECTION_ERROR;
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), &name, &nodeIdList, &watermark, &watermarkType,
        pid, isSystemCalling, &res]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        res = connection->mainThread_->SetSurfaceWatermark(pid, name, watermark,
            nodeIdList, watermarkType, isSystemCalling);
    };
    mainThread_->PostSyncTask(task);
    return res;
}
    
void RSClientToServiceConnection::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId> &nodeIdList)
{
    if (!mainThread_) {
        return;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::CLEAR_SURFACE_WATERMARK_FOR_NODES");
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), &name, pid,
        &nodeIdList, isSystemCalling]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, isSystemCalling);
    };
    mainThread_->PostTask(task);
}
    
void RSClientToServiceConnection::ClearSurfaceWatermark(pid_t pid, const std::string& name)
{
    if (!mainThread_) {
        return;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::CLEAR_SURFACE_WATERMARK");
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), pid, &name, isSystemCalling]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->ClearSurfaceWatermark(pid, name, isSystemCalling);
    };
    mainThread_->PostTask(task);
}

ErrCode RSClientToServiceConnection::GetDefaultScreenId(uint64_t& screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_OPERATION;
    }
    screenId = screenManager_->GetDefaultScreenId();
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetActiveScreenId(uint64_t& screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_OPERATION;
    }
    
    screenId = screenManager_->GetActiveScreenId();
    return ERR_OK;
}

std::vector<ScreenId> RSClientToServiceConnection::GetAllScreenIds()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return {};
    }
    return screenManager_->GetAllScreenIds();
}

ScreenId RSClientToServiceConnection::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId associatedScreenId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    if (whiteList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: white list is over max size!", __func__);
        return INVALID_SCREEN_ID;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto newVirtualScreenId = screenManager_->CreateVirtualScreen(
        name, width, height, surface, associatedScreenId, flags, whiteList);
    virtualScreenIds_.insert(newVirtualScreenId);
    if (surface != nullptr) {
        EventInfo event = { "VOTER_VIRTUALDISPLAY", ADD_VOTE, OLED_60_HZ, OLED_60_HZ, name };
        NotifyRefreshRateEvent(event);
        ROSEN_LOGI("%{public}s vote 60hz", __func__);
    }
    return newVirtualScreenId;
}

int32_t RSClientToServiceConnection::SetVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    if (blackList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: black list is over max size!", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (blackList.empty()) {
        RS_LOGW("%{public}s: blackList is empty.", __func__);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManager_ == nullptr) {
        RS_LOGW("%{public}s: screenManager is nullptr.", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenBlackList(id, blackList);
}

ErrCode RSClientToServiceConnection::SetVirtualScreenTypeBlackList(
    ScreenId id, std::vector<NodeType>& typeBlackListVector, int32_t& repCode)
{
    if (typeBlackListVector.empty()) {
        RS_LOGW("SetVirtualScreenTypeBlackList typeBlackList is empty.");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManager_->SetVirtualScreenTypeBlackList(id, typeBlackListVector);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AddVirtualScreenBlackList(
    ScreenId id, const std::vector<NodeId>& blackList, int32_t& repCode)
{
    if (blackList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: black list is over max size!", __func__);
        repCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    if (blackList.empty()) {
        RS_LOGW("%{public}s: blackList is empty.", __func__);
        repCode = StatusCode::BLACKLIST_IS_EMPTY;
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManager_ == nullptr) {
        RS_LOGW("%{public}s: screenManager is nullptr.", __func__);
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManager_->AddVirtualScreenBlackList(id, blackList);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RemoveVirtualScreenBlackList(
    ScreenId id, const std::vector<NodeId>& blackList, int32_t& repCode)
{
    if (blackList.empty()) {
        RS_LOGW("%{public}s: blackList is empty.", __func__);
        repCode = StatusCode::BLACKLIST_IS_EMPTY;
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManager_ == nullptr) {
        RS_LOGW("%{public}s: screenManager is nullptr.", __func__);
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManager_->RemoveVirtualScreenBlackList(id, blackList);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AddVirtualScreenWhiteList(
    ScreenId id, const std::vector<NodeId>& whiteList, int32_t& repCode)
{
    if (whiteList.size() > MAX_SPECIAL_LAYER_NUM) {
        RS_LOGW("%{public}s: whitelist is over max size!", __func__);
        repCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    if (whiteList.empty()) {
        RS_LOGW("%{public}s: whitelist is empty.", __func__);
        repCode = StatusCode::WHITELIST_IS_EMPTY;
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManager_ == nullptr) {
        RS_LOGW("%{public}s: screenManager is nullptr.", __func__);
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManager_->AddVirtualScreenWhiteList(id, whiteList);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RemoveVirtualScreenWhiteList(
    ScreenId id, const std::vector<NodeId>& whiteList, int32_t& repCode)
{
    if (whiteList.empty()) {
        RS_LOGW("%{public}s: whitelist is empty.", __func__);
        repCode = StatusCode::WHITELIST_IS_EMPTY;
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManager_ == nullptr) {
        RS_LOGW("%{public}s: screenManager is nullptr.", __func__);
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManager_->RemoveVirtualScreenWhiteList(id, whiteList);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSClientToServiceConnection::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenSecurityMask(id, std::move(securityMask));
}

int32_t RSClientToServiceConnection::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    if (screenManager_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSClientToServiceConnection::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSClientToServiceConnection::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSurface(id, surface);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSClientToServiceConnection::SetPointerColorInversionConfig(float darkBuffer,
    float brightBuffer, int64_t interval, int32_t rangeSize)
{
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionConfig(darkBuffer, brightBuffer,
        interval, rangeSize);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::SetPointerColorInversionEnabled(bool enable)
{
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionEnabled(enable);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterPointerLuminanceChangeCallback(
    sptr<RSIPointerLuminanceChangeCallback> callback)
{
    if (!callback) {
        RS_LOGE("RegisterPointerLuminanceChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    RSMagicPointerRenderManager::GetInstance().RegisterPointerLuminanceChangeCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::UnRegisterPointerLuminanceChangeCallback()
{
    RSMagicPointerRenderManager::GetInstance().UnRegisterPointerLuminanceChangeCallback(remotePid_);
    return StatusCode::SUCCESS;
}
#endif

void RSClientToServiceConnection::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return;
    }
    screenManager_->RemoveVirtualScreen(id);
    virtualScreenIds_.erase(id);
    EventInfo event = { "VOTER_VIRTUALDISPLAY", REMOVE_VOTE };
    NotifyRefreshRateEvent(event);
}

int32_t RSClientToServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (!callback) {
        RS_LOGE("%{public}s: callback is nullptr", __func__);
        return INVALID_ARGUMENTS;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (screenChangeCallback_ == callback) {
        RS_LOGE("%{public}s: the callback has been set", __func__);
        return INVALID_ARGUMENTS;
    }
    if (screenManager_ == nullptr) {
        RS_LOGE("%{public}s: screenManager_ is nullptr", __func__);
        return SCREEN_NOT_FOUND;
    }

    if (screenChangeCallback_ != nullptr) {
        // remove the old callback
        RS_LOGW("%{public}s: last screenChangeCallback_ should be removed", __func__);
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
    }

    // update
    int32_t status = screenManager_->AddScreenChangeCallback(callback);
    screenChangeCallback_ = callback;
    return status;
}

int32_t RSClientToServiceConnection::SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback)
{
    if (screenManager_ == nullptr) {
        RS_LOGE("%{public}s: screenManager_ is nullptr", __func__);
        return SCREEN_NOT_FOUND;
    }

    // update
    int32_t status = screenManager_->SetScreenSwitchingNotifyCallback(callback);
    return status;
}

int32_t RSClientToServiceConnection::SetBrightnessInfoChangeCallback(sptr<RSIBrightnessInfoChangeCallback> callback)
{
    if (mainThread_ == nullptr) {
        return INVALID_ARGUMENTS;
    }
    auto task = [this, &callback]() {
        auto& context = mainThread_->GetContext();
        return context.SetBrightnessInfoChangeCallback(remotePid_, callback);
    };
    return mainThread_->ScheduleTask(task).get();
}

void RSClientToServiceConnection::CleanBrightnessInfoChangeCallbacks() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    context.SetBrightnessInfoChangeCallback(remotePid_, nullptr);
}

int32_t RSClientToServiceConnection::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    brightnessInfo = RSLuminanceControl::Get().GetBrightnessInfo(screenId);
    return StatusCode::SUCCESS;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSClientToServiceConnection::CleanCanvasCallbacksAndPendingBuffer() noexcept
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.RegisterCanvasCallback(remotePid_, nullptr);
    bufferCache.ClearPendingBufferByPid(remotePid_);
}
#endif

uint32_t RSClientToServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenActiveMode(id, modeId); });
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenActiveMode(id, modeId); }).get();
    }
    return StatusCode::RS_CONNECTION_ERROR;
}

void RSClientToServiceConnection::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    RS_TRACE_NAME_FMT("RSRenderService::SetScreenRefreshRate");
    HgmTaskHandleThread::Instance().PostTask([id, sceneId, rate] () {
        int32_t setResult = HgmCore::Instance().SetScreenRefreshRate(id, sceneId, rate);
        if (setResult != 0) {
            RS_LOGW("SetScreenRefreshRate request of screen %{public}" PRIu64 " of rate %{public}d is refused",
                id, rate);
            return;
        }
    });
}

void RSClientToServiceConnection::SetRefreshRateMode(int32_t refreshRateMode)
{
    if (!mainThread_) {
        return;
    }
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSClientToServiceConnection::SetRefreshRateMode");
    HgmTaskHandleThread::Instance().PostTask([refreshRateMode] () {
        int32_t setResult = HgmCore::Instance().SetRefreshRateMode(refreshRateMode);
        RSSystemProperties::SetHgmRefreshRateModesEnabled(std::to_string(refreshRateMode));
        if (setResult != 0) {
            RS_LOGW("SetRefreshRateMode mode %{public}d is not supported", refreshRateMode);
        }
    });
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSClientToServiceConnection::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this), id, &range, animatorExpectedFrameRate]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            auto& context = connection->mainThread_->GetContext();
            auto& linkerMap = context.GetMutableFrameRateLinkerMap();
            auto linker = linkerMap.GetFrameRateLinker(id);
            if (linker == nullptr) {
                RS_LOGW("SyncFrameRateRange there is no frameRateLinker for id %{public}" PRIu64, id);
                return;
            }
            linker->SetExpectedRange(range);
            linker->SetAnimatorExpectedFrameRate(animatorExpectedFrameRate);
            if (range.type_ != OHOS::Rosen::NATIVE_VSYNC_FRAME_RATE_TYPE) {
                return;
            }
            auto appVSyncDistributor = connection->appVSyncDistributor_;
            if (appVSyncDistributor == nullptr) {
                return;
            }
            auto conn = appVSyncDistributor->GetVSyncConnection(id);
            if (conn == nullptr) {
                return;
            }
            std::weak_ptr<RSRenderFrameRateLinker> weakPtr = linker;
            conn->RegisterRequestNativeVSyncCallback([weakPtr]() {
                RS_TRACE_NAME("NativeVSync request frame, update timepoint");
                auto linker = weakPtr.lock();
                if (linker == nullptr) {
                    return;
                }
                linker->UpdateNativeVSyncTimePoint();
            });
        }).wait();
}

void RSClientToServiceConnection::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this), id]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            auto& context = connection->mainThread_->GetContext();
            auto& linkerMap = context.GetMutableFrameRateLinkerMap();
            auto linker = linkerMap.GetFrameRateLinker(id);
            if (linker == nullptr) {
                RS_LOGE("UnregisterFrameRateLinker there is no frameRateLinker for id %{public}" PRIu64, id);
                return;
            }
            linkerMap.UnregisterFrameRateLinker(id);
        }).wait();
}

uint32_t RSClientToServiceConnection::GetScreenCurrentRefreshRate(ScreenId id)
{
    uint32_t rate = HgmTaskHandleThread::Instance().ScheduleTask([id] () -> uint32_t {
        return HgmCore::Instance().GetScreenCurrentRefreshRate(id);
    }).get();
    if (rate == 0) {
        RS_LOGW("GetScreenCurrentRefreshRate failed to get current refreshrate screen : %{public}" PRIu64, id);
    }
    return rate;
}

std::vector<int32_t> RSClientToServiceConnection::GetScreenSupportedRefreshRates(ScreenId id)
{
    return HgmTaskHandleThread::Instance().ScheduleTask([id] () -> std::vector<int32_t> {
        return HgmCore::Instance().GetScreenComponentRefreshRates(id);
    }).get();
}

ErrCode RSClientToServiceConnection::GetShowRefreshRateEnabled(bool& enable)
{
    enable = RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled();
    return ERR_OK;
}

void RSClientToServiceConnection::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    return RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enabled, type);
}

uint32_t RSClientToServiceConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    return RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRate(screenId);
}

ErrCode RSClientToServiceConnection::GetRefreshInfo(pid_t pid, std::string& enable)
{
    if (!mainThread_) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();
    std::string surfaceName = nodeMap.GetSelfDrawSurfaceNameByPid(pid);
    if (surfaceName.empty()) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    std::string dumpString;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSRenderComposerManager::GetInstance().FpsDump(dumpString, surfaceName);
#endif
    } else {
        mainThread_->ScheduleTask(
            [weakThis = wptr<RSClientToServiceConnection>(this), &dumpString, &surfaceName]() {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->FpsDump(dumpString, surfaceName);
            }).wait();
    }
    enable = dumpString;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetRefreshInfoToSP(NodeId id, std::string& enable)
{
    if (!mainThread_) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    std::string dumpString;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    auto dumpTask = [weakThis = wptr<RSClientToServiceConnection>(this), &dumpString, &id]() {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr) {
            RS_LOGE("GetRefreshInfoToSP connection is nullptr");
            return;
        }
        if (connection->screenManager_ == nullptr) {
            RS_LOGE("GetRefreshInfoToSP connection->screenManager_ is nullptr");
            return;
        }
        RSSurfaceFpsManager::GetInstance().Dump(dumpString, id);
    };
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // Direct call to screenManager as it aggregates data from all screens
        dumpTask();
#endif
    } else {
        mainThread_->ScheduleTask(dumpTask).wait();
    }
    enable = dumpString;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetRefreshInfoByPidAndUniqueId(pid_t pid, uint64_t uniqueId, std::string& enable)
{
    if (!mainThread_) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();
    std::string surfaceName = (uniqueId == 0 ? nodeMap.GetSelfDrawSurfaceNameByPid(pid)
                                             : nodeMap.GetSelfDrawSurfaceNameByPidAndUniqueId(pid, uniqueId));
    if (surfaceName.empty()) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    std::string dumpString;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSRenderComposerManager::GetInstance().FpsDump(dumpString, surfaceName);
#endif
    } else {
        mainThread_
            ->ScheduleTask([weakThis = wptr<RSClientToServiceConnection>(this), &dumpString, &surfaceName]() {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->FpsDump(dumpString, surfaceName);
            })
            .wait();
    }
    enable = dumpString;
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetCurrentRefreshRateMode()
{
    return HgmTaskHandleThread::Instance().ScheduleTask([] () -> int32_t {
        return HgmCore::Instance().GetCurrentRefreshRateMode();
    }).get();
}

int32_t RSClientToServiceConnection::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManager_->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSClientToServiceConnection::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetVirtualScreenResolution(id, width, height); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetVirtualScreenResolution(id, width, height); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManager_->SetRogScreenResolution(id, width, height);
}

int32_t RSClientToServiceConnection::GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManager_->GetRogScreenResolution(id, width, height);
}

ErrCode RSClientToServiceConnection::MarkPowerOffNeedProcessOneFrame()
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return ERR_INVALID_VALUE;
    }
#ifdef RS_ENABLE_GPU
    renderThread_.PostTask([weakThis = wptr<RSClientToServiceConnection>(this)]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->screenManager_ == nullptr) {
                return;
            }
            connection->screenManager_->MarkPowerOffNeedProcessOneFrame();
        });
#endif
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RepaintEverything()
{
    if (mainThread_ == nullptr) {
        RS_LOGE("RepaintEverything, mainThread_ is null, return");
        return ERR_INVALID_VALUE;
    }
    auto task = []() -> void {
        RS_LOGI("RepaintEverything, setDirtyflag, forceRefresh in mainThread");
        RSMainThread::Instance()->SetDirtyFlag();
        RSMainThread::Instance()->ForceRefreshForUni();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ForceRefreshOneFrameWithNextVSync()
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return ERR_INVALID_VALUE;
    }

    auto task = [weakThis = wptr<RSClientToServiceConnection>(this)]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }

        RS_LOGI("ForceRefreshOneFrameWithNextVSync, setDirtyflag, forceRefresh in mainThread");
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}


void RSClientToServiceConnection::DisablePowerOffRenderControl(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        renderThread_.PostTask(
            [weakThis = wptr<RSClientToServiceConnection>(this), id]() {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->DisablePowerOffRenderControl(id);
            }
        );
#endif
    }
}

void RSClientToServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (screenManager_ == nullptr || mainThread_ == nullptr) {
        RS_LOGE("%{public}s screenManager or mainThread is null, id: %{public}" PRIu64, __func__, id);
        return;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSRenderComposerManager::GetInstance().PostSyncTask(id, [=]() {
            screenManager_->SetScreenPowerStatus(id, status);
        });
        RSRenderComposerManager::GetInstance().HandlePowerStatus(id, status);
        screenManager_->WaitScreenPowerStatusTask();
        mainThread_->SetDiscardJankFrames(true);
        RSJankStatsRenderFrameHelper::GetInstance().SetDiscardJankFrames(true);
        HgmTaskHandleThread::Instance().PostTask([id, status]() {
            HgmCore::Instance().NotifyScreenPowerStatus(id, status);
        });
#endif
    } else {
        mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenPowerStatus(id, status); }).wait();
    }
}

int32_t RSClientToServiceConnection::SetDualScreenState(ScreenId id, DualScreenStatus status)
{
    if (!screenManager_) {
        RS_LOGE("%{public}s screenManager is null, id: %{public}" PRIu64, __func__, id);
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetDualScreenState(id, status); });
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetDualScreenState(id, status); }).get();
    } else {
        RS_LOGE("%{public}s mainThread_ is null, id: %{public}" PRIu64, __func__, id);
        return StatusCode::MAIN_THREAD_NULL;
    }
}

namespace {
void TakeSurfaceCaptureForUiParallel(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const Drawing::Rect& specifiedAreaRect)
{
#ifdef RS_ENABLE_GPU
    RS_LOGI("TakeSurfaceCaptureForUiParallel nodeId:[%{public}" PRIu64 "], issync:%{public}s", id,
        captureConfig.isSync ? "true" : "false");
    std::function<void()> captureTask = [id, callback, captureConfig, specifiedAreaRect]() {
        RSUiCaptureTaskParallel::Capture(id, callback, captureConfig, specifiedAreaRect);
    };
    auto& context = RSMainThread::Instance()->GetContext();
    if (captureConfig.isSync) {
        context.GetUiCaptureHelper().InsertUiCaptureCmdsExecutedFlag(id, false);
        RSMainThread::Instance()->AddUiCaptureTask(id, captureTask);
        return;
    }

    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(id);
    if (!node) {
        RS_LOGE("TakeSurfaceCaptureForUiParallel node is nullptr");
        callback->OnSurfaceCapture(id, captureConfig, nullptr);
        return;
    }

    if (node->IsOnTheTree() && !node->IsDirty() && !node->IsSubTreeDirty()) {
        RSMainThread::Instance()->PostTask(captureTask);
    } else {
        RSMainThread::Instance()->AddUiCaptureTask(id, captureTask);
    }
#endif
}

void TakeSurfaceCaptureForUIWithUni(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
#ifdef RS_ENABLE_GPU
    std::function<void()> offscreenRenderTask = [id, callback, captureConfig]() -> void {
        RS_LOGD("RSRenderService::TakeSurfaceCaptureForUIWithUni callback->OnOffscreenRender"
            " nodeId:[%{public}" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCaptureForUIWithUni");
        std::shared_ptr<RSUniUICapture> rsUniUICapture = std::make_shared<RSUniUICapture>(id, captureConfig);
        std::shared_ptr<Media::PixelMap> pixelmap = rsUniUICapture->TakeLocalCapture();
        callback->OnSurfaceCapture(id, captureConfig, pixelmap.get());
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    };
    if (!captureConfig.isSync) {
        RSOffscreenRenderThread::Instance().PostTask(offscreenRenderTask);
    } else {
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(id);
        if (node == nullptr || !node->GetCommandExecuted()) {
            RSOffscreenRenderThread::Instance().InSertCaptureTask(id, offscreenRenderTask);
            return;
        }
        RSOffscreenRenderThread::Instance().PostTask(offscreenRenderTask);
        node->SetCommandExecuted(false);
    }
#endif
}
}

ErrCode RSClientToServiceConnection::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (!mainThread_) {
        RS_LOGE("RegisterApplicationAgent mainThread_ is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto captureTask = [weakThis = wptr<RSClientToServiceConnection>(this), pid, app]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            RS_LOGE("RegisterApplicationAgent connection or mainThread_ is nullptr");
            return;
        }
        connection->mainThread_->RegisterApplicationAgent(pid, app);
    };
    mainThread_->PostTask(captureTask);

    app->AsObject()->AddDeathRecipient(applicationDeathRecipient_);
    return ERR_OK;
}

void RSClientToServiceConnection::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    auto captureTask = [app]() -> void {
        RSMainThread::Instance()->UnRegisterApplicationAgent(app);
    };
    RSMainThread::Instance()->ScheduleTask(captureTask).wait();
}

RSVirtualScreenResolution RSClientToServiceConnection::GetVirtualScreenResolution(ScreenId id)
{
    RSVirtualScreenResolution virtualScreenResolution;
    if (!screenManager_) {
        return virtualScreenResolution;
    }
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    return virtualScreenResolution;
}

ErrCode RSClientToServiceConnection::GetScreenActiveMode(uint64_t screenId, RSScreenModeInfo& screenModeInfo)
{
    if (!screenManager_) {
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSRenderComposerManager::GetInstance().PostSyncTask(screenId, [=, &screenModeInfo]() {
            return screenManager_->GetScreenActiveMode(screenId, screenModeInfo);
        });
#else
        return screenModeInfo;
#endif
    } else if (mainThread_ != nullptr) {
        mainThread_->ScheduleTask([=, &screenModeInfo]() {
            return screenManager_->GetScreenActiveMode(screenId, screenModeInfo);
        }).wait();
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
#ifdef RS_ENABLE_GPU
    RSMainThread::Instance()->GetAppMemoryInMB(cpuMemSize, gpuMemSize);
    gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
#endif
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (pid == 0) { // pid of app can not be 0
        return ERR_INVALID_VALUE;
    }
    bool enable;
    if (GetUniRenderEnabled(enable) == ERR_OK && enable) {
        renderThread_.PostSyncTask(
            [weakThis = wptr<RSClientToServiceConnection>(this), &memoryGraphic, &pid] {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr) {
                    return;
                }
                memoryGraphic = MemoryManager::CountPidMemory(pid,
                    connection->renderThread_.GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
            });
        return ERR_OK;
    } else {
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    bool res;
    if (!mainThread_ || GetUniRenderEnabled(res) != ERR_OK || !res) {
        return ERR_INVALID_VALUE;
    }
    
    mainThread_->ScheduleTask([mainThread = mainThread_, &memoryGraphics]() {
            mainThread->CountMem(memoryGraphics);
        }).wait();
    return ERR_OK;
}

std::vector<RSScreenModeInfo> RSClientToServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    if (!screenManager_) {
        return std::vector<RSScreenModeInfo>();
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->GetScreenSupportedModes(id); });
#else
        return std::vector<RSScreenModeInfo>();
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenSupportedModes(id); }).get();
    } else {
        return std::vector<RSScreenModeInfo>();
    }
}

RSScreenCapability RSClientToServiceConnection::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    if (!screenManager_) {
        return screenCapability;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->GetScreenCapability(id); });
#else
        return screenCapability;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenCapability(id); }).get();
    } else {
        return screenCapability;
    }
}

ErrCode RSClientToServiceConnection::GetScreenPowerStatus(uint64_t screenId, uint32_t& status)
{
    if (!screenManager_) {
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
        return ERR_INVALID_OPERATION;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        status = RSRenderComposerManager::GetInstance().PostSyncTask(screenId,
            [=]() { return screenManager_->GetScreenPowerStatus(screenId); });
#else
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
#endif
    } else if (mainThread_ != nullptr) {
        status = mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenPowerStatus(screenId); }).get();
    } else {
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

RSScreenData RSClientToServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    if (!screenManager_) {
        return screenData;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->GetScreenData(id); });
#else
        return screenData;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenData(id); }).get();
    } else {
        return screenData;
    }
}

ErrCode RSClientToServiceConnection::GetScreenBacklight(uint64_t screenId, int32_t& level)
{
    if (!screenManager_) {
        level = INVALID_BACKLIGHT_VALUE;
        return ERR_INVALID_OPERATION;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        level = RSRenderComposerManager::GetInstance().PostSyncTask(screenId,
            [=]() { return screenManager_->GetScreenBacklight(screenId); });
#else
        level = INVALID_BACKLIGHT_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        level = mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenBacklight(screenId); }).get();
    } else {
        level = INVALID_BACKLIGHT_VALUE;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    if (!screenManager_) {
        RS_LOGE("%{public}s screenManager_ is nullptr.", __func__);
        return;
    }
    RSLuminanceControl::Get().SetSdrLuminance(id, level);
    if (RSLuminanceControl::Get().IsHdrOn(id) && level > 0 && mainThread_ != nullptr) {
        auto task = [weakThis = wptr<RSClientToServiceConnection>(this), id]() {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                RS_LOGE("SetScreenBacklight fail");
                return;
            }
            connection->mainThread_->SetForceUpdateUniRenderFlag(true);
            connection->mainThread_->SetLuminanceChangingStatus(id, true);
            connection->mainThread_->SetDirtyFlag();
            connection->mainThread_->RequestNextVSync();
        };
        mainThread_->PostTask(task);
        return;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        screenManager_->SetScreenBacklight(id, level);
#endif
    } else if (mainThread_ != nullptr) {
        mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenBacklight(id, level); }).wait();
    }
}

ErrCode RSClientToServiceConnection::GetPanelPowerStatus(ScreenId screenId, PanelPowerStatus& status)
{
    if (!screenManager_) {
        RS_LOGE("%{public}s screenManager_ is nullptr.", __func__);
        status = PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
        return ERR_INVALID_OPERATION;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        status = RSRenderComposerManager::GetInstance().PostSyncTask(screenId,
            [screenManager = screenManager_, screenId]() {
                return screenManager->GetPanelPowerStatus(screenId);
            });
#else
        status = PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
#endif
    } else if (mainThread_ != nullptr) {
        status = mainThread_->ScheduleTask(
            [screenManager = screenManager_, screenId]() {
                return screenManager->GetPanelPowerStatus(screenId);
            }).get();
    } else {
        status = PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferClearListener =
        [id, callback, weakThis = wptr<RSClientToServiceConnection>(this)]() -> bool {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return false;
            }
            if (auto node = connection->mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
                node->RegisterBufferClearListener(callback);
                return true;
            }
            return false;
    };
    mainThread_->PostTask(registerBufferClearListener);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferAvailableListener =
        [id, callback, isFromRenderThread, weakThis = wptr<RSClientToServiceConnection>(this)]() -> bool {
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return false;
            }
            if (auto node = connection->mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
                node->RegisterBufferAvailableListener(callback, isFromRenderThread);
                return true;
            }
            return false;
    };
    mainThread_->PostTask(registerBufferAvailableListener);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetScreenSupportedColorGamuts(ScreenId id,
    std::vector<ScreenColorGamut>& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &mode]() { return screenManager_->GetScreenSupportedColorGamuts(id, mode); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenSupportedColorGamuts(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::GetScreenSupportedMetaDataKeys(
    ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &keys]() { return screenManager_->GetScreenSupportedMetaDataKeys(id, keys); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &keys]() { return screenManager_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &mode]() { return screenManager_->GetScreenColorGamut(id, mode); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenColorGamut(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenColorGamut(id, modeIdx); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenColorGamut(id, modeIdx); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenGamutMap(id, mode); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenGamutMap(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenCorrection(id, screenRotation);
}

bool RSClientToServiceConnection::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return false;
    }
    return screenManager_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSClientToServiceConnection::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManager_->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

ErrCode RSClientToServiceConnection::SetGlobalDarkColorMode(bool isDark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), isDark]() {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            RS_LOGE("SetGlobalDarkColorMode fail");
            return;
        }
        connection->mainThread_->SetGlobalDarkColorMode(isDark);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

bool RSClientToServiceConnection::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return false;
    }
    return screenManager_->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

int32_t RSClientToServiceConnection::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &mode]() { return screenManager_->GetScreenGamutMap(id, mode); });
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenGamutMap(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenHDRCapability(id, screenHdrCapability);
}

ErrCode RSClientToServiceConnection::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &pixelFormat]() { return screenManager_->GetPixelFormat(id, pixelFormat); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=, &pixelFormat]() { return screenManager_->GetPixelFormat(id, pixelFormat); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    if (pixelFormat < 0 ||
        (pixelFormat >= GRAPHIC_PIXEL_FMT_END_OF_VALID && pixelFormat != GRAPHIC_PIXEL_FMT_VENDER_MASK)) {
        resCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetPixelFormat(id, pixelFormat); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetPixelFormat(id, pixelFormat); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetScreenSupportedHDRFormats(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &hdrFormats]() { return screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=, &hdrFormats]() { return screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &hdrFormat]() { return screenManager_->GetScreenHDRFormat(id, hdrFormat); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=, &hdrFormat]() { return screenManager_->GetScreenHDRFormat(id, hdrFormat); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenHDRFormat(id, modeIdx); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenHDRFormat(id, modeIdx); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &colorSpaces]() { return screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=, &colorSpaces]() { return screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace,
    int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=, &colorSpace]() { return screenManager_->GetScreenColorSpace(id, colorSpace); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=, &colorSpace]() { return screenManager_->GetScreenColorSpace(id, colorSpace); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::SetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenColorSpace(id, colorSpace); });
        return ERR_OK;
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        resCode = mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenColorSpace(id, colorSpace); }).get();
        return ERR_OK;
    } else {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
}

int32_t RSClientToServiceConnection::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenType(id, screenType);
}

ErrCode RSClientToServiceConnection::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
    RSMainThread* mainThread = mainThread_;
#ifdef RS_ENABLE_GPU
    RSUniRenderThread* renderThread = &renderThread_;
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

ErrCode RSClientToServiceConnection::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
#ifdef RS_ENABLE_GPU
    RSMainThread* mainThread = mainThread_;
    RSUniRenderThread* renderThread = &renderThread_;
    auto getPixelMapTask = [id, pixelmap, rect, drawCmdList, mainThread, renderThread, &result]() {
        auto node = mainThread->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
        if (node == nullptr) {
            RS_LOGD("GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", id);
            result.set_value(false);
            return;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            result.set_value(false);
            return;
        }

        auto tid = node->GetTid(); // planning: id may change in subthread
        auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
        if (drawableNode) {
            tid = std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)->GetTid();
        }
        auto getDrawablePixelmapTask = [drawableNode, pixelmap, rect, &result, tid, drawCmdList]() {
            result.set_value(std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)->
                GetPixelmap(pixelmap, rect, tid, drawCmdList));
        };
        if (!node->IsOnTheTree()) {
            node->ClearOp();
        }
        if (tid == UNI_MAIN_THREAD_INDEX) {
            if (!mainThread->IsIdle() && mainThread->GetContext().HasActiveNode(node)) {
                result.set_value(false);
                return;
            }
            result.set_value(node->GetPixelmap(pixelmap, rect, tid, drawCmdList));
        } else if (tid == UNI_RENDER_THREAD_INDEX) {
            renderThread->PostTask(getDrawablePixelmapTask);
        } else {
            const auto& tidMap = RSSubThreadManager::Instance()->GetReThreadIndexMap();
            if (auto found = tidMap.find(tid); found != tidMap.end()) {
                RSTaskDispatcher::GetInstance().PostTask(found->second, getDrawablePixelmapTask, false);
            } else {
                renderThread->PostTask(getDrawablePixelmapTask);
            }
        }
    };
    mainThread_->PostTask(getPixelMapTask);
#endif
    success = future.get();
    return ERR_OK;
}

bool RSClientToServiceConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    RS_LOGI("Reg typeface, pid[%{public}d], family name:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

int32_t RSClientToServiceConnection::RegisterTypeface(Drawing::SharedTypeface& sharedTypeface, int32_t& needUpdate)
{
    if (sharedTypeface.originId_ > 0) {
        ::close(sharedTypeface.fd_);
        needUpdate = -1;
        return RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface);
    }
    if (sharedTypeface.fd_ < 0 || sharedTypeface.size_ == 0) {
        RS_LOGE("RegisterTypeface: invalid parameters");
        ::close(sharedTypeface.fd_);
        needUpdate = -1;
        return -1;
    }
    int realSize = AshmemGetSize(sharedTypeface.fd_);
    if (realSize < 0 || static_cast<uint32_t>(realSize) < sharedTypeface.size_) {
        RS_LOGE("RegisterTypeface, realSize < 0 or realSize < given size");
        ::close(sharedTypeface.fd_);
        needUpdate = -1;
        return -1;
    }
    auto tf = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    if (tf != nullptr) {
        ::close(sharedTypeface.fd_);
        sharedTypeface.fd_ = -1;
        needUpdate = 1;
        RS_LOGI("RegisterTypeface: Find same typeface in cache, use existed typeface.");
        return tf->GetFd();
    }
    RS_LOGI("RegisterTypeface: Failed to find typeface in cache, create a new typeface.");
    needUpdate = 0;
    tf = Drawing::Typeface::MakeFromAshmem(sharedTypeface);
    if (tf != nullptr) {
        RSTypefaceCache::Instance().CacheDrawingTypeface(sharedTypeface.id_, tf);
        sharedTypeface.fd_ = -1;
        return tf->GetFd();
    }
    ::close(sharedTypeface.fd_);
    sharedTypeface.fd_ = -1;
    needUpdate = -1;
    RS_LOGE("RegisterTypeface: failed to register typeface");
    return -1;
}

bool RSClientToServiceConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    RS_LOGW("Unreg typeface, pid[%{public}d], uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    auto typeface = RSTypefaceCache::Instance().GetDrawingTypefaceCache(globalUniqueId);
    if (typeface == nullptr) {
        return true;
    }
    uint32_t uniqueId = typeface->GetUniqueID();
    auto task = [uniqueId]() {
        auto context = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        if (context) {
            context->FreeCpuCache(uniqueId);
            context->PurgeUnlockAndSafeCacheGpuResources();
        }
    };
    RSUniRenderThread::Instance().PostTask(task);
    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);
    return true;
}

int32_t RSClientToServiceConnection::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetDisplayIdentificationData(id, outPort, edidData);
}

ErrCode RSClientToServiceConnection::SetScreenSkipFrameInterval(
    uint64_t id, uint32_t skipFrameInterval, int32_t& resCode)
{
    if (!screenManager_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        resCode = RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); });
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
#endif
    } else {
        if (!mainThread_) {
            resCode = StatusCode::INVALID_ARGUMENTS;
            return ERR_INVALID_VALUE;
        }
        resCode = mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); })
            .get();
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal)
{
    if (!screenManager_) {
        retVal = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    retVal = screenManager_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode)
{
    if (!screenManager_) {
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    GraphicIRect dstActiveRect {
        .x = activeRect.x,
        .y = activeRect.y,
        .w = activeRect.w,
        .h = activeRect.h,
    };
    if (!mainThread_) {
        repCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> task = [weakScreenManager = wptr<RSScreenManager>(screenManager_),
                                    id, dstActiveRect]() -> void {
        sptr<RSScreenManager> screenManager = weakScreenManager.promote();
        if (!screenManager) {
            return;
        }
        screenManager->SetScreenActiveRect(id, dstActiveRect);
    };
    mainThread_->ScheduleTask(std::move(task)).wait();

    HgmTaskHandleThread::Instance().PostTask([id, dstActiveRect]() {
        HgmCore::Instance().NotifyScreenRectFrameRateChange(id, dstActiveRect);
    });
    repCode = StatusCode::SUCCESS;
    return ERR_OK;
}

void RSClientToServiceConnection::SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    if (!screenManager_) {
        return;
    }
    screenManager_->SetScreenOffset(id, offsetX, offsetY);
}

ErrCode RSClientToServiceConnection::RegisterOcclusionChangeCallback(
    sptr<RSIOcclusionChangeCallback> callback, int32_t& repCode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        repCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    if (!callback) {
        RS_LOGD("RegisterOcclusionChangeCallback: callback is nullptr");
        repCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    mainThread_->RegisterOcclusionChangeCallback(remotePid_, callback);
    repCode = StatusCode::SUCCESS;
    return ERR_OK;
}

int32_t RSClientToServiceConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGD("RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterSurfaceOcclusionChangeCallback(id, remotePid_, callback, partitionPoints);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RegisterHgmConfigChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([pid = remotePid_, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmConfigChangeCallback(pid, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterHgmRefreshRateModeChangeCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RegisterHgmRefreshRateModeChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([pid = remotePid_, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateModeChangeCallback(pid, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterHgmRefreshRateUpdateCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    HgmTaskHandleThread::Instance().PostSyncTask([pid = remotePid_, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateUpdateCallback(pid, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterFirstFrameCommitCallback(
    sptr<RSIFirstFrameCommitCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RSFirstFrameNotifier::GetInstance().RegisterFirstFrameCommitCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    if (dstPid == 0) {
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostTask([remotePid = remotePid_, dstPid, callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterXComponentExpectedFrameRateCallback(
            remotePid, dstPid, callback);
    });

    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
#ifdef RS_ENABLE_GPU
    RSUifirstManager::Instance().OnProcessAnimateScene(systemAnimatedScenes);
    success = mainThread_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
    return ERR_OK;
#else
    success = false;
    return ERR_INVALID_VALUE;
#endif
}

void RSClientToServiceConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), watermarkImg, isShow]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->ShowWatermark(watermarkImg, isShow);
    };
    mainThread_->PostTask(task);
}

int32_t RSClientToServiceConnection::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSRenderComposerManager::GetInstance().PostSyncTask(id,
            [weakThis = wptr<RSClientToServiceConnection>(this), id, width, height]() -> int32_t {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return RS_CONNECTION_ERROR;
                }
                return connection->screenManager_->ResizeVirtualScreen(id, width, height);
            }
        );
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [weakThis = wptr<RSClientToServiceConnection>(this), id, width, height]() -> int32_t {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return RS_CONNECTION_ERROR;
                }
                return connection->screenManager_->ResizeVirtualScreen(id, width, height);
            }
        ).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

ErrCode RSClientToServiceConnection::ReportJankStats()
{
#ifdef RS_ENABLE_GPU
    auto task = []() -> void { RSJankStats::GetInstance().ReportJankStats(); };
    renderThread_.PostTask(task);
#endif
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, lightFactorStatus]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleLightFactorStatus(pid, lightFactorStatus);
        }
    });
    return ERR_OK;
}

void RSClientToServiceConnection::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->NotifyPackageEvent(packageList);
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), packageList]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (!connection || !connection->mainThread_ || !connection->mainThread_->GetHwcContext()) {
            return;
        }
        connection->mainThread_->GetHwcContext()->CheckPackageInConfigList(packageList);
    };
    mainThread_->PostTask(task);

    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, listSize, packageList]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandlePackageEvent(pid, packageList);
        }
    });
}

void RSClientToServiceConnection::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, listSize, pkgName, newConfig] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleAppStrategyConfigEvent(pid, pkgName, newConfig);
        }
    });
}

void RSClientToServiceConnection::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    if (!mainThread_) {
        return;
    }
    if (VOTER_SCENE_BLUR == eventInfo.eventName) {
        RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
        return;
    }

    if (VOTER_SCENE_GPU == eventInfo.eventName) {
        RsFrameReport::GetInstance().ReportScbSceneInfo(eventInfo.description, eventInfo.eventStatus);
        return;
    }
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, eventInfo]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleRefreshRateEvent(pid, eventInfo);
        }
    });
}

void RSClientToServiceConnection::SetWindowExpectedRefreshRate(
    const std::unordered_map<uint64_t, EventInfo>& eventInfos
)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, eventInfos]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            auto& softVsyncMgr = frameRateMgr->SoftVSyncMgrRef();
            softVsyncMgr.SetWindowExpectedRefreshRate(pid, eventInfos);
        }
    });
}

void RSClientToServiceConnection::SetWindowExpectedRefreshRate(
    const std::unordered_map<std::string, EventInfo>& eventInfos
)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, eventInfos]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            auto& softVsyncMgr = frameRateMgr->SoftVSyncMgrRef();
            softVsyncMgr.SetWindowExpectedRefreshRate(pid, eventInfos);
        }
    });
}

ErrCode RSClientToServiceConnection::NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount)
{
    if (!appVSyncDistributor_) {
        return ERR_INVALID_VALUE;
    }
    appVSyncDistributor_->SetQosVSyncRateByPidPublic(pid, rateDiscount, true);
    return ERR_OK;
}

bool RSClientToServiceConnection::NotifySoftVsyncRateDiscountEvent(uint32_t pid,
    const std::string &name, uint32_t rateDiscount)
{
    if (!appVSyncDistributor_) {
        return false;
    }

    std::vector<uint64_t> linkerIds = appVSyncDistributor_->GetVsyncNameLinkerIds(pid, name);
    if (linkerIds.empty()) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}u linkerIds is nullptr.", pid);
        return false;
    }

    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}u frameRateMgr is nullptr.", pid);
        return false;
    }

    if (!frameRateMgr->SetVsyncRateDiscountLTPO(linkerIds, rateDiscount)) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}u Set LTPO fail.", pid);
        return false;
    }

    VsyncError ret = appVSyncDistributor_->SetVsyncRateDiscountLTPS(pid, name, rateDiscount);
    if (ret != VSYNC_ERROR_OK) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}u Set LTPS fail.", pid);
        return false;
    }

    return true;
}

ErrCode RSClientToServiceConnection::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt, int32_t sourceType)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    mainThread_->HandleTouchEvent(touchStatus, touchCnt);
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        frameRateMgr->HandleTouchEvent(remotePid_, touchStatus, touchCnt, sourceType);
    }
    return ERR_OK;
}

void RSClientToServiceConnection::NotifyDynamicModeEvent(bool enableDynamicModeEvent)
{
    HgmTaskHandleThread::Instance().PostTask([enableDynamicModeEvent] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            RS_LOGW("NotifyDynamicModeEvent: frameRateMgr is nullptr.");
            return;
        }
        frameRateMgr->HandleDynamicModeEvent(enableDynamicModeEvent);
    });
}

ErrCode RSClientToServiceConnection::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    HgmTaskHandleThread::Instance().PostTask([eventName, state] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            RS_LOGW("NotifyHgmConfigEvent: frameRateMgr is nullptr.");
            return;
        }
        RS_LOGI("NotifyHgmConfigEvent: recive notify %{public}s, %{public}d", eventName.c_str(), state);
        if (eventName == "HGMCONFIG_HIGH_TEMP") {
            frameRateMgr->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
        } else if (eventName == "IA_DRAG_SLIDE") {
            frameRateMgr->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_DRAGSLIDE_SUFFIX);
        } else if (eventName == "IA_THROW_SLIDE") {
            frameRateMgr->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_THROWSLIDE_SUFFIX);
        }
    });
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
    HgmTaskHandleThread::Instance().PostTask([remotePid = remotePid_, id, expectedFrameRate] () {
        HgmConfigCallbackManager::GetInstance()->SyncXComponentExpectedFrameRateCallback(
            remotePid, id, expectedFrameRate);
    });
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventResponse(DataBaseRs info)
{
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), info]() -> void {
        RSJankStats::GetInstance().SetReportEventResponse(info);
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->SetForceRsDVsync(info.sceneId);
    };
#ifdef RS_ENABLE_GPU
    renderThread_.PostTask(task);
    RSUifirstManager::Instance().OnProcessEventResponse(info);
#endif
    RSUifirstFrameRateControl::Instance().SetAnimationStartInfo(info);
    UpdateAnimationOcclusionStatus(info.sceneId, true);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventComplete(DataBaseRs info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportEventComplete(info);
    };
#ifdef RS_ENABLE_GPU
    renderThread_.PostTask(task);
    RSUifirstManager::Instance().OnProcessEventComplete(info);
#endif
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventJankFrame(DataBaseRs info)
{
#ifdef RS_ENABLE_GPU
    bool isReportTaskDelayed = renderThread_.IsMainLooping();
    auto task = [info, isReportTaskDelayed]() -> void {
        RSJankStats::GetInstance().SetReportEventJankFrame(info, isReportTaskDelayed);
    };
    renderThread_.PostTask(task);
#endif
    RSUifirstFrameRateControl::Instance().SetAnimationEndInfo(info);
    UpdateAnimationOcclusionStatus(info.sceneId, false);
    return ERR_OK;
}

void RSClientToServiceConnection::UpdateAnimationOcclusionStatus(const std::string& sceneId, bool isStart)
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), sceneId, isStart]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->SetAnimationOcclusionInfo(sceneId, isStart);
    };
    mainThread_->PostTask(task);
}

void RSClientToServiceConnection::ReportRsSceneJankStart(AppInfo info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankStart(info);
    };
    renderThread_.PostTask(task);
}

void RSClientToServiceConnection::ReportRsSceneJankEnd(AppInfo info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankEnd(info);
    };
    renderThread_.PostTask(task);
}

ErrCode RSClientToServiceConnection::ReportGameStateData(GameStateData info)
{
    RS_LOGD("ReportGameStateData = %{public}s, uid = %{public}d, state = %{public}d, "
            "pid = %{public}d renderTid = %{public}d ",
        info.bundleName.c_str(), info.uid, info.state, info.pid, info.renderTid);

    FrameReport::GetInstance().SetGameScene(info.pid, info.state);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), id, isEnabled, selfDrawingType,
        dynamicHardwareEnable]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHardwareEnabled(isEnabled, selfDrawingType, dynamicHardwareEnable);
        }
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode)
{
    if (!mainThread_) {
        resCode = static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), id, needHidePrivacyContent]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHidePrivacyContent(needHidePrivacyContent);
        }
    };
    mainThread_->PostTask(task);
    resCode = static_cast<uint32_t>(RSInterfaceErrorCode::NO_ERROR);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetCacheEnabledForRotation(bool isEnabled)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [isEnabled]() {RSSystemProperties::SetCacheEnabledForRotation(isEnabled); };
    mainThread_->PostTask(task);
    return ERR_OK;
}

std::vector<ActiveDirtyRegionInfo> RSClientToServiceConnection::GetActiveDirtyRegionInfo()
{
#ifdef RS_ENABLE_GPU
    const auto& activeDirtyRegionInfos = GpuDirtyRegionCollection::GetInstance().GetActiveDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetActiveDirtyRegionInfo();
    return activeDirtyRegionInfos;
#else
    return {};
#endif
}

GlobalDirtyRegionInfo RSClientToServiceConnection::GetGlobalDirtyRegionInfo()
{
#ifdef RS_ENABLE_GPU
    const auto& globalDirtyRegionInfo = GpuDirtyRegionCollection::GetInstance().GetGlobalDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetGlobalDirtyRegionInfo();
    return globalDirtyRegionInfo;
#else
    return {};
#endif
}

LayerComposeInfo RSClientToServiceConnection::GetLayerComposeInfo()
{
    const auto& layerComposeInfo = LayerComposeCollection::GetInstance().GetLayerComposeInfo();
    LayerComposeCollection::GetInstance().ResetLayerComposeInfo();
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSClientToServiceConnection::GetHwcDisabledReasonInfo()
{
    return HwcDisabledReasonCollection::GetInstance().GetHwcDisabledReasonInfo();
}

ErrCode RSClientToServiceConnection::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    auto rsHdrCollection = RsHdrCollection::GetInstance();
    if (rsHdrCollection == nullptr) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = rsHdrCollection->GetHdrOnDuration();
    rsHdrCollection->ResetHdrOnDuration();
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetVmaCacheStatus(bool flag)
{
#ifdef RS_ENABLE_GPU
    renderThread_.SetVmaCacheStatus(flag);
#endif
    return ERR_OK;
}

#ifdef TP_FEATURE_ENABLE
ErrCode RSClientToServiceConnection::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    switch (tpFeatureConfigType) {
        case TpFeatureConfigType::DEFAULT_TP_FEATURE: {
            if (!TOUCH_SCREEN->IsSetFeatureConfigHandleValid()) {
                RS_LOGW("RSClientToServiceConnection::SetTpFeatureConfig: SetFeatureConfigHandl is nullptr");
                return ERR_INVALID_VALUE;
            }
            if (TOUCH_SCREEN->SetFeatureConfig(feature, config) < 0) {
                RS_LOGW("SetTpFeatureConfig: SetFeatureConfig failed");
                return ERR_INVALID_VALUE;
            }
            return ERR_OK;
        }
        case TpFeatureConfigType::AFT_TP_FEATURE: {
            if (!TOUCH_SCREEN->IsSetAftConfigHandleValid()) {
                RS_LOGW("RSClientToServiceConnection::SetTpFeatureConfig: SetAftConfigHandl is nullptr");
                return ERR_INVALID_VALUE;
            }
            if (TOUCH_SCREEN->SetAftConfig(config) < 0) {
                RS_LOGW("SetTpFeatureConfig: SetAftConfig failed");
                return ERR_INVALID_VALUE;
            }
            return ERR_OK;
        }
        default: {
            RS_LOGW("SetTpFeatureConfig: unknown TpFeatureConfigType: %" PRIu8"",
                static_cast<uint8_t>(tpFeatureConfigType));
            return ERR_INVALID_VALUE;
        }
    }
}
#endif

void RSClientToServiceConnection::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    if (isVirtualScreenUsingStatus) {
        EventInfo event = { "VOTER_VIRTUALDISPLAY", ADD_VOTE, OLED_60_HZ, OLED_60_HZ };
        NotifyRefreshRateEvent(event);
    } else {
        EventInfo event = { "VOTER_VIRTUALDISPLAY", REMOVE_VOTE };
        NotifyRefreshRateEvent(event);
    }
    return;
}

ErrCode RSClientToServiceConnection::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), isCurtainScreenOn]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t> pidList)
{
    if (!mainThread_ || pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToServiceConnection>(this), pidList]() {
            // don't use 'this' directly
            sptr<RSClientToServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            RSGpuDirtyCollector::GetInstance().SetSelfDrawingGpuDirtyPidList(pidList);
        }
    );
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    if (pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::RegisterUIExtensionCallback(uint64_t userId, sptr<RSIUIExtensionCallback> callback,
    bool unobscured)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterUIExtensionCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterUIExtensionCallback(remotePid_, userId, callback, unobscured);
    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::SetVirtualScreenStatus(ScreenId id,
    VirtualScreenStatus screenStatus, bool& success)
{
    if (!screenManager_) {
        success = false;
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_LOGD("SetVirtualScreenStatus ScreenId:%{public}" PRIu64 " screenStatus:%{public}d", id, screenStatus);
    success = screenManager_->SetVirtualScreenStatus(id, screenStatus);
    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::SetLayerTop(const std::string& nodeIdStr, bool isTop)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), nodeIdStr, isTop]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isTop](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                surfaceNode->SetLayerTop(isTop, false);
                return;
            }
        });
        // It can be displayed immediately after layer-top changed.
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetForceRefresh(const std::string& nodeIdStr, bool isForceRefresh)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), nodeIdStr, isForceRefresh]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
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

void RSClientToServiceConnection::SetFreeMultiWindowStatus(bool enable)
{
#ifdef RS_ENABLE_GPU
    if (mainThread_ == nullptr) {
        return;
    }
    auto task = [enable]() -> void {
        RSUifirstManager::Instance().SetFreeMultiWindowStatus(enable);
    };
    mainThread_->PostTask(task);
#endif
}

void RSClientToServiceConnection::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), nodeIdStr, isColorFollow]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isColorFollow](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                surfaceNode->SetColorFollow(isColorFollow);
                return;
            }
        });
        // It can be displayed immediately after layer-top changed
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
}

ErrCode RSClientToServiceConnection::NotifyScreenSwitched()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        RS_LOGE("NotifyScreenSwitched screenManager_ is nullptr");
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("NotifyScreenSwitched SetScreenSwitchStatus true");
    RS_TRACE_NAME_FMT("NotifyScreenSwitched");
    screenManager_->SetScreenSwitchStatus(true);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSelfDrawingNodeRectChangeCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }

    auto task = [pid = remotePid_, constraint, callback]() {
        SelfDrawingNodeMonitor::GetInstance().RegisterRectChangeCallback(pid, constraint, callback);
    };
    mainThread_->PostTask(task);
    return StatusCode::SUCCESS;
}

int32_t RSClientToServiceConnection::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [pid = remotePid_]() { SelfDrawingNodeMonitor::GetInstance().UnRegisterRectChangeCallback(pid); };
    mainThread_->PostTask(task);
    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::NotifyPageName(const std::string &packageName,
    const std::string &pageName, bool isEnter)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, packageName, pageName, isEnter]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->NotifyPageName(pid, packageName, pageName, isEnter);
        }
    });
    return StatusCode::SUCCESS;
}

bool RSClientToServiceConnection::GetHighContrastTextState()
{
    return RSBaseRenderEngine::IsHighContrastEnabled();
}

ErrCode RSClientToServiceConnection::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    auto task = [uniqueIdList, surfaceNameList, fps, reportTime]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    auto task = [uniqueIdList, surfaceNameList, fps]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AvcodecVideoGet(uint64_t uniqueId)
{
    auto task = [uniqueId]() -> void {
        RSJankStats::GetInstance().AvcodecVideoGet(uniqueId);
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}
 
ErrCode RSClientToServiceConnection::AvcodecVideoGetRecent()
{
    auto task = []() -> void {
        RSJankStats::GetInstance().AvcodecVideoGetRecent();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSClientToServiceConnection::SetOverlayDisplayMode(int32_t mode)
{
    RS_LOGI("RSClientToServiceConnection::SetOverlayDisplayMode: mode: [%{public}d]", mode);
    return RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(mode) == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif

ErrCode RSClientToServiceConnection::SetBehindWindowFilterEnabled(bool enabled)
{
    if (!mainThread_) {
        RS_LOGE("SetBehindWindowFilterEnabled mainThread_ is nullptr.");
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), enabled]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        if (RSSystemProperties::GetBehindWindowFilterEnabled() == enabled) {
            return;
        }
        RSSystemProperties::SetBehindWindowFilterEnabled(enabled);
        auto& nodeMap = connection->mainThread_->GetContext().GetNodeMap();
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
            connection->mainThread_->RequestNextVSync();
            connection->mainThread_->SetForceUpdateUniRenderFlag(true);
        }
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetBehindWindowFilterEnabled(bool& enabled)
{
    enabled = RSSystemProperties::GetBehindWindowFilterEnabled();
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetPidGpuMemoryInMB(pid_t pid, float& gpuMemInMB)
{
    MemorySnapshotInfo memorySnapshotInfo;

    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, memorySnapshotInfo);
    if (!ret) {
        RS_LOGE("RSClientToServiceConnection::GetPidGpuMemoryInMB fail to find pid!");
        return ERR_INVALID_VALUE;
    }
    gpuMemInMB = static_cast<float>(memorySnapshotInfo.gpuMemory) / MEM_BYTE_TO_MB;
    RS_LOGE("RSClientToServiceConnection::GetPidGpuMemoryInMB called, GetPidGpuMemoryInMB: %{public}f", gpuMemInMB);
    return ERR_OK;
}

RetCodeHrpService RSClientToServiceConnection::ProfilerServiceOpenFile(const HrpServiceDirInfo& dirInfo,
    const std::string& fileName, int32_t flags, int& outFd)
{
#ifdef RS_PROFILER_ENABLED
    if (fileName.length() == 0 || !HrpServiceValidDirOrFileName(fileName)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }

    return RSProfiler::HrpServiceOpenFile(dirInfo, fileName, flags, outFd);
#else
    outFd = -1;
    return RET_HRP_SERVICE_ERR_UNSUPPORTED;
#endif
}

RetCodeHrpService RSClientToServiceConnection::ProfilerServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
    uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles)
{
#ifdef RS_PROFILER_ENABLED
    return RSProfiler::HrpServicePopulateFiles(dirInfo, firstFileIndex, outFiles);
#else
    outFiles.clear();
    return RET_HRP_SERVICE_ERR_UNSUPPORTED;
#endif
}

bool RSClientToServiceConnection::ProfilerIsSecureScreen()
{
#ifdef RS_PROFILER_ENABLED
    if (!RSSystemProperties::GetProfilerEnabled()) {
        return false;
    }
    return RSProfiler::IsSecureScreen();
#else
    return false;
#endif
}

std::string RSClientToServiceConnection::GetBundleName(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pidToBundleMutex_);
    if (auto it = pidToBundleName_.find(pid); it != pidToBundleName_.end()) {
        return it->second;
    }

    static const auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
    if (appMgrClient == nullptr) {
        RS_LOGE("GetBundleName get appMgrClient fail.");
        return {};
    }

    std::string bundleName{};
    int32_t uid{0};
    int32_t ret = appMgrClient->GetBundleNameByPid(pid, bundleName, uid);
    if ((ret != ERR_OK) || bundleName.empty()) {
        RS_LOGE("GetBundleName failed, ret=%{public}d.", ret);
        return {};
    }

    pidToBundleName_.emplace(pid, bundleName);
    return bundleName;
}
} // namespace Rosen
} // namespace OHOS
