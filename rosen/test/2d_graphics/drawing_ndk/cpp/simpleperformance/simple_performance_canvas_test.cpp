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

#include "simple_performance_canvas_test.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_pixel_map.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

#include "common/log_common.h"

void SimplePerformanceCanvasDrawRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 保证性能测试稳定性: 绘制100，100的矩形
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawRect(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasDrawBitmapRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(100, 100, 256, 256); // 指定区域 100, 100, 256, 256
    OH_Drawing_SamplingOptions* sampling = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    for (int i = 0; i < testCount_; i++) {
        // 绘制指定区域的纯色bitmap
        OH_Drawing_CanvasDrawBitmapRect(canvas, bm, rect, dst, sampling);
    }
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasDrawCircle::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point* point = OH_Drawing_PointCreate(200, 200); // 200, 200 圆心
    for (int i = 0; i < testCount_; i++) {
        // 保证性能测试稳定性: 绘制圆心（200, 200）半径100的圆
        OH_Drawing_CanvasDrawCircle(canvas, point, 100);
    }
    OH_Drawing_PointDestroy(point);
}

void SimplePerformanceCanvasDrawOval::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawOval(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasDrawArc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawArc(canvas, rect, 0.f, 180.f); // 0.f, 180.f 绘制圆弧
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasDrawRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20); // 20, 20 圆角半径
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

void SimplePerformanceCanvasDrawTextBlob::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 0, 0);
    }
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(blob);
}

void SimplePerformanceCanvasClipRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasClipRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20); // 20, 20 圆角半径
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipRoundRect(canvas, roundRect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

void SimplePerformanceCanvasRotate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasRotate(canvas, 120, 50, 50); // 120, 50, 50 旋转平移角度
    }
}

void SimplePerformanceCanvasTranslate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasTranslate(canvas, 100, 100); // 100, 100 平移
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasScale::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasScale(canvas, 2, 2); // 2, 2 缩放比例
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasConcatMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(100, 100); // 100, 100 平移
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    }
    OH_Drawing_MatrixDestroy(matrix);
}

void SimplePerformanceCanvasClear::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t color = 0xFF0000FF;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClear(canvas, color);
    }
}

void SimplePerformanceCanvasSetMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matRix = OH_Drawing_MatrixCreateRotation(45, 100, 100); // 45 矩阵旋转角度，100,100 矩阵偏移量
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSetMatrix(canvas, matRix);
    }
    OH_Drawing_MatrixDestroy(matRix);
}

void SimplePerformanceCanvasResetMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasResetMatrix(canvas);
    }
}

void SimplePerformanceCanvasDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Image* Image = OH_Drawing_ImageCreate();

    // 创建采样选项对象
    OH_Drawing_FilterMode filterMode = FILTER_MODE_LINEAR;
    OH_Drawing_MipmapMode mipmapMode = MIPMAP_MODE_NEAREST;
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(filterMode, mipmapMode);
    OH_Drawing_Rect* recty = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
    OH_Drawing_Rect* rectm = OH_Drawing_RectCreate(100, 100, 256, 256); // 100, 100, 256, 256 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawImageRectWithSrc(canvas, Image, recty, rectm, samplingOptions, FAST_SRC_RECT_CONSTRAINT);
    }
    OH_Drawing_RectDestroy(recty);
    OH_Drawing_RectDestroy(rectm);
    OH_Drawing_ImageDestroy(Image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

void SimplePerformanceCanvasDrawShadow::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* pathCreate = OH_Drawing_PathCreate(); // 用于创建一个路径对象。
    OH_Drawing_Point3D planeParams;
    OH_Drawing_Point3D devLightPos;
    planeParams.x = 0;        // 0 用于坐标计算
    planeParams.y = 0;        // 0 用于坐标计算
    planeParams.z = 50.f;     // 50.f 用于坐标计算
    devLightPos.x = 100.f;    // 100.f 用于坐标计算
    devLightPos.y = 100.f;    // 100.f 用于坐标计算
    devLightPos.z = 100.f;    // 100.f 用于坐标计算
    float lightRadius = 50.f; // 50.f 用于坐标计算
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_TRANSPARENT_OCCLUDER;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawShadow(
            canvas, pathCreate, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
    // 在使用完路径后销毁它
    OH_Drawing_PathDestroy(pathCreate);
}

void SimplePerformanceCanvasSkew::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSkew(canvas, 50, 50); // 50, 50 斜切值
    }
}

void SimplePerformanceCanvasDrawImageRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR,
        MIPMAP_MODE_NEAREST);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

void SimplePerformanceCanvasDrawVertices::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D positions[] = { { 100, 100 }, { 200, 100 },
        { 150, 200 } }; // 100, 200, 150, 200 这些数字用于指定要在画布上绘制的三角形的顶点位置
    OH_Drawing_Point2D texs[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f },
        { 0.5f, 1.0f } }; // 0.0f, 1.0f, 0.5f, 1.0f 这些浮点数值定义了texs数组中三个纹理坐标
    const uint16_t indices[] = { 0, 1, 2 }; // 0, 1, 2 这些整数定义了indices数组中的索引值
    uint32_t colors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    int32_t indexCount = sizeof(indices) / sizeof(indices[0]);
    int32_t vertexCount = sizeof(positions) / sizeof(positions[0]);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawVertices(canvas, OH_Drawing_VertexMode::VERTEX_MODE_TRIANGLES, vertexCount, positions,
            texs, colors, indexCount, indices, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    }
}

void SimplePerformanceCanvasReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    bool res = false;
    OH_Drawing_Image_Info imageInfo { 100, 100, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE }; // 100 用于宽高
    char* dstPixels = static_cast<char*>(malloc(100 * 100 * 4)); // 100 4 用于像素计算
    if (dstPixels == nullptr) {
        return;
    }
    for (int i = 0; i < testCount_; i++) {
        // 从画布中拷贝像素数据到指定地址，去掉readPixels接口就只有drawRect接口画的一个矩形,用日志看读数据的结果
        res = OH_Drawing_CanvasReadPixels(canvas, &imageInfo, dstPixels, 100 * 4, 0, 0); // 100, 4 用于像素计算
    }
    free(dstPixels);
    DRAWING_LOGI("CanvasReadPixels::OnTestPerformance readPixels success=%{public}s", res ? "true" : "false");
}

void SimplePerformanceCanvasReadPixelsToBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    bool res = false;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    OH_Drawing_CanvasDrawRect(canvas, rect);
    for (int i = 0; i < testCount_; i++) {
        // 从画布拷贝像素数据到位图中,去掉readPixels接口就只有drawRect接口画的一个矩形,用日志看读数据的结果
        res = OH_Drawing_CanvasReadPixelsToBitmap(canvas, bm, 0, 0); // 0, 0 位置
    }
    DRAWING_LOGI("CanvasReadPixelsToBitmap::OnTestPerformance readPixels success=%{public}s", res ? "true" : "false");
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(bm);
}

void SimplePerformanceCanvasDrawPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_PathDestroy(path);
}

void SimplePerformanceCanvasRestoreToCount::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasRestoreToCount(canvas, 0);
    }
}

void SimplePerformanceCanvasDrawPoints::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D pts[2]; // 2 点个数
    pts[0].x = 50; // 50 点位置
    pts[0].y = 50; // 50 点位置
    pts[1].x = 100; // 100 点位置
    pts[1].y = 100; // 100 点位置
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, 2, pts); // 2 点个数
    }
}

void SimplePerformanceCanvasDrawLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawLine(canvas, 0, 0, 100, 100); // 0, 0, 100, 100 创建直线
    }
}

void SimplePerformanceCanvasDrawLayer::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSaveLayer(canvas, bounds, styleBrush_);
    }
    OH_Drawing_RectDestroy(bounds);
}

void SimplePerformanceCanvasDrawBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0); // 0, 0 位置坐标
    }
    OH_Drawing_BitmapDestroy(bitmap);
}

void SimplePerformanceCanvasDrawBackground::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBackground(canvas, styleBrush_);
    }
}

void SimplePerformanceCanvasDrawRegion::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawRegion(canvas, region);
    }
    OH_Drawing_RegionDestroy(region);
}

