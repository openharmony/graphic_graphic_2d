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

#include "pipeline/rs_surface_render_node.h"

#include "command/rs_command_verify_helper.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"
#include "display_engine/rs_color_temperature.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/rs_rt_refresh_callback.h"
#include "monitor/self_drawing_node_monitor.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties_painter.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "metadata_helper.h"
#include <v1_0/cm_color_space.h>
#endif
namespace OHOS {
namespace Rosen {

// set the offset value to prevent the situation where the float number
// with the suffix 0.000x is still rounded up.
constexpr float RECT_CEIL_DEVIATION = 0.001;

namespace {
bool CheckRootNodeReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child->IsInstanceOf<RSRootRenderNode>()) {
        auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
        // when rootnode is occluded, its applymodifiers will be skipped
        // need to applymodifiers to update its properties here
        rootNode->ApplyModifiers();
        const auto& property = rootNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0 && rootNode->GetEnableRender()) {
            return true;
        }
    }
    return false;
}

bool CheckScbReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child->IsInstanceOf<RSCanvasRenderNode>()) {
        auto canvasRenderNode = child->ReinterpretCastTo<RSCanvasRenderNode>();
        const auto& property = canvasRenderNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0) {
            return true;
        }
    }
    return false;
}

bool IsFirstFrameReadyToDraw(RSSurfaceRenderNode& node)
{
    auto sortedChildren = node.GetSortedChildren();
    if (node.IsScbScreen() || node.IsSCBNode()) {
        for (const auto& child : *sortedChildren) {
            if (CheckScbReadyToDraw(child)) {
                return true;
            }
        }
    }
    for (auto& child : *sortedChildren) {
        if (CheckRootNodeReadyToDraw(child)) {
            return true;
        }
        // when appWindow has abilityComponent node
        if (child->IsInstanceOf<RSSurfaceRenderNode>()) {
            for (const auto& surfaceNodeChild : *child->GetSortedChildren()) {
                if (CheckRootNodeReadyToDraw(surfaceNodeChild)) {
                    return true;
                }
            }
        }
    }
    return false;
}
}

RSSurfaceRenderNode::RSSurfaceRenderNode(
    const RSSurfaceRenderNodeConfig& config, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(config.id, context, config.isTextureExportNode),
      nodeType_(config.nodeType), surfaceWindowType_(config.surfaceWindowType),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>()),
      cacheSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>()),
      surfaceHandler_(std::make_shared<RSSurfaceHandler>(config.id)), name_(config.name),
      bundleName_(config.bundleName)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(config.id), config.id, 0,
        MEMORY_TYPE::MEM_RENDER_NODE, ExtractPid(config.id)};
    MemoryTrack::Instance().AddNodeRecord(config.id, info);
#endif
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(config.id), sizeof(*this));
    RsCommandVerifyHelper::GetInstance().AddSurfaceNodeCreateCnt(ExtractPid(config.id));
}

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSSurfaceRenderNode(RSSurfaceRenderNodeConfig { .id = id, .name = "SurfaceNode",
    .isTextureExportNode = isTextureExportNode}, context)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode()
{
#ifdef USE_SURFACE_TEXTURE
    SetSurfaceTexture(nullptr);
#endif
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
    RsCommandVerifyHelper::GetInstance().SubSurfaceNodeCreateCnt(ExtractPid(GetId()));
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetConsumer(const sptr<IConsumerSurface>& consumer)
{
    GetMutableRSSurfaceHandler()->SetConsumer(consumer);
}
#endif

void RSSurfaceRenderNode::UpdateSrcRect(const Drawing::Canvas& canvas, const Drawing::RectI& dstRect,
    bool hasRotation)
{
    // if surfaceNode bound change, send message to aps
#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
    SendSurfaceNodeBoundChange();
#endif
    auto localClipRect = RSPaintFilterCanvas::GetLocalClipBounds(canvas, &dstRect).value_or(Drawing::Rect());
    const RSProperties& properties = GetRenderProperties();
    int left = std::clamp<int>(localClipRect.GetLeft(), 0, properties.GetBoundsWidth());
    int top = std::clamp<int>(localClipRect.GetTop(), 0, properties.GetBoundsHeight());
    int width = std::clamp<int>(std::ceil(localClipRect.GetWidth() - RECT_CEIL_DEVIATION), 0,
        std::ceil(properties.GetBoundsWidth() - left));
    int height = std::clamp<int>(std::ceil(localClipRect.GetHeight() - RECT_CEIL_DEVIATION), 0,
        std::ceil(properties.GetBoundsHeight() - top));
    RectI srcRect = {left, top, width, height};
    SetSrcRect(srcRect);
    // We allow 1px error value to avoid disable dss by mistake [this flag only used for YUV buffer format]
    if (IsYUVBufferFormat()) {
        isHardwareForcedDisabledBySrcRect_ = !GetAncoForceDoDirect() &&
            (hasRotation ? (width + 1 < static_cast<int>(properties.GetBoundsWidth())) :
            (height + 1 < static_cast<int>(properties.GetBoundsHeight())));
#ifndef ROSEN_CROSS_PLATFORM
        RS_OPTIONAL_TRACE_NAME_FMT("UpdateSrcRect hwcDisableBySrc:%d localClip:[%.2f, %.2f, %.2f, %.2f]" \
            " bounds:[%.2f, %.2f] hasRotation:%d name:%s id:%llu",
            isHardwareForcedDisabledBySrcRect_,
            localClipRect.GetLeft(), localClipRect.GetTop(), localClipRect.GetWidth(), localClipRect.GetHeight(),
            properties.GetBoundsWidth(), properties.GetBoundsHeight(), hasRotation,
            GetName().c_str(), GetId());
#endif
    }
}

bool RSSurfaceRenderNode::IsYUVBufferFormat() const
{
#ifndef ROSEN_CROSS_PLATFORM
    auto curBuffer = surfaceHandler_->GetBuffer();
    if (!curBuffer) {
        return false;
    }
    auto format = curBuffer->GetFormat();
    if (format < GRAPHIC_PIXEL_FMT_YUV_422_I || format == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
        format > GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

void RSSurfaceRenderNode::UpdateInfoForClonedNode(NodeId nodeId)
{
    bool isClonedNode = GetId() == nodeId;
    if (isClonedNode && IsMainWindowType() && clonedSourceNodeNeedOffscreen_) {
        SetNeedCacheSurface(true);
        SetHwcChildrenDisabledState();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " children disabled by isCloneNode",
            GetName().c_str(), GetId());
    }
    SetIsCloned(isClonedNode);
}

bool RSSurfaceRenderNode::ShouldPrepareSubnodes()
{
    // if a appwindow or abilitycomponent has a empty dstrect, its subnodes' prepare stage can be skipped
    // most common scenario: HiBoard (SwipeLeft screen on home screen)
    if (GetDstRect().IsEmpty() && (IsAppWindow() || IsAbilityComponent())) {
        return false;
    }
    return true;
}

std::string RSSurfaceRenderNode::DirtyRegionDump() const
{
    std::string dump = GetName() +
        " SurfaceNodeType [" + std::to_string(static_cast<unsigned int>(GetSurfaceNodeType())) + "]" +
        " Transparent [" + std::to_string(IsTransparent()) +"]" +
        " DstRect: " + GetDstRect().ToString() +
        " VisibleRegion: " + GetVisibleRegion().GetRegionInfo();
    if (GetDirtyManager()) {
        dump += " DirtyRegion: " + GetDirtyManager()->GetDirtyRegion().ToString();
    }
    return dump;
}

void RSSurfaceRenderNode::ResetRenderParams()
{
    stagingRenderParams_.reset();
    renderDrawable_->renderParams_.reset();
}

void RSSurfaceRenderNode::SetStencilVal(int64_t stencilVal)
{
    stencilVal_ = stencilVal;
}

void RSSurfaceRenderNode::PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    // Save the current state of the canvas before modifying it.
    renderNodeSaveCount_ = canvas.SaveAllStatus();

    // Apply alpha to canvas
    const RSProperties& properties = GetRenderProperties();
    canvas.MultiplyAlpha(properties.GetAlpha());

    // Apply matrix to canvas
    auto& currentGeoPtr = (properties.GetBoundsGeometry());
    if (currentGeoPtr != nullptr) {
        currentGeoPtr->UpdateByMatrixFromSelf();
        auto matrix = currentGeoPtr->GetMatrix();
        matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
        matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
        canvas.ConcatMatrix(matrix);
    }

    // Clip by bounds
    canvas.ClipRect(Drawing::Rect(0, 0, std::floor(properties.GetBoundsWidth()),
        std::floor(properties.GetBoundsHeight())), Drawing::ClipOp::INTERSECT, false);

    // Extract srcDest and dstRect from Drawing::Canvas, localCLipBounds as SrcRect, deviceClipBounds as DstRect
    auto deviceClipRect = canvas.GetDeviceClipBounds();
    UpdateSrcRect(canvas, deviceClipRect);
    RectI dstRect = { deviceClipRect.GetLeft() + offsetX_, deviceClipRect.GetTop() + offsetY_,
        deviceClipRect.GetWidth(), deviceClipRect.GetHeight() };
    SetDstRect(dstRect);

    // Save TotalMatrix and GlobalAlpha for compositor
    SetTotalMatrix(canvas.GetTotalMatrix());
    SetGlobalAlpha(canvas.GetAlpha());
}

void RSSurfaceRenderNode::PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreStatus(renderNodeSaveCount_);
}

void RSSurfaceRenderNode::CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
    std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender, bool onlyFirstLevel)
{
    if (IsScbScreen()) {
        for (auto& child : *node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
        }
        return;
    }
    if (IsStartingWindow()) {
        if (isUniRender) {
            vec.emplace_back(shared_from_this());
        }
        return;
    }
    if (IsLeashWindow()) {
        if (isUniRender) {
            vec.emplace_back(shared_from_this());
        }
        if (onlyFirstLevel) {
            return;
        }
        for (auto& child : *node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
        }
        return;
    }

#ifndef ROSEN_CROSS_PLATFORM
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && consumer->GetTunnelHandle() != nullptr) {
        return;
    }
#endif
    auto num = find(vec.begin(), vec.end(), shared_from_this());
    if (num != vec.end()) {
        return;
    }
    if (isUniRender && ShouldPaint()) {
        vec.emplace_back(shared_from_this());
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        if (surfaceHandler_->GetBuffer() != nullptr && ShouldPaint()) {
            vec.emplace_back(shared_from_this());
        }
#endif
    }
}

void RSSurfaceRenderNode::CollectSelfDrawingChild(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<NodeId>& vec)
{
    if (IsSelfDrawingType()) {
        vec.push_back(node->GetId());
    }
    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSelfDrawingChild(child, vec);
    }
}

    void RSSurfaceRenderNode::ClearChildrenCache()
{
    for (auto& child : *GetChildren()) {
        auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            continue;
        }
#ifndef ROSEN_CROSS_PLATFORM
        auto consumer = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
        if (consumer != nullptr) {
            consumer->GoBackground();
        }
#endif
    }
    // Temporary solution, GetChildren will generate fullChildrenList_, which will cause memory leak
    OnTreeStateChanged();
}

void RSSurfaceRenderNode::FindScreenId()
{
    // The results found across screen windows are inaccurate
    auto nodeTemp = GetParent().lock();
    screenId_ = -1;
    while (nodeTemp != nullptr) {
        if (nodeTemp->GetId() == 0) {
            break;
        }
        if (nodeTemp->GetType() == RSRenderNodeType::DISPLAY_NODE) {
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(nodeTemp);
            screenId_ = displayNode->GetScreenId();
            break;
        }
        nodeTemp = nodeTemp->GetParent().lock();
    }
}

void RSSurfaceRenderNode::OnTreeStateChanged()
{
    NotifyTreeStateChange();
    RSRenderNode::OnTreeStateChanged();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!IsOnTheTree()) {
        if (auto context = GetContext().lock()) {
            RS_TRACE_NAME_FMT("need purgeUnlockedResources this SurfaceNode isn't on the tree Id:%" PRIu64 " Name:%s",
                GetId(), GetName().c_str());
            RS_LOGD("need purgeUnlockedResources this SurfaceNode isn't on the tree Id:%" PRIu64 " Name:%s",
                GetId(), GetName().c_str());
            if (IsLeashWindow()) {
                context->MarkNeedPurge(ClearMemoryMoment::COMMON_SURFACE_NODE_HIDE, RSContext::PurgeType::GENTLY);
            }
            if (surfaceWindowType_ == SurfaceWindowType::SYSTEM_SCB_WINDOW) {
                context->MarkNeedPurge(ClearMemoryMoment::SCENEBOARD_SURFACE_NODE_HIDE, RSContext::PurgeType::STRONGLY);
            }
        }
    } else if (GetSurfaceNodeType() == RSSurfaceNodeType::CURSOR_NODE) {
        FindScreenId();
    }
#endif
    if (IsAbilityComponent()) {
        if (auto instanceRootNode = GetInstanceRootNode()) {
            if (auto surfaceNode = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                surfaceNode->UpdateAbilityNodeIds(GetId(), IsOnTheTree());
            }
        }
    } else if (IsHardwareEnabledType() && RSUniRenderJudgement::IsUniRender()) {
        if (auto instanceRootNode = GetInstanceRootNode()) {
            if (auto surfaceNode = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                surfaceNode->UpdateChildHardwareEnabledNode(GetId(), IsOnTheTree());
            }
        }
    }
    OnSubSurfaceChanged();

    // sync skip & security info
    UpdateSpecialLayerInfoByOnTreeStateChange();
    SyncPrivacyContentInfoToFirstLevelNode();
    SyncColorGamutInfoToFirstLevelNode();
}

bool RSSurfaceRenderNode::HasSubSurfaceNodes() const
{
    return childSubSurfaceNodes_.size() != 0;
}

void RSSurfaceRenderNode::SetIsSubSurfaceNode(bool isSubSurfaceNode)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams) {
        surfaceParams->SetIsSubSurfaceNode(isSubSurfaceNode);
        isSubSurfaceNode_ = isSubSurfaceNode;
        AddToPendingSyncList();
    }
#endif
}

bool RSSurfaceRenderNode::IsSubSurfaceNode() const
{
    return isSubSurfaceNode_;
}

const std::map<NodeId, RSSurfaceRenderNode::WeakPtr>& RSSurfaceRenderNode::GetChildSubSurfaceNodes() const
{
    return childSubSurfaceNodes_;
}

void RSSurfaceRenderNode::OnSubSurfaceChanged()
{
    if (!IsMainWindowType() || !RSUniRenderJudgement::IsUniRender()) {
        return;
    }
    auto parentNode = GetParent().lock();
    if (!parentNode) {
        return;
    }
    std::shared_ptr<RSSurfaceRenderNode> parentSurfaceNode = parentNode->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (!parentSurfaceNode || !parentSurfaceNode->IsMainWindowType()) {
        if (auto instanceRootNode = parentNode->GetInstanceRootNode()) {
            parentSurfaceNode = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>();
        }
    }
    if (parentSurfaceNode && parentSurfaceNode->IsLeashOrMainWindow()) {
        parentSurfaceNode->UpdateChildSubSurfaceNodes(ReinterpretCastTo<RSSurfaceRenderNode>(), IsOnTheTree());
    }
}

void RSSurfaceRenderNode::UpdateChildSubSurfaceNodes(RSSurfaceRenderNode::SharedPtr node, bool isOnTheTree)
{
    if (isOnTheTree) {
        childSubSurfaceNodes_[node->GetId()] = node;
    } else {
        childSubSurfaceNodes_.erase(node->GetId());
    }
    if (!IsLeashWindow()) {
        node->SetIsSubSurfaceNode(isOnTheTree);
    }
}

