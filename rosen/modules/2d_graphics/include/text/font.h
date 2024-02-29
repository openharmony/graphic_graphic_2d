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

#ifndef FONT_H
#define FONT_H

#include <memory>
#include <cstdint>

#include "impl_interface/font_impl.h"
#include "text/font_metrics.h"
#include "text/font_types.h"
#include "text/typeface.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class FontEdging {
    ALIAS,
    ANTI_ALIAS,
    SUBPIXEL_ANTI_ALIAS,
};

class DRAWING_API Font {
public:
    Font();
    Font(std::shared_ptr<Typeface> typeface, scalar size, scalar scaleX, scalar skewX);
    virtual ~Font() = default;

    /**
     * @brief         Set font edge pixels pattern.
     * @param edging  Edge pixels pattern.
     */
    void SetEdging(FontEdging edging);

    /**
     * @brief             Set glyphs are drawn at sub-pixel offsets.
     * @param isSubpixel  Glyphs should be drawn at sub-pixel.
     */
    void SetSubpixel(bool isSubpixel);

    /**
     * @brief               Set font hinting pattern.
     * @param hintingLevel  Font hinting level.
     */
    void SetHinting(FontHinting hintingLevel);

    /**
     * @brief           Set Typeface to font.
     * @param typeface  A shared point to typeface.
     */
    void SetTypeface(std::shared_ptr<Typeface> typeface);

    /**
     * @brief           Set text size.
     * @param textSize  Text size.
     */
    void SetSize(scalar textSize);

    /**
     * @brief             Set to increase stroke width when creating glyph bitmaps to approximate a bold typeface.
     * @param isEmbolden  Should increase stroke width.
     */
    void SetEmbolden(bool isEmbolden);

    /**
     * @brief         Set text scale on x-axis.
     * @param scaleX  Text horizontal scale.
     */
    void SetScaleX(scalar scaleX);

    /**
     * @brief        Set text skew on x-axis.
     * @param skewX  Additional shear on x-axis relative to y-axis.
     */
    void SetSkewX(scalar skewX);

    /**
     * @brief                  Set Font and glyph metrics should ignore hinting and rounding.
     * @param isLinearMetrics  Should ignore hinting and rounding.
     */
    void SetLinearMetrics(bool isLinearMetrics);

    /**
     * @brief          Get fontMetrics associated with typeface.
     * @param metrics  The fontMetrics value returned to the caller.
     * @return         Recommended spacing between lines.
     */
    scalar GetMetrics(FontMetrics* metrics) const;

    /**
     * @brief         Retrieves the advance and bounds for each glyph in glyphs.
     * @param glyphs  Array of glyph indices to be measured
     * @param count   Number of glyphs
     * @param widths  Text advances for each glyph returned to the caller.
     */
    void GetWidths(const uint16_t glyphs[], int count, scalar widths[]) const;

    /**
     * @brief         Retrieves the advance and bounds for each glyph in glyphs.
     * @param glyphs  Array of glyph indices to be measured
     * @param count   Number of glyphs
     * @param widths  Text advances for each glyph returned to the caller.
     * @param bounds  Bounds for each glyph relative to (0, 0) returned to the caller.
     */
    void GetWidths(const uint16_t glyphs[], int count, scalar widths[], Rect bounds[]) const;

    /**
     * @brief         Returns text size in points.
     * @return        The size of text.
     */
    scalar GetSize() const;

    /**
     * @brief         Returns Typeface if set, or nullptr.
     * @return        Typeface if previously set, nullptr otherwise.
     */
    std::shared_ptr<Typeface> GetTypeface();

    /**
     * @brief         Get font edge pixels pattern.
     * @return        Edge pixels pattern.
     */
    FontEdging GetEdging() const;

    /**
     * @brief               Get font hinting pattern.
     * @return              Font hinting level.
     */
    FontHinting GetHinting() const;

    /**
     * @brief         Returns text scale on x-axis.
     * @return        Text horizontal scale.
     */
    scalar GetScaleX() const;

    /**
     * @brief         Returns text skew on x-axis.
     * @return        Additional shear on x-axis relative to y-axis.
     */
    scalar GetSkewX() const;

    /**
     * @brief         Returns true if glyphs may be drawn at sub-pixel offsets.
     * @return        True if glyphs may be drawn at sub-pixel offsets.
     */
    bool IsSubpixel() const;

    /**
     * @brief         Returns glyph index for Unicode character.
     * @param uni     Unicode character.
     * @return        Glyph index.
     */
    uint16_t UnicharToGlyph(int32_t uni) const;

    /**
     * @brief               Converts text into glyph indices.
     * @param text          Character storage encoded with TextEncoding.
     * @param byteLength    Length of character storage in bytes.
     * @param glyphs        Storage for glyph indices; may be nullptr.
     * @param maxGlyphCount Storage capacity.
     * @return              Number of glyphs represented by text of length byteLength.
     */
    int TextToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
        uint16_t glyphs[], int maxGlyphCount) const;

    /**
     * @brief             Measure the width of text.
     * @param text        Character storage encoded with TextEncoding
     * @param byteLength  Length of character storage in bytes
     * @param encoding    Text encoding.
     * @param bounds      Bounding box relative to (0, 0)
     * @return            The width of text.
     */
    scalar MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds = nullptr);

    int CountText(const void* text, size_t byteLength, TextEncoding encoding) const;

    template<typename T>
    T* GetImpl() const
    {
        return fontImpl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<FontImpl> fontImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif