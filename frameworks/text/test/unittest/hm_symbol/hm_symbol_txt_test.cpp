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
#include "rosen_text/text_style.h"
#include "convert.h"
#include "symbol_engine/hm_symbol_run.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHHmSymbolTxtTest : public testing::Test {};

/*
 * @tc.name: OHHmSymbolTxtTest001
 * @tc.desc: test for font isSymbolGlyph
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest001, TestSize.Level1)
{
    TextStyle style;
    SPText::TextStyle textStyle;
    style.isSymbolGlyph = true;
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.isSymbolGlyph, true);
}

/*
 * @tc.name: OHHmSymbolTxtTest002
 * @tc.desc: test for symbol SetRenderMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetRenderMode(0); // this 0 is single
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);

    style.symbol.SetRenderMode(2); // this 2 is multiple opacity
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);

    style.symbol.SetRenderMode(1); // this 1 is multiple color
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);

    style.symbol.SetRenderMode(5); // this 5 is Incorrect input parameter.
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
}

/*
 * @tc.name: OHHmSymbolTxtTest003
 * @tc.desc: test for symbol GetSymbolLayers
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest003, TestSize.Level1)
{
    uint16_t glyphId = 0;
    SPText::TextStyle textStyle;
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    SPText::HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    SPText::HMSymbolRun hmSymbolRun = SPText::HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    auto symbolInfo = hmSymbolRun.GetSymbolLayers(glyphId, textStyle.symbol);
    bool flag = symbolInfo.renderGroups.empty() && symbolInfo.layers.empty();
    EXPECT_TRUE(flag);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetRenderColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest004, TestSize.Level1)
{
    HMSymbolTxt symbol;
    Drawing::Color color1 = Drawing::Color::COLOR_BLUE;
    Drawing::Color color2 = Drawing::Color::COLOR_GREEN;
    Drawing::Color color3 = Drawing::Color::COLOR_RED;
    std::vector<Drawing::Color> colors = {color1, color2, color3};
    symbol.SetRenderColor(colors);
    auto colors1 = symbol.GetRenderColor();
    EXPECT_EQ(colors1.size(), 3); // this 3 is the size of RenderColor

    symbol.SetRenderColor(color1);
    auto colors2 = symbol.GetRenderColor();
    EXPECT_EQ(colors2.size(), 1); // this 1 is the size of RenderColor

    RSSColor color4 = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color5 = {1.0, 0, 0, 0}; // the 1.0 is alpha, 0, 0, 0 is RGB
    std::vector<RSSColor> rsscolors = {color4, color5};
    symbol.SetRenderColor(rsscolors);
    auto colors3 = symbol.GetRenderColor();
    EXPECT_EQ(colors3.size(), 2); // this 2 is the size of RenderColor

    symbol.SetRenderColor(color4);
    auto colors4 = symbol.GetRenderColor();
    EXPECT_EQ(colors4.size(), 1); // this 1 is the size of RenderColor
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetEffectStrategy
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest005, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetSymbolEffect(100); // this 100 is wrong value
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);

    style.symbol.SetSymbolEffect(0); // this 0 is NONE animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);

    style.symbol.SetSymbolEffect(1); // this 1 is scale animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::SCALE);

    style.symbol.SetSymbolEffect(2); // this 2 is variable color animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::VARIABLE_COLOR);

    style.symbol.SetSymbolEffect(3); // this 3 is appear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::APPEAR);

    style.symbol.SetSymbolEffect(4); // this 4 is disappear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::DISAPPEAR);

    style.symbol.SetSymbolEffect(5); // this 5 is bounce animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::BOUNCE);

    style.symbol.SetSymbolEffect(6); // this 6 is pulse animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::PULSE);

    style.symbol.SetSymbolEffect(7); // this 7 is replace_appear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetAnimationMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest006, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAnimationMode(0); // the 0 is the wholeSymbol or cumulative effect
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 0);

    style.symbol.SetAnimationMode(1); // the 1 is the byLayer or iteratuve effect
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 1);

    style.symbol.SetAnimationMode(500); // 500 is test Boundary Value
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 1);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetRepeatCount
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest007, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    int count = 5; // the numbers of animation playback
    style.symbol.SetRepeatCount(count); // the 0 is the wholeSymbol or cumulative effect
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRepeatCount(), count);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetAnimationStart
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest008, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAnimationStart(true);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationStart(), true);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetVisualMode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest009, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetVisualMode(VisualMode::VISUAL_SMALL);
    auto visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), false);

    style.symbol.SetVisualMode(VisualMode::VISUAL_LARGER);
    visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), false);

    style.symbol.SetVisualMode(VisualMode::VISUAL_MEDIUM);
    visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), true);
}

/*
 * @tc.name: OHHmSymbolTxtTest
 * @tc.desc: test for symbol SetCommonSubType
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest010, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetCommonSubType(Drawing::DrawingCommonSubType::DOWN);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetCommonSubType(), Drawing::DrawingCommonSubType::DOWN);

    style.symbol.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetCommonSubType(), Drawing::DrawingCommonSubType::UP);
}

/*
 * @tc.name: OHHmSymbolTxtTest011
 * @tc.desc: test for symbol HMSymbolTxt::operator
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest011, TestSize.Level1)
{
    RSSColor color1 = {1.0, 255, 0, 0}; // the 1.0 is alpha, 255, 0, 0 is RGB
    RSSColor color2 = {1.0, 0, 0, 0}; // the 1.0 is alpha, 0, 0, 0 is RGB
    RSSColor color3 = {1.0, 255, 255, 0}; // the 1.0 is alpha, 255, 255, 0 is RGB
    RSSColor color4 = {1.0, 255, 0, 255}; // the 1.0 is alpha, 255, 0, 255 is RGB
    RSSColor color5 = {0.5, 255, 0, 0}; // the 0.5 is alpha, 255, 0, 0 is RGB
    std::vector<RSSColor> colors = {color1, color2};
    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetRenderColor(color1);
    SPText::HMSymbolTxt symbolTxt1;
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt1.SetRenderColor(color2);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt1.SetRenderColor(color3);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt1.SetRenderColor(color4);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt1.SetRenderColor(color5);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt.SetRenderColor(colors);
    symbolTxt1.SetRenderColor(colors);
    EXPECT_EQ(symbolTxt == symbolTxt1, true);

    symbolTxt.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    symbolTxt1.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    symbolTxt1.SetSymbolEffect(Drawing::DrawingEffectStrategy::APPEAR);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    symbolTxt1.SetSymbolEffect(Drawing::DrawingEffectStrategy::APPEAR);
    symbolTxt.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    symbolTxt1.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_EQ(symbolTxt == symbolTxt1, false);

    symbolTxt.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    symbolTxt1.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    symbolTxt.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    symbolTxt1.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_EQ(symbolTxt == symbolTxt1, true);
}

/*
 * @tc.name: SymbolUid001
 * @tc.desc: test GetSymbolUid and SetSymbolUid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SymbolUid001, TestSize.Level1)
{
    SPText::HMSymbolTxt symbolTxt;
    EXPECT_EQ(symbolTxt.GetSymbolUid(), 0);
    symbolTxt.SetSymbolUid(100);
    EXPECT_EQ(symbolTxt.GetSymbolUid(), 100);
}

/*
 * @tc.name: SymbolBitmap001
 * @tc.desc: test GetSymbolBitmap and SetSymbolBitmap
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SymbolBitmap001, TestSize.Level1)
{
    SPText::HMSymbolTxt symbolTxt;
    // test get method
    auto bitmap = symbolTxt.GetSymbolBitmap();
    EXPECT_TRUE(bitmap.none());

    // test set method
    SymbolBitmapType symbolBitmap;
    symbolBitmap.flip();
    symbolTxt.SetSymbolBitmap(symbolBitmap);
    int symbolAttrLen = static_cast<int>(RelayoutSymbolStyleAttribute::SYMBOL_ATTRIBUTE_BUTT);
    EXPECT_EQ(symbolTxt.GetSymbolBitmap().size(), symbolAttrLen);
}
} // namespace Rosen
} // namespace OHOS