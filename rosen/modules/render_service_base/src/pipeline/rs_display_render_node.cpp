/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_display_render_node.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "screen_manager/screen_types.h"
#include "visitor/rs_node_visitor.h"
#include "transaction/rs_render_service_client.h"
namespace OHOS {
namespace Rosen {
RSDisplayRenderNode::RSDisplayRenderNode(
    NodeId id, const RSDisplayNodeConfig& config, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(id, context), RSSurfaceHandler(id), screenId_(config.screenId), offsetX_(0), offsetY_(0),
      isMirroredDisplay_(config.isMirrored), dirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{
    RS_LOGI("RSDisplayRenderNode ctor id:%{public}" PRIu64 "", id);
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    syncDirtyManager_ = RSSystemProperties::GetRenderParallelEnabled() ?
        std::make_shared<RSDirtyRegionManager>(true) : dirtyManager_;
}

RSDisplayRenderNode::~RSDisplayRenderNode()
{
    RS_LOGI("RSDisplayRenderNode dtor id:%{public}" PRIu64 "", GetId());
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

void RSDisplayRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender,
    bool onlyFirstLevel)
{
    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
    }
}

void RSDisplayRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareDisplayRenderNode(*this);
}

void RSDisplayRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareDisplayRenderNode(*this);
}

void RSDisplayRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessDisplayRenderNode(*this);
}

void RSDisplayRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId)
{
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSRenderNode::SetIsOnTheTree(flag, GetId(), firstLevelNodeId, cacheNodeId);
}

RSDisplayRenderNode::CompositeType RSDisplayRenderNode::GetCompositeType() const
{
    return compositeType_;
}

void RSDisplayRenderNode::SetCompositeType(RSDisplayRenderNode::CompositeType type)
{
    compositeType_ = type;
}

void RSDisplayRenderNode::SetForceSoftComposite(bool flag)
{
    forceSoftComposite_ = flag;
}

bool RSDisplayRenderNode::IsForceSoftComposite() const
{
    return forceSoftComposite_;
}

void RSDisplayRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirroredDisplay_ || node == nullptr) {
        return;
    }
    mirrorSource_ = node;
}

void RSDisplayRenderNode::ResetMirrorSource()
{
    mirrorSource_.reset();
}

bool RSDisplayRenderNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

void RSDisplayRenderNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

bool RSDisplayRenderNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSDisplayRenderNode::SetIsMirrorDisplay(bool isMirror)
{
    isMirroredDisplay_ = isMirror;
    RS_LOGD("RSDisplayRenderNode::SetIsMirrorDisplay, node id:[%{public}" PRIu64 "], isMirrorDisplay: [%{public}s]",
        GetId(), IsMirrorDisplay() ? "true" : "false");
}

void RSDisplayRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:[%{public}" PRIu64 ", isBootAnimation:%{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;

    auto parent = GetParent().lock();
    if (parent == nullptr) {
        return;
    }
    if (isBootAnimation) {
        parent->SetContainBootAnimation(true);
    }
}

bool RSDisplayRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

void RSDisplayRenderNode::InitRenderParams()
{
    stagingRenderParams_ = std::make_unique<RSDisplayRenderParams>(GetId());
    renderParams_ = std::make_unique<RSDisplayRenderParams>(GetId());
    uifirstRenderParams_ = std::make_unique<RSDisplayRenderParams>(GetId());
}

void RSDisplayRenderNode::OnSync()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSDisplayRenderNode::OnSync global dirty[%s]",
        dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("RSDisplayRenderNode::OnSync displayParams is null");
        return;
    }
    dirtyManager_->OnSync(syncDirtyManager_);
    displayParams->SetNeedSync(true);
    RSRenderNode::OnSync();
    curMainAndLeashSurfaceNodes_.clear();
}

void RSDisplayRenderNode::RecordMainAndLeashSurfaces(RSBaseRenderNode::SharedPtr surface)
{
    curMainAndLeashSurfaceNodes_.push_back(surface);
}

