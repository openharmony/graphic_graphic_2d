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

#include "common/rs_obj_abs_geometry.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
}
namespace OHOS::Rosen {
// ============================================================================
// Bounds geometry
RSBoundsGeometryDrawable::RSBoundsGeometryDrawable(const SkMatrix& boundsMatrix) : boundsMatrix_(boundsMatrix) {}

void RSBoundsGeometryDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->concat(boundsMatrix_);
}
std::unique_ptr<RSPropertyDrawable> RSBoundsGeometryDrawable::Generate(const RSProperties& properties)
{
    auto& matrix = properties.GetBoundsGeometry()->GetMatrix();
    if (matrix.isIdentity()) {
        return nullptr;
    }
    return std::make_unique<RSBoundsGeometryDrawable>(matrix);
}

// ============================================================================
// Border
std::unique_ptr<RSPropertyDrawable> RSBorderDrawable::Generate(const RSProperties& properties)
{
    auto& border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return nullptr;
    }
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
}
RSBorderDRRectDrawable::RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{
    OnGeometryChange(properties);
}
void RSBorderDRRectDrawable::OnGeometryChange(const RSProperties& properties)
{
    inner_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
}
void RSBorderDRRectDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->drawDRRect(outer_, inner_, paint_);
}

RSBorderFourLineDrawable::RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{
    OnGeometryChange(properties);
}
void RSBorderFourLineDrawable::OnGeometryChange(const RSProperties& properties)
{
    rect_ = properties.GetBoundsRect();
}
void RSBorderFourLineDrawable::Draw(RSModifierContext& context)
{
    context.property_.GetBorder()->PaintFourLine(*context.canvas_, paint_, rect_);
}

RSBorderPathDrawable::RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{
    OnGeometryChange(properties);
}
void RSBorderPathDrawable::OnGeometryChange(const RSProperties& properties)
{
    auto borderWidth = properties.GetBorder()->GetWidth();
    RRect rrect = properties.GetRRect();
    rrect.rect_.width_ -= borderWidth;
    rrect.rect_.height_ -= borderWidth;
    rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
    borderPath_.reset();
    borderPath_.addRRect(RSPropertiesPainter::RRect2SkRRect(rrect));
}
void RSBorderPathDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->drawPath(borderPath_, paint_);
}

RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{
    paint_.setStyle(SkPaint::Style::kStroke_Style);
    OnGeometryChange(properties);
}
void RSBorderFourLineRoundCornerDrawable::OnGeometryChange(const RSProperties& properties)
{
    innerRrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
}
void RSBorderFourLineRoundCornerDrawable::Draw(RSModifierContext& context)
{
    SkAutoCanvasRestore acr(context.canvas_, true);
    context.canvas_->clipRRect(innerRrect_, SkClipOp::kDifference, true);
    context.property_.GetBorder()->PaintTopPath(*context.canvas_, paint_, rrect_);
    context.property_.GetBorder()->PaintRightPath(*context.canvas_, paint_, rrect_);
    context.property_.GetBorder()->PaintBottomPath(*context.canvas_, paint_, rrect_);
    context.property_.GetBorder()->PaintLeftPath(*context.canvas_, paint_, rrect_);
}

} // namespace OHOS::Rosen
