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
 * @tc.desc: test SetSymbolRenderColor with renderMode
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
 * @tc.desc: test SetSymbolRenderColor with renderMode
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
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    RSSColor color1 = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color2 = {1.0, 0, 0, 0}; // the 1.0 is alpha, 0, 0, 0 is RGB
    std::vector<RSSColor> rsscolors1 = {color1};
    std::vector<RSSColor> rsscolors2 = {color1, color2};

    hmSymbolRun.SetRenderColor(rsscolors1);
    auto ret1 = hmSymbolRun.symbolTxt_.GetRenderColor();
    EXPECT_EQ(ret1.size(), 1);
    hmSymbolRun.SetRenderColor(rsscolors2);
    auto ret2 = hmSymbolRun.symbolTxt_.GetRenderColor();
    EXPECT_EQ(ret2.size(), 2);
}

/*
 * @tc.name: SetRenderMode001
 * @tc.desc: test SetRenderMode with renderMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetRenderMode001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);

    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);

    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
}

/*
 * @tc.name: SetSymbolEffect001
 * @tc.desc: test SetSymbolEffect with symbolEffect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetSymbolEffect001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::NONE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::SCALE);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::VARIABLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::VARIABLE_COLOR);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::APPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::APPEAR);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::DISAPPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::DISAPPEAR);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::BOUNCE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::BOUNCE);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::PULSE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::PULSE);

    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
}

/*
 * @tc.name: SetAnimationMode001
 * @tc.desc: test SetAnimationMode with animationMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetAnimationMode001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetAnimationMode(0); // the 0 is the wholeSymbol or cumulative effect
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 0);

    hmSymbolRun.SetAnimationMode(1); // the 1 is the byLayer or iteratuve effect
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 1);

    hmSymbolRun.SetAnimationMode(500); // 500 is test Boundary Value
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetAnimationMode(), 1);
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
 * @tc.desc: test SetCommonSubType with commonSubType
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTest, SetCommonSubType001, TestSize.Level1)
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    hmSymbolRun.SetCommonSubType(Drawing::DrawingCommonSubType::DOWN);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetCommonSubType(), Drawing::DrawingCommonSubType::DOWN);

    hmSymbolRun.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetCommonSubType(), Drawing::DrawingCommonSubType::UP);
}

/*
 * @tc.name: SetTextBlob001
 * @tc.desc: test SetTextBlob with textBlob
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

    //glyphId1:"A", glyphId2:"A"
    //glyphId1.size() == 1,glyphId2.size() == 1,glyphId1[0] == glyphId2[0]
    const char* str2 = "A";
    auto textblob2 = Drawing::TextBlob::MakeFromText(str2, strlen(str2), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"A", glyphId2:"B"
    //glyphId1.size() == 1,glyphId2.size() == 1,glyphId1[0] != glyphId2[0]
    const char* str3 = "B";
    auto textblob3 = Drawing::TextBlob::MakeFromText(str3, strlen(str3), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.SetTextBlob(textblob3);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"B", glyphId2:"BA"
    //glyphId1.size() == 1,glyphId2.size() != 1,glyphId1[0] == glyphId2[0]
    const char* str4 = "BA";
    auto textblob4 = Drawing::TextBlob::MakeFromText(str4, strlen(str4), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob4);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"BA", glyphId2:"A"
    //glyphId1.size() != 1,glyphId2.size() == 1,glyphId1[0] != glyphId2[0]
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob2);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"A", glyphId2:"BA"
    //glyphId1.size() == 1,glyphId2.size() != 1,glyphId1[0] != glyphId2[0]
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob4);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"BA", glyphId2:"AB"
    //glyphId1.size() != 1,glyphId2.size() != 1,glyphId1[0] != glyphId2[0]
    const char* str5 = "AB";
    auto textblob5 = Drawing::TextBlob::MakeFromText(str5, strlen(str5), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob5);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"AB", glyphId2:"AC"
    //glyphId1.size() != 1,glyphId2.size() != 1,glyphId1[0] == glyphId2[0]
    const char* str6 = "AC";
    auto textblob6 = Drawing::TextBlob::MakeFromText(str6, strlen(str6), font, Drawing::TextEncoding::UTF8);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob6);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);

    //glyphId1:"AC", glyphId2:"B"
    //glyphId1.size() != 1,glyphId2.size() == 1,glyphId1[0] != glyphId2[0]
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetTextBlob(textblob3);
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
} // namespace SPText
} // namespace Rosen
} // namespace OHOS