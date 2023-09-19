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
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPoint3.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
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

// ============================================================================
// Mask
std::unique_ptr<RSPropertyDrawable> RSMaskDrawable::Generate(const RSProperties& properties)
{
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return nullptr;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        return nullptr;
    }
    if (mask->IsSvgMask()) {
        if (mask->GetSvgDom()) {
            return std::make_unique<RSSvgDomMaskDrawable>(mask, properties.GetBoundsRect());
        } else if (mask->GetSvgPicture()) {
            return std::make_unique<RSSvgPictureMaskDrawable>(mask, properties.GetBoundsRect());
        }
    } else if (mask->IsGradientMask()) {
        return std::make_unique<RSGradientMaskDrawable>(mask, properties.GetBoundsRect());
    } else if (mask->IsPathMask()) {
        return std::make_unique<RSPathMaskDrawable>(mask, properties.GetBoundsRect());
    }
    return nullptr;
}

RSMaskDrawable::RSMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    :mask_(mask)
{
    maskBounds_ = RSPropertiesPainter::Rect2SkRect(bounds);
    maskPaint_.setBlendMode(SkBlendMode::kSrcIn);
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskfilter_.setColorFilter(filter);
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{
}

void RSSvgDomMaskDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskfilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        context.canvas_->scale(mask_->GetScaleX(), mask_->GetScaleY());
        mask_->GetSvgDom()->render(context.canvas_);
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{
}

void RSSvgPictureMaskDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskfilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        context.canvas_->scale(mask_->GetScaleX(), mask_->GetScaleY());
        context.canvas_->drawPicture(mask_->GetSvgPicture());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{
}

void RSGradientMaskDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->Save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskfilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft, maskBounds_.fTop);
        SkRect rect = SkRect::MakeIWH(maskBounds_.fRight - maskBounds_.fLeft, maskBounds_.fBottom - maskBounds_.fTop);
        context.canvas_->drawRect(rect, mask_->GetMaskPaint());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{
}

void RSPathMaskDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskfilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft, maskBounds_.fTop);
        context.canvas_->drawPath(mask_->GetMaskPath(), mask_->GetMaskPaint());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

// ============================================================================
// Shadow
std::unique_ptr<RSPropertyDrawable> RSShadowBaseDrawable::Generate(const RSProperties& properties)
{
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
    SkPath skPath;
    // PLANNING: Generate SkPath and dealwith LeashWindow
    
    if (properties.GetShadowMask()) {
        return std::make_unique<RSColorfulShadowDrawable>(skPath, properties);
    } else {
        if (properties.GetShadow()->GetHardwareAcceleration()) {
            if (properties.GetShadowElevation() <= 0.f) {
                return nullptr;
            }
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(skPath, properties);
        } else {
            return std::make_unique<RSShadowDrawable>(skPath, properties);
        }
    }
}

RSShadowBaseDrawable::RSShadowBaseDrawable(SkPath skPath, const RSProperties& properties)
    :skPath_(skPath)
{
    offsetX_ = properties.GetShadowOffsetX();
    offsetY_ = properties.GetShadowOffsetY();
    color_ = properties.GetShadowColor();
}

RSShadowDrawable::RSShadowDrawable(SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
{
    radius_ = properties.GetShadowRadius();
}

void RSShadowDrawable::Draw(RSModifierContext& context)
{
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(context.canvas_);
    auto matrix = context.canvas_->getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    context.canvas_->setMatrix(matrix);
    SkPaint paint;
    paint.setColor(color_.AsArgbInt());
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_));
    context.canvas_->drawPath(skPath_, paint);
}

RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(
    SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
{
    shadowElevation_ = properties.GetShadowElevation();
}

void RSHardwareAccelerationShadowDrawable::Draw(RSModifierContext& context)
{
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(context.canvas_);
    auto matrix = context.canvas_->getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    context.canvas_->setMatrix(matrix);
    SkPoint3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    SkPoint3 lightPos = { context.canvas_->getTotalMatrix().getTranslateX() + skPath_.getBounds().centerX(),
        context.canvas_->getTotalMatrix().getTranslateY() + skPath_.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(context.canvas_->GetAlpha());
    color_.MultiplyAlpha(context.canvas_->GetAlpha());
    SkShadowUtils::DrawShadow(context.canvas_, skPath_, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
        ambientColor.AsArgbInt(), color_.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
}

RSColorfulShadowDrawable::RSColorfulShadowDrawable(SkPath skPath, const RSProperties& properties)
    :RSShadowBaseDrawable(skPath, properties)
{
    const SkScalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    blurPaint_.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(RSModifierContext& context)
{
    SkAutoCanvasRestore acr(context.canvas_, true);
    // save layer, draw image with clipPath, blur and draw back
    context.canvas_->saveLayer(nullptr, &blurPaint_);
    context.canvas_->translate(offsetX_, offsetY_);
    context.canvas_->clipPath(skPath_);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(*context.canvas_);
    }
}

// ============================================================================
// DynamicLightUp
std::unique_ptr<RSPropertyDrawable> RSDynamicLightUpDrawable::Generate(const RSProperties& properties)
{
    return std::make_unique<RSDynamicLightUpDrawable>(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value());
}

RSDynamicLightUpDrawable::RSDynamicLightUpDrawable(float rate, float degree)
    :rate_(rate), degree_(degree)
{
}

void RSDynamicLightUpDrawable::Draw(RSModifierContext& context)
{
    SkSurface* skSurface = context.canvas_->GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    auto clipBounds = context.canvas_->getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeDynamicLightUpShader(rate_, degree_, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    SkAutoCanvasRestore acr(context.canvas_, true);
    context.canvas_->translate(clipBounds.left(), clipBounds.top());
    context.canvas_->drawPaint(paint);
}

// ============================================================================
// BackgroundEffect
std::unique_ptr<RSPropertyDrawable> RSBackgroundEffectDrawable::Generate(const RSProperties& properties)
{
    return std::make_unique<RSBackgroundEffectDrawable>();
}

void RSBackgroundEffectDrawable::Draw(RSModifierContext& context)
{
    const auto& effectData = context.canvas_->GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(context.canvas_, true);
    context.canvas_->resetMatrix();
    auto visibleIRect = context.canvas_->GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        context.canvas_->clipIRect(visibleIRect);
    }

    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(context.canvas_->getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    SkPaint defaultPaint;
    context.canvas_->drawImageRect(effectData->cachedImage_, srcRect, dstRect,
        SkSamplingOptions(), &defaultPaint, SkCanvas::kFast_SrcRectConstraint);
}
} // namespace OHOS::Rosen
