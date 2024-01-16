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

#ifndef C_INCLUDE_DRAWING_FONT_H
#define C_INCLUDE_DRAWING_FONT_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 11
 * @version 1.0
 */

/**
 * @file drawing_font.h
 *
 * @brief Declares functions related to the <b>font</b> object in the drawing module.
 *
 * @since 11
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Font</b> object created.
 * @since 11
 * @version 1.0
 */
OH_Drawing_Font* OH_Drawing_FontCreate(void);

/**
 * @brief Sets an <b>OH_Drawing_Typeface</b> object for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_Typeface Indicates the pointer to an <b>OH_Drawing_Typeface</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTypeface(OH_Drawing_Font*, OH_Drawing_Typeface*);

/**
 * @brief Sets text size for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param textSize Indicates the text size.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTextSize(OH_Drawing_Font*, float textSize);

/**
 * @brief Enables or disables linearly scalable font for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param isLinearText Indicates whether to enable linearly scalable font.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetLinearText(OH_Drawing_Font*, bool isLinearText);

/**
 * @brief Sets text skew on x-axis for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param skewX Indicates the additional shear on x-axis relative to y-axis.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTextSkewX(OH_Drawing_Font*, float skewX);

/**
 * @brief Enables or disables to increase stroke width to approximate bold fonts for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param isFakeBoldText Indicates whether to enable to increase stroke width.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetFakeBoldText(OH_Drawing_Font*, bool isFakeBoldText);

/**
 * @brief Destroys an <b>OH_Drawing_Font</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontDestroy(OH_Drawing_Font*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
