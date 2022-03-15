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
 * @brief Provides 2d drawing functions.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_text_typography.h
 *
 * @brief Defines the typography functions of the Drawing module.
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
 * @brief direction of text.
 */
enum OH_Drawing_TextDirection {
    /** direction: from right to left */
    TEXT_DIRECTION_RTL,
    /** direction: from left to right */
    TEXT_DIRECTION_LTR,
};

/**
 * @brief text layout.
 */
enum OH_Drawing_TextAlign {
    /** Lines start at the left edge */
    TEXT_ALIGN_LEFT,
    /** Lines start at the right edge */
    TEXT_ALIGN_RIGHT,
    /** Lines are centered within the the line box */
    TEXT_ALIGN_CENTER,
    /**
     * Lines are justified.
     * Text is spaced to line up its left and right edges,
     * except for the last line.
     */
    TEXT_ALIGN_JUSTIFY,
    /**
     * The same as TEXT_ALIGN_LEFT if OH_Drawing_TextDirection
     * is TEXT_DIRECTION_LTR. Similarly, it is TEXT_ALIGN_RIGHT
     * if OH_Drawing_TextDirection is TEXT_DIRECTION_RTL.
     */
    TEXT_ALIGN_START,
    /**
     * The same as TEXT_ALIGN_RIGHT if OH_Drawing_TextDirection
     * is TEXT_DIRECTION_LTR. Similarly, it is TEXT_ALIGN_LEFT
     * if OH_Drawing_TextDirection is TEXT_DIRECTION_RTL.
     */
    TEXT_ALIGN_END,
};

/**
 * @brief font weight.
 */
enum OH_Drawing_FontWeight {
    /** font weight is thin */
    FONT_WEIGHT_100,
    /** font weight is extra-light */
    FONT_WEIGHT_200,
    /** font weight is light */
    FONT_WEIGHT_300,
    /** font weight is normal/regular */
    FONT_WEIGHT_400,
    /** font weight is medium */
    FONT_WEIGHT_500,
    /** font weight is semi-bold */
    FONT_WEIGHT_600,
    /** font weight is bold */
    FONT_WEIGHT_700,
    /** font weight is extra-bold */
    FONT_WEIGHT_800,
    /** font weight is black */
    FONT_WEIGHT_900,
};

/**
 * @brief baseline position.
 */
enum OH_Drawing_TextBaseline {
    /** It is used by alphabetic language, baseline is at the lower center */
    TEXT_BASELINE_ALPHABETIC,
    /** It is used by ideographic language, baseline is at the bottom */
    TEXT_BASELINE_IDEOGRAPHIC,
};

/**
 * @brief text decoration.
 */
enum OH_Drawing_TextDecoration {
    /** no decorations */
    TEXT_DECORATION_NONE = 0x0,
    /** a line at the bottom of text */
    TEXT_DECORATION_UNDERLINE = 0x1,
    /** a line at the top of text */
    TEXT_DECORATION_OVERLINE = 0x2,
    /** a line through the text */
    TEXT_DECORATION_LINE_THROUGH = 0x4,
};

/**
 * @brief to distinguish if fonts are italic.
 */
enum OH_Drawing_FontStyle {
    /** font is normal */
    FONT_STYLE_NORMAL,
    /** font is italic */
    FONT_STYLE_ITALIC,
};

/**
 * @brief Create a new TypographyStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_TypographyStyle
 * @since 8
 * @version 1.0
 */
OH_Drawing_TypographyStyle* OH_Drawing_CreateTypographyStyle(void);

/**
 * @brief Release the memory storing the OH_Drawing_TypographyStyle object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypographyStyle(OH_Drawing_TypographyStyle*);

/**
 * @brief set typography text direction.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int enum OH_Drawing_TextDirection
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextDirection(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextDirection */);

/**
 * @brief set typography text align.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int enum OH_Drawing_TextAlign
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextAlign(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextAlign */);

/**
 * @brief set typography text max lines.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int max lines
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTypographyTextMaxLines(OH_Drawing_TypographyStyle*, int /* maxLines */);

/**
 * @brief Create a new OH_Drawing_TextStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return OH_Drawing_TextStyle a pointer to created OH_Drawing_TextStyle
 * @since 8
 * @version 1.0
 */
OH_Drawing_TextStyle* OH_Drawing_CreateTextStyle(void);

/**
 * @brief Release the memory storing the OH_Drawing_TextStyle object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTextStyle(OH_Drawing_TextStyle*);

/**
 * @brief set typography text color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param uint32_t color
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief set typography font size.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param double font size
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontSize(OH_Drawing_TextStyle*, double /* fontSize */);

/**
 * @brief set typography font weight.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int enum OH_Drawing_FontWeight
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontWeight(OH_Drawing_TextStyle*, int /* OH_Drawing_FontWeight */);

/**
 * @brief set typography baseline.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int enum OH_Drawing_TextBaseline
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleBaseLine(OH_Drawing_TextStyle*, int /* OH_Drawing_TextBaseline */);

/**
 * @brief set typography decoration.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int enum OH_Drawing_TextDecoration
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecoration(OH_Drawing_TextStyle*, int /* OH_Drawing_TextDecoration */);

/**
 * @brief set typography decoration color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param uint32_t color
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleDecorationColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief set typography font height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param double font height
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontHeight(OH_Drawing_TextStyle*, double /* fontHeight */);

/**
 * @brief set typography font families.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int number of font families
 * @param char a pointer to fontFamilies name
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontFamilies(OH_Drawing_TextStyle*,
    int /* fontFamiliesNumber */, const char* fontFamilies[]);

/**
 * @brief set typography font style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int fontStyle
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleFontStyle(OH_Drawing_TextStyle*, int /* fontStyle */);

/**
 * @brief set typography locale.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param char locale, a pointer to char
 * @since 8
 * @version 1.0
 */
void OH_Drawing_SetTextStyleLocale(OH_Drawing_TextStyle*, const char*);

/**
 * @brief Generate a new OH_Drawing_TypographyCreate.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle
 * @param OH_Drawing_FontCollection a pointer to OH_Drawing_FontCollection
 * @return a pointer to created OH_Drawing_TypographyCreate
 * @since 8
 * @version 1.0
 */
OH_Drawing_TypographyCreate* OH_Drawing_CreateTypographyHandler(OH_Drawing_TypographyStyle*,
    OH_Drawing_FontCollection*);

/**
 * @brief Release the memory storing the OH_Drawing_TypographyCreate object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypographyHandler(OH_Drawing_TypographyCreate*);

/**
 * @brief set typography style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerPushTextStyle(OH_Drawing_TypographyCreate*, OH_Drawing_TextStyle*);

/**
 * @brief set typography text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @param char describe text
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerAddText(OH_Drawing_TypographyCreate*, const char*);

/**
 * @brief pop typography text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyHandlerPopTextStyle(OH_Drawing_TypographyCreate*);

/**
 * @brief Generate a new OH_Drawing_Typography.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_Typography
 * @return OH_Drawing_Typography a pointer to OH_Drawing_Typography
 * @since 8
 * @version 1.0
 */
OH_Drawing_Typography* OH_Drawing_CreateTypography(OH_Drawing_TypographyCreate*);

/**
 * @brief Release the memory storing the OH_Drawing_Typography object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_DestroyTypography(OH_Drawing_Typography*);

/**
 * @brief typography layout.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 * @param double max width of text
 * @since 8
 * @version 1.0
 */
void OH_Drawing_TypographyLayout(OH_Drawing_Typography*, double /* maxWidth */);

/**
 * @brief typography paint.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param double position X
 * @param double position Y
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