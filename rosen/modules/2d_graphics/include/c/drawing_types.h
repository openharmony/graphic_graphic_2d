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

#ifndef C_INCLUDE_DRAWING_TYPES_H
#define C_INCLUDE_DRAWING_TYPES_H

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
 * @file drawing_types.h
 *
 * @brief Declares the data types for drawing 2D graphics, including the canvas, brush, pen, bitmap, and path.
 *
 * @since 8
 * @version 1.0
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a rectangular canvas on which various shapes, images,
 * and texts can be drawn by using the brush and pen.
 * 
 * @since 8
 * @version 1.0
 */
typedef struct OH_Drawing_Canvas OH_Drawing_Canvas;

/**
 * @brief Defines a pen, which is used to describe the style and color to outline a shape.
 * 
 * @since 8
 * @version 1.0
 */
typedef struct OH_Drawing_Pen OH_Drawing_Pen;

/**
 * @brief Defines as a brush, which is used to describe the style and color to fill in a shape.
 * 
 * @since 8
 * @version 1.0
 */
typedef struct OH_Drawing_Brush OH_Drawing_Brush;

/**
 * @brief Defines a path, which is used to customize various shapes.
 * 
 * @since 8
 * @version 1.0
 */
typedef struct OH_Drawing_Path OH_Drawing_Path;

/**
 * @brief Defines a bitmap, which is a memory that contains the pixel data of a shape.
 * 
 * @since 8
 * @version 1.0
 */
typedef struct OH_Drawing_Bitmap OH_Drawing_Bitmap;

/**
 * @brief Enumerates storage formats of bitmap pixels.
 * 
 * @since 8
 * @version 1.0
 */
typedef enum {
    /** Unknown format. */
    COLOR_FORMAT_UNKNOWN,
    /** Each pixel is represented by 8 bits, which together indicate alpha. */
    COLOR_FORMAT_ALPHA_8,
    /**
     * Each pixel is represented by 16 bits. From the most significant bit to the least significant bit,
     * the first 5 bits indicate red, the subsequent 6 bits indicate green, and the last 5 bits indicate blue.
     */
    COLOR_FORMAT_RGB_565,
    /**
     * Each pixel is represented by 16 bits. From the most significant bit to the least significant bit,
     * every 4 bits indicate alpha, red, green, and blue, respectively.
     */
    COLOR_FORMAT_ARGB_4444,
    /**
     * Each pixel is represented by 32 bits. From the most significant bit to the least significant bit,
     * every 8 bits indicate alpha, red, green, and blue, respectively.
     */
    COLOR_FORMAT_RGBA_8888,
    /**
     * Each pixel is represented by 32 bits. From the most significant bit to the least significant bit,
     * every 8 bits indicate blue, green, red, and alpha, respectively.
     */
    COLOR_FORMAT_BGRA_8888
} OH_Drawing_ColorFormat;

/**
 * @brief Enumerates alpha formats of bitmap pixels.
 * 
 * @since 8
 * @version 1.0
 */
typedef enum {
    /** Unknown format. */
    ALPHA_FORMAT_UNKNOWN,
    /** The bitmap does not have the alpha component. */
    ALPHA_FORMAT_OPAQUE,
    /** The color component of each pixel is premultiplied by the alpha component. */
    ALPHA_FORMAT_PREMUL,
    /** The color component of each pixel is not premultiplied by the alpha component. */
    ALPHA_FORMAT_UNPREMUL
} OH_Drawing_AlphaFormat;

#ifdef __cplusplus
}
#endif
/** @} */
#endif
