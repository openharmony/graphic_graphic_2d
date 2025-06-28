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

#include "hm_symbol_run_fuzzer.h"

#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>

#include "symbol_engine/hm_symbol_run.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
void DrawSymbolFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    const char* str1 = "A";
    Drawing::Font font;
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);

    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsety = fdp.ConsumeFloatingPoint<float>();
    RSPoint paint = { offsetX, offsety };
    RSEffectStrategy effectStrategy = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());
    HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolEffect(effectStrategy);

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint);

    // test the TextBlob is nullptr
    textblob1 = nullptr;
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint);

    // test the multiple glyphs
    const char* str2 = "Test multiple glyphs";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    HMSymbolRun hmSymbolRun1 = HMSymbolRun(1, symbolTxt, textblob2, animationFunc);
    hmSymbolRun1.DrawSymbol(rsCanvas.get(), paint);

    const char* str3 = "B";
    Drawing::Font font1;
    auto textblob = Drawing::TextBlob::MakeFromText(str3, strlen(str3), font, Drawing::TextEncoding::UTF8);
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc1 =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    uint64_t symbolId = fdp.ConsumeIntegral<uint64_t>();
    HMSymbolRun hmSymbolRun2 = HMSymbolRun(symbolId, symbolTxt, textblob, animationFunc1);
    hmSymbolRun2.SetAnimationStart(true);
    RSEffectStrategy effectStrategy1 = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());
    hmSymbolRun2.SetSymbolEffect(effectStrategy1);
    hmSymbolRun2.DrawSymbol(rsCanvas.get(), paint);
}

void HmSymbolRunFuzzTestInnerSetAttributes(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    const char* str1 = "A";
    Drawing::Font font;
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);

    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsetY = fdp.ConsumeFloatingPoint<float>();
    RSPoint paint = { offsetX, offsetY };
    RSEffectStrategy effectStrategy1 = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());
    HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolEffect(effectStrategy1);

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc = nullptr;

    uint64_t symbolId = fdp.ConsumeIntegral<uint64_t>();
    HMSymbolRun hmSymbolRun = HMSymbolRun(symbolId, symbolTxt, textblob1, animationFunc);

    U8CPU red1 = fdp.ConsumeIntegral<U8CPU>();
    U8CPU gree1 = fdp.ConsumeIntegral<U8CPU>();
    U8CPU blue1 = fdp.ConsumeIntegral<U8CPU>();
    float alpha1 = fdp.ConsumeFloatingPoint<float>();
    RSSColor color1 = { alpha1, red1, gree1, blue1 };
    std::vector<RSSColor> colors1 = { color1 };
    hmSymbolRun.SetRenderColor(colors1);

    // Simulation input data with attributes of symbol
    RSSymbolRenderingStrategy renderMode =  static_cast<RSSymbolRenderingStrategy>(fdp.ConsumeIntegral<uint32_t>());
    RSEffectStrategy effectStrategy2 = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());
    uint16_t animationMode = fdp.ConsumeIntegral<uint16_t>();
    bool animationStart = fdp.ConsumeBool();
    Drawing::DrawingCommonSubType commonSubType =
        static_cast<Drawing::DrawingCommonSubType>(fdp.ConsumeIntegral<uint8_t>());
    hmSymbolRun.SetRenderMode(renderMode);
    hmSymbolRun.SetSymbolEffect(effectStrategy2);
    hmSymbolRun.SetAnimationMode(animationMode);
    hmSymbolRun.SetAnimationStart(animationStart);
    hmSymbolRun.SetCommonSubType(commonSubType);

    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc1 =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    hmSymbolRun.SetAnimation(animationFunc1);

    Drawing::Font font1;
    const char* str2 = "B";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font1, Drawing::TextEncoding::UTF8);
    hmSymbolRun.SetTextBlob(textblob2);
}

void HmSymbolRunFuzzTestInnerSetColor(const uint8_t* data, size_t size)
{
    // SymbolAnimation
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    const char* str1 = "A";
    Drawing::Font font;
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);

    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsetY = fdp.ConsumeFloatingPoint<float>();
    RSPoint paint = { offsetX, offsetY };
    RSEffectStrategy effectStrategy = static_cast<RSEffectStrategy>(fdp.ConsumeIntegral<uint8_t>());
    HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolEffect(effectStrategy);

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc = nullptr;

    uint64_t symbolId = fdp.ConsumeIntegral<uint64_t>();
    HMSymbolRun hmSymbolRun = HMSymbolRun(symbolId, symbolTxt, textblob1, animationFunc);

    RSRenderGroup group1;
    RSRenderGroup group2;
    RSSymbolLayers symbolInfo;
    symbolInfo.renderGroups.push_back(group1);
    symbolInfo.renderGroups.push_back(group2);
    RSSymbolRenderingStrategy renderMode1 =  static_cast<RSSymbolRenderingStrategy>(fdp.ConsumeIntegral<uint32_t>());

    uint16_t colorNum = fdp.ConsumeIntegral<uint16_t>();
    std::vector<RSSColor> colors;
    colors.reserve(colorNum);
    for (uint16_t i = 0; i < colorNum; i++) {
        U8CPU red = fdp.ConsumeIntegral<U8CPU>();
        U8CPU gree = fdp.ConsumeIntegral<U8CPU>();
        U8CPU blue = fdp.ConsumeIntegral<U8CPU>();
        float alpha = fdp.ConsumeFloatingPoint<float>();
        RSSColor color = { alpha, red, gree, blue };
        colors.emplace_back(color);
    }

    hmSymbolRun.SetSymbolRenderColor(renderMode1, colors, symbolInfo);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::SPText::DrawSymbolFuzzTest(data, size);
    OHOS::Rosen::SPText::HmSymbolRunFuzzTestInnerSetAttributes(data, size);
    OHOS::Rosen::SPText::HmSymbolRunFuzzTestInnerSetColor(data, size);
    return 0;
}