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

#include "rs_render_pipeline_agent.h"
#include "common/rs_background_thread.h"
#include "command/rs_command_verify_helper.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_layer_transaction_data.h"
#include "screen_manager/screen_types.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#ifdef RS_ENABLE_GPU
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/capture/rs_ui_capture.h"
#endif
#include "feature/capture/rs_uni_ui_capture.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "feature/capture/rs_ui_capture_solo_task_parallel.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "feature/special_layer/rs_special_layer_utils.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "memory/rs_canvas_dma_buffer_cache.h"
#endif
#include "core/dfx/rs_pipline_dump_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "rs_profiler.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pixel_map_from_surface.h"
#include "render/rs_typeface_cache.h"
#include "system/rs_system_parameters.h"
#include "transaction/rs_unmarshal_thread.h"
#include "transaction/rs_transaction_data_callback_manager.h"
#include "pipeline/rs_render_node_gc.h"
#include "app_mgr_client.h"
#undef LOG_TAG
#define LOG_TAG "RSRenderPipelineAgent"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int SLEEP_TIME_US = 1000;
const std::string REGISTER_NODE = "RegisterNode";
constexpr uint32_t MEM_BYTE_TO_MB = 1024 * 1024;
constexpr uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;
constexpr uint32_t PIDLIST_SIZE_MAX = 128;
constexpr uint64_t MAX_TIME_OUT_NS = 1e9;
constexpr int64_t MAX_FREEZE_SCREEN_TIME = 3000;
const std::string UNFREEZE_SCREEN_TASK_NAME = "UNFREEZE_SCREEN_TASK";
}

RSRenderPipelineAgent::RSRenderPipelineAgent(std::shared_ptr<RSRenderPipeline>& renderPipeline) :
    rsRenderPipeline_(renderPipeline) {}


bool RSRenderPipelineAgent::RemoveConnection(const sptr<RSIConnectionToken>& token)
{
    return rsRenderPipeline_->RemoveConnection(token);
}

ErrCode RSRenderPipelineAgent::CommitTransaction(pid_t callingPid, bool isTokenTypeValid,
    bool isNonSystemAppCalling, std::unique_ptr<RSTransactionData>& transactionData)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    bool shouldDrop = RSUnmarshalThread::Instance().ReportTransactionDataStatistics(
        callingPid, transactionData.get(), isNonSystemAppCalling);
    if (shouldDrop) {
        RS_LOGW("CommitTransaction data droped");
        return ERR_INVALID_VALUE;
    }
    if (transactionData && transactionData->GetDVSyncUpdate()) {
        rsRenderPipeline_->GetMainThread()->DVSyncUpdate(transactionData->GetDVSyncTime(), transactionData->GetTimestamp());
    }
    bool isProcessBySingleFrame = rsRenderPipeline_->GetMainThread()->IsNeedProcessBySingleFrameComposer(transactionData);
    if (isProcessBySingleFrame) {
        rsRenderPipeline_->GetMainThread()->ProcessDataBySingleFrameComposer(transactionData);
    } else {
        rsRenderPipeline_->GetMainThread()->RecvRSTransactionData(transactionData);
    }
    return ERR_OK;
}


void RSRenderPipelineAgent::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent rsRenderPipeline_ or  is null!");
        return;
    }

    rsRenderPipeline_->ScheduleMainThreadTask([mainThread = rsRenderPipeline_->GetMainThread(), app]() {
        if (mainThread == nullptr) {
            RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent mainThread or is null!");
            return;
        }
        mainThread->UnRegisterApplicationAgent(app);
    }).wait();
}

ErrCode RSRenderPipelineAgent::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr || rsRenderPipeline_ == nullptr) {
        RS_LOGW("ExecuteSynchronousTask, task or RenderPipeline is null!");
        return ERR_INVALID_VALUE;
    }
    // After a synchronous task times out, it will no longer be executed.
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    std::chrono::nanoseconds span(std::min(task->GetTimeout(), MAX_TIME_OUT_NS));
    rsRenderPipeline_->ScheduleMainThreadTask([task, mainThread = rsRenderPipeline_->GetMainThread(), isTimeoutWeak] {
        if (task == nullptr || mainThread == nullptr || isTimeoutWeak.expired()) {
            return;
        }
        task->Process(mainThread->GetContext());
    }).wait_for(span);
    isTimeout.reset();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    auto node = DisplayNodeCommandHelper::CreateWithConfigInRS(
        rsRenderPipeline_->GetMainThread()->GetContext(), nodeId, displayNodeConfig);
    if (node == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::CreateDisplayNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [renderPipeline = rsRenderPipeline_, nodeId, node, &displayNodeConfig]() {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto& nodeMap = context.GetMutableNodeMap();
        nodeMap.RegisterRenderNode(node);
        nodeMap.TraverseScreenNodes([&node, id = node->GetScreenId()](auto& screenNode) {
            if (!screenNode || screenNode->GetScreenId() != id) {
                return;
            }
            screenNode->AddChild(node);
        });

        DisplayNodeCommandHelper::SetDisplayMode(context, nodeId, displayNodeConfig);
    };
    rsRenderPipeline_->PostMainThreadSyncTask(registerNode);
    success = true;
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ForceRefreshOneFrameWithNextVSync()
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_]() -> void {
        RS_LOGI("ForceRefreshOneFrameWithNextVSync, setDirtyflag, forceRefresh in mainThread");
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->RequestNextVSync();
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetAppWindowNum(uint32_t num)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, num]() -> void {
        renderPipeline->GetMainThread()->SetAppWindowNum(num);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK; 
}

