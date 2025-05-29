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
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::SPText::DrawSymbolFuzzTest(data, size);
    return 0;
}
