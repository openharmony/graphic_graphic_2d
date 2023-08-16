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

#ifndef USE_ROSEN_DRAWING
class SkCanvas;
class SkPaint;
class SkRRect;
#else
#include "draw/canvas.h"
#include "draw/brush.h"
#include "draw/pen.h"
#include "utils/rect.h"
#include "utils/round_rect.h"
#endif

namespace OHOS {
namespace Rosen {
enum class BorderStyle : uint32_t {
    SOLID = 0,
    DASHED,
    DOTTED,
    NONE
};

class RSBorder final {
public:
    RSBorder() = default;
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

    void SetColorFour(Vector4<Color> color);
    void SetWidthFour(Vector4f width);
    void SetStyleFour(Vector4<uint32_t> style);
    Vector4<Color> GetColorFour() const;
    Vector4f GetWidthFour() const;
    Vector4<uint32_t> GetStyleFour() const;

    bool HasBorder() const;

    std::string ToString() const;

protected:
#ifndef USE_ROSEN_DRAWING
    bool ApplyFillStyle(SkPaint& paint) const;
    bool ApplyPathStyle(SkPaint& paint) const;
    bool ApplyFourLine(SkPaint& paint) const;
    bool ApplyLineStyle(SkPaint& paint, int borderIdx, float length) const;
    void PaintFourLine(SkCanvas& canvas, SkPaint& paint, RectF rect) const;
    void PaintTopPath(SkCanvas& canvas, SkPaint& paint, SkRRect& rrect) const;
    void PaintRightPath(SkCanvas& canvas, SkPaint& paint, SkRRect& rrect) const;
    void PaintBottomPath(SkCanvas& canvas, SkPaint& paint, SkRRect& rrect) const;
    void PaintLeftPath(SkCanvas& canvas, SkPaint& paint, SkRRect& rrect) const;
#else
    bool ApplyFillStyle(Drawing::Brush& brush) const;
    bool ApplyPathStyle(Drawing::Pen& pen) const;
    bool ApplyFourLine(Drawing::Pen& pen) const;
    bool ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const;

    void PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const;
    void PaintTopPath(Drawing::Canvas& canvas, Drawing::Pen& pen, Drawing::RoundRect& rrect) const;
    void PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, Drawing::RoundRect& rrect) const;
    void PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, Drawing::RoundRect& rrect) const;
    void PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, Drawing::RoundRect& rrect) const;
#endif

private:
    // Vectors containing uniform or four-sided border attributes.
    // If four-sided, the order of contents is left, top, right, bottom.
    std::vector<Color> colors_;
    std::vector<float> widths_;
    std::vector<BorderStyle> styles_;

    friend class RSPropertiesPainter;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H
