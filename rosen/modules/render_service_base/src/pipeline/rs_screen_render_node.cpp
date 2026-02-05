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

#include "pipeline/rs_screen_render_node.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_special_layer_manager.h"
#include "feature/hdr/rs_colorspace_util.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "screen_manager/screen_types.h"
#include "visitor/rs_node_visitor.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
constexpr int64_t MAX_JITTER_NS = 2000000; // 2ms

RSScreenRenderNode::RSScreenRenderNode(
    NodeId id, ScreenId screenId, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(id, context), screenId_(screenId), dirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{
    RS_LOGI("RSScreenRenderNode ctor nodeId[%{public}" PRIu64 "], screenId[%{public}" PRIu64 "]", id, screenId_);
    RS_TRACE_NAME_FMT("RSScreenRenderNode ctor nodeId[%" PRIu64 "], screenId[%" PRIu64 "]", id, screenId_);
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryTrack::Instance().RegisterNodeMem(ExtractPid(id),
        sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSScreenRenderNode::~RSScreenRenderNode()
{
    RS_LOGI("RSScreen RSScreenRenderNode dtor id:%{public}" PRIu64 ", screenId:%{public}" PRIu64, GetId(), screenId_);
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemoryTrack::Instance().UnRegisterNodeMem(ExtractPid(GetId()),
        sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSScreenRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender,
    bool onlyFirstLevel)
{
    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
    }
}

void RSScreenRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareScreenRenderNode(*this);
}

void RSScreenRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareScreenRenderNode(*this);
}

void RSScreenRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessScreenRenderNode(*this);
}

void RSScreenRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId, NodeId screenNodeId, NodeId logicalDisplayNodeId)
{
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSRenderNode::SetIsOnTheTree(flag, GetId(), firstLevelNodeId, cacheNodeId, uifirstRootNodeId, GetId(),
        logicalDisplayNodeId);
}

void RSScreenRenderNode::SetForceSoftComposite(bool flag)
{
    forceSoftComposite_ = flag;
}

void RSScreenRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirroredScreen_ || node == nullptr || node == mirrorSource_.lock()) {
        return;
    }

    if (auto mirrorSource = mirrorSource_.lock()) {
        mirrorSource->SetHasMirrorScreen(false);
    }
    node->SetHasMirrorScreen(true);
    mirrorSource_ = node;
}

void RSScreenRenderNode::ResetMirrorSource()
{
    if (auto mirrorSource = mirrorSource_.lock()) {
        mirrorSource->SetHasMirrorScreen(false);
    }
    mirrorSource_.reset();
}

void RSScreenRenderNode::SetIsMirrorScreen(bool isMirror)
{
    if (isMirroredScreen_ != isMirror) {
        hasMirroredScreenChanged_ = true;
    }
    isMirroredScreen_ = isMirror;
}

void RSScreenRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        HILOG_COMM_ERROR("RSScreenRenderNode::InitRenderParams failed");
        return;
    }
#endif
}

ReleaseDmaBufferTask RSScreenRenderNode::releaseScreenDmaBufferTask_;
void RSScreenRenderNode::SetReleaseTask(ReleaseDmaBufferTask callback)
{
    if (!releaseScreenDmaBufferTask_ && callback) {
        releaseScreenDmaBufferTask_ = callback;
    } else {
        RS_LOGE("RreleaseScreenDmaBufferTask_ register failed!");
    }
}

void RSScreenRenderNode::OnSync()
{
#ifdef RS_ENABLE_GPU
    RS_OPTIONAL_TRACE_NAME_FMT("RSScreenRenderNode::OnSync global dirty[%s]",
        dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::OnSync screenParams is null");
        return;
    }
    if (!renderDrawable_) {
        return;
    }
    auto syncDirtyManager = renderDrawable_->GetSyncDirtyManager();
    dirtyManager_->OnSync(syncDirtyManager);
    screenParams->SetZoomed(curZoomState_);
    screenParams->SetNeedSync(true);
    RSRenderNode::OnSync();
#endif
}

void RSScreenRenderNode::HandleCurMainAndLeashSurfaceNodes()
{
    surfaceCountForMultiLayersPerf_ = 0;
    for (const auto& surface : curMainAndLeashSurfaceNodes_) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
        if (!surfaceNode || surfaceNode->IsLeashWindow() || !surfaceNode->IsOnTheTree()) {
            continue;
        }
        surfaceCountForMultiLayersPerf_++;
    }
    curMainAndLeashSurfaceNodes_.clear();
}

