/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "property/rs_property_drawable_bounds_geometry.h"

#include <utility>

#include "include/core/SkMaskFilter.h"
#include "include/core/SkPoint3.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
} // namespace
namespace OHOS::Rosen {
bool RSBackgroundDrawable::forceBgAntiAlias_ = true;
// ============================================================================
// Bounds geometry
void RSBoundsGeometryDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.concat(node.GetRenderProperties().GetBoundsGeometry()->GetMatrix());
#else
    canvas.ConcatMatrix(node.GetRenderProperties().GetBoundsGeometry()->GetMatrix());
#endif
}
RSPropertyDrawable::DrawablePtr RSBoundsGeometryDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSBoundsGeometryDrawable>();
}

void RSClipBoundsDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // Planning: Generate() should calculate the draw op and cache it
    auto& properties = node.GetMutableRenderProperties();
#ifndef USE_ROSEN_DRAWING
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToRRect()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetClipRRect()));
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), true);
    } else {
        canvas.clipRect(RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect()));
    }
#else
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, false);
    }
#endif
}

RSPropertyDrawable::DrawablePtr RSClipBoundsDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSClipBoundsDrawable>();
}

// ============================================================================
// Border
RSPropertyDrawable::DrawablePtr RSBorderDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setAntiAlias(true);
    if (border->ApplyFillStyle(paint)) {
        return std::make_unique<RSBorderDRRectDrawable>(std::move(paint), properties);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(paint)) {
        return std::make_unique<RSBorderFourLineDrawable>(std::move(paint), properties);
    } else if (border->ApplyPathStyle(paint)) {
        return std::make_unique<RSBorderPathDrawable>(std::move(paint), properties);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(std::move(paint), properties);
    }
#else
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        return std::make_unique<RSBorderDRRectDrawable>(std::move(brush), std::move(pen), properties);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(pen)) {
        return std::make_unique<RSBorderFourLineDrawable>(std::move(brush), std::move(pen), properties);
    } else if (border->ApplyPathStyle(pen)) {
        return std::make_unique<RSBorderPathDrawable>(std::move(brush), std::move(pen), properties);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(std::move(brush), std::move(pen), properties);
    }
#endif
}
#ifndef USE_ROSEN_DRAWING
RSBorderDRRectDrawable::RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderDRRectDrawable::RSBorderDRRectDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

void RSBorderDRRectDrawable::OnBoundsChange(const RSProperties& properties)
{
#ifndef USE_ROSEN_DRAWING
    inner_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
#else
    inner_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
#endif
}
void RSBorderDRRectDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawDRRect(outer_, inner_, paint_);
#else
    canvas.AttachPen(pen_);
    canvas.AttachBrush(brush_);
    canvas.DrawNestedRoundRect(outer_, inner_);
    canvas.DetachPen();
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderFourLineDrawable::RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderFourLineDrawable::RSBorderFourLineDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

void RSBorderFourLineDrawable::OnBoundsChange(const RSProperties& properties)
{
    rect_ = properties.GetBoundsRect();
}
void RSBorderFourLineDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    node.GetMutableRenderProperties().GetBorder()->PaintFourLine(canvas, paint_, rect_);
#else
    node.GetMutableRenderProperties().GetBorder()->PaintFourLine(canvas, pen_, rect_);
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderPathDrawable::RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderPathDrawable::RSBorderPathDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

void RSBorderPathDrawable::OnBoundsChange(const RSProperties& properties)
{
    auto borderWidth = properties.GetBorder()->GetWidth();
    RRect rrect = properties.GetRRect();
    rrect.rect_.width_ -= borderWidth;
    rrect.rect_.height_ -= borderWidth;
    rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
#ifndef USE_ROSEN_DRAWING
    borderPath_.reset();
    borderPath_.addRRect(RSPropertiesPainter::RRect2SkRRect(rrect));
#else
    borderPath_.Reset();
    borderPath_.AddRoundRect(RSPropertiesPainter::RRect2DrawingRRect(rrect));
#endif
}
void RSBorderPathDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawPath(borderPath_, paint_);
#else
    canvas.AttachPen(pen_);
    canvas.AttachBrush(brush_);
    canvas.DrawPath(borderPath_);
    canvas.DetachPen();
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
#ifndef USE_ROSEN_DRAWING
    paint_.setStyle(SkPaint::Style::kStroke_Style);
