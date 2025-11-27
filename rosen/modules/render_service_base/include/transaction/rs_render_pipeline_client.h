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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_PIPELINE_CLIENT_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_PIPELINE_CLIENT_H

#include "common/rs_common_def.h"
#include "rs_client_render_comm_def_info.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <refbase.h>
#include <surface_type.h>
#ifndef ROSEN_CROSS_PLATFORM
#include <surface.h>
#include <utility>
#endif

#include "common/rs_self_draw_rect_change_callback_constraint.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/iapplication_agent.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_change_callback.h"
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "ipc_callbacks/rs_icanvas_surface_buffer_callback.h"
#endif
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/screen_switching_notify_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "ipc_callbacks/rs_transaction_data_callback.h"
#include "memory/rs_memory_graphic.h"
#include "platform/drawing/rs_surface.h"
#include "rs_hrp_service.h"
#include "rs_irender_client.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_virtual_screen_resolution.h"
#include "vsync_receiver.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "rs_hgm_config_data.h"
#include "rs_occlusion_data.h"
#include "rs_self_drawing_node_rect_data.h"
#include "rs_uiextension_data.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
// normal callback functor for client users.
using ScreenChangeCallback = std::function<void(ScreenId, ScreenEvent, ScreenChangeReason)>;
using BrightnessInfoChangeCallback = std::function<void(ScreenId, BrightnessInfo)>;
using ScreenSwitchingNotifyCallback = std::function<void(bool)>;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
using PointerLuminanceChangeCallback = std::function<void(int32_t)>;
#endif
using BufferAvailableCallback = std::function<void()>;
using BufferClearCallback = std::function<void()>;
using OcclusionChangeCallback = std::function<void(std::shared_ptr<RSOcclusionData>)>;
using SurfaceOcclusionChangeCallback = std::function<void(float)>;
using HgmConfigChangeCallback = std::function<void(std::shared_ptr<RSHgmConfigData>)>;
using OnRemoteDiedCallback = std::function<void()>;
using HgmRefreshRateModeChangeCallback = std::function<void(int32_t)>;
using HgmRefreshRateUpdateCallback = std::function<void(int32_t)>;
using FrameRateLinkerExpectedFpsUpdateCallback = std::function<void(int32_t, const std::string&, int32_t)>;
using UIExtensionCallback = std::function<void(std::shared_ptr<RSUIExtensionData>, uint64_t)>;
using SelfDrawingNodeRectChangeCallback = std::function<void(std::shared_ptr<RSSelfDrawingNodeRectData>)>;
using FirstFrameCommitCallback = std::function<void(uint64_t, int64_t)>;


class RSB_EXPORT RSRenderPipelineClient : public RSIRenderClient {
public:
    RSRenderPipelineClient() = default;
    ~RSRenderPipelineClient() = default;
    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig,
        const RSSurfaceCaptureBlurParam& blurParam = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig);

    bool TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig);

    bool SetWindowFreezeImmediately(NodeId id, bool isFreeze, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam = {});

    bool TakeSurfaceCaptureWithAllWindows(NodeId id,
        std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
        bool checkDrmAndSurfaceLock);

    bool FreezeScreen(NodeId id, bool isFreeze);

    bool TakeUICaptureInRange(
        NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig);

    bool SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW);

    int32_t SetFocusAppInfo(const FocusAppInfo& info);

    bool SetAncoForceDoDirect(bool direct);

    void SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder);

    int32_t GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus);

    void DropFrameByPid(const std::vector<int32_t> pidList);

    bool RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        std::shared_ptr<SurfaceBufferCallback> callback);

    bool UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    void SetWindowContainer(NodeId nodeId, bool value);

    void ClearUifirstCache(NodeId id);

    void SetScreenFrameGravity(ScreenId id, int32_t gravity);

    bool RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp, std::function<void()> callback);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback);

    int32_t SubmitCanvasPreAllocatedBuffer(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex);
#endif

private:
    void TriggerSurfaceCaptureCallback(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        std::shared_ptr<Media::PixelMap> pixelmap, std::shared_ptr<Media::PixelMap> pixelmapHDR = nullptr);
    void TriggerOnFinish(const FinishCallbackRet& ret) const;
    void TriggerOnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) const;
    void TriggerTransactionDataCallbackAndErase(uint64_t token, uint64_t timeStamp);
    struct RectHash {
        std::size_t operator()(const Drawing::Rect& rect) const {
            std::size_t h1 = std::hash<Drawing::scalar>()(rect.left_);
            std::size_t h2 = std::hash<Drawing::scalar>()(rect.top_);
            std::size_t h3 = std::hash<Drawing::scalar>()(rect.right_);
            std::size_t h4 = std::hash<Drawing::scalar>()(rect.bottom_);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    // Hash for solo node capture
    struct UICaptureParamHash {
        std::size_t operator()(const RSUICaptureInRangeParam& param) const {
            std::size_t h1 = std::hash<NodeId>()(param.endNodeId);
            std::size_t h2 = std::hash<bool>()(param.useBeginNodeSize);
            return h1 ^ (h2 << 1);
        }
    };

    struct PairHash {
        std::size_t operator()(const std::pair<NodeId, RSSurfaceCaptureConfig>& p) const {
            std::size_t h1 = std::hash<NodeId>()(p.first);
            std::size_t h2 = RectHash()(p.second.mainScreenRect);
            std::size_t h3 = UICaptureParamHash()(p.second.uiCaptureInRangeParam);
            std::size_t h4 = RectHash()(p.second.specifiedAreaRect);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    std::mutex mutex_;
    std::mutex surfaceCaptureCbDirectorMutex_;
    std::map<NodeId, sptr<RSIBufferAvailableCallback>> bufferAvailableCbRTMap_;
    std::mutex mapMutex_;
    std::map<NodeId, sptr<RSIBufferAvailableCallback>> bufferAvailableCbUIMap_;
    sptr<RSIScreenChangeCallback> screenChangeCb_ = nullptr;
    sptr<RSIScreenSwitchingNotifyCallback> screenSwitchingNotifyCb_ = nullptr;
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCbDirector_ = nullptr;
    std::unordered_map<std::pair<NodeId, RSSurfaceCaptureConfig>,
    std::vector<std::shared_ptr<SurfaceCaptureCallback>>, PairHash> surfaceCaptureCbMap_;

    sptr<RSISurfaceBufferCallback> surfaceBufferCbDirector_;
    std::map<uint64_t, std::shared_ptr<SurfaceBufferCallback>> surfaceBufferCallbacks_;
    mutable std::shared_mutex surfaceBufferCallbackMutex_;

    sptr<RSITransactionDataCallback> transactionDataCbDirector_;
    std::map<std::pair<uint64_t, uint64_t>, std::function<void()>> transactionDataCallbacks_;
    std::mutex transactionDataCallbackMutex_;

    friend class SurfaceCaptureCallbackDirector;
    friend class SurfaceBufferCallbackDirector;
    friend class TransactionDataCallbackDirector;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CLIENT_RENDER_COMM_DEF_INFO_H