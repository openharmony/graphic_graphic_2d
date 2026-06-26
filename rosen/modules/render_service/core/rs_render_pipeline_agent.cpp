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
#include "command/rs_delegate_composite_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "surface_utils.h"

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_layer_transaction_data.h"
#include "screen_manager/screen_types.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#ifdef RS_ENABLE_GPU
#include "gpuComposition/rs_gpu_cache_manager.h"
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
#include "feature/frame_stability/rs_frame_stability_manager.h"
#include "feature/hwc_event/rs_uni_hwc_event_manager.h"
#include "feature/pointer_window_manager/rs_pointer_window_manager.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif
#include "feature/special_layer/rs_special_layer_utils.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "memory/rs_canvas_dma_buffer_cache.h"
#endif
#include "core/dfx/rs_pipeline_dump_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "rs_profiler.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#ifdef RS_MODIFIERS_DRAW_ENABLE
#include "pipeline/main_thread/rs_canvas_drawing_node_buffer_consumer_listener.h"
#endif
#include "pipeline/rs_uni_render_judgement.h"
#include "pixel_map_from_surface.h"
#include "render/rs_typeface_cache.h"
#include "system/rs_system_parameters.h"
#include "transaction/rs_unmarshal_thread.h"
#include "transaction/rs_transaction_data_callback_manager.h"
#include "pipeline/rs_render_node_gc.h"
#include "app_mgr_client.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
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

void RSRenderPipelineAgent::ConfigureForceTunnelLayer(
    const RSSurfaceRenderNodeConfig& config, const sptr<IConsumerSurface>& surface)
{
    ConfigureForceTunnelLayer(config, surface, SurfaceUtils::GetInstance());
}

void RSRenderPipelineAgent::ConfigureForceTunnelLayer(
    const RSSurfaceRenderNodeConfig& config, const sptr<IConsumerSurface>& surface, SurfaceUtils* utils)
{
    if (surface == nullptr || utils == nullptr || !utils->NeedForceTunnelLayer(config.name, config.bundleName)) {
        return;
    }
    TunnelLayerInfo info;
    info.tunnelTypeMask = TUNNEL_TYPE_VIDEO;
    if (surface->SetTunnelLayerInfo(info) != GSERROR_OK) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s set tunnel layer failed, nodeId=%" PRIu64 ", name=%s",
            __func__, config.id, config.name.c_str());
        RS_LOGW("RSRenderPipeline::CreateNodeAndSurface set tunnel layer info failed, name:%{public}s",
            config.name.c_str());
        return;
    }
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s force tunnel layer, nodeId=%" PRIu64 ", name=%s, bundle=%s",
        __func__, config.id, config.name.c_str(), config.bundleName.c_str());
    RS_LOGD("TUNNEL_DEBUG %s force tunnel layer, nodeId:%{public}" PRIu64
        ", name:%{public}s, bundle:%{public}s", __func__, config.id, config.name.c_str(),
        config.bundleName.c_str());
}

bool ValidateTargetId(const RSRenderNodeMap& nodeMap, uint64_t id)
{
    bool exists = false;
    nodeMap.TraverseScreenNodes([&exists, id](auto& node) {
        if (node && node->GetScreenId() == id) {
            exists = true;
            return;
        }
    });
    nodeMap.TraverseSurfaceNodes([&exists, id](auto& node) {
        if (node && node->GetId() == id) {
            exists = true;
            return;
        }
    });
    return exists;
}

RSRenderPipelineAgent::RSRenderPipelineAgent(std::shared_ptr<RSRenderPipeline> renderPipeline)
    : rsRenderPipeline_(renderPipeline) {}

bool RSRenderPipelineAgent::RemoveConnection(pid_t remotePid, const sptr<RSIConnectionToken>& token)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("RSRenderPipelineAgent::%{public}s, pipeline is nullptr", __func__);
        return false;
    }
    return pipeline->RemoveConnection(remotePid, token);
}

ErrCode RSRenderPipelineAgent::CommitTransaction(pid_t callingPid, bool isTokenTypeValid,
    bool isNonSystemAppCalling, std::unique_ptr<RSTransactionData>& transactionData)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("RSRenderPipelineAgent::%{public}s, pipeline is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().PrepareDelegateCompositeCommand(transactionData);
#endif

    bool shouldDrop = RSUnmarshalThread::Instance().ReportTransactionDataStatistics(
        callingPid, transactionData.get(), isNonSystemAppCalling);
    if (shouldDrop) {
        RS_LOGW("CommitTransaction data droped");
        return ERR_INVALID_VALUE;
    }
    if (transactionData && transactionData->GetDVSyncUpdate()) {
        pipeline->GetMainThread()->DVSyncUpdate(
            transactionData->GetDVSyncTime(), transactionData->GetTimestamp());
    }
    bool isProcessBySingleFrame =
        pipeline->GetMainThread()->IsNeedProcessBySingleFrameComposer(transactionData);
    if (isProcessBySingleFrame) {
        pipeline->GetMainThread()->ProcessDataBySingleFrameComposer(transactionData);
    } else {
        pipeline->GetMainThread()->RecvRSTransactionData(transactionData);
    }
    return ERR_OK;
}


void RSRenderPipelineAgent::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent rsRenderPipeline_ or  is null!");
        return;
    }

    pipeline->ScheduleMainThreadTask([mainThread = pipeline->GetMainThread(), app]() {
        if (mainThread == nullptr) {
            RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent mainThread or is null!");
            return;
        }
        mainThread->UnRegisterApplicationAgent(app);
    }).wait();
}

sptr<IApplicationAgent> RSRenderPipelineAgent::UnRegisterApplicationAgent(uint32_t pid)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent rsRenderPipeline_ is null!");
        return nullptr;
    }
    sptr<IApplicationAgent> app = nullptr;
    pipeline->ScheduleMainThreadTask([mainThread = pipeline->GetMainThread(), pid, &app]() {
        if (mainThread == nullptr) {
            RS_LOGE("RSRenderPipelineAgent::UnRegisterApplicationAgent mainThread is null!");
            return;
        }
        app = mainThread->UnRegisterApplicationAgent(pid);
    }).wait();
    return app;
}

ErrCode RSRenderPipelineAgent::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (task == nullptr || !pipeline) {
        RS_LOGW("ExecuteSynchronousTask, task or RenderPipeline is null!");
        return ERR_INVALID_VALUE;
    }
    // After a synchronous task times out, it will no longer be executed.
    auto isTimeout = std::make_shared<bool>(0);
    std::weak_ptr<bool> isTimeoutWeak = isTimeout;
    std::chrono::nanoseconds span(std::min(task->GetTimeout(), MAX_TIME_OUT_NS));
    pipeline->ScheduleMainThreadTask([task, mainThread = pipeline->GetMainThread(), isTimeoutWeak] {
        if (task == nullptr || mainThread == nullptr || isTimeoutWeak.expired()) {
            return;
        }
        task->Process(mainThread->GetContext());
    }).wait_for(span);
    isTimeout.reset();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::CreateDisplayNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    auto node = DisplayNodeCommandHelper::CreateWithConfigInRS(
        pipeline->GetMainThread()->GetContext(), nodeId, displayNodeConfig);
    if (node == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::CreateDisplayNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [renderPipeline = pipeline, nodeId, node, displayNodeConfig]() {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto& nodeMap = context.GetMutableNodeMap();
        nodeMap.RegisterRenderNode(node);

        DisplayNodeCommandHelper::AddDisplayNodeToTree(context, nodeId);
        DisplayNodeCommandHelper::SetDisplayMode(context, nodeId, displayNodeConfig);
    };
    if (!pipeline->PostMainThreadSyncTask(registerNode)) {
        RS_LOGW("%{public}s: Sync task not ready, Post async tasks instead.", __func__);
        pipeline->PostMainThreadTask(registerNode);
    }
    success = true;
    return ERR_OK;
}

