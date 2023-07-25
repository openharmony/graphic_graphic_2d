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

#include "pipeline/rs_render_node.h"

#include <algorithm>
#include <mutex>
#include <set>

#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::set<RSModifierType> GROUPABLE_ANIMATION_TYPE = {
    RSModifierType::ALPHA,
    RSModifierType::ROTATION,
    RSModifierType::SCALE,
};
const std::set<RSModifierType> CACHEABLE_ANIMATION_TYPE = {
    RSModifierType::ALPHA,
    RSModifierType::ROTATION,
    RSModifierType::BOUNDS,
    RSModifierType::FRAME,
};
// Only enable filter cache when uni-render is enabled and filter cache is enabled
const bool FILTER_CACHE_ENABLED = RSSystemProperties::GetFilterCacheEnabled() && RSUniRenderJudgement::IsUniRender();
}

RSRenderNode::RSRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSBaseRenderNode(id, context) {}

RSRenderNode::~RSRenderNode()
{
    if (fallbackAnimationOnDestroy_) {
        FallbackAnimationsToRoot();
    }
    if (clearCacheSurfaceFunc_ && (cacheSurface_ || cacheCompletedSurface_)) {
        clearCacheSurfaceFunc_(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

void RSRenderNode::FallbackAnimationsToRoot()
{
    if (animationManager_.animations_.empty()) {
        return;
    }

    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return;
    }
    auto target = context->GetNodeMap().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }
    context->RegisterAnimatingRenderNode(target);

    for (auto& [unused, animation] : animationManager_.animations_) {
        animation->Detach();
        // avoid infinite loop for fallback animation
        animation->SetRepeatCount(1);
        target->animationManager_.AddAnimation(std::move(animation));
    }
    animationManager_.animations_.clear();
}

std::pair<bool, bool> RSRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp, IsOnTheTree());
}

FrameRateRange RSRenderNode::GetRSFrameRateRange()
{
    if (rsRange_.IsValid()) {
        return rsRange_;
    }
    rsRange_ = animationManager_.GetFrameRateRangeFromRSAnimations();
    return rsRange_;
}

void RSRenderNode::ResetRSFrameRateRange()
{
    rsRange_.Reset();
}

void RSRenderNode::ResetUIFrameRateRange()
{
    uiRange_.Reset();
}

bool RSRenderNode::Update(
    RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, std::optional<RectI> clipRect)
{
    // no need to update invisible nodes
    if (!ShouldPaint() && !isLastVisible_) {
        SetClean();
        renderProperties_.ResetDirty();
        return false;
    }
    // [planning] surfaceNode use frame instead
#ifndef USE_ROSEN_DRAWING
    std::optional<SkPoint> offset;
    if (parent != nullptr && !IsInstanceOf<RSSurfaceRenderNode>()) {
        offset = SkPoint { parent->GetFrameOffsetX(), parent->GetFrameOffsetY() };
    }
#else
    std::optional<Drawing::Point> offset;
    if (parent != nullptr && !IsInstanceOf<RSSurfaceRenderNode>()) {
        Drawing::Point offset(parent->GetFrameOffsetX(), parent->GetFrameOffsetY());
    }
#endif
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty, offset, GetContextClipRegion());
    if ((IsDirty() || dirty) && drawCmdModifiers_.count(RSModifierType::GEOMETRYTRANS)) {
        RSModifierContext context = { GetMutableRenderProperties() };
        for (auto& modifier : drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
            modifier->Apply(context);
        }
    }
    isDirtyRegionUpdated_ = false;
    isLastVisible_ = ShouldPaint();
    renderProperties_.ResetDirty();

#ifndef USE_ROSEN_DRAWING
    // Note:
    // 1. cache manager will use dirty region to update cache validity, background filter cache manager should use
    // 'dirty region of all the nodes drawn before this node', and foreground filter cache manager should use 'dirty
    // region of all the nodes drawn before this node, this node, and the children of this node'
    // 2. Filter must be valid when filter cache manager is valid, we make sure that in RSRenderNode::ApplyModifiers().
    UpdateFilterCacheWithDirty(dirtyManager, false);
#endif

    UpdateDirtyRegion(dirtyManager, dirty, clipRect);
    return dirty;
}

