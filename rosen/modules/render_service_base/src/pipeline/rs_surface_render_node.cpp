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

#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/rs_rt_refresh_callback.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties_painter.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

namespace {
bool CheckRootNodeReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child->IsInstanceOf<RSRootRenderNode>()) {
        auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
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

const int SCB_NODE_NAME_PREFIX_LENGTH = 3;
RSSurfaceRenderNode::RSSurfaceRenderNode(
    const RSSurfaceRenderNodeConfig& config, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(config.id, context, config.isTextureExportNode), RSSurfaceHandler(config.id), name_(config.name),
      bundleName_(config.bundleName), nodeType_(config.nodeType),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>()),
      cacheSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(config.id), config.id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(config.id, info);
#endif
    if (RSUniRenderJudgement::IsUniRender()) {
        syncDirtyManager_ = RSSystemProperties::GetRenderParallelEnabled() ?
            std::make_shared<RSDirtyRegionManager>() : dirtyManager_;
    }
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
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetConsumer(const sptr<IConsumerSurface>& consumer)
{
    consumer_ = consumer;
}
#endif

void RSSurfaceRenderNode::UpdateSrcRect(const Drawing::Canvas& canvas, const Drawing::RectI& dstRect,
    bool hasRotation)
{
    auto localClipRect = RSPaintFilterCanvas::GetLocalClipBounds(canvas, &dstRect).value_or(Drawing::Rect());
    const RSProperties& properties = GetRenderProperties();
    int left = std::clamp<int>(localClipRect.GetLeft(), 0, properties.GetBoundsWidth());
    int top = std::clamp<int>(localClipRect.GetTop(), 0, properties.GetBoundsHeight());
    int width = std::clamp<int>(std::ceil(localClipRect.GetWidth()), 0,
        std::ceil(properties.GetBoundsWidth() - left));
    int height = std::clamp<int>(std::ceil(localClipRect.GetHeight()), 0,
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

void RSSurfaceRenderNode::UpdateHwcDisabledBySrcRect(bool hasRotation)
{
#ifndef ROSEN_CROSS_PLATFORM
    const auto& buffer = GetBuffer();
    isHardwareForcedDisabledBySrcRect_ = false;
    if (buffer == nullptr) {
        return;
    }
     // We allow 1px error value to avoid disable dss by mistake [this flag only used for YUV buffer format]
    if (IsYUVBufferFormat()) {
        auto width = static_cast<int>(buffer->GetSurfaceBufferWidth());
        auto height = static_cast<int>(buffer->GetSurfaceBufferHeight());
        isHardwareForcedDisabledBySrcRect_ =  !GetAncoForceDoDirect() &&
            (hasRotation ? srcRect_.width_ + 1 < width : srcRect_.height_ + 1 < height);
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disableBySrc:%d src:[%d, %d]" \
            " buffer:[%d, %d] hasRotation:%d", GetName().c_str(), GetId(),
            isHardwareForcedDisabledBySrcRect_, srcRect_.width_, srcRect_.height_, width, height, hasRotation);
    }
#endif
}

bool RSSurfaceRenderNode::IsYUVBufferFormat() const
{
#ifndef ROSEN_CROSS_PLATFORM
    if (GetBuffer() == nullptr) {
        return false;
    }
    auto format = GetBuffer()->GetFormat();
    if (format < GRAPHIC_PIXEL_FMT_YUV_422_I || format == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
        format > GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        return false;
    }
    return true;
#else
    return false;
#endif
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

void RSSurfaceRenderNode::StoreMustRenewedInfo()
{
    mustRenewedInfo_ = RSRenderNode::HasMustRenewedInfo() || GetHasSecurityLayer() ||
        GetHasSkipLayer() || GetHasProtectedLayer();
}

std::string RSSurfaceRenderNode::DirtyRegionDump() const
{
    std::string dump = GetName() +
        " SurfaceNodeType [" + std::to_string(static_cast<unsigned int>(GetSurfaceNodeType())) + "]" +
        " Transparent [" + std::to_string(IsTransparent()) +"]" +
        " DstRect: " + GetDstRect().ToString() +
        " VisibleRegion: " + GetVisibleRegion().GetRegionInfo() +
        " VisibleDirtyRegion: " + GetVisibleDirtyRegion().GetRegionInfo() +
        " GlobalDirtyRegion: " + GetGlobalDirtyRegion().GetRegionInfo();
    if (GetDirtyManager()) {
        dump += " DirtyRegion: " + GetDirtyManager()->GetDirtyRegion().ToString();
    }
    return dump;
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
    auto& consumer = GetConsumer();
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
        if (GetBuffer() != nullptr && ShouldPaint()) {
            vec.emplace_back(shared_from_this());
        }
#endif
    }

    if (isSubSurfaceEnabled_) {
        if (onlyFirstLevel) {
            return;
        }
        for (auto &nodes : node->GetSubSurfaceNodes()) {
            for (auto &node : nodes.second) {
                auto surfaceNode = node.lock();
                if (surfaceNode != nullptr) {
                    surfaceNode->CollectSurface(surfaceNode, vec, isUniRender, onlyFirstLevel);
                }
            }
        }
    }
}

void RSSurfaceRenderNode::CollectSurfaceForUIFirstSwitch(uint32_t& leashWindowCount, uint32_t minNodeNum)
{
    if (IsLeashWindow() || IsStartingWindow()) {
        leashWindowCount++;
    }
    return;
}

void RSSurfaceRenderNode::ClearChildrenCache()
{
    for (auto& child : *GetChildren()) {
        auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            continue;
        }
#ifndef ROSEN_CROSS_PLATFORM
        auto& consumer = surfaceNode->GetConsumer();
        if (consumer != nullptr) {
            consumer->GoBackground();
        }
#endif
    }
    // Temporary solution, GetChildren will generate fullChildrenList_, which will cause memory leak
    OnTreeStateChanged();
}

void RSSurfaceRenderNode::OnTreeStateChanged()
{
    NotifyTreeStateChange();
    RSRenderNode::OnTreeStateChanged();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (grContext_ && !IsOnTheTree()) {
        if (auto context = GetContext().lock()) {
            RS_TRACE_NAME_FMT("need purgeUnlockedResources this SurfaceNode isn't on the tree Id:%" PRIu64 " Name:%s",
                GetId(), GetName().c_str());
            RS_LOGD("need purgeUnlockedResources this SurfaceNode isn't on the tree Id:%" PRIu64 " Name:%s",
                GetId(), GetName().c_str());
            if (IsLeashWindow()) {
                context->MarkNeedPurge(ClearMemoryMoment::COMMON_SURFACE_NODE_HIDE, RSContext::PurgeType::GENTLY);
            }
            if (GetName().substr(0, 3) == "SCB") {
                context->MarkNeedPurge(ClearMemoryMoment::SCENEBOARD_SURFACE_NODE_HIDE, RSContext::PurgeType::STRONGLY);
            }
        }
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
    SyncSecurityInfoToFirstLevelNode();
    SyncSkipInfoToFirstLevelNode();
    SyncProtectedInfoToFirstLevelNode();
}

bool RSSurfaceRenderNode::HasSubSurfaceNodes() const
{
    return childSubSurfaceNodes_.size() != 0;
}

void RSSurfaceRenderNode::SetIsSubSurfaceNode(bool isSubSurfaceNode)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams) {
        surfaceParams->SetIsSubSurfaceNode(isSubSurfaceNode);
        isSubSurfaceNode_ = isSubSurfaceNode;
    }
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
    if (!parentSurfaceNode || (parentSurfaceNode && !parentSurfaceNode->IsMainWindowType())) {
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

void RSSurfaceRenderNode::GetAllSubSurfaceNodes(
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>>& allSubSurfaceNodes)
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
        auto& consumer = GetConsumer();
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

    if ((IsAppWindow() || IsScbScreen()) && !IsNotifyUIBufferAvailable() && IsFirstFrameReadyToDraw(*this)) {
        NotifyUIBufferAvailable();
    }
}

bool RSSurfaceRenderNode::IsSubTreeNeedPrepare(bool filterInGlobal, bool isOccluded)
{
    // force preparation case
    if (IsLeashWindow()) {
        SetSubTreeDirty(false);
        UpdateChildrenOutOfRectFlag(false); // collect again
        return true;
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
    RSPropertiesPainter::DrawBackground(property, canvas, true, IsSelfDrawingNode() && (GetBuffer() != nullptr));
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

const std::shared_ptr<RSDirtyRegionManager>& RSSurfaceRenderNode::GetSyncDirtyManager() const
{
    return syncDirtyManager_;
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetCacheSurfaceDirtyManager() const
{
    return cacheSurfaceDirtyManager_;
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

void RSSurfaceRenderNode::SetForceHardwareAndFixRotation(bool flag)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetForceHardwareByUser(flag);
    AddToPendingSyncList();

    isForceHardwareByUser_ = flag;
}

bool RSSurfaceRenderNode::GetForceHardwareByUser() const
{
    return isForceHardwareByUser_;
}

bool RSSurfaceRenderNode::GetForceHardware() const
{
    return isForceHardware_;
}

void RSSurfaceRenderNode::SetForceHardware(bool flag)
{
    if (isForceHardwareByUser_ && !isForceHardware_ && flag) {
        originalDstRect_ = dstRect_;
    }
    isForceHardware_ = isForceHardwareByUser_ && flag;
    SetContentDirty();
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
    SetDirty();
    if (isSecurityLayer) {
        securityLayerIds_.insert(GetId());
    } else {
        securityLayerIds_.erase(GetId());
    }
    SyncSecurityInfoToFirstLevelNode();
}

void RSSurfaceRenderNode::SetSkipLayer(bool isSkipLayer)
{
    isSkipLayer_ = isSkipLayer;
    SetDirty();
    if (isSkipLayer) {
        skipLayerIds_.insert(GetId());
    } else {
        skipLayerIds_.erase(GetId());
    }
    SyncSkipInfoToFirstLevelNode();
}

void RSSurfaceRenderNode::SetProtectedLayer(bool isProtectedLayer)
{
    isProtectedLayer_ = isProtectedLayer;
    SetDirty();
    if (isProtectedLayer) {
        protectedLayerIds_.insert(GetId());
    } else {
        protectedLayerIds_.erase(GetId());
    }
    SyncProtectedInfoToFirstLevelNode();
}

bool RSSurfaceRenderNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}


bool RSSurfaceRenderNode::GetSkipLayer() const
{
    return isSkipLayer_;
}

bool RSSurfaceRenderNode::GetProtectedLayer() const
{
    return isProtectedLayer_;
}

bool RSSurfaceRenderNode::GetHasSecurityLayer() const
{
    return securityLayerIds_.size() != 0;
}

bool RSSurfaceRenderNode::GetHasSkipLayer() const
{
    return skipLayerIds_.size() != 0;
}

bool RSSurfaceRenderNode::GetHasProtectedLayer() const
{
    return protectedLayerIds_.size() != 0;
}

void RSSurfaceRenderNode::SyncSecurityInfoToFirstLevelNode()
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    // firstLevelNode is the nearest app window / leash node
    if (firstLevelNode && GetFirstLevelNodeId() != GetId()) {
        if (isSecurityLayer_ && IsOnTheTree()) {
            firstLevelNode->securityLayerIds_.insert(GetId());
        } else {
            firstLevelNode->securityLayerIds_.erase(GetId());
        }
    }
}

void RSSurfaceRenderNode::SyncSkipInfoToFirstLevelNode()
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
    // firstLevelNode is the nearest app window / leash node
    if (firstLevelNode && GetFirstLevelNodeId() != GetId()) {
        if (isSkipLayer_ && IsOnTheTree()) {
            firstLevelNode->skipLayerIds_.insert(GetId());
        } else {
            firstLevelNode->skipLayerIds_.erase(GetId());
        }
    }
}

void RSSurfaceRenderNode::SyncProtectedInfoToFirstLevelNode()
{
    if (isProtectedLayer_) {
        auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetFirstLevelNode());
        // firstLevelNode is the nearest app window / leash node
        if (firstLevelNode && GetFirstLevelNodeId() != GetId()) {
            firstLevelNode->SetDirty();
            // should always sync protectedLayerIds_ to firstLevelNode
            if (isProtectedLayer_ && IsOnTheTree()) {
                firstLevelNode->protectedLayerIds_.insert(GetId());
            } else {
                firstLevelNode->protectedLayerIds_.erase(GetId());
            }
        }
    }
}

void RSSurfaceRenderNode::SetFingerprint(bool hasFingerprint)
{
    hasFingerprint_ = hasFingerprint;
}

bool RSSurfaceRenderNode::GetFingerprint() const
{
    return hasFingerprint_;
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

void RSSurfaceRenderNode::SetHDRPresent(bool hasHdrPresent)
{
    hasHdrPresent_ = hasHdrPresent;
}

bool RSSurfaceRenderNode::GetHDRPresent() const
{
    return hasHdrPresent_;
}

void RSSurfaceRenderNode::SetForceUIFirstChanged(bool forceUIFirstChanged)
{
    forceUIFirstChanged_ = forceUIFirstChanged;
}
bool RSSurfaceRenderNode::GetForceUIFirstChanged()
{
    return forceUIFirstChanged_;
}

void RSSurfaceRenderNode::SetAncoForceDoDirect(bool ancoForceDoDirect)
{
    ancoForceDoDirect_ = ancoForceDoDirect;
}
bool RSSurfaceRenderNode::GetAncoForceDoDirect() const
{
    return ancoForceDoDirect_;
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

void RSSurfaceRenderNode::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorGamut RSSurfaceRenderNode::GetColorSpace() const
{
    return colorSpace_;
}

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (consumer_ != nullptr) {
        consumer_->SetDefaultWidthAndHeight(width, height);
    }
#else
#ifdef USE_SURFACE_TEXTURE
    auto texture = GetSurfaceTexture();
    if (texture) {
        texture->UpdateSurfaceDefaultSize(width, height);
    }
#endif
#endif
}

void RSSurfaceRenderNode::OnSkipSync()
{
#ifndef ROSEN_CROSS_PLATFORM
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams && surfaceParams->IsLayerDirty()) {
        auto& preBuffer = surfaceParams->GetPreBuffer();
        if (!preBuffer) {
            return;
        }
        auto context = GetContext().lock();
        if (context && !surfaceParams->GetHardwareEnabled()) {
            context->GetMutableSkipSyncBuffer().push_back(
                { preBuffer, GetConsumer(), surfaceParams->GetLastFrameHardwareEnabled() });
        }
    }
#endif
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::UpdateBufferInfo(const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
    const sptr<SurfaceBuffer>& preBuffer)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetBuffer(buffer);
    surfaceParams->SetAcquireFence(acquireFence);
    surfaceParams->SetPreBuffer(preBuffer);
    AddToPendingSyncList();
}

