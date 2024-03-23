/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_H

#include "include/core/SkRect.h"
#include "modules/skparagraph/include/Metrics.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "paragraph_style.h"
#include "line_metrics.h"

#include "rosen_text/symbol_animation_config.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Canvas;
struct FontMetrics;
}
} // namespace Rosen
} // namespace OHOS

namespace OHOS {
namespace Rosen {
namespace SPText {
enum class RectWidthStyle {
    TIGHT,
    MAX
};

enum class RectHeightStyle {
    TIGHT,
    MAX,
    INCLUDE_LINESPACING_MIDDLE,
    INCLUDE_LINESPACING_TOP,
    INCLUDE_LINESPACING_BOTTOM,
    STRUT
};

enum class Affinity {
    UPSTREAM,
    DOWNSTREAM,
};

struct PositionWithAffinity {
    PositionWithAffinity(size_t p, Affinity a) : position(p), affinity(a) {}

    const size_t position;
    const Affinity affinity;
};

struct TextBox {
    TextBox(SkRect r, TextDirection d) : rect(r), direction(d) {}

    SkRect rect;
    TextDirection direction;
};

template<typename T>
struct Range {
    Range() : start(), end() {}
    Range(T s, T e) : start(s), end(e) {}

    bool operator==(const Range<T>& other) const
    {
        return start == other.start && end == other.end;
    }

    T start;
    T end;
};

// Paragraph can be laid out and then drawn on the canvas.
// Relevant information can be obtained from Paragraph.
class Paragraph {
public:
    virtual ~Paragraph() = default;

    // Returns the width limit provided in Layout() method.
    // This is the maximum width limit for multi-line text.
    virtual double GetMaxWidth() = 0;

    // Returns the height of the laid out Paragraph.
    virtual double GetHeight() = 0;

    // Returns the width of the longest line as found in Layout().
    virtual double GetLongestLine() = 0;

    // Returns the actual max width of the longest line after Layout().
    virtual double GetMinIntrinsicWidth() = 0;

    // Returns the total width covered by the paragraph without linebreaking.
    virtual double GetMaxIntrinsicWidth() = 0;

    // Returns the distance from top of Paragraph to
    // the alphabetic baseline of the first line.
    // Used for alphabetic fonts (A-Z, a-z, greek, etc.)
    virtual double GetAlphabeticBaseline() = 0;

    // Returns the distance from top of Paragraph to the ideographic baseline
    // of the first line.Used for ideographic fonts (Chinese, Japanese, Korean, etc.)
    virtual double GetIdeographicBaseline() = 0;

    // Returns the distance from the horizontal line indicated by the baseline
    // attribute to the top of the bounding rectangle of the given text.
    virtual double GetGlyphsBoundsTop() = 0;

    // Returns the distance from the horizontal line indicated by the baseline
    // attribute to the bottom of the bounding rectangle of the given text.
    virtual double GetGlyphsBoundsBottom() = 0;

    // Returns the distance parallel to the baseline from the alignment point given by the
    // textAlign attribute to the left side of the bounding rectangle of the given text.
    virtual double GetGlyphsBoundsLeft() = 0;

    // Returns the distance parallel to the baseline from the alignment point given by the
    // textAlign attribute to the right side of the bounding rectangle of the given text.
    virtual double GetGlyphsBoundsRight() = 0;

    // Returns true if Paragraph exceeds max lines, it also means that
    // some content was replaced by an ellipsis.
    virtual bool DidExceedMaxLines() = 0;

    // Returns the total number of visible lines in the paragraph.
    virtual size_t GetLineCount() const = 0;

    // Set the text indent.
    // indents  The indents for multi-line text.
    virtual void SetIndents(const std::vector<float>& indents) = 0;

    // Get the text indent in index.
    // index The index of element in indents vector.
    virtual float DetectIndents(size_t index) = 0;

    // Mark the Typography as dirty, and initially state the Typography.
    virtual void MarkDirty() = 0;

    // Get the unresolved Glyphs count of lines in a text.
    virtual int32_t GetUnresolvedGlyphsCount() = 0;

    // Update the font size of lines in a text.
    virtual void UpdateFontSize(size_t from, size_t to, float fontSize) = 0;

    // Layout calculates the positioning of all the glyphs.
    // This method must be called before other methods are called.
    virtual void Layout(double width) = 0;

    // Paints the laid out text onto the supplied canvas at (x, y).
    virtual void Paint(SkCanvas* canvas, double x, double y) = 0;

    // Paints the laid out text onto the supplied canvas at (x, y).
    virtual void Paint(Drawing::Canvas* canvas, double x, double y) = 0;

    // Returns a vector of bounding boxes that enclose all text
    // between start and end glyph indexes. The bounding boxes
    // can be used to display selections.
    virtual std::vector<TextBox> GetRectsForRange(size_t start, size_t end,
        RectHeightStyle rectHeightStyle, RectWidthStyle rectWidthStyle) = 0;

    // Return a vector of bounding boxes that bound all placeholders in Paragraph.
    // The bounds are tight and each box include only one placeholder.
    virtual std::vector<TextBox> GetRectsForPlaceholders() = 0;

    // Returns the index of the glyph corresponding to the provided coordinates.
    // The upper left corner is the origin, and the +y direction is downward.
    virtual PositionWithAffinity GetGlyphPositionAtCoordinate(double dx, double dy) = 0;

    // Returns the word range of a given glyph in a paragraph.
    virtual Range<size_t> GetWordBoundary(size_t offset) = 0;

    virtual Range<size_t> GetActualTextRange(int lineNumber, bool includeSpaces) = 0;

    virtual std::vector<LineMetrics>& GetLineMetrics() = 0;

    virtual bool GetLineMetricsAt(int lineNumber, skia::textlayout::LineMetrics* lineMetrics) const = 0;

    virtual void SetAnimation(
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc) = 0;

    virtual void SetParagraghId(uint32_t id) = 0;

    virtual OHOS::Rosen::Drawing::FontMetrics MeasureText() = 0;
    virtual OHOS::Rosen::Drawing::FontMetrics GetFontMetricsResult(const OHOS::Rosen::SPText::TextStyle& textStyle) = 0;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_H
