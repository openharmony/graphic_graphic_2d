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

#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "include/gpu/GrContext.h"

#include "command/rs_surface_node_command.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRenderNode::RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(config.id, context),
      RSSurfaceHandler(config.id),
      name_(config.name),
      nodeType_(config.nodeType),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    MemoryInfo info = {sizeof(*this), ExtractPid(config.id), MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(config.id, info);
}

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSSurfaceRenderNode(RSSurfaceRenderNodeConfig{id, "SurfaceNode"}, context)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetConsumer(const sptr<IConsumerSurface>& consumer)
{
    consumer_ = consumer;
}
#endif

static SkRect GetLocalClipBounds(const RSPaintFilterCanvas& canvas, SkIRect dstRect)
{
    if (dstRect.isEmpty()) {
        return SkRect::MakeEmpty();
    }

    SkMatrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas.getTotalMatrix().invert(&inverse))) {
        return SkRect::MakeEmpty();
    }
    SkRect bounds;
    SkRect r = SkRect::Make(dstRect);
    inverse.mapRect(&bounds, r);
    return bounds;
}

void RSSurfaceRenderNode::UpdateSrcRect(const RSPaintFilterCanvas& canvas, SkIRect dstRect)
{
    auto localClipRect = GetLocalClipBounds(canvas, dstRect);
    const RSProperties& properties = GetRenderProperties();
    RectI srcRect = {
        std::clamp<int>(localClipRect.left(), 0, properties.GetBoundsWidth()),
        std::clamp<int>(localClipRect.top(), 0, properties.GetBoundsHeight()),
        std::clamp<int>(localClipRect.width(), 0, properties.GetBoundsWidth() - localClipRect.left()),
        std::clamp<int>(localClipRect.height(), 0, properties.GetBoundsHeight() - localClipRect.top())
    };
    SetSrcRect(srcRect);
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
    renderNodeSaveCount_ = canvas.SaveCanvasAndAlpha();

    // apply intermediate properties from RT to canvas
    canvas.MultiplyAlpha(GetContextAlpha());
    canvas.concat(GetContextMatrix());
    auto clipRectFromRT = GetContextClipRegion();
    if (clipRectFromRT.width() > std::numeric_limits<float>::epsilon() &&
        clipRectFromRT.height() > std::numeric_limits<float>::epsilon()) {
        canvas.clipRect(clipRectFromRT);
    }

    // apply node properties to canvas
    const RSProperties& properties = GetRenderProperties();
    canvas.MultiplyAlpha(properties.GetAlpha());
    auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (currentGeoPtr != nullptr) {
        currentGeoPtr->UpdateByMatrixFromSelf();
        auto matrix = currentGeoPtr->GetMatrix();
        matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
        matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
        canvas.concat(matrix);
    }

    // clip by bounds
    canvas.clipRect(
        SkRect::MakeWH(std::floor(properties.GetBoundsWidth()), std::floor(properties.GetBoundsHeight())));

    // extract srcDest and dstRect from SkCanvas, localCLipBounds as SrcRect, deviceClipBounds as DstRect
    auto deviceClipRect = canvas.getDeviceClipBounds();
    UpdateSrcRect(canvas, deviceClipRect);
    RectI dstRect = { deviceClipRect.left(), deviceClipRect.top(), deviceClipRect.width(), deviceClipRect.height() };
    SetDstRect(dstRect);

    // save TotalMatrix and GlobalAlpha for compositor
    SetTotalMatrix(canvas.getTotalMatrix());
    SetGlobalAlpha(canvas.GetAlpha());
}

void RSSurfaceRenderNode::PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreCanvasAndAlpha(renderNodeSaveCount_);
}

void RSSurfaceRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender)
{
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
        for (auto& child : node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender);
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
}

void RSSurfaceRenderNode::ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node)
{
    for (auto& child : node->GetSortedChildren()) {
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
}

void RSSurfaceRenderNode::ResetParent()
{
    RSBaseRenderNode::ResetParent();

    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        ClearChildrenCache(shared_from_this());
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
    isNotifyUIBufferAvailable_.store(available);
}

void RSSurfaceRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
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

void RSSurfaceRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas)
{
    const auto& property = GetRenderProperties();
    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, canvas, &absClipRRect);

    if (!property.GetCornerRadius().IsZero()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas.clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }

    RSPropertiesPainter::DrawBackground(property, canvas);
    RSPropertiesPainter::DrawMask(property, canvas);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
    SetTotalMatrix(canvas.getTotalMatrix());
}

void RSSurfaceRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    const auto& property = GetRenderProperties();
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
}