ErrCode RSRenderPipelineAgent::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, rsRenderPipeline_->GetMainThread()->GetContext());
    if (node == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::CreateNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [renderPipeline = rsRenderPipeline_, node]() -> void {
        renderPipeline->GetMainThread()->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    rsRenderPipeline_->PostMainThreadTask(registerNode);
    success = true;
    return ERR_OK;
}

void RSRenderPipelineAgent::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto captureTask = [renderPipeline = rsRenderPipeline_, pid, app]() -> void {
        renderPipeline->GetMainThread()->RegisterApplicationAgent(pid, app);
    };
    rsRenderPipeline_->PostMainThreadTask(captureTask);
}

ErrCode RSRenderPipelineAgent::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferClearListener = [renderPipeline = rsRenderPipeline_, id, callback]() -> bool {
        if (auto node =
                renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferClearListener(callback);
            return true;
        }
        return false;
    };
    rsRenderPipeline_->PostMainThreadTask(registerBufferClearListener);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferAvailableListener = [renderPipeline = rsRenderPipeline_, id, callback,
                                               isFromRenderThread]() -> bool {
        if (auto node =
                renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferAvailableListener(callback, isFromRenderThread);
            return true;
        }
        return false;
    };
    rsRenderPipeline_->PostMainThreadTask(registerBufferAvailableListener);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetGlobalDarkColorMode(bool isDark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, isDark]() {
        renderPipeline->GetMainThread()->SetGlobalDarkColorMode(isDark);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    RSUifirstManager::Instance().OnProcessAnimateScene(systemAnimatedScenes);
    success = rsRenderPipeline_->GetMainThread()->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, id, isEnabled, selfDrawingType, dynamicHardwareEnable]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHardwareEnabled(isEnabled, selfDrawingType, dynamicHardwareEnable);
        }
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode)
{
    if (rsRenderPipeline_ == nullptr) {
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, id, needHidePrivacyContent]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHidePrivacyContent(needHidePrivacyContent);
        }
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    resCode = static_cast<uint32_t>(RSInterfaceErrorCode::NO_ERROR);
    return ERR_OK;
}

bool RSRenderPipelineAgent::GetHighContrastTextState()
{
    if (rsRenderPipeline_ == nullptr) {
        return false;
    }
    return RSBaseRenderEngine::IsHighContrastEnabled();
}

