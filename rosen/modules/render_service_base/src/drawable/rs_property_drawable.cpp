/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable.h"

#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shape_base.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
constexpr int TRACE_LEVEL_TWO = 2;
void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    std::swap(drawCmdList_, stagingDrawCmdList_);
    propertyDescription_ = stagingPropertyDescription_;
    stagingPropertyDescription_.clear();
    needSync_ = false;
}

void RSPropertyDrawable::OnPurge()
{
    if (drawCmdList_) {
        drawCmdList_->Purge();
    }
}

void RSPropertyDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSPROPERTYDRAWABLE);
#endif
    if (drawCmdList_) {
        drawCmdList_->Playback(*canvas);
    }
    if (!propertyDescription_.empty()) {
        RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "RSPropertyDrawable:: %s, bounds:%s",
            propertyDescription_.c_str(), rect->ToString().c_str());
    }
}

// ============================================================================
// Updater
RSPropertyDrawCmdListUpdater::RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target)
    : target_(target)
{
    // PLANNING: use RSRenderNode to determine the correct recording canvas size
    recordingCanvas_ = ExtendRecordingCanvas::Obtain(10, 10, false); // width 10, height 10
}

RSPropertyDrawCmdListUpdater::~RSPropertyDrawCmdListUpdater()
{
    if (recordingCanvas_ && target_) {
        target_->stagingDrawCmdList_ = RSSimpleDrawCmdList::CreateFromDrawCmdList(recordingCanvas_->GetDrawCmdList());
        target_->needSync_ = true;
        ExtendRecordingCanvas::Recycle(recordingCanvas_);
        recordingCanvas_.reset();
        target_ = nullptr;
    } else {
        ROSEN_LOGE("Update failed, recording canvas is null!");
    }
}

const std::unique_ptr<ExtendRecordingCanvas>& RSPropertyDrawCmdListUpdater::GetRecordingCanvas() const
{
    return recordingCanvas_;
}

// ============================================================================
RSDrawable::Ptr RSFrameOffsetDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSFrameOffsetDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSFrameOffsetDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->Translate(frameOffsetX, frameOffsetY);
    return true;
}

// ============================================================================
RSDrawable::Ptr RSClipToBoundsDrawable::OnGenerate(const RSRenderNode& node)
{
    auto ret = std::make_shared<RSClipToBoundsDrawable>();
    ret->OnUpdate(node);
#ifdef USE_PRIMITIVE
    ret->OnPrimitiveSync();
#else
    ret->OnSync();
#endif
    return std::move(ret);
};

bool RSClipToBoundsDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    stagingGeContainer_ = nullptr;
    if (properties.GetClipToShape()) {
        stagingType_ = RSClipToBoundsType::CLIP_RRECT;
        const auto& shape = properties.GetClipShape();
        auto bounds = properties.GetBoundsRect();
        RRect rrect(RectT<float>(bounds.left_, bounds.top_,
            shape->rect_.width_, shape->rect_.height_), Vector4f(shape->radius_[0][0], shape->radius_[1][0],
            shape->radius_[2][0], shape->radius_[3][0]));
        stagingClipRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(rrect);
        stagingIsClipRRectOptimization_ = properties.NeedCornerOptimization();
    } else if (properties.GetClipBounds() != nullptr) {
        stagingType_ = RSClipToBoundsType::CLIP_PATH;
        stagingDrawingPath_ = properties.GetClipBounds()->GetDrawingPath();
    } else if (auto sdfShape = RSPropertyDrawableUtils::GetResolvedSDFShape(properties)) {
        stagingType_ = RSClipToBoundsType::CLIP_SDF;
        std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = sdfShape->GenerateGEVisualEffect();
        std::shared_ptr<Drawing::GEShaderShape> geShape =
            geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
        auto geFilter = std::make_shared<Drawing::GEVisualEffect>(
            Drawing::GE_SHADER_SDF_CLIP, Drawing::DrawingPaintType::BRUSH);
        geFilter->SetParam(Drawing::GE_SHADER_SDF_CLIP_SHAPE, geShape);
        stagingGeContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
        stagingGeContainer_->AddToChainedFilter(geFilter);
        auto bounds = properties.GetBoundsRect();
        auto shapeRect = sdfShape->GetTransformDrawRect();
        stagingSdfDrawRect_ = RSPropertyDrawableUtils::Rect2DrawingRect(
            (bounds == node.CalcBoundingBox() && !shapeRect.IsEmpty()) ? shapeRect : node.CalcBoundingBox());
    } else if (properties.GetClipToRRect()) {
        stagingType_ = RSClipToBoundsType::CLIP_RRECT;
        stagingClipRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetClipRRect());
        stagingIsClipRRectOptimization_ = properties.NeedCornerOptimization();
    } else if (!properties.GetCornerRadius().IsZero()) {
        stagingType_ = RSClipToBoundsType::CLIP_RRECT;
        stagingClipRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect());
        stagingIsClipRRectOptimization_ = properties.NeedCornerOptimization();
    } else if (node.GetType() == RSRenderNodeType::SURFACE_NODE && RSSystemProperties::GetCacheEnabledForRotation() &&
        node.ReinterpretCastTo<RSSurfaceRenderNode>()->IsAppWindow()) {
        stagingType_ = RSClipToBoundsType::CLIP_IRECT;
        stagingBoundsRect_ = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    } else {
        stagingType_ = RSClipToBoundsType::CLIP_RECT;
        stagingBoundsRect_ = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    }
    stagingNodeId_ = node.GetId();
    needSync_ = true;
    return true;
}

void RSClipToBoundsDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawingPath_ = stagingDrawingPath_;
    clipRRect_ = stagingClipRRect_;
    boundsRect_ = stagingBoundsRect_;
    type_ = stagingType_;
    nodeId_ = stagingNodeId_;
    isClipRRectOptimization_ = stagingIsClipRRectOptimization_;
    geContainer_ = std::move(stagingGeContainer_);
    needSync_ = false;
    sdfDrawRect_ = stagingSdfDrawRect_;
}

void RSClipToBoundsDrawable::OnDraw(Drawing::Canvas *canvas, const Drawing::Rect *rect) const
{
    switch (type_) {
        case RSClipToBoundsType::CLIP_PATH: {
            canvas->ClipPath(drawingPath_, Drawing::ClipOp::INTERSECT, true);
            break;
        }
        case RSClipToBoundsType::CLIP_RRECT: {
            if (RSSystemProperties::GetClipRRectOptimizationEnabled() || isClipRRectOptimization_) {
                RS_TRACE_NAME_FMT("RSClipToBoundsDrawable ClipRRectOptimization NodeId[%llu]", nodeId_);
                auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
                auto matrix = canvas->GetTotalMatrix();
                canvas->ResetMatrix();
                Drawing::RectF absRect;
                matrix.MapRect(absRect, clipRRect_.GetRect());
                canvas->ClipRect(absRect.RoundOut(), Drawing::ClipOp::INTERSECT, false);
                canvas->SetMatrix(matrix);
                paintFilterCanvas->ClipRRectOptimization(clipRRect_);
            } else {
                canvas->ClipRoundRect(clipRRect_, Drawing::ClipOp::INTERSECT, true);
            }
            break;
        }
        case RSClipToBoundsType::CLIP_IRECT: {
            canvas->ClipIRect(Drawing::RectI(
                static_cast<int>(boundsRect_.GetLeft()),
                static_cast<int>(boundsRect_.GetTop()),
                static_cast<int>(boundsRect_.GetRight()),
                static_cast<int>(boundsRect_.GetBottom())), Drawing::ClipOp::INTERSECT);
            break;
        }
        case RSClipToBoundsType::CLIP_RECT: {
            canvas->ClipRect(boundsRect_, Drawing::ClipOp::INTERSECT, false);
            break;
        }
        case RSClipToBoundsType::CLIP_SDF: { // add lambda by sdfClip
            bool invalid = !geContainer_ || rect == nullptr;
            if (invalid) {
                ROSEN_LOGE("Clip SDF failed, geContainer or rect is nullptr");
                return;
            }
            geContainer_->SetGeometry(canvas->GetTotalMatrix(), *rect, *rect, rect->GetWidth(), rect->GetHeight());
            Drawing::Rect rectRelative { sdfDrawRect_ };
            rectRelative.MakeOutset(1, 1);
            RSPaintFilterCanvas::DrawFunc customFunc = [geContainer = geContainer_,
                rect = rectRelative](Drawing::Canvas& canvas) {
                auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
                geRender->DrawShaderEffect(canvas, *geContainer, rect);
            };
            auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
            Drawing::SaveLayerOps slo(&sdfDrawRect_, nullptr);
            paintFilterCanvas->SaveLayer(slo);
            paintFilterCanvas->CustomSaveLayer(customFunc);
            break;
        }
        default:
            break;
    }
}

RSDrawable::Ptr RSClipToFrameDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSClipToFrameDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.GetClipToFrame()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->ClipRect(
        RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetFrameRect()), Drawing::ClipOp::INTERSECT, false);
    return true;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