void RSRenderPipelineAgent::ForceRefreshOneFrameWithNextVSync()
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("%{public}s: rsRenderPipeline_ is nullptr, return.", __func__);
        return;
    }
    auto task = [renderPipeline = pipeline]() -> void {
        RS_LOGI("ForceRefreshOneFrameWithNextVSync, setDirtyflag, forceRefresh in mainThread");
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->RequestNextVSync();
    };
    pipeline->PostMainThreadTask(task);
}

ErrCode RSRenderPipelineAgent::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node =
        SurfaceNodeCommandHelper::CreateWithConfigInRS(config, pipeline->GetMainThread()->GetContext());
    if (node == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::CreateNode fail");
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::function<void()> registerNode = [renderPipeline = pipeline, node]() -> void {
        renderPipeline->GetMainThread()->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    pipeline->PostMainThreadTask(registerNode);
    success = true;
    return ERR_OK;
}

void RSRenderPipelineAgent::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto captureTask = [renderPipeline = pipeline, pid, app]() -> void {
        renderPipeline->GetMainThread()->RegisterApplicationAgent(pid, app);
    };
    pipeline->PostMainThreadTask(captureTask);
}

ErrCode RSRenderPipelineAgent::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferClearListener = [renderPipeline = pipeline, id, callback]() -> bool {
        if (auto node =
                renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferClearListener(callback);
            return true;
        }
        return false;
    };
    pipeline->PostMainThreadTask(registerBufferClearListener);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto registerBufferAvailableListener = [renderPipeline = pipeline, id, callback,
                                               isFromRenderThread]() -> bool {
        if (auto node =
                renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferAvailableListener(callback, isFromRenderThread);
            return true;
        }
        return false;
    };
    pipeline->PostMainThreadTask(registerBufferAvailableListener);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetGlobalDarkColorMode(bool isDark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, isDark]() {
        renderPipeline->GetMainThread()->SetGlobalDarkColorMode(isDark);
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    RSUifirstManager::Instance().OnProcessAnimateScene(systemAnimatedScenes);
    success = pipeline->GetMainThread()->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, id, isEnabled, selfDrawingType, dynamicHardwareEnable]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHardwareEnabled(isEnabled, selfDrawingType, dynamicHardwareEnable);
        }
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, id, needHidePrivacyContent]() -> void {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHidePrivacyContent(needHidePrivacyContent);
        }
    };
    pipeline->PostMainThreadTask(task);
    resCode = static_cast<uint32_t>(RSInterfaceErrorCode::NO_ERROR);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    pipeline->ScheduleMainThreadTask(
        [info, mainThread = pipeline->GetMainThread()]() {
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
    if (node != nullptr && node->IsOnTheTree() && !node->IsDirty() && !node->IsSubTreeDirty()) {
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    std::function<void()> captureTask =
        [renderPipeline = pipeline, id, callback, captureConfig, blurParam, specifiedAreaRect,
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
                callback->OnSurfaceCapture(id, captureConfig, nullptr, CaptureError::CAPTURE_NO_SECURE_PERMISSION);
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
            callback->OnSurfaceCapture(id, captureConfig, nullptr, CaptureError::CAPTURE_NO_NODE);
            return;
        }
        auto displayCaptureHasPermission = screenCapturePermission && isSystemCalling;
        auto surfaceCaptureHasPermission = blurParam.isNeedBlur ? isSystemCalling : (selfCapture || isSystemCalling);
        if ((node->GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE && !displayCaptureHasPermission) ||
            (node->GetType() == RSRenderNodeType::SURFACE_NODE && !surfaceCaptureHasPermission)) {
            RS_LOGE("TakeSurfaceCapture failed, node type: %{public}u, "
                "screenCapturePermission: %{public}u, isSystemCalling: %{public}u, selfCapture: %{public}u",
                node->GetType(), screenCapturePermission, isSystemCalling, selfCapture);
            callback->OnSurfaceCapture(id, captureConfig, nullptr, CaptureError::CAPTURE_NO_SECURE_PERMISSION);
            return;
        }

        auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (captureConfig.isSyncRender && surfaceNode &&
            !surfaceNode->GetSpecialLayerMgr().Find(HAS_GENERAL_SPECIAL)) {
            RS_LOGI("RSClientToRenderConnection::TakeSurfaceCapture, Node:%{public}" PRIu64
                ", isSyncRender:%{public}d, hasSpecialLayer: %{public}d, specialLayerType:%{public}u",
                surfaceNode->GetId(), captureConfig.isSyncRender,
                surfaceNode->GetSpecialLayerMgr().Find(HAS_GENERAL_SPECIAL),
                surfaceNode->GetSpecialLayerMgr().Get());
            surfaceNode->RegisterCaptureCallback(callback, captureConfig);
            surfaceNode->SetDirty(true);
            RSMainThread::Instance()->SetDirtyFlag();
            RSMainThread::Instance()->RequestNextVSync();
            RS_TRACE_NAME_FMT("RSClientToRenderConnection::TakeSurfaceCapture SetNeedSyncCaptureWindow");
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
            bool needSyncSurface = false;
            RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow, &needSyncSurface);
            if (node->GetType() == RSRenderNodeType::SURFACE_NODE &&
                (needSyncSurface || node->IsDirty() || node->IsSubTreeDirty())) {
                captureParam.hasDirtyContentInSurfaceCapture = true;
                RS_TRACE_NAME_FMT("RSClientToRenderConnection::TakeSurfaceCapture surfaceCap dirtyFlag = true,"
                    " nodeId:[%" PRIu64 "]", id);
            }
            if (node->GetType() == RSRenderNodeType::SURFACE_NODE && captureConfig.windowSync) {
                RS_TRACE_NAME_FMT("RSRenderPipelineAgent::TakeSurfaceCapture surface windowSync"
                    ", NodeId: [%" PRIu64 "]", id);
                std::function<void()> windowCapTask = [callback, captureParam, id, captureConfig]() {
                    RSSurfaceCaptureTaskParallel::CheckModifiers(id, captureConfig.useCurWindow);
                    RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
                };
                RSMainThread::Instance()->AddWindowCapTask(id, windowCapTask);
                return;
            }
            RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);
#endif
        }
    };
    pipeline->PostMainThreadTask(captureTask);
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderPipelineAgent::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions permissions)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("RSRenderPipelineAgent::%{public}s, pipeline is nullptr", __func__);
        return {};
    }
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
    pipeline->PostMainThreadSyncTask(captureTask);
    return pixelMapIdPairVector;
}

void RSRenderPipelineAgent::TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, bool isSystemCalling)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    std::function<void()> selfCaptureTask = [renderPipeline = pipeline, id, callback, captureConfig,
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
    pipeline->PostMainThreadTask(selfCaptureTask);
}

