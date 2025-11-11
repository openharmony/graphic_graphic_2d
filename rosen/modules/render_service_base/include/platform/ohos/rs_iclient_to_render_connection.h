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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_ICLIENT_TO_RENDER_CONNECTION_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_ICLIENT_TO_RENDER_CONNECTION_H

#include <iremote_broker.h>
#include <string>

#include "feature/capture/rs_ui_capture.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "ivsync_connection.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "vsync_iconnection_token.h"
#include "common/rs_self_draw_rect_change_callback_constraint.h"

#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "ipc_callbacks/brightness_info_change_callback.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iself_drawing_node_rect_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/screen_switching_notify_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "ipc_callbacks/rs_transaction_data_callback.h"
#include "memory/rs_memory_graphic.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_virtual_screen_resolution.h"
#include "transaction/rs_transaction_data.h"

#include "transaction/rs_render_service_client_info.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "ipc_callbacks/rs_ifirst_frame_commit_callback.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_iuiextension_callback.h"

namespace OHOS {
namespace Rosen {
namespace {
    static constexpr uint32_t MAX_DROP_FRAME_PID_LIST_SIZE = 1024;
}

class RSIClientToRenderConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.ClientToRenderConnection");

    RSIClientToRenderConnection() = default;
    virtual ~RSIClientToRenderConnection() = default;

    virtual ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) = 0;
    virtual ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) = 0;
    virtual ErrCode SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode) = 0;
    
    virtual void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f),
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) = 0;

    virtual std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(NodeId id,
        const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) = 0;

    virtual void TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig) = 0;

    virtual ErrCode SetWindowFreezeImmediately(NodeId id, bool isFreeze,
        sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
        const RSSurfaceCaptureBlurParam& blurParam = {}) = 0;

    virtual ErrCode TakeSurfaceCaptureWithAllWindows(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, bool checkDrmAndSurfaceLock,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) = 0;

    virtual ErrCode FreezeScreen(NodeId id, bool isFreeze) = 0;

    virtual void TakeUICaptureInRange(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig) = 0;

    virtual ErrCode SetHwcNodeBounds(
        int64_t rsNodeId, float positionX, float positionY, float positionZ, float positionW) = 0;

    virtual ErrCode GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode) = 0;

    virtual ErrCode DropFrameByPid(const std::vector<int32_t> pidList) = 0;

    virtual ErrCode RegisterSurfaceBufferCallback(
        pid_t pid, uint64_t uid, sptr<RSISurfaceBufferCallback> callback) = 0;

    virtual ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid) = 0;

    virtual void RegisterTransactionDataCallback(
        uint64_t token, uint64_t timeStamp, sptr<RSITransactionDataCallback> callback) = 0;

    virtual ErrCode SetWindowContainer(NodeId nodeId, bool value) = 0;

    virtual void ClearUifirstCache(NodeId id) = 0;

    virtual ErrCode SetAncoForceDoDirect(bool direct, bool& res) = 0;

    virtual ErrCode SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder) = 0;

    virtual void SetScreenFrameGravity(ScreenId id, int32_t gravity) = 0;

    virtual void RemoveToken() = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_ICLIENT_TO_RENDER_CONNECTION_H