RSProperties& RSRenderNode::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderNode::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderNode::UpdateDirtyRegion(
    RSDirtyRegionManager& dirtyManager, bool geoDirty, std::optional<RectI> clipRect)
{
    if (!IsDirty() && !geoDirty) {
        return;
    }
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        // while node absRect not change and other content not change, return directly for not generate dirty region
        if (!geometryChangeNotPerceived_ && !geoDirty) {
            return;
        }
        geometryChangeNotPerceived_ = false;
    }
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    // merge old dirty if switch to invisible
    if (!ShouldPaint() && isLastVisible_) {
        ROSEN_LOGD("RSRenderNode:: id %" PRIu64 " UpdateDirtyRegion visible->invisible", GetId());
    } else {
        RectI drawRegion;
        RectI shadowRect;
        auto dirtyRect = renderProperties_.GetDirtyRect(drawRegion);
        if (renderProperties_.IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                const RectF absBounds = {0, 0, renderProperties_.GetBoundsWidth(), renderProperties_.GetBoundsHeight()};
                RRect absClipRRect = RRect(absBounds, renderProperties_.GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, renderProperties_, &absClipRRect);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, renderProperties_);
            }
            if (!shadowRect.IsEmpty()) {
                dirtyRect = dirtyRect.JoinRect(shadowRect);
            }
        }

        if (renderProperties_.IsPixelStretchValid() || renderProperties_.IsPixelStretchPercentValid()) {
            auto stretchDirtyRect = renderProperties_.GetPixelStretchDirtyRect();
            dirtyRect = dirtyRect.JoinRect(stretchDirtyRect);
        }

        if (clipRect.has_value()) {
            dirtyRect = dirtyRect.IntersectRect(*clipRect);
        }
        oldDirty_ = dirtyRect;
        oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
        // filter invalid dirtyrect
        if (!dirtyRect.IsEmpty()) {
            dirtyManager.MergeDirtyRect(dirtyRect);
            isDirtyRegionUpdated_ = true;
            // save types of dirty region of target dirty manager for dfx
            if (dirtyManager.IsTargetForDfx() &&
                (GetType() == RSRenderNodeType::CANVAS_NODE || GetType() == RSRenderNodeType::SURFACE_NODE)) {
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OVERLAY_RECT, drawRegion);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::SHADOW_RECT, shadowRect);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::PREPARE_CLIP_RECT, clipRect.value_or(RectI()));
            }
        }
    }

    SetClean();
}

bool RSRenderNode::IsDirty() const
{
    return RSBaseRenderNode::IsDirty() || renderProperties_.IsDirty();
}

bool RSRenderNode::IsContentDirty() const
{
    // Considering renderNode, it should consider both basenode's case and its properties
    return RSBaseRenderNode::IsContentDirty() || renderProperties_.IsContentDirty();
}

void RSRenderNode::UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled)
{
    auto dirtyRect = renderProperties_.GetDirtyRect();
    // should judge if there's any child out of parent
    if (!isPartialRenderEnabled || HasChildrenOutOfRect()) {
        isRenderUpdateIgnored_ = false;
    } else if (dirtyRegion.IsEmpty() || dirtyRect.IsEmpty()) {
        isRenderUpdateIgnored_ = true;
    } else {
        RectI intersectRect = dirtyRegion.IntersectRect(dirtyRect);
        isRenderUpdateIgnored_ = intersectRect.IsEmpty();
    }
}

void RSRenderNode::UpdateParentChildrenRect(std::shared_ptr<RSBaseRenderNode> parentNode) const
{
    auto renderParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(parentNode);
    if (renderParent) {
        // accumulate current node's all children region(including itself)
        // apply oldDirty_ as node's real region(including overlay and shadow)
        RectI accumulatedRect = GetChildrenRect().JoinRect(oldDirty_);
        renderParent->UpdateChildrenRect(accumulatedRect);
        // check each child is inside of parent
        if (!accumulatedRect.IsInsideOf(renderParent->GetOldDirty())) {
            renderParent->UpdateChildrenOutOfRectFlag(true);
        }
    }
}

