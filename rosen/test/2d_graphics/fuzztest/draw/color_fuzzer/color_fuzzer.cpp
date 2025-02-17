/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "color_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/**
 * 该函数主要测试了 Color 的下列方法：
 * - SetRgb(...)
 * - GetAlpha()
 * - GetRed()
 * - GetGreen()
 * - GetBlue()
 * - SetColorQuad(...)
 */
bool ColorFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    bool isAntiAlias = GetObject<bool>();
    Brush brush;
    brush.SetAntiAlias(isAntiAlias);
    Color color;
    color.SetRgb(red, green, blue, alpha);
    color.GetAlpha();
    color.GetRed();
    color.GetGreen();
    color.GetBlue();
    brush.SetColor(color);

    uint32_t colorValue = GetObject<uint32_t>();
    color.SetColorQuad(colorValue);
    brush.SetColor(color);
    return true;
}

/*
 * 测试了以下 Color 接口：
 * 1. 构造函数：
 *    - Color()
 *    - Color(const Color&)
 *    - Color(uint32_t, uint32_t, uint32_t, uint32_t)
 *    - Color(ColorQuad)
 * 2. SetRed() / SetGreen() / SetBlue() / SetAlpha()
 * 3. operator== / operator!=
 */
bool ColorFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();

    Color colorOne = Color();
    colorOne.SetRed(red);
    colorOne.SetGreen(green);
    colorOne.SetBlue(blue);
    colorOne.SetAlpha(alpha);
    Color colorTwo = Color(colorOne);
    Color colorThree = Color(red, green, blue, alpha);
    ColorQuad rgba = GetObject<ColorQuad>();
    Color colorFour = Color(rgba);
    if (colorOne == colorTwo) {}
    if (colorOne != colorTwo) {}
    return true;
}

/*
 * 测试了以下 Color 接口：
 * 1. SetRedF() / SetGreenF() / SetBlueF() / SetAlphaF()
 * 2. GetRedF() / GetGreenF() / GetBlueF() / GetAlphaF()
 * 3. GetColor4f()
 * 4. SetRgbF(...)
 * 5. CastToColorQuad()
 */
bool ColorFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float alpha = GetObject<float>();
    float red = GetObject<float>();
    float blue = GetObject<float>();
    float green = GetObject<float>();
    Color color;
    color.SetRedF(red);
    color.SetGreenF(green);
    color.SetBlueF(blue);
    color.SetAlphaF(alpha);
    color.GetRedF();
    color.GetGreenF();
    color.GetBlueF();
    color.GetAlphaF();
    color.GetColor4f();
    color.SetRgbF(red, green, blue, alpha);
    color.CastToColorQuad();
    return true;
}

/*
 * 测试了以下 Color 接口：
 * 1. ColorQuadSetARGB(...)
 * 2. ColorQuadGetA(...)
 * 3. ColorQuadGetR(...)
 * 4. ColorQuadGetG(...)
 * 5. ColorQuadGetB(...)
 */
bool ColorFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color;
    color.ColorQuadSetARGB(alpha, red, green, blue);
    ColorQuad colorQuad = GetObject<ColorQuad>();
    color.ColorQuadGetA(colorQuad);
    color.ColorQuadGetR(colorQuad);
    color.ColorQuadGetG(colorQuad);
    color.ColorQuadGetB(colorQuad);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ColorFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ColorFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ColorFuzzTest003(data, size);
    OHOS::Rosen::Drawing::ColorFuzzTest004(data, size);
    return 0;
}
