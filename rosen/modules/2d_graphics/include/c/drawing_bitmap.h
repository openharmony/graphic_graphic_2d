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

#ifndef C_INCLUDE_DRAWING_BITMAP_H
#define C_INCLUDE_DRAWING_BITMAP_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OH_Drawing_BitmapFormat used to describe the format of OH_Drawing_Bitmap.
 */
typedef struct {
    OH_Drawing_ColorFormat colorFormat;
    OH_Drawing_AlphaFormat alphaFormat;
} OH_Drawing_BitmapFormat;

/**
 * @brief Create a new bitmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return a pointer to created OH_Drawing_Bitmap
 */
OH_Drawing_Bitmap* OH_Drawing_BitmapCreate(void);

/**
 * @brief Release the memory storing the OH_Drawing_Bitmap object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 */
void OH_Drawing_BitmapDestroy(OH_Drawing_Bitmap*);

/**
 * @brief Initialize bitmap size and format.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 * @param width width of full OH_Drawing_Bitmap
 * @param height height of full OH_Drawing_Bitmap
 * @param OH_Drawing_BitmapFormat format consisting of OH_Drawing_ColorFormat and OH_Drawing_AlphaFormat
 */
void OH_Drawing_BitmapBuild(
    OH_Drawing_Bitmap*, const uint32_t width, const uint32_t height, const OH_Drawing_BitmapFormat*);

/**
 * @brief Returns OH_Drawing_Bitmap width.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 * @return OH_Drawing_Bitmap width
 */
uint32_t OH_Drawing_BitmapGetWidth(OH_Drawing_Bitmap*);

/**
 * @brief Returns OH_Drawing_Bitmap height.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 * @return OH_Drawing_Bitmap height
 */
uint32_t OH_Drawing_BitmapGetHeight(OH_Drawing_Bitmap*);

/**
 * @brief Returns pixel data address of OH_Drawing_Bitmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Bitmap a pointer to OH_Drawing_Bitmap object
 * @return a pointer to pixel data
 */
void* OH_Drawing_BitmapGetPixels(OH_Drawing_Bitmap*);

#ifdef __cplusplus
}
#endif

#endif