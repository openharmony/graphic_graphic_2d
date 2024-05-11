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
#include "luma_filter.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_font_mgr.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_typeface.h>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 600, // 600 是位图宽度
    K_H = 420, // 420 是位图高度
};

LumaFilter::LumaFilter()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "lumafilter";
}

uint32_t ColorSetA(uint32_t c, uint8_t a)
{
    return (c & 0x00FFFFFF) | (a << 24); // 24  ColorSetA
}

static float g_kSize = 80;  // 80 是大小
static float g_kInset = 10; // 10 用于控制内边距

uint32_t kColor1 = 0xFFFFFF00; // 0xFFFFFF00 用于绘制图形
uint32_t kColor2 = 0xFF82FF00; // 0xFF82FF00 用于绘制图形

OH_Drawing_BlendMode g_modes[] = {
    OH_Drawing_BlendMode::BLEND_MODE_SRC_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_DST_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_DST_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_IN,
    OH_Drawing_BlendMode::BLEND_MODE_DST_IN,
};

typedef struct {
    OH_Drawing_ShaderEffect* fShader1;
    OH_Drawing_ShaderEffect* fShader2;
} ST_SHADER;

const char* g_modeStrings[] = { "Clear", "Src", "Dst", "SrcOver", "DstOver", "SrcIn", "DstIn", "SrcOut", "DstOut",
    "SrcATop", "DstATop", "Xor", "Plus", "Modulate", "Screen", "Overlay", "Darken", "Lighten", "ColorDodge",
    "ColorBurn", "HardLight", "SoftLight", "Difference", "Exclusion", "Multiply", "Hue", "Saturation", "Color",
    "Luminosity" };

void draw_label(OH_Drawing_Canvas* canvas, const char* label, OH_Drawing_Point2D offset)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface* type = OH_Drawing_TypefaceCreateDefault();
    // 默认字体不一致
    OH_Drawing_FontSetTypeface(font, type);
    size_t len = strlen(label);

    // 缺乏计算文本宽度的接口，OH_Drawing_FontCountText临时替代
    int width = OH_Drawing_FontCountText(font, label, len, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(label, len, font, TEXT_ENCODING_UTF8);

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, offset.x - width / 2, offset.y); // 2 CanvasDrawTextBlob参数

    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(type);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void draw_clip(OH_Drawing_Canvas* canvas, DrawRect& c, OH_Drawing_Rect* rect, uint32_t kColor, OH_Drawing_Brush* brush)
{
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_Rect* cRect = OH_Drawing_RectCreate(c.left, c.top, c.right, c.bottom);
    OH_Drawing_CanvasClipRect(canvas, cRect, OH_Drawing_CanvasClipOp::INTERSECT, false);
    OH_Drawing_BrushSetColor(brush, kColor);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_RectDestroy(cRect);
}

void draw_scene_oval(OH_Drawing_Canvas* canvas, OH_Drawing_Brush* brush, DrawRect bounds, OH_Drawing_ShaderEffect* s1)
{
    DrawRect r = bounds;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetColor(brush, 0x200000FF);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_CanvasSaveLayer(canvas, rect, nullptr);
    OH_Drawing_RectDestroy(rect);
    r = bounds;
    r.Inset(g_kInset, 0); // 0 bounds
    rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_BrushSetShaderEffect(brush, s1);
    OH_Drawing_BrushSetColor(brush, s1 ? 0xFF000000 : ColorSetA(kColor1, 0x80));
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
}

