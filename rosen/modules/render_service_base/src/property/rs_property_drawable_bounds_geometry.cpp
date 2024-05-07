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

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"
#include "render/rs_drawing_filter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
constexpr int TRACE_LEVEL_TWO = 2;
} // namespace
namespace OHOS::Rosen {
// ============================================================================
// Bounds geometry
void RSBoundsGeometryDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    canvas.ConcatMatrix(content.GetRenderProperties().GetBoundsGeometry()->GetMatrix());
}
RSPropertyDrawable::DrawablePtr RSBoundsGeometryDrawable::Generate(const RSRenderContent& content)
{
    return std::make_unique<RSBoundsGeometryDrawable>();
}

void RSClipBoundsDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    // Planning: Generate() should calculate the draw op and cache it
    auto& properties = content.GetRenderProperties();
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
            RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, true);
    }
}

RSPropertyDrawable::DrawablePtr RSClipBoundsDrawable::Generate(const RSRenderContent& content)
{
    return std::make_unique<RSClipBoundsDrawable>();
}

// ============================================================================
// PointLight
RSPropertyDrawable::DrawablePtr RSPointLightDrawable::Generate(const RSRenderContent& content)
{
    const auto& properties = content.GetRenderProperties();
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (illuminatedPtr && illuminatedPtr->IsIlluminatedValid()) {
        return std::make_unique<RSPointLightDrawable>();
    }
    return nullptr;
}

void RSPointLightDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (illuminatedPtr && illuminatedPtr->IsIlluminated()) {
        RSPropertiesPainter::DrawLight(properties, canvas);
    }
}

// ============================================================================
// Border
RSPropertyDrawable::DrawablePtr RSBorderDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return nullptr;
    }
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        return std::make_unique<RSBorderDRRectDrawable>(
            std::move(brush), std::move(pen), properties, true);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(pen)) {
        return std::make_unique<RSBorderFourLineDrawable>(
            std::move(brush), std::move(pen), properties, true);
    } else if (border->ApplyPathStyle(pen)) {
        return std::make_unique<RSBorderPathDrawable>(
            std::move(brush), std::move(pen), properties, true);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(
            std::move(brush), std::move(pen), properties, true);
    }
}
RSBorderDRRectDrawable::RSBorderDRRectDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties, const bool& drawBorder)
    : RSBorderDrawable(std::move(brush), std::move(pen))
{
    drawBorder_ = drawBorder;
    OnBoundsChange(properties);
}

void RSBorderDRRectDrawable::OnBoundsChange(const RSProperties& properties)
{
    if (drawBorder_) {
        inner_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
        outer_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
    } else {
        inner_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
        RectF rect = properties.GetBoundsRect().MakeOutset(properties.GetOutline()->GetWidthFour());
        outer_ = RSPropertiesPainter::RRect2DrawingRRect(RRect(rect, properties.GetOutline()->GetRadiusFour()));
    }
}
void RSBorderDRRectDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    canvas.AttachBrush(brush_);
    canvas.DrawNestedRoundRect(outer_, inner_);
    canvas.DetachBrush();
}

RSBorderFourLineDrawable::RSBorderFourLineDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties, const bool& drawBorder)
    : RSBorderDrawable(std::move(brush), std::move(pen))
{
    drawBorder_ = drawBorder;
    OnBoundsChange(properties);
}

void RSBorderFourLineDrawable::OnBoundsChange(const RSProperties& properties)
{
    if (drawBorder_) {
        rect_ = properties.GetBoundsRect();
    } else {
        rect_ = properties.GetBoundsRect().MakeOutset(properties.GetOutline()->GetWidthFour());
    }
}
void RSBorderFourLineDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto pen = pen_;
    if (drawBorder_) {
        content.GetRenderProperties().GetBorder()->PaintFourLine(canvas, pen, rect_);
    } else {
        content.GetRenderProperties().GetOutline()->PaintFourLine(canvas, pen, rect_);
    }
}

RSBorderPathDrawable::RSBorderPathDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties, const bool& drawBorder)
    : RSBorderDrawable(std::move(brush), std::move(pen))
{
    drawBorder_ = drawBorder;
    OnBoundsChange(properties);
}

void RSBorderPathDrawable::OnBoundsChange(const RSProperties& properties)
{
    float borderWidth;
    RRect rrect;
    if (drawBorder_) {
        borderWidth = properties.GetBorder()->GetWidth();
        rrect = properties.GetRRect();
    } else {
        borderWidth = properties.GetOutline()->GetWidth();
        RectF rect = properties.GetBoundsRect().MakeOutset(properties.GetOutline()->GetWidthFour());
        rrect = RRect(rect, properties.GetOutline()->GetRadiusFour());
    }
    rrect.rect_.width_ -= borderWidth;
    rrect.rect_.height_ -= borderWidth;
    rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
    borderPath_.Reset();
    borderPath_.AddRoundRect(RSPropertiesPainter::RRect2DrawingRRect(rrect));
}
void RSBorderPathDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    canvas.AttachPen(pen_);
    canvas.DrawPath(borderPath_);
    canvas.DetachPen();
}

RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties, const bool& drawBorder)
    : RSBorderDrawable(std::move(brush), std::move(pen)), drawBorder_(drawBorder)
{
    OnBoundsChange(properties);
}

void RSBorderFourLineRoundCornerDrawable::OnBoundsChange(const RSProperties& properties)
{
    if (drawBorder_) {
        innerRrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
        rrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
    } else {
        innerRrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
        RectF rect = properties.GetBoundsRect().MakeOutset(properties.GetOutline()->GetWidthFour());
        rrect_ = RSPropertiesPainter::RRect2DrawingRRect(RRect(rect, properties.GetOutline()->GetRadiusFour()));
    }
}
void RSBorderFourLineRoundCornerDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto pen = pen_;
    Drawing::AutoCanvasRestore acr(canvas, true);
    auto& properties = content.GetRenderProperties();
    canvas.ClipRoundRect(rrect_,Drawing::ClipOp::INTERSECT, true);
    canvas.ClipRoundRect(innerRrect_, Drawing::ClipOp::DIFFERENCE, true);
    Drawing::scalar centerX = innerRrect_.GetRect().GetLeft() + innerRrect_.GetRect().GetWidth() / 2;
    Drawing::scalar centerY = innerRrect_.GetRect().GetTop() + innerRrect_.GetRect().GetHeight() / 2;
    Drawing::Point center = { centerX, centerY };
    auto rect = rrect_.GetRect();
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    if (drawBorder_) {
        properties.GetBorder()->PaintTopPath(canvas, pen, rrect_, center);
        properties.GetBorder()->PaintRightPath(canvas, pen, rrect_, center);
        properties.GetBorder()->PaintBottomPath(canvas, pen, rrect_, center);
        properties.GetBorder()->PaintLeftPath(canvas, pen, rrect_, center);
    } else {
        properties.GetOutline()->PaintTopPath(canvas, pen, rrect_, center);
        properties.GetOutline()->PaintRightPath(canvas, pen, rrect_, center);
        properties.GetOutline()->PaintBottomPath(canvas, pen, rrect_, center);
        properties.GetOutline()->PaintLeftPath(canvas, pen, rrect_, center);
    }
}

// ============================================================================
// Outline
RSPropertyDrawable::DrawablePtr RSOutlineDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto& outline = properties.GetOutline();
    if (!outline || !outline->HasBorder()) {
        return nullptr;
    }
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if (outline->ApplyFillStyle(brush)) {
        return std::make_unique<RSBorderDRRectDrawable>(std::move(brush), std::move(pen), properties, false);
    } else if (outline->GetRadiusFour().IsZero() && outline->ApplyFourLine(pen)) {
        return std::make_unique<RSBorderFourLineDrawable>(std::move(brush), std::move(pen), properties, false);
    } else if (outline->ApplyPathStyle(pen)) {
        return std::make_unique<RSBorderPathDrawable>(std::move(brush), std::move(pen), properties, false);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(
            std::move(brush), std::move(pen), properties, false);
    }
}

