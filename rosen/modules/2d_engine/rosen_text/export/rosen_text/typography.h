/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H

#include <cstddef>
#include <vector>

#include "common/rs_macros.h"
#include "draw/canvas.h"
#include "include/core/SkCanvas.h" // SKIA
#include "text/font_metrics.h"
#include "utils/rect.h"

#include "text_style.h"
#include "text_line_base.h"
#include "typography_types.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
enum class TextRectWidthStyle {
    TIGHT,
    MAX,
};

enum class TextRectHeightStyle {
    TIGHT,
    COVER_TOP_AND_BOTTOM,
    COVER_HALF_TOP_AND_BOTTOM,
    COVER_TOP,
    COVER_BOTTOM,
    FOLLOW_BY_STRUT,
};

struct RS_EXPORT TextRect {
    Drawing::RectF rect;
    TextDirection direction;
    TextRect(Drawing::RectF rec, TextDirection dir);
};

enum class Affinity {
    PREV,
    NEXT,
};

struct IndexAndAffinity {
    size_t index;
    Affinity affinity;
    IndexAndAffinity(size_t charIndex, Affinity charAffinity);
};

class RunMetrics {
public:
    explicit RunMetrics(const TextStyle* style) : textStyle(style) {}

    RunMetrics(const TextStyle* style, const Drawing::FontMetrics& metrics)
        : textStyle(style), fontMetrics(metrics) {}

    const TextStyle* textStyle;
    Drawing::FontMetrics fontMetrics;
};

struct LineMetrics {
    /** Text ascender height */
    double ascender;
    /** Tex descender height */
    double descender;
    /** The height of a capital letter */
    double capHeight;
    /** The height of a lowercase letter */
    double xHeight;
    /** Text width */
    double width;
    /** Line height */
    double height;
    /**
     * The distance from the left end of the text to the left end of the container,
     * aligned to 0, is the width of the container minus the width of the line of text
     */
    double x;
    /**
     * The height from the top of the text to the top of the container, the first line is 0,
     *  and the second line is the height of the first line
     */
    double y;
    /** Start Index */
    size_t startIndex;
    /** End Index */
    size_t endIndex;

    Drawing::FontMetrics firstCharMetrics;
    /** The y position of the baseline for this line from the top of the paragraph */
    double baseline;
    /** Zero indexed line number */
    size_t lineNumber;

    std::map<size_t, RunMetrics> runMetrics;
};

class Typography {
public:
    virtual ~Typography() = default;

    virtual double GetMaxWidth() const = 0;
    virtual double GetHeight() const = 0;
    virtual double GetActualWidth() const = 0;
    virtual double GetMinIntrinsicWidth() = 0;
    virtual double GetMaxIntrinsicWidth() = 0;
    virtual double GetAlphabeticBaseline() = 0;
    virtual double GetIdeographicBaseline() = 0;
    virtual double GetGlyphsBoundsTop() = 0;
    virtual double GetGlyphsBoundsBottom() = 0;
    virtual double GetGlyphsBoundsLeft() = 0;
    virtual double GetGlyphsBoundsRight() = 0;
    virtual bool DidExceedMaxLines() const = 0;
    virtual int GetLineCount() const = 0;
    virtual void MarkDirty() = 0;
    virtual int32_t GetUnresolvedGlyphsCount() = 0;
    virtual void UpdateFontSize(size_t from, size_t to, float fontSize) = 0;

    virtual void SetIndents(const std::vector<float>& indents) = 0;
    virtual float DetectIndents(size_t index) = 0;
    virtual void Layout(double width) = 0;
    virtual void Paint(SkCanvas *canvas, double x, double y) = 0; // SKIA
    virtual void Paint(Drawing::Canvas *canvas, double x, double y) = 0; // DRAWING

    virtual std::vector<TextRect> GetTextRectsByBoundary(size_t left, size_t right,
        TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle) = 0;
    virtual std::vector<TextRect> GetTextRectsOfPlaceholders() = 0;
    virtual IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) = 0;
    virtual Boundary GetWordBoundaryByIndex(size_t index) = 0;
    virtual Boundary GetActualTextRange(int lineNumber, bool includeSpaces) = 0;
    virtual double GetLineHeight(int lineNumber) = 0;
    virtual double GetLineWidth(int lineNumber) = 0;
    virtual void SetAnimation(
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc)= 0;
    virtual void SetParagraghId(uint32_t id) = 0;
    virtual Drawing::FontMetrics MeasureText() = 0;
    virtual bool GetLineInfo(int lineNumber, bool oneLine, bool includeWhitespace, LineMetrics* lineMetrics) = 0;
    virtual std::vector<LineMetrics> GetLineMetrics() = 0;
    virtual bool GetLineMetricsAt(int lineNumber, LineMetrics* lineMetrics) = 0;
    virtual Drawing::FontMetrics GetFontMetrics(const OHOS::Rosen::TextStyle& textStyle) = 0;
    virtual bool GetLineFontMetrics(const size_t lineNumber,
        size_t& charNumber, std::vector<Drawing::FontMetrics>& fontMetrics) = 0;
    virtual std::vector<std::unique_ptr<TextLineBase>> GetTextLines() const = 0;
    virtual std::unique_ptr<Typography> CloneSelf() = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H
