/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_border.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#else
#include "draw/path.h"
#endif
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PARAM_DOUBLE = 2;
constexpr int32_t DASHED_LINE_LENGTH = 3;
constexpr float TOP_START = 225.0f;
constexpr float TOP_END = 270.0f;
constexpr float RIGHT_START = 315.0f;
constexpr float RIGHT_END = 0.0f;
constexpr float BOTTOM_START = 45.0f;
constexpr float BOTTOM_END = 90.0f;
constexpr float LEFT_START = 135.0f;
constexpr float LEFT_END = 180.0f;
constexpr float SWEEP_ANGLE = 45.0f;
constexpr float EXTEND = 1024.0f;
} // namespace

RSBorder::RSBorder(const bool& isOutline)
{
    if (isOutline) {
        SetStyle(BorderStyle::SOLID);
        SetColor(Color(0, 0, 0));
    }
}

void RSBorder::SetColor(Color color)
{
    colors_.clear();
    colors_.push_back(color);
}

void RSBorder::SetWidth(float width)
{
    widths_.clear();
    widths_.push_back(width);
}

void RSBorder::SetStyle(BorderStyle style)
{
    styles_.clear();
    styles_.push_back(style);
}

Color RSBorder::GetColor(int idx) const
{
    if (colors_.empty()) {
        return RgbPalette::Transparent();
    } else if (colors_.size() == 1) {
        return colors_.front();
    } else {
        return colors_.at(idx);
    }
}

float RSBorder::GetWidth(int idx) const
{
    if (widths_.empty()) {
        return 0.f;
    } else if (widths_.size() == 1) {
        return widths_.front();
    } else {
        return widths_.at(idx);
    }
}

BorderStyle RSBorder::GetStyle(int idx) const
{
    if (styles_.empty()) {
        return BorderStyle::NONE;
    } else if (styles_.size() == 1) {
        return styles_.front();
    } else {
        return styles_.at(idx);
    }
}

void RSBorder::SetColorFour(const Vector4<Color>& color)
{
    if (color.x_ == color.y_ && color.x_ == color.z_ && color.x_ == color.w_) {
        return SetColor(color.x_);
    }
    colors_ = { color.x_, color.y_, color.z_, color.w_ };
}

void RSBorder::SetWidthFour(const Vector4f& width)
{
    if (width.x_ == width.y_ && width.x_ == width.z_ && width.x_ == width.w_) {
        return SetWidth(width.x_);
    }
    widths_ = { width.x_, width.y_, width.z_, width.w_ };
}

void RSBorder::SetStyleFour(const Vector4<uint32_t>& style)
{
    if (style.x_ == style.y_ && style.x_ == style.z_ && style.x_ == style.w_) {
        return SetStyle(static_cast<BorderStyle>(style.x_));
    }
    styles_ = { static_cast<BorderStyle>(style.x_), static_cast<BorderStyle>(style.y_),
                static_cast<BorderStyle>(style.z_), static_cast<BorderStyle>(style.w_) };
}

void RSBorder::SetRadiusFour(const Vector4f& radius)
{
    radius_ = { radius.x_, radius.y_, radius.z_, radius.w_ };
}

Vector4<Color> RSBorder::GetColorFour() const
{
    if (colors_.size() == 4) {
        return Vector4<Color>(colors_[0], colors_[1], colors_[2], colors_[3]);
    } else {
        return Vector4<Color>(GetColor());
    }
}

Vector4f RSBorder::GetWidthFour() const
{
    if (widths_.size() == 4) {
        return Vector4f(widths_[0], widths_[1], widths_[2], widths_[3]);
    } else {
        return Vector4f(GetWidth());
    }
}

Vector4<uint32_t> RSBorder::GetStyleFour() const
{
    if (styles_.size() == 4) {
        return Vector4<uint32_t>(static_cast<uint32_t>(styles_[0]), static_cast<uint32_t>(styles_[1]),
                                 static_cast<uint32_t>(styles_[2]), static_cast<uint32_t>(styles_[3]));
    } else {
        return Vector4<uint32_t>(static_cast<uint32_t>(GetStyle()));
    }
}

Vector4f RSBorder::GetRadiusFour() const
{
    return radius_;
}

