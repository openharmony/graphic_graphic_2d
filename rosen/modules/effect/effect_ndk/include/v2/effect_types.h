/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_EFFECT_TYPES_V2_H
#define C_INCLUDE_EFFECT_TYPES_V2_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OH_Graphic2D_EffectKit_Filter OH_Graphic2D_EffectKit_Filter;

typedef struct OH_PixelmapNative OH_PixelmapNative;

typedef struct OH_Graphic2D_EffectKit_ColorMatrix {
    float val[20];
} OH_Graphic2D_EffectKit_ColorMatrix;

typedef enum {
    OH_GRAPHIC2D_EFFECTKIT_SUCCESS = 0,
    OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER = 401,
    OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR = 7600200,
    OH_GRAPHIC2D_EFFECTKIT_MEMORY_ERROR = 7600201,
} OH_Graphic2D_EffectKit_ErrorCode;

typedef enum {
    OH_GRAPHIC2D_EFFECTKIT_CLAMP = 0,
    OH_GRAPHIC2D_EFFECTKIT_REPEAT,
    OH_GRAPHIC2D_EFFECTKIT_MIRROR,
    OH_GRAPHIC2D_EFFECTKIT_DECAL,
} OH_Graphic2D_EffectKit_TileMode;

#ifdef __cplusplus
}
#endif

#endif
