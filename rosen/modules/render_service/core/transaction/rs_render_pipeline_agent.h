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

#ifndef RS_RENDER_PIPELINE_AGENT_H
#define RS_RENDER_PIPELINE_AGENT_H

#include "rs_render_pipeline.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"

namespace OHOS {
namespace Rosen {
class RSRenderPipelineAgent : public RefBase {
public:
    RSRenderPipelineAgent(std::shared_ptr<RSRenderPipeline>& rsRenderPipeline);
    ~RSRenderPipelineAgent() = default;

    ErrCode CommitTransaction(pid_t callingPid, bool isTokenTypeValid, bool isNonSystemAppCalling,
        std::unique_ptr<RSTransactionData>& transactionData);
    ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task);
    
    ErrCode CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
        bool& success);

    ErrCode CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success);

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);

    ErrCode RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback);

    ErrCode RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    ErrCode SetGlobalDarkColorMode(bool isDark);

    ErrCode SetSystemAnimatedScenes(
        SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success);

    ErrCode SetHardwareEnabled(NodeId id, bool isEnabled,
        SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable);

    ErrCode SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode);

    bool GetHighContrastTextState();

    ErrCode SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode);

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f),
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions());

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions());

    void TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, bool isSystemCalling);

    ErrCode SetWindowFreezeImmediately(NodeId id, bool isFreeze, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam, bool isSystemCalling);

    ErrCode TakeSurfaceCaptureWithAllWindows(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, bool checkDrmAndSurfaceLock,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions());

    ErrCode FreezeScreen(NodeId id, bool isFreeze);

    void TakeUICaptureInRange(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig);

    ErrCode SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW);

    ErrCode GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode);

    ErrCode DropFrameByPid(const std::vector<int32_t> pidList);

    ErrCode SetAncoForceDoDirect(bool direct, bool& res);

    ErrCode SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder);

    ErrCode RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback);
    ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    void RegisterTransactionDataCallback(uint64_t token,
        uint64_t timeStamp, sptr<RSITransactionDataCallback> callback);

    ErrCode SetWindowContainer(NodeId nodeId, bool value);
    void ClearUifirstCache(NodeId id);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void RegisterCanvasCallback(pid_t remotePid, sptr<RSICanvasSurfaceBufferCallback> callback);

    int32_t SubmitCanvasPreAllocatedBuffer(
        pid_t remotePid, NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex);
#endif
    ErrCode SetCurtainScreenUsingStatus(bool isCurtainScreenOn);
    ErrCode GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success);
    ErrCode SetDiscardJankFrames(bool discardJankFrames);
    ErrCode ReportJankStats();
    ErrCode ReportEventResponse(DataBaseRs info);
    ErrCode ReportEventComplete(DataBaseRs info);
    ErrCode ReportEventJankFrame(DataBaseRs info);
    ErrCode ReportRsSceneJankStart(AppInfo info);
    ErrCode ReportRsSceneJankEnd(AppInfo info);
    ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime);
    ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps);
    ErrCode CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured = false);

    int32_t RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback);
    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints);
    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id);
    ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap);  
    ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic);
    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);
    void HgmForceUpdateTask(bool flag, const std::string& fromWhom);
    ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop);
    ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);
    ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics);
    ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode);
    float GetRotationInfoFromSurfaceBuffer(const sptr<SurfaceBuffer>& buffer);
    void SetVmaCacheStatus(bool flag);
    ErrCode SetWatermark(pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success);
    ErrCode GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success);
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);
    void GetSurfaceRootNodeId(NodeId &windowNodeId);
    ErrCode SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh);
    void DoDump(std::unordered_set<std::u16string> &argSets);
    void NotifyHwcEventToRender(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData);
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ErrCode SetOverlayDisplayMode(int32_t mode);
#endif
    void SetBehindWindowFilterEnabled(bool enabled);
    bool GetBehindWindowFilterEnabled();
    int32_t RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false);
    bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface);
    bool UnRegisterTypeface(uint64_t globalUniqueId);
    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB);
    ErrCode RepaintEverything();
    ErrCode SetColorFollow(const std::string &nodeIdStr, bool isColorFollow);
    ErrCode CleanResources(pid_t pid);
    void SetFreeMultiWindowStatus(bool enable);
    int32_t RegisterSelfDrawingNodeRectChangeCallback(
        pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback);
    int32_t UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid);

    uint32_t GetRealtimeRefreshRate(ScreenId screenId);
    void SetShowRefreshRateEnabled(bool enabled, int32_t type);
    ErrCode GetShowRefreshRateEnabled(bool& enable);
    ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList);

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo();
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo();
    LayerComposeInfo GetLayerComposeInfo();
    HwcDisabledReasonInfos GetHwcDisabledReasonInfo();
    ErrCode GetHdrOnDuration(int64_t& hdrOnDuration);
    ErrCode SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList);
    void OnScreenBacklightChanged(ScreenId screenId, uint32_t level);
    void SetScreenFrameGravity(ScreenId id, Gravity gravity);
    int32_t NotifyScreenRefresh(ScreenId screenId);

private:
    // TODO: maybe do not use reference of a std::shared pointer
    std::shared_ptr<RSRenderPipeline>& rsRenderPipeline_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_RENDER_PIPELINE_AGENT_H