#ifndef USE_ROSEN_DRAWING
void SetBorderEffect(SkPaint& paint, BorderStyle style, float width, float spaceBetweenDot, float borderLength)
#else
void SetBorderEffect(Drawing::Pen& pen, BorderStyle style, float width, float spaceBetweenDot, float borderLength)
#endif
{
    if (ROSEN_EQ(width, 0.f)) {
        return;
    }
    if (style == BorderStyle::DOTTED) {
#ifndef USE_ROSEN_DRAWING
        SkPath dotPath;
#else
        Drawing::Path dotPath;
#endif
        if (ROSEN_EQ(spaceBetweenDot, 0.f)) {
            spaceBetweenDot = width * PARAM_DOUBLE;
        }
#ifndef USE_ROSEN_DRAWING
        dotPath.addCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        paint.setPathEffect(SkPath1DPathEffect::Make(dotPath, spaceBetweenDot, 0.0, SkPath1DPathEffect::kRotate_Style));
#else
        dotPath.AddCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        pen.SetPathEffect(Drawing::PathEffect::CreatePathDashEffect(dotPath, spaceBetweenDot, 0.0,
            Drawing::PathDashStyle::ROTATE));
#endif
    } else if (style == BorderStyle::DASHED) {
        double addLen = 0.0; // When left < 2 * gap, splits left to gaps.
        double delLen = 0.0; // When left > 2 * gap, add one dash and shortening them.
        if (!ROSEN_EQ(borderLength, 0.f)) {
            float count = borderLength / width;
            float leftLen = fmod((count - DASHED_LINE_LENGTH), (DASHED_LINE_LENGTH + 1));
            if (leftLen > DASHED_LINE_LENGTH - 1) {
                delLen = (DASHED_LINE_LENGTH + 1 - leftLen) * width /
                         static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1) + 2);
            } else {
                addLen = leftLen * width / static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1));
            }
        }
        const float intervals[] = { width * DASHED_LINE_LENGTH - delLen, width + addLen };
