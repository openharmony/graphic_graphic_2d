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
constexpr float SWEEP_ANGLE = 60.0f;
constexpr float TOP_END = 270.0f;
constexpr float TOP_START = TOP_END - SWEEP_ANGLE;
constexpr float RIGHT_END = 0.0f;
constexpr float RIGHT_START = 360.0f - SWEEP_ANGLE;
constexpr float BOTTOM_END = 90.0f;
constexpr float BOTTOM_START = BOTTOM_END - SWEEP_ANGLE;
constexpr float LEFT_END = 180.0f;
constexpr float LEFT_START = LEFT_END - SWEEP_ANGLE;
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

void RSBorder::SetDashWidth(float dashWidth)
{
    dashWidth_.clear();
    dashWidth_.push_back(dashWidth);
}

void RSBorder::SetDashGap(float dashGap)
{
    dashGap_.clear();
    dashGap_.push_back(dashGap);
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

float RSBorder::GetDashWidth(int idx) const
{
    if (dashWidth_.empty()) {
        // if dashWidth is not set, return -1 and the value will be calculated
        return -1.f;
    } else if (dashWidth_.size() == 1) {
        return dashWidth_.front();
    } else {
        return dashWidth_.at(idx);
    }
}

float RSBorder::GetDashGap(int idx) const
{
    if (dashGap_.empty()) {
        // if dashGap is not set, return -1 and the value will be calculated
        return -1.f;
    } else if (dashGap_.size() == 1) {
        return dashGap_.front();
    } else {
        return dashGap_.at(idx);
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

void RSBorder::SetDashWidthFour(const Vector4f& dashWidth)
{
    if (dashWidth.x_ == dashWidth.y_ && dashWidth.x_ == dashWidth.z_ && dashWidth.x_ == dashWidth.w_) {
        return SetDashWidth(dashWidth.x_);
    }
    dashWidth_ = { dashWidth.x_, dashWidth.y_, dashWidth.z_, dashWidth.w_ };
}

void RSBorder::SetDashGapFour(const Vector4f& dashGap)
{
    if (dashGap.x_ == dashGap.y_ && dashGap.x_ == dashGap.z_ && dashGap.x_ == dashGap.w_) {
        return SetDashGap(dashGap.x_);
    }
    dashGap_ = { dashGap.x_, dashGap.y_, dashGap.z_, dashGap.w_ };
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

Vector4f RSBorder::GetDashWidthFour() const
{
    if (dashWidth_.size() <= 1) {
        return Vector4f(GetDashWidth());
    } else {
        return Vector4f(GetDashWidth(BorderType::LEFT), GetDashWidth(BorderType::TOP),
                        GetDashWidth(BorderType::RIGHT), GetDashWidth(BorderType::BOTTOM));
    }
}

Vector4f RSBorder::GetDashGapFour() const
{
    if (dashGap_.size() <= 1) {
        return Vector4f(GetDashGap());
    } else {
        return Vector4f(GetDashGap(BorderType::LEFT), GetDashGap(BorderType::TOP),
                        GetDashGap(BorderType::RIGHT), GetDashGap(BorderType::BOTTOM));
    }
}

void RSBorder::SetBorderEffect(Drawing::Pen& pen, int idx, float spaceBetweenDot, float borderLength) const
{
    float width = GetWidth(idx);
    if (ROSEN_EQ(width, 0.f)) {
        return;
    }
    BorderStyle style = GetStyle(idx);
    if (style == BorderStyle::DOTTED) {
        Drawing::Path dotPath;
        if (ROSEN_EQ(spaceBetweenDot, 0.f)) {
            spaceBetweenDot = width * PARAM_DOUBLE;
        }
        dotPath.AddCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        pen.SetPathEffect(Drawing::PathEffect::CreatePathDashEffect(dotPath, spaceBetweenDot, 0.0,
            Drawing::PathDashStyle::ROTATE));
        return;
    }
    if (style == BorderStyle::DASHED) {
        float dashWidth = GetDashWidth(idx);
        float dashGap = GetDashGap(idx);
        bool bothZero = ROSEN_EQ(dashWidth, 0.f) && ROSEN_EQ(dashGap, 0.f);
        if (dashWidth >= 0.f && dashGap >= 0.f) {
            // Set fake gap for the case when dashWidth and dashGap params are both zero to prevent solid border line
            float intervals[] = { dashWidth, bothZero ? 1.f : dashGap };
            pen.SetPathEffect(
                Drawing::PathEffect::CreateDashPathEffect(intervals, sizeof(intervals)/sizeof(float), 0.0));
            return;
        }
        double addLen = 0.0; // When left < 2 * gap, splits left to gaps.
        double delLen = 0.0; // When left > 2 * gap, add one dash and shortening them.
        if (!ROSEN_EQ(borderLength, 0.f) && width > 0) {
            float count = borderLength / width;
            float leftLen = fmod((count - DASHED_LINE_LENGTH), (DASHED_LINE_LENGTH + 1));
            if (leftLen > DASHED_LINE_LENGTH - 1) {
                delLen = (DASHED_LINE_LENGTH + 1 - leftLen) * width /
                         static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1) + PARAM_DOUBLE);
            } else {
                addLen = leftLen * width / static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1));
            }
        }
        float intervals[] = {
            (dashWidth >= 0.f ? dashWidth : width * DASHED_LINE_LENGTH - delLen),
            (dashGap >= 0.f ? dashGap : width + addLen) };
        pen.SetPathEffect(Drawing::PathEffect::CreateDashPathEffect(intervals, sizeof(intervals)/sizeof(float), 0.0));
        return;
    }
    pen.SetPathEffect(nullptr);
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
    if (colors_.size() != 1 || widths_.size() != 1 || styles_.size() != 1 ||
        dashWidth_.size() != 1 || dashGap_.size() != 1) {
        return false;
    }
    pen.SetWidth(widths_.front());
    pen.SetColor(colors_.front().AsArgbInt());
    SetBorderEffect(pen, BorderType::LEFT, 0.f, 0.f);
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
    SetBorderEffect(pen, borderIdx, borderLength / rawNumber, borderLength);
    return true;
}

bool RSBorder::ApplySimpleBorder(const RRect& rrect) const
{
    if (!(colors_.size() == 1 && widths_.size() == 1 && styles_.size() == 1)) {
        return false;
    }
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4u;
    for (uint32_t i = 1; i < NUM_OF_CORNERS_IN_RECT; i++) {
        if (rrect.radius_[0].x_ != rrect.radius_[i].x_) {
            return false;
        }
    }
    if (styles_.front() == BorderStyle::SOLID) {
        return true;
    }
    // To avoid artefacts at corner - corner radius should be more than half the stroke width
    return rrect.radius_[0].x_ > widths_.front() / PARAM_DOUBLE;
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

void RSBorder::DrawBorders(Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo) const
{
    CalcBorderPath(borderGeo);

    int sameColorFlag = (CanBeCombined(RSBorder::LEFT, RSBorder::TOP) ? 1 << 3 : 0) |
                        (CanBeCombined(RSBorder::TOP, RSBorder::RIGHT) ? 1 << 2 : 0) |
                        (CanBeCombined(RSBorder::RIGHT, RSBorder::BOTTOM) ? 1 << 1 : 0) |
                        (CanBeCombined(RSBorder::BOTTOM, RSBorder::LEFT) ? 1 : 0);

    switch (sameColorFlag) {
        case 0b0000: // all different
            for (int borderIdx = 0; borderIdx < MAX_BORDER_NUM; borderIdx++) {
                DrawBorderImpl(canvas, pen, borderGeo, borderIdx);
            }
            break;

        case 0b1000: // LT same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT, RSBorder::TOP);
            break;

        case 0b0100: // TR same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP, RSBorder::RIGHT);
            break;

        case 0b0010: // RB same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT, RSBorder::BOTTOM);
            break;

        case 0b0001: // BL same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM, RSBorder::LEFT);
            break;

        case 0b0101: // RB same, LT same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP, RSBorder::RIGHT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM, RSBorder::LEFT);
            break;

        case 0b1010: // LT same, RB same
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT, RSBorder::TOP);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT, RSBorder::BOTTOM);
            break;

        case 0b0110: // only LEFT different
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP, RSBorder::RIGHT, RSBorder::BOTTOM);
            break;

        case 0b0011: // only TOP different
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT, RSBorder::BOTTOM, RSBorder::LEFT);
            break;

        case 0b1001: // only RIGHT different
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM, RSBorder::LEFT, RSBorder::TOP);
            break;

        case 0b1100: // only BOTTOM different
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM);
            DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT, RSBorder::TOP, RSBorder::RIGHT);
            break;

        default:
            ROSEN_LOGE("DrawBorders, style error: %{public}d, draw no border", sameColorFlag);
            break;
    }

    return;
}

