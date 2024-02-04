/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_TYPOGRAPHY_H
#define C_INCLUDE_DRAWING_TEXT_TYPOGRAPHY_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides the 2D drawing capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_text_typography.h
 *
 * @brief Declares functions related to <b>typography</b> in the drawing module.
 *
 * @since 8
 * @version 1.0
 */

#include "cstddef"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_text_declaration.h"
#include "drawing_types.h"

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates text directions.
 */
enum OH_Drawing_TextDirection {
    /** Right to left (RTL) */
    TEXT_DIRECTION_RTL,
    /** Left to right (LTR) */
    TEXT_DIRECTION_LTR,
};

/**
 * @brief Enumerates text alignment modes.
 */
enum OH_Drawing_TextAlign {
    /** Left-aligned */
    TEXT_ALIGN_LEFT,
    /** Right-aligned */
    TEXT_ALIGN_RIGHT,
    /** Center-aligned */
    TEXT_ALIGN_CENTER,
    /**
     * Justified, which means that each line (except the last line) is stretched so that every line has equal width,
     * and the left and right margins are straight.
     */
    TEXT_ALIGN_JUSTIFY,
    /**
     * <b>TEXT_ALIGN_START</b> achieves the same effect as <b>TEXT_ALIGN_LEFT</b>
     * when <b>OH_Drawing_TextDirection</b> is <b>TEXT_DIRECTION_LTR</b>;
     * it achieves the same effect as <b>TEXT_ALIGN_RIGHT</b>
     * when <b>OH_Drawing_TextDirection</b> is <b>TEXT_DIRECTION_RTL</b>.
     */
    TEXT_ALIGN_START,
    /**
     * <b>TEXT_ALIGN_END</b> achieves the same effect as <b>TEXT_ALIGN_RIGHT</b>
     * when <b>OH_Drawing_TextDirection</b> is <b>TEXT_DIRECTION_LTR</b>;
     * it achieves the same effect as <b>TEXT_ALIGN_LEFT</b>
     * when <b>OH_Drawing_TextDirection</b> is <b>TEXT_DIRECTION_RTL</b>.
     */
    TEXT_ALIGN_END,
};

/**
 * @brief Enumerates font weights.
 */
enum OH_Drawing_FontWeight {
    /** Thin */
    FONT_WEIGHT_100,
    /** Extra-light */
    FONT_WEIGHT_200,
    /** Light */
    FONT_WEIGHT_300,
    /** Normal/Regular */
    FONT_WEIGHT_400,
    /** Medium*/
    FONT_WEIGHT_500,
    /** Semi-bold */
    FONT_WEIGHT_600,
    /** Bold */
    FONT_WEIGHT_700,
    /** Extra-bold */
    FONT_WEIGHT_800,
    /** Black */
    FONT_WEIGHT_900,
};

/**
 * @brief Enumerates text baselines.
 */
enum OH_Drawing_TextBaseline {
    /** Alphabetic, where the letters in alphabets like English sit on. */
    TEXT_BASELINE_ALPHABETIC,
    /** Ideographic. The baseline is at the bottom of the text area. */
    TEXT_BASELINE_IDEOGRAPHIC,
};

/**
 * @brief Enumerates text decorations.
 */
enum OH_Drawing_TextDecoration {
    /** No decoration. */
    TEXT_DECORATION_NONE = 0x0,
    /** A underline is used for decoration. */
    TEXT_DECORATION_UNDERLINE = 0x1,
    /** An overline is used for decoration. */
    TEXT_DECORATION_OVERLINE = 0x2,
    /** A strikethrough is used for decoration. */
    TEXT_DECORATION_LINE_THROUGH = 0x4,
};

/**
 * @brief Enumerates font styles.
 */
enum OH_Drawing_FontStyle {
    /** Normal style */
    FONT_STYLE_NORMAL,
    /** Italic style */
    FONT_STYLE_ITALIC,
};

/**
 * @brief Enumerates placeholder vertical alignment.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Offset At Baseline */
    ALIGNMENT_OFFSET_AT_BASELINE,
    /** Above Baseline */
    ALIGNMENT_ABOVE_BASELINE,
    /** Below Baseline */
    ALIGNMENT_BELOW_BASELINE,
    /** Top of Row Box */
    ALIGNMENT_TOP_OF_ROW_BOX,
    /** Bottom of Row Box */
    ALIGNMENT_BOTTOM_OF_ROW_BOX,
    /** Center of Row Box */
    ALIGNMENT_CENTER_OF_ROW_BOX,
} OH_Drawing_PlaceholderVerticalAlignment;

