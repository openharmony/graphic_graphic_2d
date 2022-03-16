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
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 * 
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_path.h
 *
 * @brief Declares functions related to the <b>path</b> object in the drawing module.
 *
 * @since 8
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Path</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Path</b> object created.
 * @since 8
 * @version 1.0
 */
OH_Drawing_Path* OH_Drawing_PathCreate(void);

/**
 * @brief Destroys an <b>OH_Drawing_Path</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathDestroy(OH_Drawing_Path*);

/**
 * @brief Sets the start point of a path.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param x Indicates the x coordinate of the start point.
 * @param y Indicates the y coordinate of the start point.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathMoveTo(OH_Drawing_Path*, float x, float y);

/**
 * @brief Draws a line segment from the last point of a path to the target point.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param x Indicates the x coordinate of the target point.
 * @param y Indicates the y coordinate of the target point.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathLineTo(OH_Drawing_Path*, float x, float y);

/**
 * @brief Draws an arc to a path. 
 * 
 * This is done by using angle arc mode. In this mode, a rectangle that encloses an ellipse is specified first,
 * and then a start angle and a sweep angle are specified.
 * The arc is a portion of the ellipse defined by the start angle and the sweep angle. 
 * By default, a line segment from the last point of the path to the start point of the arc is also added.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param x1 Indicates the x coordinate of the upper left corner of the rectangle.
 * @param y1 Indicates the y coordinate of the upper left corner of the rectangle.
 * @param x2 Indicates the x coordinate of the lower right corner of the rectangle.
 * @param y3 Indicates the y coordinate of the lower right corner of the rectangle.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathArcTo(OH_Drawing_Path*, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg);

/**
 * @brief Draws a quadratic Bezier curve from the last point of a path to the target point.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param ctrlX Indicates the x coordinate of the control point.
 * @param ctrlY Indicates the y coordinate of the control point.
 * @param endX Indicates the x coordinate of the target point.
 * @param endY Indicates the y coordinate of the target point.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathQuadTo(OH_Drawing_Path*, float ctrlX, float ctrlY, float endX, float endY);

/**
 * @brief Draws a cubic Bezier curve from the last point of a path to the target point.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param ctrlX1 Indicates the x coordinate of the first control point.
 * @param ctrlY1 Indicates the y coordinate of the first control point.
 * @param ctrlX2 Indicates the x coordinate of the second control point.
 * @param ctrlY2 Indicates the y coordinate of the second control point.
 * @param endX Indicates the x coordinate of the target point.
 * @param endY Indicates the y coordinate of the target point.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathCubicTo(
    OH_Drawing_Path*, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY);

/**
 * @brief Closes a path. A line segment from the start point to the last point of the path is added.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathClose(OH_Drawing_Path*);

/**
 * @brief Resets path data.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @since 8
 * @version 1.0
 */
void OH_Drawing_PathReset(OH_Drawing_Path*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
