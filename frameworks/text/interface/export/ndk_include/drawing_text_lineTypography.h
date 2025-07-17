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

#ifndef DRAWING_TEXT_LINETYPOGRAPHY_H
#define DRAWING_TEXT_LINETYPOGRAPHY_H

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
 * @file drawing_text_linetypography.h
 *
 * @brief Declares functions related to <b>lineTypography</b> in the drawing module.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 14
 * @version 1.0
 */

#include "cstddef"
#include "drawing_text_declaration.h"
#include "drawing_canvas.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_LineTypography</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_LineTypography</b> object created.
 * @since 14
 * @version 1.0
 */
OH_Drawing_LineTypography* OH_Drawing_CreateLineTypography(OH_Drawing_TypographyCreate* handler);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_LineTypography</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_LineTypography Indicates the pointer to an <b>OH_Drawing_LineTypography</b> object.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroyLineTypography(OH_Drawing_LineTypography* lineTypography);

/**
 * @brief Calculate the line breakpoint based on the width provided.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @param startIndex Indicates the starting point for the line-break calculations.
 * @param width Indicates the requested line-break width.
 * @return Returns the count of the characters from startIndex that would cause the line break.
 * @since 14
 * @version 1.0
 */
size_t OH_Drawing_LineTypographyGetLineBreak(OH_Drawing_LineTypography* lineTypography,
                                             size_t startIndex, double width);

/**
 * @brief Creates a text line object based on the text range provided.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_LineTypography Indicates the pointer to an <b>OH_Drawing_TypographyCreate</b> object.
 * @param startIndex Indicates the starting index of the text range.
 * @param count Indicates the characters count of the text range.
 * @return Returns the pointer to the <b>OH_Drawing_TextLine</b> object created.
 * @since 14
 * @version 1.0
 */
OH_Drawing_TextLine* OH_Drawing_LineTypographyCreateLine(OH_Drawing_LineTypography* lineTypography,
                                                         size_t startIndex, size_t count);
#ifdef __cplusplus
}
#endif
/** @} */
#endif