void RSScreenRenderNode::UpdateRenderParams()
{
#ifdef RS_ENABLE_GPU
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::UpdateRenderParams ScreenParams is null");
        return;
    }
    auto mirroredNode = GetMirrorSource().lock();
    if (mirroredNode) {
        screenParams->mirrorSourceDrawable_ = mirroredNode->GetRenderDrawable();
    } else {
        screenParams->mirrorSourceDrawable_.reset();
    }
    screenParams->childDisplayCount_ = GetChildrenCount();
    screenParams->screenInfo_ = screenInfo_;
    screenParams->screenProperty_ = screenProperty_;
    screenParams->logicalDisplayNodeDrawables_ = std::move(logicalDisplayNodeDrawables_);
    screenParams->SetHasMirroredScreenChanged(hasMirroredScreenChanged_);
    screenParams->isVirtualSurfaceChanged_ = isVirtualSurfaceChanged_;
    screenParams->roundCornerSurfaceDrawables_.clear();
    if (rcdSurfaceNodeTop_ && rcdSurfaceNodeTop_->GetRenderDrawable() != nullptr) {
        screenParams->roundCornerSurfaceDrawables_.push_back(rcdSurfaceNodeTop_->GetRenderDrawable());
    }
    if (rcdSurfaceNodeBottom_ && rcdSurfaceNodeBottom_->GetRenderDrawable() != nullptr) {
        screenParams->roundCornerSurfaceDrawables_.push_back(rcdSurfaceNodeBottom_->GetRenderDrawable());
    }
    RSRenderNode::UpdateRenderParams();
#endif
}

void RSScreenRenderNode::UpdateScreenRenderParams()
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::UpdateScreenRenderParams screenParams is null");
        return;
    }
    screenParams->compositeType_ = GetCompositeType();
    screenParams->hasChildCrossNode_ = HasChildCrossNode();
    screenParams->isMirrorScreen_ = IsMirrorScreen();
    screenParams->isFirstVisitCrossNodeDisplay_ = IsFirstVisitCrossNodeDisplay();
}

void RSScreenRenderNode::UpdatePartialRenderParams()
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::UpdatePartialRenderParams screenParams is null");
        return;
    }
    screenParams->SetAllMainAndLeashSurfaces(curMainAndLeashSurfaceNodes_);
}

bool RSScreenRenderNode::SkipFrame(uint32_t refreshRate, uint32_t skipFrameInterval)
{
    if (refreshRate == 0 || skipFrameInterval <= 1) {
        return false;
    }
    int64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    // when refreshRate % skipFrameInterval != 0 means the skipFrameInterval is the virtual screen refresh rate
    if (refreshRate % skipFrameInterval != 0) {
        int64_t minFrameInterval = 1000000000LL / skipFrameInterval;
        if (minFrameInterval == 0) {
            return false;
        }
        // lastRefreshTime_ is next frame expected refresh time for virtual Display
        if (lastRefreshTime_ <= 0) {
            lastRefreshTime_ = currentTime + minFrameInterval;
            return false;
        }
        if (currentTime < (lastRefreshTime_ - MAX_JITTER_NS)) {
            return true;
        }
        int64_t intervalNums = (currentTime - lastRefreshTime_ + MAX_JITTER_NS) / minFrameInterval;
        lastRefreshTime_ += (intervalNums + 1) * minFrameInterval;
        return false;
    }
    // the skipFrameInterval is equal to 60 divide the virtual screen refresh rate
    int64_t refreshInterval = currentTime - lastRefreshTime_;
    // 1000000000ns == 1s, 110/100 allows 10% over.
    bool needSkip = refreshInterval < (1000000000LL / refreshRate) * (skipFrameInterval - 1) * 110 / 100;
    if (!needSkip) {
        lastRefreshTime_ = currentTime;
    }
    return needSkip;
}

void RSScreenRenderNode::SetDisplayGlobalZOrder(float zOrder)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::SetDisplayGlobalZOrder screenParams is null");
        return;
    }
    screenParams->SetGlobalZOrder(zOrder);
}

void RSScreenRenderNode::UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion)
{
    dirtyManager_->SetBufferAge(bufferage);
    dirtyManager_->UpdateDirty(useAlignedDirtyRegion);
}

void RSScreenRenderNode::ClearCurrentSurfacePos()
{
    lastFrameSurfacePos_ = std::move(currentFrameSurfacePos_);
    lastFrameSurfacesByDescZOrder_ = std::move(currentFrameSurfacesByDescZOrder_);
}

