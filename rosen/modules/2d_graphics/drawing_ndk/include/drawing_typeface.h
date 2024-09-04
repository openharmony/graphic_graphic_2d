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

#ifndef C_INCLUDE_DRAWING_TYPEFACE_H
#define C_INCLUDE_DRAWING_TYPEFACE_H

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
 * @file drawing_typeface.h
 *
 * @brief Declares functions related to the <b>typeface</b> object in the drawing module.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 11
 * @version 1.0
 */

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a default <b>OH_Drawing_Typeface</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object created.
 * @since 11
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_TypefaceCreateDefault(void);

/**
 * @brief Creates an <b>OH_Drawing_Typeface</b> object by file.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param path  file path.
 * @param index  file index.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromFile(const char* path, int index);

/**
 * @brief Creates an <b>OH_Drawing_Typeface</b> object with the specified font arguments from a file.
 * If the <b>OH_Drawing_Typeface</b> object does not support the variations described in fontArguments,
 * this function creates an <b>OH_Drawing_Typeface</b> object without font arguments.
 * In this case, this function provides the same capability as {@link OH_Drawing_TypefaceCreateFromFile}.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param path Indicates the file path.
 * @param fontArguments Indicates the pointer to an <b>OH_Drawing_FontArguments</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is that the available memory is empty,
 *         or either path or fontArguments is nullptr, or the path is invalid.
 * @since 13
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromFileWithArguments(const char* path,
    const OH_Drawing_FontArguments* fontArguments);

/**
 * @brief Creates an <b>OH_Drawing_Typeface</b> object with the specified font arguments from
 * an existing <b>OH_Drawing_Typeface</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param current Indicates the existing <b>OH_Drawing_Typeface</b> object.
 * @param fontArguments Indicates the pointer to an <b>OH_Drawing_FontArguments</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is that the available memory is empty,
 *         or either current or fontArguments is nullptr,
 *         or current does not support the variations described in fontArguments.
 * @since 13
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromCurrent(const OH_Drawing_Typeface* current,
    const OH_Drawing_FontArguments* fontArguments);

/**
 * @brief Creates an <b>OH_Drawing_Typeface</b> object by given a stream. If the stream is not a valid
 * font file, returns nullptr. Ownership of the stream is transferred, so the caller must not reference
 * it or free it again.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_MemoryStream Indicates the pointer to an <b>OH_Drawing_MemoryStream</b> object.
 * @param index  memory stream index.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromStream(OH_Drawing_MemoryStream*, int32_t index);

/**
 * @brief Destroys an <b>OH_Drawing_Typeface</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typeface Indicates the pointer to an <b>OH_Drawing_Typeface</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_TypefaceDestroy(OH_Drawing_Typeface*);

/**
 * @brief Creates an <b>OH_Drawing_FontArguments</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_FontArguments</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is that the available memory is empty.
 * @since 13
 * @version 1.0
 */
OH_Drawing_FontArguments* OH_Drawing_FontArgumentsCreate(void);

/**
 * @brief Adds a font variation axis for an <b>OH_Drawing_FontArguments</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param fontArguments Indicates the pointer to an <b>OH_Drawing_FontArguments</b> object.
 * @param axis Indicates the axis tag, which must contain four ASCII characters.
 * @param value Indicates the value of the axis field.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if either fontArguments or axis is nullptr,
 *                 or the length of axis is not 4.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_FontArgumentsAddVariation(OH_Drawing_FontArguments* fontArguments,
    const char* axis, float value);

/**
 * @brief Destroys an <b>OH_Drawing_FontArguments</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param fontArguments Indicates the pointer to an <b>OH_Drawing_FontArguments</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if fontArguments is nullptr.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_FontArgumentsDestroy(OH_Drawing_FontArguments* fontArguments);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
