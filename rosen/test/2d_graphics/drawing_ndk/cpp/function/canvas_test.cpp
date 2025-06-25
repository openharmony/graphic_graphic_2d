/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "canvas_test.h"

#include <cstdint>
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
#include <native_drawing/drawing_pixel_map.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3601
@Description:
    1、OH_Drawing_CanvasDrawLine接口绘制在画布外
    2、OH_Drawing_CanvasDrawLine接口绘制在一半画布内一半画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawLine，点击draw按钮
@Result:
    1、页面无绘制效果
    2、页面绘制一条（0，0，200，200）的直线
 */
void FunctionCanvasDrawLine::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawLine OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_CanvasDrawLine接口绘制在画布外
    OH_Drawing_CanvasDrawLine(canvas, -100, -100, 0, 0); // -100, -100, 0, 0 for test
    // OH_Drawing_CanvasDrawLine接口绘制在一半画布内一半画布外
    OH_Drawing_CanvasDrawLine(canvas, -200, -200, 200, 200); // -200, -200, 200, 200 for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3801
@Description:
    1、OH_Drawing_CanvasDrawRegion绘制在画布外
    2、OH_Drawing_CanvasDrawRegion一半绘制在画布内一半绘制在画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawRegion，点击draw按钮
@Result:
    1、页面无绘制效果
    2、页面绘制一半蓝色矩形
 */
void FunctionCanvasDrawRegion::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawRegion OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    // OH_Drawing_CanvasDrawRegion绘制在画布外
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(-200, -200, 0, 0); // -200, -200, 0, 0 for test
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_CanvasDrawRegion(canvas, region);
    // OH_Drawing_CanvasDrawRegion一半绘制在画布内一半绘制在画布外
    rect = OH_Drawing_RectCreate(-200, -200, 200, 200); // -200, -200, 200, 200 for test
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_CanvasDrawRegion(canvas, region);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_BrushDestroy(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_3901
@Description:OH_Drawing_CanvasDrawPoint点绘制在画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawPoint，点击draw按钮
@Result:
    1、页面无绘制
 */
void FunctionCanvasDrawPoint::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawPoint OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Point2D pts;
    pts.x = -50; // -50 for test
    pts.y = -50; // -50 for test
    OH_Drawing_CanvasDrawPoint(canvas, &pts);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4001
@Description:
    1、OH_Drawing_CanvasDrawPoints点绘制在画布外
    2、OH_Drawing_CanvasDrawPoints点个数一半绘制在画布内一半绘制在画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawPoints，点击draw按钮
@Result:
    1、页面无绘制效果
    2、页面绘制一半蓝色矩形
 */
void FunctionCanvasDrawPoints::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawPoints OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Point2D pts[2]; // 2 for test
    // OH_Drawing_CanvasDrawPoints点绘制在画布外
    pts[0].x = -50;                                                                       // -50 for test
    pts[0].y = -50;                                                                       // -50 for test
    pts[1].x = -100;                                                                      // -100 for test
    pts[1].y = -100;                                                                      // -100 for test
    OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, 2, pts); // 2 for test
    // OH_Drawing_CanvasDrawPoints点个数一半绘制在画布内一半绘制在画布外
    pts[0].x = -50;                                                                       // -50 for test
    pts[0].y = -50;                                                                       // -50 for test
    pts[1].x = 100;                                                                       // 100 for test
    pts[1].y = 100;                                                                       // 100 for test
    OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, 2, pts); // 2 for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4101
@Description:
    1、OH_Drawing_CanvasDrawOval绘制在画布外
    2、OH_Drawing_CanvasDrawOval一半绘制在画布内一半绘制在画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawOval，点击draw按钮
@Result:
    1、页面无绘制效果
    2、页面绘制半个椭圆
 */
void FunctionCanvasDrawOval::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawOval OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // OH_Drawing_CanvasDrawOval绘制在画布外
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(-100, -100, -300, -300); // -100, -100, -300, -300 for test
    OH_Drawing_PathAddOval(path, rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    // OH_Drawing_CanvasDrawOval一半绘制在画布内一半绘制在画布外
    rect = OH_Drawing_RectCreate(-200, 200, 200, 400); //-200, 200, 200, 400 for test
    OH_Drawing_PathAddOval(path, rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4201
@Description:OH_Drawing_CanvasClipRoundRect接口clipop传入INTERSECT
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasClipRoundRect，点击draw按钮
@Result:
    1、画布上绘制出一个红色空心的圆角矩形
 */
void FunctionCanvasClipRoundRect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasClipRoundRect OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35);                                            // 35 for test
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);              // 100, 100 for test
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20); // 20, 20 for test
    OH_Drawing_CanvasClipRoundRect(canvas, roundRect, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4202
@Description:OH_Drawing_CanvasClipRoundRect接口doAntialias遍历验证
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasClipRoundRect2，点击draw按钮
@Result:
    1、页面绘制两个红色的圆角矩形，一个做抗锯齿处理，一个没有做抗锯齿处理
 */
void FunctionCanvasClipRoundRect2::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasClipRoundRect2 OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200);          // 100, 100, 200, 200 for test
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20); // 20, 20 for test
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(210, 210, 300, 300);           // 210, 210, 300, 300 for test
    OH_Drawing_RoundRect* roundRect1 = OH_Drawing_RoundRectCreate(rect1, 40, 40); // 40, 40 for test
    OH_Drawing_CanvasClipRoundRect(canvas, roundRect1, OH_Drawing_CanvasClipOp::INTERSECT, false);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect1);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RoundRectDestroy(roundRect1);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4301
