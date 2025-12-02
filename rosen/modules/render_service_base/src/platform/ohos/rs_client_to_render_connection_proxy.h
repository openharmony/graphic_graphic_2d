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
#include <platform/ohos/rs_iclient_to_render_connection.h>
#include <platform/ohos/rs_irender_service_connection_ipc_interface_code.h>
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
class RSClientToRenderConnectionProxy : public IRemoteProxy<RSIClientToRenderConnection> {
public:
    explicit RSClientToRenderConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSClientToRenderConnectionProxy() noexcept = default;

    ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

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

    ErrCode FreezeScreen(NodeId id, bool isFreeze) override;

    void TakeUICaptureInRange(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig) override;

    ErrCode SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW) override;
    
    ErrCode GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode) override;

    ErrCode DropFrameByPid(const std::vector<int32_t> pidList) override;

    ErrCode RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback) override;

    ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid) override;

    void RegisterTransactionDataCallback(uint64_t token,
        uint64_t timeStamp, sptr<RSITransactionDataCallback> callback) override;

    ErrCode SetWindowContainer(NodeId nodeId, bool value) override;

    void ClearUifirstCache(NodeId id) override;

    void SetScreenFrameGravity(ScreenId id, int32_t gravity) override;

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

    void RemoveToken() override {};
    static inline BrokerDelegator<RSClientToRenderConnectionProxy> delegator_;

    pid_t pid_ = GetRealPid();
    std::atomic<uint32_t> transactionDataIndex_ = 0;
    OnRemoteDiedCallback OnRemoteDiedCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_RENDER_CONNECTION_PROXY_H
