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

#include "rs_render_service_connection.h"
#include <string>

#include "frame_report.h"
#include "hgm_command.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "luminance/rs_luminance_control.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "rs_main_thread.h"
#include "rs_trace.h"
//blur predict
#include "rs_frame_blur_predict.h"
#include "system/rs_system_parameters.h"

#include "command/rs_command_verify_helper.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_background_thread.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/capture/rs_uni_ui_capture.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "include/gpu/GrDirectContext.h"
#include "info_collection/rs_hdr_collection.h"
#ifdef RS_ENABLE_GPU
#include "feature/uifirst/rs_sub_thread_manager.h"
#endif
#include "feature/uifirst/rs_uifirst_manager.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_pointer_window_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_render_frame_rate_linker_map.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pixel_map_from_surface.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats.h"
#include "render/rs_typeface_cache.h"

#ifdef TP_FEATURE_ENABLE
#include "screen_manager/touch_screen.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int SLEEP_TIME_US = 1000;
const std::string REGISTER_NODE = "RegisterNode";
const std::string APS_SET_VSYNC = "APS_SET_VSYNC";
}
// we guarantee that when constructing this object,
// all these pointers are valid, so will not check them.
RSRenderServiceConnection::RSRenderServiceConnection(
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
      ApplicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this)),
      appVSyncDistributor_(distributor)
{
    if (token_ == nullptr || !token_->AddDeathRecipient(connDeathRecipient_)) {
        RS_LOGW("RSRenderServiceConnection: Failed to set death recipient.");
    }
}

RSRenderServiceConnection::~RSRenderServiceConnection() noexcept
{
    if (token_ && connDeathRecipient_) {
        token_->RemoveDeathRecipient(connDeathRecipient_);
    }
    CleanAll();
}

void RSRenderServiceConnection::CleanVirtualScreens() noexcept
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

void RSRenderServiceConnection::CleanRenderNodes() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();

    nodeMap.FilterNodeByPid(remotePid_);
}

void RSRenderServiceConnection::CleanFrameRateLinkers() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();

    frameRateLinkerMap.FilterFrameRateLinkerByPid(remotePid_);
}

void RSRenderServiceConnection::CleanFrameRateLinkerExpectedFpsCallbacks() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();
    frameRateLinkerMap.UnRegisterExpectedFpsUpdateCallbackByListener(remotePid_);
}

void RSRenderServiceConnection::CleanAll(bool toDelete) noexcept
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
    RS_LOGD("RSRenderServiceConnection::CleanAll() start.");
    RS_TRACE_NAME("RSRenderServiceConnection CleanAll begin, remotePid: " + std::to_string(remotePid_));
    RsCommandVerifyHelper::GetInstance().RemoveCntWithPid(remotePid_);
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this)]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (!connection) {
                return;
            }
            RS_TRACE_NAME_FMT("CleanVirtualScreens %d", connection->remotePid_);
            connection->CleanVirtualScreens();
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this)]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (!connection) {
                return;
            }
            RS_TRACE_NAME_FMT("CleanRenderNodes %d", connection->remotePid_);
            connection->CleanRenderNodes();
            connection->CleanFrameRateLinkers();
            connection->CleanFrameRateLinkerExpectedFpsCallbacks();
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this)]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            RS_TRACE_NAME_FMT("ClearTransactionDataPidInfo %d", connection->remotePid_);
            connection->mainThread_->ClearTransactionDataPidInfo(connection->remotePid_);
        }).wait();
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this)]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            RS_TRACE_NAME_FMT("UnRegisterCallback %d", connection->remotePid_);
            connection->mainThread_->UnRegisterOcclusionChangeCallback(connection->remotePid_);
            connection->mainThread_->ClearSurfaceOcclusionChangeCallback(connection->remotePid_);
            connection->mainThread_->UnRegisterUIExtensionCallback(connection->remotePid_);
        }).wait();
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
        auto renderService = renderService_.promote();
        if (renderService == nullptr) {
            RS_LOGW("RSRenderServiceConnection::CleanAll() RenderService is dead.");
        } else {
            renderService->RemoveConnection(GetToken());
        }
    }

    RS_LOGD("RSRenderServiceConnection::CleanAll() end.");
    RS_TRACE_NAME("RSRenderServiceConnection CleanAll end, remotePid: " + std::to_string(remotePid_));
}

RSRenderServiceConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{
}

void RSRenderServiceConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    if (rsConn->GetToken() != tokenSptr) {
        RS_LOGI("RSConnectionDeathRecipient::OnRemoteDied: token doesn't match, ignore it.");
        return;
    }

    rsConn->CleanAll(true);
}

RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{}

void RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: "
            "RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    RS_LOGD("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: Unregister.");
    auto app = iface_cast<IApplicationAgent>(tokenSptr);
    rsConn->UnRegisterApplicationAgent(app);
}

void RSRenderServiceConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!mainThread_) {
        return;
    }
    pid_t callingPid = GetCallingPid();
    bool isTokenTypeValid = true;
    bool isNonSystemAppCalling = false;
    RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
    bool shouldDrop = RSUnmarshalThread::Instance().ReportTransactionDataStatistics(
        callingPid, transactionData.get(), isNonSystemAppCalling);
    if (shouldDrop) {
        RS_LOGW("RSRenderServiceConnection::CommitTransaction data droped");
        return;
    }
    bool isProcessBySingleFrame = mainThread_->IsNeedProcessBySingleFrameComposer(transactionData);
    if (isProcessBySingleFrame) {
        mainThread_->ProcessDataBySingleFrameComposer(transactionData);
    } else {
        mainThread_->RecvRSTransactionData(transactionData);
    }
}

void RSRenderServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr || mainThread_ == nullptr) {
        RS_LOGW("RSRenderServiceConnection::ExecuteSynchronousTask, task or main thread is null!");
        return;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager && screenManager->IsScreenPoweringOn() && task->GetType() == RS_NODE_SYNCHRONOUS_READ_PROPERTY) {
        RS_LOGI("RSRenderServiceConnection::ExecuteSynchronousTask, when screen is powering on, the task is executed "
                "in the IPC thread");
        task->Process(mainThread_->GetContext());
        return;
    }
    // After a synchronous task times out, it will no longer be executed.
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    std::chrono::nanoseconds span(task->GetTimeout());
    mainThread_->ScheduleTask([task, mainThread = mainThread_, isTimeoutWeak] {
        if (task == nullptr || mainThread == nullptr || isTimeoutWeak.expired()) {
            return;
        }
        task->Process(mainThread->GetContext());
    }).wait_for(span);
    isTimeout.reset();
}