bool RSRenderNode::IsFilterCacheValid() const
{
    if (!FILTER_CACHE_ENABLED) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    // background filter
    auto& bgManager = renderProperties_.GetFilterCacheManager(false);
    // foreground filter
    auto& frManager = renderProperties_.GetFilterCacheManager(true);
    if ((bgManager && bgManager->IsCacheValid()) || (frManager && frManager->IsCacheValid())) {
        return true;
    }
#endif
    return false;
}

void RSRenderNode::UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground) const
{
#ifndef USE_ROSEN_DRAWING
    if (!FILTER_CACHE_ENABLED) {
        return;
    }
    auto& properties = GetRenderProperties();
    auto& manager = properties.GetFilterCacheManager(isForeground);
    if (manager == nullptr) {
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    auto& cachedImageRegion = manager->GetCachedImageRegion();
    RectI cachedImageRect = RectI(cachedImageRegion.x(), cachedImageRegion.y(), cachedImageRegion.width(),
        cachedImageRegion.height());
    auto isCachedImageRegionIntersectedWithDirtyRegion =
        cachedImageRect.IntersectRect(dirtyManager.GetIntersectedVisitedDirtyRect(geoPtr->GetAbsRect()));
    manager->UpdateCacheStateWithDirtyRegion(isCachedImageRegionIntersectedWithDirtyRegion);
#endif
}

void RSRenderNode::RenderTraceDebug() const
{
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(GetId(), GetRenderProperties());
    }
}

void RSRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    renderNodeSaveCount_ = canvas.Save();
#else
    renderNodeSaveCount_ = canvas.SaveAllStatus();
#endif
    auto boundsGeo = std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
#ifndef USE_ROSEN_DRAWING
        canvas.concat(boundsGeo->GetMatrix());
#else
        canvas.ConcatMatrix(boundsGeo->GetMatrix());
#endif
    }
    auto alpha = renderProperties_.GetAlpha();
    if (alpha < 1.f) {
        if ((GetChildrenCount() == 0) || !(GetRenderProperties().GetAlphaOffscreen() || IsForcedDrawInGroup())) {
            canvas.MultiplyAlpha(alpha);
        } else {
#ifndef USE_ROSEN_DRAWING
            auto rect = RSPropertiesPainter::Rect2SkRect(GetRenderProperties().GetBoundsRect());
            canvas.saveLayerAlpha(&rect, std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
#else
            auto rect = RSPropertiesPainter::Rect2DrawingRect(GetRenderProperties().GetBoundsRect());
            Drawing::Brush brush;
            brush.SetAlphaF(std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
            Drawing::SaveLayerOps slr(&rect, &brush);
            canvas.SaveLayer(slr);
#endif
        }
    }
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    GetMutableRenderProperties().ResetBounds();
    canvas.RestoreStatus(renderNodeSaveCount_);
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionAfterChildren(canvas);
}

void RSRenderNode::AddModifier(const std::shared_ptr<RSRenderModifier> modifier)
{
    if (!modifier) {
        return;
    }
    if (modifier->GetType() == RSModifierType::BOUNDS || modifier->GetType() == RSModifierType::FRAME) {
        AddGeometryModifier(modifier);
    } else if (modifier->GetType() < RSModifierType::CUSTOM) {
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
    } else {
        drawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
    }
    modifier->GetProperty()->Attach(ReinterpretCastTo<RSRenderNode>());
    SetDirty();
}

void RSRenderNode::AddGeometryModifier(const std::shared_ptr<RSRenderModifier> modifier)
{
    // bounds and frame modifiers must be unique
    if (modifier->GetType() == RSModifierType::BOUNDS) {
        if (boundsModifier_ == nullptr) {
            boundsModifier_ = modifier;
        }
        modifiers_.emplace(modifier->GetPropertyId(), boundsModifier_);
    }

    if (modifier->GetType() == RSModifierType::FRAME) {
        if (frameModifier_ == nullptr) {
            frameModifier_ = modifier;
        }
        modifiers_.emplace(modifier->GetPropertyId(), frameModifier_);
    }
}

void RSRenderNode::RemoveModifier(const PropertyId& id)
{
    SetDirty();
    auto it = modifiers_.find(id);
    if (it != modifiers_.end()) {
        if (it->second) {
            AddDirtyType(it->second->GetType());
        }
        modifiers_.erase(it);
        return;
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if([id](const auto& modifier) -> bool {
            return modifier ? modifier->GetPropertyId() == id : true;
        });
    }
}

void RSRenderNode::ApplyModifiers()
{
    if (!RSBaseRenderNode::IsDirty() || dirtyTypes_.empty()) {
        return;
    }
    RSModifierContext context = { renderProperties_ };
    for (auto type : dirtyTypes_) {
        renderProperties_.ResetProperty(type);
    }
    auto dirtyStatus = renderProperties_.GetDirtyStatus();
    renderProperties_.Reset();
    for (auto& [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->Apply(context);
        }
    }
    renderProperties_.SetDirtyStatus(dirtyStatus);
    OnApplyModifiers();
    UpdateDrawRegion();
    dirtyTypes_.clear();

#ifndef USE_ROSEN_DRAWING
    if (!FILTER_CACHE_ENABLED) {
        return;
    }
    // Create or release filter cache manager on demand, update cache state with filter hash.
    renderProperties_.CreateFilterCacheManagerIfNeed();
#endif
}

void RSRenderNode::UpdateDrawRegion()
{
    RSModifierContext context = { GetMutableRenderProperties() };
    RectF joinRect = RectF();
    if (drawRegion_) {
        joinRect = joinRect.JoinRect(*(drawRegion_));
    }
    for (auto& iterator : drawCmdModifiers_) {
        if (iterator.first > RSModifierType::NODE_MODIFIER) {
            continue;
        }
        for (auto& modifier : iterator.second) {
            auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(modifier);
            if (!drawCmdModifier) {
                continue;
            }
#ifndef USE_ROSEN_DRAWING
            auto recording = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(
#else
            auto recording = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(
#endif
                    drawCmdModifier->GetProperty())->Get();
            auto recordingRect = RectF(0, 0, recording->GetWidth(), recording->GetHeight());
            joinRect = recordingRect.IsEmpty() ? joinRect : joinRect.JoinRect(recordingRect);
        }
    }
    context.property_.SetDrawRegion(std::make_shared<RectF>(joinRect));
}

#ifndef USE_ROSEN_DRAWING
void RSRenderNode::UpdateEffectRegion(std::optional<SkPath>& region) const
#else
void RSRenderNode::UpdateEffectRegion(std::optional<Drawing::Path>& region) const
#endif
{
    if (!region.has_value()) {
        return;
    }
    const auto& property = GetRenderProperties();
    if (!property.GetUseEffect()) {
        return;
    }
    auto& effectPath = region.value();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        return;
    }

#ifndef USE_ROSEN_DRAWING
    SkPath clipPath;
    if (property.GetClipBounds() != nullptr) {
        clipPath = property.GetClipBounds()->GetSkiaPath();
    } else {
        auto rrect = RSPropertiesPainter::RRect2SkRRect(property.GetRRect());
        clipPath.addRRect(rrect);
    }

    // accumulate children clip path, with matrix
    effectPath.addPath(clipPath, geoPtr->GetAbsMatrix());
#else
    Drawing::Path clipPath;
    if (property.GetClipBounds() != nullptr) {
        clipPath = property.GetClipBounds()->GetDrawingPath();
    } else {
        auto rrect = RSPropertiesPainter::RRect2DrawingRRect(property.GetRRect());
        clipPath.AddRoundRect(rrect);
    }

    // accumulate children clip path, with matrix
    effectPath.AddPath(clipPath, geoPtr->GetAbsMatrix());
#endif
}

std::shared_ptr<RSRenderModifier> RSRenderNode::GetModifier(const PropertyId& id)
{
    if (modifiers_.count(id)) {
        return modifiers_[id];
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        auto it = std::find_if(modifiers.begin(), modifiers.end(),
            [id](const auto& modifier) -> bool { return modifier->GetPropertyId() == id; });
        if (it != modifiers.end()) {
            return *it;
        }
    }
    return nullptr;
}

void RSRenderNode::FilterModifiersByPid(pid_t pid)
{
    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    EraseIf(modifiers_, [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });

    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if(
            [pid](const auto& it) -> bool { return ExtractPid(it->GetPropertyId()) == pid; });
    }
}