void RSSurfaceRenderNode::NeedClearBufferCache()
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    std::set<int32_t> bufferCacheSet;
    if (GetBuffer()) {
        bufferCacheSet.insert(GetBuffer()->GetSeqNum());
    }
    if (GetPreBuffer().buffer) {
        bufferCacheSet.insert(GetPreBuffer().buffer->GetSeqNum());
    }
    surfaceParams->SetBufferClearCacheSet(bufferCacheSet);
    AddToPendingSyncList();
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
        ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderThread", GetId());
        RSRTRefreshCallback::Instance().ExecuteRefresh();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderService",
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
    if (isNotifyUIBufferAvailable_) {
        return;
    }
    isNotifyUIBufferAvailable_ = true;
    {
        std::lock_guard<std::mutex> lock(mutexUI_);
        if (callbackFromUI_) {
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %{public}" PRIu64, GetId());
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
    if (isCurrentFrameBufferConsumed_) {
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
            (GetDstRect().width_ > GetBuffer()->GetWidth() || GetDstRect().height_ > GetBuffer()->GetHeight()) &&
#endif
            GetRenderProperties().GetFrameGravity() != Gravity::RESIZE &&
            ROSEN_EQ(GetGlobalAlpha(), 1.0f);
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
            return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
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

bool RSSurfaceRenderNode::IsSCBNode()
{
    return GetName().substr(0, SCB_NODE_NAME_PREFIX_LENGTH) != "SCB";
}

void RSSurfaceRenderNode::UpdateHwcNodeLayerInfo(GraphicTransformType transform)
{
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
    layer.zOrder = GetGlobalZOrder();
    layer.gravity = static_cast<int32_t>(properties.GetFrameGravity());
    layer.blendType = GetBlendType();
    layer.matrix = totalMatrix_;
    isHardwareForcedDisabled_ = isProtectedLayer_ ? false : isHardwareForcedDisabled_;
    RS_LOGD("RSSurfaceRenderNode::UpdateHwcNodeLayerInfo: node: %{public}s-%{public}" PRIu64 ","
        " src: %{public}s, dst: %{public}s, bounds: [%{public}d, %{public}d]"
        " transform: %{public}d, zOrder: %{public}d, cur: %{public}d, last: %{public}d",
        GetName().c_str(), GetId(),
        srcRect_.ToString().c_str(),
        dstRect_.ToString().c_str(),
        layer.boundRect.w, layer.boundRect.h,
        transform, layer.zOrder, !IsHardwareForcedDisabled(), isLastFrameHwcEnabled_);
    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug:UpdateHwcNodeLayerInfo: node: %s-%lu,"
        " src: %s, dst: %s, bounds: [%d, %d], transform: %d, zOrder: %d, cur: %d, last: %d",
        GetName().c_str(), GetId(),
        srcRect_.ToString().c_str(),
        dstRect_.ToString().c_str(),
        layer.boundRect.w, layer.boundRect.h,
        transform, layer.zOrder, !IsHardwareForcedDisabled(), isLastFrameHwcEnabled_);
    surfaceParams->SetLayerInfo(layer);
    surfaceParams->SetHardwareEnabled(!IsHardwareForcedDisabled());
    surfaceParams->SetLastFrameHardwareEnabled(isLastFrameHwcEnabled_);
    AddToPendingSyncList();
#endif
}

void RSSurfaceRenderNode::UpdateHardwareDisabledState(bool disabled)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetLastFrameHardwareEnabled(!IsHardwareForcedDisabled());
    SetHardwareForcedDisabledState(disabled);
    surfaceParams->SetHardwareEnabled(!IsHardwareForcedDisabled());
    AddToPendingSyncList();
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

bool RSSurfaceRenderNode::SubNodeIntersectWithExtraDirtyRegion(const RectI& r) const
{
    if (!isDirtyRegionAlignedEnable_) {
        return false;
    }
    if (!extraDirtyRegionAfterAlignmentIsEmpty_) {
        return extraDirtyRegionAfterAlignment_.IsIntersectWith(r);
    }
    return false;
}

bool RSSurfaceRenderNode::SubNodeIntersectWithDirty(const RectI& r) const
{
    Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
    // if current node rect r is in global dirtyregion, it CANNOT be skipped
    if (!globalDirtyRegionIsEmpty_) {
        auto globalRect = globalDirtyRegion_.IsIntersectWith(nodeRect);
        if (globalRect) {
            return true;
        }
    }
    // if current node is in visible dirtyRegion, it CANNOT be skipped
    bool localIntersect = visibleDirtyRegion_.IsIntersectWith(nodeRect);
    if (localIntersect) {
        return true;
    }
    // if current node is transparent
    if (IsTransparent() || IsCurrentNodeInTransparentRegion(nodeRect) || IsTreatedAsTransparent()) {
        return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
    }
    return false;
}

bool RSSurfaceRenderNode::SubNodeNeedDraw(const RectI &r, PartialRenderType opDropType) const
{
    switch (opDropType) {
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP:
            return SubNodeIntersectWithDirty(r);
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION:
            return SubNodeVisible(r);
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY:
            // intersect with self visible dirty or other surfaces' extra dirty region after alignment
            return SubNodeVisible(r) && (SubNodeIntersectWithDirty(r) ||
                SubNodeIntersectWithExtraDirtyRegion(r));
        case PartialRenderType::DISABLED:
        case PartialRenderType::SET_DAMAGE:
        default:
            return true;
    }
    return true;
}

void RSSurfaceRenderNode::ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow, const Vector4<int>& cornerRadius)
{
    Occlusion::Rect absRectR { absRect };
    Occlusion::Region oldOpaqueRegion { opaqueRegion_ };

    // The transparent region of surfaceNode should include shadow area
    Occlusion::Rect dirtyRect { GetOldDirty() };
    transparentRegion_ = Occlusion::Region{ dirtyRect };

    if (IsTransparent()) {
        opaqueRegion_ = Occlusion::Region();
    } else {
        if (IsAppWindow() && HasContainerWindow()) {
            opaqueRegion_ = ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
        } else {
            if (!cornerRadius.IsZero()) {
                auto maxRadius = std::max({ cornerRadius.x_, cornerRadius.y_, cornerRadius.z_, cornerRadius.w_ });
                Vector4<int> dstCornerRadius((cornerRadius.x_ > 0 ? maxRadius : 0),
                                             (cornerRadius.y_ > 0 ? maxRadius : 0),
                                             (cornerRadius.z_ > 0 ? maxRadius : 0),
                                             (cornerRadius.w_ > 0 ? maxRadius : 0));
                opaqueRegion_ = SetCornerRadiusOpaqueRegion(absRect, dstCornerRadius);
            } else {
                opaqueRegion_ = Occlusion::Region{absRectR};
            }
        }
        transparentRegion_.SubSelf(opaqueRegion_);
    }
    Occlusion::Rect screen{screeninfo};
    Occlusion::Region screenRegion{screen};
    transparentRegion_.AndSelf(screenRegion);
    opaqueRegion_.AndSelf(screenRegion);
    opaqueRegionChanged_ = !oldOpaqueRegion.Xor(opaqueRegion_).IsEmpty();
    ResetSurfaceContainerRegion(screeninfo, absRect, screenRotation);
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
    // [planning] need to replace absRect with filterRect
    isFilterCacheFullyCovered_ = targetRect.IsInsideOf(
        filterNode.GetRenderProperties().GetBoundsGeometry()->GetAbsRect());
}

void RSSurfaceRenderNode::UpdateOccludedByFilterCache(bool val)
{
    isOccludedByFilterCache_ = val;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    surfaceParams->SetOccludedByFilterCache(isOccludedByFilterCache_);
}

void RSSurfaceRenderNode::UpdateSurfaceCacheContentStaticFlag()
{
    auto contentStatic = false;
    if (IsLeashWindow()) {
        contentStatic = (!IsSubTreeDirty() || GetForceUpdateByUifirst()) && !HasRemovedChild();
    } else {
        contentStatic = surfaceCacheContentStatic_;
    }
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSurfaceCacheContentStatic(contentStatic);
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceRenderNode::UpdateSurfaceCacheContentStaticFlag: "
        "[%d] name [%s] Id:%" PRIu64 "", contentStatic, GetName().c_str(), GetId());
}

bool RSSurfaceRenderNode::IsOccludedByFilterCache() const
{
    return isOccludedByFilterCache_;
}

void RSSurfaceRenderNode::UpdateSurfaceSubTreeDirtyFlag()
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetSurfaceSubTreeDirty(IsSubTreeDirty());
    }
    if (stagingRenderParams_->NeedSync()) {
        AddToPendingSyncList();
    }
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
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
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

void RSSurfaceRenderNode::ContainerConfig::Update(bool hasContainer, float density)
{
    this->hasContainerWindow_ = hasContainer;
    this->density = density;

    // px = vp * density
    float containerTitleHeight_ = CONTAINER_TITLE_HEIGHT * density;
    float containerContentPadding_ = CONTENT_PADDING * density;
    float containerBorderWidth_ = CONTAINER_BORDER_WIDTH * density;
    float containerOutRadius_ = CONTAINER_OUTER_RADIUS * density;
    float containerInnerRadius_ = CONTAINER_INNER_RADIUS * density;

    this->outR = RoundFloor(containerOutRadius_);
    this->inR = RoundFloor(containerInnerRadius_);
    this->bp = RoundFloor(containerBorderWidth_ + containerContentPadding_);
    this->bt = RoundFloor(containerBorderWidth_ + containerTitleHeight_);
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
    Occlusion::Rect opaqueRect1{ absRect.left_ + containerConfig_.outR,
        absRect.top_,
        absRect.GetRight() - containerConfig_.outR,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + containerConfig_.outR,
        absRect.GetRight(),
        absRect.GetBottom() - containerConfig_.outR};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);

    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
    }
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
    Occlusion::Region opaqueRegion;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bt + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bt};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
    }
    return opaqueRegion;
}