#ifndef USE_ROSEN_DRAWING
        paint.setPathEffect(SkDashPathEffect::Make(intervals, SK_ARRAY_COUNT(intervals), 0.0));
    } else {
        paint.setPathEffect(nullptr);
#else
        pen.SetPathEffect(Drawing::PathEffect::CreateDashPathEffect(intervals, sizeof(intervals)/sizeof(float), 0.0));
    } else {
        pen.SetPathEffect(nullptr);
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSBorder::ApplyFillStyle(SkPaint& paint) const
#else
bool RSBorder::ApplyFillStyle(Drawing::Brush& brush) const
#endif
{
    if (colors_.size() != 1) {
        return false;
    }
    if (styles_.size() != 1 || GetStyle() != BorderStyle::SOLID) {
        return false;
    }
    for (const float& width : widths_) {
        if (ROSEN_LE(width, 0.f)) {
            return false;
        }
    }
#ifndef USE_ROSEN_DRAWING
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setColor(GetColor().AsArgbInt());
#else
    brush.SetColor(GetColor().AsArgbInt());
#endif
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool RSBorder::ApplyPathStyle(SkPaint& paint) const
#else
bool RSBorder::ApplyPathStyle(Drawing::Pen& pen) const
#endif
{
    if (colors_.size() != 1 || widths_.size() != 1 || styles_.size() != 1) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    paint.setStrokeWidth(widths_.front());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setColor(colors_.front().AsArgbInt());
    SetBorderEffect(paint, GetStyle(), widths_.front(), 0.f, 0.f);
#else
    pen.SetWidth(widths_.front());
    pen.SetColor(colors_.front().AsArgbInt());
    SetBorderEffect(pen, GetStyle(), widths_.front(), 0.f, 0.f);
#endif
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool RSBorder::ApplyFourLine(SkPaint& paint) const
#else
bool RSBorder::ApplyFourLine(Drawing::Pen& pen) const
#endif
{
    if (colors_.size() != 1 || styles_.size() != 1) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    paint.setStyle(SkPaint::Style::kStroke_Style);
#endif
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool RSBorder::ApplyLineStyle(SkPaint& paint, int borderIdx, float length) const
#else
bool RSBorder::ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const
#endif
{
    if (GetWidth(borderIdx) <= 0.0f) {
        return false;
    }
    float borderWidth = GetWidth(borderIdx);
    BorderStyle borderStyle = GetStyle(borderIdx);
    float addLen = (borderStyle != BorderStyle::DOTTED) ? 0.0f : 0.5f;
    auto borderLength = length - borderWidth * addLen * PARAM_DOUBLE;
    int32_t rawNumber = borderLength / (PARAM_DOUBLE * borderWidth);
    if (borderStyle == BorderStyle::DOTTED && rawNumber == 0) {
        return false;
    }

#ifndef USE_ROSEN_DRAWING
    paint.setStrokeWidth(GetWidth(borderIdx));
    Color color = GetColor(borderIdx);
    paint.setColor(color.AsArgbInt());
    SetBorderEffect(paint, GetStyle(borderIdx), borderWidth, borderLength / rawNumber, borderLength);
#else
    pen.SetWidth(GetWidth(borderIdx));
    Color color = GetColor(borderIdx);
    pen.SetColor(color.AsArgbInt());
    SetBorderEffect(pen, GetStyle(borderIdx), borderWidth, borderLength / rawNumber, borderLength);
#endif
    return true;
}

#ifndef USE_ROSEN_DRAWING
void RSBorder::PaintFourLine(SkCanvas& canvas, SkPaint& paint, RectF rect) const
#else
void RSBorder::PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const
#endif
{
    float borderLeftWidth = GetWidth(RSBorder::LEFT);
    float borderRightWidth = GetWidth(RSBorder::RIGHT);
    float borderTopWidth = GetWidth(RSBorder::TOP);
    float borderBottomWidth = GetWidth(RSBorder::BOTTOM);
#ifndef USE_ROSEN_DRAWING
    if (ApplyLineStyle(paint, RSBorder::LEFT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.drawLine(
            rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.top_ + addLen * borderTopWidth,
            rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.GetBottom() - borderBottomWidth, paint);
    }
    if (ApplyLineStyle(paint, RSBorder::RIGHT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::RIGHT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.drawLine(
            rect.GetRight() - borderRightWidth / PARAM_DOUBLE, rect.GetBottom() - addLen * borderBottomWidth,
            rect.GetRight() - borderRightWidth / PARAM_DOUBLE, rect.top_ + borderTopWidth, paint);
    }
    if (ApplyLineStyle(paint, RSBorder::TOP, rect.width_)) {
        float addLen = (GetStyle(RSBorder::TOP) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.drawLine(
            rect.GetRight() - addLen * borderRightWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE,
            rect.left_ + borderLeftWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE, paint);
    }
    if (ApplyLineStyle(paint, RSBorder::BOTTOM, rect.width_)) {
        float addLen = (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.drawLine(
            rect.left_ + addLen * borderLeftWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE,
            rect.GetRight() - borderRightWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE, paint);
    }
#else
    if (ApplyLineStyle(pen, RSBorder::LEFT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.top_ + addLen * borderTopWidth),
            Drawing::Point(rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.GetBottom() - borderBottomWidth));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::RIGHT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::RIGHT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.GetRight() - borderRightWidth / PARAM_DOUBLE,
                rect.GetBottom() - addLen * borderBottomWidth),
            Drawing::Point(rect.GetRight() - borderRightWidth / PARAM_DOUBLE, rect.top_ + borderTopWidth));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::TOP, rect.width_)) {
        float addLen = (GetStyle(RSBorder::TOP) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.GetRight() - addLen * borderRightWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE),
            Drawing::Point(rect.left_ + borderLeftWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::BOTTOM, rect.width_)) {
        float addLen = (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.left_ + addLen * borderLeftWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE),
            Drawing::Point(rect.GetRight() - borderRightWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE));
        canvas.DetachPen();
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSBorder::PaintTopPath(
    SkCanvas& canvas, SkPaint& paint, const SkRRect& rrect, const SkPoint& innerRectCenter) const
#else
void RSBorder::PaintTopPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
#endif
{
#ifndef USE_ROSEN_DRAWING
    float offsetX = rrect.rect().x();
    float offsetY = rrect.rect().y();
    float width = rrect.rect().width();
#else
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
#endif
    auto style = GetStyle(RSBorder::TOP);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
#ifndef USE_ROSEN_DRAWING
    float tlX = std::max(0.0f, rrect.radii(SkRRect::kUpperLeft_Corner).x() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.radii(SkRRect::kUpperLeft_Corner).y() - (topW + leftW) / 4.0f);
    float trX = std::max(0.0f, rrect.radii(SkRRect::kUpperRight_Corner).x() - (topW + rightW) / 4.0f);
    float trY = std::max(0.0f, rrect.radii(SkRRect::kUpperRight_Corner).y() - (topW + rightW) / 4.0f);
#else
    float tlX = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetX() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetY() - (topW + leftW) / 4.0f);
    float trX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetX() - (topW + rightW) / 4.0f);
    float trY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetY() - (topW + rightW) / 4.0f);
#endif
    if (topW > 0.f) {
#ifndef USE_ROSEN_DRAWING
        ApplyLineStyle(paint, RSBorder::TOP, width);
        auto rectStart = SkRect::MakeXYWH(x, y, tlX * 2.0f, tlY * 2.0f);
        auto rectEnd = SkRect::MakeXYWH(x + w - trX * 2.0f, y, trX * 2.0f, trY * 2.0f);
        SkPath topBorder;
        paint.setStrokeWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRect({ offsetX, offsetY, offsetX + width, innerRectCenter.y() });
#else
        ApplyLineStyle(pen, RSBorder::TOP, width);
        auto rectStart = Drawing::Rect(x, y, x + tlX * 2.0f, y + tlY * 2.0f);
        auto rectEnd = Drawing::Rect(x + w - trX * 2.0f, y, x + w, y + trY * 2.0f);
        Drawing::Path topBorder;
        pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, offsetY, offsetX + width, innerRectCenter.GetY() });
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(tlX, 0.f) && !ROSEN_EQ(leftW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            topBorder.moveTo(offsetX, y);
            topBorder.lineTo(x, y);
            SkPath topClipPath;
            topClipPath.moveTo(offsetX - leftW, offsetY - topW);
            topClipPath.lineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.lineTo(offsetX, offsetY + topW * EXTEND);
            topClipPath.close();
            canvas.clipPath(topClipPath, SkClipOp::kDifference, true);
#else
            topBorder.MoveTo(offsetX, y);
            topBorder.LineTo(x, y);
            Drawing::Path topClipPath;
            topClipPath.MoveTo(offsetX - leftW, offsetY - topW);
            topClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.LineTo(offsetX, offsetY + topW * EXTEND);
            topClipPath.Close();
            topClipPath.Offset(-0.5, 0);
            canvas.ClipPath(topClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        topBorder.arcTo(rectStart, TOP_START, SWEEP_ANGLE, false);
        topBorder.arcTo(rectEnd, TOP_END, SWEEP_ANGLE + 0.5f, false);
#else
        topBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            TOP_START, SWEEP_ANGLE);
        topBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            TOP_END, SWEEP_ANGLE + 0.5f);
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(trX, 0.f) && !ROSEN_EQ(rightW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            topBorder.lineTo(offsetX + width, y);
            SkPath topClipPath;
            topClipPath.moveTo(offsetX + width + rightW, offsetY - topW);
            topClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.lineTo(offsetX + width, offsetY + topW * EXTEND);
            topClipPath.close();
            canvas.clipPath(topClipPath, SkClipOp::kDifference, true);
#else
            topBorder.LineTo(offsetX + width, y);
            Drawing::Path topClipPath;
            topClipPath.MoveTo(offsetX + width + rightW, offsetY - topW);
            topClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.LineTo(offsetX + width, offsetY + topW * EXTEND);
            topClipPath.Close();
            topClipPath.Offset(0.5, 0);
            canvas.ClipPath(topClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        if (style == BorderStyle::SOLID) {
            paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
            canvas.drawRect(topBorder.getBounds(), paint);
        } else {
            canvas.drawPath(topBorder, paint);
        }
#else
        canvas.AttachPen(pen);
        if (style == BorderStyle::SOLID) {
            Drawing::Brush brush;
            brush.SetColor(pen.GetColor());
            brush.SetAntiAlias(true);
            canvas.AttachBrush(brush);
            canvas.DrawRect(topBorder.GetBounds());
            canvas.DetachBrush();
        } else {
            canvas.DrawPath(topBorder);
        }
        canvas.DetachPen();
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
void RSBorder::PaintRightPath(
    SkCanvas& canvas, SkPaint& paint, const SkRRect& rrect, const SkPoint& innerRectCenter) const
{
    float offsetX = rrect.rect().x();
    float offsetY = rrect.rect().y();
    float width = rrect.rect().width();
    float height = rrect.rect().height();
#else
void RSBorder::PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
    float height = rrect.GetRect().GetHeight();
#endif
    auto style = GetStyle(RSBorder::RIGHT);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
#ifndef USE_ROSEN_DRAWING
    float trX = std::max(0.0f, rrect.radii(SkRRect::kUpperRight_Corner).x() - (topW + rightW) / 4.0f);
    float trY = std::max(0.0f, rrect.radii(SkRRect::kUpperRight_Corner).y() - (topW + rightW) / 4.0f);
    float brX = std::max(0.0f, rrect.radii(SkRRect::kLowerRight_Corner).x() - (bottomW + rightW) / 4.0f);
    float brY = std::max(0.0f, rrect.radii(SkRRect::kLowerRight_Corner).y() - (bottomW + rightW) / 4.0f);
#else
    float trX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetX() - (topW + rightW) / 4.0f);
    float trY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetY() - (topW + rightW) / 4.0f);
    float brX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetX() - (bottomW + rightW) / 4.0f);
    float brY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetY() - (bottomW + rightW) / 4.0f);
#endif
    if (rightW > 0.f) {
#ifndef USE_ROSEN_DRAWING
        ApplyLineStyle(paint, RSBorder::RIGHT, height);
        auto rectStart = SkRect::MakeXYWH(x + w - trX * 2.0f, y, trX * 2.0f, trY * 2.0f);
        auto rectEnd = SkRect::MakeXYWH(x + w - brX * 2.0f, y + h - brY * 2.0f, brX * 2.0f, brY * 2.0f);
        SkPath rightBorder;
        paint.setStrokeWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRect({ innerRectCenter.x(), offsetY, offsetX + width, offsetY + height });
#else
        ApplyLineStyle(pen, RSBorder::RIGHT, height);
        auto rectStart = Drawing::Rect(x + w - trX * 2.0f, y, x + w, y + trY * 2.0f);
        auto rectEnd = Drawing::Rect(x + w - brX * 2.0f, y + h - brY * 2.0f, x + w, y + h);
        Drawing::Path rightBorder;
        pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ innerRectCenter.GetX(), offsetY, offsetX + width, offsetY + height });
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(trX, 0.f) && !ROSEN_EQ(topW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            rightBorder.moveTo(offsetX + width - rightW / 2.0f, offsetY);
            rightBorder.lineTo(x + w - trX * 2.0f, y);
            SkPath rightClipPath;
            rightClipPath.moveTo(offsetX + width + rightW, offsetY - topW);
            rightClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            rightClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY);
            rightClipPath.close();
            canvas.clipPath(rightClipPath, SkClipOp::kDifference, true);