@Description:
    1、OH_Drawing_CanvasSkew参数sx设置为负数
    2、OH_Drawing_CanvasSkew参数sy设置为正数
    3、OH_Drawing_CanvasSkew参数sy设置为负数
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasSkew，点击draw按钮
@Result:
    1、页面绘制一个向左倾斜的平行四边形
    2、页面绘制一个向下倾斜的平行四边形
    3、页面绘制一个向上倾斜的平行四边形
 */
void FunctionCanvasSkew::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasSkew OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_CanvasSkew参数sx设置为负数
    OH_Drawing_CanvasSkew(canvas, -0.5, 0);                        // -0.5, 0 for test
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 100, 100 for test
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    // OH_Drawing_CanvasSkew参数sy设置为正数
    OH_Drawing_CanvasSkew(canvas, 0.5, 0.5);        // 0.5 for test
    rect = OH_Drawing_RectCreate(0, 200, 100, 300); // 0, 200, 100, 300 for test
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    // OH_Drawing_CanvasSkew参数sy设置为负数
    OH_Drawing_CanvasSkew(canvas, 0, -1);           // 0, -1 for test
    rect = OH_Drawing_RectCreate(0, 400, 100, 500); // 0, 400, 100, 500 for test
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4501
@Description:
    1、OH_Drawing_CanvasDrawImageRectWithSrc参数src设置在画布外
    2、OH_Drawing_CanvasDrawImageRectWithSrc参数src设置为一半画布内一半画布外
    3、OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置在画布外
    4、OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置为一半画布内一半画布外
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawImageRectWithSrc，点击draw按钮
@Result:
    1、src无绘制，dst有绘制
    2、src绘制一半，dst绘制完整
    3、src有绘制，dst无绘制
    4、src绘制完整，dst绘制一半
 */
