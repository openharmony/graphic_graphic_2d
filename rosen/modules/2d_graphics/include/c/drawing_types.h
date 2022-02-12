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

typedef uint32_t OH_Drawing_Color;

#define OH_Drawing_ColorSetArgb(a, r, g, b) \
    ((((a)&0xFF) << 24) | (((r)&0xFF) << 16) | (((g)&0xFF) << 8) | (((b)&0xFF) << 0))
#define OH_Drawing_ColorGetA(c) (((c) >> 24) & 0xFF)
#define OH_Drawing_ColorGetR(c) (((c) >> 16) & 0xFF)
#define OH_Drawing_ColorGetG(c) (((c) >> 8) & 0xFF)
#define OH_Drawing_ColorGetB(c) (((c) >> 0) & 0xFF)

typedef struct OH_Drawing_Canvas OH_Drawing_Canvas;

typedef struct OH_Drawing_Pen OH_Drawing_Pen;

typedef struct OH_Drawing_Brush OH_Drawing_Brush;

typedef struct OH_Drawing_Path OH_Drawing_Path;

typedef struct OH_Drawing_Bitmap OH_Drawing_Bitmap;

typedef enum {
    COLOR_FORMAT_UNKNOWN,
    COLOR_FORMAT_ALPHA_8,
    COLOR_FORMAT_RGB_565,
    COLOR_FORMAT_ARGB_4444,
    COLOR_FORMAT_RGBA_8888,
    COLOR_FORMAT_BGRA_8888
} OH_Drawing_ColorFormat;

typedef enum {
    ALPHA_FORMAT_UNKNOWN,
    ALPHA_FORMAT_OPAQUYE,
    ALPHA_FORMAT_PREMUL,
    ALPHA_FORMAT_UNPREMUL
} OH_Drawing_AlphaFormat;

#ifdef __cplusplus
}
#endif

#endif