void RSScreenRenderNode::SetMainAndLeashSurfaceDirty(bool isDirty)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetMainAndLeashSurfaceDirty(isDirty);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::SetNeedForceUpdateHwcNodes(bool needForceUpdate, bool hasVisibleHwcNodes)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        return;
    }
    HwcDisplayRecorder().SetNeedForceUpdateHwcNodes(needForceUpdate);
    HwcDisplayRecorder().SetHasVisibleHwcNodes(hasVisibleHwcNodes);
    screenParams->SetNeedForceUpdateHwcNodes(needForceUpdate);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::SetFingerprint(bool hasFingerprint)
{
#ifdef RS_ENABLE_GPU
    if (hasFingerprint_ == hasFingerprint) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetFingerprint(hasFingerprint);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    hasFingerprint_ = hasFingerprint;
#endif
}

void RSScreenRenderNode::SetFixVirtualBuffer10Bit(bool isFixVirtualBuffer10Bit)
{
    if (isFixVirtualBuffer10Bit_ == isFixVirtualBuffer10Bit) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetFixVirtualBuffer10Bit(isFixVirtualBuffer10Bit);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    isFixVirtualBuffer10Bit_ = isFixVirtualBuffer10Bit;
}

bool RSScreenRenderNode::GetFixVirtualBuffer10Bit() const
{
    return isFixVirtualBuffer10Bit_;
}

HdrStatus RSScreenRenderNode::GetDisplayHdrStatus() const
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return HdrStatus::NO_HDR;
    }
    HdrStatus currentHDRStatus = screenParams->GetScreenHDRStatus();
    lastDisplayTotalHdrStatus_ = currentHDRStatus;
    return currentHDRStatus;
}

void RSScreenRenderNode::CollectHdrStatus(HdrStatus hdrStatus)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    HdrStatus currentHDRStatus = screenParams->GetScreenHDRStatus();
    HdrStatus newHDRStatus = static_cast<HdrStatus>(currentHDRStatus | hdrStatus);
    screenParams->CollectHdrStatus(newHDRStatus);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::ResetDisplayHdrStatus()
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->ResetDisplayHdrStatus();
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::SetExistHWCNode(bool existHWCNode)
{
    if (existHWCNode_ == existHWCNode) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetExistHWCNode(existHWCNode);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    existHWCNode_ = existHWCNode;
}

bool RSScreenRenderNode::GetExistHWCNode() const
{
    return existHWCNode_;
}

void RSScreenRenderNode::SetHDRPresent(bool hdrPresent)
{
#ifdef RS_ENABLE_GPU
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetHDRStatusChanged(screenParams->GetHDRPresent() != hdrPresent);
    screenParams->SetHDRPresent(hdrPresent);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSScreenRenderNode::SetBrightnessRatio(float brightnessRatio)
{
#ifdef RS_ENABLE_GPU
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetBrightnessRatio(brightnessRatio);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSScreenRenderNode::SetColorSpace(const GraphicColorGamut& colorSpace)
{
#ifdef RS_ENABLE_GPU
    if (colorSpace_ == colorSpace) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s screenParams is nullptr", __func__);
        return;
    }
    screenParams->SetNewColorSpace(colorSpace);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    colorSpace_ = colorSpace;
#endif
}

void RSScreenRenderNode::UpdateColorSpace(const GraphicColorGamut& colorSpace)
{
    GraphicColorGamut newColorSpace = RSColorSpaceUtil::SelectBigGamut(colorSpace_, colorSpace);
    if (colorSpace_ == newColorSpace) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("%{public}s displayParams is nullptr", __func__);
        return;
    }
    screenParams->SetNewColorSpace(newColorSpace);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }

    colorSpace_ = newColorSpace;
}

void RSScreenRenderNode::SelectBestGamut(const std::vector<ScreenColorGamut>& mode)
{
    if (mode.empty()) {
        SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
        return;
    };
    std::unordered_set<GraphicColorGamut> supported;
    for (auto gamut : mode) {
        if (gamut == COLOR_GAMUT_DISPLAY_BT2020) {
            supported.insert(GRAPHIC_COLOR_GAMUT_BT2020);
        } else {
            supported.insert(static_cast<GraphicColorGamut>(gamut));
        }
    }

    if (supported.count(colorSpace_)) {
        return;
    }

    const std::array<GraphicColorGamut, 4> priority = {
        GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
        GRAPHIC_COLOR_GAMUT_BT2020,
        GRAPHIC_COLOR_GAMUT_SRGB,
        static_cast<GraphicColorGamut>(mode[0])
    }; // 4 is total gamut to determine

    for (auto& gamut : priority) {
        if (supported.count(gamut)) {
            SetColorSpace(gamut);
            return;
        }
    }
}

void RSScreenRenderNode::SetForceCloseHdr(bool isForceCloseHdr)
{
    isForceCloseHdr_ = isForceCloseHdr;
}

