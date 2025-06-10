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
 * @tc.name: IsSymbolGlyph001
 * @tc.desc: test for font isSymbolGlyph
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, IsSymbolGlyph001, TestSize.Level1)
{
    TextStyle style;
    SPText::TextStyle textStyle;
    style.isSymbolGlyph = true;
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.isSymbolGlyph, true);
}

/*
 * @tc.name: SetRenderMode001
 * @tc.desc: test SetRenderMode with single
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetRenderMode(0); // this 0 is single
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);
}

/*
 * @tc.name: SetRenderMode002
 * @tc.desc: test SetRenderMode with multiple color
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetRenderMode(1); // this 1 is multiple color
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
}

/*
 * @tc.name: SetRenderMode003
 * @tc.desc: test SetRenderMode with multi opacity
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode003, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetRenderMode(2); // this 2 is multiple opacity
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
}

/*
 * @tc.name: SetRenderMode004
 * @tc.desc: test SetRenderMode with invalid value
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode004, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetRenderMode(0); // this 0 is single
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);

    style.symbol.SetRenderMode(5);  // this 5 is Incorrect input parameter.
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);
}

/*
 * @tc.name: GetSymbolLayers001
 * @tc.desc: test for symbol GetSymbolLayers
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetSymbolLayers001, TestSize.Level1)
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
 * @tc.name: SetRenderColor001
 * @tc.desc: test for symbol SetRenderColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderColor001, TestSize.Level1)
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
 * @tc.name: SetEffectStrategy001
 * @tc.desc: test SetEffectStrategy for None animaiton
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(0); // this 0 is NONE animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);
}

/*
 * @tc.name: SetEffectStrategy002
 * @tc.desc: test SetEffectStrategy for scale animaiton
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(1); // this 1 is scale animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::SCALE);
}

/*
 * @tc.name: SetEffectStrategy003
 * @tc.desc: test SetEffectStrategy for variable color animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy003, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(2); // this 2 is variable color animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::VARIABLE_COLOR);
}

/*
 * @tc.name: SetEffectStrategy004
 * @tc.desc: test SetEffectStrategy for appear animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy004, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(3); // this 3 is appear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::APPEAR);
}

/*
 * @tc.name: SetEffectStrategy005
 * @tc.desc: test SetEffectStrategy for disappear animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy005, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(4); // this 4 is disappear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::DISAPPEAR);
}

/*
 * @tc.name: SetEffectStrategy006
 * @tc.desc: test SetEffectStrategy for bounce animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy006, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(5); // this 5 is bounce animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::BOUNCE);
}

/*
 * @tc.name: SetEffectStrategy007
 * @tc.desc: test SetEffectStrategy for pulse animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy007, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(6); // this 6 is pulse animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::PULSE);
}

/*
 * @tc.name: SetEffectStrategy008
 * @tc.desc: test SetEffectStrategy for replace_appear animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy008, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(7); // this 7 is replace_appear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
}

/*
 * @tc.name: SetEffectStrategy009
 * @tc.desc: test SetEffectStrategy for invalid value
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy009, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(100); // this 100 is wrong value
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::NONE);
}

/*
 * @tc.name: SetEffectStrategy0010
 * @tc.desc: test SetEffectStrategy for replace_appear animation
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy0010, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetSymbolEffect(9); // this 9 is quick_replace_appear animation
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetEffectStrategy(), Drawing::DrawingEffectStrategy::QUICK_REPLACE_APPEAR);
}

/*
 * @tc.name: SetAnimationMode001
 * @tc.desc: test for SetAnimationMode with cumulative effect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAnimationMode(0); // the 0 is the wholeSymbol or cumulative effect
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 0);
}

/*
 * @tc.name: SetAnimationMode002
 * @tc.desc: test for SetAnimationMode with iteratuve effect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetAnimationMode(1); // the 1 is the byLayer or iteratuve effect
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 1);
}

/*
 * @tc.name: SetAnimationMode003
 * @tc.desc: test for SetAnimationMode with invalid value
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode003, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetAnimationMode(500); // 500 is test Boundary Value
    textStyle = AdapterTxt::Convert(style);
    EXPECT_NE(textStyle.symbol.GetAnimationMode(), 500);
    EXPECT_EQ(textStyle.symbol.GetAnimationMode(), 1);
}

/*
 * @tc.name: SetRepeatCount001
 * @tc.desc: test for symbol SetRepeatCount
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRepeatCount001, TestSize.Level1)
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
 * @tc.name: SetAnimationStart001
 * @tc.desc: test SetAnimationStart with true
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetAnimationStart001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAnimationStart(true);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationStart(), true);
}

/*
 * @tc.name: SetAnimationStart002
 * @tc.desc: test SetAnimationStart with false
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetAnimationStart002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAnimationStart(false);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAnimationStart(), false);
}

/*
 * @tc.name: SetVisualMode001
 * @tc.desc: test SetVisualMode with VISUAL_SMALL
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetVisualMode(VisualMode::VISUAL_SMALL);
    auto visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), false);
}

/*
 * @tc.name: SetVisualMode002
 * @tc.desc: test SetVisualMode with VISUAL_LARGER
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetVisualMode(VisualMode::VISUAL_LARGER);
    auto visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), false);
}

/*
 * @tc.name: SetVisualMode003
 * @tc.desc: test SetVisualMode with VISUAL_MEDIUM
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode003, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetVisualMode(VisualMode::VISUAL_MEDIUM);
    auto visualMap = style.symbol.GetVisualMap();
    EXPECT_EQ(visualMap.empty(), true);
}

/*
 * @tc.name: SetCommonSubType001
 * @tc.desc: test SetCommonSubType with DOWN
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetCommonSubType001, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetCommonSubType(Drawing::DrawingCommonSubType::DOWN);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetCommonSubType(), Drawing::DrawingCommonSubType::DOWN);
}

/*
 * @tc.name: SetCommonSubType002
 * @tc.desc: test SetCommonSubType with UP
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetCommonSubType002, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;

    style.symbol.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetCommonSubType(), Drawing::DrawingCommonSubType::UP);
}

/*
 * @tc.name: HMSymbolTxtOperator001
 * @tc.desc: test for symbol HMSymbolTxt::operator==
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, HMSymbolTxtOperator001, TestSize.Level1)
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
 * @tc.name: HMSymbolTxtOperator002
 * @tc.desc: test for symbol HMSymbolTxt::operator==
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, HMSymbolTxtOperator002, TestSize.Level1)
{
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = std::make_shared<SymbolGradient>();
    color2->SetColors({0XFF0000FF}); // 0XFF0000FF is ARGB
    SPText::SymbolColor symbolColor = {SymbolColorType::COLOR_TYPE, {color1, color2}};

    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolColor(symbolColor);
    SPText::HMSymbolTxt symbolTxt1;
    symbolTxt1.SetSymbolColor(symbolTxt.GetSymbolColor());
    EXPECT_TRUE(symbolTxt == symbolTxt1);

    auto gradients = symbolTxt.GetGradients();
    ASSERT_FALSE(gradients.empty());
    gradients[0] = nullptr;
    symbolTxt.symbolColor_.gradients = gradients;
    EXPECT_FALSE(symbolTxt == symbolTxt1);
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

/*
 * @tc.name: GetRenderColor001
 * @tc.desc: test GetRenderColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetRenderColor001, TestSize.Level1)
{
    SPText::HMSymbolTxt symbolTxt;
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = nullptr;
    SPText::SymbolColor symbolColor = {SymbolColorType::COLOR_TYPE, {color1, color2}};
    symbolTxt.SetSymbolColor(symbolColor);
    auto colors = symbolTxt.GetRenderColor();
    EXPECT_FALSE(colors.empty());
}
} // namespace Rosen
} // namespace OHOS