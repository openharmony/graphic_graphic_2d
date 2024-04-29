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
#include "arcofzorro.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"

ArcOfZorro::ArcOfZorro()
{
    // file gm/arcofzorro.cpp
    bitmapWidth_ = 1000;  // 1000宽度
    bitmapHeight_ = 1000; // 1000高度
    fileName_ = "arcofzorro";
}

void ArcOfZorro::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 用例名: arcofzorro 测试 OH_Drawing_CanvasDrawArc
    // 迁移基于源码arcofzorro->arcofzorro
    DRAWING_LOGI("ArcOfZorroGM::OnTestFunction start");
    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
    // 创建一个矩形对象
    TestRend rand;
    float x = 10;
    float y = 10;
    float w = 200;
    float h = 200;
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(x, y, x + w, y + h);
    // 创建画笔pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 35); // 35宽度

    int xOffset = 0;
    int yOffset = 0;
    int direction = 0;
    for (float arc = 134.0f; arc < 136.0f; arc += 0.01f) {
        DRAWING_LOGI("ArcOfZorroGM::arc");
        uint32_t color = rand.nextU();
        color |= 0xff000000;
        OH_Drawing_PenSetColor(pen, color);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, xOffset, yOffset);
        OH_Drawing_CanvasDrawArc(canvas, rect, 0, arc);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasRestore(canvas);

        switch (direction) {
            case 0:
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction0");
                xOffset += 10;        // 10 cout
                if (xOffset >= 700) { // 700 max
                    direction = 1;    // direction 1
                }
                break;
            case 1: // 1 case
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction1");
                xOffset -= 10;      // 10 cout
                yOffset += 10;      // 10 cout
                if (xOffset < 50) { // 50 max
                    direction = 2;  // direction 2
                }
                break;
            case 2: // 2 case
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction2");
                xOffset += 10; // 10 cout
                break;
            default:
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction3");
                break;
        }
    }
    DRAWING_LOGI("ArcOfZorroGM::OnTestFunction end");
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}