ErrCode RSRenderPipelineAgent::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    if (rsRenderPipeline_ == nullptr) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    rsRenderPipeline_->ScheduleMainThreadTask(
        [info, mainThread = rsRenderPipeline_->GetMainThread()]() {
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
        RS_LOGD("RSRenderPipelineAgent::TakeSurfaceCaptureForUIWithUni callback->OnOffscreenRender"
            " nodeId:[%{public}" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderPipelineAgent::TakeSurfaceCaptureForUIWithUni");
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

void RSRenderPipelineAgent::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect, RSSurfaceCapturePermissions permissions)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    std::function<void()> captureTask =
        [renderPipeline = rsRenderPipeline_, id, callback, captureConfig, blurParam, specifiedAreaRect,
            screenCapturePermission = permissions.screenCapturePermission,
            isSystemCalling = permissions.isSystemCalling, selfCapture = permissions.selfCapture]() -> void {
        RS_TRACE_NAME_FMT("RSRenderPipelineAgent::TakeSurfaceCapture captureTask nodeId:[%" PRIu64 "]", id);
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
        auto node = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(id);
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
            RS_LOGD("RSRenderPipelineAgent::TakeSurfaceCapture captureTaskInner nodeId:[%{public}" PRIu64 "]", id);
            ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderPipelineAgent::TakeSurfaceCapture");
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
    rsRenderPipeline_->PostMainThreadTask(captureTask);
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderPipelineAgent::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions permissions)
{
    RS_LOGI("TakeSurfaceCaptureSoloNode nodeId:[%{public}" PRIu64 "]", id);
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    std::function<void()> captureTask = [id, captureConfig, &pixelMapIdPairVector,
        isSystemCalling = permissions.isSystemCalling,
        selfCapture = permissions.selfCapture]() {
        RS_TRACE_NAME_FMT("RSRenderPipelineAgent::TakeSurfaceCaptureSoloNode captureTask"
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
    rsRenderPipeline_->PostMainThreadSyncTask(captureTask);
    return pixelMapIdPairVector;
}

void RSRenderPipelineAgent::TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, bool isSystemCalling)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    std::function<void()> selfCaptureTask = [renderPipeline = rsRenderPipeline_, id, callback, captureConfig,
                                                isSystemCalling]() -> void {
        auto node = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(id);
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
    rsRenderPipeline_->PostMainThreadTask(selfCaptureTask);
}

ErrCode RSRenderPipelineAgent::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions permissions)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    bool hasPermission = permissions.screenCapturePermission && permissions.isSystemCalling;
    if (!hasPermission) {
        if (callback) {
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
        }
        RS_LOGE("%{public}s permission denied", __func__);
        return ERR_PERMISSION_DENIED;
    }
    RS_TRACE_NAME_FMT("TaskSurfaceCaptureWithAllWindows checkDrmAndSurfaceLock: %d", checkDrmAndSurfaceLock);
    std::function<void()> takeSurfaceCaptureTask = [renderPipeline = rsRenderPipeline_, id, checkDrmAndSurfaceLock,
                                                       callback, captureConfig, hasPermission]() -> void {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(
            renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(id));
        if (!displayNode) {
            if (callback) {
                callback->OnSurfaceCapture(id, captureConfig, nullptr);
            }
            RS_LOGE("%{public}s failed, displayNode is nullptr", __func__);
            return;
        }
        if (checkDrmAndSurfaceLock && renderPipeline->GetMainThread()->HasDRMOrSurfaceLockLayer()) {
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
    rsRenderPipeline_->PostMainThreadTask(takeSurfaceCaptureTask);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::FreezeScreen(NodeId id, bool isFreeze)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    RS_TRACE_NAME_FMT("FreezeScreen isFreeze: %d", isFreeze);
    std::function<void()> setScreenFreezeTask = [renderPipeline = rsRenderPipeline_, id, isFreeze]() -> void {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSLogicalDisplayRenderNode>(
            renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(id));
        auto screenNode = displayNode == nullptr ? nullptr :
            std::static_pointer_cast<RSScreenRenderNode>(displayNode->GetParent().lock());
        if (!screenNode) {
            RS_LOGE("%{public}s failed, screenNode is nullptr", __func__);
            return;
        }
        screenNode->SetForceFreeze(isFreeze);

        // cancel previous task, and unfreeze screen after 3 seconds in case unfreeze fail
        std::string taskName(UNFREEZE_SCREEN_TASK_NAME + std::to_string(id));
        renderPipeline->GetMainThread()->RemoveTask(taskName);
        if (!isFreeze) {
            return;
        }
        auto unfreezeScreenTask = [renderPipeline, screenId = screenNode->GetId()]() -> void {
            RS_LOGW("unfreeze screen[%{public}" PRIu64 "] by timer", screenId);
            RS_TRACE_NAME_FMT("UnfreezeScreen[%lld]", screenId);
            auto screenNode = RSBaseRenderNode::ReinterpretCast<RSScreenRenderNode>(
                renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(screenId));
            if (screenNode) {
                screenNode->SetForceFreeze(false);
            }
        };
        renderPipeline->GetMainThread()->PostTask(unfreezeScreenTask, taskName, MAX_FREEZE_SCREEN_TIME,
            AppExecFwk::EventQueue::Priority::IMMEDIATE);
    };
    rsRenderPipeline_->PostMainThreadTask(setScreenFreezeTask);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam, bool isSystemCalling)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    std::function<void()> setWindowFreezeTask = [renderPipeline = rsRenderPipeline_, id, isFreeze, callback,
                                                    captureConfig, blurParam, isSystemCalling]() -> void {
        auto node = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(id);
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
    rsRenderPipeline_->PostMainThreadTask(setWindowFreezeTask);
    return ERR_OK;
}

void RSRenderPipelineAgent::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    std::function<void()> captureTask = [id, callback, captureConfig]() -> void {
        RS_TRACE_NAME_FMT("RSRenderPipelineAgent::TakeUICaptureInRange captureTask nodeId:[%" PRIu64 "]", id);
        RS_LOGD("RSRenderPipelineAgent::TakeUICaptureInRange captureTask nodeId:[%{public}" PRIu64 "]", id);
        TakeSurfaceCaptureForUiParallel(id, callback, captureConfig, {});
    };
    rsRenderPipeline_->PostMainThreadTask(captureTask);
}

ErrCode RSRenderPipelineAgent::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    // adapt video scene pointer
    if (!RSPointerWindowManager::Instance().GetIsPointerEnableHwc()) {
        // when has virtual screen or pointer is enable hwc, we can't skip
        RSPointerWindowManager::Instance().SetIsPointerCanSkipFrame(false);
        RSMainThread::Instance()->RequestNextVSync();
    } else {
        RSPointerWindowManager::Instance().SetIsPointerCanSkipFrame(true);
    }

    RSPointerWindowManager::Instance().SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode)
{
    if (rsRenderPipeline_ == nullptr) {
        resCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    HdrStatus hdrStatusRet = HdrStatus::NO_HDR;
    StatusCode resCodeRet = StatusCode::SCREEN_NOT_FOUND;
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    auto task = [id, renderPipeline = rsRenderPipeline_, &resCodeRet, &hdrStatusRet, isTimeoutWeak]() {
        if (isTimeoutWeak.expired()) {
            RS_LOGE("GetScreenHDRStatus time out, ScreenId: [%{public}" PRIu64 "]", id);
            return;
        }
        std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
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
    if (rsRenderPipeline_->GetMainThread()->ScheduleTask(task).wait_for(span) == std::future_status::timeout) {
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

ErrCode RSRenderPipelineAgent::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    rsRenderPipeline_->ScheduleMainThreadTask(
        [renderPipeline = rsRenderPipeline_, pidList]() {
            // don't use 'this' directly
            renderPipeline->GetMainThread()->AddPidNeedDropFrame(pidList);
        }
    );
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetAncoForceDoDirect(bool direct, bool& res)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rsRenderPipeline_ == nullptr) {
        res = false;
        return ERR_INVALID_VALUE;
    }
    rsRenderPipeline_->GetMainThread()->SetAncoForceDoDirect(direct);
    res = true;
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid, callback);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, nodeId, isTop, zOrder]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
        if (surfaceNode == nullptr) {
            return;
        }
        surfaceNode->SetLayerTop(isTop, false);
        surfaceNode->SetTopLayerZOrder(zOrder);
        // It can be displayed immediately after layer-top changed.
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->RequestNextVSync();
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    RSTransactionDataCallbackManager::Instance().RegisterTransactionDataCallback(token, timeStamp, callback);
}

ErrCode RSRenderPipelineAgent::SetWindowContainer(NodeId nodeId, bool value)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, nodeId, value]() -> void {
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
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
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::ClearUifirstCache(NodeId id)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [id]() -> void {
        RSUifirstManager::Instance().AddMarkedClearCacheNode(id);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderPipelineAgent::RegisterCanvasCallback(pid_t remotePid, sptr<RSICanvasSurfaceBufferCallback> callback)
{
    RSCanvasDmaBufferCache::GetInstance().RegisterCanvasCallback(remotePid, callback);
}

int32_t RSRenderPipelineAgent::SubmitCanvasPreAllocatedBuffer(
    pid_t remotePid, NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    if (rsRenderPipeline_ == nullptr) {
        return INVALID_ARGUMENTS;
    }
    if (ExtractPid(nodeId) != remotePid) {
        RS_LOGE("SubmitCanvasPreAllocatedBuffer: Illegal pid, nodeId=%{public}" PRIu64 ", pid=%{public}d",
            nodeId, remotePid);
        return INVALID_ARGUMENTS;
    }
    auto task = [nodeId, buffer, resetSurfaceIndex]() {
        bool success = RSCanvasDmaBufferCache::GetInstance().AddPendingBuffer(nodeId, buffer, resetSurfaceIndex);
        return success ? SUCCESS : INVALID_ARGUMENTS;
    };
    return rsRenderPipeline_->GetMainThread()->ScheduleTask(task).get();
}
#endif

ErrCode RSRenderPipelineAgent::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, isCurtainScreenOn]() -> void {
        renderPipeline->GetMainThread()->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
    RSMainThread* mainThread = rsRenderPipeline_->GetMainThread();
#ifdef RS_ENABLE_GPU
    RSUniRenderThread* renderThread = rsRenderPipeline_->GetUniRenderThread();
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
    rsRenderPipeline_->PostMainThreadTask(getBitmapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
#ifdef RS_ENABLE_GPU
    RSMainThread* mainThread = rsRenderPipeline_->GetMainThread();
    RSUniRenderThread* renderThread = rsRenderPipeline_->GetUniRenderThread();
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
    rsRenderPipeline_->PostUniRenderThreadTask(getPixelMapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportJankStats()
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = []() -> void { RSJankStats::GetInstance().ReportJankStats(); };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportEventResponse(DataBaseRs info)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, info]() -> void {
        RSJankStats::GetInstance().SetReportEventResponse(info);
        renderPipeline->GetMainThread()->SetForceRsDVsync(info.sceneId);
    };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    RSUifirstManager::Instance().OnProcessEventResponse(info);
    RSUifirstFrameRateControl::Instance().SetAnimationStartInfo(info);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportEventComplete(DataBaseRs info)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void { RSJankStats::GetInstance().SetReportEventComplete(info); };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    RSUifirstManager::Instance().OnProcessEventComplete(info);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportEventJankFrame(DataBaseRs info)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    bool isReportTaskDelayed = rsRenderPipeline_->GetUniRenderThread()->IsMainLooping();
    auto task = [info, isReportTaskDelayed]() -> void {
        RSJankStats::GetInstance().SetReportEventJankFrame(info, isReportTaskDelayed);
    };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    RSUifirstFrameRateControl::Instance().SetAnimationEndInfo(info);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankStart(AppInfo info)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void { RSJankStats::GetInstance().SetReportRsSceneJankStart(info); };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankEnd(AppInfo info)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [info]() -> void { RSJankStats::GetInstance().SetReportRsSceneJankEnd(info); };
    rsRenderPipeline_->PostUniRenderThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [uniqueIdList, surfaceNameList, fps, reportTime]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [uniqueIdList, surfaceNameList, fps]() -> void {
        RSJankStats::GetInstance().AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node = SurfaceNodeCommandHelper::CreateWithConfigInRS(
        config, rsRenderPipeline_->GetMainThread()->GetContext(), unobscured);
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
        node->GetId(), node->GetName().c_str(), surface->GetUniqueId(), surfaceName.c_str());
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
#ifdef RS_ENABLE_GPU
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::weak_ptr<RSSurfaceHandler> handlerWeak = surfaceHandler;
    surfaceHandler->RegisterDeleteBufferListener([handlerWeak](uint64_t bufferId) {
        if (auto handler = handlerWeak.lock()) {
            handler->AddGPUCacheToCleanupSet(bufferId);
        }
    });
#endif
    std::function<void()> registerNode = [node, renderPipeline = rsRenderPipeline_]() -> void {
        if (auto preNode = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(node->GetId())) {
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
        renderPipeline->GetMainThread()->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    if (config.isSync) {
        rsRenderPipeline_->PostMainThreadSyncTask(registerNode);
    } else {
        rsRenderPipeline_->PostMainThreadTask(registerNode, REGISTER_NODE, 0, AppExecFwk::EventQueue::Priority::VIP);
    }
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    sptr<IBufferConsumerListener> listener =
        new RSRenderServiceListener(surfaceRenderNode, rsRenderPipeline_->GetComposerClientManager());
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    sfc = Surface::CreateSurfaceAsProducer(producer);
    return ERR_OK;
}

int32_t RSRenderPipelineAgent::SetBrightnessInfoChangeCallback(pid_t pid,
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [renderPipeline = rsRenderPipeline_, &callback, &pid]() {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        return context.SetBrightnessInfoChangeCallback(pid, callback);
    };
    return rsRenderPipeline_->GetMainThread()->ScheduleTask(task).get();
}

int32_t RSRenderPipelineAgent::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    brightnessInfo = RSLuminanceControl::Get().GetBrightnessInfo(screenId);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->GetMainThread()->RegisterOcclusionChangeCallback(pid, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->GetMainThread()->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->GetMainThread()->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}

ErrCode RSRenderPipelineAgent::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    if (rsRenderPipeline_ != nullptr) {
        OHOS::Media::Rect rect = {
            .left = srcRect.x,
            .top = srcRect.y,
            .width = srcRect.w,
            .height = srcRect.h,
        };
        RSBackgroundThread::Instance().PostSyncTask(
            [surface, rect, &pixelMap]() { pixelMap = Rosen::CreatePixelMapFromSurface(surface, rect); });
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = ERR_OK;
    rsRenderPipeline_->PostUniRenderThreadSyncTask(
        [renderPipeline = rsRenderPipeline_, &memoryGraphic, &pid, &ret] {
            if (!renderPipeline->GetMainThread()->GetContext().GetNodeMap().ContainPid(pid)) {
                ret = ERR_INVALID_VALUE;
                return;
            }
            if (renderPipeline->GetUniRenderThread()->GetRenderEngine() == nullptr ||
                renderPipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext() == nullptr) {
                ret = ERR_INVALID_VALUE;
                return;
            }
            auto context = renderPipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
            memoryGraphic = MemoryManager::CountPidMemory(pid, context); 
        });
    return ERR_OK;
}

void RSRenderPipelineAgent::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    rsRenderPipeline_->PostMainThreadTask([renderPipeline = rsRenderPipeline_, packageList] {
        renderPipeline->GetMainThread()->CheckPackageInConfigList(packageList);
        renderPipeline->imageEnhanceManager_->CheckPackageInConfigList(packageList);
    });
}

ErrCode RSRenderPipelineAgent::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, nodeIdStr, isTop]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isTop](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                    (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                    surfaceNode->SetLayerTop(isTop);
                    return;
                }
            });
        // It can be displayed immediately after layer-top changed.
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->RequestNextVSync();
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
#ifdef RS_ENABLE_GPU
    rsRenderPipeline_->PostUniRenderThreadSyncTask([&cpuMemSize, &gpuMemSize] {
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(
            RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        cpuMemSize = MemoryTrack::Instance().GetAppMemorySizeInMB();
    });
    gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
#endif
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    const auto& nodeMap = rsRenderPipeline_->GetMainThread()->GetContext().GetNodeMap();
    std::vector<pid_t> pids;
    nodeMap.TraverseSurfaceNodes([&pids](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        auto pid = ExtractPid(node->GetId());
        if (std::find(pids.begin(), pids.end(), pid) == pids.end()) {
            pids.emplace_back(pid);
        }
    });

    bool enable = RSUniRenderJudgement::IsUniRender();
    if (rsRenderPipeline_->GetUniRenderThread()->GetRenderEngine() == nullptr ||
        rsRenderPipeline_->GetUniRenderThread()->GetRenderEngine()->GetRenderContext() == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto context = rsRenderPipeline_->GetUniRenderThread()->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
    if (enable) {
        rsRenderPipeline_->PostUniRenderThreadSyncTask(
            [&context, &memoryGraphics, &pids] { MemoryManager::CountMemory(pids, context, memoryGraphics); });
        return ERR_OK;
    } else {
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSRenderPipelineAgent::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (rsRenderPipeline_ == nullptr) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>> sfBufferInfoVector;
    std::function<void()> getSurfaceBufferByPidTask = [renderPipeline = rsRenderPipeline_,
        &sfBufferInfoVector, pid]() -> void {
        auto selfDrawingNodeVector =
            renderPipeline->GetMainThread()->GetContext().GetMutableNodeMap().GetSelfDrawingNodeInProcess(pid);         
        RS_TRACE_NAME_FMT("RSRenderPipelineAgent::GetPixelMapByProcessId getSurfaceBufferByPidTask pid: %d", pid);
        for (auto iter = selfDrawingNodeVector.rbegin(); iter != selfDrawingNodeVector.rend(); ++iter) {
            auto node = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(*iter);
            if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                auto surfaceBuffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
                auto surfaceBufferInfo = std::make_tuple(surfaceBuffer, surfaceNode->GetName(),
                    surfaceNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect());
                sfBufferInfoVector.push_back(surfaceBufferInfo);
            }
        }
    };
    rsRenderPipeline_->PostMainThreadSyncTask(getSurfaceBufferByPidTask);

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
                    { absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight(), i }, surfaceName,
                    GetRotationInfoFromSurfaceBuffer(surfaceBuffer) });
            } else {
                RS_LOGE("RSRenderPipelineAgent::CreatePixelMapFromSurfaceBuffer pixelmap is null, nodeName:%{public}s",
                    surfaceName.c_str());
            }
        } else {
            RS_LOGE(
                "RSRenderPipelineAgent::CreatePixelMapFromSurfaceBuffer surfaceBuffer is null, nodeName:%{public}s, "
                "rect:%{public}s",
                surfaceName.c_str(), absRect.ToString().c_str());
        }
    }
    repCode = SUCCESS;
    return ERR_OK;
}