#endif
    OnBoundsChange(properties);
}

void RSBorderFourLineRoundCornerDrawable::OnBoundsChange(const RSProperties& properties)
{
#ifndef USE_ROSEN_DRAWING
    innerRrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
#else
    innerRrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
#endif
}
void RSBorderFourLineRoundCornerDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    auto& properties = node.GetMutableRenderProperties();
    canvas.clipRRect(innerRrect_, SkClipOp::kDifference, true);
    properties.GetBorder()->PaintTopPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintRightPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintBottomPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintLeftPath(canvas, paint_, rrect_);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    auto& properties = node.GetMutableRenderProperties();
    canvas.ClipRoundRect(innerRrect_, Drawing::ClipOp::DIFFERENCE, true);
    properties.GetBorder()->PaintTopPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintRightPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintBottomPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintLeftPath(canvas, pen_, rrect_);
#endif
}

// ============================================================================
// Mask
std::unique_ptr<RSPropertyDrawable> RSMaskDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return nullptr;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        return nullptr;
    }
    if (mask->IsSvgMask()) {
        if (mask->GetSvgDom()) {
            return std::make_unique<RSSvgDomMaskDrawable>(mask);
        } else if (mask->GetSvgPicture()) {
            return std::make_unique<RSSvgPictureMaskDrawable>(mask);
        }
    } else if (mask->IsGradientMask()) {
        return std::make_unique<RSGradientMaskDrawable>(mask);
    } else if (mask->IsPathMask()) {
        return std::make_unique<RSPathMaskDrawable>(mask);
    }
    return nullptr;
}

RSMaskDrawable::RSMaskDrawable(std::shared_ptr<RSMask> mask) : mask_(std::move(mask))
{
#ifndef USE_ROSEN_DRAWING
    maskPaint_.setBlendMode(SkBlendMode::kSrcIn);
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskFilter_.setColorFilter(filter);
#else
    maskBrush_.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())));
    maskFilterBrush_.SetFilter(filter);
#endif
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgDomMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());

    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft + mask_->GetSvgX(), bounds.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        mask_->GetSvgDom()->render(&canvas);
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft() + mask_->GetSvgX(), bounds.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawSVGDOM(mask_->GetSvgDom());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgPictureMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft + mask_->GetSvgX(), bounds.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.drawPicture(mask_->GetSvgPicture());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft() + mask_->GetSvgX(), bounds.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawPicture(*mask_->GetSvgPicture());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSGradientMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft, bounds.fTop);
        SkRect rect = SkRect::MakeIWH(bounds.fRight - bounds.fLeft, bounds.fBottom - bounds.fTop);
        canvas.drawRect(rect, mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft(), bounds.GetTop());
        Drawing::Rect rect =
            Drawing::Rect(0, 0, bounds.GetRight() - bounds.GetLeft(), bounds.GetBottom() - bounds.GetTop());
        canvas.AttachBrush(mask_->GetMaskBrush());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSPathMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft, bounds.fTop);
        canvas.drawPath(mask_->GetMaskPath(), mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft(), bounds.GetTop());
        canvas.AttachBrush(mask_->GetMaskBrush());
        canvas.DrawPath(*mask_->GetMaskPath());
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

// ============================================================================
// Shadow
std::unique_ptr<RSPropertyDrawable> RSShadowBaseDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
    if (properties.GetShadowMask()) {
        return std::make_unique<RSColorfulShadowDrawable>(properties);
    } else {
        if (properties.GetShadow()->GetHardwareAcceleration()) {
            if (properties.GetShadowElevation() <= 0.f) {
                return nullptr;
            }
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(properties);
        } else {
            return std::make_unique<RSShadowDrawable>(properties);
        }
    }
}

RSShadowBaseDrawable::RSShadowBaseDrawable(const RSProperties& properties)
    : offsetX_(properties.GetShadowOffsetX()), offsetY_(properties.GetShadowOffsetY()),
      color_(properties.GetShadowColor())
{}

RSShadowDrawable::RSShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), radius_(properties.GetShadowRadius())
{}

#ifndef USE_ROSEN_DRAWING
void RSShadowBaseDrawable::ClipShadowPath(RSRenderNode& node, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else {
        skPath.addRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()));
        if (!properties.GetShadowIsFilled()) {
            canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), SkClipOp::kDifference, true);
        }
    }
}
#else
void RSShadowBaseDrawable::ClipShadowPath(RSRenderNode& node, RSPaintFilterCanvas& canvas, Drawing::Path& path)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else {
        path.AddRoundRect(RRect2DrawingRRect(properties.GetRRect()));
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::DIFFERENCE, true);
        }
    }
}
#endif

void RSShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    skPath.offset(offsetX_, offsetY_);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPaint paint;
    paint.setColor(color_.AsArgbInt());
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_));
    canvas.drawPath(skPath, paint);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    path.Offset(offsetX_, offsetY_);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::SCALE_X, std::ceil(matrix.Get(Drawing::Matrix::SCALE_X)));
    matrix.Set(Drawing::Matrix::SCALE_Y, std::ceil(matrix.Get(Drawing::Matrix::SCALE_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush brush;
    brush.SetColor(color_.AsArgbInt());
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, radius_));
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawPath(path);
    canvas.DetachBrush();
#endif
}

RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), shadowElevation_(properties.GetShadowElevation())
{}

void RSHardwareAccelerationShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    skPath.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPoint3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    SkPoint3 lightPos = { canvas.getTotalMatrix().getTranslateX() + skPath.getBounds().centerX(),
        canvas.getTotalMatrix().getTranslateY() + skPath.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    Color spotColor = color_;
    spotColor.MultiplyAlpha(canvas.GetAlpha());
    SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, ambientColor.AsArgbInt(),
        spotColor.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    path.Offset(offsetX_, offsetY_);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::SCALE_X, std::ceil(matrix.Get(Drawing::Matrix::SCALE_X)));
    matrix.Set(Drawing::Matrix::SCALE_Y, std::ceil(matrix.Get(Drawing::Matrix::SCALE_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Point3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    Drawing::scalar centerX = path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2;
    Drawing::scalar centerY = path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2;
    Drawing::Point3 lightPos = { canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X) + centerX,
        canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_Y) + centerY, DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    Color spotColor = color_;
    spotColor.MultiplyAlpha(canvas.GetAlpha());
    canvas.DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, Drawing::Color(ambientColor.AsArgbInt()),
        Drawing::Color(spotColor.AsArgbInt()), Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
#endif
}

RSColorfulShadowDrawable::RSColorfulShadowDrawable(const RSProperties& properties) : RSShadowBaseDrawable(properties)
{
#ifndef USE_ROSEN_DRAWING
    const SkScalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    blurPaint_.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
#else
    const Drawing::scalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    Drawing::Filter filter;
    filter.SetImageFilter(
        Drawing::ImageFilter::CreateBlurImageFilter(blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush_.SetFilter(filter);
#endif
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    // save layer, draw image with clipPath, blur and draw back
    canvas.saveLayer(nullptr, &blurPaint_);
    canvas.translate(offsetX_, offsetY_);
    canvas.clipPath(skPath);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    // save layer, draw image with clipPath, blur and draw back
    Drawing::SaveLayerOps slr(nullptr, &blurBrush_);
    canvas.SaveLayer(slr);
    canvas.Translate(offsetX_, offsetY_);
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
#endif
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(canvas);
    }
}

// ============================================================================
// DynamicLightUp
std::unique_ptr<RSPropertyDrawable> RSDynamicLightUpDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (!properties.IsDynamicLightUpValid()) {
        return nullptr;
    }
    return std::make_unique<RSDynamicLightUpDrawable>(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value());
}

RSDynamicLightUpDrawable::RSDynamicLightUpDrawable(float rate, float degree) : rate_(rate), degree_(degree) {}

void RSDynamicLightUpDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeDynamicLightUpShader(rate_, degree_, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
#else

#endif
}

// ============================================================================
// BackgroundEffect
std::unique_ptr<RSPropertyDrawable> RSEffectDataApplyDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (context.properties_.GetUseEffect() == false) {
        return nullptr;
    }
    return std::make_unique<RSEffectDataApplyDrawable>();
}

void RSEffectDataApplyDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    const auto& effectData = canvas.GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }

    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(canvas.getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    SkPaint defaultPaint;
    canvas.drawImageRect(effectData->cachedImage_, srcRect, dstRect, SkSamplingOptions(), &defaultPaint,
        SkCanvas::kFast_SrcRectConstraint);
#else
// Drawing is not supported
#endif
}

// ============================================================================
// LightUpEffect
void RSLightUpEffectDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    auto& properties = node.GetMutableRenderProperties();
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSLightUpEffectDrawable::Draw skSurface is null");
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSLightUpEffectDrawable::Draw image is null");
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeLightUpEffectShader(properties.GetLightUpEffect(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
#endif
#endif
}

std::unique_ptr<RSPropertyDrawable> RSLightUpEffectDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    if (!context.properties_.IsLightUpEffectValid()) {
        return nullptr;
    }
    return std::make_unique<RSLightUpEffectDrawable>();
}

void RSBackgroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSFilterDrawable::DrawFilter(node, canvas, BACKGROUND_FILTER, filter_);
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (!RSFilterDrawable::GetBlurEnabled()) {
        ROSEN_LOGD("RSBackgroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = context.properties_.GetBackgroundFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    if (context.node_->IsInstanceOf<RSEffectRenderNode>()) {
        // for RSEffectRenderNode, we just use generates effect data, instead of draw filter
        return std::make_unique<RSEffectDataGenerateDrawable>(filter);
    }
    return std::make_unique<RSBackgroundFilterDrawable>(filter);
}

bool RSFilterDrawable::GetBlurEnabled()
{
    static bool BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();
    return BLUR_ENABLED;
}

void RSFilterDrawable::DrawFilter(
    RSRenderNode& node, RSPaintFilterCanvas& canvas, FilterType filterType, const std::shared_ptr<RSFilter>& rsFilter)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetMutableRenderProperties();
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());

    auto filter = std::static_pointer_cast<RSSkiaFilter>(rsFilter);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::DrawFilter skSurface null");
        auto paint = filter->GetPaint();
        SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
        canvas.saveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(&canvas,
        filterType == FilterType::FOREGROUND_FILTER ? RSPaintFilterCanvas::kAlpha : RSPaintFilterCanvas::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas.SetAlpha(1.0);
    }

#if defined(RS_ENABLE_GL)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        cacheManager->DrawFilter(canvas, filter);
        return;
    }
#endif
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipIBounds.makeOutset(-1, -1));
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSFilterDrawable::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->width(), imageSnapshot->height())) {
        ROSEN_LOGD("RSFilterDrawable cache image resource(width:%{public}d, height:%{public}d).",
            imageSnapshot->width(), imageSnapshot->height());
        as_IB(imageSnapshot)->hintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
    filter->DrawImageRect(canvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), SkRect::Make(clipIBounds));
    filter->PostProcess(canvas);
#else

#endif
}

void RSForegroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSFilterDrawable::DrawFilter(node, canvas, FOREGROUND_FILTER, filter_);
}

std::unique_ptr<RSPropertyDrawable> RSForegroundFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (!RSFilterDrawable::GetBlurEnabled()) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = context.properties_.GetFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    return std::make_unique<RSForegroundFilterDrawable>(filter);
}

void RSLinearGradientBlurFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);

    auto clipBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas.getTotalMatrix();
    uint8_t directionBias = RSPropertiesPainter::CalcDirectionBias(mat);
    auto alphaGradientShader =
        RSPropertiesPainter::MakeAlphaGradientShader(SkRect::Make(clipIPadding), para_, directionBias);
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilterDrawable::Draw alphaGradientShader null");
        return;
    }
    float radius = para_->blurRadius_ / 2;
    canvas.resetMatrix();
    canvas.translate(clipIPadding.left(), clipIPadding.top());

    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
#endif
#else
#endif
}

std::unique_ptr<RSPropertyDrawable> RSLinearGradientBlurFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    const auto& para = context.properties_.GetLinearGradientBlurPara();
    if (para == nullptr || para->blurRadius_ <= 0) {
        return nullptr;
    }
    return std::make_unique<RSLinearGradientBlurFilterDrawable>(para);
}

void RSForegroundColorDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();
#ifndef USE_ROSEN_DRAWING
    canvas.drawRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), paint_);
#else
    canvas.AttachBrush(brush_);
    canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
#endif
}

std::unique_ptr<RSPropertyDrawable> RSForegroundColorDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto bgColor = context.properties_.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(paint));
#else
    Drawing::Brush brush;
    brush.SetColor(bgColor.AsArgbInt());
    brush.SetAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(brush));
#endif
}

std::unique_ptr<RSPropertyDrawable> RSParticleDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return nullptr;
    }
    auto uniParticleDrawable = std::make_unique<RSParticleDrawable>();
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetBoundsRect();
    for (auto& particle : particles) {
        if (particle != nullptr && particle->IsAlive()) {
            auto position = particle->GetPosition();
            if (!(bounds.Intersect(position.x_, position.y_))) {
                continue;
            }
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
#ifndef USE_ROSEN_DRAWING
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlphaf(opacity);
#else
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaF(opacity);
#endif
            if (particleType == ParticleType::POINTS) {
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
#ifndef USE_ROSEN_DRAWING
                paint.setColor(color.AsArgbInt());
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSPointParticleDrawable>(std::move(paint), particle));
#else
                brush.SetColor(color.AsArgbInt());
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSPointParticleDrawable>(std::move(brush), particle, clipBounds));
#endif
            } else {
#ifndef USE_ROSEN_DRAWING
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSImageParticleDrawable>(std::move(paint), particle));
#else
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSImageParticleDrawable>(std::move(brush), particle, clipBounds));
#endif
            }
        }
    }
    return std::move(uniParticleDrawable);
}

void RSParticleDrawable::AddPropertyDrawable(std::shared_ptr<RSPropertyDrawable> drawable)
{
    propertyDrawableVec_.emplace_back(drawable);
}

void RSParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    for (auto& propertyDrawable : propertyDrawableVec_) {
        if (propertyDrawable != nullptr) {
            propertyDrawable->Draw(node, canvas);
        }
    }
}

void RSPointParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    auto radius = particles_->GetRadius();
    auto position = particles_->GetPosition();
    float scale = particles_->GetScale();
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(bounds, true);
    canvas.drawCircle(position.x_, position.y_, radius * scale, paint_);
#else
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
    canvas.AttachBrush(brush_);
    canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
    canvas.DetachBrush();
#endif
}

void RSImageParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    auto imageSize = particles_->GetImageSize();
    auto image = particles_->GetImage();
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(bounds, true);
#else
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
    auto position = particles_->GetPosition();
    float left = position.x_;
    float top = position.y_;
    float scale = particles_->GetScale();
    float right = position.x_ + imageSize.x_ * scale;
    float bottom = position.y_ + imageSize.y_ * scale;
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    canvas.translate(position.x_, position.y_);
    canvas.rotate(particles_->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#else
    canvas.Save();
    canvas.Translate(position.x_, position.y_);
    canvas.Rotate(particles_->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#endif
    RectF destRect(left, top, right, bottom);
    image->SetDstRect(destRect);
    image->SetScale(scale);
    image->SetImageRepeat(0);
#ifndef USE_ROSEN_DRAWING
    SkRect rect { left, top, right, bottom };
#ifdef NEW_SKIA
    image->CanvasDrawImage(canvas, rect, SkSamplingOptions(), paint_, false);
    canvas.restore();
#else
    image->CanvasDrawImage(canvas, rect, paint_, false);
    canvas.restore();
#endif
#else
    Drawing::Rect rect { left, top, right, bottom };
    canvas.AttachBrush(brush_);
    image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
    canvas.DetachBrush();
    canvas.Restore();
#endif
}

void RSPixelStretchDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawPixelStretch(node.GetRenderProperties(), canvas);
}
std::unique_ptr<RSPropertyDrawable> RSPixelStretchDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return nullptr;
    }
    return std::make_unique<RSPixelStretchDrawable>();
}