std::unordered_set<NodeId> RSSurfaceRenderNode::GetAllSubSurfaceNodeIds() const
{
    std::unordered_set<NodeId> allSubSurfaceNodeIds;
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>> allSubSurfaceNodes;
    GetAllSubSurfaceNodes(allSubSurfaceNodes);
    for (auto& [id, _] : allSubSurfaceNodes) {
        allSubSurfaceNodeIds.insert(id);
    }
    return allSubSurfaceNodeIds;
}

void RSSurfaceRenderNode::GetAllSubSurfaceNodes(
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>>& allSubSurfaceNodes) const
{
    for (auto& [id, node] : childSubSurfaceNodes_) {
        auto subSubSurfaceNodePtr = node.lock();
        if (!subSubSurfaceNodePtr) {
            continue;
        }
        if (subSubSurfaceNodePtr->HasSubSurfaceNodes()) {
            subSubSurfaceNodePtr->GetAllSubSurfaceNodes(allSubSurfaceNodes);
        }
        allSubSurfaceNodes.push_back({id, node});
    }
}

std::string RSSurfaceRenderNode::SubSurfaceNodesDump() const
{
    std::string out;
    out += ", subSurface";
    for (auto [id, _] : childSubSurfaceNodes_) {
        out += "[" + std::to_string(id) + "]";
    }
    return out;
}

void RSSurfaceRenderNode::OnResetParent()
{
    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        ClearChildrenCache();
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        auto consumer = GetRSSurfaceHandler()->GetConsumer();
        if (consumer != nullptr && !IsSelfDrawingType() && !IsAbilityComponent()) {
            consumer->GoBackground();
        }
#endif
    }
}

void RSSurfaceRenderNode::SetIsNotifyUIBufferAvailable(bool available)
{
#ifdef USE_SURFACE_TEXTURE
    auto texture = GetSurfaceTexture();
    if (texture) {
        texture->MarkUiFrameAvailable(available);
    }
#endif
    isNotifyUIBufferAvailable_.store(available);
}

void RSSurfaceRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareSurfaceRenderNode(*this);
}

bool RSSurfaceRenderNode::IsUIBufferAvailable()
{
    return ((IsAppWindow() || IsScbScreen() || IsUIExtension())
        && !IsNotifyUIBufferAvailable() && IsFirstFrameReadyToDraw(*this));
}

bool RSSurfaceRenderNode::IsSubTreeNeedPrepare(bool filterInGlobal, bool isOccluded)
{
    // force preparation case for occlusion
    if (IsLeashWindow()) {
        SetSubTreeDirty(false);
        UpdateChildrenOutOfRectFlag(false); // collect again
        return true;
    }

    // force preparation case for update gravity when appWindow geoDirty
    auto parentPtr = this->GetParent().lock();
    auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
    if (surfaceParentPtr != nullptr &&
        surfaceParentPtr->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        if (this->GetRenderProperties().IsCurGeoDirty()) {
            SetSubTreeDirty(false);
            UpdateChildrenOutOfRectFlag(false);
            return true;
        }
    }

    return RSRenderNode::IsSubTreeNeedPrepare(filterInGlobal, isOccluded);
}

void RSSurfaceRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    needDrawAnimateProperty_ = true;
    ProcessAnimatePropertyBeforeChildren(canvas, true);
    needDrawAnimateProperty_ = false;
}

void RSSurfaceRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty)
{
    if (GetCacheType() != CacheType::ANIMATE_PROPERTY && !needDrawAnimateProperty_) {
        return;
    }

    const auto& property = GetRenderProperties();
    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, canvas, &absClipRRect);
    RSPropertiesPainter::DrawOutline(property, canvas);

    if (!property.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }

#ifndef ROSEN_CROSS_PLATFORM
    RSPropertiesPainter::DrawBackground(
        property, canvas, true, IsSelfDrawingNode() && (surfaceHandler_->GetBuffer() != nullptr));
#else
    RSPropertiesPainter::DrawBackground(property, canvas);
#endif
    RSPropertiesPainter::DrawMask(property, canvas);
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::BACKGROUND_FILTER);
    SetTotalMatrix(canvas.GetTotalMatrix());
}

void RSSurfaceRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    needDrawAnimateProperty_ = true;
    ProcessAnimatePropertyAfterChildren(canvas);
    needDrawAnimateProperty_ = false;
}

void RSSurfaceRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (GetCacheType() != CacheType::ANIMATE_PROPERTY && !needDrawAnimateProperty_) {
        return;
    }
    const auto& property = GetRenderProperties();
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::FOREGROUND_FILTER);
    canvas.Save();
    if (GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        auto& geoPtr = (property.GetBoundsGeometry());
        canvas.ConcatMatrix(geoPtr->GetMatrix());
    }
    RSPropertiesPainter::DrawOutline(property, canvas);
    RSPropertiesPainter::DrawBorder(property, canvas);
    canvas.Restore();
}

void RSSurfaceRenderNode::SetContextBounds(const Vector4f bounds)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetBounds>(GetId(), bounds);
    SendCommandFromRT(command, GetId());
}

const std::shared_ptr<RSDirtyRegionManager>& RSSurfaceRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetCacheSurfaceDirtyManager() const
{
    return cacheSurfaceDirtyManager_;
}

void RSSurfaceRenderNode::SetSurfaceNodeType(RSSurfaceNodeType nodeType)
{
    if (nodeType_ == RSSurfaceNodeType::ABILITY_COMPONENT_NODE ||
        nodeType_ == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE ||
        nodeType_ == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE ||
        nodeType_ == RSSurfaceNodeType::CURSOR_NODE) {
        return;
    }
    if (nodeType == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE ||
        nodeType == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE) {
        RS_LOGE("RSSurfaceRenderNode::SetSurfaceNodeType prohibition of converting surfaceNodeType to uiExtension");
        return;
    }
    nodeType_ = nodeType;
}

void RSSurfaceRenderNode::MarkUIHidden(bool isHidden)
{
    isUIHidden_ = isHidden;
}

bool RSSurfaceRenderNode::IsUIHidden() const
{
    return isUIHidden_;
}

bool RSSurfaceRenderNode::IsLeashWindowSurfaceNodeVisible()
{
    if (!IsLeashWindow()) {
        return false;
    }
    auto nestedSurfaces = GetLeashWindowNestedSurfaces();
    return std::any_of(nestedSurfaces.begin(), nestedSurfaces.end(),
        [](const auto& node) -> bool {
            return node && !node->IsUIHidden();
        });
}

void RSSurfaceRenderNode::SetContextMatrix(const std::optional<Drawing::Matrix>& matrix, bool sendMsg)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    SetContentDirty();
    AddDirtyType(RSModifierType::SCALE);
    AddDirtyType(RSModifierType::SKEW);
    AddDirtyType(RSModifierType::SCALE_Z);
    AddDirtyType(RSModifierType::PERSP);
    AddDirtyType(RSModifierType::TRANSLATE);
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(GetId(), matrix);
    SendCommandFromRT(command, GetId());
}

void RSSurfaceRenderNode::SetContextAlpha(float alpha, bool sendMsg)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    SetContentDirty();
    AddDirtyType(RSModifierType::ALPHA);
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), alpha);
    SendCommandFromRT(command, GetId());
}

void RSSurfaceRenderNode::SetContextClipRegion(const std::optional<Drawing::Rect>& clipRegion, bool sendMsg)
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    SetContentDirty();
    AddDirtyType(RSModifierType::BOUNDS);
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command, GetId());
}
void RSSurfaceRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:%{public}" PRIu64 ", isBootAnimation:%{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;
}

bool RSSurfaceRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

void RSSurfaceRenderNode::SetGlobalPositionEnabled(bool isEnabled)
{
#ifdef RS_ENABLE_GPU
    if (isGlobalPositionEnabled_ == isEnabled) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        ROSEN_LOGE("RSSurfaceRenderNode::SetGlobalPositionEnabled failed! surfaceParams is null. id:%{public}"
            "" PRIu64 ", isEnabled:%{public}d", GetId(), isEnabled);
        return;
    }
    surfaceParams->SetGlobalPositionEnabled(isEnabled);
    AddToPendingSyncList();

    isGlobalPositionEnabled_ = isEnabled;
    ROSEN_LOGI("RSSurfaceRenderNode::SetGlobalPositionEnabled id:%{public}" PRIu64 ", isEnabled:%{public}d",
        GetId(), isEnabled);
#endif
}

bool RSSurfaceRenderNode::GetGlobalPositionEnabled() const
{
    return isGlobalPositionEnabled_;
}

void RSSurfaceRenderNode::SetHwcGlobalPositionEnabled(bool isEnabled)
{
    if (isHwcGlobalPositionEnabled_ == isEnabled) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        return;
    }
    surfaceParams->SetHwcGlobalPositionEnabled(isEnabled);
    AddToPendingSyncList();

    isHwcGlobalPositionEnabled_ = isEnabled;
}

bool RSSurfaceRenderNode::GetHwcGlobalPositionEnabled() const
{
    return isHwcGlobalPositionEnabled_;
}

void RSSurfaceRenderNode::SetHwcCrossNode(bool isDRMCrossNode)
{
    if (isHwcCrossNode_ == isDRMCrossNode) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        return;
    }
    surfaceParams->SetHwcCrossNode(isDRMCrossNode);
    AddToPendingSyncList();

    isHwcCrossNode_ = isDRMCrossNode;
}

bool RSSurfaceRenderNode::IsDRMCrossNode() const
{
    return isHwcCrossNode_;
}

void RSSurfaceRenderNode::SetForceHardwareAndFixRotation(bool flag)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        return;
    }
    surfaceParams->SetFixRotationByUser(flag);
    AddToPendingSyncList();

    isFixRotationByUser_ = flag;
#endif
}

bool RSSurfaceRenderNode::GetFixRotationByUser() const
{
    return isFixRotationByUser_;
}

bool RSSurfaceRenderNode::IsInFixedRotation() const
{
    return isInFixedRotation_;
}

void RSSurfaceRenderNode::SetInFixedRotation(bool isRotating)
{
    if (isFixRotationByUser_ && !isInFixedRotation_ && isRotating) {
#ifndef ROSEN_CROSS_PLATFORM
#ifdef RS_ENABLE_GPU
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
        if (surfaceParams) {
            auto layer = surfaceParams->GetLayerInfo();
            originalSrcRect_ = { layer.srcRect.x, layer.srcRect.y, layer.srcRect.w, layer.srcRect.h };
            originalDstRect_ = { layer.dstRect.x, layer.dstRect.y, layer.dstRect.w, layer.dstRect.h };
        }
#endif
#endif
    }
    isInFixedRotation_ = isFixRotationByUser_ && isRotating;
}

void RSSurfaceRenderNode::SetHidePrivacyContent(bool needHidePrivacyContent)
{
    if (needHidePrivacyContent_ == needHidePrivacyContent) {
        return;
    }
    needHidePrivacyContent_ = needHidePrivacyContent;
    SetDirty();
    if (needHidePrivacyContent) {
        privacyContentLayerIds_.insert(GetId());
    } else {
        privacyContentLayerIds_.erase(GetId());
    }
    ROSEN_LOGI("RSSurfaceRenderNode::SetHidePrivacyContent, Node id: %{public}" PRIu64 ", privacyContent:%{public}d",
        GetId(), needHidePrivacyContent);
    SyncPrivacyContentInfoToFirstLevelNode();
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    if (specialLayerManager_.Find(SpecialLayerType::SECURITY) == isSecurityLayer) {
        return;
    }
    specialLayerChanged_ = specialLayerManager_.Set(SpecialLayerType::SECURITY, isSecurityLayer);
    SetDirty();
    if (isSecurityLayer) {
        specialLayerManager_.AddIds(SpecialLayerType::SECURITY, GetId());
    } else {
        specialLayerManager_.RemoveIds(SpecialLayerType::SECURITY, GetId());
    }
    ROSEN_LOGI("RSSurfaceRenderNode::SetSecurityLayer, Node id: %{public}" PRIu64 ", SecurityLayer:%{public}d",
        GetId(), isSecurityLayer);
    UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, isSecurityLayer);
}

void RSSurfaceRenderNode::SetLeashPersistentId(NodeId leashPersistentId)
{
    if (leashPersistentId_ == leashPersistentId) {
        return;
    }

    leashPersistentId_ = leashPersistentId;
    SetDirty();

    ROSEN_LOGD("RSSurfaceRenderNode::SetLeashPersistentId, Node id: %{public}" PRIu64 ", leashPersistentId:%{public}"
        "" PRIu64, GetId(), leashPersistentId);
}

void RSSurfaceRenderNode::SetSkipLayer(bool isSkipLayer)
{
    if (specialLayerManager_.Find(SpecialLayerType::SKIP) == isSkipLayer) {
        return;
    }
    specialLayerChanged_ = specialLayerManager_.Set(SpecialLayerType::SKIP, isSkipLayer);
    SetDirty();
    if (isSkipLayer) {
        specialLayerManager_.AddIds(SpecialLayerType::SKIP, GetId());
    } else {
        specialLayerManager_.RemoveIds(SpecialLayerType::SKIP, GetId());
    }
    UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SKIP, isSkipLayer);
}

LeashPersistentId RSSurfaceRenderNode::GetLeashPersistentId() const
{
    return leashPersistentId_;
}

void RSSurfaceRenderNode::SetSnapshotSkipLayer(bool isSnapshotSkipLayer)
{
    if (specialLayerManager_.Find(SpecialLayerType::SNAPSHOT_SKIP) == isSnapshotSkipLayer) {
        return;
    }
    specialLayerChanged_ = specialLayerManager_.Set(SpecialLayerType::SNAPSHOT_SKIP, isSnapshotSkipLayer);
    SetDirty();
    if (isSnapshotSkipLayer) {
        specialLayerManager_.AddIds(SpecialLayerType::SNAPSHOT_SKIP, GetId());
    } else {
        specialLayerManager_.RemoveIds(SpecialLayerType::SNAPSHOT_SKIP, GetId());
    }
    UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SNAPSHOT_SKIP, isSnapshotSkipLayer);
}

void RSSurfaceRenderNode::SetProtectedLayer(bool isProtectedLayer)
{
    if (specialLayerManager_.Find(SpecialLayerType::PROTECTED) == isProtectedLayer) {
        return;
    }
    specialLayerChanged_ = specialLayerManager_.Set(SpecialLayerType::PROTECTED, isProtectedLayer);
    SetDirty();
    if (isProtectedLayer) {
        specialLayerManager_.AddIds(SpecialLayerType::PROTECTED, GetId());
    } else {
        specialLayerManager_.RemoveIds(SpecialLayerType::PROTECTED, GetId());
    }
    UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::PROTECTED, isProtectedLayer);
}

void RSSurfaceRenderNode::SetIsOutOfScreen(bool isOutOfScreen)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetIsOutOfScreen(isOutOfScreen);
        AddToPendingSyncList();
    }
}

bool RSSurfaceRenderNode::GetHasPrivacyContentLayer() const
{
    return privacyContentLayerIds_.size() != 0;
}

void RSSurfaceRenderNode::UpdateSpecialLayerInfoByTypeChange(uint32_t type, bool isSpecialLayer)
{
    if (!IsOnTheTree() || GetFirstLevelNodeId() == GetId()) {
        return;
    }
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    // firstLevelNode is the nearest app window / leash node
    if (firstLevelNode) {
        if (type == SpecialLayerType::PROTECTED) {
            firstLevelNode->SetDirty();
        }
        if (isSpecialLayer) {
            firstLevelNode->specialLayerManager_.AddIds(type, GetId());
        } else {
            firstLevelNode->specialLayerManager_.RemoveIds(type, GetId());
        }
        firstLevelNode->specialLayerChanged_ = specialLayerChanged_;
    }
}

