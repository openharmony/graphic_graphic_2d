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

#ifndef C_INCLUDE_DRAWING_PATH_H
#define C_INCLUDE_DRAWING_PATH_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { PATH_CW_DIRECTION, PATH_CCW_DIRECTION } OH_Drawing_PathDirection;

OH_Drawing_Path* OH_Drawing_PathCreate(void);

void OH_Drawing_PathDestory(OH_Drawing_Path*);

void OH_Drawing_PathMoveTo(OH_Drawing_Path*, float x, float y);

void OH_Drawing_PathLineTo(OH_Drawing_Path*, float x, float y);

void OH_Drawing_PathArcTo(OH_Drawing_Path*, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg);

void OH_Drawing_PathQuadTo(OH_Drawing_Path*, float ctrlX, float ctrlY, float endX, float endY);

void OH_Drawing_PathCubicTo(
    OH_Drawing_Path*, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY);

void OH_Drawing_PathClose(OH_Drawing_Path*);

void OH_Drawing_PathReset(OH_Drawing_Path*);

#ifdef __cplusplus
}
#endif

#endif