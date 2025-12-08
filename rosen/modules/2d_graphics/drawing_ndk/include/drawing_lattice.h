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
 * @file drawing_lattice.h
 *
 * @brief Declares functions related to the <b>lattice</b> object in the drawing module.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 23
 */
#ifndef C_INCLUDE_DRAWING_LATTICE_H
#define C_INCLUDE_DRAWING_LATTICE_H

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the types of rectangles used to fill the lattices. This enum is used only in Lattice.
 *
 * @since 23
 */
typedef enum {
    /** Draws an image into the lattice */
    DEFAULT,
    /** Sets the lattice to transparent */
    TRANSPARENT,
    /** Draws the colors in the fColors array in Lattice into the lattice */
    FIXED_COLOR,
} OH_Drawing_LatticeRectType;

/**
 * @brief Destroys an <b>OH_Drawing_Lattice</b> object and reclaims the memory occupied by the object.
 *
 * @param lattice Indicates the pointer to an <b>OH_Drawing_Lattice</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} If the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} If lattice is nullptr.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_LatticeDestroy(OH_Drawing_Lattice* lattice);

/**
 * @brief Creates an <b>OH_Drawing_Lattice</b> that divides the image into lattices.
 *     The lattices on both even columns and even rows are fixed,
 *     and they are drawn at their original size if the target is large enough.
 *     If the target is too small to hold the fixed lattices, all the fixed lattices are scaled down to fit the target,
 *     and the lattices that are not on even columns and even rows are scaled to accommodate the remaining space.
 *
 * @param xDivs Indicates the array of X coordinates used to divide the image. The value is an integer.
 * @param yDivs Indicates the array of Y coordinates used to divide the image. The value is an integer.
 * @param xCount Indicates the number of xDivs. The value range is [0, 5].
 * @param yCount Indicates the number of yDivs. The value range is [0, 5].
 * @param bounds Indicates source bounds to draw. The rectangle parameter must be an integer.
 *     The default value is the rectangle size of the original image. If the rectangle parameter is a decimal,
 *     the decimal part is discarded and converted into an integer.
 * @param rectTypes Indicates array that holds the rectangle types.
 * @param rectTypeCount Indicates the number of rectTypes. If rectTypes is not a null pointer,
 *     its size must be (xCount + 1) * (yCount + 1). If it is null, the size is 0.
 * @param colors Array that holds the colors used to fill the lattices.
 * @param colorCount Indicates the number of colors. If colors is not a null pointer,
 *     its size must be (xCount + 1) * (yCount + 1). If it is null, the size is 0.
 * @param lattice The pointer to the <b>OH_Drawing_Lattice</b> object created returned to the caller.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INCORRECT_PARAMETER} if xDivs or yDivs is nullptr;
 *                     if rectTypes or colors exists, rectTypeCount or colorCount
 *                         does not equal (xCount + 1) * (yCount + 1);
 *                     if rectTypes or colors is nullptr, rectTypeCount or colorCount does not equal 0.
 *         Returns {@link OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE} the enumeration values of rectTypes
 *                 exceed the enumeration range.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_LatticeCreate(const int* xDivs, const int* yDivs, uint32_t xCount, uint32_t yCount,
    const OH_Drawing_Rect* bounds, const OH_Drawing_LatticeRectType* rectTypes, uint32_t rectTypeCount,
    const uint32_t* colors, uint32_t colorCount, OH_Drawing_Lattice** lattice);

#ifdef __cplusplus
}
#endif
#endif // C_INCLUDE_DRAWING_LATTICE_H

/** @} */
