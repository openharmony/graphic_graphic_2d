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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H

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
    Color GetColor(int idx = RSBorder::LEFT) const;
    float GetWidth(int idx = RSBorder::LEFT) const;
    BorderStyle GetStyle(int idx = RSBorder::LEFT) const;

    void SetColorFour(const Vector4<Color>& color);
    void SetWidthFour(const Vector4f& width);
    void SetStyleFour(const Vector4<uint32_t>& style);
    void SetRadiusFour(const Vector4f& radius);
    Vector4<Color> GetColorFour() const;
    Vector4f GetWidthFour() const;
    Vector4<uint32_t> GetStyleFour() const;
    Vector4f GetRadiusFour() const;

    bool HasBorder() const;

    std::string ToString() const;

    bool ApplyFillStyle(Drawing::Brush& brush) const;
    bool ApplyPathStyle(Drawing::Pen& pen) const;
    bool ApplyFourLine(Drawing::Pen& pen) const;
    bool ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const;

    void PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const;
    void PaintTopPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
        const Drawing::Point& innerRectCenter) const;
    void PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
        const Drawing::Point& innerRectCenter) const;
    void PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
        const Drawing::Point& innerRectCenter) const;
    void PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
        const Drawing::Point& innerRectCenter) const;

private:
    // Vectors containing uniform or four-sided border attributes.
    // If four-sided, the order of contents is left, top, right, bottom.
    std::vector<Color> colors_;
    std::vector<float> widths_;
    std::vector<BorderStyle> styles_;

    // only be used by outline, innerBorder(border_) uses corner radius.
    Vector4f radius_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H
