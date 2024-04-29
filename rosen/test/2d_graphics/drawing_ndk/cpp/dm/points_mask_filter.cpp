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

#include "points_mask_filter.h"

#include <bits/alltypes.h>
#include <hilog/log.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 512,
    K_H = 256,
    N = 30,
};

PointsMaskFilter::PointsMaskFilter()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "point_smaskfilter";
}

// 用例名: pointsmaskfilter 测试 OH_Drawing_MaskFilterCreateBlur
void PointsMaskFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    float fRange = 220.0f;   // 点的x坐标范围
    float fOffset = 18.0f;   // 点的x坐标偏移量
    float fRadius = 6.0f;    // 模糊半径
    float fOffsetX = 256.0f; // 画布平移的x坐标量
    float fWidth = 10.0f;
    OH_Drawing_Point2D pts[N];
    TestRend testRend;
    for (OH_Drawing_Point2D& p : pts) {
        p.x = testRend.nextF() * fRange + fOffset;
        p.y = testRend.nextF() * fRange + fOffset;
    }

    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, fRadius, true);

    const OH_Drawing_PenLineCapStyle caps[] = { OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP,
        OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP };
    // 创建一个pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_PenSetWidth(pen, fWidth);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    for (auto cap : caps) {
        OH_Drawing_PenSetCap(pen, cap);
        OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));

        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, N, pts);
        OH_Drawing_FilterSetMaskFilter(filter, nullptr);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));

        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, N, pts);
        OH_Drawing_CanvasTranslate(canvas, fOffsetX, 0);
    }
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_CanvasDestroy(canvas);

    DRAWING_LOGI("pointsmaskfilter::OnTestFunction end");
}
