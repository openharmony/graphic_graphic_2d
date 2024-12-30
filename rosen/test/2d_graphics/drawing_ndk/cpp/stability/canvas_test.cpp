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

#include "canvas_test.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
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
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_text_blob.h>

#include "native_drawing/drawing_pixel_map.h"
#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0100
@Description:CanvasCreate-CanvasDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、循环创建canvas，销毁canvas
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityCanvasCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Canvas* canvas1 = OH_Drawing_CanvasCreate();
        OH_Drawing_CanvasDestroy(canvas1);
    }
}

void StabilityCanvasInvoke::CanvasBindTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasBind(canvas, bitmap1_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0200
@Description:CanvasCreate-相关配置接口全调用-CanvasDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、循环创建canvas，销毁canvas
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityCanvasInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Canvas* canvas1 = OH_Drawing_CanvasCreate();
        for (int j = 0; j < SETTING_FUNCTION_CANVAS_MAX; j++) {
            handlers_[j](canvas1);
        }
        OH_Drawing_CanvasDestroy(canvas1);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0300
@Description:Save-SaveLayer-Restore-RestoreToCount循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Canvas和Rect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityCanvasSave::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_Rect* rect5 = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
        // 2 for test,rand true or false
        OH_Drawing_CanvasClipRect(canvas, rect5, DrawGetEnum(DIFFERENCE, INTERSECT, i), i % 2 == 0);
        OH_Drawing_RectDestroy(rect5);
        OH_Drawing_CanvasRestore(canvas);

        uint32_t saveCount = OH_Drawing_CanvasGetSaveCount(canvas);
        // 0, 0, 100, 100 创建矩形
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_CanvasSaveLayer(canvas, rect, brush);
        OH_Drawing_CanvasRestoreToCount(canvas, saveCount);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_RectDestroy(rect);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0400
@Description:AttachPen/Brush-draw-DetachPen/Brush循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Canvas、Rect、Pen、Brush相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityCanvasAttach::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_CanvasAttachBrush(canvas, brush);

        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_PenDestroy(pen);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_RectDestroy(rect);
    }
}

void StabilityCanvasRandInvoke::CanvasDrawLineTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawLine(canvas, rand_.nextF(), rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityCanvasRandInvoke::CanvasDrawPathTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // 100,100,40 for test
    OH_Drawing_PathAddCircle(path, 100, 100, 40, PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void StabilityCanvasRandInvoke::CanvasDrawPixelMapRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawPixelMapRect(canvas, pixelMap_, src_, dst_, samplingOptions_);
}

void StabilityCanvasRandInvoke::CanvasDrawBackgroundTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawBackground(canvas, brush);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}

void StabilityCanvasRandInvoke::CanvasDrawRegionTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawRegion(canvas, region_);
}

void StabilityCanvasRandInvoke::CanvasDrawPointTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D point = { 10, 20 };
    OH_Drawing_CanvasDrawPoint(canvas, &point);
}

void StabilityCanvasRandInvoke::CanvasDrawPointsTest(OH_Drawing_Canvas* canvas)
{
    // 3 length,10,20,15,34,45, for test
    OH_Drawing_Point2D points[3] = { { 10, 20 }, { 15, 20 }, { 34, 45 } };
    // 3 length
    OH_Drawing_CanvasDrawPoints(canvas, DrawGetEnum(POINT_MODE_POINTS, POINT_MODE_POLYGON, rand_.nextU()), 3, points);
}

void StabilityCanvasRandInvoke::CanvasDrawBitmapTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1_, 0, 0);
}

void StabilityCanvasRandInvoke::CanvasDrawBitmapRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap1_, src_, dst_, samplingOptions_);
}

void StabilityCanvasRandInvoke::CanvasDrawRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawRect(canvas, src_);
}

void StabilityCanvasRandInvoke::CanvasDrawCircleTest(OH_Drawing_Canvas* canvas)
{
    // 5.0 for test
    OH_Drawing_CanvasDrawCircle(canvas, point_, 5.0);
}

void StabilityCanvasRandInvoke::CanvasDrawColorTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawColor(canvas, 0xFFFF0000, DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, rand_.nextU()));
}

void StabilityCanvasRandInvoke::CanvasDrawOvalTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawOval(canvas, src_);
}

void StabilityCanvasRandInvoke::CanvasDrawArcTest(OH_Drawing_Canvas* canvas)
{
    // 367 > 360
    OH_Drawing_CanvasDrawArc(canvas, src_, 0, 367);
}

void StabilityCanvasRandInvoke::CanvasDrawRoundRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect_);
}

