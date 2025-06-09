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

#ifndef ROSEN_TEXT_SYMBOL_GRADIENT_H
#define ROSEN_TEXT_SYMBOL_GRADIENT_H

#include <vector>

#include "common/rs_common_def.h"
#include "draw/color.h"
#include "draw/brush.h"
#include "draw/pen.h"
#include "utils/rect.h"

namespace OHOS::Rosen {
enum GradientType {
    NONE_GRADIENT,
    LINE_GRADIENT,
    RADIAL_GRADIENT,
};

class SymbolGradient {
public:
    SymbolGradient() = default;
    virtual ~SymbolGradient() = default;

    void SetColors(const std::vector<Drawing::ColorQuad>& colors);

    void SetPositions(const std::vector<float>& positions);

    void SetTileMode(Drawing::TileMode tileMode);

    const std::vector<Drawing::ColorQuad>& GetColors() const;

    const std::vector<float>& GetPositions() const;

    Drawing::TileMode GetTileMode() const;

    GradientType GetGradientType() const;

    Drawing::Brush CreateGradientBrush();

    Drawing::Pen CreateGradientPen();

    virtual void Make(const Drawing::Rect& bounds) {}

    virtual Drawing::Brush CreateGradientBrush(const Drawing::Point& offset);

    virtual Drawing::Pen CreateGradientPen(const Drawing::Point& offset);

    virtual std::shared_ptr<Drawing::ShaderEffect> CreateGradientShader(const Drawing::Point& offset);

    bool IsNearlyEqual(const std::shared_ptr<SymbolGradient>& gradient);

protected:
    GradientType gradientType_ = GradientType::NONE_GRADIENT;
    Drawing::TileMode tileMode_ = Drawing::TileMode::CLAMP;
    std::vector<Drawing::ColorQuad> colors_;
    std::vector<float> positions_;
};

class SymbolLineGradient : public SymbolGradient {
public:
    SymbolLineGradient(float rangle);
    ~SymbolLineGradient() override = default;

    void SetAngle(float rangle);

    float GetAngle();

    void Make(const Drawing::Rect& bounds) override;
    Drawing::Brush CreateGradientBrush(const Drawing::Point& offset) override;
    Drawing::Pen CreateGradientPen(const Drawing::Point& offset) override;
    std::shared_ptr<Drawing::ShaderEffect> CreateGradientShader(const Drawing::Point& offset) override;

private:
    float rangle_ = 0.0f;
    Drawing::Point startPoint_;
    Drawing::Point endPoint_;

    static void PointsFromAngle(float angle, const Drawing::Rect& bounds, Drawing::Point& firstPoint,
        Drawing::Point& secondPoint);
};

class SymbolRadialGradient : public SymbolGradient {
public:
    SymbolRadialGradient(const Drawing::Point& centerPtRatio, float radiusRatio);
    ~SymbolRadialGradient() override = default;

    void SetCenterPoint(const Drawing::Point& centerPtRatio);

    void SetRadiusRatio(float radiusRatio);

    void SetRadius(float radius);

    Drawing::Point GetCenterPoint();

    float GetRadius();

    float GetRadiusRatio();

    void Make(const Drawing::Rect& bounds) override;
    Drawing::Brush CreateGradientBrush(const Drawing::Point& offset) override;
    Drawing::Pen CreateGradientPen(const Drawing::Point& offset) override;
    std::shared_ptr<Drawing::ShaderEffect> CreateGradientShader(const Drawing::Point& offset) override;

private:
    Drawing::Point centerPtRatio_;
    Drawing::Point centerPt_;
    float radiusRatio_ = 0.0f;
    float radius_ = 0.0f;
    bool isRadiusRatio_ = true;
};
}
#endif