#else
            rightBorder.MoveTo(offsetX + width - rightW / 2.0f, offsetY);
            rightBorder.LineTo(x + w - trX * 2.0f, y);
            Drawing::Path rightClipPath;
            rightClipPath.MoveTo(offsetX + width + rightW, offsetY - topW);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY);
            rightClipPath.Close();
            rightClipPath.Offset(0, -0.5);
            canvas.ClipPath(rightClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        rightBorder.arcTo(rectStart, RIGHT_START, SWEEP_ANGLE, false);
        rightBorder.arcTo(rectEnd, RIGHT_END, SWEEP_ANGLE + 0.5f, false);
#else
        rightBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            RIGHT_START, SWEEP_ANGLE);
        rightBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            RIGHT_END, SWEEP_ANGLE + 0.5f);
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(brX, 0.f) && !ROSEN_EQ(bottomW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            rightBorder.lineTo(offsetX + width - rightW / 2.0f, offsetY + height);
            SkPath rightClipPath;
            rightClipPath.moveTo(offsetX + width + rightW, offsetY + height + bottomW);
            rightClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            rightClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY + height);
            rightClipPath.close();
            canvas.clipPath(rightClipPath, SkClipOp::kDifference, true);
#else
            rightBorder.LineTo(offsetX + width - rightW / 2.0f, offsetY + height);
            Drawing::Path rightClipPath;
            rightClipPath.MoveTo(offsetX + width + rightW, offsetY + height + bottomW);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height);
            rightClipPath.Close();
            rightClipPath.Offset(0, 0.5);
            canvas.ClipPath(rightClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        if (style == BorderStyle::SOLID) {
            paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
            canvas.drawRect(rightBorder.getBounds(), paint);
        } else {
            canvas.drawPath(rightBorder, paint);
        }
#else
        canvas.AttachPen(pen);
        if (style == BorderStyle::SOLID) {
            Drawing::Brush brush;
            brush.SetColor(pen.GetColor());
            brush.SetAntiAlias(true);
            canvas.AttachBrush(brush);
            canvas.DrawRect(rightBorder.GetBounds());
            canvas.DetachBrush();
        } else {
            canvas.DrawPath(rightBorder);
        }
        canvas.DetachPen();
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
void RSBorder::PaintBottomPath(
    SkCanvas& canvas, SkPaint& paint, const SkRRect& rrect, const SkPoint& innerRectCenter) const
{
    float offsetX = rrect.rect().x();
    float offsetY = rrect.rect().y();
    float width = rrect.rect().width();
    float height = rrect.rect().height();
#else
void RSBorder::PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
    float height = rrect.GetRect().GetHeight();
#endif
    auto style = GetStyle(RSBorder::BOTTOM);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
#ifndef USE_ROSEN_DRAWING
    float brX = std::max(0.0f, rrect.radii(SkRRect::kLowerRight_Corner).x() - (bottomW + rightW) / 4.0f);
    float brY = std::max(0.0f, rrect.radii(SkRRect::kLowerRight_Corner).y() - (bottomW + rightW) / 4.0f);
    float blX = std::max(0.0f, rrect.radii(SkRRect::kLowerLeft_Corner).x() - (bottomW + leftW) / 4.0f);
    float blY = std::max(0.0f, rrect.radii(SkRRect::kLowerLeft_Corner).y() - (bottomW + leftW) / 4.0f);
#else
    float brX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetX() - (bottomW + rightW) / 4.0f);
    float brY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetY() - (bottomW + rightW) / 4.0f);
    float blX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetX() - (bottomW + leftW) / 4.0f);
    float blY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetY() - (bottomW + leftW) / 4.0f);
#endif
    if (bottomW > 0.f) {
#ifndef USE_ROSEN_DRAWING
        ApplyLineStyle(paint, RSBorder::BOTTOM, width);
        auto rectStart = SkRect::MakeXYWH(x + w - brX * 2.0f, y + h - brY * 2.0f, brX * 2.0f, brY * 2.0f);
        auto rectEnd = SkRect::MakeXYWH(x, y + h - blY * 2.0f, blX * 2.0f, blY * 2.0f);
        SkPath bottomBorder;
        if (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) {
            paint.setStrokeWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRect({ offsetX, innerRectCenter.y(), offsetX + width, offsetY + height });
#else
        ApplyLineStyle(pen, RSBorder::BOTTOM, width);
        auto rectStart = Drawing::Rect(x + w - brX * 2.0f, y + h - brY * 2.0f, x + w, y + h);
        auto rectEnd = Drawing::Rect(x, y + h - blY * 2.0f, x + blX * 2.0f, y + h);
        Drawing::Path bottomBorder;
        if (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, innerRectCenter.GetY(), offsetX + width, offsetY + height });
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(brX, 0.f) && !ROSEN_EQ(rightW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            bottomBorder.moveTo(offsetX + width, offsetY + height - bottomW / 2.0f);
            bottomBorder.lineTo(x + w - brX * 2.0f, y + h - brY * 2.0f);
            SkPath bottomClipPath;
            bottomClipPath.moveTo(offsetX + width + rightW, offsetY + height + bottomW);
            bottomClipPath.lineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.lineTo(offsetX + width, offsetY + height - bottomW * EXTEND);
            bottomClipPath.close();
            canvas.clipPath(bottomClipPath, SkClipOp::kDifference, true);
#else
            bottomBorder.MoveTo(offsetX + width, offsetY + height - bottomW / 2.0f);
            bottomBorder.LineTo(x + w - brX * 2.0f, y + h - brY * 2.0f);
            Drawing::Path bottomClipPath;
            bottomClipPath.MoveTo(offsetX + width + rightW, offsetY + height + bottomW);
            bottomClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.LineTo(offsetX + width, offsetY + height - bottomW * EXTEND);
            bottomClipPath.Close();
            bottomClipPath.Offset(0.5, 0);
            canvas.ClipPath(bottomClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        bottomBorder.arcTo(rectStart, BOTTOM_START, SWEEP_ANGLE, false);
        bottomBorder.arcTo(rectEnd, BOTTOM_END, SWEEP_ANGLE + 0.5f, false);
#else
        bottomBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            BOTTOM_START, SWEEP_ANGLE);
        bottomBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            BOTTOM_END, SWEEP_ANGLE + 0.5f);
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(blX, 0.f) && !ROSEN_EQ(leftW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            bottomBorder.lineTo(offsetX, offsetY + height - bottomW / 2.0f);
            SkPath bottomClipPath;
            bottomClipPath.moveTo(offsetX - leftW, offsetY + height + bottomW);
            bottomClipPath.lineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.lineTo(offsetX, offsetY + height - bottomW * EXTEND);
            bottomClipPath.close();
            canvas.clipPath(bottomClipPath, SkClipOp::kDifference, true);
#else
            bottomBorder.LineTo(offsetX, offsetY + height - bottomW / 2.0f);
            Drawing::Path bottomClipPath;
            bottomClipPath.MoveTo(offsetX - leftW, offsetY + height + bottomW);
            bottomClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.LineTo(offsetX, offsetY + height - bottomW * EXTEND);
            bottomClipPath.Close();
            bottomClipPath.Offset(-0.5, 0);
            canvas.ClipPath(bottomClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        if (style == BorderStyle::SOLID) {
            paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
            canvas.drawRect(bottomBorder.getBounds(), paint);
        } else {
            canvas.drawPath(bottomBorder, paint);
        }
#else
        canvas.AttachPen(pen);
        if (style == BorderStyle::SOLID) {
            Drawing::Brush brush;
            brush.SetColor(pen.GetColor());
            brush.SetAntiAlias(true);
            canvas.AttachBrush(brush);
            canvas.DrawRect(bottomBorder.GetBounds());
            canvas.DetachBrush();
        } else {
            canvas.DrawPath(bottomBorder);
        }
        canvas.DetachPen();
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
void RSBorder::PaintLeftPath(
    SkCanvas& canvas, SkPaint& paint, const SkRRect& rrect, const SkPoint& innerRectCenter) const
{
    float offsetX = rrect.rect().x();
    float offsetY = rrect.rect().y();
    float height = rrect.rect().height();
#else
void RSBorder::PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float height = rrect.GetRect().GetHeight();
#endif
    auto style = GetStyle(RSBorder::LEFT);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
#ifndef USE_ROSEN_DRAWING
    float tlX = std::max(0.0f, rrect.radii(SkRRect::kUpperLeft_Corner).x() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.radii(SkRRect::kUpperLeft_Corner).y() - (topW + leftW) / 4.0f);
    float blX = std::max(0.0f, rrect.radii(SkRRect::kLowerLeft_Corner).x() - (bottomW + leftW) / 4.0f);
    float blY = std::max(0.0f, rrect.radii(SkRRect::kLowerLeft_Corner).y() - (bottomW + leftW) / 4.0f);
#else
    float tlX = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetX() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetY() - (topW + leftW) / 4.0f);
    float blX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetX() - (bottomW + leftW) / 4.0f);
    float blY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetY() - (bottomW + leftW) / 4.0f);
#endif
    if (leftW > 0.f) {
#ifndef USE_ROSEN_DRAWING
        ApplyLineStyle(paint, RSBorder::LEFT, height);
        auto rectStart = SkRect::MakeXYWH(x, y + h - blY * 2.0f, blX * 2.0f, blY * 2.0f);
        auto rectEnd = SkRect::MakeXYWH(x, y, tlX * 2.0f, tlY * 2.0f);
        SkPath leftBorder;
        if (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) {
            paint.setStrokeWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRect({ offsetX, offsetY, innerRectCenter.x(), offsetY + height });
#else
        ApplyLineStyle(pen, RSBorder::LEFT, height);
        auto rectStart = Drawing::Rect(x, y + h - blY * 2.0f, x + blX * 2.0f, y + h);
        auto rectEnd = Drawing::Rect(x, y, x + tlX * 2.0f, y + tlY * 2.0f);
        Drawing::Path leftBorder;
        if (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, offsetY, innerRectCenter.GetX(), offsetY + height });
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(blX, 0.f) && !ROSEN_EQ(bottomW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            leftBorder.moveTo(offsetX + leftW / 2.0f, offsetY + height);
            leftBorder.lineTo(x, y + h - blY * 2.0f);
            SkPath leftClipPath;
            leftClipPath.moveTo(offsetX - leftW, offsetY + height + bottomW);
            leftClipPath.lineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            leftClipPath.lineTo(offsetX + leftW * EXTEND, offsetY + height);
            leftClipPath.close();
            canvas.clipPath(leftClipPath, SkClipOp::kDifference, true);
#else
            leftBorder.MoveTo(offsetX + leftW / 2.0f, offsetY + height);
            leftBorder.LineTo(x, y + h - blY * 2.0f);
            Drawing::Path leftClipPath;
            leftClipPath.MoveTo(offsetX - leftW, offsetY + height + bottomW);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height);
            leftClipPath.Close();
            leftClipPath.Offset(0, 0.5);
            canvas.ClipPath(leftClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        leftBorder.arcTo(rectStart, LEFT_START, SWEEP_ANGLE, false);
        leftBorder.arcTo(rectEnd, LEFT_END, SWEEP_ANGLE + 0.5f, false);
#else

        leftBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            LEFT_START, SWEEP_ANGLE);
        leftBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            LEFT_END, SWEEP_ANGLE + 0.5f);
#endif
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(tlX, 0.f) && !ROSEN_EQ(topW, 0.f))) {
#ifndef USE_ROSEN_DRAWING
            leftBorder.lineTo(offsetX + leftW / 2.0f, offsetY);
            SkPath leftClipPath;
            leftClipPath.moveTo(offsetX - leftW, offsetY - topW);
            leftClipPath.lineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            leftClipPath.lineTo(offsetX + leftW * EXTEND, offsetY);
            leftClipPath.close();
            canvas.clipPath(leftClipPath, SkClipOp::kDifference, true);
#else
            leftBorder.LineTo(offsetX + leftW / 2.0f, offsetY);
            Drawing::Path leftClipPath;
            leftClipPath.MoveTo(offsetX - leftW, offsetY - topW);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY);
            leftClipPath.Close();
            leftClipPath.Offset(0, -0.5);
            canvas.ClipPath(leftClipPath, Drawing::ClipOp::DIFFERENCE, true);