void RSSurfaceRenderNode::UpdateSpecialLayerInfoByOnTreeStateChange()
{
    if (specialLayerManager_.Find(IS_GENERAL_SPECIAL) == false || GetFirstLevelNodeId() == GetId()) {
        return;
    }
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    // firstLevelNode is the nearest app window / leash node
    if (firstLevelNode) {
        if (specialLayerManager_.Find(SpecialLayerType::PROTECTED)) {
            firstLevelNode->SetDirty();
        }        
        if (IsOnTheTree()) {
            firstLevelNode->specialLayerManager_.AddIds(specialLayerManager_.Get(), GetId());
        } else {
            firstLevelNode->specialLayerManager_.RemoveIds(specialLayerManager_.Get(), GetId());
        }
        firstLevelNode->specialLayerChanged_ = specialLayerChanged_;
    }
}

void RSSurfaceRenderNode::SyncPrivacyContentInfoToFirstLevelNode()
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    // firstLevelNode is the nearest app window / leash node
    if (firstLevelNode && GetFirstLevelNodeId() != GetId()) {
        if (needHidePrivacyContent_ && IsOnTheTree()) {
            firstLevelNode->privacyContentLayerIds_.insert(GetId());
        } else {
            firstLevelNode->privacyContentLayerIds_.erase(GetId());
        }
    }
}

void RSSurfaceRenderNode::SyncColorGamutInfoToFirstLevelNode()
{
    // When the P3 appWindow node is up or down the tree, it transmits color gamut information to leashWindow node.
    if (colorSpace_ != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
        auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
        if (firstLevelNode) {
            firstLevelNode->SetFirstLevelNodeColorGamutByWindow(IsOnTheTree());
        }
    }
}

void RSSurfaceRenderNode::SetFingerprint(bool hasFingerprint)
{
    if (hasFingerprint_ == hasFingerprint) {
        return;
    }
    hasFingerprint_ = hasFingerprint;
    SetDirty();
}

bool RSSurfaceRenderNode::GetFingerprint() const
{
    return hasFingerprint_;
}

bool RSSurfaceRenderNode::IsCloneNode() const
{
    return isCloneNode_;
}

void RSSurfaceRenderNode::SetClonedNodeInfo(NodeId id, bool needOffscreen)
{
    isCloneNode_ = (id != INVALID_NODEID);
    clonedSourceNodeId_ = id;
    auto context = GetContext().lock();
    if (!context) {
        RS_LOGE("RSSurfaceRenderNode::SetClonedNodeInfo invalid context");
        return;
    }
    auto clonedSurfaceNode = context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(clonedSourceNodeId_);
    if (clonedSurfaceNode) {
        clonedSurfaceNode->clonedSourceNodeNeedOffscreen_ = needOffscreen;
    }
    RS_LOGD("RSSurfaceRenderNode::SetClonedNodeInfo clonedNode[%{public}" PRIu64 "] needOffscreen: %{public}d",
        id, needOffscreen);
}

void RSSurfaceRenderNode::SetForceUIFirst(bool forceUIFirst)
{
    if (forceUIFirst) {
        forceUIFirstChanged_ = true;
    }
    forceUIFirst_ = forceUIFirst;
}
bool RSSurfaceRenderNode::GetForceUIFirst() const
{
    return forceUIFirst_;
}

bool RSSurfaceRenderNode::GetForceDrawWithSkipped() const
{
    return uifirstForceDrawWithSkipped_;
}

void RSSurfaceRenderNode::SetForceDrawWithSkipped(bool GetForceDrawWithSkipped)
{
    uifirstForceDrawWithSkipped_ = GetForceDrawWithSkipped;
}

void RSSurfaceRenderNode::SetHDRPresent(bool hasHdrPresent)
{
    auto surfaceParam = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParam) {
        surfaceParam->SetHDRPresent(hasHdrPresent);
        AddToPendingSyncList();
    }
}

bool RSSurfaceRenderNode::GetHDRPresent() const
{
    return hdrNum_ > 0;
}

void RSSurfaceRenderNode::IncreaseHDRNum()
{
    std::lock_guard<std::mutex> lockGuard(mutexHDR_);
    hdrNum_++;
    RS_LOGD("RSSurfaceRenderNode::IncreaseHDRNum HDRClient hdrNum_: %{public}d", hdrNum_);
}

void RSSurfaceRenderNode::ReduceHDRNum()
{
    std::lock_guard<std::mutex> lockGuard(mutexHDR_);
    if (hdrNum_ == 0) {
        ROSEN_LOGE("RSSurfaceRenderNode::ReduceHDRNum error");
        return;
    }
    hdrNum_--;
    RS_LOGD("RSSurfaceRenderNode::ReduceHDRNum HDRClient hdrNum_: %{public}d", hdrNum_);
}

bool RSSurfaceRenderNode::GetIsWideColorGamut() const
{
    return wideColorGamutNum_ > 0;
}

void RSSurfaceRenderNode::IncreaseWideColorGamutNum()
{
#ifndef ROSEN_CROSS_PLATFORM
    // If the count was zero brfore this increment, meaning the first P3 resource appears in the subtree.
    // Notify firstLevelNode to increment the wide color window count.
    if (!GetIsWideColorGamut()) {
        auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
        if (!firstLevelNode) {
            RS_LOGE("RSSurfaceRenderNode::IncreaseWideColorGamutNum firstLevelNode is nullptr");
            wideColorGamutNum_++;
            return;
        }
        RS_LOGD("RSSurfaceRenderNode::IncreaseWideColorGamutNum notify firstLevelNodeId[%{public}" PRIu64
            "] now wideColorGamutNum_[%{public}d]", firstLevelNode->GetId(), wideColorGamutNum_ + 1);
        firstLevelNode->SetFirstLevelNodeColorGamutByResource(true);
    }
    wideColorGamutNum_++;
#endif
}

void RSSurfaceRenderNode::ReduceWideColorGamutNum()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!GetIsWideColorGamut()) {
        ROSEN_LOGE("RSSurfaceRenderNode::ReduceWideColorGamutNum error");
        return;
    }
    // If the count was zero after this decrement, meaning no P3 resources remain in the subtree.
    // Notify firstLevelNode to decrement the wide color window count.
    wideColorGamutNum_--;
    if (!GetIsWideColorGamut()) {
        auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
        if (!firstLevelNode) {
            RS_LOGE("RSSurfaceRenderNode::ReduceWideColorGamutNum firstLevelNode is nullptr");
            return;
        }
        RS_LOGD("RSSurfaceRenderNode::ReduceWideColorGamutNum notify firstLevelNodeId[%{public}" PRIu64
            "] now wideColorGamutNum_[%{public}d]", firstLevelNode->GetId(), wideColorGamutNum_);
        firstLevelNode->SetFirstLevelNodeColorGamutByResource(false);
    }
#endif
}

void RSSurfaceRenderNode::SetForceUIFirstChanged(bool forceUIFirstChanged)
{
    forceUIFirstChanged_ = forceUIFirstChanged;
}
bool RSSurfaceRenderNode::GetForceUIFirstChanged()
{
    return forceUIFirstChanged_;
}

void RSSurfaceRenderNode::SetAncoForceDoDirect(bool direct)
{
    ancoForceDoDirect_.store(direct);
}

bool RSSurfaceRenderNode::GetOriAncoForceDoDirect()
{
    return ancoForceDoDirect_.load();
}

bool RSSurfaceRenderNode::GetAncoForceDoDirect() const
{
    return (ancoForceDoDirect_.load() && (GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)));
}

void RSSurfaceRenderNode::SetAncoFlags(uint32_t flags)
{
    ancoFlags_.store(flags);
}

uint32_t RSSurfaceRenderNode::GetAncoFlags() const
{
    return ancoFlags_.load();
}

void RSSurfaceRenderNode::RegisterTreeStateChangeCallback(TreeStateChangeCallback callback)
{
    treeStateChangeCallback_ = callback;
}

void RSSurfaceRenderNode::NotifyTreeStateChange()
{
    if (treeStateChangeCallback_) {
        treeStateChangeCallback_(*this);
    }
}

void RSSurfaceRenderNode::SetLayerTop(bool isTop)
{
#ifdef RS_ENABLE_GPU
    isLayerTop_ = isTop;
    SetContentDirty();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        return;
    }
    surfaceParams->SetLayerTop(isTop);
    AddToPendingSyncList();
#endif
}

bool RSSurfaceRenderNode::IsHardwareEnabledTopSurface() const
{
    return GetSurfaceNodeType() == RSSurfaceNodeType::CURSOR_NODE && RSSystemProperties::GetHardCursorEnabled();
}

void RSSurfaceRenderNode::SetHardCursorStatus(bool status)
{
#ifdef RS_ENABLE_GPU
    if (isHardCursor_ == status) {
        isLastHardCursor_ = isHardCursor_;
        return;
    }
    RS_LOGI("RSSurfaceRenderNode::SetHardCursorStatus status:%{public}d", status);
    isLastHardCursor_ = isHardCursor_;
    isHardCursor_ = status;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams) {
        surfaceParams->SetHardCursorStatus(status);
        AddToPendingSyncList();
    }
#endif
}

bool RSSurfaceRenderNode::GetHardCursorStatus() const
{
    return isHardCursor_;
}

bool RSSurfaceRenderNode::GetHardCursorLastStatus() const
{
    return isLastHardCursor_;
}

void RSSurfaceRenderNode::SetColorSpace(GraphicColorGamut colorSpace)
{
    if (colorSpace_ == colorSpace) {
        return;
    }
    colorSpace_ = colorSpace;
    if (!isOnTheTree_) {
        return;
    }
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    if (!firstLevelNode) {
        RS_LOGE("RSSurfaceRenderNode::SetColorSpace firstLevelNode is nullptr");
        return;
    }
    firstLevelNode->SetFirstLevelNodeColorGamutByWindow(colorSpace_ != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

GraphicColorGamut RSSurfaceRenderNode::GetColorSpace() const
{
    if (!RSSystemProperties::GetWideColorSpaceEnabled()) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
    if (RsCommonHook::Instance().GetP3NodeCountFlag() && wideColorGamutNum_ > 0) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    }
    return colorSpace_;
}

GraphicColorGamut RSSurfaceRenderNode::GetFirstLevelNodeColorGamut() const
{
    if (!RSSystemProperties::GetWideColorSpaceEnabled()) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
    if (wideColorGamutWindowCount_ > 0 ||
        (RsCommonHook::Instance().GetP3NodeCountFlag() && wideColorGamutResourceWindowCount_ > 0)) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    } else {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
}

void RSSurfaceRenderNode::SetFirstLevelNodeColorGamutByResource(bool changeToP3)
{
    if (changeToP3) {
        wideColorGamutResourceWindowCount_++;
    } else {
        wideColorGamutResourceWindowCount_--;
    }
}

void RSSurfaceRenderNode::SetFirstLevelNodeColorGamutByWindow(bool changeToP3)
{
    if (changeToP3) {
        wideColorGamutWindowCount_++;
    } else {
        wideColorGamutWindowCount_--;
    }
}

void RSSurfaceRenderNode::UpdateColorSpaceWithMetadata()
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!GetRSSurfaceHandler() || !GetRSSurfaceHandler()->GetBuffer()) {
        RS_LOGD("RSSurfaceRenderNode::UpdateColorSpaceWithMetadata node(%{public}s) did not have buffer.",
            GetName().c_str());
        return;
    }
    const sptr<SurfaceBuffer>& buffer = GetRSSurfaceHandler()->GetBuffer();
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
        RS_LOGD("RSSurfaceRenderNode::UpdateColorSpaceWithMetadata get color space info failed.");
        return;
    }
    // currently, P3 is the only supported wide color gamut, this may be modified later.
    SetColorSpace(colorSpaceInfo.primaries != COLORPRIMARIES_SRGB ?
        GRAPHIC_COLOR_GAMUT_DISPLAY_P3 : GRAPHIC_COLOR_GAMUT_SRGB);
#endif
}

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!surfaceHandler_) {
        return;
    }
    auto consumer = surfaceHandler_->GetConsumer();
    if (!consumer) {
        return;
    }
    consumer->SetDefaultWidthAndHeight(width, height);
#else
#ifdef USE_SURFACE_TEXTURE
    auto texture = GetSurfaceTexture();
    if (texture) {
        texture->UpdateSurfaceDefaultSize(width, height);
    }
#endif
#endif
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::UpdateBufferInfo(const sptr<SurfaceBuffer>& buffer, const Rect& damageRect,
    const sptr<SyncFence>& acquireFence, const sptr<SurfaceBuffer>& preBuffer)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (!surfaceParams->IsBufferSynced()) {
        auto curBuffer = surfaceParams->GetBuffer();
        auto consumer = GetRSSurfaceHandler()->GetConsumer();
        if (curBuffer && consumer) {
            auto fence = surfaceParams->GetAcquireFence();
            consumer->ReleaseBuffer(curBuffer, fence);
        }
    } else {
        surfaceParams->SetPreBuffer(preBuffer);
    }

    surfaceParams->SetBuffer(buffer, damageRect);
    surfaceParams->SetAcquireFence(acquireFence);
    surfaceParams->SetBufferSynced(false);
    surfaceParams->SetIsBufferFlushed(true);
    AddToPendingSyncList();
#endif
}

void RSSurfaceRenderNode::NeedClearBufferCache(std::set<uint32_t>& bufferCacheSet)
{
#ifdef RS_ENABLE_GPU
    if (!surfaceHandler_) {
        return;
    }

    if (auto buffer = surfaceHandler_->GetBuffer()) {
        bufferCacheSet.insert(buffer->GetSeqNum());
        RS_OPTIONAL_TRACE_NAME_FMT("NeedClearBufferCache bufferSeqNum:%d", buffer->GetSeqNum());
    }
    if (auto preBuffer = surfaceHandler_->GetPreBuffer()) {
        bufferCacheSet.insert(preBuffer->GetSeqNum());
        RS_OPTIONAL_TRACE_NAME_FMT("NeedClearBufferCache preBufferSeqNum:%d", preBuffer->GetSeqNum());
    }
#endif
}

void RSSurfaceRenderNode::NeedClearPreBuffer(std::set<uint32_t>& bufferCacheSet)
{
#ifdef RS_ENABLE_GPU
    if (!surfaceHandler_) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        return;
    }
    if (auto preBuffer = surfaceHandler_->GetPreBuffer()) {
        bufferCacheSet.insert(preBuffer->GetSeqNum());
        RS_OPTIONAL_TRACE_NAME_FMT("NeedClearPreBuffer preBufferSeqNum:%d", preBuffer->GetSeqNum());
    }
    surfaceParams->SetPreBuffer(nullptr);
    AddToPendingSyncList();
#endif
}

#endif

#ifndef ROSEN_CROSS_PLATFORM
GraphicBlendType RSSurfaceRenderNode::GetBlendType()
{
    return blendType_;
}

void RSSurfaceRenderNode::SetBlendType(GraphicBlendType blendType)
{
    blendType_ = blendType;
}
#endif

void RSSurfaceRenderNode::RegisterBufferAvailableListener(
    sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (isFromRenderThread) {
        std::lock_guard<std::mutex> lock(mutexRT_);
        callbackFromRT_ = callback;
    } else {
        {
            std::lock_guard<std::mutex> lock(mutexUI_);
            callbackFromUI_ = callback;
        }
        isNotifyUIBufferAvailable_ = false;
    }
}

void RSSurfaceRenderNode::RegisterBufferClearListener(
    sptr<RSIBufferClearCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutexClear_);
    clearBufferCallback_ = callback;
}