/**
 * @brief Defines the placeholder span.
 *
 * @since 11
 * @version 1.0
 */
typedef struct {
    /** width of placeholder */
    double width;
    /** height of placeholder */
    double height;
    /** alignment of placeholder */
    OH_Drawing_PlaceholderVerticalAlignment alignment;
    /** baseline of placeholder */
    OH_Drawing_TextBaseline baseline;
    /** baselineoffset of placeholder */
    double baselineOffset;
} OH_Drawing_PlaceholderSpan;

/**
 * @brief Enumerates text decoration style.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Solid style */
    TEXT_DECORATION_STYLE_SOLID,
    /** Double style */
    TEXT_DECORATION_STYLE_DOUBLE,
    /** Dotted style */
    TEXT_DECORATION_STYLE_DOTTED,
    /** Dashed style */
    TEXT_DECORATION_STYLE_DASHED,
    /** Wavy style */
    TEXT_DECORATION_STYLE_WAVY,
} OH_Drawing_TextDecorationStyle;

/**
 * @brief Enumerates ellipsis modal.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Head modal */
    ELLIPSIS_MODAL_HEAD = 0,
    /** Middle modal */
    ELLIPSIS_MODAL_MIDDLE = 1,
    /** Tail modal */
    ELLIPSIS_MODAL_TAIL = 2,
} OH_Drawing_EllipsisModal;

/**
 * @brief Enumerates break strategy.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Greedy strategy */
    BREAK_STRATEGY_GREEDY = 0,
    /** Quality strategy */
    BREAK_STRATEGY_HIGH_QUALITY = 1,
    /** Balanced strategy */
    BREAK_STRATEGY_BALANCED = 2,
} OH_Drawing_BreakStrategy;

/**
 * @brief Enumerates word break type.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Normal type */
    WORD_BREAK_TYPE_NORMAL = 0,
    /** Break All type */
    WORD_BREAK_TYPE_BREAK_ALL = 1,
    /** Break Word type */
    WORD_BREAK_TYPE_BREAK_WORD = 2,
} OH_Drawing_WordBreakType;

/**
 * @brief Enumerates rect height style.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Tight style */
    RECT_HEIGHT_STYLE_TIGHT,
    /** Max style */
    RECT_HEIGHT_STYLE_MAX,
    /** Includelinespacemiddle style */
    RECT_HEIGHT_STYLE_INCLUDELINESPACEMIDDLE,
    /** Includelinespacetop style */
    RECT_HEIGHT_STYLE_INCLUDELINESPACETOP,
    /** Includelinespacebottom style */
    RECT_HEIGHT_STYLE_INCLUDELINESPACEBOTTOM,
    /** Struct style */
    RECT_HEIGHT_STYLE_STRUCT,
} OH_Drawing_RectHeightStyle;

/**
 * @brief Enumerates rect Width style.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /** Tight style */
    RECT_WIDTH_STYLE_TIGHT,
    /** Max style */
    RECT_WIDTH_STYLE_MAX,
} OH_Drawing_RectWidthStyle;

/**
 * @brief Creates an <b>OH_Drawing_TypographyStyle</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_TypographyStyle</b> object created.
 * @since 8
 * @version 1.0
 */
OH_Drawing_TypographyStyle* OH_Drawing_CreateTypographyStyle(void);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_TypographyStyle</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypographyStyle(OH_Drawing_TypographyStyle*);

/**
 * @brief Sets the text direction.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the text direction to set. For details, see the enum <b>OH_Drawing_TextDirection</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextDirection(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextDirection */);

/**
 * @brief Sets the text alignment mode.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the text alignment mode to set. For details, see the enum <b>OH_Drawing_TextAlign</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextAlign(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextAlign */);

/**
 * @brief Sets the maximum number of lines in a text file.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the maximum number of lines to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextMaxLines(OH_Drawing_TypographyStyle*, int /* maxLines */);

/**
 * @brief Creates an <b>OH_Drawing_TextStyle</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_TextStyle</b> object created.
 * @since 8
 * @version 1.0
 */
OH_Drawing_TextStyle* OH_Drawing_CreateTextStyle(void);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_TextStyle</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTextStyle(OH_Drawing_TextStyle*);

/**
 * @brief Sets the text color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param uint32_t Indicates the color to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief Sets the font size.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param double Indicates the font size to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontSize(OH_Drawing_TextStyle*, double /* fontSize */);

/**
 * @brief Sets the font weight.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the font weight to set. For details, see the enum <b>OH_Drawing_FontWeight</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontWeight(OH_Drawing_TextStyle*, int /* OH_Drawing_FontWeight */);

/**
 * @brief Sets the text baseline.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the text baseline to set. For details, see the enum <b>OH_Drawing_TextBaseline</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleBaseLine(OH_Drawing_TextStyle*, int /* OH_Drawing_TextBaseline */);

/**
 * @brief Sets the text decoration.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the text decoration to set. For details, see the enum <b>OH_Drawing_TextDecoration</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecoration(OH_Drawing_TextStyle*, int /* OH_Drawing_TextDecoration */);

/**
 * @brief Sets the color for the text decoration.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param uint32_t Indicates the color to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecorationColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief Sets the font height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param double Indicates the font height to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontHeight(OH_Drawing_TextStyle*, double /* fontHeight */);

/**
 * @brief Sets the font families.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the number of font families to set.
 * @param char Indicates the pointer to the font families to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontFamilies(OH_Drawing_TextStyle*,
    int /* fontFamiliesNumber */, const char* fontFamilies[]);

/**
 * @brief Sets the font style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the font style to set. For details, see the enum <b>OH_Drawing_FontStyle</b>.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontStyle(OH_Drawing_TextStyle*, int /* OH_Drawing_FontStyle */);

/**
 * @brief Sets the locale.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param char Indicates the pointer to the locale to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleLocale(OH_Drawing_TextStyle*, const char*);

/**
 * @brief Creates a pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param OH_Drawing_FontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_TypographyCreate</b> object created.
 * @since 8
 * @version 1.0
 */
OH_Drawing_TypographyCreate* OH_Drawing_CreateTypographyHandler(OH_Drawing_TypographyStyle*,
    OH_Drawing_FontCollection*);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_TypographyCreate</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypographyHandler(OH_Drawing_TypographyCreate*);

/**
 * @brief Sets the text style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerPushTextStyle(OH_Drawing_TypographyCreate*, OH_Drawing_TextStyle*);

/**
 * @brief Sets the text content.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @param char Indicates the pointer to the text content to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerAddText(OH_Drawing_TypographyCreate*, const char*);

/**
 * @brief Removes the topmost style in the stack, leaving the remaining styles in effect.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerPopTextStyle(OH_Drawing_TypographyCreate*);

/**
 * @brief Creates an <b>OH_Drawing_Typography</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Typography</b> object created.
 * @since 8
 * @version 1.0
 */
OH_Drawing_Typography* OH_Drawing_CreateTypography(OH_Drawing_TypographyCreate*);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_Typography</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypography(OH_Drawing_Typography*);

/**
 * @brief Lays out the typography.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param double Indicates the maximum text width to set.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyLayout(OH_Drawing_Typography*, double /* maxWidth */);

/**
 * @brief Paints text on the canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param double Indicates the x coordinate.
 * @param double Indicates the y coordinate.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyPaint(OH_Drawing_Typography*, OH_Drawing_Canvas*,
    double /* potisionX */, double /* potisionY */);

/**
 * @brief Gets the max width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the max width.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetMaxWidth(OH_Drawing_Typography*);

/**
 * @brief Gets the height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the height.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetHeight(OH_Drawing_Typography*);

/**
 * @brief Gets the longest line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the length of the longest line.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetLongestLine(OH_Drawing_Typography*);

/**
 * @brief Gets the min intrinsic width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the min intrinsic width.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetMinIntrinsicWidth(OH_Drawing_Typography*);

/**
 * @brief Gets the max intrinsic width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the max intrinsic width.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetMaxIntrinsicWidth(OH_Drawing_Typography*);

/**
 * @brief Gets the alphabetic baseline.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the alphabetic baseline.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetAlphabeticBaseline(OH_Drawing_Typography*);

/**
 * @brief Gets the ideographic baseline.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the ideographic baseline.
 * @since 9
 * @version 1.1
 */
double OH_Drawing_TypographyGetIdeographicBaseline(OH_Drawing_Typography*);

