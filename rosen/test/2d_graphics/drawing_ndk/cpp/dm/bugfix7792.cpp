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
#include "bugfix7792.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

BugFix7792::BugFix7792()
{
    bitmapWidth_ = 800;    // 800宽度
    bitmapHeight_ = 800;   // 800高度
    fileName_ = "bug7792"; // 对标 pathfill.cpp
}

void BugFix7792::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 用例名: bug7792 测试 OH_Drawing_PathReset
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // 创建一个brush对象
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    CanvasDrawPath1(canvas, path);
    CanvasDrawPath2(canvas, path);
    CanvasDrawPath3(canvas, path);
    CanvasDrawPath4(canvas, path);
    CanvasDrawPath5(canvas, path);
    // 释放内存
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}

void BugFix7792::CanvasDrawPath1(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    // 第一个图型坐标点:（0.0）
    OH_Drawing_PathMoveTo(path, 10, 10);   // path, 10, 10坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // path, 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // path, 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // path, 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第二个图型坐标点:（0.1）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 50);   // 75, 50坐标
    OH_Drawing_PathMoveTo(path, 100, 75);  // 100, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 50);   // 75, 50坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第三个图型坐标点:（0.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 10, 10);   // 10, 10坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第四个图型坐标点:（0.4）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200); // -200 * 2, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathMoveTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
}

void BugFix7792::CanvasDrawPath2(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    // 第五个图型坐标点:（1.1）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 250, 75);  // 250, 75坐标
    OH_Drawing_PathMoveTo(path, 250, 75);  // 250, 75坐标
    OH_Drawing_PathMoveTo(path, 250, 75);  // 250, 75坐标
    OH_Drawing_PathMoveTo(path, 100, 75);  // 100, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_PathLineTo(path, 0, 0);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第六个图型坐标点:（1.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathMoveTo(path, 250, 75);  // 250, 75坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第七个图型坐标点:（1.3）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200); // -200 * 2, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 10);   // 75, 10坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 10);   // 75, 10坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
}

void BugFix7792::CanvasDrawPath3(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    // 第八个图型坐标点:（1.4）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_PathMoveTo(path, 10, 10); // 10, 10坐标
    OH_Drawing_PathLineTo(path, 30, 10); // 30, 10坐标
    OH_Drawing_PathLineTo(path, 10, 30); // 10, 30坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第九个图型坐标点:（2.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十个图型坐标点:（2.3）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200); // -200 * 2, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 250);  // 75, 250坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
}

void BugFix7792::CanvasDrawPath4(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    // 第十一个图型坐标点:（2.4）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 10);   // 75, 10坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十二个图型坐标点:（2.5）
    OH_Drawing_CanvasTranslate(canvas, 200, 0); // 200, 0坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 10, 150);  // 10, 150坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十三个图型坐标点:（3.3）
    OH_Drawing_CanvasTranslate(canvas, 200, -200 * 3); //  200, -200 * 3坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 100);  // 75, 100坐标
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十四个图型坐标点:（3.4）
    OH_Drawing_CanvasTranslate(canvas, 0, 200); // 0, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 150, 100); // 150, 100坐标
    OH_Drawing_PathLineTo(path, 150, 100); // 150, 100坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 75, 150);  // 75, 150坐标
    OH_Drawing_PathLineTo(path, 75, 100);  // 75, 100坐标
    OH_Drawing_PathLineTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
}

void BugFix7792::CanvasDrawPath5(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    // 第十五个图型坐标点:（3.5）
    OH_Drawing_CanvasTranslate(canvas, 0, 200); // 0, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 140, 150); // 140, 150坐标
    OH_Drawing_PathLineTo(path, 140, 75);  // 140, 75坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十六个图型坐标点:（3.6）
    OH_Drawing_CanvasTranslate(canvas, 0, 200); // 0, 200坐标
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathLineTo(path, 150, 75);  // 150, 75坐标
    OH_Drawing_PathLineTo(path, 150, 150); // 150, 150坐标
    OH_Drawing_PathLineTo(path, 140, 150); // 140, 150坐标
    OH_Drawing_PathLineTo(path, 140, 75);  // 140, 75坐标
    OH_Drawing_PathMoveTo(path, 75, 75);   // 75, 75坐标
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
}