void RSSurfaceRenderNode::SetNotifyRTBufferAvailable(bool isNotifyRTBufferAvailable)
{
    isNotifyRTBufferAvailable_ = isNotifyRTBufferAvailable;
    if (GetIsTextureExportNode()) {
        SetContentDirty();
    }
    std::lock_guard<std::mutex> lock(mutexClear_);
    if (clearBufferCallback_) {
        clearBufferCallback_->OnBufferClear();
    }
}

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %{public}" PRIu64, GetId());
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->RegisterBufferAvailableListener(
            GetId(), [weakThis = weak_from_this()]() {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->NotifyRTBufferAvailable(node->GetIsTextureExportNode());
            }, true);
        renderServiceClient->RegisterBufferClearListener(
            GetId(), [weakThis = weak_from_this()]() {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->SetNotifyRTBufferAvailable(false);
            });
    }
}

void RSSurfaceRenderNode::NotifyRTBufferAvailable(bool isTextureExportNode)
{
    // In RS, "isNotifyRTBufferAvailable_ = true" means buffer is ready and need to trigger ipc callback.
    // In RT, "isNotifyRTBufferAvailable_ = true" means RT know that RS have had available buffer
    // and ready to trigger "callbackForRenderThreadRefresh_" to "clip" on parent surface.
    if (!isTextureExportNode) {
        isNotifyRTBufferAvailablePre_ = isNotifyRTBufferAvailable_;
        if (isNotifyRTBufferAvailable_) {
            return;
        }
        isNotifyRTBufferAvailable_ = true;
    }

    if (isRefresh_) {
        ROSEN_LOGD("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderThread", GetId());
        RSRTRefreshCallback::Instance().ExecuteRefresh();
    }
    if (isTextureExportNode) {
        SetContentDirty();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderService",
                GetId());
            callbackFromRT_->OnBufferAvailable();
        }
        if (!isRefresh_ && !callbackFromRT_) {
            isNotifyRTBufferAvailable_ = false;
        }
    }
}

void RSSurfaceRenderNode::NotifyUIBufferAvailable()
{
    RS_TRACE_NAME_FMT("RSSurfaceRenderNode::NotifyUIBufferAvailable id:%llu bufferAvailable:%d waitUifirst:%d",
        GetId(), IsNotifyUIBufferAvailable(), IsWaitUifirstFirstFrame());
    if (isNotifyUIBufferAvailable_ || isWaitUifirstFirstFrame_) {
        return;
    }
    isNotifyUIBufferAvailable_ = true;
    {
        std::lock_guard<std::mutex> lock(mutexUI_);
        if (callbackFromUI_) {
            RS_TRACE_NAME_FMT("NotifyUIBufferAvailable done. id:%llu", GetId());
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %{public}" PRIu64, GetId());
            callbackFromUI_->OnBufferAvailable();
#ifdef OHOS_PLATFORM
            if (IsAppWindow()) {
                RSJankStats::GetInstance().SetAppFirstFrame(ExtractPid(GetId()));
            }
#endif
        }
    }
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailable() const
{
#if defined(ROSEN_ANDROID) || defined(ROSEN_IOS)
    return true;
#else
    return isNotifyRTBufferAvailable_;
#endif
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailablePre() const
{
#if defined(ROSEN_ANDROID) || defined(ROSEN_IOS)
    return true;
#else
    return isNotifyRTBufferAvailablePre_;
#endif
}

bool RSSurfaceRenderNode::IsNotifyUIBufferAvailable() const
{
    return isNotifyUIBufferAvailable_;
}

void RSSurfaceRenderNode::SetCallbackForRenderThreadRefresh(bool isRefresh)
{
    isRefresh_ = isRefresh;
}

bool RSSurfaceRenderNode::NeedSetCallbackForRenderThreadRefresh()
{
    return !isRefresh_;
}

bool RSSurfaceRenderNode::IsStartAnimationFinished() const
{
    return startAnimationFinished_;
}

void RSSurfaceRenderNode::SetStartAnimationFinished()
{
    RS_LOGD("RSSurfaceRenderNode::SetStartAnimationFinished");
    startAnimationFinished_ = true;
}

bool RSSurfaceRenderNode::UpdateDirtyIfFrameBufferConsumed()
{
    if (surfaceHandler_ && surfaceHandler_->IsCurrentFrameBufferConsumed()) {
        SetContentDirty();
        return true;
    }
    return false;
}

bool RSSurfaceRenderNode::IsSurfaceInStartingWindowStage() const
{
    auto parentPtr = this->GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
        if (surfaceParentPtr->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE &&
            !this->IsNotifyUIBufferAvailable()) {
            return true;
        }
    }
    return false;
}

bool RSSurfaceRenderNode::IsParentLeashWindowInScale() const
{
    auto parentPtr = this->GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
        if (surfaceParentPtr->IsLeashWindow() && surfaceParentPtr->IsScale()) {
            return true;
        }
    }
    return false;
}

Occlusion::Rect RSSurfaceRenderNode::GetSurfaceOcclusionRect(bool isUniRender)
{
    Occlusion::Rect occlusionRect;
    if (isUniRender) {
        occlusionRect = Occlusion::Rect {GetOldDirtyInSurface()};
    } else {
        occlusionRect = Occlusion::Rect {GetDstRect()};
    }
    return occlusionRect;
}

bool RSSurfaceRenderNode::QueryIfAllHwcChildrenForceDisabledByFilter()
{
    std::shared_ptr<RSSurfaceRenderNode> appWindow;
    for (auto& child : *GetSortedChildren()) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (node && node->IsAppWindow()) {
            appWindow = node;
            break;
        }
    }
    if (appWindow) {
        auto hardwareEnabledNodes = appWindow->GetChildHardwareEnabledNodes();
        for (auto& hardwareEnabledNode : hardwareEnabledNodes) {
            auto hardwareEnabledNodePtr = hardwareEnabledNode.lock();
            if (hardwareEnabledNodePtr && !hardwareEnabledNodePtr->IsHardwareForcedDisabledByFilter()) {
                return false;
            }
        }
    }
    return true;
}

void RSSurfaceRenderNode::AccumulateOcclusionRegion(Occlusion::Region& accumulatedRegion,
    Occlusion::Region& curRegion,
    bool& hasFilterCacheOcclusion,
    bool isUniRender,
    bool filterCacheOcclusionEnabled)
{
    // when surfacenode is in starting window stage, do not occlude other window surfaces
    // fix gray block when directly open app (i.e. setting) from notification center
    if (IsSurfaceInStartingWindowStage()) {
        return;
    }
    if (!isUniRender) {
        bool diff =
#ifndef ROSEN_CROSS_PLATFORM
            (GetDstRect().width_ > surfaceHandler_->GetBuffer()->GetWidth() ||
                GetDstRect().height_ > surfaceHandler_->GetBuffer()->GetHeight()) &&
#endif
            GetRenderProperties().GetFrameGravity() != Gravity::RESIZE && ROSEN_EQ(GetGlobalAlpha(), 1.0f);
        if (!IsTransparent() && !diff) {
            accumulatedRegion.OrSelf(curRegion);
        }
    }

    if (GetName().find("hisearch") != std::string::npos) {
        return;
    }
    SetTreatedAsTransparent(false);
    // when a surfacenode is in animation (i.e. 3d animation), its dstrect cannot be trusted, we treated it as a full
    // transparent layer.
    if ((GetAnimateState() || IsParentLeashWindowInScale()) && !isOcclusionInSpecificScenes_) {
        SetTreatedAsTransparent(true);
        return;
    }

    // full surfacenode valid filter cache can be treated as opaque
    if (filterCacheOcclusionEnabled && IsTransparent() && GetFilterCacheValidForOcclusion()) {
        accumulatedRegion.OrSelf(curRegion);
        hasFilterCacheOcclusion = true;
    } else {
        accumulatedRegion.OrSelf(GetOpaqueRegion());
    }
    return;
}

WINDOW_LAYER_INFO_TYPE RSSurfaceRenderNode::GetVisibleLevelForWMS(RSVisibleLevel visibleLevel)
{
    switch (visibleLevel) {
        case RSVisibleLevel::RS_INVISIBLE:
            return WINDOW_LAYER_INFO_TYPE::INVISIBLE;
        case RSVisibleLevel::RS_ALL_VISIBLE:
            return WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE;
        case RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE:
        case RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE:
            return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
        default:
            break;
    }
    return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
}

bool RSSurfaceRenderNode::IsSCBNode() const
{
    return surfaceWindowType_ != SurfaceWindowType::SYSTEM_SCB_WINDOW;
}

void RSSurfaceRenderNode::UpdateHwcNodeLayerInfo(GraphicTransformType transform, bool isHardCursorEnable)
{
#ifdef RS_ENABLE_GPU
#ifndef ROSEN_CROSS_PLATFORM
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    auto layer = surfaceParams->GetLayerInfo();
    layer.srcRect = {srcRect_.left_, srcRect_.top_, srcRect_.width_, srcRect_.height_};
    layer.dstRect = {dstRect_.left_, dstRect_.top_, dstRect_.width_, dstRect_.height_};
    const auto& properties = GetRenderProperties();
    layer.boundRect = {0, 0,
        static_cast<uint32_t>(properties.GetBoundsWidth()),
        static_cast<uint32_t>(properties.GetBoundsHeight())};
    layer.transformType = transform;
    layer.zOrder = surfaceHandler_->GetGlobalZOrder();
    layer.gravity = static_cast<int32_t>(properties.GetFrameGravity());
    layer.blendType = GetBlendType();
    layer.matrix = totalMatrix_;
    layer.alpha = GetGlobalAlpha();
    layer.arsrTag = GetArsrTag();
    layer.copybitTag = GetCopybitTag();
    if (isHardCursorEnable) {
        layer.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR;
    } else {
        layer.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    }
    isHardwareForcedDisabled_ = specialLayerManager_.Find(SpecialLayerType::PROTECTED) ?
        false : isHardwareForcedDisabled_;
#ifndef ROSEN_CROSS_PLATFORM
    auto buffer = surfaceHandler_->GetBuffer();
    RS_LOGD("RSSurfaceRenderNode::UpdateHwcNodeLayerInfo: name:%{public}s id:%{public}" PRIu64 ", bufferFormat:%d,"
        " src:%{public}s, dst:%{public}s, bounds:[%{public}d, %{public}d] buffer:[%{public}d, %{public}d]"
        " transform:%{public}d, zOrder:%{public}d, cur:%{public}d, last:%{public}d",
        GetName().c_str(), GetId(), buffer ? buffer->GetFormat() : -1,
        srcRect_.ToString().c_str(),
        dstRect_.ToString().c_str(),
        layer.boundRect.w, layer.boundRect.h,
        buffer ? buffer->GetSurfaceBufferWidth() : 0, buffer ? buffer->GetSurfaceBufferHeight() : 0,
        transform, layer.zOrder, !IsHardwareForcedDisabled(), isLastFrameHwcEnabled_);
    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug:UpdateHwcNodeLayerInfo: name:%s id:%lu, bufferFormat:%d,"
        " src:%s, dst:%s, bounds:[%d, %d], buffer:[%d, %d], transform:%d, zOrder:%d, cur:%d, last:%d",
        GetName().c_str(), GetId(), buffer ? buffer->GetFormat() : -1,
        srcRect_.ToString().c_str(),
        dstRect_.ToString().c_str(),
        layer.boundRect.w, layer.boundRect.h,
        buffer ? buffer->GetSurfaceBufferWidth() : 0, buffer ? buffer->GetSurfaceBufferHeight() : 0,
        transform, layer.zOrder, !IsHardwareForcedDisabled(), isLastFrameHwcEnabled_);
#endif
    surfaceParams->SetLayerInfo(layer);
    surfaceParams->SetHardwareEnabled(!IsHardwareForcedDisabled());
    surfaceParams->SetNeedMakeImage(IsHardwareNeedMakeImage());
    surfaceParams->SetLastFrameHardwareEnabled(isLastFrameHwcEnabled_);
    surfaceParams->SetInFixedRotation(isInFixedRotation_);
    surfaceParams->SetAbsRotation(GetAbsRotation());
    // 1 means need source tuning
    if (RsCommonHook::Instance().GetVideoSurfaceFlag() && IsYUVBufferFormat()) {
        surfaceParams->SetLayerSourceTuning(1);
    }
    // set tuning for anco node
    if (GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) {
        surfaceParams->SetLayerSourceTuning(1);
    }
    AddToPendingSyncList();
#endif
#endif
}

void RSSurfaceRenderNode::SetPreSubHighPriorityType(bool priorityType)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetPreSubHighPriorityType surfaceParams is Null");
        return;
    }
    surfaceParams->SetPreSubHighPriorityType(priorityType);
    AddToPendingSyncList();
#endif
}

void RSSurfaceRenderNode::UpdateHardwareDisabledState(bool disabled)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetLastFrameHardwareEnabled(!IsHardwareForcedDisabled());
    SetHardwareForcedDisabledState(disabled);
    surfaceParams->SetHardwareEnabled(!IsHardwareForcedDisabled());
    AddToPendingSyncList();
#endif
}

void RSSurfaceRenderNode::SetVisibleRegionRecursive(const Occlusion::Region& region,
                                                    VisibleData& visibleVec,
                                                    std::map<NodeId, RSVisibleLevel>& visMapForVsyncRate,
                                                    bool needSetVisibleRegion,
                                                    RSVisibleLevel visibleLevel,
                                                    bool isSystemAnimatedScenes)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE || IsAbilityComponent()) {
        SetOcclusionVisible(true);
        visibleVec.emplace_back(std::make_pair(GetId(), ALL_VISIBLE));
        return;
    }

    bool vis = !region.IsEmpty();
    if (vis) {
        visibleVec.emplace_back(std::make_pair(GetId(), GetVisibleLevelForWMS(visibleLevel)));
    }

    // collect visible changed pid
    if (qosPidCal_ && GetType() == RSRenderNodeType::SURFACE_NODE && !isSystemAnimatedScenes) {
        visMapForVsyncRate[GetId()] = !IsSCBNode() ? RSVisibleLevel::RS_ALL_VISIBLE : visibleLevel;
    }

    visibleRegionForCallBack_ = region;
    if (needSetVisibleRegion) {
        visibleRegion_ = region;
        SetOcclusionVisible(vis);
    }
    // when there is filter cache occlusion, also save occlusion status without filter cache
    SetOcclusionVisibleWithoutFilter(vis);

    for (auto& child : *GetChildren()) {
        if (auto surfaceChild = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)) {
            surfaceChild->SetVisibleRegionRecursive(region, visibleVec, visMapForVsyncRate, needSetVisibleRegion,
                visibleLevel, isSystemAnimatedScenes);
        }
    }
}

