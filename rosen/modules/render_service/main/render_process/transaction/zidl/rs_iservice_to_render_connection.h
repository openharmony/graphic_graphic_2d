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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_ISERVICE_TO_RENDER_CONNECTION_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_ISERVICE_TO_RENDER_CONNECTION_H

#include <screen_manager/rs_screen_property.h>
#include "irs_render_to_composer_connection.h"
#include <sync_fence.h>

#include "platform/ohos/transaction/rs_irender_connection_token.h"

namespace OHOS {
namespace Rosen {

class RSIServiceToRenderConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.ServiceToRenderConnection");

    RSIServiceToRenderConnection() = default;
    virtual ~RSIServiceToRenderConnection() noexcept = default;

    virtual int32_t NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
        sptr<IRSRenderToComposerConnection> composerConn) = 0;
    virtual int32_t NotifyScreenDisconnectInfoToRender(ScreenId screenId) = 0;
    virtual int32_t NotifyScreenPropertyChangedInfoToRender(const sptr<RSScreenProperty>& screenProeprty) = 0;
    virtual int32_t NotifyScreenRefresh(ScreenId id) = 0;
    virtual int32_t RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback) = 0;
    virtual int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) = 0;
    virtual int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) = 0;
    virtual ErrCode ReportJankStats() = 0;
    virtual ErrCode SetDiscardJankFrames(bool discardJankFrames) = 0;
    virtual ErrCode ReportEventResponse(DataBaseRs info) = 0;
    virtual ErrCode ReportEventComplete(DataBaseRs info) = 0;
    virtual ErrCode ReportEventJankFrame(DataBaseRs info) = 0;
    virtual ErrCode ReportRsSceneJankStart(AppInfo info) = 0;
    virtual ErrCode ReportRsSceneJankEnd(AppInfo info) = 0;
    virtual ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime) = 0;
    virtual ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps) = 0;
    virtual ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) = 0;
    virtual ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics) = 0;
    virtual ErrCode GetPixelMapByProcessId(
        std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode) = 0;
    virtual ErrCode SetWatermark(
        pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success) = 0;
    virtual void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) = 0;
    virtual ErrCode GetSurfaceRootNodeId(NodeId& windowNodeId) = 0;
    virtual void DoDump(std::unordered_set<std::u16string> &argSets) = 0;
    virtual ErrCode SetColorFollow(const std::string &nodeIdStr, bool isColorFollow) = 0;
    virtual void SetFreeMultiWindowStatus(bool enable) = 0;
    virtual void HandleHwcPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList) = 0;
    virtual void HgmForceUpdateTask(bool flag, const std::string& fromWhom) = 0;
    virtual void HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) = 0;
    virtual ErrCode CleanResources(pid_t pid) = 0;
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    virtual ErrCode SetOverlayDisplayMode(int32_t mode) = 0;
#endif
    virtual ErrCode SetBehindWindowFilterEnabled(bool enabled) = 0;
    virtual ErrCode GetBehindWindowFilterEnabled(bool& enabled) = 0;
    virtual ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop) = 0;
    virtual ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap) = 0;
    virtual void SetVmaCacheStatus(bool flag) = 0;
    virtual ErrCode SetForceRefresh(const std::string& nodeIdStr, bool isForceRefresh) = 0;
    virtual int32_t GetPidGpuMemoryInMB(pid_t pid, float& gpuMemInMB) = 0;
    virtual int32_t RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false) = 0;
    virtual ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic) = 0;
    virtual bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface) = 0;
    virtual bool UnRegisterTypeface(uint64_t globalUniqueId) = 0;
    virtual ErrCode RepaintEverything() = 0;
    virtual int32_t RegisterSelfDrawingNodeRectChangeCallback(
        pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback) = 0;
    virtual int32_t UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid) = 0;
    virtual uint32_t GetRealtimeRefreshRate(ScreenId screenId) = 0;
    virtual void SetShowRefreshRateEnabled(bool enabled, int32_t type) = 0;
    virtual ErrCode GetShowRefreshRateEnabled(bool& enable) = 0;
    virtual std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() = 0;
    virtual GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() = 0;
    virtual LayerComposeInfo GetLayerComposeInfo() = 0;
    virtual HwcDisabledReasonInfos GetHwcDisabledReasonInfo() = 0;
    virtual ErrCode GetHdrOnDuration(int64_t& hdrOnDuration) = 0;
    virtual ErrCode SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList) = 0;

    virtual void ReportGameStateData(GameStateData info) = 0;
    virtual ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList) = 0;
    virtual void SetCurtainScreenUsingStatus(bool isCurtainScreenOn) = 0;
    virtual void OnScreenBacklightChanged(ScreenId screenId, uint32_t level) = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_ISERVICE_TO_RENDER_CONNECTION_H