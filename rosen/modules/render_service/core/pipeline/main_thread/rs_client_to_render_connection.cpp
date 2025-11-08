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

#include "rs_client_to_render_connection.h"

#include "frame_report.h"
#include "hgm_command.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
//blur predict
#include "rs_frame_blur_predict.h"
#include "rs_frame_report.h"
#include "rs_main_thread.h"
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
#include "memory/rs_memory_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_pointer_window_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_render_node_map.h"
#include "rs_render_service_listener.h"
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

#ifdef TP_FEATURE_ENABLE
#include "screen_manager/touch_screen.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#undef LOG_TAG
#define LOG_TAG "RSClientToRenderConnection"

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
RSClientToRenderConnection::RSClientToRenderConnection(
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
    if (token_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: Failed to set death recipient.");
    }
    if (renderService_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: renderService_ is nullptr");
    }
    if (mainThread_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: mainThread_ is nullptr");
    }
    if (screenManager_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: screenManager_ is nullptr");
    }
    if (appVSyncDistributor_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: appVSyncDistributor_ is nullptr");
    }
}

RSClientToRenderConnection::~RSClientToRenderConnection() noexcept
{
}

void RSClientToRenderConnection::CleanVirtualScreens() noexcept
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

void RSClientToRenderConnection::CleanRenderNodes() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();

    nodeMap.FilterNodeByPid(remotePid_);
}

void RSClientToRenderConnection::CleanFrameRateLinkers() noexcept
{
    if (mainThread_ == nullptr) {
        return;
    }
    auto& context = mainThread_->GetContext();
    auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();

    frameRateLinkerMap.FilterFrameRateLinkerByPid(remotePid_);
}

void RSClientToRenderConnection::CleanAll(bool toDelete) noexcept
{  
}

RSClientToRenderConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSClientToRenderConnection> conn) : conn_(conn)
{
}

void RSClientToRenderConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
}

RSClientToRenderConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSClientToRenderConnection> conn) : conn_(conn)
{}

void RSClientToRenderConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
}

ErrCode RSClientToRenderConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
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

ErrCode RSClientToRenderConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
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

ErrCode RSClientToRenderConnection::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    if (mainThread_ == nullptr) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    mainThread_->ScheduleTask(
        [info, mainThread = mainThread_]() {
            // don't use 'this' to get mainThread poninter
            mainThread->SetFocusAppInfo(info);
        }
    );
    repCode = SUCCESS;
    return ERR_OK;
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

    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(id);
    if (!node) {
        RS_LOGE("TakeSurfaceCaptureForUiParallel node is nullptr");
        callback->OnSurfaceCapture(id, captureConfig, nullptr);
        return;
    }

    auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode) {
        std::lock_guard<std::mutex> lock(surfaceNode->GetCaptureUiFirstMutex());
        surfaceNode->SetCaptureEnableUifirst(true);
    }
    bool captureNodeDirty = (node->IsDirty() || node->IsSubTreeDirty() || !node->IsOnTheTree());
    if (captureConfig.isSync || captureNodeDirty) {
        if (captureConfig.isSync) {
            context.GetUiCaptureHelper().InsertUiCaptureCmdsExecutedFlag(id, false);
        }
        RSMainThread::Instance()->AddUiCaptureTask(id, captureTask);
        return;
    }
    RSMainThread::Instance()->PostTask(captureTask);
#endif
}

void TakeSurfaceCaptureForUIWithUni(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
#ifdef RS_ENABLE_GPU
    std::function<void()> offscreenRenderTask = [id, callback, captureConfig]() -> void {
        RS_LOGD("RSClientToRenderConnection::TakeSurfaceCaptureForUIWithUni callback->OnOffscreenRender"
            " nodeId:[%{public}" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSClientToRenderConnection::TakeSurfaceCaptureForUIWithUni");
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

void RSClientToRenderConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
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
        RS_TRACE_NAME_FMT("RSClientToRenderConnection::TakeSurfaceCapture captureTask nodeId:[%" PRIu64 "]", id);
        RS_LOGD("TakeSurfaceCapture captureTask begin nodeId:[%{public}" PRIu64 "]", id);
        if (captureConfig.captureType == SurfaceCaptureType::UICAPTURE) {
            // When the isSync flag in captureConfig is true, UI capture processes commands before capture.
            // When the isSync flag in captureConfig is false, UI capture will check null node independently.
            // Therefore, a null node is valid for UI capture.
            auto uiCaptureHasPermission = selfCapture || isSystemCalling;
            if (!uiCaptureHasPermission) {
                RS_LOGE("TakeSurfaceCapture uicapture failed, nodeId:[%{public}" PRIu64
                        "], isSystemCalling: %{public}u, selfCapture: %{public}u",
                    id, isSystemCalling, selfCapture);
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
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
            RS_LOGE("TakeSurfaceCapture failed, node is nullptr");
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
            return;
        }
        auto displayCaptureHasPermission = screenCapturePermission && isSystemCalling;
        auto surfaceCaptureHasPermission = blurParam.isNeedBlur ? isSystemCalling : (selfCapture || isSystemCalling);
        if ((node->GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE && !displayCaptureHasPermission) ||
            (node->GetType() == RSRenderNodeType::SURFACE_NODE && !surfaceCaptureHasPermission)) {
            RS_LOGE("TakeSurfaceCapture failed, node type: %{public}u, "
                "screenCapturePermission: %{public}u, isSystemCalling: %{public}u, selfCapture: %{public}u",
                node->GetType(), screenCapturePermission, isSystemCalling, selfCapture);
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
            return;
        }
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_DISABLED) {
            RS_LOGD("RSClientToRenderConnection::TakeSurfaceCapture captureTaskInner nodeId:[%{public}" PRIu64 "]", id);
            ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSClientToRenderConnection::TakeSurfaceCapture");
            RSSurfaceCaptureTask task(id, captureConfig);
            if (!task.Run(callback)) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        } else {
#ifdef RS_ENABLE_GPU
            RSSurfaceCaptureParam captureParam;
            captureParam.id = id;
            captureParam.config = captureConfig;
            captureParam.isSystemCalling = isSystemCalling;
            captureParam.blurParam = blurParam;
            RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
            RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
#endif
        }
    };
    mainThread_->PostTask(captureTask);
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSClientToRenderConnection::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions permissions)
{
    RS_LOGI("TakeSurfaceCaptureSoloNode nodeId:[%{public}" PRIu64 "]", id);
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    std::function<void()> captureTask = [id, captureConfig, &pixelMapIdPairVector,
        isSystemCalling = permissions.isSystemCalling,
        selfCapture = permissions.selfCapture]() {
        RS_TRACE_NAME_FMT("RSClientToRenderConnection::TakeSurfaceCaptureSoloNode captureTask"
            " nodeId:[%" PRIu64 "]", id);
        RS_LOGI("TakeSurfaceCaptureSoloNode captureTask begin "
            "nodeId:[%{public}" PRIu64 "]", id);
        auto uiCaptureHasPermission = selfCapture || isSystemCalling;
        if (!uiCaptureHasPermission) {
            RS_LOGE("TakeSurfaceCaptureSoloNode "
                "uicapturesolo failed, nodeId:[%{public}" PRIu64
                "], isSystemCalling: %{public}u, selfCapture: %{public}u", id, isSystemCalling, selfCapture);
            return;
        }
        pixelMapIdPairVector = RSUiCaptureSoloTaskParallel::CaptureSoloNode(id, captureConfig);
    };
    RSMainThread::Instance()->PostSyncTask(captureTask);
    return pixelMapIdPairVector;
}

void RSClientToRenderConnection::TakeSelfSurfaceCapture(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr", __func__);
        return;
    }
    bool isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + "::TAKE_SELF_SURFACE_CAPTURE");
    std::function<void()> selfCaptureTask = [id, callback, captureConfig, isSystemCalling]() -> void {
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
        if (node == nullptr) {
            RS_LOGE("TakeSelfSurfaceCapture failed, node is nullptr");
            if (callback) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            return;
        }
        RSSurfaceCaptureParam captureParam;
        captureParam.id = id;
        captureParam.config = captureConfig;
        captureParam.isSystemCalling = isSystemCalling;
        captureParam.isSelfCapture = true;
        RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
        RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
    };
    mainThread_->PostTask(selfCaptureTask);
}

ErrCode RSClientToRenderConnection::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions permissions)
{
    bool hasPermission = permissions.screenCapturePermission && permissions.isSystemCalling;
    if (!mainThread_ || !hasPermission) {
        if (callback) {
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
        }
        RS_LOGE("%{public}s mainThread_ is nullptr or permission denied", __func__);
        return ERR_PERMISSION_DENIED;
    }
    RS_TRACE_NAME_FMT("TaskSurfaceCaptureWithAllWindows checkDrmAndSurfaceLock: %d", checkDrmAndSurfaceLock);
    std::function<void()> takeSurfaceCaptureTask =
        [id, checkDrmAndSurfaceLock, callback, captureConfig, hasPermission]() -> void {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(
            RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id));
        if (!displayNode) {
            if (callback) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            RS_LOGE("%{public}s failed, displayNode is nullptr", __func__);
            return;
        }
        if (checkDrmAndSurfaceLock && RSMainThread::Instance()->HasDRMOrSurfaceLockLayer()) {
            if (callback) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            RS_LOGE("%{public}s DRM or surfacelock layer exists", __func__);
            return;
        }
        RSSurfaceCaptureParam captureParam;
        captureParam.id = id;
        captureParam.config = captureConfig;
        captureParam.isSystemCalling = hasPermission;
        captureParam.needCaptureSpecialLayer = hasPermission;
        RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
        RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
    };
    mainThread_->PostTask(takeSurfaceCaptureTask);
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::FreezeScreen(NodeId id, bool isFreeze)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr", __func__);
        return ERR_INVALID_OPERATION;
    }
    RS_TRACE_NAME_FMT("FreezeScreen isFreeze: %d", isFreeze);
    std::function<void()> setScreenFreezeTask = [id, isFreeze]() -> void {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(
            RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id));
        auto screenNode = displayNode == nullptr ? nullptr :
            std::static_pointer_cast<RSScreenRenderNode>(displayNode->GetParent().lock());
        if (!screenNode) {
            RS_LOGE("%{public}s failed, screenNode is nullptr", __func__);
            return;
        }
        screenNode->SetForceFreeze(isFreeze);

        // cancel previous task, and unfreeze screen after 3 seconds in case unfreeze fail
        std::string taskName(UNFREEZE_SCREEN_TASK_NAME + std::to_string(id));
        RSMainThread::Instance()->RemoveTask(taskName);
        if (!isFreeze) {
            return;
        }
        auto unfreezeScreenTask = [screenId = screenNode->GetId()]() -> void {
            RS_LOGW("unfreeze screen[%{public}" PRIu64 "] by timer", screenId);
            RS_TRACE_NAME_FMT("UnfreezeScreen[%lld]", screenId);
            auto screenNode = RSBaseRenderNode::ReinterpretCast<RSScreenRenderNode>(
                RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(screenId));
            if (screenNode) {
                screenNode->SetForceFreeze(false);
            }
        };
        RSMainThread::Instance()->PostTask(unfreezeScreenTask, taskName, MAX_FREEZE_SCREEN_TIME,
            AppExecFwk::EventQueue::Priority::IMMEDIATE);
    };
    mainThread_->PostTask(setScreenFreezeTask);
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    bool isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + "::SET_WINDOW_FREEZE_IMMEDIATELY");
    std::function<void()> setWindowFreezeTask =
        [id, isFreeze, callback, captureConfig, blurParam, isSystemCalling]() -> void {
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
        if (node == nullptr) {
            RS_LOGE("SetWindowFreezeImmediately failed, node is nullptr");
            if (callback) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            return;
        }
        node->SetStaticCached(isFreeze);
        if (isFreeze) {
            RSSurfaceCaptureParam captureParam;
            captureParam.id = id;
            captureParam.config = captureConfig;
            captureParam.isSystemCalling = isSystemCalling;
            captureParam.isFreeze = isFreeze;
            captureParam.blurParam = blurParam;
            RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
            RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
        } else {
            RSSurfaceCaptureTaskParallel::ClearCacheImageByFreeze(id);
        }
    };
    mainThread_->PostTask(setWindowFreezeTask);
    return ERR_OK;
}

void RSClientToRenderConnection::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    std::function<void()> captureTask = [id, callback, captureConfig]() -> void {
        RS_TRACE_NAME_FMT("RSClientToRenderConnection::TakeUICaptureInRange captureTask nodeId:[%" PRIu64 "]", id);
        RS_LOGD("RSClientToRenderConnection::TakeUICaptureInRange captureTask nodeId:[%{public}" PRIu64 "]", id);
        TakeSurfaceCaptureForUiParallel(id, callback, captureConfig, {});
    };
    RSMainThread::Instance()->PostTask(captureTask);
}

ErrCode RSClientToRenderConnection::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (mainThread_ == nullptr || screenManager_ == nullptr) {
        return ERR_INVALID_VALUE;
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
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    HdrStatus hdrStatusRet = HdrStatus::NO_HDR;
    StatusCode resCodeRet = StatusCode::SCREEN_NOT_FOUND;
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    auto task = [id, mainThread = mainThread_, &resCodeRet, &hdrStatusRet, isTimeoutWeak]() {
        if (isTimeoutWeak.expired()) {
            RS_LOGE("GetScreenHDRStatus time out, ScreenId: [%{public}" PRIu64 "]", id);
            return;
        }
        std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
        auto& nodeMap = mainThread->GetContext().GetNodeMap();
        nodeMap.TraverseScreenNodes([id, &screenNode](const std::shared_ptr<RSScreenRenderNode>& node) {
            if (node && node->GetScreenId() == id) {
                screenNode = node;
            }
        });
        if (screenNode == nullptr) {
            resCodeRet = StatusCode::SCREEN_NOT_FOUND;
            return;
        }
        hdrStatusRet = screenNode->GetLastDisplayHDRStatus();
        resCodeRet = StatusCode::SUCCESS;
    };
    auto span = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(100)); // timeout 100 ms
    if (mainThread_->ScheduleTask(task).wait_for(span) == std::future_status::timeout) {
        isTimeout.reset();
    }
    if (isTimeoutWeak.expired() && resCodeRet != StatusCode::SUCCESS) {
        return ERR_TIMED_OUT;
    }
    if (resCodeRet == StatusCode::SUCCESS) {
        hdrStatus = hdrStatusRet;
    }
    resCode = resCodeRet;
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    mainThread_->ScheduleTask(
        [weakThis = wptr<RSClientToRenderConnection>(this), pidList]() {
            // don't use 'this' directly
            sptr<RSClientToRenderConnection> connection = weakThis.promote();
            if (connection == nullptr || connection->mainThread_ == nullptr) {
                return;
            }
            connection->mainThread_->AddPidNeedDropFrame(pidList);
        }
    );
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::SetAncoForceDoDirect(bool direct, bool& res)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mainThread_ == nullptr) {
        res = false;
        return ERR_INVALID_VALUE;
    }
    mainThread_->SetAncoForceDoDirect(direct);
    res = true;
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid, callback);
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    if (mainThread_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToRenderConnection>(this), nodeId, isTop, zOrder]() -> void {
        sptr<RSClientToRenderConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
        if (surfaceNode == nullptr) {
            return;
        }
        surfaceNode->SetLayerTop(isTop, false);
        surfaceNode->SetTopLayerZOrder(zOrder);
        // It can be displayed immediately after layer-top changed.
        connection->mainThread_->SetDirtyFlag();
        connection->mainThread_->RequestNextVSync();
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

void RSClientToRenderConnection::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    RSTransactionDataCallbackManager::Instance().RegisterTransactionDataCallback(token, timeStamp, callback);
}

ErrCode RSClientToRenderConnection::SetWindowContainer(NodeId nodeId, bool value)
{
    if (!mainThread_) {
        return ERR_INVALID_VALUE;
    }
    auto task = [weakThis = wptr<RSClientToRenderConnection>(this), nodeId, value]() -> void {
        sptr<RSClientToRenderConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& nodeMap = connection->mainThread_->GetContext().GetNodeMap();
        if (auto node = nodeMap.GetRenderNode<RSCanvasRenderNode>(nodeId)) {
            auto displayNodeId = node->GetLogicalDisplayNodeId();
            if (auto displayNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(displayNodeId)) {
                RS_LOGD("SetWindowContainer nodeId: %{public}" PRIu64 ", value: %{public}d",
                    nodeId, value);
                displayNode->SetWindowContainer(value ? node : nullptr);
            } else {
                RS_LOGE("SetWindowContainer displayNode is nullptr, nodeId: %{public}"
                    PRIu64, displayNodeId);
            }
        } else {
            RS_LOGE("SetWindowContainer node is nullptr, nodeId: %{public}" PRIu64,
                nodeId);
        }
    };
    mainThread_->PostTask(task);
    return ERR_OK;
}

void RSClientToRenderConnection::ClearUifirstCache(NodeId id)
{
    if (!mainThread_) {
        return;
    }
    auto task = [id]() -> void {
        RSUifirstManager::Instance().AddMarkedClearCacheNode(id);
    };
    mainThread_->PostTask(task);
}
} // namespace Rosen
} // namespace OHOS