void RSSurfaceRenderNode::ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow, const Vector4<int>& cornerRadius)
{
    Occlusion::Region absRegion { absRect };
    Occlusion::Region oldOpaqueRegion { opaqueRegion_ };

    // The transparent region of surfaceNode should include shadow area
    Occlusion::Rect dirtyRect { GetOldDirty() };
    transparentRegion_ = Occlusion::Region{ dirtyRect };

    if (IsTransparent() && !NeedDrawBehindWindow()) {
        RS_OPTIONAL_TRACE_NAME_FMT("CalcOpaqueRegion [%s] transparent: OcclusionBg:[%d], alpha:[%f], IsEmpty:[%d]",
            GetName().c_str(), static_cast<int>(GetAbilityBgAlpha()), GetGlobalAlpha(), IsEmptyAppWindow());
        opaqueRegion_ = Occlusion::Region();
    } else {
        auto maxRadius = std::max({ cornerRadius.x_, cornerRadius.y_, cornerRadius.z_, cornerRadius.w_ });
        if (IsAppWindow() && HasContainerWindow()) {
            containerConfig_.outR_ = maxRadius;
            RS_OPTIONAL_TRACE_NAME_FMT("CalcOpaqueRegion [%s] has container: inR:[%d], outR:[%d], innerRect:[%s], "
                "isFocus:[%d]", GetName().c_str(), containerConfig_.inR_, containerConfig_.outR_,
                containerConfig_.innerRect_.ToString().c_str(), isFocusWindow);
            opaqueRegion_ = ResetOpaqueRegion(GetAbsDrawRect(), screenRotation, isFocusWindow);
            opaqueRegion_.AndSelf(absRegion);
        } else {
            if (!cornerRadius.IsZero()) {
                Vector4<int> dstCornerRadius((cornerRadius.x_ > 0 ? maxRadius : 0),
                                             (cornerRadius.y_ > 0 ? maxRadius : 0),
                                             (cornerRadius.z_ > 0 ? maxRadius : 0),
                                             (cornerRadius.w_ > 0 ? maxRadius : 0));
                RS_OPTIONAL_TRACE_NAME_FMT("CalcOpaqueRegion [%s] with cornerRadius: [%d, %d, %d, %d]",
                    GetName().c_str(), dstCornerRadius.x_, dstCornerRadius.y_, dstCornerRadius.z_, dstCornerRadius.w_);
                SetCornerRadiusOpaqueRegion(absRect, dstCornerRadius);
            } else {
                opaqueRegion_ = absRegion;
                roundedCornerRegion_ = Occlusion::Region();
            }
        }
        DealWithDrawBehindWindowTransparentRegion();
        transparentRegion_.SubSelf(opaqueRegion_);
    }
    Occlusion::Rect screen{screeninfo};
    Occlusion::Region screenRegion{screen};
    transparentRegion_.AndSelf(screenRegion);
    opaqueRegion_.AndSelf(screenRegion);
    opaqueRegionChanged_ = !oldOpaqueRegion.Xor(opaqueRegion_).IsEmpty();
    ResetSurfaceContainerRegion(screeninfo, absRect, screenRotation);
}

void RSSurfaceRenderNode::DealWithDrawBehindWindowTransparentRegion()
{
    const auto& absDrawBehindWindowRegion = GetFilterRect();
    RS_OPTIONAL_TRACE_NAME_FMT("CalcOpaqueRegion [%s] needDrawBehindWindow: [%d] localRegion: [%s], absRegion: [%s]",
        GetName().c_str(), NeedDrawBehindWindow(), drawBehindWindowRegion_.ToString().c_str(),
        absDrawBehindWindowRegion.ToString().c_str());
    if (!NeedDrawBehindWindow() || drawBehindWindowRegion_.IsEmpty()) {
        return;
    }
    auto partialTransparentRegion = Occlusion::Region{ Occlusion::Rect{ absDrawBehindWindowRegion} };
    opaqueRegion_.SubSelf(partialTransparentRegion);
}

void RSSurfaceRenderNode::CalcFilterCacheValidForOcclusion()
{
    isFilterCacheStatusChanged_ = false;
    bool currentCacheValidForOcclusion = isFilterCacheFullyCovered_ && dirtyManager_->IsFilterCacheRectValid();
    if (isFilterCacheValidForOcclusion_ != currentCacheValidForOcclusion) {
        isFilterCacheValidForOcclusion_ = currentCacheValidForOcclusion;
        isFilterCacheStatusChanged_ = true;
    }
}

void RSSurfaceRenderNode::UpdateFilterNodes(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    if (nodePtr == nullptr) {
        return;
    }
    filterNodes_.emplace_back(nodePtr);
}

void RSSurfaceRenderNode::CheckValidFilterCacheFullyCoverTarget(const RSRenderNode& filterNode, const RectI& targetRect)
{
    if (filterNode.IsInstanceOf<RSEffectRenderNode>()) {
        return;
    }
    if (isFilterCacheFullyCovered_ || !filterNode.IsFilterCacheValid()) {
        return;
    }
    // AbsRect may not update here, so use filterCachedRegion to occlude
    isFilterCacheFullyCovered_ = targetRect.IsInsideOf(filterNode.GetFilterCachedRegion());
}

void RSSurfaceRenderNode::UpdateOccludedByFilterCache(bool val)
{
#ifdef RS_ENABLE_GPU
    isOccludedByFilterCache_ = val;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetOccludedByFilterCache(isOccludedByFilterCache_);
#endif
}

void RSSurfaceRenderNode::UpdateSurfaceCacheContentStaticFlag(bool isAccessibilityChanged)
{
#ifdef RS_ENABLE_GPU
    auto contentStatic = false;
    auto uifirstContentDirty = false;
    if (IsLeashWindow()) {
        for (auto& child : *GetSortedChildren()) {
            if (!child) {
                continue;
            }
            auto childSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (childSurface) {
                continue;
            }
            if (child->IsDirty() || child->IsSubTreeDirty()) {
                uifirstContentDirty = true;
            }
        }
        contentStatic = (!IsSubTreeDirty() || GetForceUpdateByUifirst()) && !HasRemovedChild();
    } else {
        contentStatic = (!IsSubTreeDirty() || GetForceUpdateByUifirst()) && !IsContentDirty();
    }
    contentStatic = contentStatic && !isAccessibilityChanged;
    uifirstContentDirty_ = uifirstContentDirty_ || uifirstContentDirty || HasRemovedChild();
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSurfaceCacheContentStatic(contentStatic, lastFrameSynced_);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceRenderNode::UpdateSurfaceCacheContentStaticFlag: "
        "name[%s] Id[%" PRIu64 "], contentStatic[%d] subTreeDirty[%d] contentDirty[%d] forceUpdate[%d] "
        "accessibilityChanged[%d] hasRemovedChild[%d], GetChildrenCount[%d], uifirstContentDirty:%d",
        GetName().c_str(), GetId(), contentStatic, IsSubTreeDirty(), IsContentDirty(), GetForceUpdateByUifirst(),
        isAccessibilityChanged, HasRemovedChild(), GetChildrenCount(), uifirstContentDirty_);
#endif
}

bool RSSurfaceRenderNode::IsOccludedByFilterCache() const
{
    return isOccludedByFilterCache_;
}

void RSSurfaceRenderNode::UpdateSurfaceSubTreeDirtyFlag()
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSurfaceSubTreeDirty(IsSubTreeDirty());
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::UpdateDrawingCacheNodes(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    if (nodePtr == nullptr) {
        return;
    }
    drawingCacheNodes_.emplace(nodePtr->GetId(), nodePtr);
}

void RSSurfaceRenderNode::ResetDrawingCacheStatusIfNodeStatic(
    std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects)
{
    // traversal drawing cache nodes including app window
    EraseIf(drawingCacheNodes_, [this, &allRects](const auto& pair) {
        auto node = pair.second.lock();
        if (node == nullptr || !node->IsOnTheTree()) {
            return true;
        }
        node->SetDrawingCacheChanged(false);
        node->GetFilterRectsInCache(allRects);
        return false;
    });
}

void RSSurfaceRenderNode::UpdateFilterCacheStatusWithVisible(bool visible)
{
    if (visible == prevVisible_) {
        return;
    }
    prevVisible_ = visible;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!RSUniRenderJudgement::IsUniRender() && !visible && !filterNodes_.empty()
        && !isOcclusionVisibleWithoutFilter_) {
        for (auto& node : filterNodes_) {
            node->GetMutableRenderProperties().ClearFilterCache();
        }
    }
#endif
}

void RSSurfaceRenderNode::UpdateFilterCacheStatusIfNodeStatic(const RectI& clipRect, bool isRotationChanged)
{
    // traversal filter nodes including app window
    for (auto node : filterNodes_) {
        if (node == nullptr || !node->IsOnTheTree() || !node->GetRenderProperties().NeedFilter()) {
            continue;
        }
        if (node->IsInstanceOf<RSEffectRenderNode>()) {
            if (auto effectNode = node->ReinterpretCastTo<RSEffectRenderNode>()) {
                effectNode->SetRotationChanged(isRotationChanged);
            }
        }
        if (node->GetRenderProperties().GetBackgroundFilter()) {
            node->UpdateFilterCacheWithBelowDirty(*dirtyManager_);
        }
        if (node->GetRenderProperties().GetFilter()) {
            node->UpdateFilterCacheWithBelowDirty(*dirtyManager_);
        }
        node->UpdateFilterCacheWithSelfDirty();
    }
    SetFilterCacheFullyCovered(false);
    if (IsTransparent() && dirtyManager_->IfCacheableFilterRectFullyCover(GetOldDirtyInSurface())) {
        SetFilterCacheFullyCovered(true);
        RS_LOGD("UpdateFilterCacheStatusIfNodeStatic surfacenode %{public}" PRIu64 " [%{public}s] rectsize %{public}s",
            GetId(), GetName().c_str(), GetOldDirtyInSurface().ToString().c_str());
    }
    CalcFilterCacheValidForOcclusion();
}

Vector4f RSSurfaceRenderNode::GetWindowCornerRadius()
{
    if (!GetRenderProperties().GetCornerRadius().IsZero()) {
        return GetRenderProperties().GetCornerRadius();
    }
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
    if (parent != nullptr && parent->IsLeashWindow()) {
        return parent->GetRenderProperties().GetCornerRadius();
    }
    return Vector4f();
}

Occlusion::Region RSSurfaceRenderNode::ResetOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow) const
{
    if (isFocusWindow) {
        return SetFocusedWindowOpaqueRegion(absRect, screenRotation);
    } else {
        return SetUnfocusedWindowOpaqueRegion(absRect, screenRotation);
    }
}

void RSSurfaceRenderNode::ContainerConfig::Update(bool hasContainer, RRect rrect)
{
    this->hasContainerWindow_ = hasContainer;
    this->inR_ = RoundFloor(rrect.radius_[0].x_);
    this->innerRect_ = {
        RoundFloor(rrect.rect_.left_),
        RoundFloor(rrect.rect_.top_),
        RoundFloor(rrect.rect_.width_),
        RoundFloor(rrect.rect_.height_) };
}

void RSSurfaceRenderNode::SetContainerWindow(bool hasContainerWindow, RRect rrect)
{
    RS_LOGD("RSSurfaceRenderNode::SetContainerWindow %{public}s %{public}" PRIu64 ", rrect: %{public}s",
        GetName().c_str(), GetId(), rrect.ToString().c_str());
    containerConfig_.Update(hasContainerWindow, rrect);
}

/*
    If a surfacenode with containerwindow is not focus window, then its opaque
region is absRect minus four roundCorner corresponding small rectangles.
This corners removed region can be assembled with two crossed rectangles.
Furthermore, when the surfacenode is not focus window, the inner content roundrect's
boundingbox rect can be set opaque.
*/
Occlusion::Region RSSurfaceRenderNode::SetUnfocusedWindowOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation) const
{
    RSSurfaceRenderNode::ContainerConfig config = GetAbsContainerConfig();
    Occlusion::Rect opaqueRect1{ absRect.left_ + config.outR_,
        absRect.top_,
        absRect.GetRight() - config.outR_,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + config.outR_,
        absRect.GetRight(),
        absRect.GetBottom() - config.outR_};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);
    return opaqueRegion;
}

/*
    If a surfacenode with containerwindow is a focused window, then its containerWindow region
should be set transparent, including: title, content padding area, border, and content corners.
Note this region is not centrosymmetric, hence it should be differentiated under different
screen rotation state as top/left/bottom/right has changed when screen rotated.
*/
Occlusion::Region RSSurfaceRenderNode::SetFocusedWindowOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation) const
{
    RSSurfaceRenderNode::ContainerConfig config = GetAbsContainerConfig();
    Occlusion::Rect outRect1{ absRect.left_ + config.outR_,
        absRect.top_,
        absRect.GetRight() - config.outR_,
        absRect.GetBottom()};
    Occlusion::Rect outRect2{ absRect.left_,
        absRect.top_ + config.outR_,
        absRect.GetRight(),
        absRect.GetBottom() - config.outR_};
    Occlusion::Region outRegion1{outRect1};
    Occlusion::Region outRegion2{outRect2};
    Occlusion::Region absRegion{Occlusion::Rect{absRect}};
    Occlusion::Region outRRegion = absRegion.Sub(outRegion1).Sub(outRegion2);
    RectI innerRect = config.innerRect_;
    Occlusion::Rect opaqueRect1{ innerRect.left_ + config.inR_,
        innerRect.top_,
        innerRect.GetRight() - config.inR_,
        innerRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ innerRect.left_,
        innerRect.top_ + config.inR_,
        innerRect.GetRight(),
        innerRect.GetBottom() - config.inR_};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2).Sub(outRRegion);
    return opaqueRegion;
}

void RSSurfaceRenderNode::SetCornerRadiusOpaqueRegion(
    const RectI& absRect, const Vector4<int>& cornerRadius)
{
    Occlusion::Rect opaqueRect0{ absRect.GetLeft(), absRect.GetTop(),
        absRect.GetRight(), absRect.GetBottom()};
    Occlusion::Rect opaqueRect1{ absRect.GetLeft(), absRect.GetTop(),
        absRect.GetLeft() + cornerRadius.x_, absRect.GetTop() + cornerRadius.x_};
    Occlusion::Rect opaqueRect2{ absRect.GetRight() - cornerRadius.y_, absRect.GetTop(),
        absRect.GetRight(), absRect.GetTop() + cornerRadius.y_};
    Occlusion::Rect opaqueRect3{ absRect.GetRight() - cornerRadius.z_, absRect.GetBottom() - cornerRadius.z_,
        absRect.GetRight(), absRect.GetBottom()};
    Occlusion::Rect opaqueRect4{ absRect.GetLeft(), absRect.GetBottom() - cornerRadius.w_,
        absRect.GetLeft() + cornerRadius.w_, absRect.GetBottom()};

    Occlusion::Region r0{opaqueRect0};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region r3{opaqueRect3};
    Occlusion::Region r4{opaqueRect4};
    roundedCornerRegion_ = r1.Or(r2).Or(r3).Or(r4);
    opaqueRegion_ = r0.Sub(r1).Sub(r2).Sub(r3).Sub(r4);
}

void RSSurfaceRenderNode::ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation)
{
    if (!HasContainerWindow()) {
        containerRegion_ = Occlusion::Region{};
        return;
    }
    Occlusion::Region absRegion{Occlusion::Rect{absRect}};
    Occlusion::Region innerRectRegion = SetFocusedWindowOpaqueRegion(absRect, screenRotation);
    containerRegion_ = absRegion.Sub(innerRectRegion);
}

RSSurfaceRenderNode::ContainerConfig RSSurfaceRenderNode::GetAbsContainerConfig() const
{
    auto& geoPtr = GetRenderProperties().GetBoundsGeometry();
    RSSurfaceRenderNode::ContainerConfig config;
    if (geoPtr) {
        auto& matrix = geoPtr->GetAbsMatrix();
        Drawing::Rect innerRadiusRect(0, 0, containerConfig_.inR_, containerConfig_.inR_);
        matrix.MapRect(innerRadiusRect, innerRadiusRect);
        Drawing::Rect outerRadiusRect(0, 0, containerConfig_.outR_, containerConfig_.outR_);
        matrix.MapRect(outerRadiusRect, outerRadiusRect);
        config.inR_ = static_cast<int>(std::round(std::max(innerRadiusRect.GetWidth(), innerRadiusRect.GetHeight())));
        config.outR_ = static_cast<int>(
            std::round(std::max(outerRadiusRect.GetWidth(), outerRadiusRect.GetHeight())));
        RectF r = {
            containerConfig_.innerRect_.left_,
            containerConfig_.innerRect_.top_,
            containerConfig_.innerRect_.width_,
            containerConfig_.innerRect_.height_};
        auto rect = geoPtr->MapAbsRect(r).IntersectRect(GetAbsDrawRect());
        config.innerRect_ = rect;
        return config;
    } else {
        return config;
    }
}

