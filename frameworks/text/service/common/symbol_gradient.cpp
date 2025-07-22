/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rosen_text/symbol_gradient.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {

void SymbolGradient::SetColors(const std::vector<Drawing::ColorQuad>& colors)
{
    colors_ = colors;
}

void SymbolGradient::SetPositions(const std::vector<float>& positions)
{
    positions_ = positions;
}

void SymbolGradient::SetTileMode(Drawing::TileMode tileMode)
{
    tileMode_ = tileMode;
}

const std::vector<Drawing::ColorQuad>& SymbolGradient::GetColors() const
{
    return colors_;
}

const std::vector<float>& SymbolGradient::GetPositions() const
{
    return positions_;
}

Drawing::TileMode SymbolGradient::GetTileMode() const
{
    return tileMode_;
}

GradientType SymbolGradient::GetGradientType() const
{
    return gradientType_;
}

Drawing::Brush SymbolGradient::CreateGradientBrush()
{
    Drawing::Point offset;
    return CreateGradientBrush(offset);
}

Drawing::Pen SymbolGradient::CreateGradientPen()
{
    Drawing::Point offset;
    return CreateGradientPen(offset);
}

Drawing::Brush SymbolGradient::CreateGradientBrush(const Drawing::Point& offset)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    if (!colors_.empty()) {
        brush.SetColor(colors_[0]);
    }
    return brush;
}

Drawing::Pen SymbolGradient::CreateGradientPen(const Drawing::Point& offset)
{
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    if (!colors_.empty()) {
        pen.SetColor(colors_[0]);
    }
    return pen;
}

std::shared_ptr<Drawing::ShaderEffect> SymbolGradient::CreateGradientShader(const Drawing::Point& offset)
{
    return nullptr;
}

bool SymbolGradient::IsNearlyEqual(const std::shared_ptr<SymbolGradient>& gradient)
{
    if (gradient == nullptr) {
        return false;
    }
    bool isUnequal = gradientType_ != gradient->GetGradientType() || tileMode_ != gradient->GetTileMode() ||
        colors_.size() != gradient->GetColors().size() || positions_.size() != gradient->GetPositions().size();
    if (isUnequal) {
        return false;
    }

    auto colors = gradient->GetColors();
    for (size_t i = 0; i < colors_.size(); i++) {
        if (colors_[i] != colors[i]) {
            return false;
        }
    }

    auto positions = gradient->GetPositions();
    for (size_t i = 0; i < positions_.size(); i++) {
        if (ROSEN_NE<float>(positions_[i], positions[i])) {
            return false;
        }
    }
    return true;
}

SymbolLineGradient::SymbolLineGradient(float rangle) : rangle_(rangle)
{
    gradientType_ = GradientType::LINE_GRADIENT;
}

void SymbolLineGradient::SetAngle(float rangle)
{
    rangle_ = rangle;
}

float SymbolLineGradient::GetAngle() const
{
    return rangle_;
}

void SymbolLineGradient::Make(const Drawing::Rect& bounds)
{
    PointsFromAngle(rangle_, bounds, startPoint_, endPoint_);
    startPoint_.Offset(bounds.GetLeft(), bounds.GetTop());
    endPoint_.Offset(bounds.GetLeft(), bounds.GetTop());
}

Drawing::Brush SymbolLineGradient::CreateGradientBrush(const Drawing::Point& offset)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    auto shader = CreateGradientShader(offset);
    if (shader == nullptr) {
        return brush;
    }
    brush.SetShaderEffect(shader);
    return brush;
}

Drawing::Pen SymbolLineGradient::CreateGradientPen(const Drawing::Point& offset)
{
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    auto shader = CreateGradientShader(offset);
    if (shader == nullptr) {
        return pen;
    }
    pen.SetShaderEffect(shader);
    return pen;
}

std::shared_ptr<Drawing::ShaderEffect> SymbolLineGradient::CreateGradientShader(const Drawing::Point& offset)
{
    if (colors_.empty()) {
        return nullptr;
    }
    auto startPoint = startPoint_;
    auto endPoint = endPoint_;
    startPoint.Offset(offset.GetX(), offset.GetY());
    endPoint.Offset(offset.GetX(), offset.GetY());
    return Drawing::ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors_, positions_, tileMode_);
}

static float AngleToRadian(float angle)
{
    return static_cast<float>(angle * PI / 180.0f); // 180.0f is used for converting angles to radians
}