void draw_scene(OH_Drawing_Canvas* canvas, OH_Drawing_ColorFilter* cFilter, OH_Drawing_BlendMode mode,
    OH_Drawing_ShaderEffect* s1, OH_Drawing_ShaderEffect* s2)
{
    DrawRect bounds = { 0, 0, g_kSize, g_kSize }; //  0, 0 bounds
    DrawRect r = bounds;
    DrawRect c = bounds;
    c.right = bounds.CenterX();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    draw_scene_oval(canvas, brush, bounds, s1);
    if (!s1) {
        draw_clip(canvas, c, rect, kColor1, brush);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_Brush* xferBrush = OH_Drawing_BrushCreate();
    r = bounds;
    rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_BrushSetBlendMode(xferBrush, mode);
    OH_Drawing_CanvasSaveLayer(canvas, rect, xferBrush);
    r = bounds;
    r.Inset(0, g_kInset);
    rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_BrushSetShaderEffect(brush, s2);
    OH_Drawing_BrushSetColor(brush, s2 ? 0xFF000000 : ColorSetA(kColor2, 0x80));
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetColorFilter(filter, cFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    if (!s2) {
        draw_clip(canvas, c, rect, kColor2, brush);
    }
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_BrushDestroy(xferBrush);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasRestore(canvas);
}

void LumaFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    uint32_t g1Colors[] = { kColor1, ColorSetA(kColor1, 0x20) };
    uint32_t g2Colors[] = { kColor2, ColorSetA(kColor2, 0x20) };
    OH_Drawing_Point* g1Points[] = { OH_Drawing_PointCreate(0, 0),
        OH_Drawing_PointCreate(0, 100) }; // 0, 0 ， 0, 100 PointCreate
    OH_Drawing_Point* g2Points[] = { OH_Drawing_PointCreate(0, 0),
        OH_Drawing_PointCreate(g_kSize, 0) }; // 0 PointCreate
    float pos[] = { 0.2f, 1.0f }; // 0.2f, 1.0f 定义了渐变效果中每种颜色在渐变中的相对位置
    OH_Drawing_ColorFilter* fFilter = OH_Drawing_ColorFilterCreateLuma();
    OH_Drawing_ShaderEffect* fGr1 = OH_Drawing_ShaderEffectCreateLinearGradient(
        g1Points[0], g1Points[1], g1Colors, pos, 2, OH_Drawing_TileMode::CLAMP); // 2 定义渐变效果中颜色的数量。
    OH_Drawing_ShaderEffect* fGr2 = OH_Drawing_ShaderEffectCreateLinearGradient(
        g2Points[0], g2Points[1], g2Colors, pos, 2, OH_Drawing_TileMode::CLAMP); // 2 定义渐变效果中颜色的数量。

    ST_SHADER shaders[] = {
        { nullptr, nullptr },
        { nullptr, fGr2 },
        { fGr1, nullptr },
        { fGr1, fGr2 },
    };
    float gridStep = g_kSize + 2 * g_kInset;
    size_t modes_size = 6; // 6 定义了modes数组的大小
    for (size_t i = 0; i < modes_size; ++i) {
        OH_Drawing_Point2D offset = { gridStep * (0.5f + i), 20 }; // 20 offset
        draw_label(canvas, g_modeStrings[g_modes[i]], offset);
    }
    size_t shaders_size = 4; // 4 定义了shaders数组的大小
    for (size_t i = 0; i < shaders_size; ++i) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, g_kInset, gridStep * i + 30); // 30 用于控制画布垂直平移的距离
        for (size_t m = 0; m < modes_size; ++m) {
            draw_scene(canvas, fFilter, g_modes[m], shaders[i].fShader1, shaders[i].fShader2);
            OH_Drawing_CanvasTranslate(canvas, gridStep, 0); // 0   CanvasTranslate参数
        }
        OH_Drawing_CanvasRestore(canvas);
    }
    OH_Drawing_ColorFilterDestroy(fFilter);
    OH_Drawing_ShaderEffectDestroy(fGr1);
    OH_Drawing_ShaderEffectDestroy(fGr2);
    for (int i = 0; i < 2; i++) { // 2 point 数组长度
        OH_Drawing_PointDestroy(g1Points[i]);
        OH_Drawing_PointDestroy(g2Points[i]);
    }
}
