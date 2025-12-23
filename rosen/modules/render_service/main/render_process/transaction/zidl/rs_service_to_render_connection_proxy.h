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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_RS_SERVICE_TO_RENDER_CONNECTION_PROXY_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_RS_SERVICE_TO_RENDER_CONNECTION_PROXY_H

#include <iremote_proxy.h>
#include <memory>

#include "rs_iservice_to_render_connection.h"
#include "rs_iservice_to_render_connection_ipc_interface_code.h"

#include "command/rs_node_showing_command.h"

namespace OHOS {
namespace Rosen {
class RSServiceToRenderConnectionProxy : public IRemoteProxy<RSIServiceToRenderConnection> {
public:
    explicit RSServiceToRenderConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSServiceToRenderConnectionProxy() noexcept = default;

    int32_t NotifyScreenRefresh(ScreenId id) override;
    void DoDump(std::unordered_set<std::u16string>& argSets) override;
    ErrCode SetColorFollow(const std::string &nodeIdStr, bool isColorFollow) override;
    void SetFreeMultiWindowStatus(bool enable) override;
    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList) override;
    void HgmForceUpdateTask(bool flag, const std::string& fromWhom) override;
    void HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) override;
    ErrCode SetBehindWindowFilterEnabled(bool enabled) override;
    ErrCode GetBehindWindowFilterEnabled(bool& enabled) override;
    ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap) override;
    ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop) override;
    void SetVmaCacheStatus(bool flag) override;
    ErrCode SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh) override;
    int32_t GetPidGpuMemoryInMB(pid_t pid, float& gpuMemInMB) override;
    int32_t RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false) override;
    ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic) override;
    bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface) override;
    bool UnRegisterTypeface(uint64_t globalUniqueId) override;
    int32_t RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback) override;
    int32_t SetBrightnessInfoChangeCallback(pid_t pid, sptr<RSIBrightnessInfoChangeCallback> callback) override;
    int32_t RegisterSurfaceOcclusionChangeCallback( NodeId id, pid_t pid,
        sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;
    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;
    ErrCode CleanResources(pid_t pid) override;
    ErrCode RepaintEverything() override;
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ErrCode SetOverlayDisplayMode(int32_t mode) override;
#endif
    ErrCode ReportJankStats() override;
    ErrCode SetDiscardJankFrames(bool discardJankFrames) override;
    ErrCode ReportEventResponse(DataBaseRs info) override;
    ErrCode ReportEventComplete(DataBaseRs info) override;
    ErrCode ReportEventJankFrame(DataBaseRs info) override;
    ErrCode ReportRsSceneJankStart(AppInfo info) override;
    ErrCode ReportRsSceneJankEnd(AppInfo info) override;
    ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime) override;
    ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps) override;
    ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) override;
    ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics) override;
    ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode) override;
    ErrCode SetWatermark(
        pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success) override;
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) override;
    ErrCode GetSurfaceRootNodeId(NodeId& windowNodeId) override;
    int32_t RegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid, const RectConstraint& constraint,
        sptr<RSISelfDrawingNodeRectChangeCallback> callback) override;
    int32_t UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid) override;

    uint32_t GetRealtimeRefreshRate(ScreenId screenId) override;
    void SetShowRefreshRateEnabled(bool enabled, int32_t type) override;
    ErrCode GetShowRefreshRateEnabled(bool& enable) override;
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
    static inline BrokerDelegator<RSServiceToRenderConnectionProxy> delegator_;
    void ReportDataBaseRs(MessageParcel& data, MessageParcel& reply, MessageOption& option, DataBaseRs info);
    void WriteAppInfo(MessageParcel& data, MessageParcel& reply, MessageOption& option, AppInfo info);
    ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList) override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_ZIDL_RS_SERVICE_TO_RENDER_CONNECTION_PROXY_H