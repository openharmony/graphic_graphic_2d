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
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, nullptr, animationFunc);

    // test rsCanvas is nullptr, textblob is nullptr
    hmSymbolRun.DrawSymbol(nullptr, paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    // test rsCanvas isn't nullptr, textblob is nullptr
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    const char* str = "A"; // "A" is one Glyph
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolRun hmSymbolRun1 = HMSymbolRun(1, symbolTxt, textblob, animationFunc);

    // test rsCanvas is nullptr, textblob isn't nullptr
    hmSymbolRun1.DrawSymbol(nullptr, paint_);
    EXPECT_FALSE(hmSymbolRun1.currentAnimationHasPlayed_);

    // test rsCanvas isn't nullptr, textblob isn't nullptr
    hmSymbolRun1.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun1.currentAnimationHasPlayed_);
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
    const char* str = "Test multiple glyphs"; // "Test multiple glyphs" is Glyphs
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };

    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
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
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    // test bounce animation
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    // test appear animation
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::APPEAR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    // test pulse aimation, this glyph not support, result is false
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::PULSE);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolRenderColor001
 * @tc.desc: test SetSymbolRenderColor with multi colors
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolRenderColor001, TestSize.Level1)
{
    // step 1: Simulation input
    RSSColor color1 = {1.0f, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color2 = {0.5f, 0, 255, 0}; // the 0.5 is alpha, 0, 255, 0 is RGB
    std::vector<RSSColor> colors = {color1, color2};
    RSRenderGroup group1;
    RSRenderGroup group2;
    RSSymbolLayers symbolInfo;
    symbolInfo.renderGroups.push_back(group1);
    symbolInfo.renderGroups.push_back(group2);
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);

    // step 2: Import different RenderingStrategy to test the color result.
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    EXPECT_EQ(symbolInfo.renderGroups[0].color.r, color1.r);

    renderMode = RSSymbolRenderingStrategy::MULTIPLE_COLOR;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    EXPECT_EQ(symbolInfo.renderGroups[1].color.a, color2.a);

    renderMode = RSSymbolRenderingStrategy::MULTIPLE_OPACITY;
    hmSymbolRun.SetSymbolRenderColor(renderMode, {color2}, symbolInfo);
    EXPECT_NE(symbolInfo.renderGroups[1].color.a, color2.a);
}

