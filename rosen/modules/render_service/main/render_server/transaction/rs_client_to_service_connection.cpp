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
#include "rs_frame_report.h"
#include "rs_trace.h"
#include "rs_profiler.h"
//blur predict
#include "rs_frame_blur_predict.h"
#include "system/rs_system_parameters.h"

#include "command/rs_command_verify_helper.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_background_thread.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "display_engine/rs_luminance_control.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/capture/rs_ui_capture.h"
#include "feature/capture/rs_uni_ui_capture.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "feature/capture/rs_ui_capture_solo_task_parallel.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "info_collection/rs_hdr_collection.h"
#ifdef RS_ENABLE_GPU
#include "feature/uifirst/rs_sub_thread_manager.h"
#endif
#include "feature/uifirst/rs_uifirst_manager.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "memory/rs_canvas_dma_buffer_cache.h"
#endif
#include "memory/rs_memory_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_pointer_window_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
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
#include "graphic_feature_param_manager.h"

#ifdef TP_FEATURE_ENABLE
#include "screen_manager/touch_screen.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#if defined(RS_ENABLE_DVSYNC)
#include "dvsync.h"
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
    sptr<RSRenderServiceAgent> renderServiceAgent,
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent,
    RSMainThread* mainThread,
    sptr<RSScreenManagerAgent> screenManagerAgent,
    sptr<IRemoteObject> token,
    sptr<VSyncDistributor> distributor)
    : remotePid_(remotePid),
      renderServiceAgent_(renderServiceAgent),
      renderProcessManagerAgent_(renderProcessManagerAgent),
      mainThread_(mainThread),
#ifdef RS_ENABLE_GPU
      renderThread_(RSUniRenderThread::Instance()),
#endif
      screenManagerAgent_(screenManagerAgent),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      applicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this)),
      appVSyncDistributor_(distributor)
{
    if (token_ == nullptr || !token_->AddDeathRecipient(connDeathRecipient_)) {
        RS_LOGW("RSClientToServiceConnection: Failed to set death recipient.");
    }
    if (renderServiceAgent_ == nullptr) {
        RS_LOGE("RSClientToServiceConnection: renderServiceAgent_ is nullptr");
    } else {
        hgmContext_ = renderServiceAgent_->GetHgmContext();
    }
    if (mainThread_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: mainThread_ is nullptr");
    }
    if (screenManagerAgent_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: screenManagerAgent_ is nullptr");
    }

    if (appVSyncDistributor_ == nullptr) {
        RS_LOGW("RSClientToServiceConnection: appVSyncDistributor_ is nullptr");
    }
}

RSClientToServiceConnection::~RSClientToServiceConnection() noexcept
{
    RS_LOGI("~RSClientToServiceConnection remotePid:%{public}d", remotePid_);
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

    RSRenderNodeGC::Instance().ReleaseFromTree(AppExecFwk::EventQueue::Priority::HIGH);
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

    {
        std::lock_guard<std::mutex> lock(pidToBundleMutex_);
        pidToBundleName_.clear();
    }
    if (toDelete) {
        auto token = iface_cast<RSIConnectionToken>(GetToken());
        renderServiceAgent_->RemoveToken(token);
    }

    RS_LOGD("CleanAll() end.");
    RS_TRACE_NAME("RSClientToServiceConnection CleanAll end, remotePid: " + std::to_string(remotePid_));
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
}

ErrCode RSClientToServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
}

ErrCode RSClientToServiceConnection::GetUniRenderEnabled(bool& enable)
{
    enable = RSUniRenderJudgement::IsUniRender();
    return  ERR_OK;
}

ErrCode RSClientToServiceConnection::CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn,
                                                         const std::string& name,
                                                         const sptr<VSyncIConnectionToken>& token,
                                                         VSyncConnParam vsyncConnParam)
{
    if (mainThread_ == nullptr || appVSyncDistributor_ == nullptr) {
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    uint64_t id = vsyncConnParam.id;
    NodeId windowNodeId = vsyncConnParam.windowNodeId;
    if (vsyncConnParam.fromXcomponent) {
        GetSurfaceRootNodeId(windowNodeId);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(appVSyncDistributor_, name, token->AsObject(), 0, windowNodeId);
    if (ExtractPid(id) == remotePid_) {
        if (hgmContext_ != nullptr) {
            renderServiceAgent_->ScheduleTask([hgmContext = hgmContext_, name, id, windowNodeId] {
                hgmContext->CreateFrameRateLinker(name, id, windowNodeId);
            }).wait();
        }
        conn->id_ = id;
        RS_LOGD("CreateVSyncConnection connect id: %{public}" PRIu64, id);
    }
    auto ret = appVSyncDistributor_->AddConnection(conn, windowNodeId);
    if (ret != VSYNC_ERROR_OK) {
        vsyncConn = nullptr;
        UnregisterFrameRateLinker(id);
        return ERR_INVALID_VALUE;
    }
    vsyncConn = conn;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        std::vector<PixelMapInfo> pixelMapInfoVectorTmp;
        int32_t repCodeTmp;
        if (conn->GetPixelMapByProcessId(pixelMapInfoVectorTmp, pid, repCodeTmp) != ERR_OK || repCodeTmp != SUCCESS) {
            RS_LOGE("RSMultiRenderProcessManager::GetPixelMapByProcessId a connection failed!");
            repCode = INVALID_ARGUMENTS;
            return ERR_INVALID_VALUE;
        }
        if (pixelMapInfoVectorTmp.size() != 0) {
            for (auto pixelMapInfo : pixelMapInfoVectorTmp) {
                pixelMapInfoVector.emplace_back(pixelMapInfo);
            }
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
    if (transformType == GRAPHIC_ROTATE_90) {
        return 90.0f;
    } else if (transformType == GRAPHIC_ROTATE_180) {
        return 180.0f;
    } else if (transformType == GRAPHIC_ROTATE_270) {
        return 270.0f;
    }
    return 0.0f;
}

ErrCode RSClientToServiceConnection::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode res = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        ErrCode res = conn->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
        if (pixelMap != nullptr) {
            res = ERR_OK;
            break;
        }
        res = ERR_INVALID_VALUE;
        RS_LOGW("%{public}s serviceToRenderConns is pixelMap is nullptr", __func__);
    }
    return res;
}

ErrCode RSClientToServiceConnection::SetWatermark(pid_t callingPid, const std::string& name,
    std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        bool successTmp = true;
        if (conn->SetWatermark(callingPid, name, watermark, successTmp) != ERR_OK || successTmp != true) {
            RS_LOGE("RSMultiRenderProcessManager::SetWatermark a connection failed!");
            success = false;
            return ERR_INVALID_VALUE;
        }
        success &= successTmp;
    }
    return ERR_OK;
}

uint32_t RSClientToServiceConnection::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType)
{
    // if (!mainThread_) {
    //     return WATER_MARK_IPC_ERROR;
    // }
    // auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
    //     RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
    //     "::SET_SURFACE_WATERMARK");
    // uint32_t res =  SurfaceWatermarkStatusCode::WATER_MARK_RS_CONNECTION_ERROR;
    // auto task = [weakThis = wptr<RSClientToServiceConnection>(this), &name, &nodeIdList, &watermark, &watermarkType,
    //     pid, isSystemCalling, &res]() -> void {
    //     sptr<RSClientToServiceConnection> connection = weakThis.promote();
    //     if (connection == nullptr || connection->mainThread_ == nullptr) {
    //         return;
    //     }
    //     res = connection->mainThread_->SetSurfaceWatermark(pid, name, watermark,
    //         nodeIdList, watermarkType, isSystemCalling);
    // };
    // mainThread_->PostSyncTask(task);
    // return res;
}
    
void RSClientToServiceConnection::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
    const std::vector<NodeId> &nodeIdList)
{
    // if (!mainThread_) {
    //     return;
    // }
    // auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
    //     RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
    //     "::CLEAR_SURFACE_WATERMARK_FOR_NODES");
    // auto task = [weakThis = wptr<RSClientToServiceConnection>(this), &name, pid,
    //     &nodeIdList, isSystemCalling]() -> void {
    //     sptr<RSClientToServiceConnection> connection = weakThis.promote();
    //     if (connection == nullptr || connection->mainThread_ == nullptr) {
    //         return;
    //     }
    //     connection->mainThread_->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, isSystemCalling);
    // };
    // mainThread_->PostTask(task);
}
    
void RSClientToServiceConnection::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    // if (!mainThread_) {
    //     return;
    // }
    // auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
    //     RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
    //     "::CLEAR_SURFACE_WATERMARK");
    // auto task = [weakThis = wptr<RSClientToServiceConnection>(this), &name, pid, isSystemCalling]() -> void {
    //     sptr<RSClientToServiceConnection> connection = weakThis.promote();
    //     if (connection == nullptr || connection->mainThread_ == nullptr) {
    //         return;
    //     }
    //     connection->mainThread_->ClearSurfaceWatermark(pid, name, isSystemCalling);
    // };
    // mainThread_->PostTask(task);
}

ErrCode RSClientToServiceConnection::GetDefaultScreenId(uint64_t& screenId)
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGW("%{public}s screenManagerAgent_ is nullptr", __func__);
        return -1; // ERR_INVALID_OPERATION
    }
    return screenManagerAgent_->GetDefaultScreenId(screenId);
}

ErrCode RSClientToServiceConnection::GetActiveScreenId(uint64_t& screenId)
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGW("%{public}s screenManagerAgent_ is nullptr", __func__);
        return -1;
    }
    return screenManagerAgent_->GetActiveScreenId(screenId);
}

std::vector<ScreenId> RSClientToServiceConnection::GetAllScreenIds()
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGW("%{public}s screenManagerAgent_ is nullptr", __func__);
        return {}; // chenke fix
    }
    return screenManagerAgent_->GetAllScreenIds();
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
    if (screenManagerAgent_ == nullptr) {
        RS_LOGW("%{public}s screenManagerAgent_ is nullptr", __func__);
        return INVALID_SCREEN_ID;
    }
    auto res = screenManagerAgent_->CreateVirtualScreen(
        name, width, height, surface, associatedScreenId, flags, whiteList);
    if (res == INVALID_SCREEN_ID) {
        return res;
    }
    if (surface != nullptr) {
        EventInfo event = { "VOTER_VIRTUALDISPLAY", ADD_VOTE, OLED_60_HZ, OLED_60_HZ, name };
        NotifyRefreshRateEvent(event);
        ROSEN_LOGI("%{public}s vote 60hz", __func__);
    }
    return res;
}

int32_t RSClientToServiceConnection::SetVirtualScreenBlackList(ScreenId id, std::vector<uint64_t>& blackListVector)
{
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetVirtualScreenBlackList(id, blackListVector);
}

ErrCode RSClientToServiceConnection::SetVirtualScreenTypeBlackList(
    ScreenId id, std::vector<uint8_t>& typeBlackListVector, int32_t& repCode)
{
    if (screenManagerAgent_ == nullptr) {
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManagerAgent_->SetVirtualScreenTypeBlackList(id, typeBlackListVector);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AddVirtualScreenBlackList(
    ScreenId id, std::vector<uint64_t>& blackListVector, int32_t& repCode)
{
    if (screenManagerAgent_ == nullptr) {
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManagerAgent_->AddVirtualScreenBlackList(id, blackListVector);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::RemoveVirtualScreenBlackList(
    ScreenId id, std::vector<uint64_t>& blackListVector, int32_t& repCode)
{
    if (blackListVector.empty()) {
        RS_LOGW("RemoveVirtualScreenBlackList blackList is empty.");
        repCode = StatusCode::BLACKLIST_IS_EMPTY;
        return ERR_INVALID_VALUE;
    }
    if (screenManagerAgent_ == nullptr) {
        repCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    repCode = screenManagerAgent_->RemoveVirtualScreenBlackList(id, blackListVector);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<uint64_t>& securityExemptionList)
{
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSClientToServiceConnection::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetScreenSecurityMask(id, securityMask);
}

int32_t RSClientToServiceConnection::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSClientToServiceConnection::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSClientToServiceConnection::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenManagerAgent_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetVirtualScreenSurface(id, surface);
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
    if (screenManagerAgent_ == nullptr) {
        return;
    }
    screenManagerAgent_->RemoveVirtualScreen(id);
    EventInfo event = { "VOTER_VIRTUALDISPLAY", REMOVE_VOTE };
    NotifyRefreshRateEvent(event);
}

int32_t RSClientToServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s screenManagerAgent_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    // update
    int32_t status;
    renderServiceAgent_->ScheduleTask([this, callback, &status]() {
        status = screenManagerAgent_->SetScreenChangeCallback(callback);
    }).wait();
    return status;
}

int32_t RSClientToServiceConnection::SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback)
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s: screenManagerAgent_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    // update
    int32_t status = screenManagerAgent_->SetScreenSwitchingNotifyCallback(callback);
    return status;
}

int32_t RSClientToServiceConnection::SetBrightnessInfoChangeCallback(sptr<RSIBrightnessInfoChangeCallback> callback)
{
    // if (mainThread_ == nullptr) {
    //     return INVALID_ARGUMENTS;
    // }
    // auto task = [this, &callback]() {
    //     auto& context = mainThread_->GetContext();
    //     return context.SetBrightnessInfoChangeCallback(remotePid_, callback);
    // };
    // return mainThread_->ScheduleTask(task).get();
}

void RSClientToServiceConnection::CleanBrightnessInfoChangeCallbacks() noexcept
{
    // if (mainThread_ == nullptr) {
    //     return;
    // }
    // auto& context = mainThread_->GetContext();
    // context.SetBrightnessInfoChangeCallback(remotePid_, nullptr);
}

int32_t RSClientToServiceConnection::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    // brightnessInfo = RSLuminanceControl::Get().GetBrightnessInfo(screenId);
    // return StatusCode::SUCCESS;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSClientToServiceConnection::CleanCanvasCallbacksAndPendingBuffer() noexcept
{
    // auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    // bufferCache.RegisterCanvasCallback(remotePid_, nullptr);
    // bufferCache.ClearPendingBufferByPid(remotePid_);
}
#endif

void RSClientToServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    if (screenManagerAgent_ == nullptr) {
        return;
    }
    screenManagerAgent_->SetScreenActiveMode(id, modeId);
}

void RSClientToServiceConnection::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->SetScreenRefreshRate(id, sceneId, rate);
}

void RSClientToServiceConnection::SetRefreshRateMode(int32_t refreshRateMode)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->SetRefreshRateMode(refreshRateMode);
}

void RSClientToServiceConnection::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
    if (renderServiceAgent_ == nullptr || hgmContext_ == nullptr) {
        RS_LOGE("%{public}s renderServiceAgent_ or hgmContext_ is nullptr", __func__);
        return;
    }
    renderServiceAgent_->ScheduleTask([this, id, range, animatorExpectedFrameRate] {
        hgmContext_->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    }).wait();
}

void RSClientToServiceConnection::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    renderServiceAgent_->ScheduleTask([hgmContext = hgmContext_, id] {
        hgmContext->UnregisterFrameRateLinker(id);
    }).wait();
}

uint32_t RSClientToServiceConnection::GetScreenCurrentRefreshRate(ScreenId id)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return 0;
    }
    return hgmContext_->GetScreenCurrentRefreshRate(id);
}

std::vector<int32_t> RSClientToServiceConnection::GetScreenSupportedRefreshRates(ScreenId id)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return {};
    }
    return hgmContext_->GetScreenSupportedRefreshRates(id);
}

ErrCode RSClientToServiceConnection::GetShowRefreshRateEnabled(bool& enable)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    return serviceToRenderConns[0]->GetShowRefreshRateEnabled(enable);
}

void RSClientToServiceConnection::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetShowRefreshRateEnabled(enabled, type);
    }
}

uint32_t RSClientToServiceConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return 0;
    }
    auto screen = HgmCore::Instance().GetScreen(screenId);
    if (screen == nullptr) {
        screenId = HgmCore::Instance().GetActiveScreenId();
    }
    auto conn = renderProcessManagerAgent_->GetServiceToRenderConn(screenId);
    if (conn == nullptr) {
        RS_LOGE("%{public}s serviceToRenderConn is nullptr", __func__);
        return 0;
    }
    return conn->GetRealtimeRefreshRate(screenId);
}

ErrCode RSClientToServiceConnection::GetRefreshInfo(pid_t pid, std::string& enable)
{
    if (renderServiceAgent_ == nullptr) {
        enable = "";
        RS_LOGE("%{public}s renderServiceAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string surfaceName = RSSurfaceFpsManager::GetInstance().GetSelfDrawSurfaceNameByPid(pid);
    if (surfaceName.empty()) {
        enable = "";
        return ERR_INVALID_VALUE;
    }
    std::string dumpString;
    renderServiceAgent_->FpsDump(dumpString, surfaceName);
    enable = dumpString;
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetRefreshInfoToSP(NodeId id, std::string& enable)
{
    if (renderServiceAgent_ == nullptr) {
        enable = "";
        RS_LOGE("%{public}s renderServiceAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string dumpString;
    renderServiceAgent_->GetRefreshInfoToSP(dumpString, id);
    enable = dumpString;
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetCurrentRefreshRateMode()
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return HGM_REFRESHRATE_MODE_AUTO;
    }
    return hgmContext_->GetCurrentRefreshRateMode();
}


int32_t RSClientToServiceConnection::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManagerAgent_->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSClientToServiceConnection::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return StatusCode::SCREEN_NOT_FOUND;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->SetVirtualScreenResolution(id, width, height); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->SetVirtualScreenResolution(id, width, height); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    // if (!screenManager_) {
    //     return StatusCode::SCREEN_MANAGER_NULL;
    // }
    // return screenManager_->SetRogScreenResolution(id, width, height);
}

int32_t RSClientToServiceConnection::GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height)
{
    // if (!screenManager_) {
    //     return StatusCode::SCREEN_MANAGER_NULL;
    // }
    // return screenManager_->GetRogScreenResolution(id, width, height);
}

ErrCode RSClientToServiceConnection::MarkPowerOffNeedProcessOneFrame()
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    screenManagerAgent_->MarkPowerOffNeedProcessOneFrame();
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ForceRefreshOneFrameWithNextVSync()
{
    // if (!mainThread_) {
    //     RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
    //     return ERR_INVALID_VALUE;
    // }

    // auto task = [weakThis = wptr<RSClientToServiceConnection>(this)]() -> void {
    //     sptr<RSClientToServiceConnection> connection = weakThis.promote();
    //     if (connection == nullptr || connection->mainThread_ == nullptr) {
    //         return;
    //     }

    //     RS_LOGI("ForceRefreshOneFrameWithNextVSync, setDirtyflag, forceRefresh in mainThread");
    //     connection->mainThread_->SetDirtyFlag();
    //     connection->mainThread_->RequestNextVSync();
    // };
    // mainThread_->PostTask(task);
    // return ERR_OK;
}

ErrCode RSClientToServiceConnection::RepaintEverything()
{
    ErrCode errCode = ERR_INVALID_OPERATION;
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is null", __func__);
        errCode = ERR_INVALID_VALUE;
        return errCode;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    int32_t connSize = serviceToRenderConns.size();
    if (connSize == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return errCode;
    }
    
    int32_t successCount = 0;
    for (auto& conn : serviceToRenderConns) {
        ErrCode code = conn->RepaintEverything();
        if (code == ERR_OK) {
            successCount++;
        }
    }

    if (successCount == connSize) {
        errCode = ERR_OK;
        RS_LOGI("%{public}s call repaintEverything, errCode: %{public}d", __func__, errCode);
    } else {
        RS_LOGE("%{public}s:  %{public}d conn call RepaintEverything failed", __func__, (connSize - successCount));
    }

    return errCode;
}

void RSClientToServiceConnection::DisablePowerOffRenderControl(ScreenId id)
{
    if (screenManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s: screenManagerAgent_ is nullptr", __func__);
        return;
    }
    screenManagerAgent_->DisablePowerOffRenderControl(id);
}

void RSClientToServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (screenManagerAgent_ == nullptr || renderServiceAgent_ == nullptr) {
        RS_LOGE("%{public}s screenManager or renderServiceAgent is null, id: %{public}" PRIu64, __func__, id);
        return;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // RSHardwareThread::Instance().ScheduleTask([=]() {
        //     screenManagerAgent_->SetScreenPowerStatus(id, status);
        // }).wait();
        // if (renderProcessManagerAgent_ == nullptr) {
        //     RS_LOGE("%{public}s renderProcessManagerAgent_ is null", __func__);
        // } else {
        //     auto serviceToRenderConn = renderProcessManagerAgent_->GetServiceToRenderConn(id);
        //     if (!serviceToRenderConn) {
        //         RS_LOGE("%{public}s serviceToRenderConn is nullptr", __func__);
        //     } else {
        //         serviceToRenderConns->SetDiscardJankFrames(true);
        //     }
        // }
        // HgmTaskHandleThread::Instance().PostTask([id, status]() {
        //     HgmCore::Instance().NotifyScreenPowerStatus(id, status);
        // });
#endif
    } else {
        renderServiceAgent_->ScheduleTask(
            [=]() { screenManagerAgent_->SetScreenPowerStatus(id, status); }).wait();
    }
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
    if (screenManagerAgent_ == nullptr) {
        return RSVirtualScreenResolution();
    }
    return screenManagerAgent_->GetVirtualScreenResolution(id);;
}

ErrCode RSClientToServiceConnection::GetScreenActiveMode(uint64_t id, RSScreenModeInfo& screenModeInfo)
{
    if (!screenManagerAgent_) {
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // RSHardwareThread::Instance().ScheduleTask(
            // [=, &screenModeInfo]() { return screenManagerAgent_->GetScreenActiveMode(id, screenModeInfo); }).wait();
#else
        return screenModeInfo;
#endif
    } else if (mainThread_ != nullptr) {
        mainThread_->ScheduleTask(
            [=, &screenModeInfo]() { return screenManagerAgent_->GetScreenActiveMode(id, screenModeInfo); }).wait();
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        float cpuMemSizeTmp = 0.f;
        float gpuMemSizeTmp = 0.f;
        if (conn->GetTotalAppMemSize(cpuMemSizeTmp, gpuMemSizeTmp) != ERR_OK) {
            RS_LOGE("RSMultiRenderProcessManager::GetTotalAppMemSize a connection failed!");
            return ERR_INVALID_VALUE;
        }
        cpuMemSize+=cpuMemSizeTmp;
        gpuMemSize+=gpuMemSizeTmp;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode code = ERR_INVALID_VALUE;
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return code;
    }
    bool enable;
    if (GetUniRenderEnabled(enable) == ERR_OK && enable) {
        MemoryGraphic memoryGraphicTemp;
        for (auto conn : serviceToRenderConns) {
            if (conn->GetMemoryGraphic(pid, memoryGraphicTemp) == ERR_OK) {
                memoryGraphic += memoryGraphicTemp;
                code = ERR_OK;
            }
        }
        return code;
    } else {
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToServiceConnection::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }

    auto firstServiceToRenderConn = serviceToRenderConns.front();
    std::vector<MemoryGraphic> firstMemoryGraphicsTmp;
    if (firstServiceToRenderConn->GetMemoryGraphics(firstMemoryGraphicsTmp) != ERR_OK) {
        RS_LOGE("RSMultiRenderProcessManager::GetMemoryGraphics first connection failed!");
        return ERR_INVALID_VALUE;
    }
    size_t size = firstMemoryGraphicsTmp.size();
    memoryGraphics.resize(size);

    for (auto conn : serviceToRenderConns) {
        std::vector<MemoryGraphic> memoryGraphicsTmp;
        if (conn->GetMemoryGraphics(memoryGraphicsTmp) != ERR_OK || memoryGraphicsTmp.size() != size) {
            RS_LOGE("RSMultiRenderProcessManager::GetMemoryGraphics a connection failed!");
            return ERR_INVALID_VALUE;
        }
        for (size_t index = 0; index < size; index++) {
            memoryGraphics[index] += memoryGraphicsTmp[index];
        }
    }
    return ERR_OK;
}

std::vector<RSScreenModeInfo> RSClientToServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    if (!screenManagerAgent_) {
        return std::vector<RSScreenModeInfo>();
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return {};
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->GetScreenSupportedModes(id); }).get();
#else
        return std::vector<RSScreenModeInfo>();
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->GetScreenSupportedModes(id); }).get();
    } else {
        return std::vector<RSScreenModeInfo>();
    }
}

RSScreenCapability RSClientToServiceConnection::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    if (!screenManagerAgent_) {
        return screenCapability;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return screenCapability;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->GetScreenCapability(id); }).get();
#else
        return screenCapability;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->GetScreenCapability(id); }).get();
    } else {
        return screenCapability;
    }
}

ErrCode RSClientToServiceConnection::GetScreenPowerStatus(uint64_t screenId, uint32_t& status)
{
    if (!screenManagerAgent_) {
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // status = RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->GetScreenPowerStatus(screenId); }).get();
#else
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
#endif
    } else if (mainThread_ != nullptr) {
        status = mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->GetScreenPowerStatus(screenId); }).get();
    } else {
        status = ScreenPowerStatus::INVALID_POWER_STATUS;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

RSScreenData RSClientToServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    if (!screenManagerAgent_) {
        return screenData;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return screenData;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->GetScreenData(id); }).get();
#else
        return screenData;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->GetScreenData(id); }).get();
    } else {
        return screenData;
    }
}

ErrCode RSClientToServiceConnection::GetScreenBacklight(uint64_t id, int32_t& level)
{
    if (!screenManagerAgent_) {
        level = INVALID_BACKLIGHT_VALUE;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // level = RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->GetScreenBacklight(id); }).get();
#else
        level = INVALID_BACKLIGHT_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        level = mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->GetScreenBacklight(id); }).get();
    } else {
        level = INVALID_BACKLIGHT_VALUE;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    if (!screenManagerAgent_) {
        RS_LOGE("%{public}s screenManagerAgent_ is nullptr.", __func__);
        return;
    }

    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        screenManagerAgent_->SetScreenBacklight(id, level);
#endif
    } else if (mainThread_ != nullptr) {
        mainThread_->ScheduleTask(
            [=]() { screenManagerAgent_->SetScreenBacklight(id, level); }).wait();
    }
}

int32_t RSClientToServiceConnection::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSClientToServiceConnection::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return StatusCode::SCREEN_NOT_FOUND;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=, &keys]() { return screenManagerAgent_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &keys]() { return screenManagerAgent_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->GetScreenColorGamut(id, mode);
}

int32_t RSClientToServiceConnection::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetScreenColorGamut(id, modeIdx);
}

int32_t RSClientToServiceConnection::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return StatusCode::SCREEN_NOT_FOUND;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->SetScreenGamutMap(id, mode); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->SetScreenGamutMap(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->SetScreenCorrection(id, screenRotation);
}

bool RSClientToServiceConnection::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return false;
    }
    return screenManagerAgent_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSClientToServiceConnection::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManagerAgent_->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

bool RSClientToServiceConnection::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return false;
    }
    return screenManagerAgent_->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

int32_t RSClientToServiceConnection::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return StatusCode::SCREEN_NOT_FOUND;
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [=, &mode]() { return screenManagerAgent_->GetScreenGamutMap(id, mode); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManagerAgent_->GetScreenGamutMap(id, mode); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSClientToServiceConnection::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->GetScreenHDRCapability(id, screenHdrCapability);
}

ErrCode RSClientToServiceConnection::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetPixelFormat(id, pixelFormat);
    return ERR_OK;
}


ErrCode RSClientToServiceConnection::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    if (pixelFormat < 0 ||
        (pixelFormat >= GRAPHIC_PIXEL_FMT_END_OF_VALID && pixelFormat != GRAPHIC_PIXEL_FMT_VENDER_MASK)) {
        resCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->SetPixelFormat(id, pixelFormat);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetScreenSupportedHDRFormats(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetScreenSupportedHDRFormats(id, hdrFormats);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetScreenHDRFormat(id, hdrFormat);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->SetScreenHDRFormat(id, modeIdx);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetScreenSupportedColorSpaces(id, colorSpaces);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetScreenColorSpace(id, colorSpace);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->GetScreenColorSpace(id, colorSpace);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    resCode = screenManagerAgent_->SetScreenColorSpace(id, colorSpace);
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->GetScreenType(id, screenType);
}

bool RSClientToServiceConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    RS_LOGI("reg typeface, pid[%{public}d], familyname:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSClientToServiceConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return false;
    }

    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);

    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return true;
    }
    bool result = true;
    for (auto conn : serviceToRenderConns) {
        result &= conn->RegisterTypeface(globalUniqueId, typeface);
    }
    return result;
}
    
bool RSClientToServiceConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return false;
    }

    RS_LOGW("%{public}s: pid[%{public}d], uniqueId:%{puiblic}u", __func__,
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));

    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);

    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return true;
    }
    bool result = true;
    for (auto conn : serviceToRenderConns) {
        result &= conn->UnRegisterTypeface(globalUniqueId);
    }
    return result;
}

int32_t RSClientToServiceConnection::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManagerAgent_->GetDisplayIdentificationData(id, outPort, edidData);
}

ErrCode RSClientToServiceConnection::SetScreenSkipFrameInterval(uint64_t id, uint32_t skipFrameInterval, int32_t& resCode)
{
    if (!screenManagerAgent_) {
        resCode = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // resCode = RSHardwareThread::Instance().ScheduleTask(
        //     [=]() { return screenManagerAgent_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
#else
        resCode = StatusCode::SCREEN_NOT_FOUND;
#endif
    } else {
        if (!mainThread_) {
            resCode = StatusCode::INVALID_ARGUMENTS;
            return ERR_INVALID_VALUE;
        }
        resCode = mainThread_->ScheduleTask(
            [=]() { return screenManagerAgent_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal)
{
    if (!screenManagerAgent_) {
        retVal = StatusCode::SCREEN_NOT_FOUND;
        return ERR_INVALID_VALUE;
    }
    retVal = screenManagerAgent_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
    return ERR_OK;
}


ErrCode RSClientToServiceConnection::SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode)
{
    if (!screenManagerAgent_) {
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
    auto task = [weakScreenManager = wptr<RSScreenManager>(screenManagerAgent_), id, dstActiveRect]() -> void {
        sptr<RSScreenManager> screenManager = weakScreenManager.promote();
        if (!screenManager) {
            return;
        }
        screenManager->SetScreenActiveRect(id, dstActiveRect);
    };
    mainThread_->ScheduleTask(task).wait();

    HgmTaskHandleThread::Instance().PostTask([id, dstActiveRect]() {
        HgmCore::Instance().NotifyScreenRectFrameRateChange(id, dstActiveRect);
    });
    repCode = StatusCode::SUCCESS;
    return ERR_OK;
}

void RSClientToServiceConnection::SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    if (!screenManagerAgent_) {
        return;
    }
    screenManagerAgent_->SetScreenOffset(id, offsetX, offsetY);
}
int32_t RSClientToServiceConnection::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }

    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (callback == nullptr) {
        RS_LOGE("%{public}s null callback, failed", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        int32_t retTmp = conn->RegisterOcclusionChangeCallback(remotePid_, callback);
        ret = (ret != ERR_OK) ? ret : retTmp;
    }
    return ret;
}

int32_t RSClientToServiceConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (callback == nullptr) {
        RS_LOGE("%{public}s null callback, failed", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        int32_t retTmp = conn->RegisterSurfaceOcclusionChangeCallback(id, remotePid_, callback, partitionPoints);
        ret = (ret != ERR_OK) ? ret : retTmp;
    }
    return ret;
}

int32_t RSClientToServiceConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        int32_t retTmp = conn->UnRegisterSurfaceOcclusionChangeCallback(id);
        ret = (ret != ERR_OK) ? ret : retTmp;
    }
    return ret;
}

int32_t RSClientToServiceConnection::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return -1;
    }
    return hgmContext_->RegisterHgmConfigChangeCallback(remotePid_, callback);
}

int32_t RSClientToServiceConnection::RegisterHgmRefreshRateModeChangeCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return -1;
    }
    return hgmContext_->RegisterHgmRefreshRateModeChangeCallback(remotePid_, callback);
}

int32_t RSClientToServiceConnection::RegisterHgmRefreshRateUpdateCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return -1;
    }
    return hgmContext_->RegisterHgmRefreshRateUpdateCallback(remotePid_, callback);
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
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return -1;
    }
    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::SetAppWindowNum(uint32_t num)
{
    // 遗漏
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToServiceConnection>(this), num]() -> void {
        sptr<RSClientToServiceConnection> connection = weakThis.promote();
        if (!connection || !connection->mainThread_) {
            return;
        }
        connection->mainThread_->SetAppWindowNum(num);
    };
    mainThread_->PostTask(task);

    return ERR_OK; 
}

void RSClientToServiceConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ShowWatermark(watermarkImg, isShow);
    }
}

int32_t RSClientToServiceConnection::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManagerAgent_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        // return RSHardwareThread::Instance().ScheduleTask(
        //     [weakThis = wptr<RSClientToServiceConnection>(this), id, width, height]() -> int32_t {
        //         sptr<RSClientToServiceConnection> connection = weakThis.promote();
        //         if (connection == nullptr || connection->screenManagerAgent_ == nullptr) {
        //             return RS_CONNECTION_ERROR;
        //         }
        //         return connection->screenManagerAgent_->ResizeVirtualScreen(id, width, height);
        //     }
        // ).get();
        return StatusCode::SCREEN_NOT_FOUND;
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [weakThis = wptr<RSClientToServiceConnection>(this), id, width, height]() -> int32_t {
                sptr<RSClientToServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManagerAgent_ == nullptr) {
                    return RS_CONNECTION_ERROR;
                }
                return connection->screenManagerAgent_->ResizeVirtualScreen(id, width, height);
            }
        ).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

ErrCode RSClientToServiceConnection::ReportJankStats()
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportJankStats();
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return hgmContext_->NotifyLightFactorStatus(remotePid_, lightFactorStatus);
}

void RSClientToServiceConnection::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    if (renderServiceAgent_ == nullptr || renderProcessManagerAgent_ == nullptr || screenManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderServiceAgent_ or renderProcessManagerAgent_ "
            "or screenManagerAgent_ is nullptr", __func__);
        return;
    }
    auto activeScreenId = HgmCore::Instance().GetActiveScreenId();
    auto serviceToRenderConn = renderProcessManagerAgent_->GetServiceToRenderConn(activeScreenId);
    if (serviceToRenderConn) {
        serviceToRenderConn->NotifyPackageEvnet(listSize, packageList);
    }

    if (hgmContext_ != nullptr) {
        hgmContext_->NotifyPackageEvent(remotePid_, packageList);
    }
}

ErrCode RSClientToServiceConnection::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return hgmContext_->NotifyAppStrategyConfigChangeEvent(remotePid_, pkgName, newConfig);
}

void RSClientToServiceConnection::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->NotifyRefreshRateEvent(remotePid_, eventInfo);
}


void RSClientToServiceConnection::SetWindowExpectedRefreshRate(
    const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->SetWindowExpectedRefreshRate(remotePid_, eventInfos);
}

void RSClientToServiceConnection::SetWindowExpectedRefreshRate(
    const std::unordered_map<std::string, EventInfo>& eventInfos)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->SetWindowExpectedRefreshRate(remotePid_, eventInfos);
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
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return false;
    }
    return hgmContext_->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount, appVSyncDistributor_);
}

ErrCode RSClientToServiceConnection::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    if (renderServiceAgent_ == nullptr) {
        RS_LOGE("%{public}s renderServiceAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    renderServiceAgent_->HandleTouchEvent(touchStatus, touchCnt);

    if (hgmContext_ != nullptr) {
        hgmContext_->HandleTouchEvent(remotePid_, touchStatus, touchCnt);
    }

    return ERR_OK;
}

void RSClientToServiceConnection::NotifyDynamicModeEvent(bool enableDynamicModeEvent)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return;
    }
    hgmContext_->NotifyDynamicModeEvent(enableDynamicModeEvent);
}

ErrCode RSClientToServiceConnection::NotifyHgmConfigEvent(const std::string& eventName, bool state)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    hgmContext_->NotifyHgmConfigEvent(eventName, state);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
    if (hgmContext_ == nullptr) {
        RS_LOGE("%{public}s hgmContext_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    
    hgmContext_->NotifyXComponentExpectedFrameRate(remotePid_, id, expectedFrameRate);
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventResponse(DataBaseRs info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportEventResponse(info);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventComplete(DataBaseRs info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportEventComplete(info);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportEventJankFrame(DataBaseRs info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportEventJankFrame(info);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportRsSceneJankStart(AppInfo info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportRsSceneJankStart(info);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::ReportRsSceneJankEnd(AppInfo info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportRsSceneJankEnd(info);
    }
    return ERR_OK;
}

void RSClientToServiceConnection::ReportGameStateData(GameStateData info)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->ReportGameStateData(info);
    }
}


ErrCode RSClientToServiceConnection::SetCacheEnabledForRotation(bool isEnabled)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RSSystemProperties::SetCacheEnabledForRotation(isEnabled);
    return ERR_OK;
}

std::vector<ActiveDirtyRegionInfo> RSClientToServiceConnection::GetActiveDirtyRegionInfo()
{
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    if (renderProcessManagerAgent_  == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return activeDirtyRegionInfos;
    }

    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return activeDirtyRegionInfos;
    }
    for (auto conn : serviceToRenderConns) {
        auto activeDirtyRegionInfosTemp = conn->GetActiveDirtyRegionInfo();
        activeDirtyRegionInfos.insert(activeDirtyRegionInfos.end(),
            make_move_iterator(activeDirtyRegionInfosTemp.begin()),
            make_move_iterator(activeDirtyRegionInfosTemp.end()));
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSClientToServiceConnection::GetGlobalDirtyRegionInfo()
{
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return globalDirtyRegionInfo;
    }
    for (auto conn : serviceToRenderConns) {
        auto globalDirtyRegionInfoTemp = conn->GetGlobalDirtyRegionInfo();
        globalDirtyRegionInfo += globalDirtyRegionInfoTemp;
    }
    return globalDirtyRegionInfo;
}

LayerComposeInfo RSClientToServiceConnection::GetLayerComposeInfo()
{
    LayerComposeInfo layerComposeInfo;
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return layerComposeInfo;
    }
    for (auto conn : serviceToRenderConns) {
        auto layerComposeInfoTemp = conn->GetLayerComposeInfo();
        layerComposeInfo += layerComposeInfoTemp;
    }
    layerComposeInfo.redrawFrameNumber = REDRAW_FRAME_NUMBER.load();
    layerComposeInfo.offlineComposeFrameNumber -= REDRAW_FRAME_NUMBER.load();
    REDRAW_FRAME_NUMBER.store(0);
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSClientToServiceConnection::GetHwcDisabledReasonInfo()
{
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return hwcDisabledReasonInfos;
    }
    for (auto conn : serviceToRenderConns) {
        auto hwcDisabledReasonInfosTemp = conn->GetHwcDisabledReasonInfo();
        hwcDisabledReasonInfos.insert(hwcDisabledReasonInfos.end(),
            make_move_iterator(hwcDisabledReasonInfosTemp.begin()),
            make_move_iterator(hwcDisabledReasonInfosTemp.end()));
    }
    return hwcDisabledReasonInfos;
}

ErrCode RSClientToServiceConnection::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        int64_t hdrOnDurationTemp = 0;
        conn->GetHdrOnDuration(hdrOnDurationTemp);
        hdrOnDuration += hdrOnDurationTemp;
    }
    return ERR_OK;
}

void RSClientToServiceConnection::SetVmaCacheStatus(bool flag)
{
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetVmaCacheStatus(flag);
    }
}

#ifdef TP_FEATURE_ENABLE
ErrCode RSClientToServiceConnection::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    switch (tpFeatureConfigType) {
        case TpFeatureConfigType::DEFAULT_TP_FEATURE: {
            if (TOUCH_SCREEN->SetFeatureConfig(feature, config) < 0) {
                RS_LOGW("SetTpFeatureConfig: SetFeatureConfig failed");
                return ERR_INVALID_VALUE;
            }
            return ERR_OK;
        }
        case TpFeatureConfigType::AFT_TP_FEATURE: {
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

void RSClientToServiceConnection::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    }
}

ErrCode RSClientToServiceConnection::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty() || pidList.size() > PIDLIST_SIZE_MAX) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetGpuCrcDirtyEnabledPidList(pidList);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    if (pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }

    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetOptimizeCanvasDirtyPidList(pidList);
    }
    return ERR_OK;
}

int32_t RSClientToServiceConnection::RegisterUIExtensionCallback(uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        int32_t retTmp = conn->RegisterUIExtensionCallback(remotePid_, userId, callback, unobscured);
        ret = (ret != ERR_OK) ? ret : retTmp;
    }
    return ret;
}

ErrCode RSClientToServiceConnection::SetVirtualScreenStatus(ScreenId id,
    VirtualScreenStatus screenStatus, bool& success)
{
    if (!screenManagerAgent_) {
        success = false;
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_LOGD("SetVirtualScreenStatus ScreenId:%{public}" PRIu64 " screenStatus:%{public}d",
        id, screenStatus);
    success = screenManagerAgent_->SetVirtualScreenStatus(id, screenStatus);
    return StatusCode::SUCCESS;
}

ErrCode RSClientToServiceConnection::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        ErrCode retTmp = conn->SetLayerTop(nodeIdStr, isTop);
        ret = (ret != ERR_OK) ? ret : retTmp;
    }
    return ret;
}

ErrCode RSClientToServiceConnection::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        ErrCode tmpRet = conn->SetForceRefresh(nodeIdStr, isForceRefresh);
        ret = (ret != ERR_OK) ? ret : tmpRet;
    }
    return ret;
}

void RSClientToServiceConnection::SetFreeMultiWindowStatus(bool enable)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetFreeMultiWindowStatus(enable);
    }
}

ErrCode RSClientToServiceConnection::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->SetColorFollow(nodeIdStr, isColorFollow);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::NotifyScreenSwitched(ScreenId id)
{
    if (!screenManagerAgent_) {
        RS_LOGE("NotifyScreenSwitched screenManagerAgent_ is nullptr");
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("NotifyScreenSwitched SetScreenSwitchStatus true");
    RS_TRACE_NAME_FMT("NotifyScreenSwitched");
    screenManagerAgent_->SetScreenSwitchStatus(id, true);
    return ERR_OK;
}


int32_t RSClientToServiceConnection::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }

    int32_t result = StatusCode::SUCCESS;
    for (auto conn : serviceToRenderConns) {
        int32_t ret = conn->RegisterSelfDrawingNodeRectChangeCallback(remotePid_, constraint, callback);
        if (ret != StatusCode::SUCCESS) {
            result = ret;
            break;
        }
    }
    return result;
}


int32_t RSClientToServiceConnection::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.size() == 0) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t result = StatusCode::SUCCESS;
    for (auto conn : serviceToRenderConns) {
        int32_t ret = conn->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid_);
        if (ret != StatusCode::SUCCESS) {
            result = ret;
            break;
        }
    }
    return result;
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

ErrCode RSClientToServiceConnection::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        conn->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    }
    return ERR_OK;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSClientToServiceConnection::SetOverlayDisplayMode(int32_t mode)
{
    RS_LOGI("SetOverlayDisplayMode: mode: [%{public}d]", mode);
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto conn : serviceToRenderConns) {
        auto ret = conn->SetOverlayDisplayMode(mode);
        if (ret != ERR_OK) {
            return ret;
        }
    }
    return ERR_OK;
}
#endif

ErrCode RSClientToServiceConnection::SetBehindWindowFilterEnabled(bool enabled)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto& conn : serviceToRenderConns) {
        conn->SetBehindWindowFilterEnabled(enabled);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnection::GetBehindWindowFilterEnabled(bool& enabled)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    for (auto& conn : serviceToRenderConns) {
        // Multiple users are performing the same operation temporarily
        conn->GetBehindWindowFilterEnabled(enabled);
        break;
    }
    return ERR_OK;
}

int32_t RSClientToServiceConnection::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    if (renderProcessManagerAgent_ == nullptr) {
        RS_LOGE("%s{public}s renderProcessManagerAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto serviceToRenderConns = renderProcessManagerAgent_->GetServiceToRenderConns();
    if (serviceToRenderConns.empty()) {
        RS_LOGE("%s{public}s serviceToRenderConns is empty", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t ret = ERR_OK;
    for (auto conn : serviceToRenderConns) {
        float tmpGpuMemInMb = 0;
        int32_t retTmp = conn->GetPidGpuMemoryInMB(pid, tmpGpuMemInMb);
        if (retTmp != ERR_OK) {
            ret = retTmp;
            RS_LOGE("%s{public}s serviceToRenderConns is error", __func__);
        } else {
            gpuMemInMB += tmpGpuMemInMb;
        }

    }
    return ret;
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
    // 遗漏，车机是直接删掉
#ifdef RS_PROFILER_ENABLED
    return RSProfiler::HrpServicePopulateFiles(dirInfo, firstFileIndex, outFiles);
#else
    outFiles.clear();
    return RET_HRP_SERVICE_ERR_UNSUPPORTED;
#endif
}

bool RSClientToServiceConnection::ProfilerIsSecureScreen()
{
    // 遗漏，车机是直接删掉
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
