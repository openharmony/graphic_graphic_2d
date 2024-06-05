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

#include "hm_symbol_txt_fuzzer.h"
#include <cstddef>
#include "get_object.h"
#include "rosen_text/hm_symbol_txt.h"
#include "symbol_engine/hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
void HmSymbolTxtFuzzTestInner01(HMSymbolTxt& symbol)
{
    // Simulation input color data
    U8CPU red1 = SPText::GetObject<U8CPU>();
    U8CPU gree1 = SPText::GetObject<U8CPU>();
    U8CPU blue1 = SPText::GetObject<U8CPU>();
    float alpha1 = SPText::GetObject<float>();
    RSSColor color1 = {alpha1, red1, gree1, blue1};
    std::vector<RSSColor> colors1 = {color1};
    symbol.SetRenderColor(color1);
    symbol.SetRenderColor(colors1);

    Drawing::Color color2 = Drawing::Color::COLOR_BLUE;
    Drawing::Color color3 = Drawing::Color::COLOR_GREEN;
    std::vector<Drawing::Color> colors2 = {color2, color3};
    symbol.SetRenderColor(color2);
    symbol.SetRenderColor(colors2);

    // Simulation input data with attributes of symbol
    uint32_t renderMode = SPText::GetObject<uint32_t>();
    uint32_t effectStrategy = SPText::GetObject<uint32_t>();
    uint16_t animationMode = SPText::GetObject<uint16_t>();
    int repeatCount = SPText::GetObject<int>();
    bool animationStart = SPText::GetObject<bool>();
    Drawing::DrawingCommonSubType commonSubType = SPText::GetObject<Drawing::DrawingCommonSubType>();
    VisualMode visual = SPText::GetObject<VisualMode>();
    symbol.SetRenderMode(renderMode);
    symbol.SetSymbolEffect(effectStrategy);
    symbol.SetAnimationMode(animationMode);
    symbol.SetRepeatCount(repeatCount);
    symbol.SetAnimationStart(animationStart);
    symbol.SetCommonSubType(commonSubType);
    symbol.SetVisualMode(visual);
}

void HmSymbolTxtFuzzTestInner02(HMSymbolTxt& symbol1, SPText::HMSymbolTxt& symbol2)
{
    // test data transfer
    symbol2.SetRenderColor(symbol1.GetRenderColor());
    symbol2.SetRenderMode(symbol1.GetRenderMode());
    symbol2.SetSymbolEffect(symbol1.GetEffectStrategy());
    symbol2.SetAnimationMode(symbol1.GetAnimationMode());
    symbol2.SetRepeatCount(symbol1.GetRepeatCount());
    symbol2.SetAnimationStart(symbol1.GetAnimationStart());
    symbol2.SetCommonSubType(symbol1.GetCommonSubType());
}

// test operator == with the color
void HmSymbolTxtFuzzTestInner03(SPText::HMSymbolTxt& symbol1, SPText::HMSymbolTxt& symbol2)
{
    // Simulation input color data
    U8CPU red = SPText::GetObject<U8CPU>();
    U8CPU gree = SPText::GetObject<U8CPU>();
    U8CPU blue = SPText::GetObject<U8CPU>();
    float alphaF = SPText::GetObject<float>();
    RSSColor color = {alphaF, red, gree, blue};
    std::vector<RSSColor> colors = {};
    symbol2.SetRenderColor(colors);
    symbol1.SetRenderColor(color);
    if (symbol1 == symbol2) {
        return;
    }

    RSSColor color1;
    symbol1.SetRenderColor(color1);
    color1.r = red;
    symbol2.SetRenderColor(color1);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderColor(color1);
    color1.g = gree;
    symbol2.SetRenderColor(color1);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderColor(color1);
    color1.b = blue;
    symbol2.SetRenderColor(color1);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderColor(color1);
    color1.a = alphaF;
    symbol2.SetRenderColor(color1);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderColor(color1);
    symbol2.SetRenderColor(color1);
    if (symbol1 == symbol2) {
        return;
    }
}

// test operator == with the color RenderMode and SymbolEffect
void HmSymbolTxtFuzzTestInner04()
{
    SPText::HMSymbolTxt symbol1;
    SPText::HMSymbolTxt symbol2;
    RSSymbolRenderingStrategy renderMode1 = SPText::GetObject<RSSymbolRenderingStrategy>();
    RSSymbolRenderingStrategy renderMode2 = SPText::GetObject<RSSymbolRenderingStrategy>();

    RSEffectStrategy effectStrategy1 = SPText::GetObject<RSEffectStrategy>();
    RSEffectStrategy effectStrategy2 = SPText::GetObject<RSEffectStrategy>();

    symbol1.SetRenderMode(renderMode1);
    symbol2.SetRenderMode(renderMode2);
    symbol1.SetSymbolEffect(effectStrategy1);
    symbol2.SetSymbolEffect(effectStrategy2);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderMode(renderMode1);
    symbol2.SetRenderMode(renderMode1);
    symbol1.SetSymbolEffect(effectStrategy1);
    symbol2.SetSymbolEffect(effectStrategy2);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderMode(renderMode1);
    symbol2.SetRenderMode(renderMode2);
    symbol1.SetSymbolEffect(effectStrategy1);
    symbol2.SetSymbolEffect(effectStrategy1);
    if (symbol1 == symbol2) {
        return;
    }

    symbol1.SetRenderMode(renderMode1);
    symbol2.SetRenderMode(renderMode1);
    symbol1.SetSymbolEffect(effectStrategy1);
    symbol2.SetSymbolEffect(effectStrategy1);
    if (symbol1 == symbol2) {
        return;
    }
}

bool HmSymbolTxtFuzzTest01(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    SPText::g_data = data;
    SPText::g_size = size;
    SPText::g_pos = 0;

    HMSymbolTxt symbol1;
    SPText::HMSymbolTxt symbol2;
    SPText::HMSymbolTxt symbol3;

    // test interface functins of symbol
    HmSymbolTxtFuzzTestInner01(symbol1);

    // test data transfer
    HmSymbolTxtFuzzTestInner02(symbol1, symbol2);
    return true;
}

bool HmSymbolTxtFuzzTest02(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    SPText::g_data = data;
    SPText::g_size = size;
    SPText::g_pos = 0;

    SPText::HMSymbolTxt symbol1;
    SPText::HMSymbolTxt symbol2;
    symbol2.SetRenderColor(symbol1.GetRenderColor());
    symbol2.SetRenderMode(symbol1.GetRenderMode());
    symbol2.SetSymbolEffect(symbol1.GetEffectStrategy());
    symbol2.SetAnimationMode(symbol1.GetAnimationMode());
    symbol2.SetRepeatCount(symbol1.GetRepeatCount());
    symbol2.SetAnimationStart(symbol1.GetAnimationStart());
    symbol2.SetCommonSubType(symbol1.GetCommonSubType());

    // test operator == with the color
    HmSymbolTxtFuzzTestInner03(symbol1, symbol2);

    // test operator == with the color RenderMode and SymbolEffect
    HmSymbolTxtFuzzTestInner04();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::HmSymbolTxtFuzzTest01(data, size);
    OHOS::Rosen::HmSymbolTxtFuzzTest02(data, size);
    return 0;
}