/**
 * @brief Sets the placeholder.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @param OH_Drawing_PlaceholderSpan Indicates the pointer to an <b>OH_Drawing_PlaceholderSpan</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerAddPlaceholder(OH_Drawing_TypographyCreate*, OH_Drawing_PlaceholderSpan*);

/**
 * @brief Gets the exceed maxLines.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the exceed maxLines.
 * @since 11
 * @version 1.0
 */
bool OH_Drawing_TypographyDidExceedMaxLines(OH_Drawing_Typography*);

/**
 * @brief Gets the rects for range.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param size_t Indicates the start of range to set.
 * @param size_t Indicates the end of range to set.
 * @param OH_Drawing_RectHeightStyle Indicates the height style to set.
 * For details, see the enum <b>OH_Drawing_RectHeightStyle</b>.
 * @param OH_Drawing_RectWidthStyle Indicates the width style to set.
 * For details, see the enum <b>OH_Drawing_RectWidthStyle</b>.
 * @return Returns the rects for range.
 * @since 11
 * @version 1.0
 */
OH_Drawing_TextBox* OH_Drawing_TypographyGetRectsForRange(OH_Drawing_Typography*,
    size_t, size_t, OH_Drawing_RectHeightStyle, OH_Drawing_RectWidthStyle);

/**
 * @brief Gets the rects for placeholders.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the rects for placeholders.
 * @since 11
 * @version 1.0
 */
OH_Drawing_TextBox* OH_Drawing_TypographyGetRectsForPlaceholders(OH_Drawing_Typography*);

/**
 * @brief Gets left from textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @param int Indicates the index of textbox.
 * @return Returns left from textbox.
 * @since 11
 * @version 1.0
 */
float OH_Drawing_GetLeftFromTextBox(OH_Drawing_TextBox*, int);

/**
 * @brief Gets right from textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @param int Indicates the index of textbox.
 * @return Returns right from textbox.
 * @since 11
 * @version 1.0
 */
float OH_Drawing_GetRightFromTextBox(OH_Drawing_TextBox*, int);

/**
 * @brief Gets top from textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @param int Indicates the index of textbox.
 * @return Returns top from textbox.
 * @since 11
 * @version 1.0
 */
float OH_Drawing_GetTopFromTextBox(OH_Drawing_TextBox*, int);

/**
 * @brief Gets bottom from textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @param int Indicates the index of textbox.
 * @return Returns bottom from textbox.
 * @since 11
 * @version 1.0
 */
float OH_Drawing_GetBottomFromTextBox(OH_Drawing_TextBox*, int);

/**
 * @brief Gets direction from textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @param int Indicates the index of textbox.
 * @return Returns direction from textbox.
 * @since 11
 * @version 1.0
 */
int OH_Drawing_GetTextDirectionFromTextBox(OH_Drawing_TextBox*, int);

/**
 * @brief Gets size of textbox.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBox Indicates the pointer to an <b>OH_Drawing_TextBox</b> object.
 * @return Returns size of textbox.
 * @since 11
 * @version 1.0
 */
size_t OH_Drawing_GetSizeOfTextBox(OH_Drawing_TextBox*);

/**
 * @brief Gets the glyphposition at coordinate.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param double Indicates the positionX of typography to set.
 * @param double Indicates the positionY of typography to set.
 * @return Returns the glyphposition at coordinate.
 * @since 11
 * @version 1.0
 */
OH_Drawing_PositionAndAffinity* OH_Drawing_TypographyGetGlyphPositionAtCoordinate(OH_Drawing_Typography*,
    double, double);

/**
 * @brief Gets the glyphposition at coordinate with cluster.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param double Indicates the positionX of typography to set.
 * @param double Indicates the positionY of typography to set.
 * @return Returns the glyphposition at coordinate with cluster.
 * @since 11
 * @version 1.0
 */
OH_Drawing_PositionAndAffinity* OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(OH_Drawing_Typography*,
    double, double);

/**
 * @brief Gets position from positionandaffinity.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_PositionAndAffinity Indicates the pointer to an <b>OH_Drawing_PositionAndAffinity</b> object.
 * @return Returns position from positionandaffinity.
 * @since 11
 * @version 1.0
 */
size_t OH_Drawing_GetPositionFromPositionAndAffinity(OH_Drawing_PositionAndAffinity*);

