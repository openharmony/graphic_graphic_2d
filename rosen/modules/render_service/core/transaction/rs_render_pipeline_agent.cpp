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

#include "rs_render_pipeline_agent.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_layer_transaction_data.h"
#include "screen_manager/screen_types.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "pipeline/rs_task_dispatcher.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderPipelineAgent"

namespace OHOS {
namespace Rosen {
RSRenderPipelineAgent::RSRenderPipelineAgent(std::shared_ptr<RSRenderPipeline>& renderPipeline) :
    rsRenderPipeline_(renderPipeline) {}

bool RSRenderPipelineAgent::GetHighContrastTextState()
{
    return rsRenderPipeline_ != nullptr && rsRenderPipeline_->GetHighContrastTextState();
}

ErrCode RSRenderPipelineAgent::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetBitmap(id, bitmap, success);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::SetDiscardJankFrames(bool discardJankFrames)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportJankStats();
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportJankStats()
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportJankStats();
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventResponse(DataBaseRs info)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportEventResponse(info);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventComplete(DataBaseRs info)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportEventComplete(info);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportEventJankFrame(DataBaseRs info)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportEventJankFrame(info);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankStart(AppInfo info)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportRsSceneJankStart(info);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::ReportRsSceneJankEnd(AppInfo info)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->ReportRsSceneJankEnd(info);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->CreateNodeAndSurface(config, sfc, unobscured);
    }
    return ERR_INVALID_VALUE;
}

int32_t RSRenderPipelineAgent::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    if (!rsRenderPipeline_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->RegisterOcclusionChangeCallback(pid, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (!rsRenderPipeline_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!callback) {
        RS_LOGE("RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    return StatusCode::SUCCESS;
}

int32_t RSRenderPipelineAgent::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (!rsRenderPipeline_) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    rsRenderPipeline_->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}
ErrCode RSRenderPipelineAgent::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
    std::shared_ptr<Media::PixelMap> &pixelMap)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetMemoryGraphic(pid, memoryGraphic);
    }
    return ERR_INVALID_VALUE;
}

void RSRenderPipelineAgent::HandleHwcPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    if (rsRenderPipeline_ != nullptr) {
        rsRenderPipeline_->HandleHwcPackageEvent(listSize, packageList);
    }
}

ErrCode RSRenderPipelineAgent::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetLayerTop(nodeIdStr, isTop);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetMemoryGraphics(memoryGraphics);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::SetWatermark(pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetWatermark(callingPid, name, watermark, success);
    }
    return ERR_INVALID_VALUE;
}

void RSRenderPipelineAgent::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    rsRenderPipeline_->ShowWatermark(watermarkImg, isShow);
}

ErrCode RSRenderPipelineAgent::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetForceRefresh(nodeIdStr, isForceRefresh);
    }
    return ERR_INVALID_VALUE;
}

int32_t RSRenderPipelineAgent::NotifyScreenRefresh(ScreenId screenId)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return -1;
    }
    rsRenderPipeline_->OnScreenRefresh(screenId);
    return 0;
}

void RSRenderPipelineAgent::DoDump(std::unordered_set<std::u16string> &argSets)
{
    if (rsRenderPipeline_ != nullptr) {
        rsRenderPipeline_->DoDump(argSets);
    }
}

void RSRenderPipelineAgent::NotifyHwcEventToRender(
    uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("NotifyHwcEventToRender renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->HandleHwcEvent(deviceId, eventId, eventData);
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSRenderPipelineAgent::SetOverlayDisplayMode(int32_t mode)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetOverlayDisplayMode(mode);
    }
    return ERR_INVALID_VALUE;
}
#endif
void RSRenderPipelineAgent::SetVmaCacheStatus(bool flag)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RSRenderPipelineAgent renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->SetVmaCacheStatus(flag);
}

void RSRenderPipelineAgent::SetBehindWindowFilterEnabled(bool enabled)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->SetBehindWindowFilterEnabled(enabled);
}

