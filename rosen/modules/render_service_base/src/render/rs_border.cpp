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
} // namespace

// defines short names for widths/half widths of each borders
#define LEFTW GetWidth(RSBorder::LEFT)
#define LEFTW2 GetWidth(RSBorder::LEFT) / 2.f
#define RIGHTW GetWidth(RSBorder::RIGHT)
#define RIGHTW2 GetWidth(RSBorder::RIGHT) / 2.f
#define TOPW GetWidth(RSBorder::TOP)
#define TOPW2 GetWidth(RSBorder::TOP) / 2.f
#define BOTTOMW GetWidth(RSBorder::BOTTOM)
#define BOTTOMW2 GetWidth(RSBorder::BOTTOM) / 2.f

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
    if (TOPW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float width = rrect.GetRect().GetWidth();
        ApplyLineStyle(pen, RSBorder::TOP, width);
        if (GetStyle(RSBorder::TOP) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(LEFTW, TOPW), std::max(RIGHTW, BOTTOMW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::Path topClipPath;
        // top left intersection point with innerRect center
        Drawing::Point tlip = GetTLIP(rrect, innerRectCenter);
        // top right intersection point with innerRect center
        Drawing::Point trip = GetTRIP(rrect, innerRectCenter);
        // draw clipping path for top border
        topClipPath.MoveTo(offsetX, offsetY);
        topClipPath.LineTo(tlip.GetX(), tlip.GetY());
        topClipPath.LineTo(trip.GetX(), trip.GetY());
        topClipPath.LineTo(offsetX + width, offsetY);
        topClipPath.Close();
        canvas.ClipPath(topClipPath, Drawing::ClipOp::INTERSECT, true);
        DrawTopBorder(canvas, pen, rrect, offsetX, offsetY);
    }
}

void RSBorder::DrawTopBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const float offsetX, const float offsetY) const
{
    float width = rrect.GetRect().GetWidth();
    float x = offsetX + LEFTW2;
    float y = offsetY + TOPW2;

    Drawing::Point tlRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
    Drawing::Point trRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);

    // calc rectangles to draw left top and right top arcs according to radii values
    float startArcWidth = std::min(width - LEFTW, tlRad.GetX() * 2.f);
    float endArcWidth = std::min(width - RIGHTW, trRad.GetX() * 2.f);
    float startArcHeight = std::min(rrect.GetRect().GetHeight() - TOPW, tlRad.GetY() * 2.f);
    float endArcHeight = std::min(rrect.GetRect().GetHeight() - TOPW, trRad.GetY() * 2.f);
    auto rs = Drawing::Rect(x, y, x + startArcWidth, y + startArcHeight);
    auto re = Drawing::Rect(
        offsetX + width - RIGHTW / 2.f - endArcWidth, y, offsetX + width - RIGHTW / 2.f, y + endArcHeight);
    // create drawing path from left top corner to right top corner
    Drawing::Path topBorder;
    topBorder.MoveTo(std::min(x, offsetX + tlRad.GetX() / 2.f), y + tlRad.GetY() / 2.f);
    topBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), TOP_START, SWEEP_ANGLE);
    topBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), TOP_END, SWEEP_ANGLE);
    topBorder.LineTo(std::max(offsetX + width - RIGHTW2, offsetX + width - trRad.GetX() / 2.f), y + trRad.GetY() / 2.f);
    canvas.AttachPen(pen);
    if (GetStyle(RSBorder::TOP) == BorderStyle::SOLID) {
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

void RSBorder::PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    if (RIGHTW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float height = rrect.GetRect().GetHeight();
        float width = rrect.GetRect().GetWidth();
        ApplyLineStyle(pen, RSBorder::RIGHT, height);
        if (GetStyle(RSBorder::RIGHT) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(LEFTW, TOPW), std::max(RIGHTW, BOTTOMW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::Path rightClipPath;
        // top rigth intersection point with innerRect center
        Drawing::Point trip = GetTRIP(rrect, innerRectCenter);
        // bottom right intersection point with innerRect center
        Drawing::Point brip = GetBRIP(rrect, innerRectCenter);
        // draw clipping path for right border
        rightClipPath.MoveTo(offsetX + width, offsetY);
        rightClipPath.LineTo(trip.GetX(), trip.GetY());
        rightClipPath.LineTo(brip.GetX(), brip.GetY());
        rightClipPath.LineTo(offsetX + width, offsetY + height);
        rightClipPath.Close();
        canvas.ClipPath(rightClipPath, Drawing::ClipOp::INTERSECT, true);
        DrawRightBorder(canvas, pen, rrect, offsetX, offsetY);
    }
}

void RSBorder::DrawRightBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const float offsetX, const float offsetY) const
{
    float width = rrect.GetRect().GetWidth();
    float height = rrect.GetRect().GetHeight();
    float x = offsetX + width - RIGHTW2;
    float y = offsetY + TOPW2;
    Drawing::Point trRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
    Drawing::Point brRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
    // calc rectangles to draw right top and right bottom arcs according to radii values
    float startArcWidth = std::min(width - RIGHTW, trRad.GetX() * 2.f);
    float endArcWidth = std::min(width - RIGHTW, brRad.GetX() * 2.f);
    float startArcHeight = std::min(height - TOPW, trRad.GetY() * 2.f);
    float endArcHeight = std::min(height - BOTTOMW, brRad.GetY() * 2.f);
    auto rs = Drawing::Rect(x - startArcWidth, y, x, y + startArcHeight);
    auto re = Drawing::Rect(x - endArcWidth, height - BOTTOMW2 - endArcHeight, x, height - BOTTOMW2);
    // create drawing path from right top corner to right bottom corner
    Drawing::Path rightBorder;
    rightBorder.MoveTo(x - trRad.GetX() / 2.f, std::min(y, offsetY + trRad.GetY() / 2.f));
    rightBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), RIGHT_START, SWEEP_ANGLE);
    rightBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), RIGHT_END, SWEEP_ANGLE);
    rightBorder.LineTo(
        x - brRad.GetX() / 2.f, std::max(offsetY + height - BOTTOMW2, offsetY + height - brRad.GetY() / 2.f));
    canvas.AttachPen(pen);
    if (GetStyle(RSBorder::RIGHT) == BorderStyle::SOLID) {
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

void RSBorder::PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    if (BOTTOMW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float width = rrect.GetRect().GetWidth();
        ApplyLineStyle(pen, RSBorder::BOTTOM, width);
        if (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(LEFTW, TOPW), std::max(RIGHTW, BOTTOMW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        // bottom left intersection point with innerRect center
        Drawing::Point blip = GetBLIP(rrect, innerRectCenter);
        // bottom right intersection point with innerRect center
        Drawing::Point brip = GetBRIP(rrect, innerRectCenter);
        // draw clipping path for bottom border
        Drawing::Path bottomClipPath;
        bottomClipPath.MoveTo(offsetX, offsetY + rrect.GetRect().GetHeight());
        bottomClipPath.LineTo(blip.GetX(), blip.GetY());
        bottomClipPath.LineTo(brip.GetX(), brip.GetY());
        bottomClipPath.LineTo(offsetX + width, offsetY + rrect.GetRect().GetHeight());
        bottomClipPath.Close();
        canvas.ClipPath(bottomClipPath, Drawing::ClipOp::INTERSECT, true);
        DrawBottomBorder(canvas, pen, rrect, offsetX, offsetY);
    }
}

void RSBorder::DrawBottomBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const float offsetX, const float offsetY) const
{
    float width = rrect.GetRect().GetWidth();
    float x = offsetX + LEFTW2;
    float y = offsetY + rrect.GetRect().GetHeight() - BOTTOMW2;
    Drawing::Point brRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
    Drawing::Point blRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
    // calc rectangles to draw right bottom and left bottom arcs according to radii values
    float startArcWidth = std::min(width - RIGHTW, brRad.GetX() * 2.f);
    float endArcWidth = std::min(width - LEFTW, blRad.GetX() * 2.f);
    float startArcHeight = std::min(rrect.GetRect().GetHeight() - BOTTOMW, brRad.GetY() * 2.f);
    float endArcHeight = std::min(rrect.GetRect().GetHeight() - BOTTOMW, blRad.GetY() * 2.f);
    auto rs = Drawing::Rect(offsetX + width - RIGHTW2 - startArcWidth, y - startArcHeight,
                            offsetX + width - RIGHTW2, y);
    auto re = Drawing::Rect(x, y - endArcHeight, x + endArcWidth, y);
    // create drawing path from right bottom corner to left bottom corner
    Drawing::Path bottomBorder;
    bottomBorder.MoveTo(std::max(offsetX + width - RIGHTW2, offsetY + width - brRad.GetX() / 2.f),
                        y - brRad.GetY() / 2.f);
    bottomBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), BOTTOM_START, SWEEP_ANGLE);
    bottomBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), BOTTOM_END, SWEEP_ANGLE);
    bottomBorder.LineTo(std::min(x, offsetX + blRad.GetX() / 2.f), y - blRad.GetY() / 2.f);
    canvas.AttachPen(pen);
    if (GetStyle(RSBorder::BOTTOM) == BorderStyle::SOLID) {
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

void RSBorder::PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const Drawing::Point& innerRectCenter) const
{
    if (LEFTW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float height = rrect.GetRect().GetHeight();
        ApplyLineStyle(pen, RSBorder::LEFT, height);
        if (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(LEFTW, TOPW), std::max(RIGHTW, BOTTOMW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::Path leftClipPath;
        // top left intersection point with innerRect center
        Drawing::Point tlip = GetTLIP(rrect, innerRectCenter);
        // bottom left intersection point with innerRect center
        Drawing::Point blip = GetBLIP(rrect, innerRectCenter);
        // draw clipping path for left border
        leftClipPath.MoveTo(offsetX, offsetY);
        leftClipPath.LineTo(tlip.GetX(), tlip.GetY());
        leftClipPath.LineTo(blip.GetX(), blip.GetY());
        leftClipPath.LineTo(offsetX, offsetY + height);
        leftClipPath.Close();
        canvas.ClipPath(leftClipPath, Drawing::ClipOp::INTERSECT, true);
        DrawLeftBorder(canvas, pen, rrect, offsetX, offsetY);
    }
}

void RSBorder::DrawLeftBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const float offsetX, const float offsetY) const
{
    float x = offsetX + LEFTW2;
    float y = offsetY + TOPW2;
    float height = rrect.GetRect().GetHeight();
    Drawing::Point tlRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
    Drawing::Point blRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
    // calc rectangles to draw left bottom and left top arcs according to radii values
    float startArcWidth = std::min(rrect.GetRect().GetWidth() - LEFTW, blRad.GetX() * 2.f);
    float endArcWidth = std::min(rrect.GetRect().GetWidth() - LEFTW, tlRad.GetX() * 2.f);
    float startArcHeight = std::min(height - BOTTOMW, blRad.GetY() * 2.f);
    float endArcHeight = std::min(height - TOPW, tlRad.GetY() * 2.f);
    auto rs = Drawing::Rect(x, offsetY + height - BOTTOMW2 - startArcHeight,
                            x + startArcWidth, offsetY + height - BOTTOMW2);
    auto re = Drawing::Rect(x, y, x + endArcWidth, y + endArcHeight);
    // create drawing path from left bottom corner to left top corner
    Drawing::Path leftBorder;
    leftBorder.MoveTo(
        x + blRad.GetX() / 2.f, std::max(offsetY + height - BOTTOMW2, offsetY + height - blRad.GetY() / 2.f));
    leftBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), LEFT_START, SWEEP_ANGLE);
    leftBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), LEFT_END, SWEEP_ANGLE);
    leftBorder.LineTo(x + tlRad.GetX() / 2.f, std::min(y, offsetY + tlRad.GetY() / 2.f));
    canvas.AttachPen(pen);
    if (GetStyle(RSBorder::LEFT) == BorderStyle::SOLID) {
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

// Return top left intersection pos for clipping
Drawing::Point RSBorder::GetTLIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const
{
    // inner round rect center point
    float x = innerRectCenter.GetX();
    float y = innerRectCenter.GetY();
    // width/height of inner round rect
    float width = rrect.GetRect().GetWidth() - LEFTW - RIGHTW;
    float height = rrect.GetRect().GetHeight() - TOPW - BOTTOMW;
    if (width > 0) {
        // kc is linear ratio of inner rect
        float kc = height / width;
        if (LEFTW > 0) {
            // k is linear ratio for external rect (cutting angle at top left corner)
            float k = TOPW / LEFTW;
            // raduii values of external round rect for calculating clipping point
            Drawing::Point tlRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
            Drawing::Point trRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
            Drawing::Point blRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
            // shows what center axis of inner round rect we intersect fist (x or y)
            if (k <= kc) {
                // top left and right raduii for inner round rect
                float dlx = std::max(tlRad.GetX() - LEFTW, 0.f);
                float drx = std::max(trRad.GetX() - RIGHTW, 0.f);
                // calc delta to prevent overlapping of top right corner
                x = (dlx > 0) ? (x + std::min(dlx, width / 2.f - drx)) : (x - width / 2.f);
                y = x * k;
            } else {
                // left top and bottom raduii for inner round rect
                float dty = std::max(tlRad.GetY() - TOPW, 0.f);
                float dby = std::max(blRad.GetY() - BOTTOMW, 0.f);
                // calc delta to prevent overlapping of bottom left corner
                y = (dty > 0) ? (y = y + std::min(dty, height / 2.f - dby)) : (y - height / 2.f);
                x = y / k;
            }
        } else {
            x = rrect.GetRect().GetLeft();
            y = std::max(y - height / 2.f, rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f);
        }
    } else {
        y = rrect.GetRect().GetTop() + TOPW;
    }
    return Drawing::Point(x, y);
}

// Return top right intersection pos for clipping
Drawing::Point RSBorder::GetTRIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const
{
    // inner round rect center point
    float x = innerRectCenter.GetX();
    float y = innerRectCenter.GetY();
    // width/height of inner round rect
    float width = rrect.GetRect().GetWidth() - LEFTW - RIGHTW;
    float height = rrect.GetRect().GetHeight() - TOPW - BOTTOMW;
    if (width > 0) {
        // kc is linear ratio of inner rect
        float kc = height / width;
        if (RIGHTW > 0) {
            // k is linear ratio for external rect (cutting angle at top right corner)
            float k = TOPW / RIGHTW;
            // raduii values of external round rect for calculating clipping point
            Drawing::Point trRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
            Drawing::Point tlRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
            Drawing::Point brRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
            // shows what center axis of inner round rect we intersect fist (x or y)
            if (k <= kc) {
                // top left and right raduii for inner round rect
                float drx = std::max(trRad.GetX() - RIGHTW, 0.f);
                float dlx = std::max(tlRad.GetX() - LEFTW, 0.f);
                // calc delta to prevent overlapping of top left corner
                x = (drx > 0) ? (x - std::min(drx, width / 2.f - dlx)) : (x + width / 2.f);
                y = (rrect.GetRect().GetWidth() - x) * k;
            } else {
                // right top and bottom raduii for inner round rect
                float dty = std::max(trRad.GetY() - TOPW, 0.f);
                float dby = std::max(brRad.GetY() - BOTTOMW, 0.f);
                // calc delta to prevent overlapping of bottom right corner
                y = (dty > 0) ? (y = y + std::min(dty, height / 2.f - dby)) : (y - height / 2.f);
                x = rrect.GetRect().GetWidth() - y / k;
            }
        } else {
            x = rrect.GetRect().GetLeft() + rrect.GetRect().GetWidth();
            y = std::max(y - height / 2.f, rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f);
        }
    } else {
        y = rrect.GetRect().GetTop() + TOPW;
    }
    return Drawing::Point(x, y);
}

// Return bottom left intersection pos for clipping
Drawing::Point RSBorder::GetBLIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const
{
    // inner round rect center point
    float x = innerRectCenter.GetX();
    float y = innerRectCenter.GetY();
    // width/height of inner round rect
    float width = rrect.GetRect().GetWidth() - LEFTW - RIGHTW;
    float height = rrect.GetRect().GetHeight() - TOPW - BOTTOMW;
    if (width > 0) {
        // kc is linear ratio of inner rect
        float kc = height / width;
        if (LEFTW > 0) {
            // k is linear ratio for external rect (cutting angle at bottom left corner)
            float k = BOTTOMW / LEFTW;
            // raduii values of external round rect for calculating clipping point
            Drawing::Point blRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
            Drawing::Point tlRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
            Drawing::Point brRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
            // shows what center axis of inner round rect we intersect fist (x or y)
            if (k <= kc) {
                // bottom left and right raduii for inner round rect
                float dlx = std::max(blRad.GetX() - LEFTW, 0.f);
                float drx = std::max(brRad.GetX() - RIGHTW, 0.f);
                // calc delta to prevent overlapping of bottom right corner
                x = (dlx > 0) ? (x + std::min(dlx, width / 2.f - drx)) : (x - width / 2.f);
                y = rrect.GetRect().GetHeight() - x * k;
            } else {
                // left bottom and top raduii for inner round rect
                float dby = std::max(blRad.GetY() - BOTTOMW, 0.f);
                float dty = std::max(tlRad.GetY() - TOPW, 0.f);
                // calc delta to prevent overlapping of top left corner
                y = (dby > 0) ? (y = y - std::min(dby, height / 2.f - dty)) : (y + height / 2.f);
                x = (rrect.GetRect().GetHeight() - y) / k;
            }
        } else {
            x = rrect.GetRect().GetLeft();
            y = std::min(y + height / 2.f,
                         std::max(rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f,
                                  rrect.GetRect().GetTop() + TOPW));
        }
    } else {
        y = rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() - BOTTOMW;
    }
    return Drawing::Point(x, y);
}

// Return bottom right intersection pos for clipping
Drawing::Point RSBorder::GetBRIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const
{
    // inner round rect center point
    float x = innerRectCenter.GetX();
    float y = innerRectCenter.GetY();
    // width/height of inner round rect
    float width = rrect.GetRect().GetWidth() - LEFTW - RIGHTW;
    float height = rrect.GetRect().GetHeight() - TOPW - BOTTOMW;
    if (width > 0) {
        // kc is linear ratio of inner rect
        float kc = height / width;
        if (RIGHTW > 0) {
            // k is linear ratio for external rect (cutting angle at bottom right corner)
            float k = BOTTOMW / RIGHTW;
            // raduii values of external round rect for calculating clipping point
            Drawing::Point brRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
            Drawing::Point blRad = rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
            Drawing::Point trRad = rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
            // shows what center axis of inner round rect we intersect fist (x or y)
            if (k <= kc) {
                // bottom left and right raduii for inner round rect
                float drx = std::max(brRad.GetX() - RIGHTW, 0.f);
                float dlx = std::max(blRad.GetX() - LEFTW, 0.f);
                // calc delta to prevent overlapping of bottom left corner
                x = (drx > 0) ? (x - std::min(drx, width / 2.f - dlx)) : (x + width / 2.f);
                y = rrect.GetRect().GetHeight() - (rrect.GetRect().GetWidth() - x) * k;
            } else {
                // right bottom and top raduii for inner round rect
                float dby = std::max(brRad.GetY() - BOTTOMW, 0.f);
                float dty = std::max(trRad.GetY() - TOPW, 0.f);
                // calc delta to prevent overlapping of top right corner
                y = (dby > 0) ? (y = y - std::min(dby, height / 2.f - dty)) : (y + height / 2.f);
                x = rrect.GetRect().GetWidth() - (rrect.GetRect().GetHeight() - y) / k;
            }
        } else {
            x = rrect.GetRect().GetLeft() + rrect.GetRect().GetWidth();
            y = std::min(y + height / 2.f,
                         std::max(rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f,
                                  rrect.GetRect().GetTop() + TOPW));
        }
    } else {
        y = rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() - BOTTOMW;
    }
    return Drawing::Point(x, y);
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