ErrCode RSRenderPipelineAgent::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions permissions)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
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
    std::function<void()> takeSurfaceCaptureTask = [renderPipeline = pipeline, id, checkDrmAndSurfaceLock,
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
    pipeline->PostMainThreadTask(takeSurfaceCaptureTask);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::FreezeScreen(NodeId id, bool isFreeze, bool needSync)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    RS_TRACE_NAME_FMT("FreezeScreen isFreeze: %d", isFreeze);
    std::function<void()> setScreenFreezeTask = [renderPipeline = pipeline, id, isFreeze]() -> void {
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
    if (needSync) {
        pipeline->PostMainThreadSyncTask(setScreenFreezeTask);
    } else {
        pipeline->PostMainThreadTask(setScreenFreezeTask);
    }
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam, bool isSystemCalling)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    std::function<void()> setWindowFreezeTask = [renderPipeline = pipeline, id, isFreeze, callback,
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
    pipeline->PostMainThreadTask(setWindowFreezeTask);
    return ERR_OK;
}

void RSRenderPipelineAgent::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    RSSurfaceCapturePermissions permissions)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    std::function<void()> captureTask = [id, callback, captureConfig,
        isSystemCalling = permissions.isSystemCalling]() -> void {
        if (!isSystemCalling) {
            RS_LOGE("TakeUICaptureInRange failed, not system calling, nodeId:[%{public}" PRIu64
                "], isSystemCalling: %{public}u", id, isSystemCalling);
            callback->OnSurfaceCapture(id, captureConfig, nullptr, CaptureError::CAPTURE_NO_PERMISSION);
            return;
        }
        RS_TRACE_NAME_FMT("RSRenderPipelineAgent::TakeUICaptureInRange captureTask nodeId:[%" PRIu64 "]", id);
        RS_LOGD("RSRenderPipelineAgent::TakeUICaptureInRange captureTask nodeId:[%{public}" PRIu64 "]", id);
        TakeSurfaceCaptureForUiParallel(id, callback, captureConfig, {});
    };
    pipeline->PostMainThreadTask(captureTask);
}

ErrCode RSRenderPipelineAgent::SetHwcNodeBounds(NodeId rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        resCode = StatusCode::INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    auto taskFuture = pipeline->ScheduleMainThreadTask([id, renderPipeline = pipeline]() {
        std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
        nodeMap.TraverseScreenNodes([id, &screenNode](const std::shared_ptr<RSScreenRenderNode>& node) {
            if (node && node->GetScreenId() == id) {
                screenNode = node;
            }
        });
        if (screenNode == nullptr) {
            return std::make_pair(StatusCode::SCREEN_NOT_FOUND, HdrStatus::NO_HDR);
        }
        return std::make_pair(StatusCode::SUCCESS, screenNode->GetLastDisplayHDRStatus());
    });
    auto span = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(100)); // timeout 100 ms
    if (taskFuture.wait_for(span) == std::future_status::timeout) {
        hdrStatus = HdrStatus::NO_HDR;
        resCode = static_cast<int32_t>(StatusCode::SCREEN_NOT_FOUND);
        return ERR_TIMED_OUT;
    }
    auto result = taskFuture.get();
    hdrStatus = result.second;
    resCode = static_cast<int32_t>(result.first);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::DropFrameByPid(const std::vector<int32_t> pidList, int32_t dropFrameLevel)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    pipeline->ScheduleMainThreadTask(
        [renderPipeline = pipeline, pidList, dropFrameLevel]() {
            // don't use 'this' directly
            renderPipeline->GetMainThread()->AddPidNeedDropFrame(pidList, dropFrameLevel);
        }
    );
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetAncoForceDoDirect(bool direct, bool& res)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        res = false;
        return ERR_INVALID_VALUE;
    }
    pipeline->GetMainThread()->SetAncoForceDoDirect(direct);
    res = true;
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid, callback);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, nodeId, isTop, zOrder]() -> void {
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
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    RSTransactionDataCallbackManager::Instance().RegisterTransactionDataCallback(token, timeStamp, callback);
}

ErrCode RSRenderPipelineAgent::SetWindowContainer(NodeId nodeId, bool value)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, nodeId, value]() -> void {
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
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::ClearUifirstCache(NodeId id)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [id]() -> void {
        RSUifirstManager::Instance().AddMarkedClearCacheNode(id);
    };
    pipeline->PostMainThreadTask(task);
}

int32_t RSRenderPipelineAgent::SetLogicalCameraRotationCorrection(ScreenId screenId, ScreenRotation logicalCorrection)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, screenId, logicalCorrection]() -> void {
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
        nodeMap.TraverseScreenNodes([screenId, logicalCorrection](const std::shared_ptr<RSScreenRenderNode>& node) {
            if (node && node->GetScreenId() == screenId) {
                RS_LOGD("SetLogicalCameraRotationCorrection nodeId: %{public}" PRIu64 ", logicalCorrection: %{public}u",
                    node->GetId(), logicalCorrection);
                node->SetLogicalCameraRotationCorrection(logicalCorrection);
            }
        });
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("GetMaxGpuBufferSize: rsRenderPipeline_ is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, &maxWidth, &maxHeight]() -> ErrCode {
        bool result = renderPipeline->GetMainThread()->GetMaxGpuBufferSize(maxWidth, maxHeight);
        return result ? ERR_OK : ERR_INVALID_VALUE;
    };
    return pipeline->GetMainThread()->ScheduleTask(task).get();
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderPipelineAgent::RegisterCanvasCallback(pid_t remotePid, sptr<RSICanvasSurfaceBufferCallback> callback)
{
    RSCanvasDmaBufferCache::GetInstance().RegisterCanvasCallback(remotePid, callback);
}

int32_t RSRenderPipelineAgent::SubmitCanvasPreAllocatedBuffer(
    pid_t remotePid, NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    if (!NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled()) {
        return FEATURE_DISABLED;
    }
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
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
    return pipeline->GetMainThread()->ScheduleTask(task).get();
}
#endif

ErrCode RSRenderPipelineAgent::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, isCurtainScreenOn]() -> void {
        renderPipeline->GetMainThread()->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
    RSMainThread* mainThread = pipeline->GetMainThread();
#ifdef RS_ENABLE_GPU
    RSUniRenderThread* renderThread = pipeline->GetUniRenderThread();
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
    pipeline->PostMainThreadTask(getBitmapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
#ifdef RS_ENABLE_GPU
    RSMainThread* mainThread = pipeline->GetMainThread();
    RSUniRenderThread* renderThread = pipeline->GetUniRenderThread();
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
    pipeline->PostUniRenderThreadTask(getPixelMapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::ReportJankStats()
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = []() -> void { RSJankStats::GetInstance().ReportJankStats(); };
        pipeline->PostUniRenderThreadTask(task);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventResponse(DataBaseRs info)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [renderPipeline = pipeline, info]() -> void {
            RSJankStats::GetInstance().SetReportEventResponse(info);
            renderPipeline->GetMainThread()->SetForceRsDVsync(info.sceneId);
        };
        pipeline->PostUniRenderThreadTask(task);
        RSUifirstManager::Instance().OnProcessEventResponse(info);
        RSUifirstFrameRateControl::Instance().SetAnimationStartInfo(info);
        UpdateAnimationOcclusionStatus(info.sceneId, true);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventComplete(DataBaseRs info)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [info]() -> void { RSJankStats::GetInstance().SetReportEventComplete(info); };
        pipeline->PostUniRenderThreadTask(task);
        RSUifirstManager::Instance().OnProcessEventComplete(info);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventJankFrame(DataBaseRs info)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        bool isReportTaskDelayed = pipeline->GetUniRenderThread()->IsMainLooping();
        auto task = [info, isReportTaskDelayed]() -> void {
            RSJankStats::GetInstance().SetReportEventJankFrame(info, isReportTaskDelayed);
        };
        pipeline->PostUniRenderThreadTask(task);
        RSUifirstFrameRateControl::Instance().SetAnimationEndInfo(info);
        UpdateAnimationOcclusionStatus(info.sceneId, false);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

void RSRenderPipelineAgent::UpdateAnimationOcclusionStatus(const std::string& sceneId, bool isStart)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [renderPipeline = pipeline, sceneId, isStart]() -> void {
            renderPipeline->GetMainThread()->SetAnimationOcclusionInfo(sceneId, isStart);
        };
        pipeline->PostMainThreadTask(task);
    }
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankStart(AppInfo info)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [info]() -> void { RSJankStats::GetInstance().SetReportRsSceneJankStart(info); };
        pipeline->PostUniRenderThreadTask(task);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankEnd(AppInfo info)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [info]() -> void { RSJankStats::GetInstance().SetReportRsSceneJankEnd(info); };
        pipeline->PostUniRenderThreadTask(task);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [uniqueIdList, surfaceNameList, fps, reportTime]() -> void {
            RSJankStats::GetInstance().AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
        };
        pipeline->PostMainThreadTask(task);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        auto task = [uniqueIdList, surfaceNameList, fps]() -> void {
            RSJankStats::GetInstance().AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
        };
        pipeline->PostMainThreadTask(task);
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoGet(uint64_t uniqueId)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [uniqueId]() -> void {
        RSJankStats::GetInstance().AvcodecVideoGet(uniqueId);
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoGetRecent()
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = []() -> void {
        RSJankStats::GetInstance().AvcodecVideoGetRecent();
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
    bool unobscured)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<RSSurfaceRenderNode> node = SurfaceNodeCommandHelper::CreateWithConfigInRS(
        config, pipeline->GetMainThread()->GetContext(), unobscured);
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
        defaultUsage |= BUFFER_USAGE_AUXILLARY_BUFFER0;
    }
    surface->SetDefaultUsage(defaultUsage | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER);
    node->GetRSSurfaceHandler()->SetConsumer(surface);
    if (auto renderEngine = pipeline->GetUniRenderThread()->GetRenderEngine()) {
        node->GetRSSurfaceHandler()->RegisterDeleteBufferListener(renderEngine->CreateBufferDeleteCallback());
    }
    std::function<void()> registerNode = [node, renderPipeline = pipeline]() -> void {
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
        pipeline->PostMainThreadSyncTask(registerNode);
    } else {
        pipeline->PostMainThreadTask(registerNode, REGISTER_NODE, 0, AppExecFwk::EventQueue::Priority::VIP);
    }
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    std::weak_ptr<RSSurfaceHandler> surfaceHandler(node->GetRSSurfaceHandler());
    sptr<IBufferConsumerListener> listener =
        new RSRenderServiceListener(surfaceRenderNode, surfaceHandler, pipeline->GetComposerClientManager());
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return ERR_INVALID_VALUE;
    }
    ConfigureForceTunnelLayer(config, surface);

    RS_TRACE_NAME_FMT("RSRenderPipelineAgent::CreateNodeAndSurface, nodeId: %" PRIu64 ", uniqueId: %" PRIu64
                      "", nodeId, surface->GetUniqueId());
    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info;
    info.first= node->GetRSSurfaceHandler();
    info.second = listener;
#ifndef ROSEN_CROSS_PLATFORM
    pipeline->GetMainThread()->GetContext().GetMutableNodeMap().SaveSurfaceHandlerInfo(nodeId, info);
#endif
    sptr<IBufferProducer> producer = surface->GetProducer();
    sfc = Surface::CreateSurfaceAsProducer(producer);
    return ERR_OK;
}

