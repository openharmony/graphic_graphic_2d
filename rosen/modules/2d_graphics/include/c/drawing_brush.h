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

#ifndef C_INCLUDE_DRAWING_BRUSH_H
#define C_INCLUDE_DRAWING_BRUSH_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OH_Drawing_Brush* OH_Drawing_BrushCreate(void);

void OH_Drawing_BrushDestory(OH_Drawing_Brush*);

bool OH_Drawing_BrushIsAntiAlias(const OH_Drawing_Brush*);

void OH_Drawing_BrushSetAntiAlias(OH_Drawing_Brush*, bool);

OH_Drawing_Color OH_Drawing_BrushGetColor(const OH_Drawing_Brush*);

void OH_Drawing_BrushSetColor(OH_Drawing_Brush*, OH_Drawing_Color);

#ifdef __cplusplus
}
#endif

#endif