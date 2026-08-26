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

#include "rs_service_to_render_connection.h"

#include "frame_report.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#undef LOG_TAG
#define LOG_TAG "RSServiceToRenderConnection"

namespace OHOS {
namespace Rosen {
bool RSServiceToRenderConnection::NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
    const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    return renderProcessAgent_->NotifyScreenConnectInfoToRender(
        screenProperty, renderToComposerConn, composerToRenderConn);
}

bool RSServiceToRenderConnection::NotifyScreenDisconnectInfoToRender(ScreenId screenId)
{
    return renderProcessAgent_->NotifyScreenDisconnectInfoToRender(screenId);
}

bool RSServiceToRenderConnection::NotifyScreenPropertyChangedInfoToRender(
    ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& screenProperty)
{
    return renderProcessAgent_->NotifyScreenPropertyChangedInfoToRender(id, type, screenProperty);
}

int32_t RSServiceToRenderConnection::NotifyScreenRefresh(ScreenId screenId)
{
    return renderPipelineAgent_->NotifyScreenRefresh(screenId);
}

ErrCode RSServiceToRenderConnection::ReportJankStats()
{
    return renderPipelineAgent_->ReportJankStats();
}

ErrCode RSServiceToRenderConnection::ReportEventResponse(DataBaseRs info)
{
    return renderPipelineAgent_->ReportEventResponse(info);
}

ErrCode RSServiceToRenderConnection::ReportEventComplete(DataBaseRs info)
{
    return renderPipelineAgent_->ReportEventComplete(info);
}

ErrCode RSServiceToRenderConnection::ReportEventJankFrame(DataBaseRs info)
{
    return renderPipelineAgent_->ReportEventJankFrame(info);
}

ErrCode RSServiceToRenderConnection::ReportRsSceneJankStart(AppInfo info)
{
    return renderPipelineAgent_->ReportRsSceneJankStart(info);
}

ErrCode RSServiceToRenderConnection::ReportRsSceneJankEnd(AppInfo info)
{
    return renderPipelineAgent_->ReportRsSceneJankEnd(info);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    return renderPipelineAgent_->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    return renderPipelineAgent_->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoGet(uint64_t uniqueId)
{
    return renderPipelineAgent_->AvcodecVideoGet(uniqueId);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoGetRecent()
{
    return renderPipelineAgent_->AvcodecVideoGetRecent();
}

void RSServiceToRenderConnection::DoDump(std::unordered_set<std::u16string>& argSets, sptr<RSIDumpCallback> callback)
{
    renderPipelineAgent_->DoDump(argSets, callback);
}

void RSServiceToRenderConnection::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    renderPipelineAgent_->NotifyPackageEvent(packageList);
}

void RSServiceToRenderConnection::NotifyWindowModeTypeEvent(uint8_t windowModeType)
{
    renderPipelineAgent_->NotifyWindowModeTypeEvent(windowModeType);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSServiceToRenderConnection::SetOverlayDisplayMode(int32_t mode)
{
    return renderPipelineAgent_->SetOverlayDisplayMode(mode);
}
#endif

#ifdef RS_ENABLE_TV_PQ_METADATA
ErrCode RSServiceToRenderConnection::SendVideoRateInfo(
    const std::unordered_map<std::string, std::string>& videoRateInfo)
{
    return renderPipelineAgent_->SendVideoRateInfo(videoRateInfo);
}
#endif

void RSServiceToRenderConnection::ReportGameStateData(GameStateData info)
{
    RS_LOGI("[game_accelerate_schedule] ReportGameStateData = %{public}s, uid = %{public}d, state = %{public}d,"
        "pid = %{public}d renderTid = %{public}d ",
        info.bundleName.c_str(), info.uid, info.state, info.pid, info.renderTid);
    FrameReport::GetInstance().SetGameScene(info.pid, info.state);
}

ErrCode RSServiceToRenderConnection::GetBehindWindowFilterEnabled(bool& enabled)
{
    enabled = renderPipelineAgent_->GetBehindWindowFilterEnabled();
    return ERR_OK;
}

ErrCode RSServiceToRenderConnection::SetApsConfigParams(
    ApsEventType event, const std::unordered_map<std::string, std::string>& params)
{
    return renderPipelineAgent_->SetApsConfigParams(event, params);
}

int32_t RSServiceToRenderConnection::SetBrightnessInfoChangeCallback(pid_t pid,
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    return renderPipelineAgent_->SetBrightnessInfoChangeCallback(pid, callback);
}

ErrCode RSServiceToRenderConnection::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    return renderPipelineAgent_->GetMemoryGraphic(pid, memoryGraphic);
}

bool RSServiceToRenderConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    return renderPipelineAgent_->RegisterTypeface(globalUniqueId, typeface);
}

bool RSServiceToRenderConnection::RegisterTypeface(Drawing::SharedTypeface& sharedTypeface, bool isLocal)
{
    return renderPipelineAgent_->RegisterTypeface(sharedTypeface, isLocal);
}

bool RSServiceToRenderConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    return renderPipelineAgent_->UnRegisterTypeface(globalUniqueId);
}