float RSRenderPipelineAgent::GetRotationInfoFromSurfaceBuffer(const sptr<SurfaceBuffer>& buffer)
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

ErrCode RSRenderPipelineAgent::SetWatermark(
    pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    if (rsRenderPipeline_ == nullptr) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    rsRenderPipeline_->GetMainThread()->SetWatermark(callingPid, name, watermark);
    success = true;
    return ERR_OK;
}

void RSRenderPipelineAgent::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [renderPipeline = rsRenderPipeline_, watermarkImg, isShow]() -> void {
        renderPipeline->GetMainThread()->ShowWatermark(watermarkImg, isShow);
    };
    rsRenderPipeline_->GetMainThread()->PostTask(task);
}

ErrCode RSRenderPipelineAgent::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, nodeIdStr, isForceRefresh]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isForceRefresh](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                    (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                    surfaceNode->SetForceRefresh(isForceRefresh);
                    return;
                }
            });
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

int32_t RSRenderPipelineAgent::NotifyScreenRefresh(ScreenId screenId)
{
    if (rsRenderPipeline_ == nullptr) {
        return -1;
    }
    rsRenderPipeline_->PostMainThreadTask([renderPipeline = rsRenderPipeline_]() {
            renderPipeline->GetMainThread()->SetForceUpdateUniRenderFlag(true);
            renderPipeline->GetMainThread()->RequestNextVSync();
        });
    return 0;
}