#endif
        }
#ifndef USE_ROSEN_DRAWING
        if (style == BorderStyle::SOLID) {
            paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
            canvas.drawRect(leftBorder.getBounds(), paint);
        } else {
            canvas.drawPath(leftBorder, paint);
        }
#else
        canvas.AttachPen(pen);
        if (style == BorderStyle::SOLID) {
            Drawing::Brush brush;
            brush.SetColor(pen.GetColor());
            brush.SetAntiAlias(true);
            canvas.AttachBrush(brush);
            canvas.DrawRect(leftBorder.GetBounds());
            canvas.DetachBrush();
        } else {
            canvas.DrawPath(leftBorder);
        }
        canvas.DetachPen();
#endif
    }
}

std::string RSBorder::ToString() const
{
    std::stringstream ss;
    if (colors_.size() > 0) {
        ss << "colors: ";
    }
    for (auto color : colors_) {
        ss << color.AsArgbInt() << ", ";
    }
    if (widths_.size() > 0) {
        ss << "widths: ";
    }
    for (auto width : widths_) {
        ss << width << ", ";
    }
    if (styles_.size() > 0) {
        ss << "styles: ";
    }
    for (auto style : styles_) {
        ss << static_cast<uint32_t>(style) << ", ";
    }
    std::string output = ss.str();
    return output;
}

bool RSBorder::HasBorder() const
{
    return !colors_.empty() && !widths_.empty() && !styles_.empty() &&
        !std::all_of(colors_.begin(), colors_.end(), [](const Color& color) { return color.GetAlpha() == 0; }) &&
        !std::all_of(widths_.begin(), widths_.end(), [](const float& width) { return width <= 0.f; }) &&
        !std::all_of(
            styles_.begin(), styles_.end(), [](const BorderStyle& style) { return style == BorderStyle::NONE; });
}
} // namespace Rosen
} // namespace OHOS