/**
 * @brief Gets affinity from positionandaffinity.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_PositionAndAffinity Indicates the pointer to an <b>OH_Drawing_PositionAndAffinity</b> object.
 * @return Returns affinity from positionandaffinity.
 * @since 11
 * @version 1.0
 */
int OH_Drawing_GetAffinityFromPositionAndAffinity(OH_Drawing_PositionAndAffinity*);

/**
 * @brief Gets the word boundary.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param size_t Indicates the size of text to set.
 * @return Returns the word boundary.
 * @since 11
 * @version 1.0
 */
OH_Drawing_Range* OH_Drawing_TypographyGetWordBoundary(OH_Drawing_Typography*, size_t);

/**
 * @brief Gets start from range.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Range Indicates the pointer to an <b>OH_Drawing_Range</b> object.
 * @return Returns start from range.
 * @since 11
 * @version 1.0
 */
size_t OH_Drawing_GetStartFromRange(OH_Drawing_Range*);

/**
 * @brief Gets end from range.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Range Indicates the pointer to an <b>OH_Drawing_Range</b> object.
 * @return Returns end from range.
 * @since 11
 * @version 1.0
 */
size_t OH_Drawing_GetEndFromRange(OH_Drawing_Range*);

/**
 * @brief Gets the line count.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @return Returns the line count.
 * @since 11
 * @version 1.0
 */
size_t OH_Drawing_TypographyGetLineCount(OH_Drawing_Typography*);

/**
 * @brief Sets the decoration style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the text decoration style to set.
 * For details, see the enum <b>OH_Drawing_TextDecorationStyle</b>.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecorationStyle(OH_Drawing_TextStyle*, int);

/**
 * @brief Sets the decoration thickness scale.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param double Indicates the thickness scale of text decoration to set.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecorationThicknessScale(OH_Drawing_TextStyle*, double);

/**
 * @brief Sets the letter spacing.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param double Indicates the letter space to set.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleLetterSpacing(OH_Drawing_TextStyle*, double);

/**
 * @brief Sets the word spacing.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param double Indicates the word space to set.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleWordSpacing(OH_Drawing_TextStyle*, double);

/**
 * @brief Sets the half leading.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param bool Indicates the half leading to set.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleHalfLeading(OH_Drawing_TextStyle*, bool);

/**
 * @brief Sets the ellipsis.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param char* Indicates the pointer to ellipsis style.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleEllipsis(OH_Drawing_TextStyle*, const char*);

/**
 * @brief Sets the ellipsis modal.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle Indicates the pointer to an <b>OH_Drawing_TextStyle</b> object.
 * @param int Indicates the ellipsis model to set. For details, see the enum <b>OH_Drawing_EllipsisModal</b>.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTextStyleEllipsisModal(OH_Drawing_TextStyle*, int);

/**
 * @brief Sets the break strategy.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the break strategy to set. For details, see the enum <b>OH_Drawing_BreakStrategy</b>.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextBreakStrategy(OH_Drawing_TypographyStyle*, int);

/**
 * @brief Sets the word break type.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the word break type to set. For details, see the enum <b>OH_Drawing_WordBreakType</b>.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextWordBreakType(OH_Drawing_TypographyStyle*, int);

/**
 * @brief Sets the ellipsis modal.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle Indicates the pointer to an <b>OH_Drawing_TypographyStyle</b> object.
 * @param int Indicates the ellipsis modal to set. For details, see the enum <b>OH_Drawing_EllipsisModal</b>.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextEllipsisModal(OH_Drawing_TypographyStyle*, int);

/**
 * @brief get line height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param int Indicates the line number.
 * @return Returns line height.
 * @since 11
 * @version 1.0
 */
double OH_Drawing_TypographyGetLineHeight(OH_Drawing_Typography*, int);

/**
 * @brief get line width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param int Indicates the line number.
 * @return Returns line width.
 * @since 11
 * @version 1.0
 */
double OH_Drawing_TypographyGetLineWidth(OH_Drawing_Typography*, int);

/**
 * @brief get line text range.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography Indicates the pointer to an <b>OH_Drawing_Typography</b> object.
 * @param int Indicates the line number.
 * @param bool Indicates whether spaces are contained.
 * @return Returns line text range.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Range* OH_Drawing_TypographyGetLineTextRange(OH_Drawing_Typography*, int, bool);

#ifdef __cplusplus
}
#endif
/** @} */
#endif