void RSRenderPipelineAgent::DoDump(std::unordered_set<std::u16string>& argSets, sptr<RSIDumpCallback> callback)
{
    if (rsRenderPipeline_ == nullptr || rsRenderPipeline_->rpDumpManager_ == nullptr) {
        RS_LOGE("RSRenderPiplineAgent::DoDump: RenderPipeline or DumpManager is null");
        return;
    }
    std::string dumpString;
    rsRenderPipeline_->rpDumpManager_->CmdExec(argSets, dumpString, callback);
}

void RSRenderPipelineAgent::NotifyHwcEventToRender(
    uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    RSUniHwcPrevalidateUtil::GetInstance().HandleHwcEvent(deviceId, eventId, eventData);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSRenderPipelineAgent::SetOverlayDisplayMode(int32_t mode)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("RSRenderPipelineAgent::SetOverlayDisplayMode: mode: [%{public}d]", mode);
    return RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(mode) == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif
void RSRenderPipelineAgent::SetVmaCacheStatus(bool flag)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    rsRenderPipeline_->GetUniRenderThread()->SetVmaCacheStatus(flag);
#endif 
}

void RSRenderPipelineAgent::SetBehindWindowFilterEnabled(bool enabled)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [renderPipeline = rsRenderPipeline_, enabled]() {
        if (RSSystemProperties::GetBehindWindowFilterEnabled() == enabled) {
            return;
        }
        RSSystemProperties::SetBehindWindowFilterEnabled(enabled);
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
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
            renderPipeline->GetMainThread()->RequestNextVSync();
            renderPipeline->GetMainThread()->SetForceUpdateUniRenderFlag(true);
        }
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}

bool RSRenderPipelineAgent::GetBehindWindowFilterEnabled()
{
    bool enabled = false;
    if (rsRenderPipeline_ == nullptr) {
        return enabled;
    }
    enabled = RSSystemProperties::GetBehindWindowFilterEnabled();
    return enabled;
}

int32_t RSRenderPipelineAgent::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterUIExtensionCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->GetMainThread()->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    return StatusCode::SUCCESS;
}

bool RSRenderPipelineAgent::RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (rsRenderPipeline_ == nullptr) {
        return false;
    }
    RS_LOGI("RSRenderPipeline::RegisterTypeface, pid[%{public}d], familyname:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSRenderPipelineAgent::UnRegisterTypeface(uint64_t globalUniqueId)
{
    if (rsRenderPipeline_ == nullptr) {
        return false;
    }
    RS_LOGW("RSRenderPipeline::UnRegisterTypeface: pid[%{public}d], uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);
    return true;
}

void RSRenderPipelineAgent::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    RS_LOGI("RSRenderPipelineAgent::HgmForceUpdateTask");
    rsRenderPipeline_->GetMainThread()->SetForceUpdateUniRenderFlag(flag);
    rsRenderPipeline_->GetMainThread()->RequestNextVSync("ltpoForceUpdate");
}

int32_t RSRenderPipelineAgent::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }

    MemorySnapshotInfo memorySnapshotInfo;

    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, memorySnapshotInfo);
    if (!ret) {
        RS_LOGD("RSRenderPipelineAgent::GetPidGpuMemoryInMB fail to find pid!");
        return ERR_INVALID_VALUE;
    }
    gpuMemInMB = static_cast<float>(memorySnapshotInfo.gpuMemory) / MEM_BYTE_TO_MB;
    RS_LOGD("RSRenderPipelineAgent::GetPidGpuMemoryInMB called succ");
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RepaintEverything()
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("RepaintEverything start");
    auto task = [renderPipeline = rsRenderPipeline_]() -> void {
        RS_LOGI("RepaintEverything, SetDirtyFlag ForceRefreshForUni");
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->ForceRefreshForUni();
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::CleanAll(pid_t pid)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    RS_LOGD("CleanAll() start.");
    RS_TRACE_NAME("RSRenderPipelineAgent::CleanAll begin, remotePid: " + std::to_string(pid));
    RsCommandVerifyHelper::GetInstance().RemoveCntWithPid(pid);
    rsRenderPipeline_->ScheduleMainThreadTask(
        [mainThread = rsRenderPipeline_->GetMainThread(), pid]() {
            if (mainThread == nullptr) {
                return;
            }
            mainThread->CleanResources(pid);
        }).wait();
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid);
    {
        std::lock_guard<std::mutex> lock(pidToBundleMutex_);
        pidToBundleName_.clear();
    }
    return;
}

ErrCode RSRenderPipelineAgent::SetColorFollow(const std::string& nodeIdStr, bool isColorFollow)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = rsRenderPipeline_, nodeIdStr, isColorFollow]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isColorFollow](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                    (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                    surfaceNode->SetColorFollow(isColorFollow);
                    return;
                }
            });
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->RequestNextVSync();
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::GetSurfaceRootNodeId(NodeId &windowNodeId)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    rsRenderPipeline_->ScheduleMainThreadTask([renderPipeline = rsRenderPipeline_, &windowNodeId]() {
        RS_TRACE_NAME("TraverseSurfaceNodes");
        const auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
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

void RSRenderPipelineAgent::SetFreeMultiWindowStatus(bool enable)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [enable]() -> void {
        RSUifirstManager::Instance().SetFreeMultiWindowStatus(enable);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}

int32_t RSRenderPipelineAgent::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSelfDrawingNodeRectChangeCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }

    auto task = [pid = remotePid, constraint, callback]() {
        SelfDrawingNodeMonitor::GetInstance().RegisterRectChangeCallback(pid, constraint, callback);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [pid = remotePid]() { SelfDrawingNodeMonitor::GetInstance().UnRegisterRectChangeCallback(pid); };
    rsRenderPipeline_->PostMainThreadTask(task);
    return StatusCode::SUCCESS;
}

uint32_t RSRenderPipelineAgent::GetRealtimeRefreshRate(ScreenId screenId)
{
    if (rsRenderPipeline_ == nullptr) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    return RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRateByScreenId(screenId);
}

void RSRenderPipelineAgent::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enabled, type);
}

ErrCode RSRenderPipelineAgent::GetShowRefreshRateEnabled(bool& enable)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    enable = RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto task = [pidList]() -> void { RSGpuDirtyCollector::GetInstance().SetSelfDrawingGpuDirtyPidList(pidList); };
    rsRenderPipeline_->PostMainThreadTask(task);
    return ERR_OK;
}

std::vector<ActiveDirtyRegionInfo> RSRenderPipelineAgent::GetActiveDirtyRegionInfo()
{
    const auto& activeDirtyRegionInfos = GpuDirtyRegionCollection::GetInstance().GetActiveDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetActiveDirtyRegionInfo();
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSRenderPipelineAgent::GetGlobalDirtyRegionInfo()
{
    const auto& globalDirtyRegionInfo = GpuDirtyRegionCollection::GetInstance().GetGlobalDirtyRegionInfo();
    GpuDirtyRegionCollection::GetInstance().ResetGlobalDirtyRegionInfo();
    return globalDirtyRegionInfo;
}

LayerComposeInfo RSRenderPipelineAgent::GetLayerComposeInfo()
{
    const auto& layerComposeInfo = LayerComposeCollection::GetInstance().GetLayerComposeInfo();
    LayerComposeCollection::GetInstance().ResetLayerComposeInfo();
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSRenderPipelineAgent::GetHwcDisabledReasonInfo()
{
    const auto& hwcDisabledReasonInfos = HwcDisabledReasonCollection::GetInstance().GetHwcDisabledReasonInfo();
    return hwcDisabledReasonInfos;
}

ErrCode RSRenderPipelineAgent::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    auto rsHdrCollection = RsHdrCollection::GetInstance();
    if (rsHdrCollection == nullptr) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = rsHdrCollection->GetHdrOnDuration();
    rsHdrCollection->ResetHdrOnDuration();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    if (rsRenderPipeline_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    if (pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    return ERR_OK;
}

void RSRenderPipelineAgent::OnScreenBacklightChanged(ScreenId screenId, uint32_t level)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    RSLuminanceControl::Get().SetSdrLuminance(screenId, level);
    if (RSLuminanceControl::Get().IsHdrOn(screenId) && level > 0) {
        rsRenderPipeline_->PostMainThreadTask([renderPipeline = rsRenderPipeline_, screenId]() {
            renderPipeline->GetMainThread()->SetForceUpdateUniRenderFlag(true);
            renderPipeline->GetMainThread()->SetLuminanceChangingStatus(screenId, true);
            renderPipeline->GetMainThread()->SetDirtyFlag();
            renderPipeline->GetMainThread()->RequestNextVSync();
        });
        return;
    }
    if (rsRenderPipeline_->GetUniRenderThread() != nullptr) {
        rsRenderPipeline_->GetComposerClientManager()->SetScreenBacklight(screenId, level);
    }
}

void RSRenderPipelineAgent::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RSRenderPipelineAgent:%{public}s rsRenderPipeline is nullptr.", __func__);
        return;
    }
    auto task = [globalBlackList, mainThread = rsRenderPipeline_->GetMainThread()]() { 
        ScreenSpecialLayerInfo::SetGlobalBlackList(globalBlackList);
        RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(mainThread->GetContext().GetNodeMap());
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}

uint32_t RSRenderPipelineAgent::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType, bool isSystemCalling)
{
    if (rsRenderPipeline_ == nullptr) {
        return WATER_MARK_IPC_ERROR;
    }
    uint32_t res =  SurfaceWatermarkStatusCode::WATER_MARK_RS_CONNECTION_ERROR;
    auto task = [renderPipeline = rsRenderPipeline_, &name, &nodeIdList, &watermark, &watermarkType,
        pid, isSystemCalling, &res]() -> void {
        res = renderPipeline->GetMainThread()->SetSurfaceWatermark(pid, name, watermark,
            nodeIdList, watermarkType, isSystemCalling);
    };
    rsRenderPipeline_->PostMainThreadSyncTask(task);
    return res;
}
    
void RSRenderPipelineAgent::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
    const std::vector<NodeId> &nodeIdList, bool isSystemCalling)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [renderPipeline = rsRenderPipeline_, name, pid, nodeIdList, isSystemCalling]() -> void {
        renderPipeline->GetMainThread()->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, isSystemCalling);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}
    
void RSRenderPipelineAgent::ClearSurfaceWatermark(pid_t pid,
    const std::string &name, bool isSystemCalling)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    auto task = [renderPipeline = rsRenderPipeline_, name, pid, isSystemCalling]() -> void {
        renderPipeline->GetMainThread()->ClearSurfaceWatermark(pid, name, isSystemCalling);
    };
    rsRenderPipeline_->PostMainThreadTask(task);
}

std::string RSRenderPipelineAgent::GetBundleName(pid_t pid)
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

void RSRenderPipelineAgent::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    rsRenderPipeline_->AddTransactionDataPidInfo(remotePid);
}

void RSRenderPipelineAgent::AddConnection(sptr<IRemoteObject>& token,
    sptr<RSIClientToRenderConnection> connectToRenderConnection)
{
    if (rsRenderPipeline_ == nullptr) {
        return;
    }
    rsRenderPipeline_->AddConnection(token, connectToRenderConnection);
}

sptr<RSIClientToRenderConnection> RSRenderPipelineAgent::FindClientToRenderConnection(const sptr<IRemoteObject>& token)
{
    if (rsRenderPipeline_ == nullptr) {
        return nullptr;
    }
    return rsRenderPipeline_->FindClientToRenderConnection(token);
}
} // namespace Rosen
} // namespace OHOS