bool RSRenderNode::ShouldPaint() const
{
    // node should be painted if either it is visible or it has disappearing transition animation, but only when its
    // alpha is not zero
    return (renderProperties_.GetVisible() || HasDisappearingTransition(false)) &&
           (renderProperties_.GetAlpha() > 0.0f);
}

void RSRenderNode::SetSharedTransitionParam(const std::optional<SharedTransitionParam>&& sharedTransitionParam)
{
    if (!sharedTransitionParam_.has_value() && !sharedTransitionParam.has_value()) {
        // both are empty, do nothing
        return;
    }
    sharedTransitionParam_ = sharedTransitionParam;
    SetDirty();
}

const std::optional<RSRenderNode::SharedTransitionParam>& RSRenderNode::GetSharedTransitionParam() const
{
    return sharedTransitionParam_;
}

void RSRenderNode::SetGlobalAlpha(float alpha)
{
    if (globalAlpha_ == alpha) {
        return;
    }
    if ((ROSEN_EQ(globalAlpha_, 1.0f) && alpha != 1.0f) ||
        (ROSEN_EQ(alpha, 1.0f) && globalAlpha_ != 1.0f)) {
        OnAlphaChanged();
    }
    globalAlpha_ = alpha;
}

float RSRenderNode::GetGlobalAlpha() const
{
    return globalAlpha_;
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
void RSRenderNode::InitCacheSurface(GrRecordingContext* grContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
#else
void RSRenderNode::InitCacheSurface(GrContext* grContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
#endif
#else
void RSRenderNode::InitCacheSurface(Drawing::GPUContext* gpuContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
#endif
{
    if (func) {
        cacheSurfaceThreadIndex_ = threadIndex;
        if (!clearCacheSurfaceFunc_) {
            clearCacheSurfaceFunc_ = func;
        }
        if (cacheSurface_) {
            func(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
            ClearCacheSurface();
        }
    } else {
        cacheSurface_ = nullptr;
    }
    auto cacheType = GetCacheType();
    float width = 0.0f, height = 0.0f;
    Vector2f size = GetOptionalBufferSize();
    boundsWidth_ = size.x_;
    boundsHeight_ = size.y_;
    if (cacheType == CacheType::ANIMATE_PROPERTY &&
        renderProperties_.IsShadowValid() && !renderProperties_.IsSpherizeValid()) {
        const RectF boundsRect = renderProperties_.GetBoundsRect();
        RRect rrect = RRect(boundsRect, {0, 0, 0, 0});
        RectI shadowRect;
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, renderProperties_, &rrect, false);
        width = shadowRect.GetWidth();
        height = shadowRect.GetHeight();
        shadowRectOffsetX_ = -shadowRect.GetLeft();
        shadowRectOffsetY_ = -shadowRect.GetTop();
    } else {
        width = boundsWidth_;
        height = boundsHeight_;
    }
#ifndef USE_ROSEN_DRAWING
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    if (grContext == nullptr) {
        if (func) {
            func(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        return;
    }
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);
#else
    cacheSurface_ = SkSurface::MakeRasterN32Premul(width, height);
#endif
#else // USE_ROSEN_DRAWING
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(width, height, format);
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    if (gpuContext == nullptr) {
        if (func) {
            func(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        return;
    }
    Drawing::Image image;
    image.BuildFromBitmap(*gpuContext, bitmap);
    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(image);
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = surface;
#else
    cacheSurface_ = std::make_shared<Drawing::Surface>();
    cacheSurface_->Bind(bitmap);
#endif
#endif // USE_ROSEN_DRAWING
}

Vector2f RSRenderNode::GetOptionalBufferSize() const
{
    const auto& modifier = boundsModifier_ ? boundsModifier_ : frameModifier_;
    if (!modifier) {
        return {0.0f, 0.0f};
    }
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(modifier->GetProperty());
    auto vector4f = renderProperty->Get();
    // bounds vector4f: x y z w -> left top width height
    return { vector4f.z_, vector4f.w_ };
}

#ifndef USE_ROSEN_DRAWING
void RSRenderNode::DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    auto cacheType = GetCacheType();
    canvas.save();
    Vector2f size = GetOptionalBufferSize();
    float scaleX = size.x_ / boundsWidth_;
    float scaleY = size.y_ / boundsHeight_;
    canvas.scale(scaleX, scaleY);
    SkPaint paint;
#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
    if (isUIFirst) {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (!cacheCompletedBackendTexture_.isValid()) {
            RS_LOGE("invalid grBackendTexture_");
            canvas.restore();
            return;
        }
        auto image = SkImage::MakeFromTexture(canvas.recordingContext(), cacheCompletedBackendTexture_,
            kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
        if (image == nullptr) {
            RS_LOGE("make Image failed");
            canvas.restore();
            return;
        }
        canvas.drawImage(image, -shadowRectOffsetX_ * scaleX, -shadowRectOffsetY_ * scaleY);
        canvas.restore();
        return;
    }
#endif
    auto surface = GetCompletedCacheSurface(threadIndex, isUIFirst);
    if (surface == nullptr || (boundsWidth_ == 0 || boundsHeight_ == 0)) {
        RS_LOGE("invalid complete cache surface");
        return;
    }
    if (cacheType == CacheType::ANIMATE_PROPERTY && renderProperties_.IsShadowValid()) {
        surface->draw(&canvas, -shadowRectOffsetX_ * scaleX, -shadowRectOffsetY_ * scaleY, &paint);
    } else {
        surface->draw(&canvas, 0.0, 0.0, &paint);
    }
    canvas.restore();
}
#else
void RSRenderNode::DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    auto cacheType = GetCacheType();
    canvas.Save();
    Vector2f size = GetOptionalBufferSize();
    float scaleX = size.x_ / boundsWidth_;
    float scaleY = size.y_ / boundsHeight_;
    canvas.Scale(scaleX, scaleY);
#if defined(RS_ENABLE_GL)
    if (isUIFirst) {
        RS_LOGE("[%s:%d] Drawing is not supported", __func__, __LINE__);
        return;
    }
#endif
    auto surface = GetCompletedCacheSurface(threadIndex, isUIFirst);
    if (surface == nullptr || (boundsWidth_ == 0 || boundsHeight_ == 0)) {
        RS_LOGE("invalid complete cache surface");
        canvas.Restore();
        return;
    }
    auto image = surface->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("invalid complete cache image");
        canvas.Restore();
        return;
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    if (cacheType == CacheType::ANIMATE_PROPERTY && renderProperties_.IsShadowValid()) {
        canvas.DrawImage(*image, -shadowRectOffsetX_ * scaleX,
            -shadowRectOffsetY_ * scaleY, Drawing::SamplingOptions());
    } else {
        canvas.DrawImage(*image, 0.0, 0.0, Drawing::SamplingOptions());
    }
    canvas.DetachBrush();
    canvas.Restore();
}
#endif

#ifdef RS_ENABLE_GL
void RSRenderNode::UpdateBackendTexture()
{
#ifndef USE_ROSEN_DRAWING
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return;
    }
    cacheBackendTexture_
        = cacheSurface_->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
#else
    RS_LOGE("[%s:%d] Drawing is not supported", __func__, __LINE__);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSRenderNode::GetCompletedCacheSurface(uint32_t threadIndex, bool isUIFirst)
#else
std::shared_ptr<Drawing::Surface> RSRenderNode::GetCompletedCacheSurface(uint32_t threadIndex, bool isUIFirst)
#endif
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (isUIFirst || cacheSurfaceThreadIndex_ == threadIndex || !cacheCompletedSurface_) {
            return cacheCompletedSurface_;
        }
    }

    // freeze cache scene
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
    }
    ClearCacheSurface();
    return nullptr;
}

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> RSRenderNode::GetCacheSurface(uint32_t threadIndex, bool needCheckThread)
#else
    std::shared_ptr<Drawing::Surface> RSRenderNode::GetCacheSurface(uint32_t threadIndex, bool needCheckThread)
#endif
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (!needCheckThread || cacheSurfaceThreadIndex_ == threadIndex || !cacheSurface_) {
            return cacheSurface_;
        }
    }

    // freeze cache scene
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(cacheSurface_, cacheCompletedSurface_, cacheSurfaceThreadIndex_);
    }
    ClearCacheSurface();
    return nullptr;
}

void RSRenderNode::CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd)
{
    if (id <= 0 || GetType() != RSRenderNodeType::CANVAS_NODE) {
        return;
    }
    auto context = GetContext().lock();
    if (!context || !context->GetNodeMap().IsResidentProcessNode(GetId())) {
        return;
    }
    auto target = modifiers_.find(id);
    if (target == modifiers_.end() || !target->second) {
        return;
    }
    if (isAnimAdd) {
        if (GROUPABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, true);
        } else if (CACHEABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            hasCacheableAnim_ = true;
        }
        return;
    }
    bool hasGroupableAnim = false;
    hasCacheableAnim_ = false;
    for (auto& [_, animation] : animationManager_.animations_) {
        if (!animation || id == animation->GetPropertyId()) {
            continue;
        }
        auto itr = modifiers_.find(animation->GetPropertyId());
        if (itr == modifiers_.end() || !itr->second) {
            continue;
        }
        hasGroupableAnim = (hasGroupableAnim | GROUPABLE_ANIMATION_TYPE.count(itr->second->GetType()));
        hasCacheableAnim_ = (hasCacheableAnim_ | CACHEABLE_ANIMATION_TYPE.count(itr->second->GetType()));
    }
    MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, hasGroupableAnim);
}

bool RSRenderNode::IsForcedDrawInGroup() const
{
    return (nodeGroupType_ == NodeGroupType::GROUPED_BY_USER) && (renderProperties_.GetAlpha() < 1.f);
}