void RSSurfaceRenderNode::OnSync()
{
#ifdef RS_ENABLE_GPU
    if (!skipFrameDirtyRect_.IsEmpty()) {
        dirtyManager_->MergeDirtyRect(skipFrameDirtyRect_);
        auto uifirstDirtyRect = dirtyManager_->GetUifirstFrameDirtyRegion();
        uifirstDirtyRect = uifirstDirtyRect.JoinRect(skipFrameDirtyRect_);
        dirtyManager_->SetUifirstFrameDirtyRect(uifirstDirtyRect);
        skipFrameDirtyRect_.Clear();
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceRenderNode::OnSync name[%s] dirty[%s]",
        GetName().c_str(), dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
    if (!renderDrawable_) {
        return;
    }
    auto syncDirtyManager = renderDrawable_->GetSyncDirtyManager();
    dirtyManager_->OnSync(syncDirtyManager);
    if (IsMainWindowType() || IsLeashWindow() || GetLastFrameUifirstFlag() != MultiThreadCacheType::NONE) {
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
        if (surfaceParams == nullptr) {
            RS_LOGE("RSSurfaceRenderNode::OnSync surfaceParams is null");
            return;
        }
        surfaceParams->SetNeedSync(true);
    }
#ifndef ROSEN_CROSS_PLATFORM
    renderDrawable_->RegisterDeleteBufferListenerOnSync(GetRSSurfaceHandler()->GetConsumer());
#endif
    RSRenderNode::OnSync();
#endif
}

void RSSurfaceRenderNode::OnSkipSync()
{
    if (!dirtyManager_->GetCurrentFrameDirtyRegion().IsEmpty()) {
        auto surfaceDirtyRect = dirtyManager_->GetCurrentFrameDirtyRegion();
        skipFrameDirtyRect_ = skipFrameDirtyRect_.JoinRect(surfaceDirtyRect);
    }
    RSRenderNode::OnSkipSync();
}

bool RSSurfaceRenderNode::CheckIfOcclusionReusable(std::queue<NodeId>& surfaceNodesIds) const
{
    if (surfaceNodesIds.empty()) {
        return true;
    }
    auto lastFrameSurfaceNodeId = surfaceNodesIds.front();
    surfaceNodesIds.pop();
    if (lastFrameSurfaceNodeId != GetId()) {
        return true;
    }
    return false;
}

bool RSSurfaceRenderNode::CheckIfOcclusionChanged() const
{
    return GetZorderChanged() || GetDstRectChanged() || IsOpaqueRegionChanged() ||
        GetDirtyManager()->IsActiveSurfaceRectChanged();
}

bool RSSurfaceRenderNode::CheckParticipateInOcclusion()
{
    // planning: Need consider others situation
    isParentScaling_ = false;
    auto nodeParent = GetParent().lock();
    if (nodeParent && nodeParent->IsScale()) {
        isParentScaling_ = true;
        if (GetDstRectChanged() && !isOcclusionInSpecificScenes_) {
            return false;
        }
    }
    if ((IsTransparent() && !NeedDrawBehindWindow()) || GetAnimateState() || IsRotating() || IsSubSurfaceNode()) {
        return false;
    }
    return true;
}

void RSSurfaceRenderNode::RotateCorner(int rotationDegree, Vector4<int>& cornerRadius) const
{
    auto begin = cornerRadius.GetData();
    auto end = begin + Vector4<int>::V4SIZE;
    switch (rotationDegree) {
        case RS_ROTATION_90: {
            constexpr int moveTime = 1;
            std::rotate(begin, end - moveTime, end);
            break;
        }
        case RS_ROTATION_180: {
            constexpr int moveTime = 2;
            std::rotate(begin, end - moveTime, end);
            break;
        }
        case RS_ROTATION_270: {
            constexpr int moveTime = 3;
            std::rotate(begin, end - moveTime, end);
            break;
        }
        default:
            break;
    }
}

void RSSurfaceRenderNode::CheckAndUpdateOpaqueRegion(const RectI& screeninfo, const ScreenRotation screenRotation,
    const bool isFocusWindow)
{
    auto absRect = GetInnerAbsDrawRect();
    Vector4f tmpCornerRadius;
    Vector4f::Max(GetWindowCornerRadius(), GetGlobalCornerRadius(), tmpCornerRadius);
    Vector4<int> cornerRadius(static_cast<int>(std::round(tmpCornerRadius.x_)),
                                static_cast<int>(std::round(tmpCornerRadius.y_)),
                                static_cast<int>(std::round(tmpCornerRadius.z_)),
                                static_cast<int>(std::round(tmpCornerRadius.w_)));
    auto boundsGeometry = GetRenderProperties().GetBoundsGeometry();
    if (boundsGeometry) {
        absRect = absRect.IntersectRect(boundsGeometry->GetAbsRect());
        auto absChildrenRectF = boundsGeometry->MapRectWithoutRounding(GetChildrenRect().ConvertTo<float>(),
            boundsGeometry->GetAbsMatrix());
        absRect = absRect.IntersectRect(boundsGeometry->DeflateToRectI(absChildrenRectF));
        const auto& absMatrix = boundsGeometry->GetAbsMatrix();
        auto rotationDegree = static_cast<int>(-round(atan2(absMatrix.Get(Drawing::Matrix::SKEW_X),
            absMatrix.Get(Drawing::Matrix::SCALE_X)) * (RS_ROTATION_180 / PI)));
        if (rotationDegree < 0) {
            rotationDegree += RS_ROTATION_360;
        }
        RotateCorner(rotationDegree, cornerRadius);
    }

    if (!CheckOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius)) {
        if (absRect.IsEmpty()) {
            RS_LOGD("%{public}s absRect is empty, absDrawRect: %{public}s",
                GetName().c_str(), GetAbsDrawRect().ToString().c_str());
            RS_TRACE_NAME_FMT("%s absRect is empty, absDrawRect: %s",
                GetName().c_str(), GetAbsDrawRect().ToString().c_str());
        }
        ResetSurfaceOpaqueRegion(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius);
        SetOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius);
    }
}

bool RSSurfaceRenderNode::CheckOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow, const Vector4<int>& cornerRadius) const
{
    // OpaqueRegion should be recalculate under following circumstances.
    auto ret =
        // 1. Screen information changed, such as screen size, screen rotation, etc.
        opaqueRegionBaseInfo_.screenRect_ == screeninfo &&
        opaqueRegionBaseInfo_.screenRotation_ == screenRotation &&
        // 2. Position and size of surface nodes changed.
        opaqueRegionBaseInfo_.absRect_ == absRect &&
        opaqueRegionBaseInfo_.oldDirty_ == GetOldDirty() &&
        // 3. Focus/Non-focus window switching.
        opaqueRegionBaseInfo_.isFocusWindow_ == isFocusWindow &&
        // 4. Transparent/Non-trasparent wndow switching.
        opaqueRegionBaseInfo_.isTransparent_ == IsTransparent() &&
        // 5. Round corner changed.
        opaqueRegionBaseInfo_.cornerRadius_ == cornerRadius &&
        // 6. Container window changed.
        opaqueRegionBaseInfo_.hasContainerWindow_ == HasContainerWindow() &&
        opaqueRegionBaseInfo_.containerConfig_ == containerConfig_ &&
        // 7. Draw behind window region changed.
        opaqueRegionBaseInfo_.needDrawBehindWindow_ == NeedDrawBehindWindow() &&
        (NeedDrawBehindWindow() ? opaqueRegionBaseInfo_.absDrawBehindWindowRegion_ == GetFilterRect() : true);
    return ret;
}

void RSSurfaceRenderNode::SetOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow, const Vector4<int>& cornerRadius)
{
    opaqueRegionBaseInfo_.screenRect_ = screeninfo;
    opaqueRegionBaseInfo_.absRect_ = absRect;
    opaqueRegionBaseInfo_.oldDirty_ = GetOldDirty();
    opaqueRegionBaseInfo_.screenRotation_ = screenRotation;
    opaqueRegionBaseInfo_.isFocusWindow_ = isFocusWindow;
    opaqueRegionBaseInfo_.cornerRadius_ = cornerRadius;
    opaqueRegionBaseInfo_.isTransparent_ = IsTransparent();
    opaqueRegionBaseInfo_.hasContainerWindow_ = HasContainerWindow();
    opaqueRegionBaseInfo_.containerConfig_ = containerConfig_;
    opaqueRegionBaseInfo_.needDrawBehindWindow_ = NeedDrawBehindWindow();
    opaqueRegionBaseInfo_.absDrawBehindWindowRegion_ = NeedDrawBehindWindow() ? RectI() : GetFilterRect();
}

// [planning] Remove this after skia is upgraded, the clipRegion is supported
void RSSurfaceRenderNode::ResetChildrenFilterRects()
{
    childrenFilterNodes_.clear();
    childrenFilterRects_.clear();
    childrenFilterRectsCacheValid_.clear();
}

void RSSurfaceRenderNode::UpdateChildrenFilterRects(std::shared_ptr<RSRenderNode> filternode,
    const RectI& rect, bool cacheValid)
{
    if (!rect.IsEmpty()) {
        childrenFilterNodes_.emplace_back(filternode);
        childrenFilterRects_.emplace_back(rect);
        childrenFilterRectsCacheValid_.emplace_back(cacheValid);
    }
}

const std::vector<RectI>& RSSurfaceRenderNode::GetChildrenNeedFilterRects() const
{
    return childrenFilterRects_;
}

const std::vector<bool>& RSSurfaceRenderNode::GetChildrenNeedFilterRectsCacheValid() const
{
    return childrenFilterRectsCacheValid_;
}

const std::vector<std::shared_ptr<RSRenderNode>>& RSSurfaceRenderNode::GetChildrenFilterNodes() const
{
    return childrenFilterNodes_;
}

std::vector<RectI> RSSurfaceRenderNode::GetChildrenNeedFilterRectsWithoutCacheValid()
{
    std::vector<RectI> childrenFilterRectsWithoutCacheValid;
    auto maxSize = std::min(childrenFilterRects_.size(), childrenFilterRectsCacheValid_.size());
    for (size_t i = 0; i < maxSize; i++) {
        if (!childrenFilterRectsCacheValid_[i]) {
            childrenFilterRectsWithoutCacheValid.emplace_back(childrenFilterRects_[i]);
        }
    }
    return childrenFilterRectsWithoutCacheValid;
};

// manage abilities' nodeid info
void RSSurfaceRenderNode::UpdateAbilityNodeIds(NodeId id, bool isAdded)
{
    if (isAdded) {
        abilityNodeIds_.emplace(id);
    } else {
        abilityNodeIds_.erase(id);
    }
}

void RSSurfaceRenderNode::AddAbilityComponentNodeIds(std::unordered_set<NodeId>& nodeIds)
{
    abilityNodeIds_.insert(nodeIds.begin(), nodeIds.end());
}

void RSSurfaceRenderNode::ResetAbilityNodeIds()
{
    abilityNodeIds_.clear();
}

void RSSurfaceRenderNode::UpdateSurfaceCacheContentStatic()
{
    dirtyContentNodeNum_ = 0;
    dirtyGeoNodeNum_ = 0;
    dirtynodeNum_ = 0;
    surfaceCacheContentStatic_ = IsOnlyBasicGeoTransform() && !IsCurFrameSwitchToPaint();
}

void RSSurfaceRenderNode::UpdateSurfaceCacheContentStatic(
    const std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& activeNodeIds)
{
    dirtyContentNodeNum_ = 0;
    dirtyGeoNodeNum_ = 0;
    dirtynodeNum_ = activeNodeIds.size();
    surfaceCacheContentStatic_ = (IsOnlyBasicGeoTransform() || GetForceUpdateByUifirst()) &&
        !IsCurFrameSwitchToPaint();
    if (dirtynodeNum_ == 0) {
        RS_LOGD("Clear surface %{public}" PRIu64 " dirtynodes surfaceCacheContentStatic_:%{public}d",
            GetId(), surfaceCacheContentStatic_);
        return;
    }
    for (auto [id, subNode] : activeNodeIds) {
        auto node = subNode.lock();
        if (node == nullptr || (id == GetId() && surfaceCacheContentStatic_)) {
            continue;
        }
        // classify active nodes except instance surface itself
        if (node->IsContentDirty() && !node->IsNewOnTree() && !node->GetRenderProperties().IsGeoDirty()) {
            dirtyContentNodeNum_++;
        } else {
            dirtyGeoNodeNum_++;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("UpdateSurfaceCacheContentStatic [%s-%lu] contentStatic: %d, dirtyContentNode: %d, "
        "dirtyGeoNode: %d", GetName().c_str(), GetId(),
        surfaceCacheContentStatic_, dirtyContentNodeNum_, dirtyGeoNodeNum_);
    // if mainwindow node only basicGeoTransform and no subnode dirty, it is marked as CacheContentStatic_
    surfaceCacheContentStatic_ = surfaceCacheContentStatic_ && dirtyContentNodeNum_ == 0 && dirtyGeoNodeNum_ == 0;
}

const std::unordered_set<NodeId>& RSSurfaceRenderNode::GetAbilityNodeIds() const
{
    return abilityNodeIds_;
}

void RSSurfaceRenderNode::ResetChildHardwareEnabledNodes()
{
    childHardwareEnabledNodes_.clear();
}

void RSSurfaceRenderNode::AddChildHardwareEnabledNode(std::weak_ptr<RSSurfaceRenderNode> childNode)
{
    childHardwareEnabledNodes_.erase(std::remove_if(childHardwareEnabledNodes_.begin(),
        childHardwareEnabledNodes_.end(),
        [&childNode](const auto& iter) {
            auto node = iter.lock();
            auto childNodePtr = childNode.lock();
            return node && childNodePtr && node == childNodePtr;
        }), childHardwareEnabledNodes_.end());
    childHardwareEnabledNodes_.emplace_back(childNode);
}

void RSSurfaceRenderNode::UpdateChildHardwareEnabledNode(NodeId id, bool isOnTree)
{
    if (isOnTree) {
        needCollectHwcNode_ = true;
    } else {
        childHardwareEnabledNodes_.erase(std::remove_if(childHardwareEnabledNodes_.begin(),
            childHardwareEnabledNodes_.end(),
            [&id](std::weak_ptr<RSSurfaceRenderNode> item) {
                std::shared_ptr<RSSurfaceRenderNode> hwcNodePtr = item.lock();
                if (!hwcNodePtr) {
                    return false;
                }
                return hwcNodePtr->GetId() == id;
            }), childHardwareEnabledNodes_.end());
    }
}

const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& RSSurfaceRenderNode::GetChildHardwareEnabledNodes() const
{
    return childHardwareEnabledNodes_;
}

void RSSurfaceRenderNode::SetHwcChildrenDisabledState()
{
    const auto TraverseHwcNodes = [](const auto& hwcNodes) {
        for (const auto& hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
                continue;
            }
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by parent",
                hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
        }
    };
    TraverseHwcNodes(GetChildHardwareEnabledNodes());
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>> allSubSurfaceNodes;
    GetAllSubSurfaceNodes(allSubSurfaceNodes);
    for (const auto& [_, weakNode] : allSubSurfaceNodes) {
        if (auto surfaceNode = weakNode.lock(); surfaceNode != nullptr) {
            TraverseHwcNodes(surfaceNode->GetChildHardwareEnabledNodes());
        }
    }
}

void RSSurfaceRenderNode::SetLocalZOrder(float localZOrder)
{
    localZOrder_ = localZOrder;
}

float RSSurfaceRenderNode::GetLocalZOrder() const
{
    return localZOrder_;
}

void RSSurfaceRenderNode::OnApplyModifiers()
{
    auto& properties = GetMutableRenderProperties();
    auto& geoPtr = (properties.GetBoundsGeometry());

    // Multiply context alpha into alpha
    properties.SetAlpha(properties.GetAlpha() * contextAlpha_);

    // Apply the context matrix into the bounds geometry
    geoPtr->SetContextMatrix(contextMatrix_);
    if (!ShouldPaint()) {
        UpdateFilterCacheStatusWithVisible(false);
    }
}

void RSSurfaceRenderNode::SetTotalMatrix(const Drawing::Matrix& totalMatrix)
{
    totalMatrix_ = totalMatrix;
    stagingRenderParams_->SetTotalMatrix(totalMatrix);
}

std::optional<Drawing::Rect> RSSurfaceRenderNode::GetContextClipRegion() const
{
    return contextClipRect_;
}

bool RSSurfaceRenderNode::LeashWindowRelatedAppWindowOccluded(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& appNodes)
{
    if (!IsLeashWindow()) {
        return false;
    }
    for (auto& childNode : *GetChildren()) {
        const auto& childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode);
        if (childNodeSurface && childNodeSurface->GetVisibleRegion().IsEmpty()) {
            appNodes.emplace_back(childNodeSurface);
        } else {
            return false;
        }
    }
    return true;
}

