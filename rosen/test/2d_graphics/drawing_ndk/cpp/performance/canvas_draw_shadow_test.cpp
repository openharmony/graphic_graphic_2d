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

#include "canvas_draw_shadow_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_text_typography.h>

#include "performance.h"
#include "test_common.h"
/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1300
@Description:
    1、超大折线段光影绘制性能-OH_Drawing_CanvasDrawShadow
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造5000段折线组成的path，其他入参随机值
    5、DrawShadow 偏移到下一行（部分重叠），重复调用1000次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceCanvasDrawShadowLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCanvasDrawShadowLine OnTestPerformance OH_Drawing_CanvasDrawShadow");

    if (!canvas) {
        DRAWING_LOGE("PerformanceCanvasDrawShadowLine OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);

    OH_Drawing_Point3D planeParams;
    OH_Drawing_Point3D devLightPos;
    planeParams.x = 0;        // 0 for test
    planeParams.y = 0;        // 0 for test
    planeParams.z = 50.f;     // 50.f for test
    devLightPos.x = 100.f;    // 100.f for test
    devLightPos.y = 100.f;    // 100.f for test
    devLightPos.z = 100.f;    // 100.f for test
    float lightRadius = 50.f; // 50.f for test
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_ALL;
    TestRend rand = TestRend();
    for (int j = 1; j <= testCount_; j++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0, 2 * (j - 1));                           // 2 for test
        for (int i = 0; i <= 1000; i++) {                                      // 1000 for test
            int x = rand.nextUScalar1() * 2;                                   // 2 for test
            OH_Drawing_PathLineTo(path, i * 2 + x, (i % 2) * 2 + 2 * (j - 1)); // 2 for test
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1400
@Description:
    1、超大曲线段光影绘制性能-OH_Drawing_CanvasDrawShadow
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造5000段曲线组成的path，其他入参随机值
    5、DrawShadow 偏移到下一行（部分重叠），重复调用1000次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceCanvasDrawShadowCurve::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCanvasDrawShadowCurve OnTestPerformance OH_Drawing_CanvasDrawShadow");

    if (!canvas) {
        DRAWING_LOGE("PerformanceCanvasDrawShadowCurve OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);

    OH_Drawing_Point3D planeParams;
    OH_Drawing_Point3D devLightPos;
    planeParams.x = 0;        // 0 for test
    planeParams.y = 0;        // 0 for test
    planeParams.z = 50.f;     // 50.f for test
    devLightPos.x = 100.f;    // 100.f for test
    devLightPos.y = 100.f;    // 100.f for test
    devLightPos.z = 100.f;    // 100.f for test
    float lightRadius = 50.f; // 50.f for test
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_ALL;

    for (int j = 0; j < testCount_; j++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        for (int i = 0; i < 100; i++) {                                                 // 100 for test
            OH_Drawing_PathMoveTo(path, (i + 1) * 4, 1 + j * 2);                        // 2 4 for test
            OH_Drawing_PathArcTo(path, i * 4, j * 2, (i + 1) * 4, (j + 1) * 2, 0, 180); // 2 4 180 for test
            OH_Drawing_PathQuadTo(path, 2 + i * 4, j * 2, (i + 1) * 4, 1 + j * 2);      // 2 4 for test
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}