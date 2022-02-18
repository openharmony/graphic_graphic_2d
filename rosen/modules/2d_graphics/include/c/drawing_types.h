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

#ifndef C_INCLUDE_DRAWING_TYPES_H
#define C_INCLUDE_DRAWING_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OH_Drawing_Canvas contains the current state of the rendering,
 * used to draw some destination such as shapes, bitmap.
 */
typedef struct OH_Drawing_Canvas OH_Drawing_Canvas;

/**
 * @brief OH_Drawing_Pen used to describe the style and color information about shape stroke.
 */
typedef struct OH_Drawing_Pen OH_Drawing_Pen;

/**
 * @brief OH_Drawing_Brush used to describe the style and color information about shape fill.
 */
typedef struct OH_Drawing_Brush OH_Drawing_Brush;

/**
 * @brief OH_Drawing_Path used to make custom shapes.
 */
typedef struct OH_Drawing_Path OH_Drawing_Path;

/**
 * @brief OH_Drawing_Bitmap used to describe image data.
 */
typedef struct OH_Drawing_Bitmap OH_Drawing_Bitmap;

/**
 * @brief OH_Drawing_ColorFormat used to describe the memory format of image data.
 */
typedef enum {
    COLOR_FORMAT_UNKNOWN,   /**< no such format exists or is supported. */
    COLOR_FORMAT_ALPHA_8,   /**< each pixel is a 8-bit quantity with 8-bit alpha. */
    COLOR_FORMAT_RGB_565,   /**< each pixel is a 16-bit quantity with 5-bit red, 6-bit green, 5-bit blue. */
    COLOR_FORMAT_ARGB_4444, /**< each pixel is a 16-bit quantity with 4-bit for alpha, red, green, blue. */
    COLOR_FORMAT_RGBA_8888, /**< each pixel is a 32-bit quantity with 8-bit for alpha, red, green, blue. */
    COLOR_FORMAT_BGRA_8888  /**< each pixel is a 32-bit quantity with 8-bit for blue, green, red, alpha. */
} OH_Drawing_ColorFormat;

/**
 * @brief OH_Drawing_AlphaFormat used to describe the alpha format of each pixel.
 */
typedef enum {
    ALPHA_FORMAT_UNKNOWN, /**< no such format exists or is supported. */
    ALPHA_FORMAT_OPAQUE,  /**< each pixel is opaque. */
    ALPHA_FORMAT_PREMUL,  /**< pixel components are premultiplied by alpha */
    ALPHA_FORMAT_UNPREMUL /**< pixel components are independent of alpha */
} OH_Drawing_AlphaFormat;

#ifdef __cplusplus
}
#endif

#endif