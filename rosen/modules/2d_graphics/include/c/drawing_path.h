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

#ifndef C_INCLUDE_DRAWING_PATH_H
#define C_INCLUDE_DRAWING_PATH_H

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
 * @file drawing_path.h
 *
 * @brief Defines the path functions of the Drawing module.
 *
 * @since 8
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new Path.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_Path
 * @since 8
 * @version 1.0
 */
OH_Drawing_Path* OH_Drawing_PathCreate(void);

/**
 * @brief Release the memory storing the OH_Drawing_Path object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathDestroy(OH_Drawing_Path*);

/**
 * @brief Sets the beginning of the path at target point(x, y).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @param x x-axis value of target point
 * @param y y-axis value of target point
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathMoveTo(OH_Drawing_Path*, float x, float y);

/**
 * @brief Adds line from path last point to target point(x, y).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @param x x-axis value of target point
 * @param y y-axis value of target point
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathLineTo(OH_Drawing_Path*, float x, float y);

/**
 * @brief Adds a arc, and then adds line from path last point to point that is beginning of the arc.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @param x1 x-axis value of the top-left point of rectangle that surrounding the ellipse
 * @param y1 y-axis value of the top-left point of rectangle that surrounding the ellipse
 * @param x2 x-axis value of the bottom-right point of rectangle that surrounding the ellipse
 * @param y3 y-axis value of the bottom-right point of rectangle that surrounding the ellipse
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathArcTo(OH_Drawing_Path*, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg);

/**
 * @brief Adds quad from path last point to target point(endX, endY).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @param ctrlX x-axis value of control point
 * @param ctrlY y-axis value of control point
 * @param endX x-axis value of target point
 * @param endY y-axis value of target point
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathQuadTo(OH_Drawing_Path*, float ctrlX, float ctrlY, float endX, float endY);

/**
 * @brief Adds cubic from path last point to target point(endX, endY).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @param ctrlX1 x-axis value of first control point
 * @param ctrlY1 y-axis value of first control point
 * @param ctrlX2 x-axis value of second control point
 * @param ctrlY2 y-axis value of second control point
 * @param endX x-axis value of target point
 * @param endY y-axis value of target point
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathCubicTo(
    OH_Drawing_Path*, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY);

/**
 * @brief Adds line from path first point to path last point.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathClose(OH_Drawing_Path*);

/**
 * @brief Sets path to its initial state.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathReset(OH_Drawing_Path*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif