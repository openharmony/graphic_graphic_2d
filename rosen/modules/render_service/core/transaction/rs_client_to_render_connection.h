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

#ifndef RENDER_SERVICE_PIPELINE_RS_CLIENT_TO_RENDER_CONNECTION_H
#define RENDER_SERVICE_PIPELINE_RS_CLIENT_TO_RENDER_CONNECTION_H

#include <mutex>
#include <unordered_set>

#include "hgm_config_callback_manager.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "render_server/rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "vsync_distributor.h"
#include "core/rs_render_pipeline_agent.h"

namespace OHOS {
namespace Rosen {
class HgmFrameRateManager;
class RSClientToRenderConnection : public RSClientToRenderConnectionStub {
public:
    RSClientToRenderConnection(
    pid_t remotePid,
    sptr<RSRenderPipelineAgent> renderPipelineAgent, sptr<IRemoteObject> token);
    ~RSClientToRenderConnection() noexcept;
    RSClientToRenderConnection(const RSClientToRenderConnection&) = delete;
    RSClientToRenderConnection& operator=(const RSClientToRenderConnection&) = delete;

    sptr<IRemoteObject> GetToken() const
    {
        return token_;
    }

    void RemoveToken() override
    {
        token_ = nullptr;
    }

private:
    void CleanAll(bool toDelete = false) noexcept;

    // IPC RSIRenderServiceConnection Interfaces
    ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;
    
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

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    void TakeSelfSurfaceCapture(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig) override;

    ErrCode SetWindowFreezeImmediately(NodeId id, bool isFreeze, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam) override;

    ErrCode TakeSurfaceCaptureWithAllWindows(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, bool checkDrmAndSurfaceLock,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    ErrCode FreezeScreen(NodeId id, bool isFreeze) override;

    void TakeUICaptureInRange(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) override;

    ErrCode SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW) override;

    int32_t GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo) override;

    ErrCode GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode) override;

    ErrCode DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel = 0) override;

    ErrCode SetAncoForceDoDirect(bool direct, bool& res) override;

    ErrCode SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder) override;

    ErrCode RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback) override;
    ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid) override;

    void RegisterTransactionDataCallback(uint64_t token,
        uint64_t timeStamp, sptr<RSITransactionDataCallback> callback) override;

    ErrCode SetWindowContainer(NodeId nodeId, bool value) override;

    void ClearUifirstCache(NodeId id) override;

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback) override;

    int32_t SubmitCanvasPreAllocatedBuffer(
        NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override;
#endif
    uint32_t SetSurfaceWatermark(pid_t pid, const std::string &name,
        const std::shared_ptr<Media::PixelMap> &watermark,
        const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType) override;
        
    void ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
        const std::vector<NodeId>& nodeIdList) override;
        
    void ClearSurfaceWatermark(pid_t pid, const std::string &name) override;

    ErrCode RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override;

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;

    ErrCode ForceRefreshOneFrameWithNextVSync() override;

    std::string GetBundleName(pid_t pid) override;
    int32_t SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection) override;

    pid_t remotePid_;
    wptr<RSRenderService> renderService_;
    sptr<RSRenderPipelineAgent> renderPipelineAgent_;
    sptr<RSScreenManager> screenManager_;
    sptr<IRemoteObject> token_;

    class RSConnectionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSConnectionDeathRecipient(wptr<RSClientToRenderConnection> conn);
        virtual ~RSConnectionDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

    private:
        wptr<RSClientToRenderConnection> conn_;
    };
    friend class RSConnectionDeathRecipient;
    sptr<RSConnectionDeathRecipient> connDeathRecipient_;

    class RSApplicationRenderThreadDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSApplicationRenderThreadDeathRecipient(wptr<RSClientToRenderConnection> conn);
        virtual ~RSApplicationRenderThreadDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

    private:
        wptr<RSClientToRenderConnection> conn_;
    };
    friend class RSApplicationRenderThreadDeathRecipient;
    sptr<RSApplicationRenderThreadDeathRecipient> applicationDeathRecipient_ = nullptr;

    mutable std::mutex mutex_;
    bool cleanDone_ = false;
    const std::string VOTER_SCENE_BLUR = "VOTER_SCENE_BLUR";
    const std::string VOTER_SCENE_GPU = "VOTER_SCENE_GPU";
    sptr<VSyncDistributor> appVSyncDistributor_;

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
    friend class RSRenderServiceStub;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_CLIENT_TO_RENDER_CONNECTION_H