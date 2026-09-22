/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_BLOB_H
#define C_INCLUDE_DRAWING_TEXT_BLOB_H

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
 * @file drawing_text_blob.h
 *
 * @brief Declares functions related to the <b>textBlob</b> object in the drawing module.
 *
 * @since 11
 * @version 1.0
 */

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_TextBlobBuilder</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_TextBlobBuilder</b> object created.
 * @since 11
 * @version 1.0
 */
OH_Drawing_TextBlobBuilder* OH_Drawing_TextBlobBuilderCreate(void);

/**
 * @brief Creates an <b>OH_Drawing_TextBlob</b> object from text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param text Indicates the the pointer to text.
 * @param byteLength Indicates the text length.
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_TextEncoding Indicates the pointer to an <b>OH_Drawing_TextEncoding</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_TextBlob</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromText(const void* text, size_t byteLength,
    const OH_Drawing_Font*, OH_Drawing_TextEncoding);

/**
 * @brief Creates a sequence of `OH_Drawing_TextBlob` objects from the text with font fallback support.
 * When the typeface of the current font does not support certain characters, it automatically finds fallback
 * typefaces from the system. One text blob is created per run of consecutive codepoints that share the same
 * typeface. All blobs share the coordinate system of the whole string: each blob's glyph positions already include
 * the advance of preceding runs, so every blob should be drawn at the same origin.
 *
 * @param text [in] Pointer to the text.
 * @param byteLength [in] Length of the text, in bytes.
 * @param cFont [in] Pointer to the {@link OH_Drawing_Font} object.
 * @param textEncoding [in] Text encoding type {@link OH_Drawing_TextEncoding}.
 * @param textBlobs [out] Pointer to an array of `OH_Drawing_TextBlob` objects.
 *        It is used as an output parameter.
 *        Uses {@link OH_Drawing_TextBlobsArrayDestroy} to release the array when it is no longer needed.
 * @param textBlobsCount [out] Pointer to the count of TextBlob in the array. It is used as an output parameter.
 * @return <ul>
 *         <li>{@link OH_DRAWING_SUCCESS} if the operation is successful.</li>
 *         <li>{@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if any of text, font,
 *         textBlobs, or textBlobsCount is NULL, or byteLength is 0.</li>
 *         <li>{@link OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE} if textEncoding is
 *         not set to one of the enumerated values.</li>
 *         <li>{@link OH_DRAWING_ERROR_ALLOCATION_FAILED} if the result array cannot be allocated.</li>
 *         </ul>
 * @release drawing_text_blob/OH_Drawing_TextBlobsArrayDestroy {textBlobs}
 * @since 26.0.1
 */
OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromTextWithFallback(const void *text, uint32_t byteLength,
    const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding, OH_Drawing_TextBlob ***textBlobs,
    uint32_t *textBlobsCount);

/**
 * @brief Creates an <b>OH_Drawing_TextBlob</b> object from pos text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param text Indicates the the pointer to text.
 * @param byteLength Indicates the text length.
 * @param OH_Drawing_Point2D Indicates the pointer to an <b>OH_Drawing_Point2D</b> array object.
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_TextEncoding Indicates the pointer to an <b>OH_Drawing_TextEncoding</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_TextBlob</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromPosText(const void* text, size_t byteLength,
    OH_Drawing_Point2D*, const OH_Drawing_Font*, OH_Drawing_TextEncoding);

/**
 * @brief Creates a sequence of `OH_Drawing_TextBlob` objects from text with font fallback support.
 * When the typeface of the current font does not support certain characters, it automatically finds fallback
 * typefaces from the system. If no fallback typeface is found, the typeface of the current font is still used.
 * One text blob is created per run of consecutive codepoints that share the same typeface.
 * The coordinates of each character in the `OH_Drawing_TextBlob` object are determined by the coordinate
 * information in the `OH_Drawing_Point2D` array.
 *
 * @param text [in] Pointer to the text.
 * @param byteLength [in] Length of the text, in bytes.
 * @param cPoints [in] Pointer to the start address of the {@link OH_Drawing_Point2D} array.
 *        The number of elements in the array is determined by {@link OH_Drawing_FontCountText}.
 * @param cFont [in] Pointer to the {@link OH_Drawing_Font} object.
 * @param textEncoding [in] Text encoding type {@link OH_Drawing_TextEncoding}.
 * @param textBlobs [out] Pointer to an array of <b>OH_Drawing_TextBlob</b> objects.
 *        It is used as an output parameter.
 *        Uses {@link OH_Drawing_TextBlobsArrayDestroy} to release the array when it is no longer needed.
 * @param textBlobsCount [out] Pointer to the count of TextBlob in the array. It is used as an output parameter.
 * @return <ul>
 *         <li>{@link OH_DRAWING_SUCCESS} if the operation is successful.</li>
 *         <li>{@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if any of text, point2D, font, textBlobs, and textBlobsCount
 *         is NULL, or byteLength is 0.</li>
 *         <li>{@link OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE} if textEncoding is
 *         not set to one of the enumerated values.</li>
 *         <li>{@link OH_DRAWING_ERROR_ALLOCATION_FAILED} if the result array cannot be allocated.</li>
 *         </ul>
 * @release drawing_text_blob/OH_Drawing_TextBlobsArrayDestroy {textBlobs}
 * @since 26.0.1
 */
OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromPosTextWithFallback(const void *text, uint32_t byteLength,
    OH_Drawing_Point2D *cPoints, const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding,
    OH_Drawing_TextBlob ***textBlobs, uint32_t *textBlobsCount);
/**
 * @brief Creates an <b>OH_Drawing_TextBlob</b> object from pos text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param str Indicates the the pointer to text.
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_TextEncoding Indicates the pointer to an <b>OH_Drawing_TextEncoding</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_TextBlob</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromString(const char* str,
    const OH_Drawing_Font*, OH_Drawing_TextEncoding);


/**
 * @brief Creates a sequence of `OH_Drawing_TextBlob` objects from a string with font fallback support.
 * When the typeface of the current font does not support certain characters, it automatically finds fallback
 * typefaces from the system. One text blob is created per run of consecutive codepoints that share the same
 * typeface. All blobs share the coordinate system of the whole string: each blob's glyph positions already include
 * the advance of preceding runs, so every blob should be drawn at the same origin.
 *
 * @param str [in] Pointer to a string.
 * @param cFont [in] Pointer to the {@link OH_Drawing_Font} object.
 * @param textEncoding [in] Text encoding type {@link OH_Drawing_TextEncoding}.
 * @param textBlobs [out] Pointer to an array of `OH_Drawing_TextBlob` objects.
 *        It is used as an output parameter.
 *        Uses {@link OH_Drawing_TextBlobsArrayDestroy} to release the array when it is no longer needed.
 * @param textBlobsCount [out] Pointer to the count of TextBlob in the array. It is used as an output parameter.
 * @return <ul>
 *         <li>{@link OH_DRAWING_SUCCESS} if the operation is successful.</li>
 *         <li>{@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if any of str, font,
 *         textBlobs, or textBlobsCount is NULL.</li>
 *         <li>{@link OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE} if textEncoding is
 *         not set to one of the enumerated values.</li>
 *         <li>{@link OH_DRAWING_ERROR_ALLOCATION_FAILED} if the result array cannot be allocated.</li>
 *         </ul>
 * @release drawing_text_blob/OH_Drawing_TextBlobsArrayDestroy {textBlobs}
 * @since 26.0.1
 */
OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromStringWithFallback(const char *str,
    const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding,
    OH_Drawing_TextBlob ***textBlobs,  uint32_t *textBlobsCount);

/**
 * @brief Gets the bounds of textblob, assigned to the pointer to an <b>OH_Drawing_Rect</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlob Indicates the pointer to an <b>OH_Drawing_TextBlob</b> object.
 * @param OH_Drawing_Rect Indicates the pointer to an <b>OH_Drawing_Rect</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_TextBlobGetBounds(OH_Drawing_TextBlob*, OH_Drawing_Rect*);

/**
 * @brief Gets a non-zero value unique among all <b>OH_Drawing_TextBlob</b> objects.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlob Indicates the pointer to an <b>OH_Drawing_TextBlob</b> object.
 * @return Returns identifier for the <b>OH_Drawing_TextBlob</b> object.
 * @since 12
 * @version 1.0
 */
uint32_t OH_Drawing_TextBlobUniqueID(const OH_Drawing_TextBlob*);

/**
 * @brief Defines a run, supplies storage for glyphs and positions.
 *
 * @since 11
 * @version 1.0
 */
typedef struct {
    /** storage for glyph indexes in run */
    uint16_t* glyphs;
    /** storage for glyph positions in run */
    float* pos;
    /** storage for text UTF-8 code units in run */
    char* utf8text;
    /** storage for glyph clusters (index of UTF-8 code unit) */
    uint32_t* clusters;
} OH_Drawing_RunBuffer;

/**
 * @brief Alloc run with storage for glyphs and positions. The returned pointer does not need to be managed
 * by the caller and is forbidden to be used after OH_Drawing_TextBlobBuilderMake is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlobBuilder Indicates the pointer to an <b>OH_Drawing_TextBlobBuilder</b> object.
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param count Indicates the number of glyphs.
 * @param OH_Drawing_Rect Indicates the optional run bounding box.
 * @since 11
 * @version 1.0
 */
const OH_Drawing_RunBuffer* OH_Drawing_TextBlobBuilderAllocRunPos(OH_Drawing_TextBlobBuilder*, const OH_Drawing_Font*,
    int32_t count, const OH_Drawing_Rect*);

/**
 * @brief Make an <b>OH_Drawing_TextBlob</b> from <b>OH_Drawing_TextBlobBuilder</b>.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlobBuilder Indicates the pointer to an <b>OH_Drawing_TextBlobBuilder</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_TextBlob</b> object.
 * @since 11
 * @version 1.0
 */
OH_Drawing_TextBlob* OH_Drawing_TextBlobBuilderMake(OH_Drawing_TextBlobBuilder*);

/**
 * @brief Destroys an <b>OH_Drawing_TextBlob</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlob Indicates the pointer to an <b>OH_Drawing_TextBlob</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_TextBlobDestroy(OH_Drawing_TextBlob*);

/**
 * @brief Destroys an <b>OH_Drawing_TextBlobBuilder</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextBlobBuilder Indicates the pointer to an <b>OH_Drawing_TextBlobBuilder</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_TextBlobBuilderDestroy(OH_Drawing_TextBlobBuilder*);

/**
 * @brief Destroys an array of `OH_Drawing_TextBlob` objects and reclaims the memory occupied by the array.
 * This function destroys every text blob in the array that is still alive and releases the array itself.
 * <b>count</b> must be exactly the number reported when the array was created; passing any other value results in
 * undefined behavior.
 *
 * @param textBlobs [in] Pointer to an array of `OH_Drawing_TextBlob` objects.
 * @param count [in] The size of textBlobs array.
 * @return <ul>
 *         <li>{@link OH_DRAWING_SUCCESS} if the operation is successful.</li>
 *         <li>{@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if textBlobs is NULL or count is 0.</li>
 *         </ul>
 * @since 26.0.1
 */
OH_Drawing_ErrorCode OH_Drawing_TextBlobsArrayDestroy(OH_Drawing_TextBlob **textBlobs, uint32_t count);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
