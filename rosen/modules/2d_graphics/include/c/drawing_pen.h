/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_PEN_H
#define C_INCLUDE_DRAWING_PEN_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new Pen.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_Pen
 */
OH_Drawing_Pen* OH_Drawing_PenCreate(void);

/**
 * @brief Release the memory storing the OH_Drawing_Pen object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 */
void OH_Drawing_PenDestroy(OH_Drawing_Pen*);

/**
 * @brief Returns true if pixels on the active edges of shapes are drawn with partial transparency.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return antialiasing state
 */
bool OH_Drawing_PenIsAntiAlias(const OH_Drawing_Pen*);

/**
 * @brief Sets antialiasing state, that edge pixels drawn opaque or with partial transparency.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param bool antialiasing state
 */
void OH_Drawing_PenSetAntiAlias(OH_Drawing_Pen*, bool);

/**
 * @brief Returns OH_Drawing_Pen stroke color, that is a 32-bit ARGB quantity.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return OH_Drawing_Pen stroke color, a 32-bit ARGB quantity
 */
uint32_t OH_Drawing_PenGetColor(const OH_Drawing_Pen*);

/**
 * @brief Sets OH_Drawing_Pen stroke color, that is a 32-bit ARGB quantity.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param color a 32-bit ARGB quantity
 */
void OH_Drawing_PenSetColor(OH_Drawing_Pen*, uint32_t color);

/**
 * @brief Returns OH_Drawing_Pen stroke width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return stroke width
 */
float OH_Drawing_PenGetWidth(const OH_Drawing_Pen*);

/**
 * @brief Sets the thickness of OH_Drawing_Pen stroke.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param width stroke width
 */
void OH_Drawing_PenSetWidth(OH_Drawing_Pen*, float width);

/**
 * @brief Returns the limit at which a sharp corner is drawn beveled.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return miter limit
 */
float OH_Drawing_PenGetMiterLimit(const OH_Drawing_Pen*);

/**
 * @brief Sets the limit at which a sharp corner is drawn beveled.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param miter miter limit
 */
void OH_Drawing_PenSetMiterLimit(OH_Drawing_Pen*, float miter);

/**
 * @brief OH_Drawing_PenLineCapStyle used to describe the beginning and end of line.
 */
typedef enum {
    LINE_FLAT_CAP,   /**< no cap. */
    LINE_SQUARE_CAP, /**< adds square cap. */
    LINE_ROUND_CAP   /**< adds round cap. */
} OH_Drawing_PenLineCapStyle;

/**
 * @brief Returns the geometry drawn at the beginning and end of line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return line cap
 */
OH_Drawing_PenLineCapStyle OH_Drawing_PenGetCap(const OH_Drawing_Pen*);

/**
 * @brief Sets the geometry drawn at the beginning and end of line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param OH_Drawing_PenLineCapStyle line cap
 */
void OH_Drawing_PenSetCap(OH_Drawing_Pen*, OH_Drawing_PenLineCapStyle);

/**
 * @brief OH_Drawing_PenLineJoinStyle used to describe the corners of line.
 */
typedef enum {
    LINE_MITER_JOIN, /**< extends miter limit. */
    LINE_ROUND_JOIN, /**< adds round. */
    LINE_BEVEL_JOIN  /**< connects outside edges. */
} OH_Drawing_PenLineJoinStyle;

/**
 * @brief Returns the geometry drawn at the corners of line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @return line join
 */
OH_Drawing_PenLineJoinStyle OH_Drawing_PenGetJoin(const OH_Drawing_Pen*);

/**
 * @brief Sets the geometry drawn at the corners of line.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pen a pointer to OH_Drawing_Pen object
 * @param OH_Drawing_PenLineJoinStyle line join
 */
void OH_Drawing_PenSetJoin(OH_Drawing_Pen*, OH_Drawing_PenLineJoinStyle);

#ifdef __cplusplus
}
#endif

#endif