void RSDisplayRenderNode::UpdateRenderParams()
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("RSDisplayRenderNode::UpdateRenderParams displayParams is null");
        return;
    }
    displayParams->offsetX_ = GetDisplayOffsetX();
    displayParams->offsetY_ = GetDisplayOffsetY();
    displayParams->nodeRotation_ = GetRotation();
    displayParams->mirrorSource_ = GetMirrorSource();

    RSRenderNode::UpdateRenderParams();
}

void RSDisplayRenderNode::UpdateScreenRenderParams(ScreenInfo& screenInfo,
    std::map<ScreenId, bool>& displayHasSecSurface, std::map<ScreenId, bool>& displayHasSkipSurface,
    std::map<ScreenId, bool>& hasCaptureWindow)
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("RSDisplayRenderNode::UpdateScreenRenderParams displayParams is null");
        return;
    }
    displayParams->screenId_ = GetScreenId();
    displayParams->screenRotation_ = GetScreenRotation();
    displayParams->compositeType_ = GetCompositeType();
    displayParams->screenInfo_ = std::move(screenInfo);
    displayParams->displayHasSecSurface_ = std::move(displayHasSecSurface);
    displayParams->displayHasSkipSurface_ = std::move(displayHasSkipSurface);
    displayParams->hasCaptureWindow_ = std::move(hasCaptureWindow);
}

void RSDisplayRenderNode::UpdatePartialRenderParams()
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("RSDisplayRenderNode::UpdatePartialRenderParams displayParams is null");
        return;
    }
    displayParams->SetAllMainAndLeashSurfaces(curMainAndLeashSurfaceNodes_);
}

#ifndef ROSEN_CROSS_PLATFORM
bool RSDisplayRenderNode::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    if (consumer_ != nullptr && surface_ != nullptr) {
        RS_LOGI("RSDisplayRenderNode::CreateSurface already created, return");
        return true;
    }
    consumer_ = IConsumerSurface::Create("DisplayNode");
    if (consumer_ == nullptr) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    surface->SetQueueSize(4); // 4 Buffer rotation
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    RS_LOGI("RSDisplayRenderNode::CreateSurface end");
    surfaceCreated_ = true;
    return true;
}
#endif

bool RSDisplayRenderNode::SkipFrame(uint32_t skipFrameInterval)
{
    frameCount_++;
    // ensure skipFrameInterval is not 0
    if (skipFrameInterval == 0) {
        return false;
    }
    if ((frameCount_ - 1) % skipFrameInterval == 0) {
        return false;
    }
    return true;
}

ScreenRotation RSDisplayRenderNode::GetRotation() const
{
    auto boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return ScreenRotation::ROTATION_0;
    }
    // -90.0f: convert rotation degree to 4 enum values
    return static_cast<ScreenRotation>(static_cast<int32_t>(std::roundf(boundsGeoPtr->GetRotation() / -90.0f)) % 4);
}

bool RSDisplayRenderNode::IsRotationChanged() const
{
    auto boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return false;
    }
    // boundsGeoPtr->IsNeedClientCompose() return false if rotation degree is times of 90
    // which means rotation is end.
    bool isRotationEnd = !boundsGeoPtr->IsNeedClientCompose();
    return !(ROSEN_EQ(boundsGeoPtr->GetRotation(), lastRotation_) && isRotationEnd);
}

void RSDisplayRenderNode::UpdateRotation()
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    displayParams->SetRotationChanged(IsRotationChanged());
    AddToPendingSyncList();

    auto boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return;
    }
    lastRotation_ = boundsGeoPtr->GetRotation();
}

void RSDisplayRenderNode::UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion, bool renderParallel)
{
    auto dirtyManager = renderParallel ? syncDirtyManager_ : dirtyManager_;
    dirtyManager->SetBufferAge(bufferage);
    dirtyManager->UpdateDirty(useAlignedDirtyRegion);
}

void RSDisplayRenderNode::ClearCurrentSurfacePos()
{
    lastFrameSurfacePos_.clear();
    lastFrameSurfacePos_.swap(currentFrameSurfacePos_);
}

void RSDisplayRenderNode::SetMainAndLeashSurfaceDirty(bool isDirty)
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(stagingRenderParams_.get());
    displayParams->SetMainAndLeashSurfaceDirty(isDirty);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

} // namespace Rosen
} // namespace OHOS
