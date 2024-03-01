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

#include "draw/path.h"
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

void SetBorderEffect(Drawing::Pen& pen, BorderStyle style, float width, float spaceBetweenDot, float borderLength)
{
    if (ROSEN_EQ(width, 0.f)) {
        return;
    }
    if (style == BorderStyle::DOTTED) {
        Drawing::Path dotPath;
        if (ROSEN_EQ(spaceBetweenDot, 0.f)) {
            spaceBetweenDot = width * PARAM_DOUBLE;
        }
        dotPath.AddCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        pen.SetPathEffect(Drawing::PathEffect::CreatePathDashEffect(dotPath, spaceBetweenDot, 0.0,
            Drawing::PathDashStyle::ROTATE));
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
        pen.SetPathEffect(Drawing::PathEffect::CreateDashPathEffect(intervals, sizeof(intervals)/sizeof(float), 0.0));
    } else {
        pen.SetPathEffect(nullptr);
    }
}

bool RSBorder::ApplyFillStyle(Drawing::Brush& brush) const
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
    brush.SetColor(GetColor().AsArgbInt());
    return true;
}

bool RSBorder::ApplyPathStyle(Drawing::Pen& pen) const
{
    if (colors_.size() != 1 || widths_.size() != 1 || styles_.size() != 1) {
        return false;
    }
    pen.SetWidth(widths_.front());
    pen.SetColor(colors_.front().AsArgbInt());
    SetBorderEffect(pen, GetStyle(), widths_.front(), 0.f, 0.f);
    return true;
}

bool RSBorder::ApplyFourLine(Drawing::Pen& pen) const
{
    if (colors_.size() != 1 || styles_.size() != 1) {
        return false;
    }
    return true;
}

bool RSBorder::ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const
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

    pen.SetWidth(GetWidth(borderIdx));
    Color color = GetColor(borderIdx);
    pen.SetColor(color.AsArgbInt());
    SetBorderEffect(pen, GetStyle(borderIdx), borderWidth, borderLength / rawNumber, borderLength);
    return true;
}

void RSBorder::PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const
{
    float borderLeftWidth = GetWidth(RSBorder::LEFT);
    float borderRightWidth = GetWidth(RSBorder::RIGHT);
    float borderTopWidth = GetWidth(RSBorder::TOP);
    float borderBottomWidth = GetWidth(RSBorder::BOTTOM);
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
}

void RSBorder::PaintTopPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
    auto style = GetStyle(RSBorder::TOP);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
    float tlX = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetX() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetY() - (topW + leftW) / 4.0f);
    float trX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetX() - (topW + rightW) / 4.0f);
    float trY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetY() - (topW + rightW) / 4.0f);
    if (topW > 0.f) {
        ApplyLineStyle(pen, RSBorder::TOP, width);
        auto rectStart = Drawing::Rect(x, y, x + tlX * 2.0f, y + tlY * 2.0f);
        auto rectEnd = Drawing::Rect(x + w - trX * 2.0f, y, x + w, y + trY * 2.0f);
        Drawing::Path topBorder;
        pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, offsetY, offsetX + width, innerRectCenter.GetY() });
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(tlX, 0.f) && !ROSEN_EQ(leftW, 0.f))) {
            topBorder.MoveTo(offsetX, y);
            topBorder.LineTo(x, y);
            Drawing::Path topClipPath;
            topClipPath.MoveTo(offsetX - leftW, offsetY - topW);
            topClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.LineTo(offsetX, offsetY + topW * EXTEND);
            topClipPath.Close();
            topClipPath.Offset(-0.5, 0);
            canvas.ClipPath(topClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
        topBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            TOP_START, SWEEP_ANGLE);
        topBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            TOP_END, SWEEP_ANGLE + 0.5f);
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(trX, 0.f) && !ROSEN_EQ(rightW, 0.f))) {
            topBorder.LineTo(offsetX + width, y);
            Drawing::Path topClipPath;
            topClipPath.MoveTo(offsetX + width + rightW, offsetY - topW);
            topClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            topClipPath.LineTo(offsetX + width, offsetY + topW * EXTEND);
            topClipPath.Close();
            topClipPath.Offset(0.5, 0);
            canvas.ClipPath(topClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
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
    }
}

void RSBorder::PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
    float height = rrect.GetRect().GetHeight();
    auto style = GetStyle(RSBorder::RIGHT);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
    float trX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetX() - (topW + rightW) / 4.0f);
    float trY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetY() - (topW + rightW) / 4.0f);
    float brX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetX() - (bottomW + rightW) / 4.0f);
    float brY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetY() - (bottomW + rightW) / 4.0f);
    if (rightW > 0.f) {
        ApplyLineStyle(pen, RSBorder::RIGHT, height);
        auto rectStart = Drawing::Rect(x + w - trX * 2.0f, y, x + w, y + trY * 2.0f);
        auto rectEnd = Drawing::Rect(x + w - brX * 2.0f, y + h - brY * 2.0f, x + w, y + h);
        Drawing::Path rightBorder;
        pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ innerRectCenter.GetX(), offsetY, offsetX + width, offsetY + height });
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(trX, 0.f) && !ROSEN_EQ(topW, 0.f))) {
            rightBorder.MoveTo(offsetX + width - rightW / 2.0f, offsetY);
            rightBorder.LineTo(x + w - trX * 2.0f, y);
            Drawing::Path rightClipPath;
            rightClipPath.MoveTo(offsetX + width + rightW, offsetY - topW);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + topW * EXTEND);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY);
            rightClipPath.Close();
            rightClipPath.Offset(0, -0.5);
            canvas.ClipPath(rightClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
        rightBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            RIGHT_START, SWEEP_ANGLE);
        rightBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            RIGHT_END, SWEEP_ANGLE + 0.5f);
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(brX, 0.f) && !ROSEN_EQ(bottomW, 0.f))) {
            rightBorder.LineTo(offsetX + width - rightW / 2.0f, offsetY + height);
            Drawing::Path rightClipPath;
            rightClipPath.MoveTo(offsetX + width + rightW, offsetY + height + bottomW);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            rightClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height);
            rightClipPath.Close();
            rightClipPath.Offset(0, 0.5);
            canvas.ClipPath(rightClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
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
    }
}

void RSBorder::PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float width = rrect.GetRect().GetWidth();
    float height = rrect.GetRect().GetHeight();
    auto style = GetStyle(RSBorder::BOTTOM);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float w = std::max(0.0f, width - (leftW + rightW) / 2.0f);
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
    float brX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetX() - (bottomW + rightW) / 4.0f);
    float brY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetY() - (bottomW + rightW) / 4.0f);
    float blX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetX() - (bottomW + leftW) / 4.0f);
    float blY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetY() - (bottomW + leftW) / 4.0f);
    if (bottomW > 0.f) {
        ApplyLineStyle(pen, RSBorder::BOTTOM, width);
        auto rectStart = Drawing::Rect(x + w - brX * 2.0f, y + h - brY * 2.0f, x + w, y + h);
        auto rectEnd = Drawing::Rect(x, y + h - blY * 2.0f, x + blX * 2.0f, y + h);
        Drawing::Path bottomBorder;
        if (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, innerRectCenter.GetY(), offsetX + width, offsetY + height });
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(brX, 0.f) && !ROSEN_EQ(rightW, 0.f))) {
            bottomBorder.MoveTo(offsetX + width, offsetY + height - bottomW / 2.0f);
            bottomBorder.LineTo(x + w - brX * 2.0f, y + h - brY * 2.0f);
            Drawing::Path bottomClipPath;
            bottomClipPath.MoveTo(offsetX + width + rightW, offsetY + height + bottomW);
            bottomClipPath.LineTo(offsetX + width - rightW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.LineTo(offsetX + width, offsetY + height - bottomW * EXTEND);
            bottomClipPath.Close();
            bottomClipPath.Offset(0.5, 0);
            canvas.ClipPath(bottomClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
        bottomBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            BOTTOM_START, SWEEP_ANGLE);
        bottomBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            BOTTOM_END, SWEEP_ANGLE + 0.5f);
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(blX, 0.f) && !ROSEN_EQ(leftW, 0.f))) {
            bottomBorder.LineTo(offsetX, offsetY + height - bottomW / 2.0f);
            Drawing::Path bottomClipPath;
            bottomClipPath.MoveTo(offsetX - leftW, offsetY + height + bottomW);
            bottomClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            bottomClipPath.LineTo(offsetX, offsetY + height - bottomW * EXTEND);
            bottomClipPath.Close();
            bottomClipPath.Offset(-0.5, 0);
            canvas.ClipPath(bottomClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
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
    }
}

void RSBorder::PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    float offsetX = rrect.GetRect().GetLeft();
    float offsetY = rrect.GetRect().GetTop();
    float height = rrect.GetRect().GetHeight();
    auto style = GetStyle(RSBorder::LEFT);
    float leftW = GetWidth(RSBorder::LEFT);
    float topW = GetWidth(RSBorder::TOP);
    float rightW = GetWidth(RSBorder::RIGHT);
    float bottomW = GetWidth(RSBorder::BOTTOM);
    float x = offsetX + leftW / 2.0f;
    float y = offsetY + topW / 2.0f;
    float h = std::max(0.0f, height - (topW + bottomW) / 2.0f);
    float tlX = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetX() - (topW + leftW) / 4.0f);
    float tlY = std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS).GetY() - (topW + leftW) / 4.0f);
    float blX =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetX() - (bottomW + leftW) / 4.0f);
    float blY =
        std::max(0.0f, rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS).GetY() - (bottomW + leftW) / 4.0f);
    if (leftW > 0.f) {
        ApplyLineStyle(pen, RSBorder::LEFT, height);
        auto rectStart = Drawing::Rect(x, y + h - blY * 2.0f, x + blX * 2.0f, y + h);
        auto rectEnd = Drawing::Rect(x, y, x + tlX * 2.0f, y + tlY * 2.0f);
        Drawing::Path leftBorder;
        if (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRect({ offsetX, offsetY, innerRectCenter.GetX(), offsetY + height });
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(blX, 0.f) && !ROSEN_EQ(bottomW, 0.f))) {
            leftBorder.MoveTo(offsetX + leftW / 2.0f, offsetY + height);
            leftBorder.LineTo(x, y + h - blY * 2.0f);
            Drawing::Path leftClipPath;
            leftClipPath.MoveTo(offsetX - leftW, offsetY + height + bottomW);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height - bottomW * EXTEND);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + height);
            leftClipPath.Close();
            leftClipPath.Offset(0, 0.5);
            canvas.ClipPath(leftClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }

        leftBorder.ArcTo(rectStart.GetLeft(), rectStart.GetTop(), rectStart.GetRight(), rectStart.GetBottom(),
            LEFT_START, SWEEP_ANGLE);
        leftBorder.ArcTo(rectEnd.GetLeft(), rectEnd.GetTop(), rectEnd.GetRight(), rectEnd.GetBottom(),
            LEFT_END, SWEEP_ANGLE + 0.5f);
        if ((style == BorderStyle::SOLID) || (ROSEN_EQ(tlX, 0.f) && !ROSEN_EQ(topW, 0.f))) {
            leftBorder.LineTo(offsetX + leftW / 2.0f, offsetY);
            Drawing::Path leftClipPath;
            leftClipPath.MoveTo(offsetX - leftW, offsetY - topW);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY + topW * EXTEND);
            leftClipPath.LineTo(offsetX + leftW * EXTEND, offsetY);
            leftClipPath.Close();
            leftClipPath.Offset(0, -0.5);
            canvas.ClipPath(leftClipPath, Drawing::ClipOp::DIFFERENCE, true);
        }
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