void RSBorder::DrawBorderImpl(
    Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx) const
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ClipPath(borderGeo.pathVec[idx], Drawing::ClipOp::INTERSECT, true);
    if (GetStyle(idx) == BorderStyle::SOLID) {
        uint32_t color = GetColor(idx).AsArgbInt();
        DrawNestedRoundRect(canvas, borderGeo, color);
    } else {
        canvas.ClipRoundRect(borderGeo.rrect, Drawing::ClipOp::INTERSECT, true);
        canvas.ClipRoundRect(borderGeo.innerRRect, Drawing::ClipOp::DIFFERENCE, true);
        float width = borderGeo.rrect.GetRect().GetWidth();
        ApplyLineStyle(pen, idx, width);
        if (GetStyle(RSBorder::TOP) != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(LEFTW, TOPW), std::max(RIGHTW, BOTTOMW)));
        }
        switch (idx) {
            case RSBorder::LEFT:
                DrawLeftBorder(canvas, pen, borderGeo);
                break;

            case RSBorder::TOP:
                DrawTopBorder(canvas, pen, borderGeo);
                break;

            case RSBorder::RIGHT:
                DrawRightBorder(canvas, pen, borderGeo);
                break;

            case RSBorder::BOTTOM:
                DrawBottomBorder(canvas, pen, borderGeo);
                break;

            default:
                break;
        }
    }
}

void RSBorder::DrawBorderImpl(
    Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx1, int idx2) const
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path clipPath;
    clipPath.Op(borderGeo.pathVec[idx1], borderGeo.pathVec[idx2], Drawing::PathOp::UNION);
    canvas.ClipPath(clipPath, Drawing::ClipOp::INTERSECT, true);
    DrawNestedRoundRect(canvas, borderGeo, GetColor(idx1).AsArgbInt());
}

void RSBorder::DrawBorderImpl(
    Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx1, int idx2, int idx3) const
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path clipPath;
    clipPath.Op(borderGeo.pathVec[idx1], borderGeo.pathVec[idx2], Drawing::PathOp::UNION);
    clipPath.Op(borderGeo.pathVec[idx3], clipPath, Drawing::PathOp::UNION);
    canvas.ClipPath(clipPath, Drawing::ClipOp::INTERSECT, true);
    DrawNestedRoundRect(canvas, borderGeo, GetColor(idx1).AsArgbInt());
    return;
}

bool RSBorder::CanBeCombined(int idx1, int idx2) const
{
    // same color, both solid style, both width > 0
    if (GetColor(idx1).AsArgbInt() != GetColor(idx2).AsArgbInt() || GetStyle(idx1) != BorderStyle::SOLID ||
        GetStyle(idx2) != BorderStyle::SOLID || ROSEN_LE(GetWidth(idx1), 0.f) ||
        ROSEN_LE(GetWidth(idx2), 0.f)) {
        return false;
    }
    return true;
}

void RSBorder::CalcBorderPath(RSBorderGeo& borderGeo) const
{
    float offsetX = borderGeo.rrect.GetRect().GetLeft();
    float offsetY = borderGeo.rrect.GetRect().GetTop();
    float height = borderGeo.rrect.GetRect().GetHeight();
    float width = borderGeo.rrect.GetRect().GetWidth();

    // calc top-left, top-right, bottom-right, top-right intersection point with innerRect center
    Drawing::Point tlip = GetTLIP(borderGeo.rrect, borderGeo.center);
    Drawing::Point trip = GetTRIP(borderGeo.rrect, borderGeo.center);
    Drawing::Point brip = GetBRIP(borderGeo.rrect, borderGeo.center);
    Drawing::Point blip = GetBLIP(borderGeo.rrect, borderGeo.center);

    if (TOPW > 0.f) {
        borderGeo.pathVec[RSBorder::TOP].MoveTo(offsetX, offsetY);
        borderGeo.pathVec[RSBorder::TOP].LineTo(tlip.GetX(), tlip.GetY());
        borderGeo.pathVec[RSBorder::TOP].LineTo(trip.GetX(), trip.GetY());
        borderGeo.pathVec[RSBorder::TOP].LineTo(offsetX + width, offsetY);
        borderGeo.pathVec[RSBorder::TOP].Close();
    }

    if (RIGHTW > 0.f) {
        borderGeo.pathVec[RSBorder::RIGHT].MoveTo(offsetX + width, offsetY);
        borderGeo.pathVec[RSBorder::RIGHT].LineTo(trip.GetX(), trip.GetY());
        borderGeo.pathVec[RSBorder::RIGHT].LineTo(brip.GetX(), brip.GetY());
        borderGeo.pathVec[RSBorder::RIGHT].LineTo(offsetX + width, offsetY + height);
        borderGeo.pathVec[RSBorder::RIGHT].Close();
    }

    if (BOTTOMW > 0.f) {
        borderGeo.pathVec[RSBorder::BOTTOM].MoveTo(offsetX, offsetY + borderGeo.rrect.GetRect().GetHeight());
        borderGeo.pathVec[RSBorder::BOTTOM].LineTo(blip.GetX(), blip.GetY());
        borderGeo.pathVec[RSBorder::BOTTOM].LineTo(brip.GetX(), brip.GetY());
        borderGeo.pathVec[RSBorder::BOTTOM].LineTo(offsetX + width, offsetY + borderGeo.rrect.GetRect().GetHeight());
        borderGeo.pathVec[RSBorder::BOTTOM].Close();
    }

    if (LEFTW > 0.f) {
        borderGeo.pathVec[RSBorder::LEFT].MoveTo(offsetX, offsetY);
        borderGeo.pathVec[RSBorder::LEFT].LineTo(tlip.GetX(), tlip.GetY());
        borderGeo.pathVec[RSBorder::LEFT].LineTo(blip.GetX(), blip.GetY());
        borderGeo.pathVec[RSBorder::LEFT].LineTo(offsetX, offsetY + height);
        borderGeo.pathVec[RSBorder::LEFT].Close();
    }
    return;
}

void RSBorder::DrawNestedRoundRect(Drawing::Canvas& canvas, const RSBorderGeo& borderGeo, uint32_t color) const
{
    Drawing::Brush brush;
    brush.SetColor(color);
    canvas.AttachBrush(brush);
    canvas.DrawNestedRoundRect(borderGeo.rrect, borderGeo.innerRRect);
    canvas.DetachBrush();
}

void RSBorder::DrawTopBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const
{
    float width = borderGeo.rrect.GetRect().GetWidth();
    float offsetX = borderGeo.rrect.GetRect().GetLeft();
    float offsetY = borderGeo.rrect.GetRect().GetTop();
    float x = offsetX + LEFTW2;
    float y = offsetY + TOPW2;
    Drawing::Point tlRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
    Drawing::Point trRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);

    // calc rectangles to draw left top and right top arcs according to radii values
    float startArcWidth = std::min(width - LEFTW, tlRad.GetX() * 2.f);
    float endArcWidth = std::min(width - RIGHTW, trRad.GetX() * 2.f);
    float startArcHeight = std::min(borderGeo.rrect.GetRect().GetHeight() - TOPW, tlRad.GetY() * 2.f);
    float endArcHeight = std::min(borderGeo.rrect.GetRect().GetHeight() - TOPW, trRad.GetY() * 2.f);
    auto rs = Drawing::Rect(x, y, x + startArcWidth, y + startArcHeight);
    auto re = Drawing::Rect(
        offsetX + width - RIGHTW / 2.f - endArcWidth, y, offsetX + width - RIGHTW / 2.f, y + endArcHeight);
    // create drawing path from left top corner to right top corner
    Drawing::Path topBorder;
    topBorder.MoveTo(std::min(x, offsetX + tlRad.GetX() / 2.f), y + tlRad.GetY());
    topBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), TOP_START, SWEEP_ANGLE);
    topBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), TOP_END, SWEEP_ANGLE);
    topBorder.LineTo(std::max(offsetX + width - RIGHTW2, offsetX + width - trRad.GetX() / 2.f), y + trRad.GetY());
    canvas.AttachPen(pen);
    canvas.DrawPath(topBorder);
    canvas.DetachPen();
}

void RSBorder::DrawRightBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const
{
    float width = borderGeo.rrect.GetRect().GetWidth();
    float height = borderGeo.rrect.GetRect().GetHeight();
    float offsetX = borderGeo.rrect.GetRect().GetLeft();
    float offsetY = borderGeo.rrect.GetRect().GetTop();
    float x = offsetX + width - RIGHTW2;
    float y = offsetY + TOPW2;
    Drawing::Point trRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
    Drawing::Point brRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
    // calc rectangles to draw right top and right bottom arcs according to radii values
    float startArcWidth = std::min(width - RIGHTW, trRad.GetX() * 2.f);
    float endArcWidth = std::min(width - RIGHTW, brRad.GetX() * 2.f);
    float startArcHeight = std::min(height - TOPW, trRad.GetY() * 2.f);
    float endArcHeight = std::min(height - BOTTOMW, brRad.GetY() * 2.f);
    auto rs = Drawing::Rect(x - startArcWidth, y, x, y + startArcHeight);
    auto re = Drawing::Rect(x - endArcWidth, height - BOTTOMW2 - endArcHeight, x, height - BOTTOMW2);
    // create drawing path from right top corner to right bottom corner
    Drawing::Path rightBorder;
    rightBorder.MoveTo(x - trRad.GetX(), std::min(y, offsetY + trRad.GetY() / 2.f));
    rightBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), RIGHT_START, SWEEP_ANGLE);
    rightBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), RIGHT_END, SWEEP_ANGLE);
    rightBorder.LineTo(x - brRad.GetX(), std::max(offsetY + height - BOTTOMW2, offsetY + height - brRad.GetY() / 2.f));
    canvas.AttachPen(pen);
    canvas.DrawPath(rightBorder);
    canvas.DetachPen();
}

void RSBorder::DrawBottomBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const
{
    float width = borderGeo.rrect.GetRect().GetWidth();
    float offsetX = borderGeo.rrect.GetRect().GetLeft();
    float offsetY = borderGeo.rrect.GetRect().GetTop();
    float x = offsetX + LEFTW2;
    float y = offsetY + borderGeo.rrect.GetRect().GetHeight() - BOTTOMW2;
    Drawing::Point brRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
    Drawing::Point blRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
    // calc rectangles to draw right bottom and left bottom arcs according to radii values
    float startArcWidth = std::min(width - RIGHTW, brRad.GetX() * 2.f);
    float endArcWidth = std::min(width - LEFTW, blRad.GetX() * 2.f);
    float startArcHeight = std::min(borderGeo.rrect.GetRect().GetHeight() - BOTTOMW, brRad.GetY() * 2.f);
    float endArcHeight = std::min(borderGeo.rrect.GetRect().GetHeight() - BOTTOMW, blRad.GetY() * 2.f);
    auto rs =
        Drawing::Rect(offsetX + width - RIGHTW2 - startArcWidth, y - startArcHeight, offsetX + width - RIGHTW2, y);
    auto re = Drawing::Rect(x, y - endArcHeight, x + endArcWidth, y);
    // create drawing path from right bottom corner to left bottom corner
    Drawing::Path bottomBorder;
    bottomBorder.MoveTo(std::max(offsetX + width - RIGHTW2, offsetY + width - brRad.GetX() / 2.f), y - brRad.GetY());
    bottomBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), BOTTOM_START, SWEEP_ANGLE);
    bottomBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), BOTTOM_END, SWEEP_ANGLE);
    bottomBorder.LineTo(std::min(x, offsetX + blRad.GetX() / 2.f), y - blRad.GetY());
    canvas.AttachPen(pen);
    canvas.DrawPath(bottomBorder);
    canvas.DetachPen();
}

void RSBorder::DrawLeftBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const
{
    float offsetX = borderGeo.rrect.GetRect().GetLeft();
    float offsetY = borderGeo.rrect.GetRect().GetTop();
    float x = offsetX + LEFTW2;
    float y = offsetY + TOPW2;
    float height = borderGeo.rrect.GetRect().GetHeight();
    Drawing::Point tlRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
    Drawing::Point blRad = borderGeo.rrect.GetCornerRadius(Drawing::RoundRect::BOTTOM_LEFT_POS);
    // calc rectangles to draw left bottom and left top arcs according to radii values
    float startArcWidth = std::min(borderGeo.rrect.GetRect().GetWidth() - LEFTW, blRad.GetX() * 2.f);
    float endArcWidth = std::min(borderGeo.rrect.GetRect().GetWidth() - LEFTW, tlRad.GetX() * 2.f);
    float startArcHeight = std::min(height - BOTTOMW, blRad.GetY() * 2.f);
    float endArcHeight = std::min(height - TOPW, tlRad.GetY() * 2.f);
    auto rs =
        Drawing::Rect(x, offsetY + height - BOTTOMW2 - startArcHeight, x + startArcWidth, offsetY + height - BOTTOMW2);
    auto re = Drawing::Rect(x, y, x + endArcWidth, y + endArcHeight);
    // create drawing path from left bottom corner to left top corner
    Drawing::Path leftBorder;
    leftBorder.MoveTo(x + blRad.GetX(), std::max(offsetY + height - BOTTOMW2, offsetY + height - blRad.GetY() / 2.f));
    leftBorder.ArcTo(rs.GetLeft(), rs.GetTop(), rs.GetRight(), rs.GetBottom(), LEFT_START, SWEEP_ANGLE);
    leftBorder.ArcTo(re.GetLeft(), re.GetTop(), re.GetRight(), re.GetBottom(), LEFT_END, SWEEP_ANGLE);
    leftBorder.LineTo(x + tlRad.GetX(), std::min(y, offsetY + tlRad.GetY() / 2.f));
    canvas.AttachPen(pen);
    canvas.DrawPath(leftBorder);
    canvas.DetachPen();
}