// ============================================================================
// Mask
RSPropertyDrawable::DrawablePtr RSMaskDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
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
    } else if (mask->IsPixelMapMask()) {
        return std::make_unique<RSPixelMapMaskDrawable>(mask);
    }
    return nullptr;
}

RSMaskDrawable::RSMaskDrawable(std::shared_ptr<RSMask> mask) : mask_(std::move(mask))
{
    maskBrush_.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())));
    maskFilterBrush_.SetFilter(filter);
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgDomMaskDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();
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
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgPictureMaskDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();
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
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSGradientMaskDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();
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
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSPathMaskDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft(), bounds.GetTop());
        canvas.AttachBrush(mask_->GetMaskBrush());
        canvas.AttachPen(mask_->GetMaskPen());
        canvas.DrawPath(*mask_->GetMaskPath());
        canvas.DetachBrush();
        canvas.DetachPen();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
}

RSPixelMapMaskDrawable::RSPixelMapMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSPixelMapMaskDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        if (mask_->GetImage()) {
            canvas.DrawImage(*mask_->GetImage(), 0.f, 0.f, Drawing::SamplingOptions());
        }
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
}

// ============================================================================
// Shadow
RSPropertyDrawable::DrawablePtr RSShadowBaseDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
    if (properties.GetShadowMask()) {
        return std::make_unique<RSColorfulShadowDrawable>(properties);
    } else {
        if (properties.GetShadowElevation() > 0.f) {
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(properties);
        } else {
            return std::make_unique<RSShadowDrawable>(properties);
        }
        return nullptr;
    }
}

RSShadowBaseDrawable::RSShadowBaseDrawable(const RSProperties& properties)
    : offsetX_(properties.GetShadowOffsetX()), offsetY_(properties.GetShadowOffsetY()),
      color_(properties.GetShadowColor())
{}

RSShadowDrawable::RSShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), radius_(properties.GetShadowRadius())
{}

void RSShadowBaseDrawable::ClipShadowPath(
    const RSRenderContent& content, RSPaintFilterCanvas& canvas, Drawing::Path& path) const
{
    const auto& properties = content.GetRenderProperties();
    if (properties.GetShadowPath() && properties.GetShadowPath()->GetDrawingPath().IsValid()) {
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
        path.AddRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()));
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipRoundRect(
                RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::DIFFERENCE, true);
        }
    }
}

RSColor RSShadowDrawable::GetColorForShadow(const RSRenderContent& content, RSPaintFilterCanvas& canvas,
    Drawing::Path& skPath, Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds) const
{
    RSColor colorPicked;
    const RSProperties& properties = content.GetRenderProperties();
    // shadow alpha follow setting
    auto shadowAlpha = color_.GetAlpha();
    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (colorPickerTask != nullptr &&
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        if (RSPropertiesPainter::PickColor(properties, canvas, skPath, matrix, deviceClipBounds, colorPicked)) {
            RSPropertiesPainter::GetDarkColor(colorPicked);
        } else {
            shadowAlpha = 0;
        }
        if (!colorPickerTask->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        shadowAlpha = color_.GetAlpha();
        colorPicked = color_;
    }
    return RSColor(colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue(), shadowAlpha);
}

void RSShadowDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    if (content.GetRenderProperties().GetNeedSkipShadow()) {
        RS_TRACE_NAME("RSShadowDrawable::Draw NeedSkipShadow");
        return;
    }
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSShadowDrawable::DrawShadow, Radius: %f, ShadowOffsetX: %f, ShadowOffsetY: %f, bounds: %s", radius_, offsetX_,
        offsetY_, content.GetRenderProperties().GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    auto deviceClipBounds = canvas.GetDeviceClipBounds();
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(content, canvas, path);
    path.Offset(offsetX_, offsetY_);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    RSColor colorPicked = GetColorForShadow(content, canvas, path, matrix, deviceClipBounds);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(
        colorPicked.GetAlpha(), colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue()));
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, radius_));
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawPath(path);
    canvas.DetachBrush();
}

RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), shadowElevation_(properties.GetShadowElevation())
{}

void RSHardwareAccelerationShadowDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSHardwareAccelerationShadowDrawable::DrawShadow, Elevation: %f, ShadowOffsetX: %f, "
        "ShadowOffsetY: %f, bounds: %s",
        shadowElevation_, offsetX_, offsetY_,
        content.GetRenderProperties().GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(content, canvas, path);
    path.Offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Point3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    std::vector<Drawing::Point> pt{{path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2,
        path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2}};
    canvas.GetTotalMatrix().MapPoints(pt, pt, 1);
    Drawing::Point3 lightPos = {pt[0].GetX(), pt[0].GetY(), DEFAULT_LIGHT_HEIGHT};
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    Color spotColor = color_;
    spotColor.MultiplyAlpha(canvas.GetAlpha());
    canvas.DrawShadowStyle(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, Drawing::Color(ambientColor.AsArgbInt()),
        Drawing::Color(spotColor.AsArgbInt()), Drawing::ShadowFlags::TRANSPARENT_OCCLUDER, true);
}

RSColorfulShadowDrawable::RSColorfulShadowDrawable(const RSProperties& properties) : RSShadowBaseDrawable(properties)
{
    const Drawing::scalar blurRadius =
        properties.GetShadowElevation() > 0.f
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    Drawing::Filter filter;
    filter.SetImageFilter(
        Drawing::ImageFilter::CreateBlurImageFilter(blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush_.SetFilter(filter);
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(content, canvas, path);
    // save layer, draw image with clipPath, blur and draw back
    Drawing::SaveLayerOps slr(nullptr, &blurBrush_);
    canvas.SaveLayer(slr);
    canvas.Translate(offsetX_, offsetY_);
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(canvas);
    }
}

// ============================================================================
// DynamicLightUp
RSPropertyDrawable::DrawablePtr RSDynamicLightUpDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    if (!properties.IsDynamicLightUpValid()) {
        return nullptr;
    }
    return std::make_unique<RSDynamicLightUpDrawable>();
}

bool RSDynamicLightUpDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().IsDynamicLightUpValid();
}

void RSDynamicLightUpDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawDynamicLightUp(content.GetRenderProperties(), canvas);
}

// ============================================================================
// LightUpEffect
RSPropertyDrawable::DrawablePtr RSLightUpEffectDrawable::Generate(const RSRenderContent& content)
{
    if (!content.GetRenderProperties().IsLightUpEffectValid()) {
        return nullptr;
    }
    return std::make_unique<RSLightUpEffectDrawable>();
}

bool RSLightUpEffectDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().IsLightUpEffectValid();
}

// ============================================================================
//DynamicDim
RSPropertyDrawable::DrawablePtr RSDynamicDimDrawable::Generate(const RSRenderContent& context)
{
    if (!context.GetRenderProperties().IsDynamicDimValid()) {
        return nullptr;
    }
    return std::make_unique<RSDynamicDimDrawable>();
}

bool RSDynamicDimDrawable::Update(const RSRenderContent& context)
{
    return context.GetRenderProperties().IsDynamicDimValid();
}

void RSDynamicDimDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawDynamicDim(content.GetRenderProperties(), canvas);
}

// ============================================================================
// Binarization
void RSBinarizationDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawBinarizationShader(content.GetRenderProperties(), canvas);
}
 
RSPropertyDrawable::DrawablePtr RSBinarizationDrawable::Generate(const RSRenderContent& content)
{
    auto& aiInvert = content.GetRenderProperties().GetAiInvert();
    if (!aiInvert.has_value()) {
        return nullptr;
    }
    return std::make_unique<RSBinarizationDrawable>();
}

// ============================================================================
// LightUpEffect
void RSLightUpEffectDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawLightUpEffect(content.GetRenderProperties(), canvas);
}

// ============================================================================
// background filter
RSPropertyDrawable::DrawablePtr RSBackgroundFilterDrawable::Generate(const RSRenderContent& content)
{
    if (!RSPropertiesPainter::BLUR_ENABLED) {
        ROSEN_LOGD("RSBackgroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = content.GetRenderProperties().GetBackgroundFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    if (content.GetType() == RSRenderNodeType::EFFECT_NODE) {
        // for RSEffectRenderNode, we just use generates effect data, instead of draw filter
        return std::make_unique<RSEffectDataGenerateDrawable>();
    }
    return std::make_unique<RSBackgroundFilterDrawable>();
}

void RSBackgroundFilterDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    // fix compatibility with Node Group, copied from RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSPropertiesPainter::DrawFilter(content.GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
}

// compositing filter
RSPropertyDrawable::DrawablePtr RSCompositingFilterDrawable::Generate(const RSRenderContent& content)
{
    if (!RSPropertiesPainter::BLUR_ENABLED) {
        ROSEN_LOGD("RSCompositingFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = content.GetRenderProperties().GetFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    return std::make_unique<RSCompositingFilterDrawable>();
}

bool RSCompositingFilterDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetFilter() != nullptr;
}

void RSCompositingFilterDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawFilter(content.GetRenderProperties(), canvas, FilterType::FOREGROUND_FILTER);
}

bool IsForegroundFilterValid(const RSRenderContent& content)
{
    auto& rsFilter = content.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    return true;
}

// foreground filter
RSPropertyDrawable::DrawablePtr RSForegroundFilterDrawable::Generate(const RSRenderContent& content)
{
    if (!RSPropertiesPainter::FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Generate close blur.");
        return nullptr;
    }

    if (!IsForegroundFilterValid(content)) {
        return nullptr;
    }
    return std::make_unique<RSForegroundFilterDrawable>();
}

bool RSForegroundFilterDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetForegroundFilter() != nullptr;
}

void RSForegroundFilterDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RS_OPTIONAL_TRACE_NAME("RSForegroundFilterDrawable::Draw");
    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Draw main screen canvas no surface.");
        return;
    }
    auto bounds = content.GetRenderProperties().GetBoundsRect();
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(bounds.width_, bounds.height_);
    if (!offscreenSurface) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Draw create offscreenSurface fail.");
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    if (!offscreenCanvas) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Draw create offscreenCanvas fail.");
        return;
    }
    canvas.StoreCanvas();
    canvas.ReplaceMainScreenData(offscreenSurface, offscreenCanvas);
    offscreenCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    canvas.SavePCanvasList();
    canvas.RemoveAll();
    canvas.AddCanvas(offscreenCanvas.get());
}

// foreground filter restore
RSPropertyDrawable::DrawablePtr RSForegroundFilterRestoreDrawable::Generate(const RSRenderContent& content)
{
    if (!RSPropertiesPainter::FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Generate close blur.");
        return nullptr;
    }

    if (!IsForegroundFilterValid(content)) {
        return nullptr;
    }

    return std::make_unique<RSForegroundFilterRestoreDrawable>();
}

bool RSForegroundFilterRestoreDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetForegroundFilter() != nullptr;
}

void RSForegroundFilterRestoreDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawForegroundFilter(content.GetRenderProperties(), canvas);
}

// effect data
bool RSEffectDataGenerateDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetBackgroundFilter() != nullptr;
}

void RSEffectDataGenerateDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    const auto& properties = content.GetRenderProperties();
    if (properties.GetHaveEffectRegion() && properties.GetBackgroundFilter() &&
        RSSystemProperties::GetEffectMergeEnabled()) {
        RSPropertiesPainter::DrawBackgroundEffect(content.GetRenderProperties(), canvas);
    }
}

// effect data apply
RSPropertyDrawable::DrawablePtr RSEffectDataApplyDrawable::Generate(const RSRenderContent& content)
{
    if (content.GetRenderProperties().GetUseEffect() == false) {
        return nullptr;
    }
    return std::make_unique<RSEffectDataApplyDrawable>();
}

bool RSEffectDataApplyDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetUseEffect();
}

bool RSBackgroundFilterDrawable::Update(const RSRenderContent& content)
{
    return content.GetRenderProperties().GetBackgroundFilter() != nullptr;
}

void RSEffectDataApplyDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    // fix compatibility with Node Group, copied from RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSPropertiesPainter::ApplyBackgroundEffect(content.GetRenderProperties(), canvas);
}

// ============================================================================
// ForegroundColor
void RSForegroundColorDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    canvas.AttachBrush(brush_);
    canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
}

bool RSForegroundColorDrawable::Update(const RSRenderContent& content)
{
    auto fgColor = content.GetRenderProperties().GetForegroundColor();
    brush_.SetColor(fgColor.AsArgbInt());
    return fgColor != RgbPalette::Transparent();
}

RSPropertyDrawable::DrawablePtr RSForegroundColorDrawable::Generate(const RSRenderContent& content)
{
    auto fgColor = content.GetRenderProperties().GetForegroundColor();
    if (fgColor == RgbPalette::Transparent()) {
        return nullptr;
    }
    Drawing::Brush brush;
    brush.SetColor(fgColor.AsArgbInt());
    brush.SetAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(brush));
}

// ============================================================================
// Particle
RSPropertyDrawable::DrawablePtr RSParticleDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return nullptr;
    }
    return std::make_unique<RSParticleDrawable>();
}

bool RSParticleDrawable::Update(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    return properties.GetPixelStretch().has_value();
}

void RSParticleDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawParticle(content.GetRenderProperties(), canvas);
}

void RSPixelStretchDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    RSPropertiesPainter::DrawPixelStretch(content.GetRenderProperties(), canvas);
}

bool RSPixelStretchDrawable::Update(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    return properties.GetPixelStretch().has_value();
}

RSPropertyDrawable::DrawablePtr RSPixelStretchDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return nullptr;
    }
    return std::make_unique<RSPixelStretchDrawable>();
}

// ============================================================================
// Background
void RSBackgroundDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    Drawing::Brush brush = brush_;
    // use drawrrect to avoid texture update in phone screen rotation scene
    if (RSSystemProperties::IsPhoneType() && RSSystemProperties::GetCacheEnabledForRotation()) {
        bool antiAlias = RSPropertiesPainter::GetBgAntiAlias() || !properties.GetCornerRadius().IsZero();
        brush.SetAntiAlias(antiAlias);
        canvas.AttachBrush(brush);
        if (properties.GetBorderColorIsTransparent() ||
            properties.GetBorderStyle().x_ != static_cast<uint32_t>(BorderStyle::SOLID)) {
            canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()));
        } else {
            canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect()));
        }
    } else {
        canvas.AttachBrush(brush);
        if (properties.GetBorderColorIsTransparent() ||
            properties.GetBorderStyle().x_ != static_cast<uint32_t>(BorderStyle::SOLID)) {
            canvas.DrawRect(RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()));
        } else {
            canvas.DrawRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect()).GetRect());
        }
    }
    canvas.DetachBrush();
}

RSPropertyDrawable::DrawablePtr RSBackgroundColorDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto& bgColor = properties.GetBackgroundColor();
    bool isTransparent = (bgColor == RgbPalette::Transparent());
    if (isTransparent) {
        return nullptr;
    }
    return std::make_unique<RSBackgroundColorDrawable>(bgColor.AsArgbInt());
}

bool RSBackgroundColorDrawable::Update(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto& bgColor = properties.GetBackgroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return false;
    }
    brush_.SetColor(bgColor.AsArgbInt());
    return true;
}

RSPropertyDrawable::DrawablePtr RSBackgroundShaderDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return nullptr;
    }
    return std::make_unique<RSBackgroundShaderDrawable>(bgShader->GetDrawingShader());
}

bool RSBackgroundShaderDrawable::Update(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return false;
    }
    brush_.SetShaderEffect(bgShader->GetDrawingShader());
    return true;
}

RSPropertyDrawable::DrawablePtr RSBackgroundImageDrawable::Generate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return nullptr;
    }
    bgImage->SetDstRect(properties.GetBgImageRect());
    return std::make_unique<RSBackgroundImageDrawable>();
}

bool RSBackgroundImageDrawable::Update(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return false;
    }
    bgImage->SetDstRect(properties.GetBgImageRect());
    return true;
}

void RSBackgroundImageDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto& properties = content.GetRenderProperties();
    const auto& image = properties.GetBgImage();

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto pixelMap = image->GetPixelMap();
    if (pixelMap && pixelMap->IsAstc()) {
        const void* data = pixelMap->GetPixels();
        std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
        const int seekSize = 16;
        if (pixelMap->GetCapacity() > seekSize) {
            fileData->BuildWithoutCopy((void*)((char*) data + seekSize), pixelMap->GetCapacity() - seekSize);
        }
        image->SetCompressData(fileData);
    }
#endif

    auto boundsRect = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    auto innerRect = properties.GetBgImageInnerRect();
    canvas.AttachBrush(brush_);
    image->SetInnerRect(std::make_optional<Drawing::RectI>(
        innerRect.x_, innerRect.y_, innerRect.x_ + innerRect.z_, innerRect.y_ + innerRect.w_));
    image->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
}

// blend mode save and restore
std::unique_ptr<RSPropertyDrawable> BlendSaveDrawableGenerate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    int blendModeApplyType = properties.GetColorBlendApplyType();
    if (blendMode == 0) {
        // no blend
        return nullptr;
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "BlendSaveDrawableGenerate::BlendMode, blendMode: %d, blendModeApplyType: %d", blendMode, blendModeApplyType);
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        return std::make_unique<RSBlendFastDrawable>(blendMode);
    }
    return std::make_unique<RSBlendSaveLayerDrawable>(blendMode);
}

std::unique_ptr<RSPropertyDrawable> BlendRestoreDrawableGenerate(const RSRenderContent& content)
{
    auto& properties = content.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    int blendModeApplyType = properties.GetColorBlendApplyType();
    if (blendMode == 0) {
        // no blend
        return nullptr;
    }
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::SAVE_LAYER)) {
        return std::make_unique<RSBlendSaveLayerRestoreDrawable>();
    }
    return std::make_unique<RSBlendFastRestoreDrawable>();
}

RSBlendSaveLayerDrawable::RSBlendSaveLayerDrawable(int blendMode)
{
    blendBrush_.SetBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1)); // map blendMode to Drawing::BlendMode
}

void RSBlendSaveLayerDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    if (!canvas.HasOffscreenLayer() &&
        RSPropertiesPainter::IsDangerousBlendMode(
            static_cast<int>(blendBrush_.GetBlendMode()), static_cast<int>(RSColorBlendApplyType::SAVE_LAYER))) {
        Drawing::SaveLayerOps maskLayerRec(nullptr, nullptr, 0);
        canvas.SaveLayer(maskLayerRec);
        ROSEN_LOGD("Dangerous offscreen blendmode may produce transparent pixels, add extra offscreen here.");
    }
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    auto brush = blendBrush_;
    brush.SetAlphaF(canvas.GetAlpha());
    Drawing::SaveLayerOps maskLayerRec(nullptr, &brush, 0);
    canvas.SaveLayer(maskLayerRec);
    canvas.SetBlendMode(std::nullopt);
    canvas.SaveAlpha();
    canvas.SetAlpha(1.0f);
}

void RSBlendFastDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    if (!canvas.HasOffscreenLayer() &&
        RSPropertiesPainter::IsDangerousBlendMode(blendMode_ - 1, static_cast<int>(RSColorBlendApplyType::FAST))) {
        Drawing::SaveLayerOps maskLayerRec(nullptr, nullptr, 0);
        canvas.SaveLayer(maskLayerRec);
        ROSEN_LOGD("Dangerous fast blendmode may produce transparent pixels, add extra offscreen here.");
    }
    canvas.SetBlendMode({ blendMode_ - 1 }); // map blendMode to SkBlendMode
}

void RSBlendSaveLayerRestoreDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    // SAVE_ALL slot will do all necessary restore
}

void RSBlendFastRestoreDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    // SAVE_ALL slot will do all necessary restore
}

} // namespace OHOS::Rosen
