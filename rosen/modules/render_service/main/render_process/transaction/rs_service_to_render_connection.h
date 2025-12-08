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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_RS_SERVICE_TO_RENDER_CONNECTION_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_RS_SERVICE_TO_RENDER_CONNECTION_H

#include "render_server/rs_render_service_agent.h"
#include "rs_render_pipeline_agent.h"

#include "transaction/zidl/rs_service_to_render_connection_stub.h"

namespace OHOS {
namespace Rosen {
class RSServiceToRenderConnection : public RSServiceToRenderConnectionStub {
public:
    RSServiceToRenderConnection(sptr<RSRenderServiceAgent> renderServiceAgent, sptr<RSRenderPipelineAgent> renderPipelineAgent) :
        renderServiceAgent_(renderServiceAgent), renderPipelineAgent_(renderPipelineAgent) {}
    virtual ~RSServiceToRenderConnection() noexcept = default;

    int32_t NotifyScreenRefresh(ScreenId screenId) override;
    void DoDump(std::unordered_set<std::u16string> &argSets) override;
    ErrCode SetBehindWindowFilterEnabled(bool enabled) override;
    ErrCode GetBehindWindowFilterEnabled(bool& enabled) override;
    ErrCode SetColorFollow(const std::string &nodeIdStr, bool isColorFollow) override;
    void SetFreeMultiWindowStatus(bool enable) override;

    int32_t RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback) override;
    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;
    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;
    ErrCode SetDiscardJankFrames(bool discardJankFrames) override;
    ErrCode ReportJankStats() override;
    ErrCode ReportEventResponse(DataBaseRs info) override;
    ErrCode ReportEventComplete(DataBaseRs info) override;
    ErrCode ReportEventJankFrame(DataBaseRs info) override;
    ErrCode ReportRsSceneJankStart(AppInfo info) override;
    ErrCode ReportRsSceneJankEnd(AppInfo info) override;
    ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime) override;
    ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps) override;
    void NotifyPackageEvnet(uint32_t listSize, const std::vector<std::string>& packageList) override;
    void HgmForceUpdateTask(bool flag, const std::string& fromWhom) override;
    void HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) override;

    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB) override;
    ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop) override;
    ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap) override;
    void SetVmaCacheStatus(bool flag) override;
    ErrCode SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh) override;
    int32_t RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false) override;
    bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface) override;
    bool UnRegisterTypeface(uint64_t globalUniqueId) override;
    ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) override;
    ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic) override;
    ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode) override;
    ErrCode SetWatermark(
        pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success) override;
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) override;
    Errcode GetSurfaceRootNodeId(NodeId& windowNodeId) override;
    int32_t RegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid, const RectConstraint& constraint,
        sptr<RSISelfDrawingNodeRectChangeCallback> callback) override;
    int32_t UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid) override;

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ErrCode SetOverlayDisplayMode(int32_t mode) override;
#endif
    ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics) override;
    ErrCode RepaintEverything() override;
    ErrCode CleanResources(pid_t pid) override;

    uint32_t GetRealtimeRefreshRate(ScreenId screenId) override;
    void SetShowRefreshRateEnabled(bool enabled, int32_t type) override;
    ErrCode GetShowRefreshRateEnabled(bool& enable) override;
    ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList) override;
    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn) override;

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() override;
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() override;
    LayerComposeInfo GetLayerComposeInfo() override;
    HwcDisabledReasonInfos GetHwcDisabledReasonInfo() override;
    ErrCode GetHdrOnDuration(int64_t& hdrOnDuration) override;
    ErrCode SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList) override;

    void ReportGameStateData(GameStateData info) override;
    void OnScreenBacklightChanged(ScreenId screenId, uint32_t level) override;

private:
    sptr<RSRenderServiceAgent> renderServiceAgent_ = nullptr;
    sptr<RSRenderProcessAgent> renderProcessAgent_ = nullptr;
    sptr<RSRenderPipelineAgent> renderPipelineAgent_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_RS_SERVICE_TO_RENDER_CONNECTION_H