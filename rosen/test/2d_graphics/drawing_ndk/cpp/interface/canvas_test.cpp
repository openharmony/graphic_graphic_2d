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

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
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

#include "test_common.h"

#include "common/log_common.h"

void CanvasDrawRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;                               // 100 用于边界计算
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);   // 100 用于边界计算
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100); // 100 用于边界计算
        // 针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawBitmapRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_SamplingOptions* sampling = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    float l_1 = rand.nextULessThan(bitmapWidth_);
    float t_1 = rand.nextULessThan(bitmapHeight_);
    float r_1 = l_1 + rand.nextULessThan(bitmapWidth_);
    float b_1 = t_1 + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, bitmapWidth_, bitmapHeight_, &cFormat);
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bm);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(l_1, t_1, r_1, b_1); // 指定区域
    for (int i = 0; i < testCount_; i++) {
        // 将位图的指定区域绘制到画布的指定区域，指定区域
        OH_Drawing_CanvasDrawBitmapRect(canvas, bm, rect, dst, sampling);
    }
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
}

void CanvasDrawCircle::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Point* point = OH_Drawing_PointCreate(x, y);
        float z = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawCircle(canvas, point, z);
        OH_Drawing_PointDestroy(point);
    }
}

void CanvasDrawOval::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawOval(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawArc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        float startAngle = rand.nextULessThan(bitmapWidth_);
        float sweepAngle = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawArc(canvas, rect, startAngle, sweepAngle);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        float xRad = rand.nextULessThan(bitmapWidth_);
        float yRad = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawTextBlob::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount_; i++) {
        int x = rand.nextRangeF(0, bitmapWidth_);
        int y = rand.nextRangeF(0, bitmapHeight_);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, x, y);
    }
    OH_Drawing_TextBlobDestroy(blob);
}

void CanvasClipRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    }
    OH_Drawing_RectDestroy(rect);
}

void CanvasClipRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        float xRad = rand.nextULessThan(bitmapWidth_);
        float yRad = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
        OH_Drawing_CanvasClipRoundRect(canvas, roundRect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasRotate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    float_t min = 0.0f; // 0.0f  最小值
    float_t max = 360.0f;
    TestRend rand;
    float_t readTime = rand.nextRangeF(min, max); // 生成随机浮点数
    float randNumberx = rand.nextF();
    float randNumbery = rand.nextF();
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasRotate(canvas, readTime, randNumberx, randNumbery);
    }
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasTranslate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    float randNumberx = rand.nextF();
    float randNumbery = rand.nextF();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasTranslate(canvas, randNumberx, randNumbery);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasScale::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    float randNumberx = rand.nextF();
    float randNumbery = rand.nextF();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasScale(canvas, randNumberx, randNumbery);
    }
    OH_Drawing_RectDestroy(rect);
}

void CanvasConcatMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float randNumberx = rand.nextF();
    float randNumbery = rand.nextF();
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(randNumberx, randNumbery);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_RectDestroy(rect);
}

void CanvasClear::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t color = 0xFF0000FF;
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClear(canvas, color);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasSetMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matRix = OH_Drawing_MatrixCreateRotation(45, 100, 100); // 45 矩阵旋转角度，100,100 矩阵偏移量
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSetMatrix(canvas, matRix);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasResetMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matRix = OH_Drawing_MatrixCreate();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;                               // 100 用于高度计算
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);   // 100 用于宽度计算
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100); // 100 用于高度计算
        // 针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasSetMatrix(canvas, matRix);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasResetMatrix(canvas);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t w = 64; // 64 用于宽度计算
    uint32_t h = 64; // 64 用于高度计算
    OH_Drawing_Image* Image = OH_Drawing_ImageCreate();
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_ImageBuildFromBitmap(Image, bm);

    // 创建采样选项对象
    OH_Drawing_FilterMode filterMode = FILTER_MODE_LINEAR;
    OH_Drawing_MipmapMode mipmapMode = MIPMAP_MODE_NEAREST;
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(filterMode, mipmapMode);
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    // 针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
    OH_Drawing_Rect* recty = OH_Drawing_RectCreate(l, t, r, b);                     // 参数3
    OH_Drawing_Rect* rectm = OH_Drawing_RectCreate(l + 10, t + 10, r + 10, b + 10); // 10 用于矩形创建的参数计算
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_SrcRectConstraint enumConstr = i % 2 == 0 ? STRICT_SRC_RECT_CONSTRAINT : FAST_SRC_RECT_CONSTRAINT;
        OH_Drawing_CanvasDrawImageRectWithSrc(canvas, Image, recty, rectm, samplingOptions, enumConstr);
    }
    OH_Drawing_CanvasDrawRect(canvas, recty);
    OH_Drawing_CanvasDrawRect(canvas, rectm);
    OH_Drawing_RectDestroy(recty);
    OH_Drawing_RectDestroy(rectm);
    OH_Drawing_ImageDestroy(Image);
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasDrawShadow::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
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
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_PathAddRect(pathCreate, l, t, r, b, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawShadow(
            canvas, pathCreate, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
    // 在使用完路径后销毁它
    OH_Drawing_PathDestroy(pathCreate);
}

void CanvasSkew::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    for (int i = 0; i < testCount_; i++) {
        float sx = rand.nextF();
        float sy = rand.nextF();
        OH_Drawing_CanvasSkew(canvas, sx, sy);
    }
    OH_Drawing_RectDestroy(rect);
}

void CanvasDrawImageRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t w = 64; // 64 用于宽度计算
    uint32_t h = 64; // 64 用于高度计算
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_NEAREST);
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;                               // 100 用于高度计算
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);   // 100 用于宽度计算
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100); // 100 用于高度计算
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasDrawVertices::OnTestPerformance(OH_Drawing_Canvas* canvas)
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

void CanvasReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 100; // 100 用于宽度计算
    int32_t h = 100; // 100 用于高度计算
    bool res;
    TestRend rand;
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    char* dstPixels = (char*)malloc(r * b * 4); // 4 用于像素计算
    if (dstPixels == nullptr)
        return;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    for (int i = 0; i < testCount_; i++) {
        // 从画布中拷贝像素数据到指定地址，去掉readPixels接口就只有drawRect接口画的一个矩形,用日志看读数据的结果
        res = OH_Drawing_CanvasReadPixels(canvas, &imageInfo, dstPixels, r * 4, l, t); // 4 用于像素计算
    }
    free(dstPixels);
    DRAWING_LOGI("CanvasReadPixels::OnTestPerformance readPixels success=%{public}s", res ? "true" : "false");
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasReadPixelsToBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 100; // 100 用于宽度计算
    int32_t h = 100; // 100 用于高度计算
    bool res;
    TestRend rand;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
        OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    for (int i = 0; i < testCount_; i++) {
        // 从画布拷贝像素数据到位图中,去掉readPixels接口就只有drawRect接口画的一个矩形,用日志看读数据的结果
        res = OH_Drawing_CanvasReadPixelsToBitmap(canvas, bm, l, t);
    }
    DRAWING_LOGI("CanvasReadPixelsToBitmap::OnTestPerformance readPixels success=%{public}s", res ? "true" : "false");
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasDrawPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0); // 0, 0 表示路径的坐标
    TestRend rand = TestRend();
    OH_Drawing_PathLineTo(path, rand.nextRangeF(0, 720), rand.nextRangeF(0, 720)); // 720 用作上限
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_PathDestroy(path);
}

void CanvasRestoreToCount::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    fSave_Count = OH_Drawing_CanvasGetSaveCount(canvas);
    OH_Drawing_CanvasSave(canvas);
    TestRend rand;
    float x1 = rand.nextULessThan(bitmapWidth_);
    float y1 = rand.nextULessThan(bitmapHeight_);
    float x2 = rand.nextULessThan(bitmapWidth_);
    float y2 = rand.nextULessThan(bitmapHeight_);
    // 针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x1, y1, x2, y2);
    OH_Drawing_CanvasTranslate(canvas, x1, y1);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasRestoreToCount(canvas, fSave_Count);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasDrawPoints::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point2D pts[n];
    TestRend rand;
    for (int i = 0; i < n; i++) {
        pts[i].x = rand.nextULessThan(bitmapWidth_);
        pts[i].y = rand.nextULessThan(bitmapHeight_);
    }
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, n, pts);
    }
}

void CanvasDrawLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawLine(canvas, x1, y1, x2, y2);
    }
}

void CanvasDrawLayer::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    uint32_t colors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(x1, y1, x2, y2);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasSaveLayer(canvas, bounds, styleBrush_);
        OH_Drawing_CanvasClear(canvas, colors[i % 3]); // 3 为 color数组长度
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_RectDestroy(bounds);
    }
}

void CanvasDrawBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat); // 200 指定了位图（bitmap）的宽度和高度
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, x, y);
    }
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

void CanvasDrawBackground::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBackground(canvas, styleBrush_);
    }
}

void CanvasDrawRegion::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x1, y1, x2, y2);
        OH_Drawing_Region* region = OH_Drawing_RegionCreate();
        OH_Drawing_RegionSetRect(region, rect);
        OH_Drawing_CanvasDrawRegion(canvas, region);
        OH_Drawing_RegionDestroy(region);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawPixelMapRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 640, 640); // 0, 0, 640, 640  这些数字用于用于创建矩形
    // 无法获得 OH_Drawing_PixelMap
    // OH_Drawing_CanvasDrawPixelMapRect(OH_Drawing_Canvas *, OH_Drawing_PixelMap *, const OH_Drawing_Rect *src, const
    // OH_Drawing_Rect *dst, const OH_Drawing_SamplingOptions *)
    OH_Drawing_RectDestroy(rect);
}

void CanvasSave::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* Path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(Path, 0, 0); // 0, 0 这些数字用于用于设置路径
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasRestore(canvas);
    }
    float x = rand.nextULessThan(bitmapWidth_);
    float y = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_CanvasDrawLine(canvas, x, y, x - y, y - x);
    OH_Drawing_PathDestroy(Path);
}

void CanvasClipPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    float x = rand.nextULessThan(bitmapWidth_);
    float y = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_PathLineTo(path, x, y);
    OH_Drawing_CanvasDrawPath(canvas, path);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }
    OH_Drawing_PathDestroy(path);
}

void CanvasGetTotalMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l, t, r, b;
    l = rand.nextULessThan(bitmapWidth_);
    t = rand.nextULessThan(bitmapHeight_);
    r = l + rand.nextULessThan(bitmapWidth_);
    b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_Matrix* matrix_a = OH_Drawing_MatrixCreateRotation(45, 0, 0);    // 45为顺时针旋转角度
    OH_Drawing_Matrix* matrix_b = OH_Drawing_MatrixCreateTranslation(-10, -10); // -10为水平方向平移距离
    OH_Drawing_Matrix* total = OH_Drawing_MatrixCreate();
    OH_Drawing_CanvasSetMatrix(canvas, matrix_a);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix_b);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasGetTotalMatrix(canvas, total);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MatrixDestroy(matrix_a);
    OH_Drawing_MatrixDestroy(matrix_b);
    OH_Drawing_MatrixDestroy(total);
}

void CanvasRestore::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasSave(canvas);
    TestRend rand;
    OH_Drawing_CanvasScale(canvas, 0.1, 0.1); // 0.1, 0.1 缩放比例
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasRestore(canvas);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 300, 300); // 300, 300 创建矩形
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasGetLocalClipBounds::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 700, 700);      // 0, 0, 700, 700 创建矩形
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100, 100, 300, 300); // 100, 100, 300, 300 剪切矩形
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(400, 400, 700, 700); // 400, 400, 700, 700 剪切矩形
    OH_Drawing_CanvasClipRect(canvas, rect1, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasGetLocalClipBounds(canvas, rect);
    }
    OH_Drawing_CanvasClipRect(canvas, rect2, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect2);
}

void CanvasGetSaveCount::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    OH_Drawing_CanvasScale(canvas, 5, 5);                          //  5, 5 缩放因子
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasScale(canvas, 0.1, 0.1); //  0.1, 0.1 缩放因子
    OH_Drawing_CanvasSave(canvas);
    uint32_t fSave_Count = 0;
    for (int i = 0; i < testCount_; i++) {
        fSave_Count = OH_Drawing_CanvasGetSaveCount(canvas);
    }
    OH_Drawing_CanvasRestoreToCount(canvas, fSave_Count);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasGetWidth::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    int32_t canvas_width = 0;
    for (int i = 0; i < testCount_; i++) {
        canvas_width = OH_Drawing_CanvasGetWidth(canvas);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, canvas_width, 100); // 100 矩形高度
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void CanvasGetHeight::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    int32_t canvas_height = 0;
    for (int i = 0; i < testCount_; i++) {
        canvas_height = OH_Drawing_CanvasGetHeight(canvas);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, canvas_height); // 100 矩形宽度
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}