bool RSRenderNode::IsSuggestedDrawInGroup() const
{
    return nodeGroupType_ != NodeGroupType::NONE;
}

void RSRenderNode::MarkNodeGroup(NodeGroupType type, bool isNodeGroup)
{
    if (type >= nodeGroupType_) {
        nodeGroupType_ = isNodeGroup ? type : NodeGroupType::NONE;
        SetDirty();
    }
}

void RSRenderNode::CheckDrawingCacheType()
{
    if (nodeGroupType_ == NodeGroupType::NONE) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    } else if ((nodeGroupType_ == NodeGroupType::GROUPED_BY_USER) && (renderProperties_.GetAlpha() < 1.f)) {
        SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    } else {
        SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    }
}

RectI RSRenderNode::GetFilterRect() const
{
    auto& properties = GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (!geoPtr) {
        return {};
    }
    if (properties.GetClipBounds() != nullptr) {
#ifndef USE_ROSEN_DRAWING
        auto filterRect = properties.GetClipBounds()->GetSkiaPath().getBounds();
        auto absRect = geoPtr->GetAbsMatrix().mapRect(filterRect);
        return {absRect.x(), absRect.y(), absRect.width(), absRect.height()};
#else
        auto filterRect = properties.GetClipBounds()->GetDrawingPath().GetBounds();
        Drawing::Rect absRect;
        geoPtr->GetAbsMatrix().MapRect(absRect, filterRect);
        return {absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight()};
#endif
    } else {
        return geoPtr->GetAbsRect();
    }
}

void RSRenderNode::UpdateFilterCacheManagerWithCacheRegion(const std::optional<RectI>& clipRect) const
{
#ifndef USE_ROSEN_DRAWING
    if (!FILTER_CACHE_ENABLED) {
        return;
    }
    auto& renderProperties = GetRenderProperties();
    if (!renderProperties.NeedFilter()) {
        return;
    }
    auto filterRect = GetFilterRect();
    if (clipRect.has_value()) {
        filterRect.IntersectRect(*clipRect);
    }
    // background filter
    if (auto& manager = renderProperties.GetFilterCacheManager(false)) {
        manager->UpdateCacheStateWithFilterRegion(filterRect);
    }
    // foreground filter
    if (auto& manager = renderProperties.GetFilterCacheManager(true)) {
        manager->UpdateCacheStateWithFilterRegion(filterRect);
    }
#endif
}

void RSRenderNode::OnTreeStateChanged()
{
#ifndef USE_ROSEN_DRAWING
    if (!FILTER_CACHE_ENABLED) {
        return;
    }
    if (IsOnTheTree()) {
        GetMutableRenderProperties().CreateFilterCacheManagerIfNeed();
    } else {
        GetMutableRenderProperties().ResetFilterCacheManager();
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