int32_t RSRenderPipelineAgent::SetBrightnessInfoChangeCallback(pid_t pid,
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [renderPipeline = pipeline, &callback, &pid]() {
        auto& context = renderPipeline->GetMainThread()->GetContext();
        return context.SetBrightnessInfoChangeCallback(pid, callback);
    };
    return pipeline->GetMainThread()->ScheduleTask(task).get();
}

int32_t RSRenderPipelineAgent::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    brightnessInfo = RSLuminanceControl::Get().GetBrightnessInfo(screenId);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    pipeline->GetMainThread()->RegisterOcclusionChangeCallback(pid, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    pipeline->GetMainThread()->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    pipeline->GetMainThread()->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}

ErrCode RSRenderPipelineAgent::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap, bool transformEnabled)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        OHOS::Media::Rect rect = {
            .left = srcRect.x,
            .top = srcRect.y,
            .width = srcRect.w,
            .height = srcRect.h,
        };
        RSBackgroundThread::Instance().PostSyncTask([surface, rect, &pixelMap, transformEnabled]() {
            pixelMap = Rosen::CreatePixelMapFromSurface(surface, rect, transformEnabled);
        });
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        ErrCode ret = ERR_OK;
        pipeline->PostUniRenderThreadSyncTask(
            [renderPipeline = pipeline, pid, &memoryGraphic, &ret] {
                if (!renderPipeline->GetMainThread()->GetContext().GetNodeMap().ContainPid(pid)) {
                    ret = ERR_INVALID_VALUE;
                    return;
                }
                if (renderPipeline->GetUniRenderThread()->GetRenderEngine() == nullptr ||
                    renderPipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext() == nullptr) {
                    ret = ERR_INVALID_VALUE;
                    return;
                }
                auto context =
                    renderPipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
                memoryGraphic = MemoryManager::CountPidMemory(pid, context);
            });
        return ret;
    }
    return ERR_INVALID_VALUE;
}

void RSRenderPipelineAgent::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    pipeline->PostMainThreadTask([renderPipeline = pipeline, packageList] {
        renderPipeline->GetMainThread()->CheckPackageInConfigList(packageList);
        renderPipeline->imageEnhanceManager_->CheckPackageInConfigList(packageList);
    });
}

ErrCode RSRenderPipelineAgent::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, nodeIdStr, isTop]() -> void {
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
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetHdrForceHwcEnabled(const std::string& nodeIdStr, bool isHdrForceHwcEnabled)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("RSRenderPipelineAgent::SetHdrForceHwcEnabled, rsRenderPipeline_ is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto task = [pipeline, nodeIdStr, isHdrForceHwcEnabled]() -> void {
        if (pipeline->GetMainThread() == nullptr) {
            return;
        }
        auto& context = pipeline->GetMainThread()->GetContext();
        context.GetNodeMap().TraverseSurfaceNodes(
            [&nodeIdStr, &isHdrForceHwcEnabled](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
                if ((surfaceNode != nullptr) && (surfaceNode->GetName() == nodeIdStr) &&
                    (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE)) {
                    surfaceNode->SetHdrForceHwcEnabled(isHdrForceHwcEnabled);
                    return;
                }
            });
        // It can be displayed immediately after layer-top changed.
        pipeline->GetMainThread()->SetDirtyFlag();
        pipeline->GetMainThread()->RequestNextVSync();
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
#ifdef RS_ENABLE_GPU
        pipeline->PostUniRenderThreadSyncTask([&cpuMemSize, &gpuMemSize] {
            gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(
                RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
            cpuMemSize = MemoryTrack::Instance().GetAppMemorySizeInMB();
        });
        gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
#endif
        return ERR_OK;
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        const auto& nodeMap = pipeline->GetMainThread()->GetContext().GetNodeMap();
        std::vector<pid_t> pids;
        nodeMap.TraverseSurfaceNodes([&pids](const std::shared_ptr<RSSurfaceRenderNode>& node) {
            auto pid = ExtractPid(node->GetId());
            if (std::find(pids.begin(), pids.end(), pid) == pids.end()) {
                pids.emplace_back(pid);
            }
        });

        bool enable = RSUniRenderJudgement::IsUniRender();
        if (pipeline->GetUniRenderThread()->GetRenderEngine() == nullptr ||
            pipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext() == nullptr) {
            return ERR_INVALID_VALUE;
        }

        auto context =
            pipeline->GetUniRenderThread()->GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        if (enable) {
            pipeline->PostUniRenderThreadSyncTask(
                [&context, &memoryGraphics, &pids] { MemoryManager::CountMemory(pids, context, memoryGraphics); });
            return ERR_OK;
        } else {
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_INVALID_VALUE;
}

void RSRenderPipelineAgent::CollectSurfaceBuffersByProcessId(
    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>>& sfBufferInfoVector, pid_t pid)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("RSRenderPipelineAgent::%{public}s, pipeline is nullptr", __func__);
        return;
    }
    RS_TRACE_NAME_FMT("RSClientToServiceConnection::CollectSurfaceBuffersByProcessId pid: %d", pid);

    // Step 1: Get buffers from self-drawing nodes (existing logic)
    auto selfDrawingNodeVector =
            pipeline->GetMainThread()->GetContext().GetMutableNodeMap().GetSelfDrawingNodeInProcess(pid);
    for (auto iter = selfDrawingNodeVector.rbegin(); iter != selfDrawingNodeVector.rend(); ++iter) {
        auto node = pipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode(*iter);
        if (node == nullptr) {
            continue;
        }
        if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
            auto surfaceBuffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
            auto surfaceBufferInfo = std::make_tuple(surfaceBuffer, surfaceNode->GetName(),
                surfaceNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect());
            sfBufferInfoVector.push_back(surfaceBufferInfo);
        }
    }

    // Step 2: Get texture mode buffers from RSSurfaceBufferCallbackManager
    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>> textureBufferVector;
    auto textureBufferInfoList = RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferInfoByPid(pid);
    for (const auto& bufferInfo : textureBufferInfoList) {
        if (bufferInfo.surfaceBuffer_ != nullptr) {
            std::string surfaceName = "XComponent_Texture_" + std::to_string(bufferInfo.uid_);
            RectI absRect(
                static_cast<int>(bufferInfo.dstRect_.GetLeft()),
                static_cast<int>(bufferInfo.dstRect_.GetTop()),
                static_cast<int>(bufferInfo.dstRect_.GetWidth()),
                static_cast<int>(bufferInfo.dstRect_.GetHeight())
            );
            textureBufferVector.push_back(
                std::make_tuple(bufferInfo.surfaceBuffer_, surfaceName, absRect));
                RS_LOGD("CollectSurfaceBuffersByProcessId: Added texture buffer from "
                    "pid=%{public}d, uid=[%{public}" PRIu64 ",] "
                    "bufferId=%{public}u, size=%{public}dx%{public}d",
                    bufferInfo.pid_, bufferInfo.uid_,
                    bufferInfo.surfaceBuffer_->GetSeqNum(),
                    bufferInfo.surfaceBuffer_->GetWidth(),
                    bufferInfo.surfaceBuffer_->GetHeight());
        }
    }
    for (const auto &bufferInfo : textureBufferInfoList) {
        RSSurfaceBufferCallbackManager::Instance().RemoveAllSurfaceBufferInfo(pid, bufferInfo.uid_);
    }

    std::sort(textureBufferVector.begin(), textureBufferVector.end(), [](const auto &a, const auto &b) {
        const auto &bufferA = std::get<0>(a);
        const auto &bufferB = std::get<0>(b);
        if (bufferA == nullptr) {
            return false;
        }
        if (bufferB == nullptr) {
            return true;
        }
        uint64_t areaA = static_cast<uint64_t>(bufferA->GetWidth()) * static_cast<uint64_t>(bufferA->GetHeight());
        uint64_t areaB = static_cast<uint64_t>(bufferB->GetWidth()) * static_cast<uint64_t>(bufferB->GetHeight());
        return areaA > areaB;
    });

    sfBufferInfoVector.insert(sfBufferInfoVector.end(), textureBufferVector.begin(), textureBufferVector.end());
}

void RSRenderPipelineAgent::ConvertBuffersToPixelMaps(
    const std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>>& sfBufferInfoVector,
    std::vector<PixelMapInfo>& pixelMapInfoVector)
{
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
                RS_LOGE("ConvertBuffersToPixelMaps: pixelmap is null, nodeName:%{public}s", surfaceName.c_str());
            }
        } else {
            RS_LOGE("ConvertBuffersToPixelMaps: surfaceBuffer is null, nodeName:%{public}s, rect:%{public}s",
                surfaceName.c_str(), absRect.ToString().c_str());
        }
    }
}

ErrCode RSRenderPipelineAgent::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }

    std::vector<std::tuple<sptr<SurfaceBuffer>, std::string, RectI>> sfBufferInfoVector;
    std::function<void()> collectBuffersTask = [weakThis = wptr<RSRenderPipelineAgent>(this),
                                                  &sfBufferInfoVector, pid]() -> void {
        sptr<RSRenderPipelineAgent> agent = weakThis.promote();
        if (agent == nullptr || agent->rsRenderPipeline_.expired()) {
            return;
        }
        agent->CollectSurfaceBuffersByProcessId(sfBufferInfoVector, pid);
    };
    pipeline->PostMainThreadSyncTask(collectBuffersTask);

    ConvertBuffersToPixelMaps(sfBufferInfoVector, pixelMapInfoVector);
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
    pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success,
    uint32_t rowCount, uint32_t colCount)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, callingPid, name, watermark, rowCount, colCount]() -> void {
        renderPipeline->GetMainThread()->SetWatermark(callingPid, name, watermark, rowCount, colCount);
    };
    pipeline->GetMainThread()->PostTask(task);
    success = true;
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetUifirstScale(float scaleFactor)
{
    RS_LOGD("RSRenderPipelineAgent::SetUifirstScale scaleFactor:%{public}f", scaleFactor);
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, scaleFactor]() -> void {
        renderPipeline->GetMainThread()->SetUifirstScale(scaleFactor);
    };
    pipeline->GetMainThread()->PostTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [renderPipeline = pipeline, watermarkImg, isShow]() -> void {
        renderPipeline->GetMainThread()->ShowWatermark(watermarkImg, isShow);
    };
    pipeline->GetMainThread()->PostTask(task);
}

ErrCode RSRenderPipelineAgent::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, nodeIdStr, isForceRefresh]() -> void {
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
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

int32_t RSRenderPipelineAgent::NotifyScreenRefresh(ScreenId screenId)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return -1;
    }
    pipeline->PostMainThreadTask([renderPipeline = pipeline]() {
            renderPipeline->GetMainThread()->SetForceUpdateUniRenderFlag(true);
            renderPipeline->GetMainThread()->RequestNextVSync();
        });
    return 0;
}

void RSRenderPipelineAgent::DoDump(std::unordered_set<std::u16string>& argSets, sptr<RSIDumpCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline || pipeline->rpDumpManager_ == nullptr) {
        RS_LOGE("RSRenderPiplineAgent::DoDump: RenderPipeline or DumpManager is null");
        return;
    }
    std::string dumpString;
    pipeline->rpDumpManager_->CmdExec(argSets, dumpString, callback);
}

void RSRenderPipelineAgent::NotifyHwcEventToRender(
    uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    RSUniHwcEventManager::GetInstance().OnHwcEvent(deviceId, eventId, eventData);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSRenderPipelineAgent::SetOverlayDisplayMode(int32_t mode)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("RSRenderPipelineAgent::SetOverlayDisplayMode: mode: [%{public}d]", mode);
    return RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(mode) == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif

#ifdef RS_ENABLE_TV_PQ_METADATA
ErrCode RSRenderPipelineAgent::SendVideoRateInfo(const std::unordered_map<std::string, std::string>& videoRateInfo)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (pipeline != nullptr) {
        return RSTvMetadataManager::SendVideoRateInfo(videoRateInfo) == 0 ? ERR_OK : ERR_INVALID_VALUE;
    }
    return ERR_INVALID_VALUE;
}
#endif

void RSRenderPipelineAgent::SetVmaCacheStatus(bool flag)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
#ifdef RS_ENABLE_GPU
    pipeline->GetUniRenderThread()->SetVmaCacheStatus(flag);
#endif
}

void RSRenderPipelineAgent::SetBehindWindowFilterEnabled(bool enabled)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [renderPipeline = pipeline, enabled]() {
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
    pipeline->PostMainThreadTask(task);
}

bool RSRenderPipelineAgent::GetBehindWindowFilterEnabled()
{
    bool enabled = false;
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return enabled;
    }
    enabled = RSSystemProperties::GetBehindWindowFilterEnabled();
    return enabled;
}

ErrCode RSRenderPipelineAgent::SetApsConfigParams(
    ApsEventType event, const std::unordered_map<std::string, std::string>& params)
{
    return ERR_OK;
}

int32_t RSRenderPipelineAgent::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterUIExtensionCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    pipeline->GetMainThread()->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    return StatusCode::SUCCESS;
}

bool RSRenderPipelineAgent::RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return false;
    }
    RS_LOGI("RSRenderPipeline::RegisterTypeface, pid[%{public}d], familyname:%{public}s, uniqueid:%{public}u",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), typeface->GetFamilyName().c_str(),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSRenderPipelineAgent::RegisterTypeface(Drawing::SharedTypeface& sharedTypeface, bool isLocal)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return false;
    }

    if (isLocal) {
        return true;
    }
    // Variation typeface: clone from cached base typeface
    if (sharedTypeface.originId_ > 0) {
        ::close(sharedTypeface.fd_);
        RS_LOGI_LIMIT("RSRenderPipelineAgent::RegisterTypeface(var): %{public}s", sharedTypeface.ToString().c_str());
        return RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface) != -1;
    }

    if (RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface) != nullptr) {
        RS_LOGI("RSRenderPipelineAgent::RegisterTypeface(reuse): %{public}s", sharedTypeface.ToString().c_str());
        ::close(sharedTypeface.fd_);
        return true;
    }

    auto tf = Drawing::Typeface::MakeFromAshmem(sharedTypeface);
    if (tf == nullptr) {
        RS_LOGE("RSRenderPipelineAgent::RegisterTypeface failed to create typeface from ashmem, %{public}s",
            sharedTypeface.ToString().c_str());
        ::close(sharedTypeface.fd_);
        return false;
    }
    RS_LOGI("RSRenderPipelineAgent::RegisterTypeface(new): %{public}s", sharedTypeface.ToString().c_str());
    RSTypefaceCache::Instance().CacheDrawingTypeface(sharedTypeface.id_, tf);
    return true;
}

bool RSRenderPipelineAgent::UnRegisterTypeface(uint64_t globalUniqueId)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("%{public}s: rsRenderPipeline_ is nullptr", __func__);
        return false;
    }
    RS_LOGW("UnregisterTypeface, pid[%{public}d], uniqueid:%{public}u", RSTypefaceCache::GetTypefacePid(globalUniqueId),
        RSTypefaceCache::GetTypefaceId(globalUniqueId));
    auto typeface = RSTypefaceCache::Instance().GetDrawingTypefaceCache(globalUniqueId);
    if (typeface == nullptr) {
        return true;
    }
    uint32_t uniqueId = typeface->GetUniqueID();
    // Free cpu cache, this only valid in skia path. When deprecating skia, this can be removed.
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

void RSRenderPipelineAgent::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline || pipeline->GetMainThread() == nullptr) {
        RS_LOGE("%{public}s: rsRenderPipeline or mainThread is nullptr", __func__);
        return;
    }
    pipeline->GetMainThread()->SetForceUpdateUniRenderFlag(flag);
    pipeline->GetMainThread()->RequestNextVSync("ltpoForceUpdate");
}

int32_t RSRenderPipelineAgent::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }

    MemorySnapshotInfo memorySnapshotInfo;

    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, memorySnapshotInfo);
    if (!ret) {
        RS_LOGD("RSRenderPipelineAgent::GetPidGpuMemoryInMB fail to find pid!");
        return ERR_INVALID_VALUE;
    }
    gpuMemInMB =
        static_cast<float>(memorySnapshotInfo.engineGpuMemory + memorySnapshotInfo.nativeGpuMemory) / MEM_BYTE_TO_MB;
    RS_LOGD("RSRenderPipelineAgent::GetPidGpuMemoryInMB called succ");
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RepaintEverything()
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    RS_LOGI("RepaintEverything start");
    auto task = [renderPipeline = pipeline]() -> void {
        RS_LOGI("RepaintEverything, SetDirtyFlag ForceRefreshForUni");
        renderPipeline->GetMainThread()->SetDirtyFlag();
        renderPipeline->GetMainThread()->ForceRefreshForUni();
    };
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::Clean(pid_t pid, bool forRefresh)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    RS_LOGD("Clean() start, remotePid: %{public}s, forRefresh: %{public}d", std::to_string(pid).c_str(), forRefresh);
    RS_TRACE_NAME("RSRenderPipelineAgent::Clean begin, remotePid: " + std::to_string(pid));
    RsCommandVerifyHelper::GetInstance().RemoveCntWithPid(pid);
    pipeline
        ->ScheduleMainThreadTask([mainThread = pipeline->GetMainThread(), pid, forRefresh]() {
            if (mainThread == nullptr) {
                return;
            }
            mainThread->CleanResources(pid, forRefresh);
        }).wait();
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid);
    {
        std::lock_guard<std::mutex> lock(pidToBundleMutex_);
        pidToBundleName_.clear();
    }
    pipeline->PostUniRenderThreadSyncTask([pid]() {
        RSFrameStabilityManager::GetInstance().CleanResourcesByPid(pid);
    });
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().RemoveAllListenerbyPid(pid);
#endif
    RS_TRACE_NAME("RSRenderPipelineAgent::Clean end, remotePid: " + std::to_string(pid));
}

ErrCode RSRenderPipelineAgent::SetColorFollow(const std::string& nodeIdStr, bool isColorFollow)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [renderPipeline = pipeline, nodeIdStr, isColorFollow]() -> void {
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
    pipeline->PostMainThreadTask(task);
    return ERR_OK;
}

void RSRenderPipelineAgent::GetSurfaceRootNodeId(NodeId &windowNodeId)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    pipeline->ScheduleMainThreadTask([renderPipeline = pipeline, &windowNodeId]() {
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [enable]() -> void {
        RSUifirstManager::Instance().SetFreeMultiWindowStatus(enable);
    };
    pipeline->PostMainThreadTask(task);
}

int32_t RSRenderPipelineAgent::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSelfDrawingNodeRectChangeCallback register null callback, failed.");
        return StatusCode::INVALID_ARGUMENTS;
    }

    auto task = [pid = remotePid, constraint, callback]() {
        SelfDrawingNodeMonitor::GetInstance().RegisterRectChangeCallback(pid, constraint, callback);
    };
    pipeline->PostMainThreadTask(task);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto task = [pid = remotePid]() { SelfDrawingNodeMonitor::GetInstance().UnRegisterRectChangeCallback(pid); };
    pipeline->PostMainThreadTask(task);
    return StatusCode::SUCCESS;
}

uint32_t RSRenderPipelineAgent::GetRealtimeRefreshRate(ScreenId screenId)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    return RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRateByScreenId(screenId);
}

void RSRenderPipelineAgent::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enabled, type);
}

ErrCode RSRenderPipelineAgent::GetShowRefreshRateEnabled(bool& enable)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    enable = RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    auto task = [pidList]() -> void { RSGpuDirtyCollector::GetInstance().SetSelfDrawingGpuDirtyPidList(pidList); };
    pipeline->PostMainThreadTask(task);
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return ERR_INVALID_VALUE;
    }
    if (pidList.size() > PIDLIST_SIZE_MAX) {
        return ERR_INVALID_VALUE;
    }
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    return ERR_OK;
}

void RSRenderPipelineAgent::OnScreenBacklightChanged(const RsScreenBrightnessData& brightnessData)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    RSLuminanceControl::Get().SetSdrLuminance(brightnessData);
    if (RSLuminanceControl::Get().IsHdrOn(brightnessData.screenId) && brightnessData.level > 0) {
        auto task = [renderPipeline = pipeline, screenId = brightnessData.screenId]() {
            renderPipeline->GetMainThread()->SetForceUpdateUniRenderFlag(true);
            renderPipeline->GetMainThread()->SetLuminanceChangingStatus(screenId, true);
            renderPipeline->GetMainThread()->SetDirtyFlag();
            renderPipeline->GetMainThread()->RequestNextVSync();
        };
        pipeline->PostMainThreadTask(task);
        return;
    }
    if (RSUniRenderJudgement::IsUniRender()) {
        pipeline->GetComposerClientManager()->SetScreenBacklight(brightnessData);
    } else {
        auto composerClient = pipeline->GetComposerClientManager()->GetComposerClient(brightnessData.screenId);
        if (composerClient) {
            auto output = composerClient->GetOutput();
            if (output) {
                output->SetScreenBacklight(brightnessData.level);
            }
        }
    }
}

void RSRenderPipelineAgent::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGE("RSRenderPipelineAgent:%{public}s rsRenderPipeline is nullptr.", __func__);
        return;
    }
    auto task = [globalBlackList, mainThread = pipeline->GetMainThread()]() {
        ScreenSpecialLayerInfo::SetGlobalBlackList(globalBlackList);
        RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(mainThread->GetContext().GetNodeMap());
    };
    pipeline->PostMainThreadTask(task);
}

uint32_t RSRenderPipelineAgent::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType, bool isSystemCalling,
    uint32_t rowCount, uint32_t colCount)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return WATER_MARK_IPC_ERROR;
    }
    uint32_t res =  SurfaceWatermarkStatusCode::WATER_MARK_RS_CONNECTION_ERROR;
    auto task = [renderPipeline = pipeline, &name, &nodeIdList, &watermark, &watermarkType,
        pid, isSystemCalling, rowCount, colCount, &res]() -> void {
        res = renderPipeline->GetMainThread()->SetSurfaceWatermark(pid, name, watermark,
            nodeIdList, watermarkType, isSystemCalling, rowCount, colCount);
    };
    pipeline->PostMainThreadSyncTask(task);
    return res;
}

void RSRenderPipelineAgent::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
    const std::vector<NodeId> &nodeIdList, bool isSystemCalling)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [renderPipeline = pipeline, name, pid, nodeIdList, isSystemCalling]() -> void {
        renderPipeline->GetMainThread()->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, isSystemCalling);
    };
    pipeline->PostMainThreadTask(task);
}

void RSRenderPipelineAgent::ClearSurfaceWatermark(pid_t pid,
    const std::string &name, bool isSystemCalling)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    auto task = [renderPipeline = pipeline, name, pid, isSystemCalling]() -> void {
        renderPipeline->GetMainThread()->ClearSurfaceWatermark(pid, name, isSystemCalling);
    };
    pipeline->PostMainThreadTask(task);
}

void RSRenderPipelineAgent::SetCacheEnabledForRotation(bool enabled)
{
    if (RSSystemProperties::GetCacheEnabledForRotation() == enabled) {
        return;
    }
    RSSystemProperties::SetCacheEnabledForRotation(enabled);
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
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    pipeline->AddTransactionDataPidInfo(remotePid);
}

void RSRenderPipelineAgent::AddConnection(pid_t remotePid, uint64_t tokenMaskId,
    sptr<IRemoteObject>& token, sptr<RSIClientToRenderConnection> connectToRenderConnection)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return;
    }
    pipeline->AddConnection(remotePid, tokenMaskId, token, connectToRenderConnection);
}

std::pair<sptr<RSIClientToRenderConnection>, uint64_t> RSRenderPipelineAgent::FindClientToRenderConnection(
    uint64_t remotePid)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return {nullptr, INVALID_TOKEN_MASK_ID};
    }
    return pipeline->FindClientToRenderConnection(remotePid);
}

int32_t RSRenderPipelineAgent::RegisterFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    sptr<RSIFrameStabilityCallback> callback)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [renderPipeline = pipeline, pid, target, config, callback, &repCode]() -> void {
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
        if (ValidateTargetId(nodeMap, target.id)) {
            repCode = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
                pid, target, config, callback);
        } else {
            RS_LOGE("RegisterFrameStabilityDetection failed, invalid id: %{public}" PRIu64, target.id);
            repCode = static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID);
        }
    };
    pipeline->PostMainThreadSyncTask(task);
    return repCode;
}

int32_t RSRenderPipelineAgent::UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [pid, target, &repCode]() -> void {
        repCode = RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(pid, target);
    };
    pipeline->PostMainThreadSyncTask(task);
    return repCode;
}

int32_t RSRenderPipelineAgent::StartFrameStabilityCollection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [renderPipeline = pipeline, pid, target, config, &repCode]() -> void {
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
        if (ValidateTargetId(nodeMap, target.id)) {
            repCode = RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(pid, target, config);
        } else {
            RS_LOGE("StartFrameStabilityCollection failed, invalid id: %{public}" PRIu64, target.id);
            repCode = static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID);
        }
    };
    pipeline->PostMainThreadSyncTask(task);
    return repCode;
}

int32_t RSRenderPipelineAgent::GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [pid, target, &result, &repCode]() -> void {
        repCode = RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(pid, target, result);
    };
    pipeline->PostMainThreadSyncTask(task);
    return repCode;
}