void RSBackgroundDrawable::setForceBgAntiAlias(bool forceBgAntiAlias)
{
    forceBgAntiAlias_ = forceBgAntiAlias;
}

void RSBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawRRect(RSPropertiesPainter::RRect2SkRRect(node.GetMutableRenderProperties().GetInnerRRect()), paint_);
#else
    canvas.DrawBackground(brush_);
#endif
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundColorDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& bgColor = properties.GetBackgroundColor();
    bool isTransparent = (bgColor == RgbPalette::Transparent());
    if (isTransparent) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
    return std::make_unique<RSBackgroundColorDrawable>(hasRoundedCorners, bgColor.AsArgbInt());
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundShaderDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
#ifndef USE_ROSEN_DRAWING
    return std::make_unique<RSBackgroundShaderDrawable>(hasRoundedCorners, bgShader->GetSkShader());
#else
    return std::make_unique<RSBackgroundShaderDrawable>(hasRoundedCorners, bgShader->GetDrawingShader());
#endif
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundImageDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
    bgImage->SetDstRect(properties.GetBgImageRect());
    return std::make_unique<RSBackgroundImageDrawable>(hasRoundedCorners, bgImage);
}

void RSBackgroundImageDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();

#ifndef USE_ROSEN_DRAWING
    auto boundsRect = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#ifdef NEW_SKIA
    image_->CanvasDrawImage(canvas, boundsRect, SkSamplingOptions(), paint_, true);
#else
    image_->CanvasDrawImage(canvas, boundsRect, paint_, true);
#endif
#else
    auto boundsRect = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.AttachBrush(brush_);
    image_->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
#endif
}

void RSEffectDataGenerateDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (auto effectNode = node.ReinterpretCastTo<RSEffectRenderNode>()) {
        if (auto& region = effectNode->effectRegion_) {
            RSPropertiesPainter::DrawBackgroundEffect(node.GetRenderProperties(), canvas, region->getBounds());
        }
    }
}

// ============================================================================
// SavelayerBackground
std::unique_ptr<RSPropertyDrawable> RSSaveLayerBackgroundDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendModeType::NONE)) {
        return nullptr;
    }
    return std::make_unique<RSSaveLayerBackgroundDrawable>();
}

void RSSaveLayerBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.saveLayer(nullptr, nullptr);
}

// ============================================================================
// SavelayerContent
std::unique_ptr<RSPropertyDrawable> RSSaveLayerContentDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    int blendMode = properties.GetColorBlendMode();
    if (blendMode == static_cast<int>(RSColorBlendModeType::NONE)) {
        return nullptr;
    }
    static const std::vector<SkBlendMode> blendModeList = {
        SkBlendMode::kSrcIn, // RSColorBlendModeType::SRC_IN
        SkBlendMode::kDstIn, // RSColorBlendModeType::DST_IN
    };
    if (static_cast<unsigned long>(blendMode) >= blendModeList.size()) {
        ROSEN_LOGE("color blendmode is set %d which is invalid.", blendMode);
        return nullptr;
    }
    SkPaint blendPaint;
    blendPaint.setBlendMode(blendModeList[blendMode]);
    return std::make_unique<RSSaveLayerContentDrawable>(std::move(blendPaint));
}

void RSSaveLayerContentDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.saveLayer(nullptr, &blendPaint_);
}
} // namespace OHOS::Rosen
