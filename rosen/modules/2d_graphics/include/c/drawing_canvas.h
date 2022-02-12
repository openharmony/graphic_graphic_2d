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

#ifndef C_INCLUDE_DRAWING_H
#define C_INCLUDE_DRAWING_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OH_Drawing_Canvas* OH_Drawing_CanvasCreate(void);

void OH_Drawing_CanvasDestory(OH_Drawing_Canvas*);

void OH_Drawing_CanvasBind(OH_Drawing_Canvas*, OH_Drawing_Bitmap*);

void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas*, const OH_Drawing_Pen*);

void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas*);

void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas*, const OH_Drawing_Brush*);

void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas*);

void OH_Drawing_CanvasSave(OH_Drawing_Canvas*);

void OH_Drawing_CanvasRestore(OH_Drawing_Canvas*);

void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas*, float x1, float y1, float x2, float y2);

void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas*, const OH_Drawing_Path*);

void OH_Drawing_CanvasClear(OH_Drawing_Canvas*, OH_Drawing_Color);

#ifdef __cplusplus
}
#endif

#endif