/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_H

#include <string>
#include <vector>

#include "texgine_canvas.h"
#include "texgine_font.h"
#include "texgine_rect.h"
#include "texgine/typography_types.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief TextRectWidthStyle is the width option for various types
 *        of text rectangles provided by Typography::GetTextRectsByBoundary.
 */
enum class TextRectWidthStyle {
    TIGHT,     // every rectangle width bounds shrink to glyph bounds.
    MAX_WIDTH, // if the rectangle contains the last glyph of a line,
               // it will expand to the maximum width of the line.
};

/*
 * @brief TextRectWidthStyle is the width option for various types
 *        of text rectangles provided by Typography::GetTextRectsByBoundary.
 */
enum class TextRectHeightStyle {
    TIGHT,                     // every rectangle height bounds shrink to glyph bounds.
    COVER_TOP_AND_BOTTOM,      // every rectangle height bounds covers
                               // top line spacing and bottom line spacing.
    COVER_HALF_TOP_AND_BOTTOM, // every rectangle height bounds covers
                               // half top line spacing and half bottom line spacing.
    COVER_TOP,                 // every rectangle height bounds covers top line spacing.
    COVER_BOTTOM,              // every rectangle height bounds covers bottom line spacing.
    FOLLOW_BY_LINE_STYLE,      // every rectangle height bounds is determined by the line style.
};

/*
 * @brief Affinity is the affinity for the previous and next glyphs.
 */
enum class Affinity {
    PREV, // prefer the previous glyph more
    NEXT, // prefer the next glyph more
};

/*
 * @brief IndexAndAffinity contains the index of a glyph in the Typography
 *        and the affinity for the previous and next glyph.
 */
struct IndexAndAffinity {
    size_t index;
    Affinity affinity;
};

/*
 * @brief TextRect contains a TexgineRect and a text direction.
 */
struct TextRect {
    TexgineRect rect;
    TextDirection direction;
};

/*
 * @brief Boundary is the range of index.
 */
struct Boundary {
    size_t leftIndex = 0;
    size_t rightIndex = 0;

    Boundary(size_t left, size_t right);
};

/*
 * @brief Typography can be laid out and then drawn on the canvas.
 *        Relevant information can be obtained from Typography.
 */
class Typography : public MemoryObject {
public:
    virtual ~Typography() = default;

    /*
     * @brief Returns the width limit provided in Layout() method.
     *        This is the maximum width limit for multi-line text.
     */
    virtual double GetMaxWidth() const = 0;

    /*
     * @brief Returns the height of the laid out Typography.
     */
    virtual double GetHeight() const = 0;

    /*
     * @brief Returns the actual width of the longest line as found in Layout().
     */
    virtual double GetActualWidth() const = 0;

    /*
     * @brief Returns the maximum width of each word in the typography.
     *        If the word segmentation mode is BreakAll, the content in front of the space
     *        in each line is counted as a word.
     *        If there is no space in a line, the content in this line is counted as a word to
     *        calculate the width.
     */
    virtual double GetMinIntrinsicWidth() const = 0;

    /*
     * @brief Returns the maxmium width of typography without line breaks.
     */
    virtual double GetMaxIntrinsicWidth() const = 0;

    /*
     * @brief Returns the distance from top of Typography to
     *        the alphabetic baseline of the first line.
     *        Used for alphabetic fonts (A-Z, a-z, greek, etc.)
     */
    virtual double GetAlphabeticBaseline() const = 0;

    /*
     * @brief Returns the distance from top of Typography to the ideographic baseline
     *        of the first line.Used for ideographic fonts (Chinese, Japanese, Korean, etc.)
     */
    virtual double GetIdeographicBaseline() const = 0;

    /*
     * @brief Returns true if Typography exceeds max lines, it also means that
     *        some content was replaced by an ellipsis.
     */
    virtual bool DidExceedMaxLines() const = 0;

    /*
     * @brief Returns the actual line number.
     */
    virtual int GetLineCount() const = 0;

    /*
     * @brief Set the text indent.
     * @param indents  The indents for multi-line text.
     */
    virtual void SetIndents(const std::vector<float>& indents) = 0;

    /*
     * @brief Layout calculates the positioning of all the glyphs.
     *        This method must be called before other methods are called.
     * @param widthLimit The limit of the multi-line text width.
     */
    virtual void Layout(double widthLimit) = 0;

    /*
     * @brief Paints the laid out text onto the supplied canvas at (x, y).
     * @param canvas  Canvas to be drawn.
     * @param offsetx The Offset in x-asix of the starting point for drawing the Typography
     * @param offsety The Offset in y-asix of the starting point for drawing the Typography
     */
    virtual void Paint(TexgineCanvas& canvas, double offsetx, double offsety) = 0;

    /*
     * @brief Returns a vector of bounding boxes that enclose all text
     *        between start and end glyph indexes. The bounding boxes
     *        can be used to display selections.
     * @param start       The index of the starting glyph, including start.
     * @param end         The index of the ending glyph, excluding end.
     * @param heightStyle The style of the selections height.
     * @param widthStyle  The style of the selections width.
     * @return            The vector of bounding boxes.
     */
    virtual std::vector<TextRect> GetTextRectsByBoundary(Boundary boundary,
        TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle) const = 0;

    /*
     * @brief Return a vector of bounding boxes that bound all AnySpan in typography.
     *        The bounds are tight and each box include only one AnySpan.
     */
    virtual std::vector<TextRect> GetTextRectsOfPlaceholders() const = 0;

    /*
     * @brief Returns the index of the glyph corresponding to the provided coordinates.
     *        The upper left corner is the origin, and the +y direction is downward.
     * @param x The position on x-asix.
     * @param y The position on y-asix.
     */
    virtual IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) const = 0;

    /*
     * @brief Returns the word range of a given glyph in a paragraph.
     * @param position The position of glyph
     * @return The struct of word boundary
     */
    virtual Boundary GetWordBoundaryByIndex(size_t index) const = 0;
    virtual Boundary GetActualTextRange(int lineNumber, bool includeSpaces) const = 0;
    virtual void SetAnimation(
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc)= 0;

private:
    friend void ReportMemoryUsage(const std::string& member, const Typography& that, const bool needThis);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_H