// ScreeNode disable HDR only when all children displayNode disable HDR
bool RSScreenRenderNode::GetForceCloseHdr() const
{
    auto childList = GetChildrenList();
    if (childList.empty()) {
        return false;
    }
    // Todo: This condition will be removed after EDR is adapted
    if (static_cast<HdrStatus>(GetDisplayHdrStatus() & HdrStatus::HDR_EFFECT) == HdrStatus::HDR_EFFECT) {
        return false;
    }
    for (const auto& child : childList) {
        auto childPtr = child.lock();
        if (!childPtr) {
            RS_LOGE("RSScreenRenderNode::GetForceCloseHdr child is null");
            return false;
        }
        auto displayNode = childPtr->ReinterpretCastTo<RSLogicalDisplayRenderNode>();
        if (!displayNode) {
            RS_LOGE("RSScreenRenderNode::GetForceCloseHdr child is not displayNode");
            return false;
        }
        bool disableHDR = ROSEN_EQ(displayNode->GetRenderProperties().GetHDRBrightnessFactor(), 0.0f);
        if (!disableHDR) {
            return false;
        }
    }
    return true;
}

Occlusion::Region RSScreenRenderNode::GetDisappearedSurfaceRegionBelowCurrent(NodeId currentSurface) const
{
    Occlusion::Region result;
    auto it = std::find_if(lastFrameSurfacesByDescZOrder_.begin(), lastFrameSurfacesByDescZOrder_.end(),
        [currentSurface](const std::pair<NodeId, RectI>& surface) { return surface.first == currentSurface; });
    if (it == lastFrameSurfacesByDescZOrder_.end()) {
        return result;
    }

    for (++it; it != lastFrameSurfacesByDescZOrder_.end(); ++it) {
        if (currentFrameSurfacePos_.count(it->first) != 0) {
            break;
        }
        Occlusion::Region disappearedSurface{ Occlusion::Rect{ it->second } };
        result.OrSelf(disappearedSurface);
    }
    return result;
}

void RSScreenRenderNode::SetTargetSurfaceRenderNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::SetTargetSurfaceRenderNodeDrawable screenParams is null");
        return;
    }
    screenParams->SetTargetSurfaceRenderNodeDrawable(drawable);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::SetHasMirrorScreen(bool hasMirrorScreen)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::SetHasMirrorScreen screenParams is null");
        return;
    }
    screenParams->SetHasMirrorScreen(hasMirrorScreen);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSScreenRenderNode::SetForceFreeze(bool forceFreeze)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::SetForceFreeze screenParams is null");
        return;
    }
    forceFreeze_ = forceFreeze;
    screenParams->SetForceFreeze(forceFreeze);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

bool RSScreenRenderNode::GetForceFreeze() const
{
    return forceFreeze_ && RSSystemProperties::GetSupportScreenFreezeEnabled();
}

void RSScreenRenderNode::CheckSurfaceChanged()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!screenProperty_.IsVirtual()) {
        return;
    }
    auto& [lastHasSurface, lastSurfaceId] = virtualSurfaceState_;
    auto curSurface = screenProperty_.GetProducerSurface();
    bool curHasSurface = (curSurface != nullptr);
    if (lastHasSurface != curHasSurface || (curHasSurface && lastSurfaceId != curSurface->GetUniqueId())) {
        lastHasSurface = curHasSurface;
        lastSurfaceId = curSurface ? curSurface->GetUniqueId() : UINT64_MAX;
        isVirtualSurfaceChanged_ = true;
        return;
    }

    isVirtualSurfaceChanged_ = false;
#endif
}

void RSScreenRenderNode::UpdateHeadroomMapIncrease(HdrStatus status, uint32_t level)
{
    headroomCounts_[status][level]++;
}

void RSScreenRenderNode::UpdateHeadroomMapDecrease(HdrStatus status, uint32_t level)
{
    auto& map = headroomCounts_[status];
    if (map.count(level) && map[level] > 0U) {
        map[level]--;
    }
}

void RSScreenRenderNode::ResetVideoHeadroomInfo()
{
    headroomCounts_.erase(HdrStatus::HDR_VIDEO);
    headroomCounts_.erase(HdrStatus::AI_HDR_VIDEO_GTM);
    headroomCounts_.erase(HdrStatus::AI_HDR_VIDEO_GAINMAP);
}

void RSScreenRenderNode::SetCloneNodeMap(
    const std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr>& cloneNodeMap)
{
    auto screenParams = static_cast<RSScreenRenderParams*>(stagingRenderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("RSScreenRenderNode::SetCloneNodeMap screenParams is null");
        return;
    }
    screenParams->SetCloneNodeMap(cloneNodeMap);
}
} // namespace Rosen
} // namespace OHOS