Occlusion::Region RSSurfaceRenderNode::SetCornerRadiusOpaqueRegion(
    const RectI& absRect, const Vector4<int>& cornerRadius) const
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
    Occlusion::Region opaqueRegion = r0.Sub(r1).Sub(r2).Sub(r3).Sub(r4);
    return opaqueRegion;
}

void RSSurfaceRenderNode::ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation)
{
    if (!HasContainerWindow()) {
        containerRegion_ = Occlusion::Region{};
        return;
    }
    Occlusion::Region absRegion{Occlusion::Rect{absRect}};
    Occlusion::Rect innerRect;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        case ScreenRotation::ROTATION_90: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        case ScreenRotation::ROTATION_180: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt};
            break;
        }
        case ScreenRotation::ROTATION_270: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        default: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
    }
    Occlusion::Region innerRectRegion{innerRect};
    containerRegion_ = absRegion.Sub(innerRectRegion);
}

void RSSurfaceRenderNode::OnSync()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceRenderNode::OnSync name[%s] dirty[%s]",
        GetName().c_str(), dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
    dirtyManager_->OnSync(syncDirtyManager_);
    if (IsMainWindowType() || IsLeashWindow() || GetLastFrameUifirstFlag() != MultiThreadCacheType::NONE) {
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
        if (surfaceParams == nullptr) {
            RS_LOGE("RSSurfaceRenderNode::OnSync surfaceParams is null");
            return;
        }
        surfaceParams->SetNeedSync(true);
    }
    RSRenderNode::OnSync();
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
    return GetZorderChanged() || GetDstRectChanged() || IsOpaqueRegionChanged();
}

