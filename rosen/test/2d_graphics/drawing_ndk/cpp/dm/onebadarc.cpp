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

#include "onebadarc.h"

#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>

enum {
    K_W = 100, // 100 是位图宽度
    K_H = 100, // 100 是位图高度
};

OneBadArc::OneBadArc()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "onebadarc";
}

union FloatIntUnion {
    float fFloat;
    int32_t fSignBitInt;
};

static inline float Bits2Float(int32_t floatAsBits)
{
    FloatIntUnion data;
    data.fSignBitInt = floatAsBits;
    return data.fFloat;
}

void OneBadArc::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 完全按照逻辑所画出的图形不一致
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, Bits2Float(0x41a00000), Bits2Float(0x41a00000));
    OH_Drawing_PathLineTo(path, Bits2Float(0x4208918c), Bits2Float(0x4208918c));
    OH_Drawing_PathConicTo(path, Bits2Float(0x41a00000), Bits2Float(0x42412318), Bits2Float(0x40bb73a0),
        Bits2Float(0x4208918c), Bits2Float(0x3f3504f3));
    OH_Drawing_PathQuadTo(
        path, Bits2Float(0x40bb73a0), Bits2Float(0x4208918c), Bits2Float(0x40bb73a2), Bits2Float(0x4208918c));
    OH_Drawing_PathLineTo(path, Bits2Float(0x41a00000), Bits2Float(0x41a00000));
    OH_Drawing_PathClose(path);

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 15.f); // 15.f 定义了画笔的宽度
    OH_Drawing_PenSetAlpha(pen, 100);  // 100 定义了画笔的透明度
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_CanvasTranslate(canvas, 20, 0); //  20, 0定义了画布平移量

    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(60, 0, 100, 40); // 60, 0, 100, 40  定义了一个矩形
    OH_Drawing_CanvasDrawArc(canvas, rect, 45, 90); // 45,90 定义了圆弧的起始角度和扫过角度

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path);
}
