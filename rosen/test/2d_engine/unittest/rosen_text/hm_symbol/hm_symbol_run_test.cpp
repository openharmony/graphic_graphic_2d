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

#include "gtest/gtest.h"
#include "txt/text_style.h"
#include "symbol_engine/hm_symbol_run.h"
#include "symbol_engine/hm_symbol_txt.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SPText {
class OHHmSymbolRunTest : public testing::Test {};

/*
 * @tc.name: DrawSymbol001
 * @tc.desc: test DrawSymbol with one Glyph
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbol001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 1, 1 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolText;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);

    symbolText.SetSymbolEffect(RSEffectStrategy::SCALE); // whit symbol anination
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);

    textblob = nullptr; // test bullptr
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);
}

/*
 * @tc.name: DrawSymbol002
 * @tc.desc: test DrawSymbol with Glyphs
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbol002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 1, 1 is the offset
    const char* str = "Test multiple glyphs";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolText;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS