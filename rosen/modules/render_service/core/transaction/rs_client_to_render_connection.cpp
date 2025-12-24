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

#include "rs_client_to_render_connection.h"

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
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "info_collection/rs_hdr_collection.h"
#ifdef RS_ENABLE_GPU
#include "feature/uifirst/rs_sub_thread_manager.h"
#endif
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

#include "hgm_config_callback_manager.h"
#include "render_server/rs_render_service.h"

#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "rs_render_pipeline_agent.h"

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
    RSMainThread* mainThread,
    sptr<RSRenderPipelineAgent> renderPipelineAgent,
    sptr<IRemoteObject> token)
    : remotePid_(remotePid),
      mainThread_(mainThread),
      renderPipelineAgent_(renderPipelineAgent),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      applicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this))
{
    if (token_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: Failed to set death recipient.");
    }
    if (mainThread_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: mainThread_ is nullptr");
    }

    if (renderPipelineAgent_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: renderPipelineAgent_ is nullptr");
    }
}

RSClientToRenderConnection::~RSClientToRenderConnection() noexcept
{
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
    // if (mainThread_ == nullptr) {
    //     return;
    // }
    // auto& context = mainThread_->GetContext();
    // auto& frameRateLinkerMap = context.GetMutableFrameRateLinkerMap();

    // frameRateLinkerMap.FilterFrameRateLinkerByPid(remotePid_);
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
    return renderPipelineAgent_->CommitTransaction(
        callingPid, isTokenTypeValid, isNonSystemAppCalling, transactionData);
}

ErrCode RSClientToRenderConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    return renderPipelineAgent_->ExecuteSynchronousTask(task);
}

ErrCode RSClientToRenderConnection::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->CreateNode(displayNodeConfig, nodeId, success);
}

ErrCode RSClientToRenderConnection::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->CreateNode(config, success);
}

ErrCode RSClientToRenderConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    sptr<Surface>& sfc, bool unobscured)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->CreateNodeAndSurface(config, sfc, unobscured);
}

ErrCode RSClientToRenderConnection::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    renderPipelineAgent_->RegisterApplicationAgent(pid, app);

    app->AsObject()->AddDeathRecipient(applicationDeathRecipient_);
    return ERR_OK;
}

ErrCode RSClientToRenderConnection::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->RegisterBufferClearListener(id, callback);
}

ErrCode RSClientToRenderConnection::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
}

ErrCode RSClientToRenderConnection::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    if (!renderPipelineAgent_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetBitmap(id, bitmap, success);
}

ErrCode RSClientToRenderConnection::SetGlobalDarkColorMode(bool isDark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetGlobalDarkColorMode(isDark);
}

ErrCode RSClientToRenderConnection::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    if (!renderPipelineAgent_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetPixelmap(id, pixelmap, rect, drawCmdList, success);
}

ErrCode RSClientToRenderConnection::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!renderPipelineAgent_) {
        success = false;
        return ERR_INVALID_VALUE;
    }
#ifdef RS_ENABLE_GPU
    return renderPipelineAgent_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation, success);
#else
    success = false;
    return ERR_INVALID_VALUE;
#endif
}

ErrCode RSClientToRenderConnection::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
}

ErrCode RSClientToRenderConnection::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode)
{
    if (!renderPipelineAgent_) {
        resCode = static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
}

bool RSClientToRenderConnection::GetHighContrastTextState()
{
    return renderPipelineAgent_ != nullptr && renderPipelineAgent_->GetHighContrastTextState();
}


ErrCode RSClientToRenderConnection::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    if (!renderPipelineAgent_) {
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetFocusAppInfo(info, repCode);
}

void RSClientToRenderConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect, RSSurfaceCapturePermissions permissions)
{
    renderPipelineAgent_->TakeSurfaceCapture(id, callback, captureConfig, blurParam, specifiedAreaRect, permissions);
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSClientToRenderConnection::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions permissions)
{
    return renderPipelineAgent_->TakeSurfaceCaptureSoloNode(id, captureConfig, permissions);
}

void RSClientToRenderConnection::TakeSelfSurfaceCapture(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    bool isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIClientToRenderConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + "::TAKE_SELF_SURFACE_CAPTURE");
    renderPipelineAgent_->TakeSelfSurfaceCapture(id, callback, captureConfig, isSystemCalling);
}

ErrCode RSClientToRenderConnection::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions permissions)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(
        id, callback, captureConfig, checkDrmAndSurfaceLock, permissions);
}

ErrCode RSClientToRenderConnection::FreezeScreen(NodeId id, bool isFreeze)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->FreezeScreen(id, isFreeze);
}

ErrCode RSClientToRenderConnection::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    bool isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIClientToRenderConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + "::SET_WINDOW_FREEZE_IMMEDIATELY");
    return renderPipelineAgent_->SetWindowFreezeImmediately(
        id, isFreeze, callback, captureConfig, blurParam, isSystemCalling);
}

void RSClientToRenderConnection::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    renderPipelineAgent_->TakeUICaptureInRange(id, callback, captureConfig);
}

ErrCode RSClientToRenderConnection::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
}

int32_t RSClientToRenderConnection::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetBrightnessInfo(screenId, brightnessInfo);
}

ErrCode RSClientToRenderConnection::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetScreenHDRStatus(id, hdrStatus, resCode);
}

ErrCode RSClientToRenderConnection::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->DropFrameByPid(pidList);
}

ErrCode RSClientToRenderConnection::SetAncoForceDoDirect(bool direct, bool& res)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!renderPipelineAgent_) {
        res = false;
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetAncoForceDoDirect(direct, res);
}

ErrCode RSClientToRenderConnection::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->RegisterSurfaceBufferCallback(pid, uid, callback);
}

ErrCode RSClientToRenderConnection::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->UnregisterSurfaceBufferCallback(pid, uid);
}

ErrCode RSClientToRenderConnection::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetLayerTopForHWC(nodeId, isTop, zOrder);
}

void RSClientToRenderConnection::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    renderPipelineAgent_->RegisterTransactionDataCallback(token, timeStamp, callback);
}

ErrCode RSClientToRenderConnection::SetWindowContainer(NodeId nodeId, bool value)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetWindowContainer(nodeId, value);
}

void RSClientToRenderConnection::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    auto task = [weakThis = wptr<RSClientToRenderConnection>(this), id, gravity]() -> void {
        sptr<RSClientToRenderConnection> connection = weakThis.promote();
        if (connection == nullptr || connection->mainThread_ == nullptr) {
            return;
        }
        auto& context = connection->mainThread_->GetContext();
        context.GetNodeMap().TraverseScreenNodes([id, gravity](auto& node) {
            if (node && node->GetScreenId() == id) {
                node->GetMutableRenderProperties().SetFrameGravity(static_cast<Gravity>(gravity));
            }
        });
    };
    mainThread_->PostTask(task);
}

void RSClientToRenderConnection::ClearUifirstCache(NodeId id)
{
    renderPipelineAgent_->ClearUifirstCache(id);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSClientToRenderConnection::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    renderPipelineAgent_->RegisterCanvasCallback(remotePid_, callback);
}

int32_t RSClientToRenderConnection::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    if (!renderPipelineAgent_) {
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SubmitCanvasPreAllocatedBuffer(remotePid_, nodeId, buffer, resetSurfaceIndex);
}
#endif

uint32_t RSClientToRenderConnection::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType)
{
    if (!renderPipelineAgent_) {
        return WATER_MARK_RS_CONNECTION_ERROR;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIClientToRenderConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::SET_SURFACE_WATERMARK");
    return renderPipelineAgent_->SetSurfaceWatermark(pid, name, watermark,
            nodeIdList, watermarkType, isSystemCalling);
}
    
void RSClientToRenderConnection::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
    const std::vector<NodeId> &nodeIdList)
{
    if (!renderPipelineAgent_) {
        return;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIClientToRenderConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::CLEAR_SURFACE_WATERMARK_FOR_NODES");
    renderPipelineAgent_->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, isSystemCalling);
}
    
void RSClientToRenderConnection::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    if (!renderPipelineAgent_) {
        return;
    }
    auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
        RSIClientToRenderConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
        "::CLEAR_SURFACE_WATERMARK");
    renderPipelineAgent_->ClearSurfaceWatermark(pid, name, isSystemCalling);
}

} // namespace Rosen
} // namespace OHOS
