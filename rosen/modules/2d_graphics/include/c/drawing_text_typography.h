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

#ifdef __cplusplus
}
#endif
/** @} */
#endif