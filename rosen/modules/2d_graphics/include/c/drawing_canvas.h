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

#ifndef C_INCLUDE_DRAWING_H
#define C_INCLUDE_DRAWING_H

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
 * @file drawing_canvas.h
 *
 * @brief Defines the canvas functions of the Drawing module.
 *
 * @since 8
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new Canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_Canvas
 * @since 8
 * @version 1.0
 */
OH_Drawing_Canvas* OH_Drawing_CanvasCreate(void);

/**
 * @brief Release the memory storing the OH_Drawing_Canvas object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDestroy(OH_Drawing_Canvas*);

/**
 * @brief Binds bitmap to make canvas draw into bitmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasBind(OH_Drawing_Canvas*, OH_Drawing_Bitmap*);

/**
 * @brief Sets OH_Drawing_Pen to OH_Drawing_Canvas, canvas will use OH_Drawing_Pen to draw shape stroke.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas*, const OH_Drawing_Pen*);

/**
 * @brief Removes OH_Drawing_Pen from OH_Drawing_Canvas, canvas will not draw shape stroke.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas*);

/**
 * @brief Sets OH_Drawing_Brush to OH_Drawing_Canvas, canvas will use OH_Drawing_Brush to fill the shape.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas*, const OH_Drawing_Brush*);

/**
 * @brief Removes OH_Drawing_Brush from OH_Drawing_Canvas, canvas will not fill the shape.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas*);

/**
 * @brief Saves OH_Drawing_Canvas current state that is put on a stack.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasSave(OH_Drawing_Canvas*);

/**
 * @brief Restores to OH_Drawing_Canvas last saved state.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasRestore(OH_Drawing_Canvas*);

/**
 * @brief Draws line from (x1, y1) to (x2, y2).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param x1 x-axis value of line start point
 * @param y1 y-axis value of line start point
 * @param x2 x-axis value of line end point
 * @param y2 y-axis value of line end point
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas*, float x1, float y1, float x2, float y2);

/**
 * @brief Draws path.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param OH_Drawing_Path a pointer to OH_Drawing_Path object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas*, const OH_Drawing_Path*);

/**
 * @brief Fills canvas with color.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas a pointer to OH_Drawing_Canvas object
 * @param color a 32-bit ARGB color quantity
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasClear(OH_Drawing_Canvas*, uint32_t color);

#ifdef __cplusplus
}
#endif
/** @} */
#endif