std::vector<std::shared_ptr<RSSurfaceRenderNode>> RSSurfaceRenderNode::GetLeashWindowNestedSurfaces()
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> res;
    if (!IsLeashWindow()) {
        return res;
    }
    for (auto& childNode : *GetSortedChildren()) {
        if (auto childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode)) {
                res.emplace_back(childNodeSurface);
        }
    }
    return res;
}

bool RSSurfaceRenderNode::IsHistoryOccludedDirtyRegionNeedSubmit()
{
    return (hasUnSubmittedOccludedDirtyRegion_ &&
        !historyUnSubmittedOccludedDirtyRegion_.IsEmpty() &&
        !visibleRegion_.IsEmpty() &&
        visibleRegion_.IsIntersectWith(historyUnSubmittedOccludedDirtyRegion_));
}

void RSSurfaceRenderNode::ClearHistoryUnSubmittedDirtyInfo()
{
    hasUnSubmittedOccludedDirtyRegion_ = false;
    historyUnSubmittedOccludedDirtyRegion_.Clear();
}

void RSSurfaceRenderNode::UpdateHistoryUnsubmittedDirtyInfo()
{
    hasUnSubmittedOccludedDirtyRegion_ = true;
    historyUnSubmittedOccludedDirtyRegion_ = dirtyManager_->GetCurrentFrameDirtyRegion().JoinRect(
        historyUnSubmittedOccludedDirtyRegion_);
}

bool RSSurfaceRenderNode::IsUIFirstSelfDrawCheck()
{
    if (IsAppWindow()) {
        auto hardwareEnabledNodes = GetChildHardwareEnabledNodes();
        for (auto& hardwareEnabledNode : hardwareEnabledNodes) {
            auto hardwareEnabledNodePtr = hardwareEnabledNode.lock();
            if (hardwareEnabledNodePtr && hardwareEnabledNodePtr->surfaceHandler_->IsCurrentFrameBufferConsumed()) {
                return false;
            }
        }
    }
    if (IsMainWindowType()) {
        return true;
    } else if (IsLeashWindow()) {
        auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
        // leashwindow subthread cache considered static if and only if all nested surfacenode static
        // (include appwindow and starting window)
        for (auto& nestedSurface: nestedSurfaceNodes) {
            if (nestedSurface && !nestedSurface->IsUIFirstSelfDrawCheck()) {
                return false;
            }
        }
        return true;
    } else if (IsSelfDrawingType()) {
        return !surfaceHandler_->IsCurrentFrameBufferConsumed();
    } else {
        return false;
    }
}

void RSSurfaceRenderNode::UpdateCacheSurfaceDirtyManager(int bufferAge)
{
    if (!cacheSurfaceDirtyManager_ || !dirtyManager_) {
        return;
    }
    cacheSurfaceDirtyManager_->Clear();
    cacheSurfaceDirtyManager_->MergeDirtyRect(dirtyManager_->GetLatestDirtyRegion());
    cacheSurfaceDirtyManager_->SetBufferAge(bufferAge);
    cacheSurfaceDirtyManager_->UpdateDirty(false);
    // for leashwindow type, nested app surfacenode's cacheSurfaceDirtyManager update is required
    auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
    for (auto& nestedSurface : nestedSurfaceNodes) {
        if (nestedSurface) {
            nestedSurface->UpdateCacheSurfaceDirtyManager(bufferAge);
        }
    }
}

void RSSurfaceRenderNode::SetIsOnTheTree(bool onTree, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId, NodeId displayNodeId)
{
    if (GetSurfaceNodeType() == RSSurfaceNodeType::CURSOR_NODE) {
        std::string uniqueIdStr = "null";
#ifndef ROSEN_CROSS_PLATFORM
        uniqueIdStr = GetRSSurfaceHandler() != nullptr && GetRSSurfaceHandler()->GetConsumer() != nullptr ?
                        std::to_string(GetRSSurfaceHandler()->GetConsumer()->GetUniqueId()) : "null";
#endif
        RS_LOGI("RSSurfaceRenderNode:SetIsOnTheTree, node:[name: %{public}s, id: %{public}" PRIu64 "], "
            "on tree: %{public}d, nodeType: %{public}d, uniqueId: %{public}s, displayNodeId: %{public}" PRIu64,
            GetName().c_str(), GetId(), onTree, static_cast<int>(nodeType_), uniqueIdStr.c_str(), displayNodeId);
    }
#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
    SendSurfaceNodeTreeStatus(onTree);
#endif
    RS_TRACE_NAME_FMT("RSSurfaceRenderNode:SetIsOnTheTree, node:[name: %s, id: %" PRIu64 "], "
        "on tree: %d, nodeType: %d", GetName().c_str(), GetId(), onTree, static_cast<int>(nodeType_));
    instanceRootNodeId = IsLeashOrMainWindow() ? GetId() : instanceRootNodeId;
    if (IsLeashWindow()) {
        firstLevelNodeId = GetId();
    } else if (IsAppWindow()) {
        firstLevelNodeId = GetId();
        auto parentNode = GetParent().lock();
        if (parentNode && parentNode->GetFirstLevelNodeId() != INVALID_NODEID) {
            firstLevelNodeId = parentNode->GetFirstLevelNodeId();
        }
    }
    if (IsSecureUIExtension() || IsUnobscuredUIExtensionNode()) {
        if (onTree) {
            secUIExtensionNodes_.insert(std::pair<NodeId, NodeId>(GetId(), instanceRootNodeId));
        } else {
            secUIExtensionNodes_.erase(GetId());
        }
        if (auto parent = GetParent().lock()) {
            parent->SetChildrenHasUIExtension(onTree);
        }
    }
    auto &monitor = SelfDrawingNodeMonitor::GetInstance();
    if (monitor.IsListeningEnabled() && IsSelfDrawingType()) {
        if (onTree) {
            auto rect = GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
            std::string nodeName = GetName();
            monitor.InsertCurRectMap(GetId(), nodeName, rect);
        } else {
            monitor.EraseCurRectMap(GetId());
        }
    }
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSBaseRenderNode::SetIsOnTheTree(onTree, instanceRootNodeId, firstLevelNodeId, cacheNodeId,
        INVALID_NODEID, displayNodeId);
}

#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
void RSSurfaceRenderNode::SendSurfaceNodeTreeStatus(bool onTree)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && !onTree) {
        std::shared_ptr<ApsMonitorImpl> apsMonitor = std::make_shared<ApsMonitorImpl>();
        apsMonitor->SetApsSurfaceDestroyedInfo(std::to_string(GetId()));
        prevSelfDrawHeight_ = 0.0f;
        prevSelfDrawWidth_ = 0.0f;
    }
}

void RSSurfaceRenderNode::SendSurfaceNodeBoundChange()
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE && prevSelfDrawHeight_ != properties.GetBoundHeight()
        && prevSelfDrawWidth_ != properties.GetBoundsWidth()) {
        prevSelfDrawHeight_ = properties.GetBoundHeight();
        prevSelfDrawWidth_ = properties.GetBoundsWidth();
        std::shared_ptr<ApsMonitorImpl> apsMonitor_ = std::make_shared<ApsMonitorImpl>();
        apsMonitor_->SetApsSurfaceBoundChange(std::to_string(prevSelfDrawHeight_), std::to_string(prevSelfDrawWidth_),
            std::to_string((uint64_t)GetId()));
    }
}
#endif

void RSSurfaceRenderNode::SetUIExtensionUnobscured(bool obscured)
{
    UIExtensionUnobscured_ = obscured;
}

bool RSSurfaceRenderNode::GetUIExtensionUnobscured() const
{
    return UIExtensionUnobscured_;
}

CacheProcessStatus RSSurfaceRenderNode::GetCacheSurfaceProcessedStatus() const
{
    return cacheProcessStatus_.load();
}

void RSSurfaceRenderNode::SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus)
{
    cacheProcessStatus_.store(cacheProcessStatus);
}

bool RSSurfaceRenderNode::HasOnlyOneRootNode() const
{
    if (GetChildrenCount() != 1) {
        return false;
    }

    const auto child = GetFirstChild();
    if (!child || child->GetType() != RSRenderNodeType::ROOT_NODE || child->GetChildrenCount() > 0) {
        return false;
    }

    return true;
}

bool RSSurfaceRenderNode::GetNodeIsSingleFrameComposer() const
{
    bool flag = false;
    if (RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()) {
        auto idx = GetName().find("hwstylusfeature");
        if (idx != std::string::npos) {
            flag = true;
        }
    }
    return isNodeSingleFrameComposer_ || flag;
}

bool RSSurfaceRenderNode::QuerySubAssignable(bool isRotation)
{
    if (!IsFirstLevelNode()) {
        return false;
    }
    UpdateTransparentSurface();
    RS_TRACE_NAME_FMT("SubThreadAssignable node[%lld] hasTransparent: %d, childHasVisibleFilter: %d, "
        "hasFilter: %d, isRotation: %d & %d globalAlpha[%f], hasProtectedLayer: %d", GetId(), hasTransparentSurface_,
        ChildHasVisibleFilter(), HasFilter(), isRotation, RSSystemProperties::GetCacheOptimizeRotateEnable(),
        GetGlobalAlpha(), GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
    bool rotateOptimize = RSSystemProperties::GetCacheOptimizeRotateEnable() ?
        !(isRotation && ROSEN_EQ(GetGlobalAlpha(), 0.0f)) : !isRotation;
    return !(hasTransparentSurface_ && ChildHasVisibleFilter()) && !HasFilter() && rotateOptimize &&
        !GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED);
}

void RSSurfaceRenderNode::UpdateTransparentSurface()
{
    hasTransparentSurface_ = IsTransparent();
    if (IsLeashWindow() && !hasTransparentSurface_) {
        for (auto &child : *GetSortedChildren()) {
            auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (childSurfaceNode && childSurfaceNode->IsTransparent()) {
                hasTransparentSurface_ = true;
                break;
            }
        }
    }
}

bool RSSurfaceRenderNode::GetHasTransparentSurface() const
{
    return hasTransparentSurface_;
}

bool RSSurfaceRenderNode::GetHasSharedTransitionNode() const
{
    return hasSharedTransitionNode_;
}

void RSSurfaceRenderNode::SetHasSharedTransitionNode(bool hasSharedTransitionNode)
{
    hasSharedTransitionNode_ = hasSharedTransitionNode;
}

Vector2f RSSurfaceRenderNode::GetGravityTranslate(float imgWidth, float imgHeight)
{
    Gravity gravity = GetRenderProperties().GetFrameGravity();
    if (IsLeashWindow()) {
        for (auto child : *GetSortedChildren()) {
            auto childSurfaceNode = child ? child->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
            if (childSurfaceNode) {
                gravity = childSurfaceNode->GetRenderProperties().GetFrameGravity();
                break;
            }
        }
    }

    float boundsWidth = GetRenderProperties().GetBoundsWidth();
    float boundsHeight = GetRenderProperties().GetBoundsHeight();
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(gravity, RectF {0.0f, 0.0f, boundsWidth, boundsHeight},
        imgWidth, imgHeight, gravityMatrix);
    return {gravityMatrix.Get(Drawing::Matrix::TRANS_X), gravityMatrix.Get(Drawing::Matrix::TRANS_Y)};
}

void RSSurfaceRenderNode::UpdateUIFirstFrameGravity()
{
#ifdef RS_ENABLE_GPU
    Gravity gravity = GetRenderProperties().GetFrameGravity();
    if (IsLeashWindow()) {
        std::vector<Gravity> subGravity{};
        for (const auto& child : *GetSortedChildren()) {
            auto childSurfaceNode = child ? child->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
            if (childSurfaceNode) {
                subGravity.push_back(childSurfaceNode->GetRenderProperties().GetFrameGravity());
            }
        }
        // planning: how to handle gravity while leashwindow has multiple subAppWindows is not clear yet
        if (subGravity.size() == 1) {
            gravity = subGravity.front();
        }
    }
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (!stagingSurfaceParams) {
        RS_LOGE("RSSurfaceRenderNode::UpdateUIFirstFrameGravity staingSurfaceParams is null");
        return;
    }
    stagingSurfaceParams->SetUIFirstFrameGravity(gravity);
    AddToPendingSyncList();
#endif
}

void RSSurfaceRenderNode::SetOcclusionVisible(bool visible)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetOcclusionVisible(visible);
        AddToPendingSyncList();
    } else {
        RS_LOGE("RSSurfaceRenderNode::SetOcclusionVisible stagingSurfaceParams is null");
    }

    isOcclusionVisible_ = visible;
#endif
}

void RSSurfaceRenderNode::UpdatePartialRenderParams()
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdatePartialRenderParams surfaceParams is null");
        return;
    }
    if (IsMainWindowType()) {
        surfaceParams->SetVisibleRegionInVirtual(visibleRegionInVirtual_);
        surfaceParams->SetIsParentScaling(isParentScaling_);
    }
    surfaceParams->absDrawRect_ = GetAbsDrawRect();
    surfaceParams->SetOldDirtyInSurface(GetOldDirtyInSurface());
    surfaceParams->SetTransparentRegion(GetTransparentRegion());
    surfaceParams->SetOpaqueRegion(GetOpaqueRegion());
    surfaceParams->SetRoundedCornerRegion(GetRoundedCornerRegion());
    surfaceParams->SetFirstLevelCrossNode(IsFirstLevelCrossNode());
#endif
}

void RSSurfaceRenderNode::UpdateExtendVisibleRegion(Occlusion::Region& region)
{
#ifdef RS_ENABLE_GPU
    extendVisibleRegion_.Reset();
    extendVisibleRegion_ = region.Or(visibleRegion_);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdateExtendVisibleRegion surfaceParams is nullptr");
        return;
    }
    surfaceParams->SetVisibleRegion(extendVisibleRegion_);
#endif
}

void RSSurfaceRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::InitRenderParams failed");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::InitRenderParams surfaceParams is null");
        return;
    }
    surfaceParams->SetIsUnobscuredUEC(IsUnobscuredUIExtensionNode());
#endif
}

void RSSurfaceRenderNode::UpdateRenderParams()
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdateRenderParams surfaceParams is null");
        return;
    }
    auto& properties = GetRenderProperties();