void SymbolLineGradient::PointsFromAngle(float angle, const Drawing::Rect& bounds,
    Drawing::Point& firstPoint, Drawing::Point& secondPoint)
{
    float width = bounds.GetWidth();
    float height = bounds.GetHeight();
    angle = fmod(angle, 360.0f); // 360.0f is maximum angle
    if (ROSEN_LNE(angle, 0.0f)) {
        angle += 360.0f;
    }
    firstPoint = Drawing::Point(0.0f, 0.0f);
    secondPoint = Drawing::Point(0.0f, 0.0f);

    // 0.0f 90.0f 180.0f 270.0f is the Angle on the coordinate axis
    if (ROSEN_EQ<float>(angle, 0.0f)) {
        firstPoint = Drawing::Point(0.0f, height);
        return;
    } else if (ROSEN_EQ<float>(angle, 90.0f)) {
        secondPoint = Drawing::Point(width, 0.0f);
        return;
    } else if (ROSEN_EQ<float>(angle, 180.0f)) {
        secondPoint = Drawing::Point(0.0f, height);
        return;
    } else if (ROSEN_EQ<float>(angle, 270.0f)) {
        firstPoint = Drawing::Point(width, 0.0f);
        return;
    }
    // The Angle is calculated clockwise from point 0
    float slope = tan(AngleToRadian(90.0f - angle)); // Convert to Cartesian coordinates
    float perpendicularSlope = ROSEN_NE<float>(slope, 0.0f) ? -1 / slope : 1.0f; // the slope will not be zero

    float halfHeight = height / 2; // 2 is half
    float halfWidth = width / 2; // 2 is half
    Drawing::Point cornerPoint { 0.0f, 0.0f };
    if (angle < 90.0f) { // 90.0f: the first term on the coordinate axis
        cornerPoint = Drawing::Point(halfWidth, halfHeight);
    } else if (angle < 180.0f) { // 180.0f: the second term on the coorinate axis
        cornerPoint = Drawing::Point(halfWidth, -halfHeight);
    } else if (angle < 270.0f) { // 270.0f: the third term on the coordinate axis
        cornerPoint = Drawing::Point(-halfWidth, -halfHeight);
    } else {
        cornerPoint = Drawing::Point(-halfWidth, halfHeight);
    }

    // Compute b (of y = kx + b) using the corner point.
    float b = cornerPoint.GetY() - perpendicularSlope * cornerPoint.GetX();
    float endX = b / (slope - perpendicularSlope);
    float endY = perpendicularSlope * endX + b;

    secondPoint = Drawing::Point(halfWidth + endX, halfHeight - endY);
    firstPoint = Drawing::Point(halfWidth - endX, halfHeight + endY);
}


SymbolRadialGradient::SymbolRadialGradient(const Drawing::Point& centerPtRatio, float radiusRatio)
    : centerPtRatio_(centerPtRatio)
{
    radiusRatio_ = std::max(radiusRatio, 0.0f);
    gradientType_ = GradientType::RADIAL_GRADIENT;
}

void SymbolRadialGradient::SetCenterPoint(const Drawing::Point& centerPtRatio)
{
    centerPtRatio_ = centerPtRatio;
}

void SymbolRadialGradient::SetRadiusRatio(float radiusRatio)
{
    radiusRatio_ = std::max(radiusRatio, 0.0f);
    isRadiusRatio_ = true;
}

void SymbolRadialGradient::SetRadius(float radius)
{
    radius_ = std::max(radius, 0.0f);
    isRadiusRatio_ = false;
}

Drawing::Point SymbolRadialGradient::GetCenterPoint() const
{
    return centerPtRatio_;
}

float SymbolRadialGradient::GetRadius() const
{
    return radius_;
}

float SymbolRadialGradient::GetRadiusRatio() const
{
    return radiusRatio_;
}

bool SymbolRadialGradient::IsRadiusRatio() const
{
    return isRadiusRatio_;
}

void SymbolRadialGradient::Make(const Drawing::Rect& bounds)
{
    float left = bounds.GetLeft();
    float top = bounds.GetTop();
    float w = bounds.GetWidth();
    float h = bounds.GetHeight();
    float distance = std::sqrt(w * w + h * h);
    if (isRadiusRatio_) {
        radius_ = radiusRatio_ > 0 ? distance * radiusRatio_ : 0.0f;
    } else {
        radiusRatio_ = distance > 0 ? radius_ / distance : 0.0f;
    }

    w = w * centerPtRatio_.GetX();
    h = h * centerPtRatio_.GetY();
    centerPt_ = { left + w, top + h};
}

Drawing::Brush SymbolRadialGradient::CreateGradientBrush(const Drawing::Point& offset)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    auto shader = CreateGradientShader(offset);
    if (shader == nullptr) {
        return brush;
    }
    brush.SetShaderEffect(shader);
    return brush;
}

Drawing::Pen SymbolRadialGradient::CreateGradientPen(const Drawing::Point& offset)
{
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    auto shader = CreateGradientShader(offset);
    if (shader == nullptr) {
        return pen;
    }
    pen.SetShaderEffect(shader);
    return pen;
}

std::shared_ptr<Drawing::ShaderEffect> SymbolRadialGradient::CreateGradientShader(const Drawing::Point& offset)
{
    if (colors_.empty()) {
        return nullptr;
    }
    auto centerPt = centerPt_;
    centerPt.Offset(offset.GetX(), offset.GetY());
    return Drawing::ShaderEffect::CreateRadialGradient(centerPt, radius_, colors_, positions_, tileMode_);
}
} // namespace OHOS::Rosen