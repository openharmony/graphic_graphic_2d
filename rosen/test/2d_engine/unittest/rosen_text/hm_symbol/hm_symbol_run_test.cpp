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
static const float MIN_VALUE = 1e-6;
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

    // test rsCanvas is nullptr
    hmSymbolRun.DrawSymbol(nullptr, textblob.get(), paint_, symbolText);

    textblob = nullptr; // test textblob is bullptr
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);

    // test rsCanvas is nullptr, textblob is nullptr
    hmSymbolRun.DrawSymbol(nullptr, nullptr, paint_, symbolText);
}

/*
 * @tc.name: DrawSymbol002
 * @tc.desc: test DrawSymbol with Glyphs
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbol002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "Test multiple glyphs";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolText;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);
}

/*
 * @tc.name: DrawSymbol003
 * @tc.desc: test DrawSymbol with animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbol003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolText;
    symbolText.SetAnimationStart(true);
    // test bounce animation
    symbolText.SetSymbolEffect(RSEffectStrategy::BOUNCE);
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);
    EXPECT_TRUE(symbolText.GetEffectStrategy() == RSEffectStrategy::BOUNCE);

    // test pulse animation
    symbolText.SetSymbolEffect(RSEffectStrategy::PULSE);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), textblob.get(), paint_, symbolText);
    EXPECT_TRUE(symbolText.GetEffectStrategy() == RSEffectStrategy::PULSE);
}

/*
 * @tc.name: SetSymbolRenderColor001
 * @tc.desc: test SetSymbolRenderColor with renderMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolRenderColor001, TestSize.Level1)
{
    // step 1: Simulation input
    RSSColor color1 = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color2 = {1.0, 0, 255, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    std::vector<RSSColor> colors = {color1, color2};
    RSRenderGroup group1;
    RSRenderGroup group2;
    RSSymbolLayers symbolInfo;
    symbolInfo.renderGroups.push_back(group1);
    symbolInfo.renderGroups.push_back(group2);
    HMSymbolRun hmSymbolRun = HMSymbolRun();

    // step 2: Import different RenderingStrategy to test the color result.
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    bool check = false;
    if (color1.r == symbolInfo.renderGroups[0].color.r &&
        color1.g == symbolInfo.renderGroups[0].color.g &&
        color1.b == symbolInfo.renderGroups[0].color.b) {
        check = true;
    }
    EXPECT_EQ(check, true);

    renderMode = RSSymbolRenderingStrategy::MULTIPLE_OPACITY;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    bool check1 = false;
    if (color1.r == symbolInfo.renderGroups[0].color.r &&
        color1.g == symbolInfo.renderGroups[0].color.g &&
        color1.b == symbolInfo.renderGroups[0].color.b) {
        check1 = true;
    }
    EXPECT_EQ(check1, true);

    renderMode = RSSymbolRenderingStrategy::MULTIPLE_COLOR;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    bool check2 = false;
    if (color2.r == symbolInfo.renderGroups[1].color.r &&
        color2.g == symbolInfo.renderGroups[1].color.g &&
        color2.b == symbolInfo.renderGroups[1].color.b) {
        check2 = true;
    }
    EXPECT_EQ(check2, true);
}

/*
 * @tc.name: SetSymbolRenderColor002
 * @tc.desc: test SetSymbolRenderColor with renderMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolRenderColor002, TestSize.Level1)
{
    std::vector<RSSColor> colors = {};
    RSSColor color = {1.0, 0, 255, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSRenderGroup group1;
    group1.color = color;
    RSSymbolLayers symbolInfo;
    symbolInfo.renderGroups.push_back(group1);
    HMSymbolRun hmSymbolRun = HMSymbolRun();

    // step 2: Import different RenderingStrategy to test the color result.
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    bool check = false;
    if (abs(color.a - symbolInfo.renderGroups[0].color.a) < MIN_VALUE &&
        color.r == symbolInfo.renderGroups[0].color.r &&
        color.g == symbolInfo.renderGroups[0].color.g &&
        color.b == symbolInfo.renderGroups[0].color.b) {
        check = true;
    }
    EXPECT_EQ(check, true);
}

/*
 * @tc.name: SymbolAnimation001
 * @tc.desc: test SymbolAnimation with glyphid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SymbolAnimation001, TestSize.Level1)
{
    uint16_t glyphid = 3; // 3 is an existing GlyphID
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    HMSymbolTxt symbolTxt;
    RSHMSymbolData symbol;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    bool check = false;
    check = hmSymbolRun.SymbolAnimation(symbol, glyphid, offset, symbolTxt);
    EXPECT_TRUE(check == false);

    symbolTxt.SetSymbolEffect(RSEffectStrategy::SCALE);
    bool check1 = false;
    check1 = hmSymbolRun.SymbolAnimation(symbol, glyphid, offset, symbolTxt);
    EXPECT_TRUE(check1 == false);

    symbolTxt.SetAnimationMode(1);
    symbolTxt.SetSymbolEffect(RSEffectStrategy::SCALE);
    bool check2 = false;
    check2 = hmSymbolRun.SymbolAnimation(symbol, glyphid, offset, symbolTxt);
    EXPECT_TRUE(check2 == false);

    symbolTxt.SetAnimationMode(1);
    symbolTxt.SetSymbolEffect(RSEffectStrategy::VARIABLE_COLOR);
    bool check3 = false;
    check3 = hmSymbolRun.SymbolAnimation(symbol, glyphid, offset, symbolTxt);
    EXPECT_TRUE(check3 == false);
}

/*
 * @tc.name: GetAnimationGroups001
 * @tc.desc: test GetAnimationGroups with glyphid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetAnimationGroups001, TestSize.Level1)
{
    uint16_t glyphid = 3; // 3 is an existing GlyphID
    RSEffectStrategy effectStrategy = RSEffectStrategy::BOUNCE;
    RSAnimationSetting animationOut;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    bool flag = hmSymbolRun.GetAnimationGroups(glyphid, effectStrategy, animationOut);
    EXPECT_TRUE(flag);
}

/*
 * @tc.name: GetAnimationGroups002
 * @tc.desc: test GetAnimationGroups with pulse animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetAnimationGroups002, TestSize.Level1)
{
    uint16_t glyphid = 3; // 3 is an existing GlyphID
    RSEffectStrategy effectStrategy = RSEffectStrategy::PULSE;
    RSAnimationSetting animationOut;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    bool flag = hmSymbolRun.GetAnimationGroups(glyphid, effectStrategy, animationOut);
    EXPECT_TRUE(flag == false);
}

/*
 * @tc.name: GetSymbolLayers001
 * @tc.desc: test GetSymbolLayers with glyphid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetSymbolLayers001, TestSize.Level1)
{
    // step 1: init data
    uint16_t glyphid = 3; // 3 is an existing GlyphID
    RSSColor color = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    HMSymbolTxt symbolText;
    symbolText.SetRenderColor(color);
    auto symbolLayer = HMSymbolRun::GetSymbolLayers(glyphid, symbolText);
    EXPECT_TRUE(symbolLayer.symbolGlyphId == glyphid);

    if (!symbolLayer.renderGroups.empty()) {
        auto layerColor = symbolLayer.renderGroups[0].color;
        EXPECT_TRUE(layerColor.r == color.r); // the default color is {1.0, 0, 0, 0}
    }
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS