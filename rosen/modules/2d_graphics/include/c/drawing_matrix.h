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

#ifndef C_INCLUDE_DRAWING_MATRIX_H
#define C_INCLUDE_DRAWING_MATRIX_H

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
 * @file drawing_matrix.h
 *
 * @brief Declares functions related to the <b>matrix</b> object in the drawing module.
 *
 * @since 11
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Matrix</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Matrix</b> object created.
 * @since 11
 * @version 1.0
 */
OH_Drawing_Matrix* OH_Drawing_MatrixCreate(void);

/**
 * @brief Sets the params for a matrix.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Matrix Indicates the pointer to an <b>OH_Drawing_Matrix</b> object.
 * @param scaleX  horizontal scale factor to store
 * @param skewX   horizontal skew factor to store
 * @param transX  horizontal translation to store
 * @param skewY   vertical skew factor to store
 * @param scaleY  vertical scale factor to store
 * @param transY  vertical translation to store
 * @param persp0  input x-axis values perspective factor to store
 * @param persp1  input y-axis values perspective factor to store
 * @param persp2  perspective scale factor to store
 * @since 11
 * @version 1.0
 */
void OH_Drawing_MatrixSetMatrix(OH_Drawing_Matrix*, float scaleX, float skewX, float transX,
    float skewY, float scaleY, float transY, float persp0, float persp1, float persp2);

/**
 * @brief Destroys an <b>OH_Drawing_Matrix</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Matrix Indicates the pointer to an <b>OH_Drawing_Matrix</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_MatrixDestroy(OH_Drawing_Matrix*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
