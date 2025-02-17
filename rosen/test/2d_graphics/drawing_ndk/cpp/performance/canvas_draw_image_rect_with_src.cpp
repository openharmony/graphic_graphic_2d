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

#include "canvas_draw_image_rect_with_src.h"

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

#include "test_common.h"
// 绘制一个图形
OH_Drawing_Image* precondItions()
{
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
        OH_Drawing_AlphaFormat::ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Canvas* bitcanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitcanvas, bm);
    OH_Drawing_CanvasClear(bitcanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(0.f, 0.f);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(64.f, 64.f); // 64.f for test
    const uint32_t Color[] = {
        0xFFFFFFFF,
        0xFF0000FF,
    };
    OH_Drawing_ShaderEffect* shaderEffect =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(bitcanvas, brush);
    OH_Drawing_Point* center = OH_Drawing_PointCreate(32.f, 32.f); // 32.f for test
    float radius = 32;
    OH_Drawing_CanvasDrawCircle(bitcanvas, center, radius);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_CanvasDetachBrush(bitcanvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(bitcanvas);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_PointDestroy(center);
    return image;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1500
@Description:
    1、超大源绘制性能-OH_Drawing_CanvasDrawImageRectWithSrc
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、OH_Drawing_CanvasDrawImageRectWithSrc，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线；
 */
void PerformanceCanvasDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    if (!canvas) {
        DRAWING_LOGE("PerformanceCanvasDrawImageRectWithSrc OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);

    OH_Drawing_SamplingOptions* option = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    auto image = precondItions();
    OH_Drawing_Rect* arrayRect[4];                                   // 4 for test
    arrayRect[0] = OH_Drawing_RectCreate(0, 0, 32, 32);              // 32 for test
    arrayRect[1] = OH_Drawing_RectCreate(0, 0, 80, 80);              // 1 80 for test
    arrayRect[2] = OH_Drawing_RectCreate(32, 32, 96, 96);            // 2 32 96 for test
    arrayRect[3] = OH_Drawing_RectCreate(-32, -32, 32, 32);          // -32 3 32 for test
    OH_Drawing_Rect* dstR = OH_Drawing_RectCreate(0, 200, 128, 380); // 128 200 380 for test
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(0, 0, 64, 64);      // 64 for test
    OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
    for (size_t i = 0; i < testCount_; ++i) {
        if ((i + 1) % 100 == 0) {                         // 100 for test
            OH_Drawing_CanvasTranslate(canvas, -500, 30); // -500 30 for test
        }
        OH_Drawing_Rect* srcR = arrayRect[i % 4]; // 4 for test
        OH_Drawing_CanvasDrawImageRectWithSrc(
            canvas, image, srcR, dstR, option, OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        OH_Drawing_CanvasTranslate(canvas, 5, 0); // 5 for test
    }
    OH_Drawing_RectDestroy(arrayRect[0]); // 0 for test
    OH_Drawing_RectDestroy(arrayRect[1]); // 1 for test
    OH_Drawing_RectDestroy(arrayRect[2]); // 2 for test
    OH_Drawing_RectDestroy(arrayRect[3]); // 3 for test
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_RectDestroy(dstR);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}