bool RSSurfaceRenderNode::CheckParticipateInOcclusion()
{
    // planning: Need consider others situation
    isParentScaling_ = false;
    auto nodeParent = GetParent().lock();
    if (nodeParent && nodeParent->IsScale()) {
        isParentScaling_ = true;
        if (GetDstRectChanged()) {
            return false;
        }
    }
    if (IsTransparent() || GetAnimateState() || IsRotating() || IsSubSurfaceNode()) {
        return false;
    }
    return true;
}

void RSSurfaceRenderNode::CheckAndUpdateOpaqueRegion(const RectI& screeninfo, const ScreenRotation screenRotation)
{
    auto absRect = GetDstRect().IntersectRect(GetOldDirtyInSurface());
    Vector4f tmpCornerRadius;
    Vector4f::Max(GetWindowCornerRadius(), GetGlobalCornerRadius(), tmpCornerRadius);
    Vector4<int> cornerRadius(static_cast<int>(std::round(tmpCornerRadius.x_)),
                                static_cast<int>(std::round(tmpCornerRadius.y_)),
                                static_cast<int>(std::round(tmpCornerRadius.z_)),
                                static_cast<int>(std::round(tmpCornerRadius.w_)));

    bool ret = opaqueRegionBaseInfo_.screenRect_ == screeninfo &&
        opaqueRegionBaseInfo_.absRect_ == absRect &&
        opaqueRegionBaseInfo_.oldDirty_ == GetOldDirty() &&
        opaqueRegionBaseInfo_.screenRotation_ == screenRotation &&
        opaqueRegionBaseInfo_.cornerRadius_ == cornerRadius &&
        opaqueRegionBaseInfo_.isTransparent_ == IsTransparent() &&
        opaqueRegionBaseInfo_.hasContainerWindow_ == HasContainerWindow();
    if (!ret) {
        // planning: default process focus window
        ResetSurfaceOpaqueRegion(screeninfo, absRect, screenRotation, true, cornerRadius);
    }
    SetOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, true, cornerRadius);
}

bool RSSurfaceRenderNode::CheckOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow, const Vector4<int>& cornerRadius)
{
    return opaqueRegionBaseInfo_.screenRect_ == screeninfo &&
        opaqueRegionBaseInfo_.absRect_ == absRect &&
        opaqueRegionBaseInfo_.screenRotation_ == screenRotation &&
        opaqueRegionBaseInfo_.isFocusWindow_ == isFocusWindow &&
        opaqueRegionBaseInfo_.cornerRadius_ == cornerRadius &&
        opaqueRegionBaseInfo_.isTransparent_ == IsTransparent() &&
        opaqueRegionBaseInfo_.hasContainerWindow_ == HasContainerWindow();
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

void RSSurfaceRenderNode::AddAbilityComponentNodeIds(std::unordered_set<NodeId> nodeIds)
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
    surfaceCacheContentStatic_ = IsOnlyBasicGeoTransform();
}

