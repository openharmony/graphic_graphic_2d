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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_BORDER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_BORDER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cinttypes>

#include "common/rs_color_palette.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"

#include "draw/canvas.h"
#include "draw/brush.h"
#include "draw/pen.h"
#include "utils/rect.h"
#include "utils/round_rect.h"

namespace OHOS {
namespace Rosen {
enum class BorderStyle : uint32_t {
    SOLID = 0,
    DASHED,
    DOTTED,
    NONE
};

#define MAX_BORDER_NUM 4

class RSBorderGeo {
public:
    RSBorderGeo() {};
    ~RSBorderGeo() {};

    Drawing::RoundRect rrect;
    Drawing::RoundRect innerRRect;
    Drawing::Point center;
    std::vector<Drawing::Path> pathVec { MAX_BORDER_NUM };
};

// also used for Outline
class RSBorder final {
public:
    RSBorder() = default;
    RSBorder(const bool& isOutline);
    ~RSBorder() {}

    enum BorderType : int {
        LEFT = 0,
        TOP,
        RIGHT,
        BOTTOM,
    };

    void SetColor(Color color);
    void SetWidth(float width);
    void SetStyle(BorderStyle style);
    void SetDashWidth(float dashWidth);
    void SetDashGap(float dashGap);
    Color GetColor(int idx = RSBorder::LEFT) const;
    float GetWidth(int idx = RSBorder::LEFT) const;
    BorderStyle GetStyle(int idx = RSBorder::LEFT) const;
    float GetDashWidth(int idx = RSBorder::LEFT) const;
    float GetDashGap(int idx = RSBorder::LEFT) const;

    void SetColorFour(const Vector4<Color>& color);
    void SetWidthFour(const Vector4f& width);
    void SetStyleFour(const Vector4<uint32_t>& style);
    void SetRadiusFour(const Vector4f& radius);
    void SetDashWidthFour(const Vector4f& dashWidth);
    void SetDashGapFour(const Vector4f& dashGap);
    Vector4<Color> GetColorFour() const;
    Vector4f GetWidthFour() const;
    Vector4<uint32_t> GetStyleFour() const;
    Vector4f GetDashWidthFour() const;
    Vector4f GetDashGapFour() const;
    Vector4f GetRadiusFour() const;

    bool HasBorder() const;

    std::string ToString() const;

    void SetBorderEffect(Drawing::Pen& pen, int idx, float spaceBetweenDot, float borderLength) const;
    bool ApplyFillStyle(Drawing::Brush& brush) const;
    bool ApplyPathStyle(Drawing::Pen& pen) const;
    bool ApplyFourLine(Drawing::Pen& pen) const;
    bool ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const;
    bool ApplySimpleBorder(const RRect& rrect) const;
    void PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const;
    void DrawBorders(Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo) const;

private:
    void DrawBorderImpl(Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx) const;
    void DrawBorderImpl(Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx1, int idx2) const;
    void DrawBorderImpl(
        Drawing::Canvas& canvas, Drawing::Pen& pen, RSBorderGeo& borderGeo, int idx1, int idx2, int idx3) const;
    void DrawNestedRoundRect(Drawing::Canvas& canvas, const RSBorderGeo& borderGeo, uint32_t color) const;
    void CalcBorderPath(RSBorderGeo& borderGeo) const;
    bool CanBeCombined(int border1, int border2) const;

    Drawing::Point GetTLIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const;
    Drawing::Point GetTRIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const;
    Drawing::Point GetBLIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const;
    Drawing::Point GetBRIP(const Drawing::RoundRect& rrect, const Drawing::Point& innerRectCenter) const;

    void DrawLeftBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const;
    void DrawTopBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const;
    void DrawRightBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const;
    void DrawBottomBorder(Drawing::Canvas& canvas, Drawing::Pen& pen, const RSBorderGeo& borderGeo) const;

    // Vectors containing uniform or four-sided border attributes.
    // If four-sided, the order of contents is left, top, right, bottom.
    std::vector<Color> colors_;
    std::vector<float> widths_;
    std::vector<BorderStyle> styles_;
    // Dash params dashWidth and dashGap
    std::vector<float> dashWidth_;
    std::vector<float> dashGap_;

    // only be used by outline, innerBorder(border_) uses corner radius.
    Vector4f radius_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_BORDER_H