bool RSRenderPipelineAgent::GetBehindWindowFilterEnabled()
{
    bool enabled = false;
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return enabled;
    }
    enabled = rsRenderPipeline_->GetBehindWindowFilterEnabled();
    return enabled;
}

int32_t RSRenderPipelineAgent::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RegisterUIExtensionCallback rsRenderPipeline_ is nullptr, return");
        return ERR_INVALID_VALUE;
    }
    return rsRenderPipeline_->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
}

bool RSRenderPipelineAgent::RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->RegisterTypeface(globalUniqueId, typeface);
    }
    return false;
}

bool RSRenderPipelineAgent::UnRegisterTypeface(uint64_t globalUniqueId)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->UnRegisterTypeface(globalUniqueId);
    }
    return false;
}

void RSRenderPipelineAgent::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    if (rsRenderPipeline_ != nullptr) {
        RS_LOGI("dmulti_process RSRenderPipelineAgent::HgmForceUpdateTask");
        rsRenderPipeline_->HgmForceUpdateTask(flag, fromWhom);
    }
}

int32_t RSRenderPipelineAgent::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("GetPidGpuMemoryInMB rsRenderPipeline_ is nullptr, return");
        return ERR_INVALID_VALUE;
    }
    return rsRenderPipeline_->GetPidGpuMemoryInMB(pid, gpuMemInMB);
}

ErrCode RSRenderPipelineAgent::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    std::promise<bool> result;
    std::future<bool> future = result.get_future();
#ifdef RS_ENABLE_GPU
    RSMainThread* mainThread = rsRenderPipeline_->mainThread_;
    RSUniRenderThread* renderThread = rsRenderPipeline_->uniRenderThread_;
    auto getPixelMapTask = [id, pixelmap, rect, drawCmdList, mainThread, renderThread, &result]() {
        auto node = mainThread->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
        if (node == nullptr) {
            RS_LOGD("GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", id);
            result.set_value(false);
            return;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            result.set_value(false);
            return;
        }

        auto tid = node->GetTid(); // planning: id may change in subthread
        auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
        if (drawableNode) {
            tid = std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)->GetTid();
        }
        auto getDrawablePixelmapTask = [drawableNode, pixelmap, rect, &result, tid, drawCmdList]() {
            result.set_value(std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)->
                GetPixelmap(pixelmap, rect, tid, drawCmdList));
        };
        if (!node->IsOnTheTree()) {
            node->ClearOp();
        }
        if (tid == UNI_MAIN_THREAD_INDEX) {
            if (!mainThread->IsIdle() && mainThread->GetContext().HasActiveNode(node)) {
                result.set_value(false);
                return;
            }
            result.set_value(node->GetPixelmap(pixelmap, rect, tid, drawCmdList));
        } else if (tid == UNI_RENDER_THREAD_INDEX) {
            renderThread->PostTask(getDrawablePixelmapTask);
        } else {
            const auto& tidMap = RSSubThreadManager::Instance()->GetReThreadIndexMap();
            if (auto found = tidMap.find(tid); found != tidMap.end()) {
                RSTaskDispatcher::GetInstance().PostTask(found->second, getDrawablePixelmapTask, false);
            } else {
                renderThread->PostTask(getDrawablePixelmapTask);
            }
        }
    };
    renderThread->PostTask(getPixelMapTask);
#endif
    success = future.get();
    return ERR_OK;
}

ErrCode RSRenderPipelineAgent::RepaintEverything()
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RepaintEverything rsRenderPipeline_ is nullptr, return");
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = rsRenderPipeline_->RepaintEverything();
    RS_LOGI("RepaintEverything call rsRenderPipeline_, errCode: %{public}d", errCode);
    return errCode;
}

ErrCode RSRenderPipelineAgent::CleanResources(pid_t pid)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RepaintEverything rsRenderPipeline_ is nullptr, return");
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = rsRenderPipeline_->CleanResources(pid);
    RS_LOGI("CleanResources call rsRenderPipeline_, errCode: %{public}d", errCode);
    return errCode;
}

