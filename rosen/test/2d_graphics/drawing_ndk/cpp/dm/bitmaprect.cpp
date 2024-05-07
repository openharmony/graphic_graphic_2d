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
#include "bitmaprect.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_sampling_options.h>
#include "test_common.h"
#include "common/log_common.h"

enum {
    K_W = 640,
    K_H = 480,
};

OH_Drawing_Bitmap *make_big_bitmap()
{
    int gXSize = 4096;
    int gYSize = 4096;
    int gBorderWidth = 10;

    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
                                       OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, gXSize, gXSize, &format);

    void *pixel = OH_Drawing_BitmapGetPixels(bm);
    uint32_t *ptr = (uint32_t *)pixel;

    for (int y = 0; y < gYSize; ++y) {
        for (int x = 0; x < gYSize; ++x) {
            if (x <= gBorderWidth || x >= gXSize - gBorderWidth || y <= gBorderWidth || y >= gYSize - gBorderWidth) {
                //                    *DrawBitmapGetAddr32(bm, x, y) = 0xFFFFFF88;
                ptr = (uint32_t *)pixel + (uint32_t)y * gYSize + (x);
                *ptr = 0xFFFFFF88;
            } else {
                //                    *DrawBitmapGetAddr32(bm, x, y) = 0xFF000088;
                ptr = (uint32_t *)pixel + (uint32_t)y * gYSize + (x);
                *ptr = 0xFF000088;
            }
        }
    }

    return bm;
}

DrawBitmapRect4::DrawBitmapRect4(bool useIRect) : fUseIRect(useIRect)
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = fUseIRect ? "bigbitmaprect_i" : "bigbitmaprect_s";
    fBigBitmap = make_big_bitmap();
}

DrawBitmapRect4::~DrawBitmapRect4() {}

void DrawBitmapRect4::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasClear(canvas, 0x88444444);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAlpha(brush, 128); // 128 透明度值
    OH_Drawing_BrushSetBlendMode(brush, OH_Drawing_BlendMode::BLEND_MODE_XOR);

    DrawRect srcR1 = { 0.0f, 0.0f, 4096.0f, 2040.0f };
    DrawRect dstR1 = { 10.1f, 10.1f, 629.9f, 400.9f };

    DrawRect srcR2 = { 4085.0f, 10.0f, 4087.0f, 12.0f };
    DrawRect dstR2 = { 10, 410, 30, 430 };

    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    if (!fUseIRect) {
        OH_Drawing_CanvasDrawBitmapRect(canvas, fBigBitmap, DrawCreateRect(srcR1), DrawCreateRect(dstR1), option);
        OH_Drawing_CanvasDrawBitmapRect(canvas, fBigBitmap, DrawCreateRect(srcR2), DrawCreateRect(dstR2), option);
    } else {
        DRAWING_LOGI("OnTestFunction  DrawBitmapRect4 rect::rountout接口缺失");
    }
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}
