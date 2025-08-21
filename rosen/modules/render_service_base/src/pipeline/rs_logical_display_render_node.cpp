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

#include "rs_trace.h"

#include "pipeline/rs_logical_display_render_node.h"
#include "params/rs_logical_display_render_params.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS::Rosen {
RSLogicalDisplayRenderNode::RSLogicalDisplayRenderNode(NodeId id,
    const RSDisplayNodeConfig& config, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode), screenId_(config.screenId) {}

RSLogicalDisplayRenderNode::~RSLogicalDisplayRenderNode()
{
    RS_LOGI("%{public}s, NodeId:[%{public}" PRIu64 "]", __func__, GetId());
}

void RSLogicalDisplayRenderNode::InitRenderParams()
{
    stagingRenderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNode::InitRenderParams failed");
        return;
    }
}

void RSLogicalDisplayRenderNode::OnSync()
{
    RSRenderNode::OnSync();
}

void RSLogicalDisplayRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareLogicalDisplayRenderNode(*this);
}

void RSLogicalDisplayRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareLogicalDisplayRenderNode(*this);
}

void RSLogicalDisplayRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessLogicalDisplayRenderNode(*this);
}

void RSLogicalDisplayRenderNode::UpdateRenderParams()
{
    auto logicalDisplayRenderParam = static_cast<RSLogicalDisplayRenderParams*>(stagingRenderParams_.get());
    if (logicalDisplayRenderParam == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNode::UpdateRenderParams logicalDisplayRenderParam is null");
        return;
    }
    logicalDisplayRenderParam->screenId_ = screenId_;
    logicalDisplayRenderParam->SetTopSurfaceOpaqueRects(std::move(topSurfaceOpaqueRects_));
    logicalDisplayRenderParam->screenRotation_ = GetScreenRotation();
    logicalDisplayRenderParam->nodeRotation_ = GetRotation();
    logicalDisplayRenderParam->isMirrorDisplay_ = IsMirrorDisplay();
    auto mirroredNode = GetMirrorSource().lock();
    if (mirroredNode) {
        logicalDisplayRenderParam->mirrorSourceDrawable_ = mirroredNode->GetRenderDrawable();
        logicalDisplayRenderParam->virtualScreenMuteStatus_ = virtualScreenMuteStatus_;
    } else {
        logicalDisplayRenderParam->mirrorSourceDrawable_.reset();
    }
    logicalDisplayRenderParam->isSecurityDisplay_ = GetSecurityDisplay();
    logicalDisplayRenderParam->specialLayerManager_ = specialLayerManager_;
    logicalDisplayRenderParam->displaySpecialSurfaceChanged_ = displaySpecialSurfaceChanged_;
    logicalDisplayRenderParam->isSecurityExemption_ = isSecurityExemption_;
    logicalDisplayRenderParam->hasSecLayerInVisibleRect_ = hasSecLayerInVisibleRect_;
    logicalDisplayRenderParam->compositeType_ = compositeType_;
    logicalDisplayRenderParam->hasSecLayerInVisibleRectChanged_ = hasSecLayerInVisibleRectChanged_;
    logicalDisplayRenderParam->hasCaptureWindow_ = hasCaptureWindow_;
    auto screenNode = GetParent().lock();
    auto screenDrawable = screenNode ? screenNode->GetRenderDrawable() : nullptr;
    logicalDisplayRenderParam->SetAncestorScreenDrawable(screenDrawable);
    auto& boundGeo = GetRenderProperties().GetBoundsGeometry();
    if (boundGeo) {
        logicalDisplayRenderParam->offsetX_ = boundGeo->GetX();
        logicalDisplayRenderParam->offsetY_ = boundGeo->GetY();
    }
    RSRenderNode::UpdateRenderParams();
}

Occlusion::Region RSLogicalDisplayRenderNode::GetTopSurfaceOpaqueRegion() const
{
    Occlusion::Region topSurfaceOpaqueRegion;
    for (const auto& rect : topSurfaceOpaqueRects_) {
        topSurfaceOpaqueRegion.OrSelf(rect);
    }
    return topSurfaceOpaqueRegion;
}