bool RSRenderServiceConnection::GetUniRenderEnabled()
{
    return RSUniRenderJudgement::IsUniRender();
}

bool RSRenderServiceConnection::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    if (!mainThread_) {
        return false;
    }
    std::shared_ptr<RSDisplayRenderNode> node =
        DisplayNodeCommandHelper::CreateWithConfigInRS(mainThread_->GetContext(), nodeId,
            displayNodeConfig);
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateDisplayNode fail");
        return false;
    }
    std::function<void()> registerNode = [node, weakThis = wptr<RSRenderServiceConnection>(this),
        mirrorNodeId = displayNodeConfig.mirrorNodeId]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        context.GetMutableNodeMap().RegisterRenderNode(node);
        context.GetGlobalRootRenderNode()->AddChild(node);
        auto mirrorSourceNode = context.GetNodeMap()
            .GetRenderNode<RSDisplayRenderNode>(mirrorNodeId);
        if (!mirrorSourceNode) {
            return;
        }
        node->SetMirrorSource(mirrorSourceNode);
    };
    mainThread_->PostSyncTask(registerNode);
    return true;
}

bool RSRenderServiceConnection::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    if (!mainThread_) {
        return false;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, mainThread_->GetContext());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNode fail");
        return false;
    }
    std::function<void()> registerNode = [node, weakThis = wptr<RSRenderServiceConnection>(this)]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    return true;
}

sptr<Surface> RSRenderServiceConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, bool unobscured)
{
    if (!mainThread_) {
        return nullptr;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, mainThread_->GetContext(), unobscured);
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return nullptr;
    }
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    if (surface == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return nullptr;
    }
    const std::string& surfaceName = surface->GetName();
    RS_LOGI("RsDebug RSRenderService::CreateNodeAndSurface node" \
        "id:%{public}" PRIu64 " name:%{public}s surface id:%{public}" PRIu64 " name:%{public}s",
        node->GetId(), node->GetName().c_str(),
        surface->GetUniqueId(), surfaceName.c_str());
    auto defaultUsage = surface->GetDefaultUsage();
    surface->SetDefaultUsage(defaultUsage | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER);
    node->GetRSSurfaceHandler()->SetConsumer(surface);
    RSMainThread* mainThread = mainThread_;
    std::function<void()> registerNode = [node, mainThread]() -> void {
        if (auto preNode = mainThread->GetContext().GetNodeMap().GetRenderNode(node->GetId())) {
            RS_LOGE("CreateNodeAndSurface same id node:%{public}" PRIu64 ", type:%d",
                node->GetId(), preNode->GetType());
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return nullptr;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    return pSurface;
}

sptr<IVSyncConnection> RSRenderServiceConnection::CreateVSyncConnection(const std::string& name,
                                                                        const sptr<VSyncIConnectionToken>& token,
                                                                        uint64_t id,
                                                                        NodeId windowNodeId,
                                                                        bool fromXcomponent)
{
    if (mainThread_ == nullptr || appVSyncDistributor_ == nullptr) {
        return nullptr;
    }
    if (fromXcomponent) {
        auto& node = RSMainThread::Instance()->GetContext().GetNodeMap()
                    .GetRenderNode<RSRenderNode>(windowNodeId);
        if (node == nullptr) {
            RS_LOGE("RSRenderServiceConnection::CreateVSyncConnection:node is nullptr");
            return nullptr;
        }
        windowNodeId = node->GetInstanceRootNodeId();
    }
    sptr<VSyncConnection> conn = new VSyncConnection(appVSyncDistributor_, name, token->AsObject(), 0, windowNodeId);
    if (ExtractPid(id) == remotePid_) {
        auto observer = [] (const RSRenderFrameRateLinker& linker) {
            if (auto mainThread = RSMainThread::Instance(); mainThread != nullptr) {
                HgmCore::Instance().SetHgmTaskFlag(true);
            }
        };
        mainThread_->ScheduleTask([weakThis = wptr<RSRenderServiceConnection>(this), id, observer]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            auto linker = std::make_shared<RSRenderFrameRateLinker>(id, observer);
            auto& context = connection->mainThread_->GetContext();
            auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();
            frameRateLinkerMap.RegisterFrameRateLinker(linker);
        }).wait();
        conn->id_ = id;
        RS_LOGD("CreateVSyncConnection connect id: %{public}" PRIu64, id);
    }
    auto ret = appVSyncDistributor_->AddConnection(conn, windowNodeId);
    if (ret != VSYNC_ERROR_OK) {
        return nullptr;
    }
    return conn;
}

int32_t RSRenderServiceConnection::GetPixelMapByProcessId(
    std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapVector, pid_t pid)
{
    if (mainThread_ == nullptr) {
        return INVALID_ARGUMENTS;
    }
    std::vector<sptr<SurfaceBuffer>> surfaceBufferVector;
    std::function<void()> getSurfaceBufferByPidTask = [weakThis = wptr<RSRenderServiceConnection>(this),
                                                          &pixelMapVector, &surfaceBufferVector, pid]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto selfDrawingNodeMap =
            connection->mainThread_->GetContext().GetMutableNodeMap().GetSelfDrawingNodeInProcess(pid);
        for (auto iter = selfDrawingNodeMap.begin(); iter != selfDrawingNodeMap.end(); ++iter) {
            auto surfaceNode = iter->second;
            if (surfaceNode) {
                auto surfaceBuffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
                surfaceBufferVector.push_back(surfaceBuffer);
            }
        }
    };
    mainThread_->PostSyncTask(getSurfaceBufferByPidTask);

    for (const auto &surfaceBuffer : surfaceBufferVector) {
        if (surfaceBuffer) {
            OHOS::Media::Rect rect = {
                .left = 0,
                .top = 0,
                .width = surfaceBuffer->GetWidth(),
                .height = surfaceBuffer->GetHeight(),
            };
            std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
            RSBackgroundThread::Instance().PostSyncTask([&surfaceBuffer, rect, &pixelmap]() {
                pixelmap = OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, rect);
            });
            pixelMapVector.push_back(pixelmap);
        } else {
            RS_LOGE("RSRenderServiceConnection::CreatePixelMapFromSurface surfaceBuffer is null");
        }
    }
    return SUCCESS;
}