#ifndef ROSEN_CROSS_PLATFORM
    if (surfaceHandler_ && surfaceHandler_->GetConsumer()) {
        UpdatePropertyFromConsumer();
    }
#endif
    surfaceParams->alpha_ = properties.GetAlpha();
    surfaceParams->isClonedNodeOnTheTree_ = isClonedNodeOnTheTree_;
    surfaceParams->isCrossNode_ = IsCrossNode();
    surfaceParams->isSpherizeValid_ = properties.IsSpherizeValid();
    surfaceParams->isAttractionValid_ = properties.IsAttractionValid();
    surfaceParams->rsSurfaceNodeType_ = GetSurfaceNodeType();
    surfaceParams->backgroundColor_ = properties.GetBackgroundColor();
    surfaceParams->rrect_ = properties.GetRRect();
    surfaceParams->selfDrawingType_ = GetSelfDrawingNodeType();
    surfaceParams->stencilVal_ = stencilVal_;
    surfaceParams->needBilinearInterpolation_ = NeedBilinearInterpolation();
    surfaceParams->isMainWindowType_ = IsMainWindowType();
    surfaceParams->isLeashWindow_ = IsLeashWindow();
    surfaceParams->isAppWindow_ = IsAppWindow();
    surfaceParams->isCloneNode_ = isCloneNode_;
    surfaceParams->SetAncestorDisplayNode(ancestorDisplayNode_);
    surfaceParams->specialLayerManager_ = specialLayerManager_;
    surfaceParams->animateState_ = animateState_;
    surfaceParams->isRotating_ = isRotating_;
    surfaceParams->privacyContentLayerIds_ = privacyContentLayerIds_;
    surfaceParams->name_= name_;
    surfaceParams->bundleName_= bundleName_;
    surfaceParams->positionZ_ = properties.GetPositionZ();
    surfaceParams->SetVisibleRegion(visibleRegion_);
    surfaceParams->SetOldDirtyInSurface(GetOldDirtyInSurface());
    surfaceParams->dstRect_ = dstRect_;
    surfaceParams->overDrawBufferNodeCornerRadius_ = GetOverDrawBufferNodeCornerRadius();
    surfaceParams->isGpuOverDrawBufferOptimizeNode_ = isGpuOverDrawBufferOptimizeNode_;
    surfaceParams->SetSkipDraw(isSkipDraw_);
    surfaceParams->SetHidePrivacyContent(needHidePrivacyContent_);
    surfaceParams->visibleFilterChild_ = GetVisibleFilterChild();
    surfaceParams->isTransparent_ = IsTransparent();
    surfaceParams->leashPersistentId_ = leashPersistentId_;
    surfaceParams->hasSubSurfaceNodes_ = HasSubSurfaceNodes();
    surfaceParams->allSubSurfaceNodeIds_ = GetAllSubSurfaceNodeIds();
    surfaceParams->crossNodeSkipDisplayConversionMatrices_ = crossNodeSkipDisplayConversionMatrices_;
    surfaceParams->SetNeedSync(true);

    RSRenderNode::UpdateRenderParams();
#endif
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::UpdatePropertyFromConsumer()
{
    auto consumer = surfaceHandler_->GetConsumer();
    int32_t gravity = -1;
    consumer->GetFrameGravity(gravity);
    if (gravity >= 0) {
        GetMutableRenderProperties().SetFrameGravity(static_cast<Gravity>(gravity));
        RS_LOGD("RSSurfaceRenderNode, update frame gravity to = %{public}d", gravity);
    }

    int32_t fixed = -1;
    consumer->GetFixedRotation(fixed);
    if (fixed >= 0) {
        bool fixedRotation = (fixed == 1);
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
        surfaceParams->SetFixRotationByUser(fixedRotation);
        isFixRotationByUser_ = fixedRotation;
        RS_LOGD("RSSurfaceRenderNode, update fixed rotation to = %{public}d", fixedRotation);
    }
}
#endif

void RSSurfaceRenderNode::SetNeedOffscreen(bool needOffscreen)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetNeedOffscreen(needOffscreen);
        stagingSurfaceParams->SetFingerprint(GetFingerprint());
        AddToPendingSyncList();
    } else {
        RS_LOGE("RSSurfaceRenderNode::SetNeedOffscreen stagingSurfaceParams is null");
    }
#endif
}

void RSSurfaceRenderNode::SetClonedNodeRenderDrawable(
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr clonedNodeRenderDrawable)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetClonedNodeRenderDrawable stagingSurfaceParams is null");
        return;
    }
    stagingSurfaceParams->clonedNodeRenderDrawable_ = clonedNodeRenderDrawable;
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::SetSourceDisplayRenderNodeDrawable(
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetSourceDisplayRenderNodeDrawable stagingSurfaceParams is null");
        return;
    }
    // CacheImg does not support UIFirst, clost UIFirst
    SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE);
    stagingSurfaceParams->sourceDisplayRenderNodeDrawable_ = drawable;
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::UpdateAncestorDisplayNodeInRenderParams()
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdateAncestorDisplayNodeInRenderParams surfaceParams is null");
        return;
    }
    surfaceParams->SetAncestorDisplayNode(ancestorDisplayNode_);
    surfaceParams->SetNeedSync(true);
#endif
}

void RSSurfaceRenderNode::SetUifirstChildrenDirtyRectParam(RectI rect)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetUifirstChildrenDirtyRectParam(rect);
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetLeashWindowVisibleRegionEmptyParam()
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (!stagingSurfaceParams) {
        RS_LOGE("RSSurfaceRenderNode::SetLeashWindowVisibleRegionEmptyParam staingSurfaceParams is null");
        return;
    }
    stagingSurfaceParams->SetLeashWindowVisibleRegionEmptyParam(isLeashWindowVisibleRegionEmpty_);
#endif
}

bool RSSurfaceRenderNode::SetUifirstNodeEnableParam(MultiThreadCacheType b)
{
    bool ret = false;
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        ret = stagingSurfaceParams->SetUifirstNodeEnableParam(b);
        AddToPendingSyncList();
    }
#endif
    return ret;
}

void RSSurfaceRenderNode::SetIsParentUifirstNodeEnableParam(bool b)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetIsParentUifirstNodeEnableParam(b);
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetUifirstUseStarting(NodeId id)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetUifirstUseStarting(id);
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
{
#ifdef RS_ENABLE_GPU
    if (drmCornerRadiusInfo_ != drmCornerRadiusInfo) {
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
        if (surfaceParams) {
            surfaceParams->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
            drmCornerRadiusInfo_ = drmCornerRadiusInfo;
        }
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetForceDisableClipHoleForDRM(bool isForceDisable)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams == nullptr) {
        return;
    }
    stagingSurfaceParams->SetForceDisableClipHoleForDRM(isForceDisable);
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
}

void RSSurfaceRenderNode::SetSkipDraw(bool skip)
{
    isSkipDraw_ = skip;
    SetDirty();
}

bool RSSurfaceRenderNode::GetSkipDraw() const
{
    return isSkipDraw_;
}

const std::unordered_map<NodeId, NodeId>& RSSurfaceRenderNode::GetSecUIExtensionNodes()
{
    return secUIExtensionNodes_;
}

const std::unordered_map<std::string, bool>& RSSurfaceRenderNode::GetWatermark() const
{
    return watermarkHandles_;
}

bool RSSurfaceRenderNode::IsWatermarkEmpty() const
{
    return watermarkHandles_.empty();
}

void RSSurfaceRenderNode::SetSdrNit(float sdrNit)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSdrNit(sdrNit);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetDisplayNit(float displayNit)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetDisplayNit(displayNit);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetColorFollow(bool colorFollow)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetColorFollow(colorFollow);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetBrightnessRatio(float brightnessRatio)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetBrightnessRatio(brightnessRatio);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetLayerLinearMatrix(layerLinearMatrix);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

void RSSurfaceRenderNode::SetSdrHasMetadata(bool hasMetadata)
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSdrHasMetadata(hasMetadata);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
#endif
}

bool RSSurfaceRenderNode::GetSdrHasMetadata() const
{
#ifdef RS_ENABLE_GPU
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    return stagingSurfaceParams ? stagingSurfaceParams->GetSdrHasMetadata() : false;
#else
    return false;
#endif
}

void RSSurfaceRenderNode::SetWatermarkEnabled(const std::string& name, bool isEnabled)
{
    if (isEnabled) {
        RS_LOGI("RSSurfaceRenderNode::SetWatermarkEnabled[%{public}d], Name:%{public}s", isEnabled, name.c_str());
    }
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams) {
        surfaceParams->SetWatermarkEnabled(name, isEnabled);
    }
#endif
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::SetAbilityState(RSSurfaceNodeAbilityState abilityState)
{
    if (abilityState_ == abilityState) {
        ROSEN_LOGD("RSSurfaceRenderNode::SetAbilityState, surfaceNodeId:[%{public}" PRIu64 "], "
            "ability state same with before: %{public}s",
            GetId(), abilityState == RSSurfaceNodeAbilityState::FOREGROUND ? "foreground" : "background");
        return;
    }

    abilityState_ = abilityState;
    ROSEN_LOGI("RSSurfaceRenderNode::SetAbilityState, surfaceNodeId:[%{public}" PRIu64 "] ability state: %{public}s",
        GetId(), abilityState_ == RSSurfaceNodeAbilityState::FOREGROUND ? "foreground" : "background");
}

RSSurfaceNodeAbilityState RSSurfaceRenderNode::GetAbilityState() const
{
    return abilityState_;
}

bool RSSurfaceRenderNode::IsWaitUifirstFirstFrame() const
{
    return isWaitUifirstFirstFrame_;
}

void RSSurfaceRenderNode::SetWaitUifirstFirstFrame(bool wait)
{
    if (isWaitUifirstFirstFrame_ == wait) {
        return;
    }
    isWaitUifirstFirstFrame_ = wait;
    RS_TRACE_NAME_FMT("SetWaitUifirstFirstFrame id:%" PRIu64 ", wait:%d", GetId(), wait);
    RS_LOGI("SetWaitUifirstFirstFrame id:%{public}" PRIu64 ", wait:%{public}d", GetId(), wait);
}

void RSSurfaceRenderNode::SetNeedCacheSurface(bool needCacheSurface)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams) {
        surfaceParams->SetNeedCacheSurface(needCacheSurface);
    }
    AddToPendingSyncList();
#endif
}

bool RSSurfaceRenderNode::NeedUpdateDrawableBehindWindow() const
{
    bool needDrawBehindWindow = NeedDrawBehindWindow();
    return needDrawBehindWindow != oldNeedDrawBehindWindow_;
}

void RSSurfaceRenderNode::SetOldNeedDrawBehindWindow(bool val)
{
    oldNeedDrawBehindWindow_ = val;
}

bool RSSurfaceRenderNode::NeedDrawBehindWindow() const
{
    return !GetRenderProperties().GetBackgroundFilter() && !childrenBlurBehindWindow_.empty();
}

void RSSurfaceRenderNode::AddChildBlurBehindWindow(NodeId id)
{
    childrenBlurBehindWindow_.emplace(id);
}

void RSSurfaceRenderNode::RemoveChildBlurBehindWindow(NodeId id)
{
    childrenBlurBehindWindow_.erase(id);
}

void RSSurfaceRenderNode::CalDrawBehindWindowRegion()
{
    auto context = GetContext().lock();
    if (!context) {
        RS_LOGE("RSSurfaceRenderNode::CalDrawBehindWindowRegion, invalid context");
        return;
    }
    RectI region;
    auto geoPtr = GetMutableRenderProperties().GetBoundsGeometry();
    auto surfaceAbsMatrix = geoPtr->GetAbsMatrix();
    Drawing::Matrix invertSurfaceAbsMatrix;
    surfaceAbsMatrix.Invert(invertSurfaceAbsMatrix);
    for (auto& id : childrenBlurBehindWindow_) {
        if (auto child = context->GetNodeMap().GetRenderNode<RSRenderNode>(id)) {
            auto childRelativeMatrix = child->GetMutableRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
            childRelativeMatrix.PostConcat(invertSurfaceAbsMatrix);
            auto childRelativeRect = geoPtr->MapRect(child->GetSelfDrawRect(), childRelativeMatrix);
            region = region.JoinRect(childRelativeRect);
        } else {
            RS_LOGE("RSSurfaceRenderNode::CalDrawBehindWindowRegion, get child failed");
            return;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceRenderNode::CalDrawBehindWindowRegion: Id: %lu, BehindWindowRegion: %s",
        GetId(), region.ToString().c_str());
    RS_LOGD("RSSurfaceRenderNode::CalDrawBehindWindowRegion: Id: %{public}" PRIu64 ", BehindWindowRegion: %{public}s",
        GetId(), region.ToString().c_str());
    drawBehindWindowRegion_ = region;
    auto filterDrawable = GetFilterDrawable(false);
    if (!filterDrawable) {
        return;
    }
    filterDrawable->SetDrawBehindWindowRegion(region);
}

RectI RSSurfaceRenderNode::GetFilterRect() const
{
    if (!NeedDrawBehindWindow()) {
        return RSRenderNode::GetFilterRect();
    }
    auto geoPtr = GetRenderProperties().GetBoundsGeometry();
    auto surfaceAbsMatrix = geoPtr->GetAbsMatrix();
    RectF regionF(drawBehindWindowRegion_.GetLeft(), drawBehindWindowRegion_.GetTop(),
        drawBehindWindowRegion_.GetWidth(), drawBehindWindowRegion_.GetHeight());
    return geoPtr->MapRect(regionF, surfaceAbsMatrix);
}

RectI RSSurfaceRenderNode::GetBehindWindowRegion() const
{
    return drawBehindWindowRegion_;
}

bool RSSurfaceRenderNode::IsCurFrameSwitchToPaint()
{
    bool shouldPaint = ShouldPaint();
    bool changed = shouldPaint && !lastFrameShouldPaint_;
    lastFrameShouldPaint_ = shouldPaint;
    return changed;
}

void RSSurfaceRenderNode::SetApiCompatibleVersion(uint32_t apiCompatibleVersion)
{
    if (stagingRenderParams_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetApiCompatibleVersion: stagingRenderPrams is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetApiCompatibleVersion: surfaceParams is nullptr");
        return;
    }
    surfaceParams->SetApiCompatibleVersion(apiCompatibleVersion);
    AddToPendingSyncList();

    apiCompatibleVersion_ = apiCompatibleVersion;
}

void RSSurfaceRenderNode::SetIsCloned(bool isCloned)
{
    if (stagingRenderParams_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetIsCloned: stagingRenderParams_ is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::SetIsCloned: surfaceParams is nullptr");
        return;
    }
    surfaceParams->SetIsCloned(isCloned);
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::SetIsClonedNodeOnTheTree(bool isOnTheTree)
{
    isClonedNodeOnTheTree_ = isOnTheTree;
}

void RSSurfaceRenderNode::ResetIsBufferFlushed()
{
    if (stagingRenderParams_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::ResetIsBufferFlushed: stagingRenderPrams is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::ResetIsBufferFlushed: surfaceParams is nullptr");
        return;
    }
    if (!surfaceParams->GetIsBufferFlushed()) {
        return;
    }
    surfaceParams->SetIsBufferFlushed(false);
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::ResetSurfaceNodeStates()
{
    animateState_ = false;
    isRotating_ = false;
    specialLayerChanged_ = false;
    if (stagingRenderParams_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::ResetSurfaceNodeStates: stagingRenderPrams is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::ResetSurfaceNodeStates: surfaceParams is nullptr");
        return;
    }
    if (!surfaceParams->GetIsBufferFlushed()) {
        return;
    }
    surfaceParams->SetIsBufferFlushed(false);
    AddToPendingSyncList();
}
} // namespace Rosen
} // namespace OHOS
