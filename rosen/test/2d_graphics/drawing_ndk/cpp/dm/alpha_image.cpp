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
#include "alpha_image.h"
#include <multimedia/image_framework/image_mdk_common.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_sampling_options.h>
#include "test_common.h"
#include "common/log_common.h"

enum {
    K_W = 152,
    K_H = 80,
};

OH_Drawing_ColorFilter *make_color_filter()
{
    float colorMatrix[20] = {
        1, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0.5, 0.5, 0,
        0, 0, 0.5, 0.5, 0
    }; // mix G and A.
    return OH_Drawing_ColorFilterCreateMatrix(colorMatrix);
}

OH_Drawing_Image *make_alpha_image(int w, int h)
{
    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_ALPHA_8,
                                       OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Canvas *bmpCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bmpCanvas, bm);
    OH_Drawing_CanvasClear(bmpCanvas, OH_Drawing_ColorSetArgb(10, 0, 0, 0)); // 10 color

    for (int y = 0; y < h; ++y) {
        for (int x = y; x < w; ++x) {
            *DrawBitmapGetAddr8(bm, x, y) = 0xFF;
        }
    }
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_CanvasDestroy(bmpCanvas);
    OH_Drawing_BitmapDestroy(bm);
    return image;
}

AlphaImage::AlphaImage()
{
    // file gm/fontregen.cpp
    bitmapWidth_ = 256;  // 256宽度
    bitmapHeight_ = 256; // 256高度
    fileName_ = "alpha_image";
}

AlphaImage::~AlphaImage() {}

void AlphaImage::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    auto image = make_alpha_image(kSize, kSize);

    OH_Drawing_MaskFilter *maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, 10.f, true);
    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    OH_Drawing_ColorFilter *colorFilter = make_color_filter();
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Rect *dst = DrawCreateRect({ 16, 16, 16 + kSize, 16 + kSize });
    OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);

    OH_Drawing_FilterSetColorFilter(filter, nullptr);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(144, 16);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(144 + kSize, 16 + kSize);
    uint32_t colors[] = {DRAW_COLORCYAN, DRAW_COLORCYAN};
    float pos[] = {0.f, 1.f};
    OH_Drawing_ShaderEffect *effect =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, pos, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    dst = DrawCreateRect({ 144, 16, 144 + kSize, 16 + kSize });
    OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_RectDestroy(dst);

    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    dst = DrawCreateRect({ 16, 144, 16 + kSize, 144 + kSize });
    OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
    OH_Drawing_RectDestroy(dst);

    OH_Drawing_FilterSetMaskFilter(filter, nullptr);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    dst = DrawCreateRect({ 144, 144, 144 + kSize, 144 + kSize });
    OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
    OH_Drawing_RectDestroy(dst);

    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}

AlphaImageAlphaTint::AlphaImageAlphaTint()
{
    // file gm/fontregen.cpp
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "alpha_image_alpha_tint";
}

AlphaImageAlphaTint::~AlphaImageAlphaTint() {}

void AlphaImageAlphaTint::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_CanvasClear(canvas, DRAW_COLORGRAY);
    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_ALPHA_8,
                                       OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            *DrawBitmapGetAddr8(bm, x, y) = y * 4; // 4 cout
        }
    }

    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();

    OH_Drawing_BrushSetColor(brush, 0x8000FF00);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasTranslate(canvas, 8, 8); // 8平移距离
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, option);

    OH_Drawing_CanvasTranslate(canvas, 72, 0); // 72平移距离
    OH_Drawing_ShaderEffect *effect = OH_Drawing_ShaderEffectCreateImageShader(image, OH_Drawing_TileMode::CLAMP,
        OH_Drawing_TileMode::CLAMP, option, nullptr);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_ShaderEffectDestroy(effect);

    brush = nullptr;
}