void RSSurfaceRenderNode::SetContextBounds(const Vector4f bounds)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetBounds>(GetId(), bounds);
    SendCommandFromRT(command, GetId());
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

void RSSurfaceRenderNode::SetContextMatrix(const SkMatrix& matrix, bool sendMsg)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(GetId(), matrix);
    SendCommandFromRT(command, GetId());
}

const SkMatrix& RSSurfaceRenderNode::GetContextMatrix() const
{
    return contextMatrix_;
}

void RSSurfaceRenderNode::SetContextAlpha(float alpha, bool sendMsg)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), alpha);
    SendCommandFromRT(command, GetId());
}

float RSSurfaceRenderNode::GetContextAlpha() const
{
    return contextAlpha_;
}

void RSSurfaceRenderNode::SetContextClipRegion(SkRect clipRegion, bool sendMsg)
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command, GetId());
}

const SkRect& RSSurfaceRenderNode::GetContextClipRegion() const
{
    return contextClipRect_;
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
}

bool RSSurfaceRenderNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

ColorGamut RSSurfaceRenderNode::GetColorSpace() const
{
    return colorSpace_;
}
#endif

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (consumer_ != nullptr) {
        consumer_->SetDefaultWidthAndHeight(width, height);
    }
#endif
}

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
        std::lock_guard<std::mutex> lock(mutexUI_);
        callbackFromUI_ = callback;
    }
}

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %" PRIu64, GetId());
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->RegisterBufferAvailableListener(
            GetId(), [weakThis = weak_from_this()]() {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->NotifyRTBufferAvailable();
            }, true);
    }
}

void RSSurfaceRenderNode::NotifyRTBufferAvailable()
{
    // In RS, "isNotifyRTBufferAvailable_ = true" means buffer is ready and need to trigger ipc callback.
    // In RT, "isNotifyRTBufferAvailable_ = true" means RT know that RS have had available buffer
    // and ready to trigger "callbackForRenderThreadRefresh_" to "clip" on parent surface.
    isNotifyRTBufferAvailablePre_ = isNotifyRTBufferAvailable_;
    if (isNotifyRTBufferAvailable_) {
        return;
    }
    isNotifyRTBufferAvailable_ = true;

    if (callbackForRenderThreadRefresh_) {
        ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %" PRIu64 " RenderThread", GetId());
        callbackForRenderThreadRefresh_();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %" PRIu64 " RenderService", GetId());
            callbackFromRT_->OnBufferAvailable();
        }
        if (!callbackForRenderThreadRefresh_ && !callbackFromRT_) {
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
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %" PRIu64, GetId());
            callbackFromUI_->OnBufferAvailable();
        } else {
            isNotifyUIBufferAvailable_ = false;
        }
    }
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailable() const
{
    return isNotifyRTBufferAvailable_;
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailablePre() const
{
    return isNotifyRTBufferAvailablePre_;
}

bool RSSurfaceRenderNode::IsNotifyUIBufferAvailable() const
{
    return isNotifyUIBufferAvailable_;
}

void RSSurfaceRenderNode::SetCallbackForRenderThreadRefresh(std::function<void(void)> callback)
{
    callbackForRenderThreadRefresh_ = callback;
}

bool RSSurfaceRenderNode::NeedSetCallbackForRenderThreadRefresh()
{
    return (callbackForRenderThreadRefresh_ == nullptr);
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
        SetDirty();
        return true;
    }
    return false;
}

void RSSurfaceRenderNode::SetVisibleRegionRecursive(const Occlusion::Region& region,
                                                    VisibleData& visibleVec,
                                                    std::map<uint32_t, bool>& pidVisMap)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE || IsAbilityComponent()) {
        SetOcclusionVisible(true);
        return;
    }
    visibleRegion_ = region;
    bool vis = region.GetSize() > 0;
    if (vis) {
        visibleVec.emplace_back(GetId());
    }

    // collect visible changed pid
    if (qosPidCal_ && GetType() == RSRenderNodeType::SURFACE_NODE) {
        uint32_t tmpPid = ExtractPid(GetId());
        if (pidVisMap.find(tmpPid) != pidVisMap.end()) {
            pidVisMap[tmpPid] |= vis;
        } else {
            pidVisMap[tmpPid] = vis;
        }
    }

    SetOcclusionVisible(vis);
    for (auto& child : GetSortedChildren()) {
        if (auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)) {
            surface->SetVisibleRegionRecursive(region, visibleVec, pidVisMap);
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
    if (IsTransparent() || IsCurrentNodeInTransparentRegion(nodeRect)) {
        return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
    }
    return false;
}

bool RSSurfaceRenderNode::SubNodeNeedDraw(const RectI &r, PartialRenderType opDropType) const
{
    if (dirtyManager_ == nullptr) {
        return true;
    }
    if (r.IsEmpty()) {
        return true;
    }
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
    const ScreenRotation screenRotation, const bool isFocusWindow)
{
    Occlusion::Rect absRectR {absRect};
    Occlusion::Region oldOpaqueRegion { opaqueRegion_ };

    // The transparent region of surfaceNode should include shadow area
    Occlusion::Rect dirtyRect {GetOldDirty()};
    transparentRegion_ = Occlusion::Region{dirtyRect};

    if (IsTransparent()) {
        opaqueRegion_ = Occlusion::Region();
    } else {
        if (IsAppWindow() && HasContainerWindow()) {
            opaqueRegion_ = ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
        } else {
            auto corner = GetWindowCornerRadius();
            if (!corner.IsZero()) {
                opaqueRegion_ = SetCornerRadiusOpaqueRegion(absRect, std::ceil(corner.x_));
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
    Occlusion::Rect opaqueRect1{ absRect.left_ + containerOutRadius_,
        absRect.top_,
        absRect.GetRight() - containerOutRadius_,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + containerOutRadius_,
        absRect.GetRight(),
        absRect.GetBottom() - containerInnerRadius_};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);

    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerTitleHeight_ + containerBorderWidth_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerTitleHeight_ + containerBorderWidth_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_+ containerContentPadding_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerTitleHeight_ - containerBorderWidth_};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_,
                absRect.GetRight() - containerTitleHeight_ - containerBorderWidth_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerTitleHeight_ + containerBorderWidth_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
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
screen rotation state as top/left/botton/right has changed when screen rotated.
*/
Occlusion::Region RSSurfaceRenderNode::SetFocusedWindowOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation) const
{
    Occlusion::Region opaqueRegion;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_ + containerTitleHeight_ + containerInnerRadius_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.top_ + containerBorderWidth_ + containerTitleHeight_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerBorderWidth_ + containerTitleHeight_ + containerInnerRadius_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerBorderWidth_ + containerTitleHeight_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerTitleHeight_ - containerInnerRadius_};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_,
                absRect.GetBottom() - containerBorderWidth_ - containerTitleHeight_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_,
                absRect.GetRight() - containerBorderWidth_ - containerTitleHeight_ - containerInnerRadius_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.GetRight() - containerBorderWidth_ - containerTitleHeight_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_,
                absRect.top_ + containerBorderWidth_ + containerTitleHeight_ + containerInnerRadius_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerBorderWidth_ + containerContentPadding_ + containerInnerRadius_,
                absRect.top_ + containerBorderWidth_ + containerTitleHeight_,
                absRect.GetRight() - containerBorderWidth_ - containerContentPadding_ - containerInnerRadius_,
                absRect.GetBottom() - containerBorderWidth_ - containerContentPadding_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
    }
    return opaqueRegion;
}

Occlusion::Region RSSurfaceRenderNode::SetCornerRadiusOpaqueRegion(const RectI& absRect, float radius) const
{
    Occlusion::Rect opaqueRect1{ absRect.left_ + radius,
        absRect.top_,
        absRect.GetRight() - radius,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + radius,
        absRect.GetRight(),
        absRect.GetBottom() - radius};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);
    return opaqueRegion;
}

// [planning] Remove this after skia is upgraded, the clipRegion is supported
void RSSurfaceRenderNode::ResetChildrenFilterRects()
{
    childrenFilterRects_.clear();
}

void RSSurfaceRenderNode::UpdateChildrenFilterRects(const RectI& rect)
{
    if (!rect.IsEmpty()) {
        childrenFilterRects_.emplace_back(rect);
    }
}

const std::vector<RectI>& RSSurfaceRenderNode::GetChildrenNeedFilterRects() const
{
    return childrenFilterRects_;
}

// manage abilities' nodeid info
void RSSurfaceRenderNode::ResetAbilityNodeIds()
{
    abilityNodeIds_.clear();
}

void RSSurfaceRenderNode::UpdateAbilityNodeIds(NodeId id)
{
    abilityNodeIds_.emplace_back(id);
}

const std::vector<NodeId>& RSSurfaceRenderNode::GetAbilityNodeIds() const
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

std::vector<std::weak_ptr<RSSurfaceRenderNode>> RSSurfaceRenderNode::GetChildHardwareEnabledNodes() const
{
    return childHardwareEnabledNodes_;
}

void RSSurfaceRenderNode::SetLocalZOrder(float localZOrder)
{
    localZOrder_ = localZOrder;
}

float RSSurfaceRenderNode::GetLocalZOrder() const
{
    return localZOrder_;
}
} // namespace Rosen
} // namespace OHOS