void RSSurfaceRenderNode::UpdateSurfaceCacheContentStatic(
    const std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& activeNodeIds)
{
    dirtyContentNodeNum_ = 0;
    dirtyGeoNodeNum_ = 0;
    dirtynodeNum_ = activeNodeIds.size();
    surfaceCacheContentStatic_ = IsOnlyBasicGeoTransform() || GetForceUpdateByUifirst();
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
    childHardwareEnabledNodes_.emplace_back(childNode);
}

void RSSurfaceRenderNode::UpdateChildHardwareEnabledNode(NodeId id, bool isOnTree)
{
    if (isOnTree) {
        needCollectHwcNode_ = true;
    } else {
    }
}

const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& RSSurfaceRenderNode::GetChildHardwareEnabledNodes() const
{
    return childHardwareEnabledNodes_;
}

void RSSurfaceRenderNode::SetGlobalDirtyRegion(const RectI& rect, bool renderParallel)
{
    if (!renderDrawable_ || !renderDrawable_->GetRenderParams()) {
        return;
    }
    auto visibleRegion = renderParallel
        ? static_cast<RSSurfaceRenderParams*>(renderDrawable_->GetRenderParams().get())->GetVisibleRegion()
        : visibleRegion_;
    Occlusion::Rect tmpRect { rect.left_, rect.top_, rect.GetRight(), rect.GetBottom() };
    Occlusion::Region region { tmpRect };
    globalDirtyRegion_ = visibleRegion.And(region);
    globalDirtyRegionIsEmpty_ = globalDirtyRegion_.IsEmpty();
}

void RSSurfaceRenderNode::SetHwcChildrenDisabledStateByUifirst()
{
    if (IsAppWindow()) {
        auto hwcNodes = GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
                continue;
            }
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    } else if (IsLeashWindow()) {
        for (auto& child : *GetChildren()) {
            auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (surfaceNode == nullptr) {
                continue;
            }
            surfaceNode->SetHwcChildrenDisabledStateByUifirst();
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
            if (hardwareEnabledNodePtr && hardwareEnabledNodePtr->IsCurrentFrameBufferConsumed()) {
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
        return !isCurrentFrameBufferConsumed_;
    } else {
        return false;
    }
}

bool RSSurfaceRenderNode::IsCurFrameStatic(DeviceType deviceType)
{
    bool isDirty = deviceType == DeviceType::PC ? !surfaceCacheContentStatic_ :
        !dirtyManager_->GetCurrentFrameDirtyRegion().IsEmpty();
    if (isDirty) {
        return false;
    }
    if (IsMainWindowType()) {
        return true;
    } else if (IsLeashWindow()) {
        auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
        // leashwindow children changed or has other type node except surfacenode
        if (deviceType == DeviceType::PC && lastFrameChildrenCnt_ != GetChildren()->size()) {
            return false;
        }
        for (auto& nestedSurface: nestedSurfaceNodes) {
            if (nestedSurface && !nestedSurface->IsCurFrameStatic(deviceType)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool RSSurfaceRenderNode::IsVisibleDirtyEmpty(DeviceType deviceType)
{
    bool isStaticUnderVisibleRegion = false;
    if (!dirtyManager_->GetCurrentFrameDirtyRegion().IsEmpty()) {
        if (deviceType != DeviceType::PC) {
            return false;
        }
        // Visible dirty region optimization takes effecct only in PC or TABLET scenarios
        Occlusion::Rect currentFrameDirty(dirtyManager_->GetCurrentFrameDirtyRegion());
        if (!visibleRegion_.IsEmpty() && visibleRegion_.IsIntersectWith(currentFrameDirty)) {
            ClearHistoryUnSubmittedDirtyInfo();
            return false;
        }
        isStaticUnderVisibleRegion = true;
    }
    if (IsMainWindowType()) {
        if (deviceType == DeviceType::PC) {
            if (IsHistoryOccludedDirtyRegionNeedSubmit()) {
                ClearHistoryUnSubmittedDirtyInfo();
                return false;
            }
            if (isStaticUnderVisibleRegion) {
                UpdateHistoryUnsubmittedDirtyInfo();
            }
        }
        return true;
    } else if (IsLeashWindow()) {
        auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
        if (nestedSurfaceNodes.empty()) {
            return false;
        }
        for (auto& nestedSurface: nestedSurfaceNodes) {
            if (nestedSurface && !nestedSurface->IsVisibleDirtyEmpty(deviceType)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool RSSurfaceRenderNode::IsUIFirstCacheReusable(DeviceType deviceType)
{
    return GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DONE &&
        HasCachedTexture() && IsUIFirstSelfDrawCheck() && IsCurFrameStatic(deviceType);
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

void RSSurfaceRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId, NodeId uifirstRootNodeId)
{
    instanceRootNodeId = IsLeashOrMainWindow() ? GetId() : instanceRootNodeId;
    if (IsLeashWindow()) {
        firstLevelNodeId = GetId();
    } else if (IsAppWindow()) {
        firstLevelNodeId = GetId();
        auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
        if (parentNode && parentNode->GetFirstLevelNodeId() != INVALID_NODEID) {
            firstLevelNodeId = parentNode->GetFirstLevelNodeId ();
        }
    }
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSBaseRenderNode::SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
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
    hasTransparentSurface_ = false;
    if (IsLeashWindow()) {
        for (auto &child : *GetSortedChildren()) {
            auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (childSurfaceNode && childSurfaceNode->IsTransparent()) {
                hasTransparentSurface_ = true;
                break;
            }
        }
    } else {
        hasTransparentSurface_ = IsTransparent();
    }
    return !(hasTransparentSurface_ && ChildHasVisibleFilter()) && !HasFilter() && !isRotation
        && QueryIfAllHwcChildrenForceDisabledByFilter();
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

void RSSurfaceRenderNode::SetOcclusionVisible(bool visible)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetOcclusionVisible(visible);
        AddToPendingSyncList();
    } else {
        RS_LOGE("RSSurfaceRenderNode::SetOcclusionVisible stagingSurfaceParams is null");
    }

    isOcclusionVisible_ = visible;
}

void RSSurfaceRenderNode::UpdatePartialRenderParams()
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdatePartialRenderParams surfaceParams is null");
        return;
    }
    if (IsMainWindowType()) {
        surfaceParams->SetVisibleRegion(visibleRegion_);
        surfaceParams->SetIsParentScaling(isParentScaling_);
    }
    surfaceParams->absDrawRect_ = GetAbsDrawRect();
    surfaceParams->SetOldDirtyInSurface(GetOldDirtyInSurface());
    surfaceParams->SetTransparentRegion(GetTransparentRegion());
}

void RSSurfaceRenderNode::InitRenderParams()
{
    stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::InitRenderParams failed");
        return;
    }
}

void RSSurfaceRenderNode::UpdateRenderParams()
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdateRenderParams surfaceParams is null");
        return;
    }
    auto& properties = GetRenderProperties();
    surfaceParams->alpha_ = properties.GetAlpha();
    surfaceParams->isSpherizeValid_ = properties.IsSpherizeValid();
    surfaceParams->rsSurfaceNodeType_ = GetSurfaceNodeType();
    surfaceParams->backgroundColor_ = properties.GetBackgroundColor();
    surfaceParams->rrect_ = properties.GetRRect();
    surfaceParams->selfDrawingType_ = GetSelfDrawingNodeType();
    surfaceParams->needBilinearInterpolation_ = NeedBilinearInterpolation();
    surfaceParams->isMainWindowType_ = IsMainWindowType();
    surfaceParams->isLeashWindow_ = IsLeashWindow();
    surfaceParams->SetAncestorDisplayNode(ancestorDisplayNode_);
    surfaceParams->isSecurityLayer_ = isSecurityLayer_;
    surfaceParams->isSkipLayer_ = isSkipLayer_;
    surfaceParams->isProtectedLayer_ = isProtectedLayer_;
    surfaceParams->skipLayerIds_= skipLayerIds_;
    surfaceParams->securityLayerIds_= securityLayerIds_;
    surfaceParams->protectedLayerIds_= protectedLayerIds_;
    surfaceParams->name_= name_;
    surfaceParams->positionZ_ = properties.GetPositionZ();
    surfaceParams->overDrawBufferNodeCornerRadius_ = GetOverDrawBufferNodeCornerRadius();
    surfaceParams->isGpuOverDrawBufferOptimizeNode_ = isGpuOverDrawBufferOptimizeNode_;
    surfaceParams->SetNeedSync(true);

    RSRenderNode::UpdateRenderParams();
}

void RSSurfaceRenderNode::UpdateAncestorDisplayNodeInRenderParams()
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::UpdateAncestorDisplayNodeInRenderParams surfaceParams is null");
        return;
    }
    surfaceParams->SetAncestorDisplayNode(ancestorDisplayNode_);
    surfaceParams->SetNeedSync(true);
}

void RSSurfaceRenderNode::SetUifirstChildrenDirtyRectParam(RectI rect)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetUifirstChildrenDirtyRectParam(rect);
        AddToPendingSyncList();
    }
}

void RSSurfaceRenderNode::SetUifirstNodeEnableParam(MultiThreadCacheType b)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetUifirstNodeEnableParam(b);
        AddToPendingSyncList();
    }
}

void RSSurfaceRenderNode::SetIsParentUifirstNodeEnableParam(bool b)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (stagingSurfaceParams) {
        stagingSurfaceParams->SetIsParentUifirstNodeEnableParam(b);
        AddToPendingSyncList();
    }
}

} // namespace Rosen
} // namespace OHOS
