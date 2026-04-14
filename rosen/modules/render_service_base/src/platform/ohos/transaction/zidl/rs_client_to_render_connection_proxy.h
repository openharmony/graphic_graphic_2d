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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_RENDER_CONNECTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_RENDER_CONNECTION_PROXY_H

#include "command/rs_node_showing_command.h"
#include <iremote_proxy.h>
#include <memory>
#include <platform/ohos/transaction/zidl/rs_iclient_to_render_connection.h>
#include <platform/ohos/transaction/rs_iclient_to_render_connection_ipc_interface_code.h>
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
class RSClientToRenderConnectionProxy : public IRemoteProxy<RSIClientToRenderConnection> {
public:
    explicit RSClientToRenderConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSClientToRenderConnectionProxy() noexcept = default;

    ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    bool FillParcelWithTransactionData(std::unique_ptr<RSTransactionData>& transactionData,
        std::shared_ptr<MessageParcel>& data);

    ErrCode CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
        bool& success) override;

    ErrCode CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success) override;

    ErrCode CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
        sptr<Surface>& sfc, bool unobscured) override;

    ErrCode RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override;

    ErrCode RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback) override;

    ErrCode RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) override;

    ErrCode GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success) override;

    ErrCode SetGlobalDarkColorMode(bool isDark) override;

    ErrCode GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
        const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success) override;

    ErrCode SetSystemAnimatedScenes(
        SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success) override;

    ErrCode SetHardwareEnabled(NodeId id, bool isEnabled,
        SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable) override;

    ErrCode SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode) override;

    bool GetHighContrastTextState() override;

    ErrCode SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f),
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
        TakeSurfaceCaptureSoloNode(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
            RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    void TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig) override;

    ErrCode SetWindowFreezeImmediately(NodeId id, bool isFreeze, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam) override;

    ErrCode TakeSurfaceCaptureWithAllWindows(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, bool checkDrmAndSurfaceLock,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    ErrCode FreezeScreen(NodeId id, bool isFreeze, bool needSync = false) override;

    void TakeUICaptureInRange(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    ErrCode SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW) override;
    
    int32_t GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo) override;

    bool ReadBrightnessInfo(BrightnessInfo& brightnessInfo, MessageParcel& data);
    
    ErrCode GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode) override;

    ErrCode DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel = 0) override;

    ErrCode RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback) override;

    ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid) override;

    void RegisterTransactionDataCallback(uint64_t token,
        uint64_t timeStamp, sptr<RSITransactionDataCallback> callback) override;

    ErrCode SetWindowContainer(NodeId nodeId, bool value) override;

    void ClearUifirstCache(NodeId id) override;

    bool WriteSurfaceCaptureConfig(const RSSurfaceCaptureConfig& captureConfig, MessageParcel& data);

    bool WriteSurfaceCaptureBlurParam(const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data);

    bool WriteSurfaceCaptureAreaRect(const Drawing::Rect& specifiedAreaRect, MessageParcel& data);

    int32_t SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    ErrCode SetAncoForceDoDirect(bool direct, bool& res) override;

    ErrCode SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder) override;

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback) override;

    int32_t SubmitCanvasPreAllocatedBuffer(
        NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override;
#endif
    uint32_t SetSurfaceWatermark(pid_t pid, const std::string &name,
        const std::shared_ptr<Media::PixelMap> &watermark,
        const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType) override;
        
    void ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
        const std::vector<NodeId> &nodeIdList) override;
        
    void ClearSurfaceWatermark(pid_t pid, const std::string &name) override;
    
    ErrCode RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override;
    int32_t SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection) override;

    int32_t GetMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight) override;

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;

    int32_t RegisterFrameStabilityDetection(
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config,
        sptr<RSIFrameStabilityCallback> callback
    ) override;

    int32_t UnregisterFrameStabilityDetection(const FrameStabilityTarget& target) override;

    int32_t StartFrameStabilityCollection(
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config
    ) override;

    int32_t GetFrameStabilityResult(const FrameStabilityTarget& target, bool& result) override;

    void RemoveToken() override {};

    void RegisterRemoteRefreshCallback() override {};

    static inline BrokerDelegator<RSClientToRenderConnectionProxy> delegator_;

    pid_t pid_ = GetRealPid();
    std::atomic<uint32_t> transactionDataIndex_ = 0;
    OnRemoteDiedCallback OnRemoteDiedCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_RENDER_CONNECTION_PROXY_H