// Return top left intersection pos for clipping
Drawing::Point RSBorder::GetTLIP(const Drawing::RoundRect& rrect, const Drawing::Point& center) const
{
    // inner round rect center point
    float x = center.GetX();
    float y = center.GetY();
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
                y = rrect.GetRect().GetTop() + (x - rrect.GetRect().GetLeft()) * k;
            } else {
                // left top and bottom raduii for inner round rect
                float dty = std::max(tlRad.GetY() - TOPW, 0.f);
                float dby = std::max(blRad.GetY() - BOTTOMW, 0.f);
                // calc delta to prevent overlapping of bottom left corner
                y = (dty > 0) ? (y = y + std::min(dty, height / 2.f - dby)) : (y - height / 2.f);
                x = rrect.GetRect().GetLeft() + (y - rrect.GetRect().GetTop()) / k;
            }
        } else {
            x = rrect.GetRect().GetLeft();
            y = std::max(y - height / 2.f,
                         std::min(rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f,
                                  rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() - BOTTOMW));
        }
    } else {
        y = rrect.GetRect().GetTop() + TOPW;
    }
    return Drawing::Point(x, y);
}

// Return top right intersection pos for clipping
Drawing::Point RSBorder::GetTRIP(const Drawing::RoundRect& rrect, const Drawing::Point& center) const
{
    // inner round rect center point
    float x = center.GetX();
    float y = center.GetY();
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
                y = rrect.GetRect().GetTop() + (rrect.GetRect().GetRight() - x) * k;
            } else {
                // right top and bottom raduii for inner round rect
                float dty = std::max(trRad.GetY() - TOPW, 0.f);
                float dby = std::max(brRad.GetY() - BOTTOMW, 0.f);
                // calc delta to prevent overlapping of bottom right corner
                y = (dty > 0) ? (y = y + std::min(dty, height / 2.f - dby)) : (y - height / 2.f);
                x = rrect.GetRect().GetRight() - (y - rrect.GetRect().GetTop()) / k;
            }
        } else {
            x = rrect.GetRect().GetLeft() + rrect.GetRect().GetWidth();
            y = std::max(y - height / 2.f,
                         std::min(rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f,
                                  rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() - BOTTOMW));
        }
    } else {
        y = rrect.GetRect().GetTop() + TOPW;
    }
    return Drawing::Point(x, y);
}

// Return bottom left intersection pos for clipping
Drawing::Point RSBorder::GetBLIP(const Drawing::RoundRect& rrect, const Drawing::Point& center) const
{
    // inner round rect center point
    float x = center.GetX();
    float y = center.GetY();
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
                y = rrect.GetRect().GetBottom() - (x - rrect.GetRect().GetLeft()) * k;
            } else {
                // left bottom and top raduii for inner round rect
                float dby = std::max(blRad.GetY() - BOTTOMW, 0.f);
                float dty = std::max(tlRad.GetY() - TOPW, 0.f);
                // calc delta to prevent overlapping of top left corner
                y = (dby > 0) ? (y = y - std::min(dby, height / 2.f - dty)) : (y + height / 2.f);
                x = rrect.GetRect().GetLeft() + (rrect.GetRect().GetBottom() - y) / k;
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
Drawing::Point RSBorder::GetBRIP(const Drawing::RoundRect& rrect, const Drawing::Point& center) const
{
    // inner round rect center point
    float x = center.GetX();
    float y = center.GetY();
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
                y = rrect.GetRect().GetBottom() - (rrect.GetRect().GetRight() - x) * k;
            } else {
                // right bottom and top raduii for inner round rect
                float dby = std::max(brRad.GetY() - BOTTOMW, 0.f);
                float dty = std::max(trRad.GetY() - TOPW, 0.f);
                // calc delta to prevent overlapping of top right corner
                y = (dby > 0) ? (y = y - std::min(dby, height / 2.f - dty)) : (y + height / 2.f);
                x = rrect.GetRect().GetRight() - (rrect.GetRect().GetBottom() - y) / k;
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