/*
 * @tc.name: SetSymbolRenderColor002
 * @tc.desc: test SetSymbolRenderColor with one color
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolRenderColor002, TestSize.Level1)
{
    std::vector<RSSColor> colors = {};
    RSSColor color = {1.0f, 0, 255, 0}; // the 1.0 is alpha, 0, 255, 0 is RGB
    RSRenderGroup group1;
    group1.color = color;
    RSSymbolLayers symbolInfo;
    symbolInfo.renderGroups.push_back(group1);
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);

    // step 2: inputMode is SINGLE, colors is empty.
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    hmSymbolRun.SetSymbolRenderColor(renderMode, colors, symbolInfo);
    EXPECT_EQ(symbolInfo.renderGroups[0].color.g, color.g);
}

/*
 * @tc.name: SymbolAnimation001
 * @tc.desc: test SymbolAnimation with glyphId
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SymbolAnimation001, TestSize.Level1)
{
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    RSHMSymbolData symbol;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    bool check = false;
    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    check = hmSymbolRun.SymbolAnimation(symbol, offset);
    EXPECT_FALSE(check);
}

/*
 * @tc.name: GetAnimationGroups001
 * @tc.desc: test GetAnimationGroups with glyphId
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetAnimationGroups001, TestSize.Level1)
{
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    RSEffectStrategy effectStrategy = RSEffectStrategy::BOUNCE;
    RSAnimationSetting animationOut;
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    bool flag = hmSymbolRun.GetAnimationGroups(effectStrategy, animationOut);
    EXPECT_TRUE(flag);
}

/*
 * @tc.name: GetAnimationGroups002
 * @tc.desc: test GetAnimationGroups with pulse animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetAnimationGroups002, TestSize.Level1)
{
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    RSEffectStrategy effectStrategy = RSEffectStrategy::PULSE;
    RSAnimationSetting animationOut;
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    bool flag = hmSymbolRun.GetAnimationGroups(effectStrategy, animationOut);
    EXPECT_FALSE(flag);
}

/*
 * @tc.name: GetSymbolLayers001
 * @tc.desc: test GetSymbolLayers with glyphId
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, GetSymbolLayers001, TestSize.Level1)
{
    // step 1: init data
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    RSSColor color = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    HMSymbolTxt symbolTxt;
    symbolTxt.SetRenderColor(color);
    std::shared_ptr<RSTextBlob> textBlob = nullptr;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textBlob, animationFunc);
    auto symbolLayer = hmSymbolRun.GetSymbolLayers(glyphId, symbolTxt);
    EXPECT_TRUE(symbolLayer.renderGroups.empty());

    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    symbolLayer = hmSymbolRun.GetSymbolLayers(glyphId, symbolTxt);
    EXPECT_EQ(symbolLayer.symbolGlyphId, glyphId);

    if (!symbolLayer.renderGroups.empty()) {
        auto layerColor = symbolLayer.renderGroups[0].color;
        EXPECT_EQ(layerColor.r, color.r); // the default color is {1.0, 0, 0, 0}
    }
}

/*
 * @tc.name: SetRenderColor001
 * @tc.desc: test SetRenderColor with colorList
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetRenderColor001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    RSSColor color1 = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color2 = {1.0, 0, 0, 0}; // the 1.0 is alpha, 0, 0, 0 is RGB
    std::vector<RSSColor> rsscolors1 = {color1};
    std::vector<RSSColor> rsscolors2 = {color1, color2};

    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetRenderColor(rsscolors1);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
    auto ret1 = hmSymbolRun.symbolTxt_.GetRenderColor();
    EXPECT_EQ(ret1.size(), 1);

    hmSymbolRun.SetRenderColor(rsscolors2);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
    auto ret2 = hmSymbolRun.symbolTxt_.GetRenderColor();
    EXPECT_EQ(ret2.size(), 2);
}

/*
 * @tc.name: SetRenderMode001
 * @tc.desc: test SetRenderMode with SINGLE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetRenderMode001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetRenderMode002
 * @tc.desc: test SetRenderMode with MULTIPLE_COLOR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetRenderMode002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetRenderMode003
 * @tc.desc: test SetRenderMode with MULTIPLE_OPACITY
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetRenderMode003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect001
 * @tc.desc: test SetSymbolEffect with DrawingEffectStrategy::NONE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::NONE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect002
 * @tc.desc: test SetSymbolEffect with DrawingEffectStrategy::SCALE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::SCALE);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect003
 * @tc.desc: test SetSymbolEffect with DrawingEffectStrategy::VARIABLE_COLOR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::VARIABLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::VARIABLE_COLOR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect004
 * @tc.desc: test SetSymbolEffect with DrawingEffectStrategy::APPEAR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect004, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::APPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::APPEAR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect005
 * @tc.desc: test SetSymbolEffect with different DrawingEffectStrategy::DISAPPEAR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect005, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::DISAPPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::DISAPPEAR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect006
 * @tc.desc: test SetSymbolEffect with different DrawingEffectStrategy::BOUNCE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect006, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::BOUNCE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::BOUNCE);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect007
 * @tc.desc: test SetSymbolEffect with different DrawingEffectStrategy::PULSE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect007, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::PULSE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::PULSE);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect008
 * @tc.desc: test SetSymbolEffect with different DrawingEffectStrategy::REPLACE_APPEAR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect008, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffect009
 * @tc.desc: test SetSymbolEffect with different DrawingEffectStrategy::QUICK_REPLACE_APPEAR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect009, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::QUICK_REPLACE_APPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::QUICK_REPLACE_APPEAR);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_); // Draw once for Appear
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_); // Draw twice for DisAppear
}

/*
 * @tc.name: SetAnimationMode001
 * @tc.desc: test SetAnimationMode with wholeSymbol
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimationMode001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::VARIABLE_COLOR);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetAnimationMode(0); // the 0 is the wholeSymbol or cumulative effect
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 0);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetAnimationMode002
 * @tc.desc: test SetAnimationMode with byLayer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimationMode002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::VARIABLE_COLOR);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetAnimationMode(1); // the 1 is the byLayer or iteratuve effect
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 1);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetAnimationMode003
 * @tc.desc: test SetAnimationMode with invalid value
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimationMode003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::VARIABLE_COLOR);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetAnimationMode(500); // 500 is test Boundary Value
    EXPECT_NE(hmSymbolRun.symbolTxt_.GetAnimationMode(), 500);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 1);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetAnimationStart001
 * @tc.desc: test SetAnimationStart with animationStart
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimationStart001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    EXPECT_FALSE(hmSymbolRun.symbolTxt_.GetAnimationStart());
    hmSymbolRun.SetAnimationStart(true);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetAnimationStart());
}

/*
 * @tc.name: SetCommonSubType001
 * @tc.desc: test SetCommonSubType with commonSubType::DOWN
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetCommonSubType001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetCommonSubType(Drawing::DrawingCommonSubType::DOWN);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetCommonSubType(), Drawing::DrawingCommonSubType::DOWN);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetCommonSubType002
 * @tc.desc: test SetCommonSubType with commonSubType::UP
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetCommonSubType002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint paint_ = {100, 100}; // 100, 100 is the offset
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetCommonSubType(), Drawing::DrawingCommonSubType::UP);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint_);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob001
 * @tc.desc: test SetTextBlob with null textBlob
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob001, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "A";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    EXPECT_NE(hmSymbolRun.textBlob_, nullptr);
    hmSymbolRun.SetTextBlob(nullptr);
    EXPECT_NE(hmSymbolRun.textBlob_, nullptr);
}

/*
 * @tc.name: SetTextBlob002
 * @tc.desc: test SetTextBlob with same content(before:"A",after:"A",Corresponding truth table: 111)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob002, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "A";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"A", glyphId2:"A"
    //glyphId1.size() == 1,glyphId2.size() == 1,glyphId1[0] == glyphId2[0]
    const char* str2 = "A";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob003
 * @tc.desc: test SetTextBlob with different content(before:"A",after:"B",Corresponding truth table: 110)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob003, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "A";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"A", glyphId2:"B"
    //glyphId1.size() == 1,glyphId2.size() == 1,glyphId1[0] != glyphId2[0]
    const char* str2 = "B";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob004
 * @tc.desc: test SetTextBlob with different content(before:"A",after:"AB",Corresponding truth table: 101)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob004, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "A";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"A", glyphId2:"AB"
    //glyphId1.size() == 1,glyphId2.size() != 1,glyphId1[0] == glyphId2[0]
    const char* str2 = "AB";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob005
 * @tc.desc: test SetTextBlob with different content(before:"A",after:"BA",Corresponding truth table: 100)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob005, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "A";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"A", glyphId2:"BA"
    //glyphId1.size() == 1,glyphId2.size() != 1,glyphId1[0] != glyphId2[0]
    const char* str2 = "BA";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob006
 * @tc.desc: test SetTextBlob with different content(before:"AB",after:"A",Corresponding truth table: 011)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob006, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "AB";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"AB", glyphId2:"A"
    //glyphId1.size() != 1,glyphId2.size() == 1,glyphId1[0] == glyphId2[0]
    const char* str2 = "A";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob007
 * @tc.desc: test SetTextBlob with different content(before:"AB",after:"B",Corresponding truth table: 010)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob007, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "AB";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"AB", glyphId2:"B"
    //glyphId1.size() != 1,glyphId2.size() == 1,glyphId1[0] != glyphId2[0]
    const char* str2 = "B";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob008
 * @tc.desc: test SetTextBlob with different content(before:"AB",after:"AC",Corresponding truth table: 001)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob008, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "AB";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"AB", glyphId2:"AC"
    //glyphId1.size() != 1,glyphId2.size() != 1,glyphId1[0] == glyphId2[0]
    const char* str2 = "AC";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetTextBlob009
 * @tc.desc: test SetTextBlob with different content(before:"AB",after:"BA",Corresponding truth table: 000)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetTextBlob009, TestSize.Level1)
{
    Drawing::Font font;
    const char* str1 = "AB";
    auto textblob1 = Drawing::TextBlob::MakeFromText(str1, strlen(str1), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob1, animationFunc);

    //glyphId1:"AB", glyphId2:"BA"
    //glyphId1.size() != 1,glyphId2.size() != 1,glyphId1[0] != glyphId2[0]
    const char* str2 = "BA";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetAnimation001
 * @tc.desc: test SetAnimation with animationFunc
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimation001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolTxt symbolTxt;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    EXPECT_EQ(hmSymbolRun.animationFunc_, nullptr);

    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc1 =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    hmSymbolRun.SetAnimation(animationFunc1);
    EXPECT_NE(hmSymbolRun.animationFunc_, nullptr);
    hmSymbolRun.SetAnimation(animationFunc);
    EXPECT_NE(hmSymbolRun.animationFunc_, nullptr);
}

/*
 * @tc.name: UpdateSymbolLayersGroups001
 * @tc.desc: test UpdateSymbolLayersGroups
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, UpdateSymbolLayersGroups001, TestSize.Level1)
{
    HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolType(SymbolType::SYSTEM);
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    std::shared_ptr<RSTextBlob> textBlob = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textBlob, animationFunc);
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    EXPECT_EQ(hmSymbolRun.symbolLayersGroups_.symbolGlyphId, glyphId);

    glyphId = 0; // 0 is a nonexistent GlyphID
    HMSymbolRun hmSymbolRun1 = HMSymbolRun(0, symbolTxt, textBlob, animationFunc);
    hmSymbolRun1.UpdateSymbolLayersGroups(glyphId);
    EXPECT_TRUE(hmSymbolRun1.symbolLayersGroups_.renderModeGroups.empty());

    symbolTxt.SetSymbolType(SymbolType::CUSTOM);
    HMSymbolRun hmSymbolRun2 = HMSymbolRun(0, symbolTxt, textBlob, animationFunc);
    hmSymbolRun2.UpdateSymbolLayersGroups(glyphId);
    EXPECT_TRUE(hmSymbolRun2.symbolLayersGroups_.renderModeGroups.empty());
}

/*
 * @tc.name: SymbolUid001
 * @tc.desc: test GetSymbolUid and GetSymbolUid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SymbolUid001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    EXPECT_EQ(hmSymbolRun.GetSymbolUid(), 0);
    hmSymbolRun.SetSymbolUid(100);
    EXPECT_EQ(hmSymbolRun.GetSymbolUid(), 100);
}

/*
 * @tc.name: SymbolTxt001
 * @tc.desc: test GetSymbolTxt and SetSymbolTxt
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SymbolTxt001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = HMSymbolRun();

    //test get method
    HMSymbolTxt hmSymbolEmpty;
    HMSymbolTxt hmSymbolTxt = hmSymbolRun.GetSymbolTxt();
    EXPECT_EQ(hmSymbolEmpty, hmSymbolTxt);

    //test set method
    HMSymbolTxt hmSymbolTest;
    hmSymbolTest.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    hmSymbolRun.SetSymbolTxt(hmSymbolTest);

    EXPECT_EQ(hmSymbolRun.GetSymbolTxt().GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
}

/*
 * @tc.name: SymbolAnimationTest001
 * @tc.desc: test SymbolAnimation with disable effectStrategy
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SymbolAnimationTest001, TestSize.Level1)
{
    // init data
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::DISABLE);
    RSHMSymbolData symbol;
    std::pair<float, float> offsetXY(10.0f, 15.0f);
    Drawing::DrawingAnimationSetting animationSettingOne = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::DISABLE_TYPE
        },
        // groupSettings
        {
            // {0, 1}: layerIndes, 0: animationIndex
            {{{{0, 1}}}, 0}
        }
    };
    hmSymbolRun.symbolLayersGroups_.animationSettings.push_back(animationSettingOne);
    // test SymbolAnimation by DISABLE
    bool result = hmSymbolRun.SymbolAnimation(symbol, offsetXY);
    EXPECT_FALSE(result);
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    gradients.push_back(std::make_shared<SymbolGradient>());
    hmSymbolRun.gradients_ = gradients;
    gradients.push_back(std::make_shared<SymbolGradient>());
    SymbolColor symbolColor = {SymbolColorType::GRADIENT_TYPE, gradients};
    hmSymbolRun.SetSymbolColor(symbolColor);
    hmSymbolRun.SetRenderMode(RSSymbolRenderingStrategy::MULTIPLE_COLOR);
    result = hmSymbolRun.SymbolAnimation(symbol, offsetXY);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: SetGradients001
 * @tc.desc: test SetGradients
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetGradients001, TestSize.Level1)
{
    uint16_t glyphId = 3; // 3 is an existing GlyphID
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = std::make_shared<SymbolGradient>();
    color2->SetColors({0XFF0000FF}); // 0XFF0000FF is ARGB
    SymbolColor symbolColor = {SymbolColorType::GRADIENT_TYPE, {color1, color2}};
    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolColor(symbolColor);
    hmSymbolRun.SetSymbolTxt(symbolTxt);
    hmSymbolRun.UpdateSymbolLayersGroups(glyphId);
    auto symbolLayer = hmSymbolRun.GetSymbolLayers(glyphId, symbolTxt);

    std::vector<std::shared_ptr<SymbolGradient>> gradients = {};
    symbolColor.colorType = SymbolColorType::GRADIENT_TYPE;
    symbolColor.gradients = gradients;
    hmSymbolRun.SetSymbolColor(symbolColor);
    symbolLayer = hmSymbolRun.GetSymbolLayers(glyphId, symbolTxt);
    EXPECT_EQ(symbolLayer.symbolGlyphId, glyphId);
}

/*
 * @tc.name: SetGradientColor001
 * @tc.desc: test SetGradientColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetGradientColor001, TestSize.Level1)
{
    // test gradients is empty
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    RSSymbolLayers symbolInfo;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.SetGradientColor(renderMode, symbolInfo);
    EXPECT_TRUE(hmSymbolRun.gradients_.empty());

    // test gradients is not empty
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    gradients.push_back(std::make_shared<SymbolGradient>());
    gradients.push_back(std::make_shared<SymbolGradient>());
    SymbolColor symbolColor = {SymbolColorType::GRADIENT_TYPE, gradients};
    hmSymbolRun.SetSymbolColor(symbolColor);
    hmSymbolRun.SetGradientColor(renderMode, symbolInfo);
    EXPECT_TRUE(hmSymbolRun.gradients_.size() == 1);

    // test renderMode is MULTIPLE_COLOR
    renderMode = RSSymbolRenderingStrategy::MULTIPLE_COLOR;
    symbolInfo.renderGroups.push_back({});
    symbolInfo.renderGroups.push_back({});
    gradients[0] = nullptr;
    symbolColor.gradients = gradients;
    hmSymbolRun.SetSymbolColor(symbolColor);
    hmSymbolRun.SetGradientColor(renderMode, symbolInfo);
    EXPECT_FALSE(hmSymbolRun.gradients_.empty());

    gradients = {};
    gradients.push_back(std::make_shared<SymbolLineGradient>(50.0f)); // 50.0f is angle of lineGradient
    symbolColor.gradients = gradients;
    hmSymbolRun.SetSymbolColor(symbolColor);
    hmSymbolRun.SetGradientColor(renderMode, symbolInfo);
    EXPECT_FALSE(hmSymbolRun.gradients_.empty());
}

/*
 * @tc.name: SetDrawPath001
 * @tc.desc: test SetDrawPath
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetDrawPath001, TestSize.Level1)
{
    // test renderMode is SINGLE
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSSymbolRenderingStrategy renderMode = RSSymbolRenderingStrategy::SINGLE;
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    hmSymbolRun.SetRenderMode(renderMode);
    std::vector<RSPath> multPaths;
    RSPath path;
    hmSymbolRun.DrawPaths(rsCanvas.get(), multPaths, path);
    EXPECT_TRUE(hmSymbolRun.gradients_.empty());

    // Test multPaths not is empty
    path.AddCircle(100.0f, 100.0f, 50.0f); // 100.0f x, 100.0f y, 50.0f radius
    multPaths.push_back(path);
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    gradients.push_back(std::make_shared<SymbolGradient>());
    gradients.push_back(std::make_shared<SymbolGradient>());
    SymbolColor symbolColor = {SymbolColorType::GRADIENT_TYPE, gradients};
    hmSymbolRun.SetSymbolColor(symbolColor);
    hmSymbolRun.DrawPaths(rsCanvas.get(), multPaths, path);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), RSSymbolRenderingStrategy::SINGLE);
    
    // test renderMode is MULTIPLE_COLOR
    renderMode = RSSymbolRenderingStrategy::MULTIPLE_COLOR;
    hmSymbolRun.SetRenderMode(renderMode);
    hmSymbolRun.gradients_ = gradients;
    hmSymbolRun.DrawPaths(rsCanvas.get(), multPaths, path);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), RSSymbolRenderingStrategy::MULTIPLE_COLOR);
}

/*
 * @tc.name: SetSymbolShadow001
 * @tc.desc: test SetSymbolShadow with animation current is false and true
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolShadow001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {10.0F, 10.0f} is offsetXY, 10.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {10.0F, 10.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: OnDrawSymbolTestShadow
 * @tc.desc: test OnDrawSymbol with shadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, OnDrawSymbolTestShadow, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSPath path;
    path.AddCircle(100.0f, 100.0f, 40.0f); // 100.0f x, 100.0f, 40.0f radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = {{0}}; // this one layers;
    symbol.symbolInfo_.renderGroups = {{{{{0}}}}}; // the {0} is layerIndexes of one group

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {10.0F, 10.0f} is offsetXY, 10.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {10.0F, 10.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());

    // test radius < 1.0f
    shadow.blurRadius = 0.5f;
    symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: DrawSymbolShadow001
 * @tc.desc: test DrawSymbolShadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbolShadow001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSPath path1;
    path1.AddCircle(100.0f, 100.0f, 40.0f); // 100.0f x, 100.0f y, 40.0f radius
    RSPath path2;
    path2.AddCircle(100.0f, 50.0f, 40.0f); // 100.0f x, 50.0f y, 40.0f radius
    std::vector<RSPath> paths = {path1, path2};
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {10.0F, 10.0f} is offsetXY, 10.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {10.0F, 10.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.DrawSymbolShadow(rsCanvas.get(), paths);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());

    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = std::make_shared<SymbolGradient>();
    color2->SetColors({0XFF0000FF}); // 0XFF0000FF is ARGB
    hmSymbolRun.gradients_ = {color1, color2};
    hmSymbolRun.DrawSymbolShadow(rsCanvas.get(), paths);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: DrawSymbolShadow002
 * @tc.desc: test DrawSymbolShadow width recordingCanvas
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, DrawSymbolShadow002, TestSize.Level1)
{
    // 100 is width, 200 is height of canvas
    std::shared_ptr<RSRecordingCanvas> rsCanvas = std::make_shared<RSRecordingCanvas>(100, 200);
    RSPoint offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSPath path1;
    path1.AddCircle(100.0f, 100.0f, 40.0f); // 100.0f x, 100.0f y, 40.0f radius
    std::vector<RSPath> paths = {path1};
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    SymbolShadow shadow;
    shadow.blurRadius = 1.5f; // random value
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.DrawSymbolShadow(rsCanvas.get(), paths);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS