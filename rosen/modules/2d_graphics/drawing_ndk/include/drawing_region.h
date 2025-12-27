/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_REGION_H
#define C_INCLUDE_DRAWING_REGION_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_region.h
 *
 * @brief Declares functions related to the <b>region</b> object in the drawing module.
 *
 * @since 12
 * @version 1.0
 */

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates of operations when two regions are combined.
 *
 * @since 12
 * @version 1.0
 */
typedef enum {
    /**
     * Difference operation.
     */
    REGION_OP_MODE_DIFFERENCE,
    /**
     * Intersect operation.
     */
    REGION_OP_MODE_INTERSECT,
    /**
     * Union operation.
     */
    REGION_OP_MODE_UNION,
    /**
     * Xor operation.
     */
    REGION_OP_MODE_XOR,
    /**
     * Reverse difference operation.
     */
    REGION_OP_MODE_REVERSE_DIFFERENCE,
    /**
     * Replace operation.
     */
    REGION_OP_MODE_REPLACE,
} OH_Drawing_RegionOpMode;

/**
 * @brief Creates an <b>OH_Drawing_Region</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Region</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Region* OH_Drawing_RegionCreate(void);

/**
 * @brief Creates an <b>OH_Drawing_Region</b> copy object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object to copy.
 * @return Returns the pointer to the <b>OH_Drawing_Region</b> object created.
 * @since 20
 * @version 1.0
 */
OH_Drawing_Region* OH_Drawing_RegionCopy(const OH_Drawing_Region* region);

/**
 * @brief Determines whether the region contains the specified coordinates.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param int32_t x-coordinate.
 * @param int32_t y-coordinate.
 * @return Returns <b>true</b> if (x, y) is inside region; returns <b>false</b> otherwise.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_RegionContains(OH_Drawing_Region* region, int32_t x, int32_t y);

/**
 * @brief Combines two regions.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param dst Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param op Indicates the operation to apply to combine.
 * @return Returns <b>true</b> if constructed region is not empty; returns <b>false</b> otherwise.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_RegionOp(OH_Drawing_Region* region, const OH_Drawing_Region* dst, OH_Drawing_RegionOpMode op);

/**
 * @brief Set the region to the specified rect.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param OH_Drawing_Rect Indicates the pointer to an <b>OH_Drawing_Rect</b> object.
 * @return Return true if constructed Region is not empty.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_RegionSetRect(OH_Drawing_Region* region, const OH_Drawing_Rect* rect);

/**
 * @brief Constructs region that matchs outline of path within clip.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param clip Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @return Returns <b>true</b> if constructed region is not empty; returns <b>false</b> otherwise.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_RegionSetPath(OH_Drawing_Region* region, const OH_Drawing_Path* path, const OH_Drawing_Region* clip);

/**
 * @brief Destroys an <b>OH_Drawing_Region</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_RegionDestroy(OH_Drawing_Region*);

/**
 * @brief Set an <b>OH_Drawing_Region</b> empty.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region is nullptr.
 * @since 22
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_RegionEmpty(OH_Drawing_Region* region);

/**
 * @brief Set the path to the boundary of the region. If the region is empty, the path will also be empty.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or path is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionGetBoundaryPath(const OH_Drawing_Region* region, OH_Drawing_Path* path);

/**
 * @brief Obtains the minimum bounding rectangle that encloses this path.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param rect Indicates the pointer to an <b>OH_Drawing_Rect</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or rect is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionGetBounds(const OH_Drawing_Region* region, OH_Drawing_Rect* rect);

/**
 * @brief Checks if the region contains more than one rectangle.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param isComplex Indicates if the region is complex.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or isComplex is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionIsComplex(const OH_Drawing_Region* region, bool* isComplex);

/**
 * @brief Checks if the region is empty.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param isEmpty Indicates if the region is empty.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or isEmpty is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionIsEmpty(const OH_Drawing_Region* region, bool* isEmpty);

/**
 * @brief Checks if the region is equivalent to a single rectangle.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param isRect Indicates if the region contains a single rectangle.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or isRect is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionIsRect(const OH_Drawing_Region* region, bool* isRect);

/**
 * @brief Checks if the region is equivalent to a single rectangle and it contains the specified rectangle.
 * A false indicates that the region is not equivalent to a single rectangle or the rectangle is not contained by
 * this region. A true is a guarantee that the rectangle is contained by this region.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param left Indicates the left position of the rect.
 * @param top Indicates the top position of the rect.
 * @param right Indicates the right position of the rect.
 * @param bottom Indicates the bottom position of the rect.
 * @param isContained Indicates if the specified rectangle is contained in the region.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or isContained is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionQuickContains(const OH_Drawing_Region* region,
    int32_t left, int32_t top, int32_t right, int32_t bottom, bool* isContained);

/**
 * @brief Checks if the region is empty, or if the specified rectangle does not intersect the region.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param left Indicates the left position of the rect.
 * @param top Indicates the top position of the rect.
 * @param right Indicates the right position of the rect.
 * @param bottom Indicates the bottom position of the rect.
 * @param isReject Indicates if the specified rectangle does not intersect the region.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region or isReject is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionQuickReject(const OH_Drawing_Region* region,
    int32_t left, int32_t top, int32_t right, int32_t bottom, bool* isReject);

/**
 * @brief Translates the region by dx, dy. If the region is empty, do nothing.
 *
 * @param region Indicates the pointer to an <b>OH_Drawing_Region</b> object.
 * @param dx Indicates the distance to translate on x-axis in pixels.
 * @param dy Indicates the distance to translate on y-axis in pixels.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if region is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_RegionTranslate(OH_Drawing_Region* region, int32_t dx, int32_t dy);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
