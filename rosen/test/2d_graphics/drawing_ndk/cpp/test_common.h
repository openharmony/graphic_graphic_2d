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
#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "napi/native_api.h"
#include <native_drawing/drawing_types.h>
#include <string>
#include <bits/alltypes.h>

#define TMUL  1664525
#define TADD 1013904223

#define DRAW_COLORWHITE 0xFFFFFFFF
#define DRAW_COLORBLACK 0xFF000000
#define DRAW_COLORRED 0xFFFF0000
#define DRAW_COLORGREEN 0xFF00FF00
#define DRAW_COLORBLUE 0xFF0000FF
#define DRAW_COLORYELLOW 0xFFFFFF00
#define DRAW_COLORCYAN 0xFF00FFFF
#define DRAW_COLORMAGENTA 0xFFFF00FF
#define DRAW_COLORGRAY 0xFF808080
#define DRAW_COLORTRANSPARENT 0x00000000

class TestRend {
public:
    TestRend() {init(0);}
    explicit TestRend(uint32_t seed) { init(seed); }
    ~TestRend() = default;
    
    uint32_t nextU();
    float_t nextUScalar1();
    uint32_t nextULessThan(uint32_t count);
    float_t nextF();
    float_t nextRangeF(float_t min, float_t max);
    uint32_t nextBits(unsigned bitCount);
protected:
    void init(uint32_t seed);
    uint32_t next(uint32_t seed);

    uint32_t a;
    uint32_t b;
};

uint32_t color_to_565(uint32_t color);

struct DrawRect {
    float left;
    float top;
    float right;
    float bottom;
    bool Contains(float x, float y) const { return x >= left && x < right && y >= top && y < bottom; }
    float Width(){return (right - left);}
    float Height(){return (bottom - top);}
    float CenterX(){return (right - left) / 2;} // 2 for mid
    float CenterY(){return (bottom - top) / 2;} // 2 for mid
    bool Inset(float dx, float dy)
    {
        float l = left + dx;
        float t = top + dy;
        float r = right - dx;
        float b = bottom - dy;
        if((r <= l) || (b <= t)) {
            return false;
        }
        left = l;
        top = t;
        right = r;
        bottom = b;
        return true;
    }
    void Offset(float dx, float dy)
    {
        left += dx;
        top += dy;
        right += dx;
        bottom += dy;
    }
    void SetXYWH(float x, float y, float width, float height)
    {
        left = x;
        top = y;
        right = x + width;
        bottom = y + height;
    }
};

OH_Drawing_Rect* DrawCreateRect(DrawRect r);

void DrawPathAddCircle(OH_Drawing_Path* path, float centerX, float centerY, float radius);
uint8_t* DrawBitmapGetAddr8(OH_Drawing_Bitmap* bitmap, int x, int y);
uint16_t* DrawBitmapGetAddr16(OH_Drawing_Bitmap* bitmap, int x, int y);
uint32_t* DrawBitmapGetAddr32(OH_Drawing_Bitmap* bitmap, int x, int y);

void DrawPathGetBound(DrawRect& r, float x, float y);
#endif // TEST_COMMON_H