RSRenderNode::ChildrenListSharedPtr RSLogicalDisplayRenderNode::GetSortedChildren() const
{
    int32_t currentScbPid = GetCurrentScbPid();
    ChildrenListSharedPtr fullChildrenList = RSRenderNode::GetSortedChildren();
    if (currentScbPid < 0) {
        return fullChildrenList;
    }
    if (isNeedWaitNewScbPid_) {
        for (auto it = (*fullChildrenList).rbegin(); it != (*fullChildrenList).rend(); ++it) {
            auto& child = *it;
            auto childPid = ExtractPid(child->GetId());
            if (childPid == currentScbPid) {
                RS_LOGI("child scb pid equals current scb pid and set valid false");
                isNeedWaitNewScbPid_ = false;
                break;
            }
        }
    }
    if (isNeedWaitNewScbPid_ && lastScbPid_ < 0) {
        return fullChildrenList;
    }
    std::vector<int32_t> oldScbPids = GetOldScbPids();
    currentChildrenList_->clear();
    for (auto& child : *fullChildrenList) {
        if (child == nullptr) {
            continue;
        }
        auto childPid = ExtractPid(child->GetId());
        auto pidIter = std::find(oldScbPids.begin(), oldScbPids.end(), childPid);
        // only when isNeedWaitNewScbPid_ is ture, put lastScb's child to currentChildrenList_
        if (pidIter != oldScbPids.end() && (!isNeedWaitNewScbPid_ || childPid != lastScbPid_)) {
            child->SetIsOntheTreeOnlyFlag(false);
            continue;
        } else if (childPid == currentScbPid) {
            child->SetIsOntheTreeOnlyFlag(true);
        }
        currentChildrenList_->emplace_back(child);
    }
    return std::atomic_load_explicit(&currentChildrenList_, std::memory_order_acquire);
}

void RSLogicalDisplayRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId, NodeId screenNodeId, NodeId logicalDisplayNodeId)
{
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSRenderNode::SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId, uifirstRootNodeId,
        screenNodeId, GetId());
}

void RSLogicalDisplayRenderNode::SetWindowContainer(std::shared_ptr<RSBaseRenderNode> container)
{
    if (auto oldContainer = std::exchange(windowContainer_, container)) {
        if (container) {
            RS_LOGD("RSLogicalDisplayRenderNode::SetWindowContainer oldContainer: %{public}" PRIu64
                ", newContainer: %{public}" PRIu64, oldContainer->GetId(), container->GetId());
        } else {
            RS_LOGD("RSLogicalDisplayRenderNode::SetWindowContainer oldContainer: %{public}" PRIu64,
                oldContainer->GetId());
        }
    }
}

std::shared_ptr<RSBaseRenderNode> RSLogicalDisplayRenderNode::GetWindowContainer() const
{
    return windowContainer_;
}

void RSLogicalDisplayRenderNode::SetScreenStatusNotifyTask(ScreenStatusNotifyTask task)
{
    screenStatusNotifyTask_ = task;
}

void RSLogicalDisplayRenderNode::NotifyScreenNotSwitching()
{
    if (screenStatusNotifyTask_) {
        screenStatusNotifyTask_(false);
        ROSEN_LOGI("RSLogicalDisplayRenderNode::NotifyScreenNotSwitching SetScreenSwitchStatus false");
        RS_TRACE_NAME_FMT("NotifyScreenNotSwitching");
    }
}

ScreenRotation RSLogicalDisplayRenderNode::GetRotation() const
{
    auto& boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return ScreenRotation::ROTATION_0;
    }
    // -90.0f: convert rotation degree to 4 enum values
    return static_cast<ScreenRotation>(static_cast<int32_t>(std::roundf(boundsGeoPtr->GetRotation() / -90.0f)) % 4);
}

bool RSLogicalDisplayRenderNode::IsRotationChanged() const
{
    auto& boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return false;
    }
    // boundsGeoPtr->IsNeedClientCompose() return false if rotation degree is times of 90
    // which means rotation is end.
    bool isRotationEnd = !boundsGeoPtr->IsNeedClientCompose();
    return !(ROSEN_EQ(boundsGeoPtr->GetRotation(), lastRotation_) && isRotationEnd);
}

void RSLogicalDisplayRenderNode::UpdateRotation()
{
#ifdef RS_ENABLE_GPU
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("%{public}s displayParams is nullptr", __func__);
        return;
    }
    AddToPendingSyncList();

    auto& boundsGeoPtr = (GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return;
    }
    lastRotationChanged_ = IsRotationChanged();
    lastRotation_ = boundsGeoPtr->GetRotation();
    preRotationStatus_ = curRotationStatus_;
    curRotationStatus_ = IsRotationChanged();
    displayParams->SetRotationChanged(curRotationStatus_);
#endif
}

bool RSLogicalDisplayRenderNode::IsLastRotationChanged() const
{
    return lastRotationChanged_;
}

bool RSLogicalDisplayRenderNode::GetPreRotationStatus() const
{
    return preRotationStatus_;
}

bool RSLogicalDisplayRenderNode::GetCurRotationStatus() const
{
    return curRotationStatus_;
}