void SimplePerformanceCanvasDrawPixelMapRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Pixelmap_InitializationOptions* options = nullptr;
    OH_PixelmapNative* pixelMap = nullptr;
    OH_PixelmapInitializationOptions_Create(&options);
    // 4 means width
    OH_PixelmapInitializationOptions_SetWidth(options, 4);
    // 4 means height
    OH_PixelmapInitializationOptions_SetHeight(options, 4);
    // 3 means RGBA format
    OH_PixelmapInitializationOptions_SetPixelFormat(options, 3);
    // 2 means ALPHA_FORMAT_PREMUL format
    OH_PixelmapInitializationOptions_SetAlphaType(options, 2);
    // 255 means rgba data
    uint8_t data[] = { 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255 };
    // 16 means data length
    size_t dataLength = 16;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, options, &pixelMap);
    OH_Drawing_PixelMap* drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    OH_Drawing_Rect* srcRect = OH_Drawing_RectCreate(0, 0, 4, 4); // 0, 0, 4, 4 创建矩形
    OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 4, 4); // 0, 0, 4, 4 创建矩形
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPixelMapRect(canvas, drPixelMap, srcRect, dstRect, samplingOptions);
    }
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

void SimplePerformanceCanvasSave::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasRestore(canvas);
    }
}

void SimplePerformanceCanvasClipPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }
    OH_Drawing_PathDestroy(path);
}

void SimplePerformanceCanvasGetTotalMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* total = OH_Drawing_MatrixCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasGetTotalMatrix(canvas, total);
    }
    OH_Drawing_MatrixDestroy(total);
}

void SimplePerformanceCanvasRestore::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasRestore(canvas);
    }
}

void SimplePerformanceCanvasGetLocalClipBounds::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 400); // 0, 0, 400, 400 创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasGetLocalClipBounds(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
}

void SimplePerformanceCanvasGetSaveCount::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t fSave_Count = 0;
    for (int i = 0; i < testCount_; i++) {
        fSave_Count = OH_Drawing_CanvasGetSaveCount(canvas);
    }
}

void SimplePerformanceCanvasGetWidth::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t canvas_width = 0;
    for (int i = 0; i < testCount_; i++) {
        canvas_width = OH_Drawing_CanvasGetWidth(canvas);
    }
}

void SimplePerformanceCanvasGetHeight::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t canvas_height = 0;
    for (int i = 0; i < testCount_; i++) {
        canvas_height = OH_Drawing_CanvasGetHeight(canvas);
    }
}

void SimplePerformanceCanvasCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Canvas* canvasT = nullptr;
    for (int i = 0; i < testCount_; i++) {
        canvasT = OH_Drawing_CanvasCreate();
        OH_Drawing_CanvasDestroy(canvasT);
    }
}

void SimplePerformanceCanvasDestroy::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Canvas* canvasT = nullptr;
    for (int i = 0; i < testCount_; i++) {
        canvasT = OH_Drawing_CanvasCreate();
        OH_Drawing_CanvasDestroy(canvasT);
    }
}

void SimplePerformanceCanvasAttachPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
    }
    OH_Drawing_PenDestroy(pen);
}

void SimplePerformanceCanvasDetachPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDetachPen(canvas);
    }
}

void SimplePerformanceCanvasAttachBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_BrushDestroy(brush);
}

void SimplePerformanceCanvasDetachBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDetachBrush(canvas);
    }
}

void SimplePerformanceCanvasDrawPoint::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D point2D = { 10, 10 }; // 10 坐标
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPoint(canvas, &point2D);
    }
}

void SimplePerformanceCanvasDrawColor::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t color = 0xFF0000FF;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawColor(canvas, color, BLEND_MODE_CLEAR);
    }
}

void SimplePerformanceCanvasDrawSingleCharacter::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    const char* str = "a";
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawSingleCharacter(canvas, str, font, 0.f, 0.f);
    }
    OH_Drawing_FontDestroy(font);
}

void SimplePerformanceCanvasClipRegion::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipRegion(canvas, region, DIFFERENCE);
    }
    OH_Drawing_RegionDestroy(region);
}

void SimplePerformanceCanvasIsClipEmpty::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasIsClipEmpty(canvas, 0);
    }
}

void SimplePerformanceCanvasGetImageInfo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Image_Info imageInfo;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasGetImageInfo(canvas, &imageInfo);
    }
}