void FunctionCanvasDrawImageRectWithSrc::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawImageRectWithSrc OnTestFunction");
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap1_);
    // OH_Drawing_CanvasDrawImageRectWithSrc参数src设置在画布外
    OH_Drawing_Rect* srcR = OH_Drawing_RectCreate(0, 0, -200, -200); // 0, 0, -200, -200 for test
    OH_Drawing_SamplingOptions* option = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NONE);
    OH_Drawing_Rect* dstR = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 for test
    OH_Drawing_CanvasDrawImageRectWithSrc(
        canvas, image, srcR, dstR, option, OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    OH_Drawing_RectDestroy(srcR);
    OH_Drawing_RectDestroy(dstR);
    // OH_Drawing_CanvasDrawImageRectWithSrc参数src设置为一半画布内一半画布外
    srcR = OH_Drawing_RectCreate(-200, -200, 200, 200); // -200, -200, 200, 200 for test
    dstR = OH_Drawing_RectCreate(250, 250, 350, 350);   // 250, 250, 350, 350 for test
    OH_Drawing_CanvasDrawImageRectWithSrc(
        canvas, image, srcR, dstR, option, OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    OH_Drawing_RectDestroy(srcR);
    OH_Drawing_RectDestroy(dstR);
    // OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置在画布外
    srcR = OH_Drawing_RectCreate(0, 0, 200, 200);         // 0, 0, 200, 200 for test
    dstR = OH_Drawing_RectCreate(-100, -100, -200, -200); // -100, -100, -200, -200 for test
    OH_Drawing_CanvasDrawImageRectWithSrc(
        canvas, image, srcR, dstR, option, OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    OH_Drawing_RectDestroy(srcR);
    OH_Drawing_RectDestroy(dstR);
    // OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置为一半画布内一半画布外
    srcR = OH_Drawing_RectCreate(250, 250, 350, 350);   // 250, 250, 350, 350 for test
    dstR = OH_Drawing_RectCreate(-200, -200, 200, 200); // -200, -200, 200, 200 for test
    OH_Drawing_CanvasDrawImageRectWithSrc(
        canvas, image, srcR, dstR, option, OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);

    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_RectDestroy(srcR);
    OH_Drawing_RectDestroy(dstR);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4701
@Description:
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_OVER
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_OVER
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_IN
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_IN
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_OUT
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_OUT
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_ATOP
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_ATOP
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_XOR
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_PLUS
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_MODULATE
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SCREEN
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_OVERLAY
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DARKEN
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_LIGHTEN
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR_DODGE
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR_BURN
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_HARD_LIGHT
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SOFT_LIGHT
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DIFFERENCE
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_EXCLUSION
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_MULTIPLY
    1、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_HUE
    2、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SATURATION
    3、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR
    4、OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_LUMINOSITY
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices2，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices3，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices4，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices5，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices6，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices7，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices8，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices9，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices10，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices11，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices12，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices13，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices14，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices15，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices16，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices17，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices18，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices19，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices20，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices21，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices22，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices23，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices24，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices25，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices26，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices27，点击draw按钮
1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasDrawVertices28，点击draw按钮
@Result:
    1、根据实际调用枚举反馈
 */
void FunctionCanvasDrawVertices::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasDrawVertices OnTestFunction");
    OH_Drawing_Point2D positions[] = { { 100, 100 }, { 200, 100 }, { 150, 200 } };  // 100, 200, 150, 200 for test
    OH_Drawing_Point2D texs[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.5f, 1.0f } }; // 0.0f, 1.0f, 0.5f, 1.0f for test
    const uint16_t indices[] = { 0, 1, 2 };                                         // 0, 1, 2 for test
    uint32_t colors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    int32_t indexCount = sizeof(indices) / sizeof(indices[0]);      // 0 for test
    int32_t vertexCount = sizeof(positions) / sizeof(positions[0]); // 0 for test
    OH_Drawing_CanvasDrawVertices(canvas, OH_Drawing_VertexMode::VERTEX_MODE_TRIANGLES, vertexCount, positions, texs,
        colors, indexCount, indices, mode_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_4800
@Description:OH_Drawing_CanvasIsClipEmpty参数isClipEmpty获取为true
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasIsClipEmpty，点击draw按钮
@Result:
    1、页面无绘制，日志打印：CanvasIsClipEmpty isClipEmpty1 is 0;CanvasIsClipEmpty isClipEmpty2 is 1
 */
void FunctionCanvasIsClipEmpty::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasIsClipEmpty OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 35); // 35 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200); // 100, 100, 200, 200 for test
    bool isClipEmpty = true;
    OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasIsClipEmpty(canvas, &isClipEmpty);
    DRAWING_LOGI("CanvasIsClipEmpty isClipEmpty1 =%{public}d ", isClipEmpty);
    bool isClipEmpty1 = true;
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100, 100, 200, 200); // 100, 100, 200, 200 for test
    OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    OH_Drawing_CanvasIsClipEmpty(canvas, &isClipEmpty1);
    DRAWING_LOGI("CanvasIsClipEmpty isClipEmpty2 =%{public}d ", isClipEmpty1);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_CANVAS_5001
@Description:OH_Drawing_CanvasClipRegion参数clipop传入DIFFERENCE
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionCanvasClipRegion，点击draw按钮
@Result:
    1、页面无绘制
 */
void FunctionCanvasClipRegion::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionCanvasClipRegion OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0xFFFF0000);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 100, 100 for test
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_CanvasClipRegion(canvas, region, DIFFERENCE);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRegion(canvas, region);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_BrushDestroy(brush);
}

void FunctionCanvasQuickRejectPath::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    bool quickReject = false;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 5, 5);
    OH_Drawing_PathLineTo(path, 10, 10);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_ErrorCode code = OH_Drawing_CanvasQuickRejectPath(canvas, path, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);
    
    OH_Drawing_Path* path1 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path1, -5, -5);
    OH_Drawing_PathLineTo(path1, 0, 10);
    OH_Drawing_ErrorCode code1 = OH_Drawing_CanvasQuickRejectPath(canvas, path1, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code1);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);
    
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path2, -5, -5);
    OH_Drawing_PathLineTo(path2, -5, 10);
    OH_Drawing_ErrorCode code2 = OH_Drawing_CanvasQuickRejectPath(canvas, path2, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code2);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);
    
    OH_Drawing_Path* path3 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path3, 10, -5);
    OH_Drawing_PathLineTo(path3, -5, 10);
    OH_Drawing_ErrorCode code3 = OH_Drawing_CanvasQuickRejectPath(canvas, path3, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code3);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);

    OH_Drawing_ErrorCode code4 = OH_Drawing_CanvasQuickRejectPath(canvas, nullptr, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code4);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);
    
    OH_Drawing_ErrorCode code5 = OH_Drawing_CanvasQuickRejectPath(nullptr, path3, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test code = %{public}d", code5);
    DRAWING_LOGI("FunctionCanvasQuickRejectPath test quickRejectPath = %{public}d", quickReject);
    
    OH_Drawing_PathDestroy(path1);
    OH_Drawing_PathDestroy(path2);
    OH_Drawing_PathDestroy(path3);
}

