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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_render_composer_client.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSServiceToRenderConnection"

namespace OHOS {
namespace Rosen {
int32_t RSServiceToRenderConnection::NotifyScreenRefresh(ScreenId screenId)
{
    return renderPipelineAgent_->NotifyScreenRefresh(screenId);
}

ErrCode RSServiceToRenderConnection::SetDiscardJankFrames(bool discardJankFrames)
{
    return renderPipelineAgent_->SetDiscardJankFrames(discardJankFrames);
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

void RSServiceToRenderConnection::DoDump(std::unordered_set<std::u16string>& argSets, sptr<RSIDumpCallback> callback)
{
    renderPipelineAgent_->DoDump(argSets, callback);
}

void RSServiceToRenderConnection::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    renderPipelineAgent_->NotifyPackageEvent(packageList);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSServiceToRenderConnection::SetOverlayDisplayMode(int32_t mode)
{
    return renderPipelineAgent_->SetOverlayDisplayMode(mode);
}
#endif

void RSServiceToRenderConnection::ReportGameStateData(GameStateData info)
{
    RS_LOGI("[game_accelerate_schedule] ReportGameStateData = %{public}s, uid = %{public}d, state = %{public}d,"
        "pid = %{public}d renderTid = %{public}d ",
        info.bundleName.c_str(), info.uid, info.state, info.pid, info.renderTid);
    FrameReport::GetInstance().SetGameScene(info.pid, info.state);
}

ErrCode RSServiceToRenderConnection::SetBehindWindowFilterEnabled(bool enabled)
{
    renderPipelineAgent_->SetBehindWindowFilterEnabled(enabled);
    return ERR_OK;
}

ErrCode RSServiceToRenderConnection::GetBehindWindowFilterEnabled(bool& enabled)
{
    enabled = renderPipelineAgent_->GetBehindWindowFilterEnabled();
    return ERR_OK;
}

int32_t RSServiceToRenderConnection::SetBrightnessInfoChangeCallback(pid_t pid,
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    return renderPipelineAgent_->SetBrightnessInfoChangeCallback(pid, callback);
}

int32_t RSServiceToRenderConnection::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    return renderPipelineAgent_->RegisterOcclusionChangeCallback(pid, callback);
}

int32_t RSServiceToRenderConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    return renderPipelineAgent_->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
}

int32_t RSServiceToRenderConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
  return renderPipelineAgent_->UnRegisterSurfaceOcclusionChangeCallback(id);
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

bool RSServiceToRenderConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    return renderPipelineAgent_->UnRegisterTypeface(globalUniqueId);
}

void RSServiceToRenderConnection::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    renderPipelineAgent_->HgmForceUpdateTask(flag, fromWhom);
}

void RSServiceToRenderConnection::HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    renderPipelineAgent_->NotifyHwcEventToRender(deviceId, eventId, eventData);
}

ErrCode RSServiceToRenderConnection::RepaintEverything()
{
    ErrCode errCode = renderPipelineAgent_->RepaintEverything();
    RS_LOGI("%{public}s call renderPipelineAgent_ errCode: %{public}d", __func__, errCode);
    return errCode;
}

ErrCode RSServiceToRenderConnection::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    ErrCode errCode = renderPipelineAgent_->SetLayerTop(nodeIdStr, isTop);
    return errCode;
}

ErrCode RSServiceToRenderConnection::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    return renderPipelineAgent_->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
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

ErrCode RSServiceToRenderConnection::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    return renderPipelineAgent_->SetForceRefresh(nodeIdStr, isForceRefresh);
}

int32_t RSServiceToRenderConnection::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    return renderPipelineAgent_->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
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

ErrCode RSServiceToRenderConnection::SetWatermark(pid_t callingPid, const std::string& name,
    std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    return renderPipelineAgent_->SetWatermark(callingPid, name, watermark, success);
}

void RSServiceToRenderConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    renderPipelineAgent_->ShowWatermark(watermarkImg, isShow);
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

void RSServiceToRenderConnection::SetFreeMultiWindowStatus(bool enable)
{
    renderPipelineAgent_->SetFreeMultiWindowStatus(enable);
}

int32_t RSServiceToRenderConnection::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    return renderPipelineAgent_->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
}

int32_t RSServiceToRenderConnection::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    return renderPipelineAgent_->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
}

uint32_t RSServiceToRenderConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    return renderPipelineAgent_->GetRealtimeRefreshRate(screenId);
}

void RSServiceToRenderConnection::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    renderPipelineAgent_->SetShowRefreshRateEnabled(enabled, type);
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

ErrCode RSServiceToRenderConnection::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    return renderPipelineAgent_->SetGpuCrcDirtyEnabledPidList(pidList);
}

ErrCode RSServiceToRenderConnection::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    return renderPipelineAgent_->SetOptimizeCanvasDirtyPidList(pidList);
}

void RSServiceToRenderConnection::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    renderPipelineAgent_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
}

void RSServiceToRenderConnection::OnScreenBacklightChanged(ScreenId screenId, uint32_t level)
{
    renderPipelineAgent_->OnScreenBacklightChanged(screenId, level);
}

void RSServiceToRenderConnection::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->OnGlobalBlacklistChanged(globalBlackList);
}
} // namespace Rosen
} // namespace OHOS