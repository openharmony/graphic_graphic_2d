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
#include "blur_large_rrects.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <hilog/log.h>
#include "common/log_common.h"
#include "test_common.h"

BlurLargeRrects::BlurLargeRrects()
{
    bitmapWidth_ = 300;  // 300宽度
    bitmapHeight_ = 300; // 300高度
    fileName_ = "blur_large_rrects";
}

// 用例名: blurlargerrects 测试 OH_Drawing_CanvasDrawRoundRect
// 迁移基于源码 gm\blurroundrect.cpp->dm\blur_large_rrects.cpp
void BlurLargeRrects::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    DRAWING_LOGI("BlurLargeRrects::OnTestFunction start");
    // 创建一个brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 创建一个滤波器对象
    OH_Drawing_Filter *filter = OH_Drawing_FilterCreate();
    // 创建一个模板滤波器
    float sigma = 20.f;
    OH_Drawing_MaskFilter *maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, sigma, true);
    // 为滤波器设置模板
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    // 设置画笔滤波器
    OH_Drawing_BrushSetFilter(brush, filter);
    // 创建矩形、圆角矩形
    float left = 5.f;
    float top = -20000.f;
    float right = 240.f;
    float bottom = 25.f;
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(left, top, right, bottom);
    float xRad = 40.f;
    float yRad = 40.f;
    OH_Drawing_RoundRect *rrect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
    int maxCount = 4;
    for (int i = 0; i < maxCount; ++i) {
        // 设置画笔颜色
        // i = 0:蓝色。1的二进制是01,2的二进制是10.红色分量00与01相与的结果是00分量是0x00.绿色也是0x00.蓝色分量:(0 <
        // 2)为真,分量是0xFF.
        // i = 1:紫色。根据二进制红色01与01结果是01分量是0xFF绿色01与10结果00蓝色为真.故最终颜色是(0xFF, 0x00, 0xFF,
        // 0xFF),即一个完全不透明的紫色(红色+蓝色混合).
        // i = 2;绿色。计算与上面同理，红色00绿色10蓝色为假.
        // i = 3;黄色。3的二进制是11红色11与01相与结果01分量0xFF,绿色也是0xFF,蓝色是0x00,最终颜色是(0xFF, 0xFF, 0x00,
        // 0xFF),即一个完全不透明的黄色(红色和绿色).
        int bitwise1 = 1;
        int bitwise2 = 2;
        OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, (i & bitwise1) ? 0xFF : 0x00,
            (i & bitwise2) ? 0xFF : 0x00, (i < bitwise2) ? 0xFF : 0x00));
        DRAWING_LOGI("BlurLargeRrects::OnTestFunction The number of times i = %{public}d ,The value of i = %{public}u",
            i,
            OH_Drawing_ColorSetArgb(0xFF, (i & bitwise1) ? 0xFF : 0x00, (i & bitwise2) ? 0xFF : 0x00,
            (i < bitwise2) ? 0xFF : 0x00));
        // 上画布
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        // 画一个圆角矩形
        OH_Drawing_CanvasDrawRoundRect(canvas, rrect);
        // 设置画布旋转角度
        float degrees = 90.f;
        float px = 150.f;
        float py = 150.f;
        OH_Drawing_CanvasRotate(canvas, degrees, px, py);
    }

    // 释放内存
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    DRAWING_LOGI("BlurLargeRrects::OnTestFunction end");
}
