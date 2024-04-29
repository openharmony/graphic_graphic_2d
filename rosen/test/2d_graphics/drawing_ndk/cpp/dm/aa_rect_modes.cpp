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
#include "aa_rect_modes.h"
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_common.h"
#include "common/log_common.h"

static void test4(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Point2D pts[] = {
        {10, 160}, {610, 160}, {610, 160}, {10, 160},

        {610, 160},
        {610, 160},
        {610, 199},
        {610, 199},

        {10, 198},
        {610, 198},
        {610, 199},
        {10, 199},

        {10, 160},
        {10, 160},
        {10, 199},
        {10, 199}
    };
    char verbs[] = {
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4
    };
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Point2D *ptPtr = pts;
    for (size_t i = 0; i < sizeof(verbs); ++i) {
        switch (verbs[i]) {
            case 0: // SkPath::kMove_Verb:
                OH_Drawing_PathMoveTo(path, ptPtr->x, ptPtr->y);
                ++ptPtr;
                break;
            case 1: // 1 SkPath::kLine_Verb:
                OH_Drawing_PathLineTo(path, ptPtr->x, ptPtr->y);
                ++ptPtr;
                break;
            case 4: // 4 SkPath::kClose_Verb:
                OH_Drawing_PathClose(path);
                break;
            default:
                break;
        }
    }
    OH_Drawing_Rect *clip = OH_Drawing_RectCreate(0, 130, 772, 531); // 0, 130, 772, 531正方形
    OH_Drawing_CanvasClipRect(canvas, clip, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    OH_Drawing_PathDestroy(path);
    path = nullptr;
}

constexpr OH_Drawing_BlendMode gModes[] = {
    OH_Drawing_BlendMode::BLEND_MODE_CLEAR,
    OH_Drawing_BlendMode::BLEND_MODE_SRC,
    OH_Drawing_BlendMode::BLEND_MODE_DST,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_DST_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_IN,

    OH_Drawing_BlendMode::BLEND_MODE_DST_IN,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_OUT,
    OH_Drawing_BlendMode::BLEND_MODE_DST_OUT,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_DST_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_XOR,
};

const float W = 64;
const float H = 64;

static float drawCell(OH_Drawing_Canvas *canvas, OH_Drawing_BlendMode mode, uint8_t a0, uint8_t a1)
{
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);

    OH_Drawing_Rect *r = OH_Drawing_RectCreate(W / 10, H / 10, W - W / 10, H - H / 10); // 10正方形

    OH_Drawing_BrushSetColor(brush, 0xFF0000FF);
    OH_Drawing_BrushSetAlpha(brush, a0);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, r);

    OH_Drawing_BrushSetColor(brush, 0xFFFF0000);
    OH_Drawing_BrushSetAlpha(brush, a1);
    OH_Drawing_BrushSetBlendMode(brush, mode);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    float offset = 1.0 / 3;
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(W / 4 + offset, H / 4 + offset, W / 4 + offset + W / 2,
        H / 4 + offset + H / 2); // 4,2正方形
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(r);
    return H;
}

OH_Drawing_ShaderEffect *make_bg_shader()
{
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapBuild(bitmap, 2, 2, &format); // 2,2 width and height
    *DrawBitmapGetAddr32(bitmap, 0, 0) = 0xFFFFFFFF;
    *DrawBitmapGetAddr32(bitmap, 1, 1) = 0xFFFFFFFF;
    *DrawBitmapGetAddr32(bitmap, 1, 0) = 0xFFCECFCE;
    *DrawBitmapGetAddr32(bitmap, 0, 1) = 0xFFCECFCE;

    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);

    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateScale(6, 6, 0, 0);
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateImageShader(image, OH_Drawing_TileMode::REPEAT,
        OH_Drawing_TileMode::REPEAT, option, matrix);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_MatrixDestroy(matrix);

    return shaderEffect;
}

AARectModes::AARectModes()
{
    bitmapWidth_ = 640;  // 640宽度
    bitmapHeight_ = 480; // 480高度
    fileName_ = "aarectmodes";
}

AARectModes::~AARectModes() {}

void AARectModes::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Brush *bgBrush = OH_Drawing_BrushCreate();
    OH_Drawing_ShaderEffect *shaderEffect = make_bg_shader();
    OH_Drawing_BrushSetShaderEffect(bgBrush, shaderEffect);
    if (false)
        test4(canvas);

    OH_Drawing_Rect *bounds = OH_Drawing_RectCreate(0, 0, W, H);
    uint8_t gAlphaValue[] = {0xFF, 0x88, 0x88};
    OH_Drawing_CanvasTranslate(canvas, 4.0, 4.0); // 4.0距离

    for (int alpha = 0; alpha < 4; ++alpha) { // 4max
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasSave(canvas);

        for (size_t i = 0; i < 12; ++i) { //12 max
            if (6 == i) { // 6 cout
                OH_Drawing_CanvasRestore(canvas);
                OH_Drawing_CanvasTranslate(canvas, W * 5.0, 0); // 5.0距离
                OH_Drawing_CanvasSave(canvas);
            }
            OH_Drawing_CanvasAttachBrush(canvas, bgBrush);
            OH_Drawing_CanvasDrawRect(canvas, bounds);
            OH_Drawing_CanvasSaveLayer(canvas, bounds, nullptr);
            float dy = drawCell(canvas, gModes[i], gAlphaValue[alpha & 1], gAlphaValue[alpha & 2]);
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasTranslate(canvas, 0, dy * 5.0f / 4.0f); // dy * 5.0f / 4.0f距离
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, W * 5.0f / 4.0f, 0); // W * 5.0f / 4.0f 距离
    }
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_BrushDestroy(bgBrush);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
}
