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
#include "test_common.h"
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include "common/log_common.h"

union drawFloatIntUnion {
    float   fFloat;
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
    return TMUL*seed + TADD;
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
    a = 30345*(a & 0xffff) + (a >> 16); // 16, 30345 for rand
    b = 18000*(b & 0xffff) + (b >> 16); // 16, 18000 for rand
    return (((a << 16) | (a >> 16)) + b); // 16 for rand
}

float_t TestRend::nextUScalar1()
{
    return (this->nextU()>>16) * 1.52587890625e-5f; // 1.52587890625e-5f:int to float, high 16 bits
}

uint32_t TestRend::nextULessThan(uint32_t count)
{
    uint32_t min = 0;
    uint32_t max = count-1;
    uint32_t range = max-min+1;
    if (0 == range) {
        return this->nextU();
    } else {
        return (min+this->nextU()%range);
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

uint32_t color_to_565(uint32_t color)
{
    //这个接口按照32位图转565格式，再用32位来显示的方式运行，但是和dm的出图有色差 参见 addarc case
    uint8_t r = (color >> 16) & 0xFF;  // 16 for r
    uint8_t g = (color >> 8) & 0xFF; // 8 for g
    uint8_t b = color & 0xFF;

    // 预乘 RGB 分量  由于目前代码中alpha为255 ，所以忽略此过程， dm中有此过程，代码不同
    // 将RGB通道的值缩放到565格式
    r = (r >> 3) & 0x1F; // 3:8位到5位
    g = (g >> 2) & 0x3F; // 2:8位到6位
    b = (b >> 3) & 0x1F; // 3:8位到5位

    uint16_t rgb565 = (r << 11) | (g << 5) | b; // 11:r, 5:g
    
    //还原成32位用于api绘制，其中色彩信息在压缩过程中有所缺失
    uint8_t r2 = (rgb565 >> 11) & 0x1F; // 11:r
    uint8_t g2 = (rgb565 >> 5) & 0x3F; // 5:g
    uint8_t b2 = rgb565 & 0x1F;
    
    r2 = r2<<3 | r2>>3; // 3: 8 to 565
    g2 = g2<<2 | g2>>2; // 2: 8 to 565
    b2 = b2<<3 | b2>>3; // 3: 8 to 565
    
    uint32_t argb = 0xFF000000 | (r2<<16) | (g2<<8) | b2; // 16, 8 : argb
    return  argb;
}

OH_Drawing_Rect* DrawCreateRect(DrawRect r)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    return rect;
}

void DrawPathAddCircle(OH_Drawing_Path* path, float centerX, float centerY, float radius)
{
    OH_Drawing_Rect *rc = DrawCreateRect({centerX - radius, centerY - radius, centerX + radius, centerY + radius});
    OH_Drawing_PathAddArc(path, rc, 0, 360); // 0,360 arc param
    OH_Drawing_RectDestroy(rc);
}

uint8_t* DrawBitmapGetAddr8(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint8_t* ptr = (uint8_t*)pixel + (size_t)y*W +x;
    return ptr;
}

uint16_t* DrawBitmapGetAddr16(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint16_t* ptr = (uint16_t*)pixel + (uint16_t)(y*W) +(x);
    return ptr;
}

uint32_t* DrawBitmapGetAddr32(OH_Drawing_Bitmap* bitmap, int x, int y)
{
    uint32_t H = OH_Drawing_BitmapGetHeight(bitmap);
    uint32_t W = OH_Drawing_BitmapGetWidth(bitmap);
    void* pixel = OH_Drawing_BitmapGetPixels(bitmap);
    uint32_t* ptr = (uint32_t*)pixel + (uint32_t)(y*W) +(x);
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


bool ConvertStringFromJsValue(napi_env env, napi_value jsValue, std::string &value)
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

bool ConvertIntFromJsValue(napi_env env, napi_value jsValue, uint32_t &value)
{
    return napi_get_value_uint32(env, jsValue, &value) == napi_ok;
}