void FunctionCanvasQuickRejectRect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    bool quickReject = false;
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(10, 10, 20, 20);
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    OH_Drawing_ErrorCode code1 = OH_Drawing_CanvasQuickRejectRect(canvas, rect1, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code1);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(-20, -20, 0, 0);
    OH_Drawing_CanvasDrawRect(canvas, rect2);
    OH_Drawing_ErrorCode code2 = OH_Drawing_CanvasQuickRejectRect(canvas, rect2, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code2);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(-20, -20, -10, -10);
    OH_Drawing_CanvasDrawRect(canvas, rect3);
    OH_Drawing_ErrorCode code3 = OH_Drawing_CanvasQuickRejectRect(canvas, rect3, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code3);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(-20, -20, 999999, 999999);
    OH_Drawing_CanvasDrawRect(canvas, rect3);
    OH_Drawing_ErrorCode code4 = OH_Drawing_CanvasQuickRejectRect(canvas, rect4, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code4);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_ErrorCode code5 = OH_Drawing_CanvasQuickRejectRect(canvas, nullptr, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code5);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_ErrorCode code6 = OH_Drawing_CanvasQuickRejectRect(nullptr, rect4, &quickReject);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test code = %{public}d", code6);
    DRAWING_LOGI("FunctionCanvasQuickRejectRect test quickRejectRect = %{public}d", quickReject);
    
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
}

void FunctionCanvasDrawArcWithCenter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    float penWidth = 3.f;
    OH_Drawing_PenSetWidth(pen, penWidth);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(20, 20, 120, 120);
    OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, 10.f, 100.f, true);
    
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(220, 20, 320, 120);
    OH_Drawing_CanvasDrawArcWithCenter(canvas, rect1, -90.f, 270.f, false);
    
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(20, 220, 320, 320);
    OH_Drawing_CanvasDrawArcWithCenter(canvas, rect2, 90.f, 570.f, true);
    
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

void FunctionCanvasDrawNestedRoundRect::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0x45, 0x71, 0xC4));
    OH_Drawing_PenSetAntiAlias(pen, true);
    float penWidth = 3.f;
    OH_Drawing_PenSetWidth(pen, penWidth);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_RoundRect* outer = OH_Drawing_RoundRectCreate(rect, 0, 0);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_RoundRect* inner = OH_Drawing_RoundRectCreate(rect1, 0, 0);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, inner);
    
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, inner);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_RoundRect* outer2 = OH_Drawing_RoundRectCreate(rect2, 10, 10);
    OH_Drawing_Rect* rect12 = OH_Drawing_RectCreate(400, 120, 600, 300);
    OH_Drawing_RoundRect* inner2 = OH_Drawing_RoundRectCreate(rect12, 10, 10);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer2, inner2);

    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(100, 400, 300, 600);
    OH_Drawing_RoundRect* outer3 = OH_Drawing_RoundRectCreate(rect3, 10, 10);
    OH_Drawing_Rect* rect13 = OH_Drawing_RectCreate(120, 420, 280, 580);
    OH_Drawing_RoundRect* inner3 = OH_Drawing_RoundRectCreate(rect13, 10, 10);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer3, inner3);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, inner3, outer3);
    
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RoundRectDestroy(outer);
    OH_Drawing_RoundRectDestroy(inner);
    OH_Drawing_PenDestroy(pen);
}

void FunctionCanvasDrawPixelMapNine::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Pixelmap_InitializationOptions* createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    int32_t imageWidth = 100;
    int32_t imageHeight = 100;
    OH_PixelmapInitializationOptions_SetWidth(createOps, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2);      // 2 is ALPHA_FORMAT_PREMUL
    size_t bufferSize = imageWidth * imageHeight * 4;                 // 4 for test
    void* bitmapAddr = malloc(bufferSize);
    if (bitmapAddr == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)bitmapAddr)[i] = DRAW_COLORBLUE;
    }
    OH_PixelmapNative* pixelMapNative = nullptr;
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t*)bitmapAddr, bufferSize, createOps, &pixelMapNative);
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(0, 0, 100, 100);

    OH_Drawing_ErrorCode code = OH_Drawing_CanvasDrawPixelMapNine(
        canvas, pixelMap, nullptr, dst, OH_Drawing_FilterMode::FILTER_MODE_NEAREST);
    
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_PixelmapNative_Release(pixelMapNative);
    free(bitmapAddr);
    OH_PixelmapInitializationOptions_Release(createOps);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}