int32_t RSRenderPipelineAgent::UpdateFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& oldTarget,
    const FrameStabilityTarget& newTarget)
{
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [renderPipeline = pipeline, pid, oldTarget, newTarget, &repCode]() -> void {
        auto& nodeMap = renderPipeline->GetMainThread()->GetContext().GetNodeMap();
        if (ValidateTargetId(nodeMap, newTarget.id)) {
            repCode = RSFrameStabilityManager::GetInstance().UpdateFrameStabilityDetection(pid, oldTarget, newTarget);
        } else {
            RS_LOGE("UpdateFrameStabilityDetection failed, invalid newId: %{public}" PRIu64, newTarget.id);
            repCode = static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID);
        }
    };
    pipeline->PostMainThreadSyncTask(task);
    return repCode;
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
sptr<Surface> RSRenderPipelineAgent::CreateCanvasDrawingNodeSurface(NodeId nodeId, pid_t remotePid)
{
    auto rsRenderPipeline = rsRenderPipeline_.lock();
    if (rsRenderPipeline == nullptr) {
        RS_LOGW("RSRenderPipelineAgent::CreateCanvasDrawingNodeSurface, pipeline is nullptr");
        return nullptr;
    }
    if (!RSCanvasDrawingRenderNode::IsHybridEnabled()) {
        return nullptr;
    }
    if (ExtractPid(nodeId) != remotePid) {
        RS_LOGE("CreateCanvasDrawingNodeSurface: Illegal pid, nodeId=%{public}" PRIu64 ", pid=%{public}d", nodeId,
            remotePid);
        return nullptr;
    }
    auto bundleName = GetBundleName(remotePid);
    if (!NodeMemReleaseParam::IsCanvasDrawingNodeBufferEnabled()) {
        RS_LOGE("CreateCanvasDrawingNodeSurface: ccm disabled, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }
    if (!bundleName.empty() && !NodeMemReleaseParam::IsCanvasBufferEnabled(bundleName)) {
        RS_LOGE("CreateCanvasDrawingNodeSurface: bundleName ccm blacklist, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }
 
    sptr<Surface> producerPurface = nullptr;
    auto task = [&producerPurface, nodeId, mainThread = rsRenderPipeline->GetMainThread()]() -> void {
        auto rsContext = mainThread->GetWeakContext().lock();
        if (rsContext == nullptr) {
            RS_LOGE("CreateCanvasDrawingNodeSurface: null rsContext, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
        auto surfaceHandler = CreateCanvasSurfaceHandler(nodeId);
        if (surfaceHandler == nullptr) {
            RS_LOGE("CreateCanvasDrawingNodeSurface: null surfaceHandler, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
        auto consumerSurface = surfaceHandler->GetConsumer();
        sptr<IBufferProducer> producer = consumerSurface->GetProducer();
        if (producer == nullptr) {
            RS_LOGE("CreateCanvasDrawingNodeSurface: null producer, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
        producerPurface = Surface::CreateSurfaceAsProducer(producer);
        if (producerPurface == nullptr) {
            RS_LOGE("CreateCanvasDrawingNodeSurface: null producerPurface, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
 
        sptr<IBufferConsumerListener> listener =
            new RSCanvasDrawingNodeBufferConsumerListener(mainThread->GetWeakContext(), nodeId);
        consumerSurface->RegisterConsumerListener(listener);
        auto& nodeMap = rsContext->GetMutableNodeMap();
        auto canvasDrawingNode = nodeMap.GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (canvasDrawingNode != nullptr) {
            canvasDrawingNode->SetSurfaceHandler(surfaceHandler);
        } else {
            nodeMap.RegisterSurfaceHandler(nodeId, surfaceHandler);
        }
    };
    rsRenderPipeline->PostMainThreadSyncTask(task);
    return producerPurface;
}
 
void RSRenderPipelineAgent::ReleaseCanvasDrawingNodeSurface(NodeId nodeId, pid_t remotePid)
{
    auto rsRenderPipeline = rsRenderPipeline_.lock();
    if (rsRenderPipeline == nullptr) {
        RS_LOGW("RSRenderPipelineAgent::ReleaseCanvasDrawingNodeSurface, pipeline is nullptr");
        return;
    }
    if (!RSCanvasDrawingRenderNode::IsHybridEnabled()) {
        return;
    }
    if (ExtractPid(nodeId) != remotePid) {
        RS_LOGE("ReleaseCanvasDrawingNodeSurface: Illegal pid, nodeId=%{public}" PRIu64 ", pid=%{public}d", nodeId,
            remotePid);
        return;
    }
 
    auto task = [nodeId, mainThread = rsRenderPipeline->GetMainThread()]() -> void {
        auto rsContext = mainThread->GetWeakContext().lock();
        if (rsContext == nullptr) {
            RS_LOGE("ReleaseCanvasDrawingNodeSurface: null rsContext, nodeId=%{public}" PRIu64, nodeId);
            return;
        }
        auto& nodeMap = rsContext->GetMutableNodeMap();
        auto canvasDrawingNode = nodeMap.GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (canvasDrawingNode != nullptr) {
            canvasDrawingNode->SetSurfaceHandler(nullptr);
        }
        nodeMap.GetSurfaceHandler(nodeId, true);
    };
    rsRenderPipeline->PostMainThreadSyncTask(task);
}
 
std::shared_ptr<RSSurfaceHandler> RSRenderPipelineAgent::CreateCanvasSurfaceHandler(NodeId nodeId)
{
    if (!RSCanvasDrawingRenderNode::IsHybridEnabled()) {
        return nullptr;
    }
 
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("Canvas"); // Surface name is 'Canvas'
    if (surface == nullptr) {
        return nullptr;
    }
 
    surface->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    surfaceHandler->SetConsumer(surface);
    return surfaceHandler;
}
#endif // RS_MODIFIERS_DRAW_ENABLE
bool RSRenderPipelineAgent::SetDelegateMode(NodeId id, bool isSetDelegateMode, pid_t pid)
{
#ifndef ROSEN_CROSS_PLATFORM
    RS_TRACE_NAME_FMT("RSRenderPipelineAgent::SetDelegateMode: NodeId=%llu, isSetDelegateMode=%d, pid=%u",
        id, isSetDelegateMode, pid);
    auto pipeline = rsRenderPipeline_.lock();
    if (!pipeline) {
        RS_LOGW("RSRenderPipelineAgent::%{public}s, pipeline is nullptr", __func__);
        return false;
    }
    int32_t repCode = static_cast<int32_t>(FrameStabilityErrorCode::UNKNOWN);
    auto task = [renderPipeline = pipeline, id, isSetDelegateMode, pid]() -> void {
        auto node = renderPipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (!node || !node->GetRSSurfaceHandler()) {
            return;
        }
        node->SetDelegateMode(isSetDelegateMode);
        sptr<IConsumerSurface> consumer = node->GetRSSurfaceHandler()->GetConsumer();
        if (consumer) {
            RsDelegateCompositeCallbackManager::GetInstance().SetInfo(consumer, id, pid);
        }
        RS_TRACE_NAME_FMT("SetDelegateMode %llu, success", id);
    };
    pipeline->PostMainThreadSyncTask(task);

    auto node = pipeline->GetMainThread()->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
    if (node && node->GetRSSurfaceHandler()) {
        sptr<IConsumerSurface> consumer = node->GetRSSurfaceHandler()->GetConsumer();
        RsDelegateCompositeCallbackManager::GetInstance().RegisterReleaseListener(consumer);
    }
    return true;
#else
    return false;
#endif
}

bool RSRenderPipelineAgent::RegisterSurfaceTransactionListener(sptr<RSISurfaceTransactionListener> listener,
    uint64_t listenerId, uint32_t pid, uint32_t tid)
{
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().RegisterSurfaceTransactionListener(
        listener, listenerId, pid, tid);
    return true;
#else
    return false;
#endif
}

bool RSRenderPipelineAgent::UnRegisterSurfaceTransactionListener(uint64_t listenerId)
{
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().UnRegisterSurfaceTransactionListener(listenerId);
    return true;
#else
    return false;
#endif
}

bool RSRenderPipelineAgent::RegisterSurfaceNodeBufferReleaseListener(
    pid_t pid, sptr<RSISurfaceNodeBufferReleaseCallback> listener)
{
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().RegisterSurfaceNodeBufferReleaseListener(pid, listener);
    return true;
#else
    return false;
#endif
}

bool RSRenderPipelineAgent::UnRegisterSurfaceNodeBufferReleaseListener(pid_t pid)
{
#ifndef ROSEN_CROSS_PLATFORM
    RsDelegateCompositeCallbackManager::GetInstance().UnRegisterSurfaceNodeBufferReleaseListener(pid);
    return true;
#else
    return false;
#endif
}
} // namespace Rosen
} // namespace OHOS
