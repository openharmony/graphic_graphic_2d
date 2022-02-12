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

typedef struct {
    OH_Drawing_ColorFormat colorFormat;
    OH_Drawing_AlphaFormat alphaFormat;
} OH_Drawing_BitmapFormat;

OH_Drawing_Bitmap* OH_Drawing_BitmapCreate(void);

void OH_Drawing_BitmapDestory(OH_Drawing_Bitmap*);

void OH_Drawing_BitmapBuild(
    OH_Drawing_Bitmap*, const uint32_t width, const uint32_t height, const OH_Drawing_BitmapFormat*);

uint32_t OH_Drawing_BitmapGetWidth(OH_Drawing_Bitmap*);

uint32_t OH_Drawing_BitmapGetHeight(OH_Drawing_Bitmap*);

void* OH_Drawing_BitmapGetPixels(OH_Drawing_Bitmap*);

#ifdef __cplusplus
}
#endif

#endif