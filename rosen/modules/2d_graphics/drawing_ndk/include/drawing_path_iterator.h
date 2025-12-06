/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @since 8
 */

/**
 * @file drawing_path_iterator.h
 *
 * @brief Declares functions related to the <b>pathIterator</b> object in the drawing module.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 23
 */
#ifndef C_INCLUDE_DRAWING_PATH_ITERATOR_H
#define C_INCLUDE_DRAWING_PATH_ITERATOR_H

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the path operation types contained in an iterator. It is used to read path operation instructions.
 *
 * @since 23
 */
typedef enum {
    /** Sets the start point. */
    MOVE = 0,
    /** Adds a line segment. */
    LINE = 1,
    /** Adds a quadratic Bezier curve for smooth transitions. */
    QUAD = 2,
    /** Adds a conic curve. */
    CONIC = 3,
    /** Adds a cubic Bezier curve for smooth transitions. */
    CUBIC = 4,
    /** Closes a path. */
    CLOSE = 5,
    /** The path setting is complete. */
    DONE = CLOSE + 1,
} OH_Drawing_PathIteratorVerb;

/**
 * @brief Creates an <b>OH_Drawing_PathIterator</b> object from a path.
 *
 * @param path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param pathIterator The pointer to the <b>OH_Drawing_PathIterator</b> object created returned to the caller.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if path or pathIterator is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_PathIteratorCreate(const OH_Drawing_Path* path, OH_Drawing_PathIterator** pathIterator);

/**
 * @brief Destroys an <b>OH_Drawing_PathIterator</b> object and reclaims the memory occupied by the object.
 *
 * @param pathIterator Indicates the pointer to an <b>OH_Drawing_PathIterator</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if pathIterator is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_PathIteratorDestroy(OH_Drawing_PathIterator* pathIterator);

/**
 * @brief Checks if there is a next operation in the path operation iterator.
 *
 * @param pathIterator Indicates the pointer to an <b>OH_Drawing_PathIterator</b> object.
 * @param hasNext Indicates if there is a next operation in the path operation iterator.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if pathIterator or hasNext is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_PathIteratorHasNext(const OH_Drawing_PathIterator* pathIterator, bool* hasNext);

/**
 * @brief Retrieves the next operation in the path and moves the iterator to that operation.
 *
 * @param pathIterator Indicates the pointer to an <b>OH_Drawing_PathIterator</b> object.
 * @param points Indicates the point array.
 * @param count Indicates the size of the point array.
 * @param offset Indicates the offset into the array where entries should be placed.
 * @param verb Indicates the next operation in the path.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if any of pathIterator, points and verb is nullptr.
 *         Returns {@link OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE} if count is less than offset plus 4.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_PathIteratorNext(OH_Drawing_PathIterator* pathIterator,
    OH_Drawing_Point2D* points, uint32_t count, uint32_t offset, OH_Drawing_PathIteratorVerb* verb);

/**
 * @brief Retrieves the next operation in this path, without moving the iterator.
 *
 * @param pathIterator Indicates the pointer to an <b>OH_Drawing_PathIterator</b> object.
 * @param verb Indicates the next operation in the path.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if pathIterator or verb is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_PathIteratorPeek(const OH_Drawing_PathIterator* pathIterator,
    OH_Drawing_PathIteratorVerb* verb);

#ifdef __cplusplus
}
#endif
#endif // C_INCLUDE_DRAWING_PATH_ITERATOR_H

/** @} */