void RSLogicalDisplayRenderNode::UpdateOffscreenRenderParams(bool needOffscreen)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(stagingRenderParams_.get());
    if (displayParams == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNode::UpdateOffscreenRenderParams displayParams is null");
        return;
    }
    displayParams->SetNeedOffscreen(needOffscreen);
}

void RSLogicalDisplayRenderNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

bool RSLogicalDisplayRenderNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSLogicalDisplayRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirrorDisplay_ || node == nullptr) {
        return;
    }
    mirrorSource_ = node;
}

void RSLogicalDisplayRenderNode::ResetMirrorSource()
{
    mirrorSource_.reset();
}

RSLogicalDisplayRenderNode::WeakPtr RSLogicalDisplayRenderNode::GetMirrorSource() const
{
    return mirrorSource_;
}

void RSLogicalDisplayRenderNode::SetIsMirrorDisplay(bool isMirror)
{
    if (isMirrorDisplay_ != isMirror) {
        isMirrorDisplayChanged_ = true;
    }
    isMirrorDisplay_ = isMirror;
    RS_TRACE_NAME_FMT("RSLogicalDisplayRenderNode::SetIsMirrorDisplay, node id:[%" PRIu64 "], isMirrorDisplay: [%d]",
        GetId(), IsMirrorDisplay());
    RS_LOGI("RSLogicalDisplayRenderNode::SetIsMirrorDisplay, node id:[%{public}" PRIu64
        "], isMirrorDisplay: [%{public}d]", GetId(), IsMirrorDisplay());
}

bool RSLogicalDisplayRenderNode::IsMirrorDisplay() const
{
    return isMirrorDisplay_;
}

bool RSLogicalDisplayRenderNode::IsMirrorDisplayChanged() const
{
    return isMirrorDisplayChanged_;
}

void RSLogicalDisplayRenderNode::ResetMirrorDisplayChangedFlag()
{
    isMirrorDisplayChanged_ = false;
}

void RSLogicalDisplayRenderNode::SetVirtualScreenMuteStatus(bool virtualScreenMuteStatus)
{
    virtualScreenMuteStatus_ = virtualScreenMuteStatus;
}

bool RSLogicalDisplayRenderNode::GetVirtualScreenMuteStatus() const
{
    return virtualScreenMuteStatus_;
}

void RSLogicalDisplayRenderNode::SetDisplaySpecialSurfaceChanged(bool displaySpecialSurfaceChanged)
{
    displaySpecialSurfaceChanged_ = displaySpecialSurfaceChanged;
}

RSSpecialLayerManager& RSLogicalDisplayRenderNode::GetMultableSpecialLayerMgr()
{
    return specialLayerManager_;
}

const RSSpecialLayerManager& RSLogicalDisplayRenderNode::GetSpecialLayerMgr() const
{
    return specialLayerManager_;
}

void RSLogicalDisplayRenderNode::AddSecurityLayer(NodeId id)
{
    securityLayerList_.emplace_back(id);
}

void RSLogicalDisplayRenderNode::ClearSecurityLayerList()
{
    securityLayerList_.clear();
}

const std::vector<NodeId>& RSLogicalDisplayRenderNode::GetSecurityLayerList()
{
    return securityLayerList_;
}

void RSLogicalDisplayRenderNode::AddSecurityVisibleLayer(NodeId id)
{
    securityVisibleLayerList_.emplace_back(id);
}

void RSLogicalDisplayRenderNode::ClearSecurityVisibleLayerList()
{
    securityVisibleLayerList_.clear();
}

const std::vector<NodeId>& RSLogicalDisplayRenderNode::GetSecurityVisibleLayerList()
{
    return securityVisibleLayerList_;
}

void RSLogicalDisplayRenderNode::SetSecurityExemption(bool isSecurityExemption)
{
    isSecurityExemption_ = isSecurityExemption;
}

bool RSLogicalDisplayRenderNode::GetSecurityExemption() const
{
    return isSecurityExemption_;
}

void RSLogicalDisplayRenderNode::SetHasSecLayerInVisibleRect(bool hasSecLayer)
{
    bool lastHasSecLayerInVisibleRect = hasSecLayerInVisibleRect_;
    hasSecLayerInVisibleRect_ = hasSecLayer;
    hasSecLayerInVisibleRectChanged_ =
        lastHasSecLayerInVisibleRect != hasSecLayerInVisibleRect_;
}

CompositeType RSLogicalDisplayRenderNode::GetCompositeType() const
{
    return compositeType_;
}

void RSLogicalDisplayRenderNode::SetCompositeType(CompositeType type)
{
    compositeType_ = type;
}

} // namespace OHOS::Rosen