void RSServiceToRenderConnection::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    renderPipelineAgent_->HgmForceUpdateTask(flag, fromWhom);
}

ErrCode RSServiceToRenderConnection::RepaintEverything()
{
    ErrCode errCode = renderPipelineAgent_->RepaintEverything();
    RS_LOGI("%{public}s call renderPipelineAgent_ errCode: %{public}d", __func__, errCode);
    return errCode;
}

ErrCode RSServiceToRenderConnection::SetRogScreenResolution(ScreenId screenId, uint32_t width, uint32_t height,
    ScreenSamplingMode samplingMode)
{
    ErrCode errCode = renderPipelineAgent_->SetRogScreenResolution(screenId, width, height, samplingMode);
    RS_LOGI("%{public}s call renderPipelineAgent_ screenId: %{public}" PRIu64 " width: %{public}u, height: %{public}u, "
        "samplingMode: %{public}u, errCode: %{public}d",
        __func__, screenId, width, height, static_cast<uint32_t>(samplingMode), errCode);
    return errCode;
}

void RSServiceToRenderConnection::ForceRefreshOneFrameWithNextVSync()
{
    renderPipelineAgent_->ForceRefreshOneFrameWithNextVSync();
}

ErrCode RSServiceToRenderConnection::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    ErrCode errCode = renderPipelineAgent_->SetLayerTop(nodeIdStr, isTop);
    return errCode;
}

ErrCode RSServiceToRenderConnection::SetHdrForceHwcEnabled(const std::string &nodeIdStr, bool isHdrForceHwcEnabled)
{
    return renderPipelineAgent_->SetHdrForceHwcEnabled(nodeIdStr, isHdrForceHwcEnabled);
}

ErrCode RSServiceToRenderConnection::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap, bool transformEnabled)
{
    return renderPipelineAgent_->CreatePixelMapFromSurface(surface, srcRect, pixelMap, transformEnabled);
}

int32_t RSServiceToRenderConnection::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    int32_t errCode = renderPipelineAgent_->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    return errCode;
}

void RSServiceToRenderConnection::SetVmaCacheStatus(bool flag)
{
    renderPipelineAgent_->SetVmaCacheStatus(flag);
}

ErrCode RSServiceToRenderConnection::SetUIMode3D(UIMode3D mode)
{
    return renderPipelineAgent_->SetUIMode3D(mode);
}

ErrCode RSServiceToRenderConnection::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    return renderPipelineAgent_->SetForceRefresh(nodeIdStr, isForceRefresh);
}

int32_t RSServiceToRenderConnection::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    return renderPipelineAgent_->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
}

