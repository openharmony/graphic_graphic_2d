/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_LINE_H
#define C_INCLUDE_DRAWING_TEXT_LINE_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides the 2D drawing capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 14
 * @version 1.0
 */

/**
 * @file drawing_text_line.h
 *
 * @brief Declares functions related to <b>textLine</b> in the drawing module.
 *
 * @since 14
 * @version 1.0
 */

#include "drawing_text_declaration.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get text line information.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param typography Indicates the pointer to a typography object <b>OH_Drawing_Typography</b>.
 * @return Indicates the pointer to a text line array object <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_Array* OH_Drawing_TypographyGetTextLines(OH_Drawing_Typography* typography);

/**
 * @brief Releases the memory occupied by the text line array object <b>OH_Drawing_Array</b>.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param lines Indicates the pointer to the text line array object <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroyTextLines(OH_Drawing_Array* lines);

/**
 * @brief Releases the memory occupied by the text line object <b>OH_Drawing_TextLine</b>.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to the text line object <b>OH_Drawing_TextLine</b>.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroyTextLine(OH_Drawing_TextLine* line);

/**
 * @brief Get the text line object by index.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param lines Indicates the pointer to the text line array object <b>OH_Drawing_Array</b>.
 * @param index The text line object index.
 * @return Indicates the pointer to a text line object <b>OH_Drawing_TextLine</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_TextLine* OH_Drawing_GetTextLineByIndex(OH_Drawing_Array* lines, size_t index);

/**
 * @brief Get the count of glyphs.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @return Returns the count of glyphs.
 * @since 14
 * @version 1.0
 */
double OH_Drawing_TextLineGetGlyphCount(OH_Drawing_TextLine* line);

/**
 * @brief Get the range of text line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param start Indicates the pointer to text line start position.
 * @param end Indicates the pointer to text line end position.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_TextLineGetTextRange(OH_Drawing_TextLine* line, size_t* start, size_t* end);

/**
 * @brief Get the glyph runs array of text line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @return Indicates the pointer to a glyph runs array object of text line <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_Array* OH_Drawing_TextLineGetGlyphRuns(OH_Drawing_TextLine* line);

/**
 * @brief Releases the memory occupied by the run array object <b>OH_Drawing_Array</b>.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param runs Indicates the pointer to the run array object <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroyRuns(OH_Drawing_Array* runs);

/**
 * @brief Get the run object by index.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param runs Indicates the pointer to the run array object <b>OH_Drawing_Array</b>.
 * @param index The run object index.
 * @return Indicates the pointer to a run object <b>OH_Drawing_Run</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_Run* OH_Drawing_GetRunByIndex(OH_Drawing_Array* runs, size_t index);

/**
 * @brief Paint the range of text line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param x Represents the X-axis position on the canvas.
 * @param y Represents the Y-axis position on the canvas.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_TextLinePaint(OH_Drawing_TextLine* line, OH_Drawing_Canvas* canvas, double x, double y);

/**
 * @brief Creates a truncated text line object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param width Indicates the text line width to set.
 * @param mode Indicates the ellipsis model to set, EllipsisMode:MIDDLE is not supported.
 * For details, see the enum <b>OH_Drawing_EllipsisModal</b>.
 * @param ellipsis Indicates the ellipsis string to set.
 * @return Returns the pointer to the <b>OH_Drawing_TextLine</b> object created.
 * @since 14
 * @version 1.0
 */
OH_Drawing_TextLine* OH_Drawing_TextLineCreateTruncatedLine(OH_Drawing_TextLine* line, double width, int mode,
    const char* ellipsis);

/**
 * @brief Gets the text line typographic bounds.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param ascent Indicates the distance that the pointer points to remain above the baseline.
 * @param descent Indicates the pointer to the distance that remains below the baseline.
 * @param leading Indicates the pointer to the line Spacing.
 * @return Returns The total width of the typesetting border.
 * @since 14
 * @version 1.0
 */
double OH_Drawing_TextLineGetTypographicBounds(OH_Drawing_TextLine* line, double* ascent, double* descent,
    double* leading);

/**
 * @brief Gets the text line image bounds.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Rect</b> struct created.
 * @since 14
 * @version 1.0
 */
OH_Drawing_Rect* OH_Drawing_TextLineGetImageBounds(OH_Drawing_TextLine* line);

/**
 * @brief Gets the tail space width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @return Returns the tail space width.
 * @since 14
 * @version 1.0
 */
double OH_Drawing_TextLineGetTrailingSpaceWidth(OH_Drawing_TextLine* line);

/**
 * @brief Gets the string index of the given position.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param point Indicates the pointer to an <b>OH_Drawing_Point</b> object.
 * @return Returns the string index for a given position.
 * @since 14
 * @version 1.0
 */
int32_t OH_Drawing_TextLineGetStringIndexForPosition(OH_Drawing_TextLine* line, OH_Drawing_Point* point);

/**
 * @brief Gets the offset of the given string index.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param index The given string index.
 * @return Returns the offset for a given string index.
 * @since 14
 * @version 1.0
 */
double OH_Drawing_TextLineGetOffsetForStringIndex(OH_Drawing_TextLine* line, int32_t index);

/**
 * @brief User-defined callback functions for using offsets and indexes.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param offset Character offset is traversed as an argument to the callback function.
 * @param index Character index is traversed as an argument to the callback function.
 * @param leadingEdge Whether the current offset is at the character front, as an argument to the callback function.
 * @return The return value of the user-defined callback function.
 *         If false is returned, the traversal continues.
 *         If true is returned, the traversal stops.
 * @since 14
 * @version 1.0
 */
typedef bool (*Drawing_CaretOffsetsCallback)(double offset, int32_t index, bool leadingEdge);

/**
 * @brief Enumerate caret offset and index in text lines.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param callback User-defined callback functions, see <b>Drawing_CaretOffsetsCallback</b>.
 * @since 14
 */
void OH_Drawing_TextLineEnumerateCaretOffsets(OH_Drawing_TextLine* line, Drawing_CaretOffsetsCallback callback);

/**
 * @brief Gets the text offset based on the given alignment factor and alignment width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param line Indicates the pointer to an <b>OH_Drawing_TextLine</b> object.
 * @param alignmentFactor The coefficients that text needs to be aligned.
 *                        Less than or equal to 0 is left justified, 0.5 is center justified,
 *                        and greater than or equal to 1 is right justified.
 * @param alignmentWidth The width of the text to be aligned.
 *                       Returns 0 if it is less than the actual width of the text.
 * @return Returns the offset of the aligned text.
 * @since 14
 * @version 1.0
 */
double OH_Drawing_TextLineGetAlignmentOffset(OH_Drawing_TextLine* line, double alignmentFactor, double alignmentWidth);

#ifdef __cplusplus
}
#endif
/** @} */
#endif // C_INCLUDE_DRAWING_TEXT_LINE_H