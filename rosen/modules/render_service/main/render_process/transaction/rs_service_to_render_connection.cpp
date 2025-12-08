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

#include "transaction/rs_service_to_render_connection.h"

#include "rs_render_composer_client.h"
#include "rs_trace.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "frame_report.h"

#undef LOG_TAG
#define LOG_TAG "RSServiceToRenderConnection"

namespace OHOS {
namespace Rosen {
int32_t RSServiceToRenderConnection::NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
    sptr<IRSRenderToComposerConnection> composerConn)
{
    if (renderProcessAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s: renderProcessAgent_ is nullptr", __func__);
        return -1;
    }
    renderProcessAgent_->NotifyScreenConnectInfoToRender(screenProperty, composerConn);
    return 0;
}

int32_t RSServiceToRenderConnection::NotifyScreenDisconnectInfoToRender(ScreenId screenId)
{
    if (renderProcessAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s: renderProcessAgent_ is nullptr", __func__);
        return -1;
    }
    renderProcessAgent_->NotifyScreenDisconnectInfoToRender(screenId);
    return 0;
}

int32_t RSServiceToRenderConnection::NotifyScreenPropertyChangedInfoToRender(const sptr<RSScreenProperty>& screenProperty)
{
    if (renderProcessAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s: renderProcessAgent_ is nullptr", __func__);
        return -1;
    }
    renderProcessAgent_->NotifyScreenPropertyChangedInfoToRender(screenProperty);
    return 0;
}

int32_t RSServiceToRenderConnection::NotifyScreenRefresh(ScreenId screenId)
{
    return renderPipelineAgent_->NotifyScreenRefresh(screenId);
}

ErrCode RSServiceToRenderConnection::SetDiscardJankFrames(bool discardJankFrames)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetDiscardJankFrames(discardJankFrames);
}

ErrCode RSServiceToRenderConnection::ReportJankStats()
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportJankStats();
}

ErrCode RSServiceToRenderConnection::ReportEventResponse(DataBaseRs info)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportEventResponse(info);
}

ErrCode RSServiceToRenderConnection::ReportEventComplete(DataBaseRs info)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportEventComplete(info);
}

ErrCode RSServiceToRenderConnection::ReportEventJankFrame(DataBaseRs info)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportEventJankFrame(info);
}

ErrCode RSServiceToRenderConnection::ReportRsSceneJankStart(AppInfo info)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportRsSceneJankStart(info);
}

ErrCode RSServiceToRenderConnection::ReportRsSceneJankEnd(AppInfo info)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->ReportRsSceneJankEnd(info);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
}

ErrCode RSServiceToRenderConnection::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
}

void RSServiceToRenderConnection::DoDump(std::unordered_set<std::u16string> &argSets)
{
    renderPipelineAgent_->DoDump(argSets);
}

void RSServiceToRenderConnection::NotifyPackageEvnet(uint32_t listSize, const std::vector<std::string>& packageList)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->NotifyPackageEvnet(listSize, packageList);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSServiceToRenderConnection::SetOverlayDisplayMode(int32_t mode)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process RSServiceToRenderConnection renderPipelineAgent is nullptr");
        return ERR_INVALID_VALUE;
    }
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
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    renderPipelineAgent_->SetBehindWindowFilterEnabled(enabled);
    return ERR_OK;
}

ErrCode RSServiceToRenderConnection::GetBehindWindowFilterEnabled(bool& enabled)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    enabled = renderPipelineAgent_->GetBehindWindowFilterEnabled();
    return ERR_OK;
}

int32_t RSServiceToRenderConnection::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->RegisterOcclusionChangeCallback(pid, callback);
}

int32_t RSServiceToRenderConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
}

int32_t RSServiceToRenderConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
  return renderPipelineAgent_->UnRegisterSurfaceOcclusionChangeCallback(id);
}

ErrCode RSServiceToRenderConnection::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetMemoryGraphic(pid, memoryGraphic);
}

bool RSServiceToRenderConnection::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return false;
    }
    return renderPipelineAgent_->RegisterTypeface(globalUniqueId, typeface);
}

bool RSServiceToRenderConnection::UnRegisterTypeface(uint64_t globalUniqueId)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return false;
    }
    return renderPipelineAgent_->UnRegisterTypeface(globalUniqueId);
}

void RSServiceToRenderConnection::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->HgmForceUpdateTask(flag, fromWhom);
}

void RSServiceToRenderConnection::HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->NotifyHwcEventToRender(deviceId, eventId, eventData);
}

ErrCode RSServiceToRenderConnection::CleanResources(pid_t pid)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    RS_TRACE_NAME_FMT("RSServiceToRenderConnection::CleanResources pid is %d", pid);
    return renderPipelineAgent_->CleanResources(pid);
}

ErrCode RSServiceToRenderConnection::RepaintEverything()
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = renderPipelineAgent_->RepaintEverything();
    RS_LOGI("%{public}s call renderPipelineAgent_ errCode: %{public}d", __func__, errCode);
    return errCode;
}

ErrCode RSServiceToRenderConnection::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = renderPipelineAgent_->SetLayerTop(nodeIdStr, isTop);
    return errCode;
}

ErrCode RSServiceToRenderConnection::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
}

int32_t RSServiceToRenderConnection::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t errCode = renderPipelineAgent_->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    return errCode;
}

void RSServiceToRenderConnection::SetVmaCacheStatus(bool flag)
{
    renderPipelineAgent_->SetVmaCacheStatus(flag);
}

ErrCode RSServiceToRenderConnection::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetForceRefresh(nodeIdStr, isForceRefresh);
}

int32_t RSServiceToRenderConnection::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    return renderPipelineAgent_->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
}

ErrCode RSServiceToRenderConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
}

ErrCode RSServiceToRenderConnection::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetMemoryGraphics(memoryGraphics);
}

ErrCode RSServiceToRenderConnection::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
}

ErrCode RSServiceToRenderConnection::SetWatermark(pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetWatermark(callingPid, name, watermark, success);
}

void RSServiceToRenderConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->ShowWatermark(watermarkImg, isShow);
}

ErrCode RSServiceToRenderConnection::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->SetColorFollow(nodeIdStr, isColorFollow);
}

ErrCode RSServiceToRenderConnection::GetSurfaceRootNodeId(NodeId& windowNodeId)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    renderPipelineAgent_->GetSurfaceRootNodeId(windowNodeId);
    return ERR_OK;
}

void RSServiceToRenderConnection::SetFreeMultiWindowStatus(bool enable)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    return renderPipelineAgent_->SetFreeMultiWindowStatus(enable);
}

int32_t RSServiceToRenderConnection::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    return renderPipelineAgent_->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
}

int32_t RSServiceToRenderConnection::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    return renderPipelineAgent_->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
}

uint32_t RSServiceToRenderConnection::GetRealtimeRefreshRate(ScreenId screenId)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    return renderPipelineAgent_->GetRealtimeRefreshRate(screenId);
}

void RSServiceToRenderConnection::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->SetShowRefreshRateEnabled(enabled, type);
}

ErrCode RSServiceToRenderConnection::GetShowRefreshRateEnabled(bool& enable)
{
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
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
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
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
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGE("%{public}s renderPipelineAgent_ is nullptr", __func__);
        return;
    }
    renderPipelineAgent_->OnScreenBacklightChanged(screenId, level);
}
} // namespace Rosen
} // namespace OHOS