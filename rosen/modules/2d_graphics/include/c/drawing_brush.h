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

#ifndef C_INCLUDE_DRAWING_BRUSH_H
#define C_INCLUDE_DRAWING_BRUSH_H

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
 * @file drawing_brush.h
 *
 * @brief Defines the brush functions of the Drawing module.
 *
 * @since 8
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new Brush.
 *
 * @return a pointer to created OH_Drawing_Brush
 * @since 8
 * @version 1.0
 */
OH_Drawing_Brush* OH_Drawing_BrushCreate(void);

/**
 * @brief Release the memory storing the OH_Drawing_Brush object.
 *
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @since 8
 * @version 1.0
 */
void OH_Drawing_BrushDestroy(OH_Drawing_Brush*);

/**
 * @brief Returns true if pixels on the active edges of shapes are drawn with partial transparency.
 *
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @return antialiasing state
 * @since 8
 * @version 1.0
 */
bool OH_Drawing_BrushIsAntiAlias(const OH_Drawing_Brush*);

/**
 * @brief Sets antialiasing state, that edge pixels drawn opaque or with partial transparency.
 *
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @param bool antialiasing state
 * @since 8
 * @version 1.0
 */
void OH_Drawing_BrushSetAntiAlias(OH_Drawing_Brush*, bool);

/**
 * @brief Returns OH_Drawing_Brush fill color, that is a 32-bit ARGB quantity.
 *
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @return OH_Drawing_Brush fill color, a 32-bit ARGB quantity
 * @since 8
 * @version 1.0
 */
uint32_t OH_Drawing_BrushGetColor(const OH_Drawing_Brush*);

/**
 * @brief Sets OH_Drawing_Brush fill color, that is a 32-bit ARGB quantity.
 *
 * @param OH_Drawing_Brush a pointer to OH_Drawing_Brush object
 * @param color a 32-bit ARGB quantity
 * @since 8
 * @version 1.0
 */
void OH_Drawing_BrushSetColor(OH_Drawing_Brush*, uint32_t color);

#ifdef __cplusplus
}
#endif
/** @} */
#endif