std::shared_ptr<Media::PixelMap> RSRenderServiceConnection::CreatePixelMapFromSurface(sptr<Surface> surface,
    const Rect &srcRect)
{
    OHOS::Media::Rect rect = {
        .left = srcRect.x,
        .top = srcRect.y,
        .width = srcRect.w,
        .height = srcRect.h,
    };
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    RSBackgroundThread::Instance().PostSyncTask([surface, rect, &pixelmap]() {
        pixelmap = OHOS::Rosen::CreatePixelMapFromSurface(surface, rect);
    });
    return pixelmap;
}

int32_t RSRenderServiceConnection::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
{
    if (mainThread_ == nullptr) {
        return INVALID_ARGUMENTS;
    }
    mainThread_->ScheduleTask(
        [pid, uid, bundleName, abilityName, focusNodeId, mainThread = mainThread_]() {
            // don't use 'this' to get mainThread poninter
            mainThread->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
        }
    );
    return SUCCESS;
}

bool RSRenderServiceConnection::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    if (!mainThread_) {
        return false;
    }
    mainThread_->SetWatermark(name, watermark);
    return true;
}

ScreenId RSRenderServiceConnection::GetDefaultScreenId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetDefaultScreenId();
}

ScreenId RSRenderServiceConnection::GetActiveScreenId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetActiveScreenId();
}

std::vector<ScreenId> RSRenderServiceConnection::GetAllScreenIds()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return std::vector<ScreenId>();
    }
    return screenManager_->GetAllScreenIds();
}

ScreenId RSRenderServiceConnection::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto newVirtualScreenId = screenManager_->CreateVirtualScreen(
        name, width, height, surface, mirrorId, flags, whiteList);
    virtualScreenIds_.insert(newVirtualScreenId);
    if (surface != nullptr) {
        EventInfo event = { "VOTER_VIRTUALDISPLAY", ADD_VOTE, OLED_60_HZ, OLED_60_HZ, name };
        NotifyRefreshRateEvent(event);
    }
    return newVirtualScreenId;
}

int32_t RSRenderServiceConnection::SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    if (blackListVector.empty()) {
        RS_LOGW("SetVirtualScreenBlackList blackList is empty.");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenBlackList(id, blackListVector);
}

int32_t RSRenderServiceConnection::AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    if (blackListVector.empty()) {
        RS_LOGW("AddVirtualScreenBlackList blackList is empty.");
        return StatusCode::BLACKLIST_IS_EMPTY;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->AddVirtualScreenBlackList(id, blackListVector);
}

int32_t RSRenderServiceConnection::RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    if (blackListVector.empty()) {
        RS_LOGW("RemoveVirtualScreenBlackList blackList is empty.");
        return StatusCode::BLACKLIST_IS_EMPTY;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->RemoveVirtualScreenBlackList(id, blackListVector);
}

int32_t RSRenderServiceConnection::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSRenderServiceConnection::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenSecurityMask(id, std::move(securityMask));
}

int32_t RSRenderServiceConnection::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    if (screenManager_ == nullptr) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSRenderServiceConnection::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSRenderServiceConnection::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSurface(id, surface);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSRenderServiceConnection::SetPointerColorInversionConfig(float darkBuffer,
    float brightBuffer, int64_t interval, int32_t rangeSize)
{
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionConfig(darkBuffer, brightBuffer,
        interval, rangeSize);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::SetPointerColorInversionEnabled(bool enable)
{
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionEnabled(enable);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterPointerLuminanceChangeCallback(
    sptr<RSIPointerLuminanceChangeCallback> callback)
{
    if (!callback) {
        RS_LOGE("RSRenderServiceConnection::RegisterPointerLuminanceChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    RSMagicPointerRenderManager::GetInstance().RegisterPointerLuminanceChangeCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::UnRegisterPointerLuminanceChangeCallback()
{
    RSMagicPointerRenderManager::GetInstance().UnRegisterPointerLuminanceChangeCallback(remotePid_);
    return StatusCode::SUCCESS;
}
#endif

void RSRenderServiceConnection::RemoveVirtualScreen(ScreenId id)
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

int32_t RSRenderServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (!callback) {
        return INVALID_ARGUMENTS;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (screenChangeCallback_ == callback) {
        return INVALID_ARGUMENTS;
    }
    if (screenManager_ == nullptr) {
        return SCREEN_NOT_FOUND;
    }

    if (screenChangeCallback_ != nullptr) {
        // remove the old callback
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
    }

    // update
    int32_t status = screenManager_->AddScreenChangeCallback(callback);
    auto tmp = screenChangeCallback_;
    screenChangeCallback_ = callback;
    lock.unlock();
    return status;
}

void RSRenderServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    if (!screenManager_) {
        return;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { screenManager_->SetScreenActiveMode(id, modeId); }).wait();
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenActiveMode(id, modeId); }).wait();
    }
}

void RSRenderServiceConnection::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::SetScreenRefreshRate");
    HgmTaskHandleThread::Instance().PostTask([id, sceneId, rate] () {
        int32_t setResult = OHOS::Rosen::HgmCore::Instance().SetScreenRefreshRate(id, sceneId, rate);
        if (setResult != 0) {
            RS_LOGW("SetScreenRefreshRate request of screen %{public}" PRIu64 " of rate %{public}d is refused",
                id, rate);
            return;
        }
    });
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderServiceConnection::SetRefreshRateMode(int32_t refreshRateMode)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::SetRefreshRateMode");
    HgmTaskHandleThread::Instance().PostTask([refreshRateMode] () {
        int32_t setResult = OHOS::Rosen::HgmCore::Instance().SetRefreshRateMode(refreshRateMode);
        RSSystemProperties::SetHgmRefreshRateModesEnabled(std::to_string(refreshRateMode));
        if (setResult != 0) {
            RS_LOGW("SetRefreshRateMode mode %{public}d is not supported", refreshRateMode);
        }
    });
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderServiceConnection::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this), id, &range, animatorExpectedFrameRate]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
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
        }).wait();
}

void RSRenderServiceConnection::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this), id]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
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

uint32_t RSRenderServiceConnection::GetScreenCurrentRefreshRate(ScreenId id)
{
    uint32_t rate = HgmTaskHandleThread::Instance().ScheduleTask([id] () -> uint32_t {
        return OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(id);
    }).get();
    if (rate == 0) {
        RS_LOGW("GetScreenCurrentRefreshRate failed to get current refreshrate of"
            " screen : %{public}" PRIu64 "", id);
    }
    return rate;
}

std::vector<int32_t> RSRenderServiceConnection::GetScreenSupportedRefreshRates(ScreenId id)
{
    return HgmTaskHandleThread::Instance().ScheduleTask([id] () -> std::vector<int32_t> {
        return OHOS::Rosen::HgmCore::Instance().GetScreenComponentRefreshRates(id);
    }).get();
}

bool RSRenderServiceConnection::GetShowRefreshRateEnabled()
{
    return RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled();
}

void RSRenderServiceConnection::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    return RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enabled, type);
}

uint32_t RSRenderServiceConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    return RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRate(screenId);
}

std::string RSRenderServiceConnection::GetRefreshInfo(pid_t pid)
{
    if (!mainThread_) {
        return "";
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();
    std::string surfaceName = nodeMap.GetSelfDrawSurfaceNameByPid(pid);
    if (surfaceName.empty()) {
        return "";
    }
    std::string dumpString;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().ScheduleTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), &dumpString, &surfaceName]() {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->FpsDump(dumpString, surfaceName);
            }).wait();
#endif
    } else {
        mainThread_->ScheduleTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), &dumpString, &surfaceName]() {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->FpsDump(dumpString, surfaceName);
            }).wait();
    }
    return dumpString;
}

int32_t RSRenderServiceConnection::GetCurrentRefreshRateMode()
{
    return HgmTaskHandleThread::Instance().ScheduleTask([] () -> int32_t {
        return OHOS::Rosen::HgmCore::Instance().GetCurrentRefreshRateMode();
    }).get();
}

int32_t RSRenderServiceConnection::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_MANAGER_NULL;
    }
    return screenManager_->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSRenderServiceConnection::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetVirtualScreenResolution(id, width, height); }).get();
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

void RSRenderServiceConnection::MarkPowerOffNeedProcessOneFrame()
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        renderThread_.PostTask(
            [weakThis = wptr<RSRenderServiceConnection>(this)]() {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->MarkPowerOffNeedProcessOneFrame();
            }
        );
#endif
    }
}

void RSRenderServiceConnection::RepaintEverything()
{
    if (mainThread_ == nullptr) {
        RS_LOGE("RepaintEverything, mainThread_ is null, return");
    }
    auto task = []() -> void {
        RS_LOGI("RepaintEverything, setDirtyflag, forceRefresh in mainThread");
        RSMainThread::Instance()->SetDirtyFlag();
        RSMainThread::Instance()->ForceRefreshForUni();
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ForceRefreshOneFrameWithNextVSync()
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }

    auto task = [weakThis = wptr<RSRenderServiceConnection>(this)]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }

        RS_LOGI("ForceRefreshOneFrameWithNextVSync, setDirtyflag, forceRefresh in mainThread");
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::DisablePowerOffRenderControl(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        renderThread_.PostTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), id]() {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return;
                }
                connection->screenManager_->DisablePowerOffRenderControl(id);
            }
        );
#endif
    }
}

void RSRenderServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (screenManager_ == nullptr || mainThread_ == nullptr) {
        RS_LOGE("%{public}s screenManager or mainThread is null, id: %{public}" PRIu64, __func__, id);
        return;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().ScheduleTask(
            [=]() { screenManager_->SetScreenPowerStatus(id, status); }).wait();
        mainThread_->SetDiscardJankFrames(true);
        renderThread_.SetDiscardJankFrames(true);
        HgmTaskHandleThread::Instance().PostTask([id, status]() {
            OHOS::Rosen::HgmCore::Instance().NotifyScreenPowerStatus(id, status);
        });
#endif
    } else {
        mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenPowerStatus(id, status); }).wait();
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

    if (captureConfig.isSync) {
        RSMainThread::Instance()->AddUiCaptureTask(id, captureTask);
        return;
    }

    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(id);
    if (!node) {
        RS_LOGE("RSRenderServiceConnection::TakeSurfaceCaptureForUiParallel node is nullptr");
        callback->OnSurfaceCapture(id, nullptr);
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
        callback->OnSurfaceCapture(id, pixelmap.get());
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

void RSRenderServiceConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect, RSSurfaceCapturePermissions permissions)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr", __func__);
        return;
    }
    std::function<void()> captureTask = [id, callback, captureConfig, blurParam, specifiedAreaRect,
        screenCapturePermission = permissions.screenCapturePermission,
        isSystemCalling = permissions.isSystemCalling,
        selfCapture = permissions.selfCapture]() -> void {
        if (captureConfig.captureType == SurfaceCaptureType::UICAPTURE) {
            // When the isSync flag in captureConfig is true, UI capture processes commands before capture.
            // When the isSync flag in captureConfig is false, UI capture will check null node independently.
            // Therefore, a null node is valid for UI capture.
            auto uiCaptureHasPermission = selfCapture || isSystemCalling;
            if (!uiCaptureHasPermission) {
                RS_LOGE("RSRenderServiceConnection::TakeSurfaceCapture uicapture failed, nodeId:[%{public}" PRIu64
                        "], isSystemCalling: %{public}u, selfCapture: %{public}u",
                    id, isSystemCalling, selfCapture);
                callback->OnSurfaceCapture(id, nullptr);
                return;
            }
            if (RSUniRenderJudgement::IsUniRender()) {
                TakeSurfaceCaptureForUiParallel(id, callback, captureConfig, specifiedAreaRect);
            } else {
                TakeSurfaceCaptureForUIWithUni(id, callback, captureConfig);
            }
            return;
        }
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
        if (node == nullptr) {
            RS_LOGE("RSRenderServiceConnection::TakeSurfaceCapture failed, node is nullptr");
            callback->OnSurfaceCapture(id, nullptr);
            return;
        }
        auto displayCaptureHasPermission = screenCapturePermission && isSystemCalling;
        auto surfaceCaptureHasPermission = blurParam.isNeedBlur ? isSystemCalling : (selfCapture || isSystemCalling);
        if ((node->GetType() == RSRenderNodeType::DISPLAY_NODE && !displayCaptureHasPermission) ||
            (node->GetType() == RSRenderNodeType::SURFACE_NODE && !surfaceCaptureHasPermission)) {
            RS_LOGE("RSRenderServiceConnection::TakeSurfaceCapture failed, node type: %{public}u, "
                "screenCapturePermission: %{public}u, isSystemCalling: %{public}u, selfCapture: %{public}u",
                node->GetType(), screenCapturePermission, isSystemCalling, selfCapture);
            callback->OnSurfaceCapture(id, nullptr);
            return;
        }
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_DISABLED) {
            RS_LOGD("RSRenderService::TakeSurfaceCapture captureTaskInner nodeId:[%{public}" PRIu64 "]", id);
            ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCapture");
            RSSurfaceCaptureTask task(id, captureConfig);
            if (!task.Run(callback)) {
                callback->OnSurfaceCapture(id, nullptr);
            }
            ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        } else {
#ifdef RS_ENABLE_GPU
            RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
            RSSurfaceCaptureTaskParallel::Capture(id, callback, captureConfig, isSystemCalling, false, blurParam);
#endif
        }
    };
    mainThread_->PostTask(captureTask);
}

void RSRenderServiceConnection::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr", __func__);
        return;
    }
    std::function<void()> setWindowFreezeTask = [id, isFreeze, callback, captureConfig, blurParam]() -> void {
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
        if (node == nullptr) {
            RS_LOGE("RSRenderServiceConnection::SetWindowFreezeImmediately failed, node is nullptr");
            if (callback) {
                callback->OnSurfaceCapture(id, nullptr);
            }
            return;
        }
        node->SetStaticCached(isFreeze);
        if (isFreeze) {
            bool isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
                RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
                "::SET_WINDOW_FREEZE_IMMEDIATELY");
            RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
            RSSurfaceCaptureTaskParallel::Capture(id, callback, captureConfig, isSystemCalling, isFreeze, blurParam);
        } else {
            RSSurfaceCaptureTaskParallel::ClearCacheImageByFreeze(id);
        }
    };
    mainThread_->PostTask(setWindowFreezeTask);
}

void RSRenderServiceConnection::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (mainThread_ == nullptr || screenManager_ == nullptr) {
        return;
    }

    // adapt video scene pointer
    if (screenManager_->GetCurrentVirtualScreenNum() > 0 ||
        !RSPointerWindowManager::Instance().GetIsPointerEnableHwc()) {
        // when has virtual screen or pointer is enable hwc, we can't skip
        RSPointerWindowManager::Instance().SetIsPointerCanSkipFrame(false);
        RSMainThread::Instance()->RequestNextVSync();
    } else {
        RSPointerWindowManager::Instance().SetIsPointerCanSkipFrame(true);
    }

    RSPointerWindowManager::Instance().SetHwcNodeBounds(rsNodeId, positionX, positionY,
        positionZ, positionW);
}

void RSRenderServiceConnection::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (!mainThread_) {
        RS_LOGE("RSRenderServiceConnection::RegisterApplicationAgent mainThread_ is nullptr");
        return;
    }
    auto captureTask = [weakThis = wptr<RSRenderServiceConnection>(this), pid, app]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            RS_LOGE("RSRenderServiceConnection::RegisterApplicationAgent connection or mainThread_ is nullptr");
            return;
        }
        connection->mainThread_->RegisterApplicationAgent(pid, app);
    };
    mainThread_->PostTask(captureTask);

    app->AsObject()->AddDeathRecipient(ApplicationDeathRecipient_);
}

void RSRenderServiceConnection::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    auto captureTask = [=]() -> void {
        RSMainThread::Instance()->UnRegisterApplicationAgent(app);
    };
    RSMainThread::Instance()->ScheduleTask(captureTask).wait();
}

RSVirtualScreenResolution RSRenderServiceConnection::GetVirtualScreenResolution(ScreenId id)
{
    RSVirtualScreenResolution virtualScreenResolution;
    if (!screenManager_) {
        return virtualScreenResolution;
    }
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    return virtualScreenResolution;
}

RSScreenModeInfo RSRenderServiceConnection::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (!screenManager_) {
        return screenModeInfo;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        RSHardwareThread::Instance().ScheduleTask(
            [=, &screenModeInfo]() { return screenManager_->GetScreenActiveMode(id, screenModeInfo); }).wait();
#else
        return screenModeInfo;
#endif
    } else if (mainThread_ != nullptr) {
        mainThread_->ScheduleTask(
            [=, &screenModeInfo]() { return screenManager_->GetScreenActiveMode(id, screenModeInfo); }).wait();
    }
    return screenModeInfo;
}

bool RSRenderServiceConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
#ifdef RS_ENABLE_GPU
    RSMainThread::Instance()->GetAppMemoryInMB(cpuMemSize, gpuMemSize);
    gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
#endif
    return true;
}

MemoryGraphic RSRenderServiceConnection::GetMemoryGraphic(int pid)
{
    MemoryGraphic memoryGraphic;
    if (!mainThread_) {
        return memoryGraphic;
    }
    if (GetUniRenderEnabled()) {
        RSMainThread* mainThread = mainThread_;
        mainThread_->ScheduleTask([mainThread, &pid, &memoryGraphic]() {
            if (RSMainThread::Instance()->GetContext().GetNodeMap().ContainPid(pid)) {
                mainThread->CountMem(pid, memoryGraphic);
            }
        }).wait();
        return memoryGraphic;
    } else {
        return memoryGraphic;
    }
}

std::vector<MemoryGraphic> RSRenderServiceConnection::GetMemoryGraphics()
{
    std::vector<MemoryGraphic> memoryGraphics;
    if (!mainThread_) {
        return memoryGraphics;
    }
    if (GetUniRenderEnabled()) {
        mainThread_->ScheduleTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), &memoryGraphics]() {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->mainThread_ == nullptr) {
                    return;
                }
                return connection->mainThread_->CountMem(memoryGraphics);
            }).wait();
        return memoryGraphics;
    } else {
        return memoryGraphics;
    }
}

std::vector<RSScreenModeInfo> RSRenderServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    if (!screenManager_) {
        return std::vector<RSScreenModeInfo>();
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenSupportedModes(id); }).get();
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

RSScreenCapability RSRenderServiceConnection::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    if (!screenManager_) {
        return screenCapability;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenCapability(id); }).get();
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

ScreenPowerStatus RSRenderServiceConnection::GetScreenPowerStatus(ScreenId id)
{
    if (!screenManager_) {
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenPowerStatus(id); }).get();
#else
        return ScreenPowerStatus::INVALID_POWER_STATUS;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenPowerStatus(id); }).get();
    } else {
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }
}

RSScreenData RSRenderServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    if (!screenManager_) {
        return screenData;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenData(id); }).get();
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

int32_t RSRenderServiceConnection::GetScreenBacklight(ScreenId id)
{
    if (!screenManager_) {
        return INVALID_BACKLIGHT_VALUE;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenBacklight(id); }).get();
#else
        return INVALID_BACKLIGHT_VALUE;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenBacklight(id); }).get();
    } else {
        return INVALID_BACKLIGHT_VALUE;
    }
}

void RSRenderServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    if (!screenManager_) {
        return;
    }
    RSLuminanceControl::Get().SetSdrLuminance(id, level);
    if (RSLuminanceControl::Get().IsHdrOn(id) && level > 0 && mainThread_ != nullptr) {
        auto task = [weakThis = wptr<RSRenderServiceConnection>(this), id]() {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                RS_LOGE("RSRenderServiceConnection::SetScreenBacklight fail");
                return;
            }
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

void RSRenderServiceConnection::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (!mainThread_) {
        return;
    }
    auto registerBufferClearListener =
        [id, callback, weakThis = wptr<RSRenderServiceConnection>(this)]() -> bool {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
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
}

void RSRenderServiceConnection::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (!mainThread_) {
        return;
    }
    auto registerBufferAvailableListener =
        [id, callback, isFromRenderThread, weakThis = wptr<RSRenderServiceConnection>(this)]() -> bool {
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
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
}

int32_t RSRenderServiceConnection::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenSupportedColorGamuts(id, mode); }).get();
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

int32_t RSRenderServiceConnection::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &keys]() { return screenManager_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
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

int32_t RSRenderServiceConnection::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenColorGamut(id, mode); }).get();
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

int32_t RSRenderServiceConnection::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenColorGamut(id, modeIdx); }).get();
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

int32_t RSRenderServiceConnection::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenGamutMap(id, mode); }).get();
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

int32_t RSRenderServiceConnection::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenCorrection(id, screenRotation);
}

bool RSRenderServiceConnection::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return false;
    }
    return screenManager_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

bool RSRenderServiceConnection::SetGlobalDarkColorMode(bool isDark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return false;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), isDark]() {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            RS_LOGE("RSRenderServiceConnection::SetGlobalDarkColorMode fail");
            return;
        }
        connection->mainThread_->SetGlobalDarkColorMode(isDark);
    };
    mainThread_->PostTask(task);
    return true;
}

bool RSRenderServiceConnection::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return false;
    }
    return screenManager_->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

int32_t RSRenderServiceConnection::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenGamutMap(id, mode); }).get();
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

int32_t RSRenderServiceConnection::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSRenderServiceConnection::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &pixelFormat]() { return screenManager_->GetPixelFormat(id, pixelFormat); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &pixelFormat]() { return screenManager_->GetPixelFormat(id, pixelFormat); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetPixelFormat(id, pixelFormat); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetPixelFormat(id, pixelFormat); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &hdrFormats]() { return screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &hdrFormats]() { return screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &hdrFormat]() { return screenManager_->GetScreenHDRFormat(id, hdrFormat); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &hdrFormat]() { return screenManager_->GetScreenHDRFormat(id, hdrFormat); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenHDRFormat(id, modeIdx); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenHDRFormat(id, modeIdx); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &colorSpaces]() { return screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &colorSpaces]() { return screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &colorSpace]() { return screenManager_->GetScreenColorSpace(id, colorSpace); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=, &colorSpace]() { return screenManager_->GetScreenColorSpace(id, colorSpace); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenColorSpace(id, colorSpace); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenColorSpace(id, colorSpace); }).get();
    } else {
        return StatusCode::SCREEN_NOT_FOUND;
    }
}

int32_t RSRenderServiceConnection::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenType(id, screenType);
}

bool RSRenderServiceConnection::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    if (!mainThread_) {
        return false;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
    RSMainThread* mainThread = mainThread_;
#ifdef RS_ENABLE_GPU
    RSUniRenderThread* renderThread = &renderThread_;
    auto getBitmapTask = [id, &bitmap, mainThread, renderThread, &result]() {
        auto node = mainThread->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
        if (node == nullptr) {
            RS_LOGE("RSRenderServiceConnection::GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", id);
            result.set_value(false);
            return;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSRenderServiceConnection::GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
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
    return future.get();
}

bool RSRenderServiceConnection::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    if (!mainThread_) {
        return false;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
#ifdef RS_ENABLE_GPU
    RSMainThread* mainThread = mainThread_;
    RSUniRenderThread* renderThread = &renderThread_;
    auto getPixelMapTask = [id, pixelmap, rect, drawCmdList, mainThread, renderThread, &result]() {
        auto node = mainThread->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
        if (node == nullptr) {
            RS_LOGD("RSRenderServiceConnection::GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", id);
            result.set_value(false);
            return;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSRenderServiceConnection::GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
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
            RSTaskDispatcher::GetInstance().PostTask(
                RSSubThreadManager::Instance()->GetReThreadIndexMap()[tid], getDrawablePixelmapTask, false);
        }
    };
    mainThread_->PostTask(getPixelMapTask);
#endif
    return future.get();
}

bool RSRenderServiceConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    RS_LOGI("RSRenderServiceConnection:reg typeface, pid[%{public}d], familyname:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSRenderServiceConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    RS_LOGW("RSRenderServiceConnection:uneg typeface: pid[%{public}d], uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);
    return true;
}

int32_t RSRenderServiceConnection::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetDisplayIdentificationData(id, outPort, edidData);
}

int32_t RSRenderServiceConnection::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else {
        if (!mainThread_) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
    }
}

int32_t RSRenderServiceConnection::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

uint32_t RSRenderServiceConnection::SetScreenActiveRect(
    ScreenId id, const Rect& activeRect)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    GraphicIRect dstActiveRect {
        .x = activeRect.x,
        .y = activeRect.y,
        .w = activeRect.w,
        .h = activeRect.h,
    };
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [weakScreenManager = wptr<RSScreenManager>(screenManager_), id, dstActiveRect]() -> void {
        sptr<RSScreenManager> screenManager = weakScreenManager.promote();
        if (!screenManager) {
            return;
        }
        screenManager->SetScreenActiveRect(id, dstActiveRect);
    };
    mainThread_->ScheduleTask(task).wait();

    HgmTaskHandleThread::Instance().PostTask([id, dstActiveRect]() {
            OHOS::Rosen::HgmCore::Instance().NotifyScreenRectFrameRateChange(id, dstActiveRect);
    });
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterOcclusionChangeCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterSurfaceOcclusionChangeCallback(id, remotePid_, callback, partitionPoints);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterHgmConfigChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([this, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmConfigChangeCallback(remotePid_, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterHgmRefreshRateModeChangeCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterHgmRefreshRateModeChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([this, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateModeChangeCallback(remotePid_, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterHgmRefreshRateUpdateCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    HgmTaskHandleThread::Instance().PostSyncTask([this, &callback] () {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateUpdateCallback(remotePid_, callback);
    });
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    if (!mainThread_ || dstPid == 0) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [pid = remotePid_, dstPid, callback, weakThis = wptr<RSRenderServiceConnection>(this)]() {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (!connection || !connection->mainThread_) {
            return;
        }
        connection->mainThread_->GetContext().GetMutableFrameRateLinkerMap()
            .RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid, dstPid, callback);
    };
    mainThread_->PostTask(task);
    return StatusCode::SUCCESS;
}

void RSRenderServiceConnection::SetAppWindowNum(uint32_t num)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), num]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (!connection || !connection->mainThread_) {
            return;
        }
        connection->mainThread_->SetAppWindowNum(num);
    };
    mainThread_->PostTask(task);
}

bool RSRenderServiceConnection::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return false;
    }
#ifdef RS_ENABLE_GPU
    RSUifirstManager::Instance().OnProcessAnimateScene(systemAnimatedScenes);
    return mainThread_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
#else
    return false;
#endif
}

void RSRenderServiceConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), watermarkImg, isShow]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->ShowWatermark(watermarkImg, isShow);
    };
    mainThread_->PostTask(task);
}

int32_t RSRenderServiceConnection::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
#ifdef RS_ENABLE_GPU
        return RSHardwareThread::Instance().ScheduleTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), id, width, height]() -> int32_t {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
                if (connection == nullptr || connection->screenManager_ == nullptr) {
                    return RS_CONNECTION_ERROR;
                }
                return connection->screenManager_->ResizeVirtualScreen(id, width, height);
            }
        ).get();
#else
        return StatusCode::SCREEN_NOT_FOUND;
#endif
    } else if (mainThread_ != nullptr) {
        return mainThread_->ScheduleTask(
            [weakThis = wptr<RSRenderServiceConnection>(this), id, width, height]() -> int32_t {
                sptr<RSRenderServiceConnection> connection = weakThis.promote();
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

void RSRenderServiceConnection::ReportJankStats()
{
#ifdef RS_ENABLE_GPU
    auto task = []() -> void { RSJankStats::GetInstance().ReportJankStats(); };
    renderThread_.PostTask(task);
#endif
}

void RSRenderServiceConnection::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, lightFactorStatus]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleLightFactorStatus(pid, lightFactorStatus);
        }
    });
}

void RSRenderServiceConnection::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    mainThread_->NotifyPackageEvent(packageList);
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, listSize, packageList]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandlePackageEvent(pid, packageList);
        }
    });
}

void RSRenderServiceConnection::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, listSize, pkgName, newConfig] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleAppStrategyConfigEvent(pid, pkgName, newConfig);
        }
    });
}

void RSRenderServiceConnection::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    if (VOTER_SCENE_BLUR == eventInfo.eventName) {
        RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
        return;
    }

    HgmTaskHandleThread::Instance().PostTask([pid = remotePid_, eventInfo]() {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr != nullptr) {
            frameRateMgr->HandleRefreshRateEvent(pid, eventInfo);
        }
    });
}

void RSRenderServiceConnection::NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount)
{
    if (!appVSyncDistributor_) {
        return;
    }
    appVSyncDistributor_->SetQosVSyncRateByPidPublic(pid, rateDiscount, true);
}

void RSRenderServiceConnection::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    mainThread_->NotifyTouchEvent(touchStatus, touchCnt);
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        frameRateMgr->HandleTouchEvent(remotePid_, touchStatus, touchCnt);
    }
}

void RSRenderServiceConnection::NotifyDynamicModeEvent(bool enableDynamicModeEvent)
{
    HgmTaskHandleThread::Instance().PostTask([enableDynamicModeEvent] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            RS_LOGW("RSRenderServiceConnection::NotifyDynamicModeEvent: frameRateMgr is nullptr.");
            return;
        }
        frameRateMgr->HandleDynamicModeEvent(enableDynamicModeEvent);
    });
}

void RSRenderServiceConnection::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    HgmTaskHandleThread::Instance().PostTask([eventName, state] () {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            RS_LOGW("RSRenderServiceConnection::NotifyHgmConfigEvent: frameRateMgr is nullptr.");
            return;
        }
        if (eventName == "HGMCONFIG_HIGH_TEMP") {
            frameRateMgr->HandleThermalFrameRate(state);
        }
    });
}

void RSRenderServiceConnection::ReportEventResponse(DataBaseRs info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportEventResponse(info);
    };
#ifdef RS_ENABLE_GPU
    renderThread_.PostTask(task);
    RSUifirstManager::Instance().OnProcessEventResponse(info);
#endif
}

void RSRenderServiceConnection::ReportEventComplete(DataBaseRs info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportEventComplete(info);
    };
#ifdef RS_ENABLE_GPU
    renderThread_.PostTask(task);
    RSUifirstManager::Instance().OnProcessEventComplete(info);
#endif
}

void RSRenderServiceConnection::ReportEventJankFrame(DataBaseRs info)
{
#ifdef RS_ENABLE_GPU
    bool isReportTaskDelayed = renderThread_.IsMainLooping();
    auto task = [info, isReportTaskDelayed]() -> void {
        RSJankStats::GetInstance().SetReportEventJankFrame(info, isReportTaskDelayed);
    };
    renderThread_.PostTask(task);
#endif
}

void RSRenderServiceConnection::ReportRsSceneJankStart(AppInfo info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankStart(info);
    };
    renderThread_.PostTask(task);
}

void RSRenderServiceConnection::ReportRsSceneJankEnd(AppInfo info)
{
    auto task = [info]() -> void {
        RSJankStats::GetInstance().SetReportRsSceneJankEnd(info);
    };
    renderThread_.PostTask(task);
}

void RSRenderServiceConnection::ReportGameStateData(GameStateData info)
{
    RS_LOGD("RSRenderServiceConnection::ReportGameStateData = %{public}s, uid = %{public}d, state = %{public}d, "
            "pid = %{public}d renderTid = %{public}d ",
        info.bundleName.c_str(), info.uid, info.state, info.pid, info.renderTid);

    FrameReport::GetInstance().SetGameScene(info.pid, info.state);
}

void RSRenderServiceConnection::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), id, isEnabled, selfDrawingType,
        dynamicHardwareEnable]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
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
}

uint32_t RSRenderServiceConnection::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    if (!mainThread_) {
        return static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), id, needHidePrivacyContent]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
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
    return static_cast<uint32_t>(RSInterfaceErrorCode::NO_ERROR);
}

void RSRenderServiceConnection::SetCacheEnabledForRotation(bool isEnabled)
{
    if (!mainThread_) {
        return;
    }
    auto task = [isEnabled]() {
        RSSystemProperties::SetCacheEnabledForRotation(isEnabled);
    };
    mainThread_->PostTask(task);
}

std::vector<ActiveDirtyRegionInfo> RSRenderServiceConnection::GetActiveDirtyRegionInfo()
{
#ifdef RS_ENABLE_GPU
    const auto& activeDirtyRegionInfos = GpuDirtyRegionCollection::GetInstance().GetActiveDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetActiveDirtyRegionInfo();
    return activeDirtyRegionInfos;
#else
    return {};
#endif
}

GlobalDirtyRegionInfo RSRenderServiceConnection::GetGlobalDirtyRegionInfo()
{
#ifdef RS_ENABLE_GPU
    const auto& globalDirtyRegionInfo = GpuDirtyRegionCollection::GetInstance().GetGlobalDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetGlobalDirtyRegionInfo();
    return globalDirtyRegionInfo;
#else
    return {};
#endif
}

LayerComposeInfo RSRenderServiceConnection::GetLayerComposeInfo()
{
    const auto& layerComposeInfo = LayerComposeCollection::GetInstance().GetLayerComposeInfo();
    LayerComposeCollection::GetInstance().ResetLayerComposeInfo();
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSRenderServiceConnection::GetHwcDisabledReasonInfo()
{
    return HwcDisabledReasonCollection::GetInstance().GetHwcDisabledReasonInfo();
}

int64_t RSRenderServiceConnection::GetHdrOnDuration()
{
    auto rsHdrCollection = RsHdrCollection::GetInstance();
    if (rsHdrCollection == nullptr) {
        return -1;
    }
    int64_t duration = rsHdrCollection->GetHdrOnDuration();
    rsHdrCollection->ResetHdrOnDuration();
    return duration;
}

void RSRenderServiceConnection::SetVmaCacheStatus(bool flag)
{
#ifdef RS_ENABLE_GPU
    renderThread_.SetVmaCacheStatus(flag);
#endif
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceConnection::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    switch (tpFeatureConfigType) {
        case TpFeatureConfigType::DEFAULT_TP_FEATURE: {
            if (!TOUCH_SCREEN->IsSetFeatureConfigHandleValid()) {
                RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: SetFeatureConfigHandl is nullptr");
                return;
            }
            if (TOUCH_SCREEN->SetFeatureConfig(feature, config) < 0) {
                RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: SetFeatureConfig failed");
                return;
            }
            break;
        }
        case TpFeatureConfigType::AFT_TP_FEATURE: {
            if (!TOUCH_SCREEN->IsSetAftConfigHandleValid()) {
                RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: SetAftConfigHandl is nullptr");
                return;
            }
            if (TOUCH_SCREEN->SetAftConfig(config) < 0) {
                RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: SetAftConfig failed");
                return;
            }
            break;
        }
        default: {
            RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: unknown TpFeatureConfigType: %" PRIu8"",
                static_cast<uint8_t>(tpFeatureConfigType));
            return;
        }
    }
}
#endif

void RSRenderServiceConnection::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
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

void RSRenderServiceConnection::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), isCurtainScreenOn]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        connection->mainThread_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (!mainThread_) {
        return;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSRenderServiceConnection>(this), pidList]() {
            // don't use 'this' directly
            sptr<RSRenderServiceConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            connection->mainThread_->AddPidNeedDropFrame(pidList);
        }
    );
}

int32_t RSRenderServiceConnection::RegisterUIExtensionCallback(uint64_t userId, sptr<RSIUIExtensionCallback> callback,
    bool unobscured)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mainThread_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RSRenderServiceConnection::RegisterUIExtensionCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterUIExtensionCallback(remotePid_, userId, callback, unobscured);
    return StatusCode::SUCCESS;
}

bool RSRenderServiceConnection::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    if (!screenManager_) {
        return StatusCode::SCREEN_NOT_FOUND;
    }
    RS_LOGD("RSRenderServiceConnection::SetVirtualScreenStatus ScreenId:%{public}" PRIu64 " screenStatus:%{public}d",
        id, screenStatus);
    return screenManager_->SetVirtualScreenStatus(id, screenStatus);
}

bool RSRenderServiceConnection::SetAncoForceDoDirect(bool direct)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mainThread_ == nullptr) {
        return false;
    }
    mainThread_->SetAncoForceDoDirect(direct);
    return true;
}

void RSRenderServiceConnection::SetFreeMultiWindowStatus(bool enable)
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

void RSRenderServiceConnection::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid, callback);
}

void RSRenderServiceConnection::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
}

void RSRenderServiceConnection::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), nodeIdStr, isTop]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isTop](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                surfaceNode->SetLayerTop(isTop);
                return;
            }
        });
        // It can be displayed immediately after layer-top changed.
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::NotifyScreenSwitched()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenManager_) {
        RS_LOGE("RSRenderServiceConnection::NotifyScreenSwitched screenManager_ is nullptr");
        return;
    }
    RS_LOGI("RSRenderServiceConnection::NotifyScreenSwitched SetScreenSwitchStatus true");
    RS_TRACE_NAME_FMT("NotifyScreenSwitched");
    screenManager_->SetScreenSwitchStatus(true);
}

void RSRenderServiceConnection::SetWindowContainer(NodeId nodeId, bool value)
{
    if (!mainThread_) {
        return;
    }
    auto task = [weakThis = wptr<RSRenderServiceConnection>(this), nodeId, value]() -> void {
        sptr<RSRenderServiceConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& nodeMap = connection->mainThread_->GetContext().GetNodeMap();
        if (auto node = nodeMap.GetRenderNode<RSCanvasRenderNode>(nodeId)) {
            auto displayNodeId = node->GetDisplayNodeId();
            if (auto displayNode = nodeMap.GetRenderNode<RSDisplayRenderNode>(displayNodeId)) {
                RS_LOGD("RSRenderServiceConnection::SetWindowContainer nodeId: %{public}" PRIu64 ", value: %{public}d",
                    nodeId, value);
                displayNode->SetWindowContainer(value ? node : nullptr);
            } else {
                RS_LOGE("RSRenderServiceConnection::SetWindowContainer displayNode is nullptr, nodeId: %{public}"
                    PRIu64, displayNodeId);
            }
        } else {
            RS_LOGE("RSRenderServiceConnection::SetWindowContainer node is nullptr, nodeId: %{public}" PRIu64,
                nodeId);
        }
    };
    mainThread_->PostTask(task);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
int32_t RSRenderServiceConnection::SetOverlayDisplayMode(int32_t mode)
{
    RS_LOGI("RSRenderServiceConnection::SetOverlayDisplayMode: mode: [%{public}d]", mode);
    return RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(mode);
}
#endif
} // namespace Rosen
} // namespace OHOS