int32_t RSServiceToRenderConnection::AuthorizeUIExtensionPid(pid_t pid, NodeId nodeId, pid_t guestPid,
    bool authorized, bool enforceQuota)
{
    // defense in depth: render_service already enforced ownership, re-verify the forwarded
    // host pid matches the NodeId before touching the node map
    if (ExtractPid(nodeId) != pid) {
        RS_LOGW("RSServiceToRenderConnection::AuthorizeUIExtensionPid denied, nodeId[%{public}" PRIu64
                "] hostPid[%{public}d]",
            nodeId, static_cast<int32_t>(pid));
        return ERR_INVALID_DATA;
    }
    // only render subprocesses (renderProcessAgent_ != nullptr, multi-process mode) require the
    // guest to hold a client-to-render connection before accepting the authorization; the
    // single-process pipeline observes every client's death via RSRenderService::CreateConnection
    // and recycles entries there (divided render clients never call CreateRenderConnection)
    return renderPipelineAgent_->AuthorizeUIExtensionPid(
        nodeId, guestPid, authorized, enforceQuota, renderProcessAgent_ != nullptr);
}

ErrCode RSServiceToRenderConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    return renderPipelineAgent_->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
}

ErrCode RSServiceToRenderConnection::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    return renderPipelineAgent_->GetMemoryGraphics(memoryGraphics);
}

ErrCode RSServiceToRenderConnection::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector,
    pid_t pid, int32_t& repCode)
{
    return renderPipelineAgent_->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
}

ErrCode RSServiceToRenderConnection::SetUifirstScale(float scaleFactor)
{
    RS_LOGD("RSServiceToRenderConnection::SetUifirstScale scaleFactor:%{public}f", scaleFactor);
    return renderPipelineAgent_->SetUifirstScale(scaleFactor);
}

ErrCode RSServiceToRenderConnection::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    return renderPipelineAgent_->SetColorFollow(nodeIdStr, isColorFollow);
}

ErrCode RSServiceToRenderConnection::GetSurfaceRootNodeId(NodeId& windowNodeId)
{
    renderPipelineAgent_->GetSurfaceRootNodeId(windowNodeId);
    return ERR_OK;
}

uint32_t RSServiceToRenderConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    return renderPipelineAgent_->GetRealtimeRefreshRate(screenId);
}

ErrCode RSServiceToRenderConnection::GetShowRefreshRateEnabled(bool& enable)
{
    return renderPipelineAgent_->GetShowRefreshRateEnabled(enable);
}

std::vector<ActiveDirtyRegionInfo> RSServiceToRenderConnection::GetActiveDirtyRegionInfo()
{
    return renderPipelineAgent_->GetActiveDirtyRegionInfo();
}

GlobalDirtyRegionInfo RSServiceToRenderConnection::GetGlobalDirtyRegionInfo()
{
    return renderPipelineAgent_->GetGlobalDirtyRegionInfo();
}

LayerComposeInfo RSServiceToRenderConnection::GetLayerComposeInfo()
{
    return renderPipelineAgent_->GetLayerComposeInfo();
}

HwcDisabledReasonInfos RSServiceToRenderConnection::GetHwcDisabledReasonInfo()
{
    return renderPipelineAgent_->GetHwcDisabledReasonInfo();
}

ErrCode RSServiceToRenderConnection::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    return renderPipelineAgent_->GetHdrOnDuration(hdrOnDuration);
}

ErrCode RSServiceToRenderConnection::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    return renderPipelineAgent_->SetOptimizeCanvasDirtyPidList(pidList);
}

void RSServiceToRenderConnection::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    renderPipelineAgent_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
}

void RSServiceToRenderConnection::OnScreenBacklightChanged(const RsScreenBrightnessData& brightnessData)
{
    renderPipelineAgent_->OnScreenBacklightChanged(brightnessData);
}

void RSServiceToRenderConnection::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    renderPipelineAgent_->OnGlobalBlacklistChanged(globalBlackList);
}

void RSServiceToRenderConnection::SetCacheEnabledForRotation(bool enabled)
{
    renderPipelineAgent_->SetCacheEnabledForRotation(enabled);
}

int32_t RSServiceToRenderConnection::SendTransfer(const std::shared_ptr<RSIpcTransferBase>& transfer)
{
    if (renderPipelineAgent_ == nullptr || !transfer) {
        return StatusCode::RS_CONNECTION_ERROR;
    }
    return transfer->Apply(renderPipelineAgent_) ? StatusCode::SUCCESS : StatusCode::RS_CONNECTION_ERROR;
}
} // namespace Rosen
} // namespace OHOS
