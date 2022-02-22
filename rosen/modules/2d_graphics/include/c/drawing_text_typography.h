/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_text_declaration.h"
#include "drawing_types.h"

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OH_Drawing_TextDirection used to the direction of text.
 */
enum OH_Drawing_TextDirection {
    TEXT_DIRECTION_RTL,
    TEXT_DIRECTION_LTR,
};

/**
 * @brief OH_Drawing_TextAlign used to describe layout.
 */
enum OH_Drawing_TextAlign {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_JUSTIFY,
    TEXT_ALIGN_START,
    TEXT_ALIGN_END,
};

/**
 * @brief OH_Drawing_FontWeight used to describe font weight.
 */
enum OH_Drawing_FontWeight {
    FONT_WEIGHT_100, // Thin
    FONT_WEIGHT_200, // Extra-Light
    FONT_WEIGHT_300, // Light
    FONT_WEIGHT_400, // Normal/Regular
    FONT_WEIGHT_500, // Medium
    FONT_WEIGHT_600, // Semi-bold
    FONT_WEIGHT_700, // Bold
    FONT_WEIGHT_800, // Extra-Bold
    FONT_WEIGHT_900, // Black
};

/**
 * @brief OH_Drawing_TextBaseline used to distinguish baseline position.
 */
enum OH_Drawing_TextBaseline {
    TEXT_BASELINE_ALPHABETIC,
    TEXT_BASELINE_IDEOGRAPHIC,
};

/**
 * @brief OH_Drawing_TextDecoration used to describe decorations.
 */
enum OH_Drawing_TextDecoration {
    TEXT_DECORATION_NONE = 0x0,
    TEXT_DECORATION_UNDERLINE = 0x1,
    TEXT_DECORATION_OVERLINE = 0x2,
    TEXT_DECORATION_LINE_THROUGH = 0x4,
};

/**
 * @brief OH_Drawing_FontStyle used to describe if fonts are italic.
 */
enum OH_Drawing_FontStyle {
    FONT_STYLE_NORMAL,
    FONT_STYLE_ITALIC,
};

/**
 * @brief Create a new TypographyStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_TypographyStyle
 */
OH_Drawing_TypographyStyle* OH_Drawing_CreateTypographyStyle(void);

/**
 * @brief Release the memory storing the OH_Drawing_TypographyStyle object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 */
void OH_Drawing_DestroyTypographyStyle(OH_Drawing_TypographyStyle*);

/**
 * @brief set typography text direction.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int enum OH_Drawing_TextDirection
 */
void OH_Drawing_SetTypographyTextDirection(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextDirection */);

/**
 * @brief set typography text align.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int enum OH_Drawing_TextAlign
 */
void OH_Drawing_SetTypographyTextAlign(OH_Drawing_TypographyStyle*, int /* OH_Drawing_TextAlign */);

/**
 * @brief set typography text max lines.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @param int max lines
 */
void OH_Drawing_SetTypographyTextMaxLines(OH_Drawing_TypographyStyle*, int /* maxLines */);

/**
 * @brief Create a new OH_Drawing_TextStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_TextStyle
 */
OH_Drawing_TextStyle* OH_Drawing_CreateTextStyle(void);

/**
 * @brief Release the memory storing the OH_Drawing_TextStyle object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TextStyle object
 */
void OH_Drawing_DestroyTextStyle(OH_Drawing_TextStyle*);

/**
 * @brief set typography text color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param uint32_t color
 */
void OH_Drawing_SetTextStyleColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief set typography font size.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param double font size
 */
void OH_Drawing_SetTextStyleFontSize(OH_Drawing_TextStyle*, double /* fontSize */);

/**
 * @brief set typography font weight.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int font weight
 */
void OH_Drawing_SetTextStyleFontWeight(OH_Drawing_TextStyle*, int /* OH_Drawing_FontWeight */);

/**
 * @brief set typography baseline.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int baseline position(enum OH_Drawing_TextBaseline)
 */
void OH_Drawing_SetTextStyleBaseLine(OH_Drawing_TextStyle*, int /* OH_Drawing_TextBaseline */);

/**
 * @brief set typography decoration.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int enum OH_Drawing_TextDecoration
 */
void OH_Drawing_SetTextStyleDecoration(OH_Drawing_TextStyle*, int /* OH_Drawing_TextDecoration */);

/**
 * @brief set typography decoration color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param uint32_t color
 */
void OH_Drawing_SetTextStyleDecorationColor(OH_Drawing_TextStyle*, uint32_t /* color */);

/**
 * @brief set typography font height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param double font height
 */
void OH_Drawing_SetTextStyleFontHeight(OH_Drawing_TextStyle*, double /* fontHeight */);

/**
 * @brief set typography font families.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int number of font families
 * @param char* fontFamilies name
 */
void OH_Drawing_SetTextStyleFontFamilies(OH_Drawing_TextStyle*,
    int /* fontFamiliesNumber */, const char* fontFamilies[]);

/**
 * @brief set typography font style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param int fontStyle
 */
void OH_Drawing_SetTextStyleFontStyle(OH_Drawing_TextStyle*, int /* fontStyle */);

/**
 * @brief set typography locale.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @param char* locale
 */
void OH_Drawing_SetTextStyleLocale(OH_Drawing_TextStyle*, const char*);

/**
 * @brief Generate a new OH_Drawing_TypographyCreate.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_TypographyCreate
 */
OH_Drawing_TypographyCreate* OH_Drawing_CreateTypographyHandler(OH_Drawing_TypographyStyle*,
    OH_Drawing_FontCollection*);

/**
 * @brief Release the memory storing the OH_Drawing_TypographyCreate object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 */
void OH_Drawing_DestroyTypographyHandler(OH_Drawing_TypographyCreate*);

/**
 * @brief set typography style.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 */
void OH_Drawing_TypographyHandlerPushStyle(OH_Drawing_TypographyCreate*, OH_Drawing_TextStyle*);

/**
 * @brief set typography text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @param char describe text
 */
void OH_Drawing_TypographyHandlerAddText(OH_Drawing_TypographyCreate*, const char*);

/**
 * @brief pop typography text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 */
void OH_Drawing_TypographyHandlerPop(OH_Drawing_TypographyCreate*);

/**
 * @brief Generate a new OH_Drawing_Typography.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to OH_Drawing_Typography
 */
OH_Drawing_Typography* OH_Drawing_CreateTypography(OH_Drawing_TypographyCreate*);

/**
 * @brief Release the memory storing the OH_Drawing_Typography object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 */
void OH_Drawing_DestroyTypography(OH_Drawing_Typography*);

/**
 * @brief typography layout.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 * @param double max width of text
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
 */
void OH_Drawing_TypographyPaint(OH_Drawing_Typography*, OH_Drawing_Canvas*,
    double /* potisionX */, double /* potisionY */);

#ifdef __cplusplus
}
#endif

#endif