ErrCode RSRenderPipelineAgent::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("SetColorFollow rsRenderPipeline_ is nullptr, return");
        return ERR_INVALID_VALUE;
    }
    return rsRenderPipeline_->SetColorFollow(nodeIdStr, isColorFollow);
}

void RSRenderPipelineAgent::GetSurfaceRootNodeId(NodeId &windowNodeId)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->GetSurfaceRootNodeId(windowNodeId);
}

void RSRenderPipelineAgent::SetFreeMultiWindowStatus(bool enable)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmuti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->SetFreeMultiWindowStatus(enable);
}

int32_t RSRenderPipelineAgent::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s rsRenderPipeline_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    return rsRenderPipeline_->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
}

int32_t RSRenderPipelineAgent::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process %{public}s rsRenderPipeline_ is nullptr", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    return rsRenderPipeline_->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
}

uint32_t RSRenderPipelineAgent::GetRealtimeRefreshRate(ScreenId screenId)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetRealtimeRefreshRate(screenId);
    }
    return 0;
}

void RSRenderPipelineAgent::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    if (rsRenderPipeline_ != nullptr) {
        rsRenderPipeline_->SetShowRefreshRateEnabled(enabled, type);
    }
}

ErrCode RSRenderPipelineAgent::GetShowRefreshRateEnabled(bool& enable)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->GetShowRefreshRateEnabled(enable);
    }
    return ERR_INVALID_VALUE;
}

ErrCode RSRenderPipelineAgent::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    if (rsRenderPipeline_ != nullptr) {
        return rsRenderPipeline_->SetGpuCrcDirtyEnabledPidList(pidList);
    }
    return ERR_INVALID_VALUE;
}

std::vector<ActiveDirtyRegionInfo> RSRenderPipelineAgent::GetActiveDirtyRegionInfo()
{
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return activeDirtyRegionInfos;
    }
    return rsRenderPipeline_->GetActiveDirtyRegionInfo();
}

GlobalDirtyRegionInfo RSRenderPipelineAgent::GetGlobalDirtyRegionInfo()
{
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return globalDirtyRegionInfo;
    }
    return rsRenderPipeline_->GetGlobalDirtyRegionInfo();
}

LayerComposeInfo RSRenderPipelineAgent::GetLayerComposeInfo()
{
    LayerComposeInfo layerComposeInfo;
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return layerComposeInfo;
    }
    return rsRenderPipeline_->GetLayerComposeInfo();
}

HwcDisabledReasonInfos RSRenderPipelineAgent::GetHwcDisabledReasonInfo()
{
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return hwcDisabledReasonInfos;
    }
    return rsRenderPipeline_->GetHwcDisabledReasonInfo();
}

ErrCode RSRenderPipelineAgent::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return hdrOnDuration;
    }
    return rsRenderPipeline_->GetHdrOnDuration(hdrOnDuration);
}

ErrCode RSRenderPipelineAgent::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return ERR_INVALID_VALUE;
    }
    return rsRenderPipeline_->SetOptimizeCanvasDirtyPidList(pidList);
}

void RSRenderPipelineAgent::OnScreenBacklightChanged(ScreenId screenId, uint32_t level)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("dmulti_process RSRenderPipelineAgent renderPipeline is nullptr");
        return;
    }
    rsRenderPipeline_->OnScreenBacklightChanged(screenId, level);
}

void RSRenderPipelineAgent::SetScreenFrameGravity(ScreenId id, Gravity gravity)
{
    if (rsRenderPipeline_ == nullptr) {
        RS_LOGE("RSRenderPipelineAgent:%{public}s renderPipeline is nullptr", __func__);
        return;
    }
    rsRenderPipeline_->SetScreenFrameGravity(id, gravity);
}
} // namespace Rosen
} // namespace OHOS
