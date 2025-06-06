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

    void SetColors(const std::vector<Drawing::ColorQuad>& colors)
    {
        colors_ = colors;
    }

    void SetPositions(const std::vector<float>& positions)
    {
        positions_ = positions;
    }

    void SetTileMode(Drawing::TileMode tileMode)
    {
        tileMode_ = tileMode;
    }

    const std::vector<Drawing::ColorQuad>& GetColors() const
    {
        return colors_;
    }

    const std::vector<float>& GetPositions() const
    {
        return positions_;
    }

    Drawing::TileMode GetTileMode() const
    {
        return tileMode_;
    }

    GradientType GetGradientType() const
    {
        return gradientType_;
    }

    Drawing::Brush CreateGradientBrush()
    {
        Drawing::Point offset;
        return CreateGradientBrush(offset);
    }

    Drawing::Pen CreateGradientPen()
    {
        Drawing::Point offset;
        return CreateGradientPen(offset);
    }

    virtual void Make(const Drawing::Rect& bounds) {}

    virtual Drawing::Brush CreateGradientBrush(const Drawing::Point& offset)
    {
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        if (!colors_.empty()) {
            brush.SetColor(colors_[0]);
        }
        return brush;
    }

    virtual Drawing::Pen CreateGradientPen(const Drawing::Point& offset)
    {
        Drawing::Pen pen;
        pen.SetAntiAlias(true);
        if (!colors_.empty()) {
            pen.SetColor(colors_[0]);
        }
        return pen;
    }

    virtual std::shared_ptr<Drawing::ShaderEffect> CreateGradientShader(const Drawing::Point& offset)
    {
        return nullptr;
    }

    bool IsNearlyEqual(const std::shared_ptr<SymbolGradient>& gradient)
    {
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
protected:
    GradientType gradientType_ = GradientType::NONE_GRADIENT;
    Drawing::TileMode tileMode_ = Drawing::TileMode::CLAMP;
    std::vector<Drawing::ColorQuad> colors_;
    std::vector<float> positions_;
};

class SymbolLineGradient : public SymbolGradient {
public:
    SymbolLineGradient(float rangle) : rangle_(rangle)
    {
        gradientType_ = GradientType::LINE_GRADIENT;
    }
    ~SymbolLineGradient() override = default;

    void SetAngle(float rangle)
    {
        rangle_ = rangle;
    }

    float GetAngle()
    {
        return rangle_;
    }

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
    SymbolRadialGradient(const Drawing::Point& centerPtRatio, float radiusRatio)
        : centerPtRatio_(centerPtRatio), radiusRatio_(radiusRatio)
    {
        gradientType_ = GradientType::RADIAL_GRADIENT;
    }
    ~SymbolRadialGradient() override = default;

    void SetCenterPoint(const Drawing::Point& centerPtRatio)
    {
        centerPtRatio_ = centerPtRatio;
    }

    void SetRadiusRatio(float radiusRatio)
    {
        radiusRatio_ = radiusRatio;
        isRadiusRatio_ = true;
    }

    void SetRadius(float radius)
    {
        radius_ = radius;
        isRadiusRatio_ = false;
    }

    Drawing::Point GetCenterPoint()
    {
        return centerPtRatio_;
    }

    float GetRadius()
    {
        return radius_;
    }

    float GetRadiusRatio()
    {
        return radiusRatio_;
    }

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