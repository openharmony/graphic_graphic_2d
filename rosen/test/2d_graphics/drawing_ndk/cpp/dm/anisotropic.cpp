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
#include "anisotropic.h"
#include <linux/pcitest.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_surface.h>
#include "test_common.h"
#include "common/log_common.h"

enum SkAlphaType : int {
    K_UNKNOWN_SK_ALPHA_TYPE,                              // !< uninitialized
    K_OPAQUE_SK_ALPHA_TYPE,                               // !< pixel is opaque
    K_PREMUL_SK_ALPHA_TYPE,                               // !< pixel components are premultiplied by alpha
    K_UNPREMUL_SK_ALPHA_TYPE,                             // !< pixel components are independent of alpha
    K_LAST_ENUM_SK_ALPHA_TYPE = K_UNPREMUL_SK_ALPHA_TYPE, // !< last valid value
};

float g_scales[] = {0.9f, 0.8f, 0.75f, 0.6f, 0.5f, 0.4f, 0.25f, 0.2f, 0.1f};

void Anisotropic::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap, kImageSize, kImageSize, &cFormat);
    OH_Drawing_Canvas *bimap_canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bimap_canvas, bitmap);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);

    OH_Drawing_CanvasClear(bimap_canvas, 0xFFFFFFFF);

    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(bimap_canvas, pen);
    OH_Drawing_CanvasTranslate(bimap_canvas, kImageSize / 2.0f, kImageSize / 2.0f); // 2.0f距离
    for (int i = 0; i < kNumLines; ++i, angle += kAngleStep) {
        float sin = sinf(angle);
        float cos = cosf(angle);
        OH_Drawing_CanvasDrawLine(bimap_canvas, cos * kInnerOffset, sin * kInnerOffset,
            cos * kImageSize / 2, sin * kImageSize / 2); // 2 cout
    }

    OH_Drawing_SamplingOptions *fsampling = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_LINEAR);

    for (int i = 0; i < sizeof(g_scales); ++i) {
        int height = (int)(OH_Drawing_BitmapGetHeight(bitmap) * g_scales[i]);
        if (i <= static_cast<int>(sizeof(g_scales) / sizeof(g_scales[0])) / 2) { // 2cout
            yOff = kSpacer + i * (OH_Drawing_BitmapGetHeight(bitmap) + kSpacer);
        } else {
            yOff = (sizeof(g_scales) - i) * (OH_Drawing_BitmapGetHeight(bitmap) + kSpacer) - height;
        }

        OH_Drawing_Rect *rect =
            OH_Drawing_RectCreate((float)kSpacer, (float)yOff, (float)OH_Drawing_BitmapGetWidth(bitmap), (float)height);
        OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
        OH_Drawing_CanvasDrawImageRect(canvas, image, rect, fsampling);

        OH_Drawing_CanvasDrawRect(canvas, rect);
    }

    for (int i = 0; i < sizeof(g_scales); ++i) {
        int width = (int)(OH_Drawing_BitmapGetWidth(bitmap) * g_scales[i]);
        if (i <= static_cast<int>(sizeof(g_scales) / sizeof(g_scales[0])) / 2) { // 2被除数
            xOff = OH_Drawing_BitmapGetWidth(bitmap) + 2 * kSpacer;              // 2 cout
            yOff = kSpacer + i * (OH_Drawing_BitmapGetHeight(bitmap) + kSpacer);
        } else {
            xOff = OH_Drawing_BitmapGetWidth(bitmap) + 2 * kSpacer + OH_Drawing_BitmapGetWidth(bitmap) - width; // 2cout
            yOff = kSpacer + (sizeof(g_scales) - i - 1) * (OH_Drawing_BitmapGetHeight(bitmap) + kSpacer);
        }

        OH_Drawing_Rect *rect =
            OH_Drawing_RectCreate((float)kSpacer, (float)yOff, (float)width, (float)OH_Drawing_BitmapGetHeight(bitmap));
        OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
        OH_Drawing_CanvasDrawImageRect(canvas, image, rect, fsampling);
        OH_Drawing_CanvasDrawRect(canvas, rect);
    }
    OH_Drawing_BitmapDestroy(bitmap);
}