void StabilityCanvasRandInvoke::CanvasDrawSingleCharacterTest(OH_Drawing_Canvas* canvas)
{
    // 100,100 position for test
    OH_Drawing_CanvasDrawSingleCharacter(canvas, "c", font_, 100, 100);
}

void StabilityCanvasRandInvoke::CanvasDrawTextBlobTest(OH_Drawing_Canvas* canvas)
{
    // 100, position x,y for test
    OH_Drawing_CanvasDrawTextBlob(canvas, textBlob_, 100, 100);
}

void StabilityCanvasRandInvoke::CanvasClipRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClipRect(canvas, src_, DrawGetEnum(DIFFERENCE, INTERSECT, rand_.nextU()), rand_.nextRangeU(0, 1));
}

void StabilityCanvasRandInvoke::CanvasClipRoundRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClipRoundRect(
        canvas, roundRect_, DrawGetEnum(DIFFERENCE, INTERSECT, rand_.nextU()), rand_.nextRangeU(0, 1));
}

void StabilityCanvasRandInvoke::CanvasClipPathTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClipPath(canvas, path_, DrawGetEnum(DIFFERENCE, INTERSECT, rand_.nextU()), rand_.nextRangeU(0, 1));
}

void StabilityCanvasRandInvoke::CanvasClipRegionTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClipRegion(canvas, region_, DrawGetEnum(DIFFERENCE, INTERSECT, rand_.nextU()));
}

void StabilityCanvasRandInvoke::CanvasRotateTest(OH_Drawing_Canvas* canvas)
{
    // 50.0, 100,100 for test
    OH_Drawing_CanvasRotate(canvas, 50.0, 100, 100);
}

void StabilityCanvasRandInvoke::CanvasTranslateTest(OH_Drawing_Canvas* canvas)
{
    // 100, 100 for test
    OH_Drawing_CanvasTranslate(canvas, 100, 100);
}

void StabilityCanvasRandInvoke::CanvasScaleTest(OH_Drawing_Canvas* canvas)
{
    // 2, 3 for test
    OH_Drawing_CanvasScale(canvas, 2, 3);
}

void StabilityCanvasRandInvoke::CanvasSkewTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasSkew(canvas, rand_.nextF(), rand_.nextF());
}

void StabilityCanvasRandInvoke::CanvasGetWidthTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasGetWidth(canvas);
}

void StabilityCanvasRandInvoke::CanvasGetHeightTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasGetHeight(canvas);
}

void StabilityCanvasRandInvoke::CanvasGetLocalClipBoundsTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasGetLocalClipBounds(canvas, src_);
}

void StabilityCanvasRandInvoke::CanvasGetTotalMatrixTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasGetTotalMatrix(canvas, matrix_);
}

void StabilityCanvasRandInvoke::CanvasConcatMatrixTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasConcatMatrix(canvas, matrix_);
}

void StabilityCanvasRandInvoke::CanvasDrawShadowTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawShadow(canvas, path_, { 100, 100, 10 }, { 100, 100, 10 }, 10.f, 0xFF000000, 0xFFFF0000,
        DrawGetEnum(SHADOW_FLAGS_NONE, SHADOW_FLAGS_ALL, rand_.nextU()));
}

void StabilityCanvasRandInvoke::CanvasClearTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, rand_.nextU());
}

void StabilityCanvasRandInvoke::CanvasSetMatrixTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasSetMatrix(canvas, matrix_);
}

void StabilityCanvasRandInvoke::CanvasResetMatrixTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasResetMatrix(canvas);
}

void StabilityCanvasRandInvoke::CanvasDrawImageRectWithSrcTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawImageRectWithSrc(canvas, image_, src_, dst_, samplingOptions_,
        DrawGetEnum(STRICT_SRC_RECT_CONSTRAINT, FAST_SRC_RECT_CONSTRAINT, rand_.nextU()));
}

void StabilityCanvasRandInvoke::CanvasDrawImageRectTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawImageRect(canvas, image_, dst_, samplingOptions_);
}

void StabilityCanvasRandInvoke::CanvasDrawVerticesTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D positions[] = { { 100, 100 }, { 200, 100 },
        { 150, 200 } }; // 100, 200, 150, 200 这些数字用于指定要在画布上绘制的三角形的顶点位置
    OH_Drawing_Point2D texs[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f },
        { 0.5f, 1.0f } }; // 0.0f, 1.0f, 0.5f, 1.0f 这些浮点数值定义了texs数组中三个纹理坐标
    const uint16_t indices[] = { 0, 1, 2 }; // 0, 1, 2 这些整数定义了indices数组中的索引值
    uint32_t colors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    int32_t indexCount = sizeof(indices) / sizeof(indices[0]);
    int32_t vertexCount = sizeof(positions) / sizeof(positions[0]);
    OH_Drawing_CanvasDrawVertices(canvas, DrawGetEnum(VERTEX_MODE_TRIANGLES, VERTEX_MODE_TRIANGLE_FAN, rand_.nextU()),
        vertexCount, positions, texs, colors, indexCount, indices,
        DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, rand_.nextU()));
}

void StabilityCanvasRandInvoke::CanvasReadPixelsTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Image_Info imageInfo { (int32_t)bitmapWidth_, (int32_t)bitmapHeight_, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL }; // 100 用于宽高
    OH_Drawing_CanvasReadPixels(canvas, &imageInfo, bitmapAddr_, bitmapWidth_ * 4, 0, 0); // 4 rgba
}

void StabilityCanvasRandInvoke::CanvasReadPixelsToBitmapTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasReadPixelsToBitmap(canvas, bitmap1_, 0, 0);
}

void StabilityCanvasRandInvoke::CanvasIsClipEmptyTest(OH_Drawing_Canvas* canvas)
{
    bool isClipEmpty = false;
    OH_Drawing_CanvasIsClipEmpty(canvas, &isClipEmpty);
}

void StabilityCanvasRandInvoke::CanvasGetImageInfoTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Image_Info imageInfo { 0, 0, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL }; // 100 用于宽高
    OH_Drawing_CanvasGetImageInfo(canvas, &imageInfo);
}

StabilityCanvasRandInvoke::~StabilityCanvasRandInvoke()
{
    OH_Drawing_Rect* src_ = nullptr;
    OH_Drawing_RectDestroy(src_);
    OH_Drawing_RectDestroy(dst_);
    OH_Drawing_PointDestroy(point_);
    OH_Drawing_RoundRectDestroy(roundRect_);
    OH_Drawing_FontDestroy(font_);
    OH_Drawing_TextBlobDestroy(textBlob_);
    OH_Drawing_PathDestroy(path_);
    OH_Drawing_RegionDestroy(region_);
    OH_Drawing_MatrixDestroy(matrix_);

    OH_Drawing_PixelMapDissolve(pixelMap_);
    OH_PixelmapNative_Release(pixelMapNative_);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions_);
    free(bitmapAddr_);
    OH_PixelmapInitializationOptions_Release(createOps_);
    OH_Drawing_BitmapDestroy(bitmap1_);
    OH_Drawing_ImageDestroy(image_);
};

void StabilityCanvasRandInvoke::CreatePixelMap()
{
    int32_t imageWidth = bitmapWidth_;
    int32_t imageHeight = bitmapHeight_;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps_);
    if (ret != IMAGE_SUCCESS || !createOps_) {
        DRAWING_LOGE("StabilityCanvasRandInvoke:: OH_PixelmapInitializationOptions_Create"
                     " failed %{public}d",
            ret);
        return;
    }
    OH_PixelmapInitializationOptions_SetWidth(createOps_, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps_, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps_, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps_, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps_, 2);      // 2 is ALPHA_FORMAT_PREMUL

    size_t bufferSize = imageWidth * imageHeight * 4;

    bitmapAddr_ = malloc(bufferSize);
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        // 3 for test
        if (i < imageWidth * imageHeight / 3) {
            ((uint32_t*)bitmapAddr_)[i] = 0xFF5500FF;
            // 2,3 for test
        } else if (i < imageWidth * imageHeight * 2 / 3) {
            ((uint32_t*)bitmapAddr_)[i] = 0xFF0055FF;
        } else {
            ((uint32_t*)bitmapAddr_)[i] = 0xFFFF5500;
        }
    }

    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_PixelmapNative_CreatePixelmap((uint8_t*)bitmapAddr_, bufferSize, createOps_, &pixelMapNative_);
    pixelMap_ = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative_);

    OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, rand_.nextU());
    OH_Drawing_BitmapFormat format = { colorFormat,
        DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, rand_.nextU()) };
    OH_Drawing_Image_Info imageInfo { (int32_t)bitmapWidth_, (int32_t)bitmapHeight_, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL };
    bitmap1_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, bitmapAddr_, bitmapWidth_ * 4); //4 rgba
    image_ = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image_, bitmap1_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_CANVAS_0500
@Description:Canvas相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Canvas相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityCanvasRandInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        uint32_t index = rand_.nextRangeU(0, OPERATION_FUNCTION_CANVAS_MAX - 1);
        handlers_[index](canvas);
    }
}