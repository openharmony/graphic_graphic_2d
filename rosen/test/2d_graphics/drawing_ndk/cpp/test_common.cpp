/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "test_common.h"

#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <random>

#include "common/log_common.h"

union drawFloatIntUnion {
    float fFloat;
    int32_t fSignBitInt;
};

static inline float drawBits2Float(int32_t floatAsBits)
{
    drawFloatIntUnion data;
    data.fSignBitInt = floatAsBits;
    return data.fFloat;
}

uint32_t TestRend::next(uint32_t seed)
{
    return TMUL * seed + TADD;
}

void TestRend::init(uint32_t seed)
{
    a = next(seed);
    if (0 == a) {
        a = next(a);
    }
    b = next(a);
    if (0 == b) {
        b = next(b);
    }
}

uint32_t TestRend::nextU()
{
    a = 30345 * (a & 0xffff) + (a >> 16); // 16, 30345 for rand
    b = 18000 * (b & 0xffff) + (b >> 16); // 16, 18000 for rand
    return (((a << 16) | (a >> 16)) + b); // 16 for rand
}

uint32_t TestRend::nextRangeU(uint32_t min, uint32_t max)
{
    uint32_t range = max - min + 1;
    if (0 == range) {
        return this->nextU();
    } else {
        return (min + this->nextU() % range);
    }
}

float_t TestRend::nextUScalar1()
{
    return (this->nextU() >> 16) * 1.52587890625e-5f; // 1.52587890625e-5f:int to float, high 16 bits
}

uint32_t TestRend::nextULessThan(uint32_t count)
{
    uint32_t min = 0;
    uint32_t max = count - 1;
    uint32_t range = max - min + 1;
    if (0 == range) {
        return this->nextU();
    } else {
        return (min + this->nextU() % range);
    }
}

float_t TestRend::nextF()
{
    int i = (this->nextU() >> 9);
    int floatint = 0x3f800000 | i;
    float f = drawBits2Float(floatint) - 1.0f;
    return f;
}

float_t TestRend::nextRangeF(float_t min, float_t max)
{
    return this->nextUScalar1() * (max - min) + min;
}

uint32_t TestRend::nextBits(unsigned bitCount)
{
    return this->nextU() >> (32 - bitCount); // 32 : size
}

uint32_t CreateRandomNum(uint32_t min, uint32_t max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    uint32_t randomNumber = dis(gen);
    return randomNumber;
}

uint32_t color_to_565(uint32_t color)
{
    unsigned r = ((color >> 19) & 0x1F) << 11;
    unsigned g = ((color >> 10) & 0x3F) << 5;
    unsigned b = ((color >> 3) & 0x1F);

    uint16_t rgb565 = r | g | b;

    // 将RGB通道的值缩放到565格式
    unsigned r1 = ((unsigned)rgb565 >> 11) & 0x1F;
    unsigned g1 = ((unsigned)rgb565 >> 5) & 0x3F;
    unsigned b1 = (unsigned)rgb565 & 0x1F;

    unsigned r2 = (r1 << 3) | (r1 >> 2);
    unsigned g2 = (g1 << 2) | (g1 >> 4);
    unsigned b2 = (b1 << 3) | (b1 >> 2);

    uint32_t argb = (0xFF << 24) | (r2 << 16) | (g2 << 8) | b2;
    return argb;
}

OH_Drawing_Rect* DrawCreateRect(DrawRect r)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    return rect;
}

void DrawPathAddCircle(OH_Drawing_Path* path, float centerX, float centerY, float radius)
{
    OH_Drawing_Rect* rc = DrawCreateRect({ centerX - radius, centerY - radius, centerX + radius, centerY + radius });
    OH_Drawing_PathAddArc(path, rc, 0, 360); // 0,360 arc param
    OH_Drawing_RectDestroy(rc);
}

uint8_t* DrawBitmapGetAddr8(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint8_t* ptr = (uint8_t*)pixel + (size_t)y * W + x;
    return ptr;
}

uint16_t* DrawBitmapGetAddr16(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint16_t* ptr = (uint16_t*)pixel + (uint16_t)(y * W) + (x);
    return ptr;
}

uint32_t* DrawBitmapGetAddr32(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint32_t* ptr = (uint32_t*)pixel + (uint32_t)(y * W) + (x);
    return ptr;
}

void DrawPathGetBound(DrawRect& r, float x, float y)
{
    // 比最小的小，比最大的大就设置
    if (x < r.left) {
        r.left = x;
    }
    if (x > r.right) {
        r.right = x;
    }
    if (y < r.top) {
        r.top = y;
    }
    if (y > r.bottom) {
        r.bottom = y;
    }
}

bool ConvertStringFromJsValue(napi_env env, napi_value jsValue, std::string& value)
{
    size_t len = 0;
    if (napi_get_value_string_utf8(env, jsValue, nullptr, 0, &len) != napi_ok) {
        return false;
    }
    auto buffer = std::make_unique<char[]>(len + 1);
    size_t strLength = 0;
    if (napi_get_value_string_utf8(env, jsValue, buffer.get(), len + 1, &strLength) == napi_ok) {
        value = buffer.get();
        return true;
    }
    return false;
}

bool ConvertIntFromJsValue(napi_env env, napi_value jsValue, uint32_t& value)
{
    return napi_get_value_uint32(env, jsValue, &value) == napi_ok;
}

void DrawingFunc(OH_Drawing_Canvas* canvas)
{
    // 1.圆弧
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
    OH_Drawing_CanvasTranslate(canvas, 0, -100);                   // -100 for test
    OH_Drawing_CanvasDrawArc(canvas, rect, 0, 180);                // 180 for test
    OH_Drawing_RectDestroy(rect);
    // 2.圆形
    OH_Drawing_Point* point = OH_Drawing_PointCreate(450.f, 220.f); // 450.f, 220.f for test
    OH_Drawing_CanvasDrawCircle(canvas, point, 100);                // 100 for test
    OH_Drawing_PointDestroy(point);
    // 3.画线
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 2, 350);   // 2, 350 for test
    OH_Drawing_PathLineTo(path, 100, 450); // 100, 450 for test
    OH_Drawing_PathLineTo(path, 200, 350); // 200, 350 for test
    OH_Drawing_PathLineTo(path, 300, 450); // 300, 450 for test
    OH_Drawing_PathLineTo(path, 400, 350); // 400, 350 for test
    OH_Drawing_PathLineTo(path, 420, 450); // 420, 450 for test
    OH_Drawing_PathLineTo(path, 600, 350); // 600, 350 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
    // 4.字体
    const char* text = "RELIABILITY";
    size_t len = strlen(text);
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 75); // 75 for test
    OH_Drawing_TextBlob* textblob =
        OH_Drawing_TextBlobCreateFromText(text, len, font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasDrawTextBlob(canvas, textblob, 30, 540); // 30, 540 for test
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(textblob);
    // 5.OH_Drawing_CanvasDrawBitmapRect
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_Rect* src1 = OH_Drawing_RectCreate(0, 0, 700, 200);    // 0, 0, 700, 200 for test
    OH_Drawing_Rect* dst1 = OH_Drawing_RectCreate(10, 600, 710, 800); // 10, 600, 710, 800 for test
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_SamplingOptions* sampling = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    OH_Drawing_BitmapBuild(bitmap, 720, 720, &format); // 720, 720 for test
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, src1, dst1, sampling);
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_RectDestroy(src1);
    OH_Drawing_RectDestroy(dst1);
}