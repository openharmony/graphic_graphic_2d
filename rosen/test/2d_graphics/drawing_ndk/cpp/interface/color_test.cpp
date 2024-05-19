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

#include "color_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_color_space.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

void ColorFilterCreateBlendMode::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0x11, BLEND_MODE_DST_ATOP);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0x11, BLEND_MODE_DST_ATOP);
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_RectDestroy(rect);
}

void ColorFilterCreateCompose::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_ColorFilter* colorFilter1 = OH_Drawing_ColorFilterCreateBlendMode(0xFF, BLEND_MODE_DST_ATOP);
    OH_Drawing_ColorFilter* colorFilter2 = OH_Drawing_ColorFilterCreateBlendMode(0x11, BLEND_MODE_DST_ATOP);
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateCompose(colorFilter1, colorFilter2);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateCompose(colorFilter1, colorFilter2);
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_ColorFilterDestroy(colorFilter1);
    OH_Drawing_ColorFilterDestroy(colorFilter2);
}

void ColorFilterCreateMatrix::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    float colorMatrix[20] = { 1, 0, 0, 0, 0, // 1, 0, 0, 0, 0, 颜色矩阵参数
        0, 1, 0, 0, 0,                       // 0, 1, 0, 0, 0, 颜色矩阵参数
        0, 0, 0.5, 0.5, 0,                   // 0, 0, 0.5, 0.5, 0, 颜色矩阵参数
        0, 0, 0.5, 0.5, 0 };                 // 0, 0, 0.5, 0.5, 0 颜色矩阵参数
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateMatrix(colorMatrix);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateMatrix(colorMatrix);
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

void ColorFilterCreateLinearToSrgbGamma::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

void ColorFilterCreateSrgbGammaToLinear::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

void ColorFilterCreateLuma::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_ColorFilter* colorFilter;

    for (int i = 0; i < testCount_; i++) {
        colorFilter = OH_Drawing_ColorFilterCreateLuma();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    colorFilter = OH_Drawing_ColorFilterCreateLuma();
    OH_Drawing_FilterSetColorFilter(styleFilter_, colorFilter);
    OH_Drawing_PenSetFilter(stylePen_, styleFilter_);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

// 接口使用方法暂未开放
void ColorSpaceCreateSrgb::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
        OH_Drawing_ColorSpaceDestroy(colorSpace);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

// 接口使用方法暂未开放
void ColorSpaceCreateSrgbLinear::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_) + __LINE__;
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgbLinear();
        OH_Drawing_ColorSpaceDestroy(colorSpace);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}
