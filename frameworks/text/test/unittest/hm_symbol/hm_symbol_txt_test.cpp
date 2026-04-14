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
HWTEST_F(OHHmSymbolTxtTest, IsSymbolGlyph001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode003, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRenderMode004, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, GetSymbolLayers001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRenderColor001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy003, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy004, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy005, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy006, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy007, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy008, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy009, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetEffectStrategy0010, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetAnimationMode003, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetRepeatCount001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetAnimationStart001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetAnimationStart002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetVisualMode003, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetCommonSubType001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SetCommonSubType002, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, HMSymbolTxtOperator001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, HMSymbolTxtOperator002, TestSize.Level0)
{
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = std::make_shared<SymbolGradient>();
    color2->SetColors({0XFF0000FF}); // 0XFF0000FF is ARGB
    OHOS::Rosen::SymbolColor symbolColor = {SymbolColorType::COLOR_TYPE, {color1, color2}};

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
    symbolTxt1.symbolColor_.gradients = gradients;
    EXPECT_TRUE(symbolTxt == symbolTxt1);
}

/*
 * @tc.name: SymbolUid001
 * @tc.desc: test GetSymbolUid and SetSymbolUid
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SymbolUid001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, SymbolBitmap001, TestSize.Level0)
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
HWTEST_F(OHHmSymbolTxtTest, GetRenderColor001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto color2 = nullptr;
    OHOS::Rosen::SymbolColor symbolColor = {SymbolColorType::COLOR_TYPE, {color1, color2}};
    symbolTxt.SetSymbolColor(symbolColor);
    auto colors = symbolTxt.GetRenderColor();
    EXPECT_FALSE(colors.empty());
}

/*
 * @tc.name: GradientColorTest001
 * @tc.desc: test for gradient colors
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GradientColorTest001, TestSize.Level1)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    auto gradient1 = std::make_shared<SymbolGradient>();
    gradient1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto gradient2 = nullptr;
    OHOS::Rosen::SymbolColor symbolColor = {SymbolColorType::GRADIENT_TYPE, {gradient1, gradient2}};
    symbolTxt.SetSymbolColor(symbolColor);
    auto localSymbolColor = symbolTxt.GetSymbolColor();
    EXPECT_FALSE(localSymbolColor.gradients.empty());
    EXPECT_EQ(localSymbolColor.colorType, SymbolColorType::GRADIENT_TYPE); // 2 is colors size
    EXPECT_EQ(localSymbolColor.gradients.size(), 2); // 2 is colors size
}

/*
 * @tc.name: CommonColorTest001
 * @tc.desc: test for common colors
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, CommonColorTest001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    auto gradient1 = std::make_shared<SymbolGradient>();
    gradient1->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    auto gradient2 = nullptr;
    OHOS::Rosen::SymbolColor symbolColor = {SymbolColorType::COLOR_TYPE, {gradient1, gradient2}};
    symbolTxt.SetSymbolColor(symbolColor);
    auto localSymbolColor = symbolTxt.GetRenderColor();
    EXPECT_EQ(localSymbolColor.size(), 1); // 1 is colors size
}

/*
 * @tc.name: SetFirstActiveTest
 * @tc.desc: test SetFirstActive
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetFirstActiveTest, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetFirstActive(true);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetFirstActive(),  style.symbol.GetFirstActive());
}

/*
 * @tc.name: SetRenderUIColors001
 * @tc.desc: test SetRenderUIColors on interface layer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor };
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    symbolTxt.SetRenderUIColor(uiColors, colorSpaces);

    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.colorType, SymbolColorType::COLOR_TYPE);
    EXPECT_EQ(symbolColor.gradients.size(), 1); // 1 is the size of gradients
    ASSERT_TRUE(symbolColor.gradients[0] != nullptr);
    EXPECT_TRUE(symbolColor.gradients[0]->HasUIColor());
    EXPECT_EQ(symbolColor.gradients[0]->GetColorSpace(), SymbolColorSpace::DISPLAY_P3);
    EXPECT_EQ(symbolColor.gradients[0]->GetGradientType(), GradientType::NONE_GRADIENT);
}

/*
 * @tc.name: SetRenderUIColorThenRenderColor001
 * @tc.desc: test SetRenderUIColor then SetRenderColor, Color replaces UIColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColorThenRenderColor001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    // First set UIColor
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::BT2020 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);

    // Then set Drawing::Color
    Drawing::Color color1 = Drawing::Color::COLOR_BLUE;
    std::vector<Drawing::Color> colors = { color1 };
    symbolTxt.SetRenderColor(colors);

    // Verify Color replaced UIColor
    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.colorType, SymbolColorType::COLOR_TYPE);
    EXPECT_EQ(symbolColor.gradients.size(), 1); // 1 is the size of gradients
    ASSERT_TRUE(symbolColor.gradients[0] != nullptr);
    EXPECT_FALSE(symbolColor.gradients[0]->HasUIColor());
}

/*
 * @tc.name: SetRenderColorThenRenderUIColor001
 * @tc.desc: test SetRenderColor then SetRenderUIColor, UIColor replaces Color
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderColorThenRenderUIColor001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    // First set Drawing::Color
    Drawing::Color color1 = Drawing::Color::COLOR_BLUE;
    std::vector<Drawing::Color> colors = { color1 };
    symbolTxt.SetRenderColor(colors);

    // Then set UIColor
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);

    // Verify UIColor replaced Color
    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.colorType, SymbolColorType::COLOR_TYPE);
    EXPECT_EQ(symbolColor.gradients.size(), 1); // 1 is the size of gradients
    ASSERT_TRUE(symbolColor.gradients[0] != nullptr);
    EXPECT_TRUE(symbolColor.gradients[0]->HasUIColor());
    EXPECT_EQ(symbolColor.gradients[0]->GetColorSpace(), SymbolColorSpace::DISPLAY_P3);
}

/*
 * @tc.name: SetRenderUIColors002
 * @tc.desc: test SetRenderUIColors with multiple colors
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors002, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor1(1.0f, 0.0f, 0.0f, 1.0f);
    Drawing::UIColor uiColor2(0.0f, 1.0f, 0.0f, 1.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor1, uiColor2 };
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::BT2020 };
    symbolTxt.SetRenderUIColor(uiColors, colorSpaces);

    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.gradients.size(), 2); // 2 is the size of gradients
    EXPECT_TRUE(symbolColor.gradients[0]->HasUIColor());
    EXPECT_TRUE(symbolColor.gradients[1]->HasUIColor());
    EXPECT_EQ(symbolColor.gradients[0]->GetColorSpace(), SymbolColorSpace::BT2020);
    EXPECT_EQ(symbolColor.gradients[1]->GetColorSpace(), SymbolColorSpace::SRGB);
}

/*
 * @tc.name: SetRenderUIColors_Convert001
 * @tc.desc: test SetRenderUIColors converted to adapter layer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_Convert001, TestSize.Level0)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor };
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    style.symbol.SetRenderUIColor(uiColors, colorSpaces);

    SPText::TextStyle textStyle = AdapterTxt::Convert(style);
    auto uiColorResults = textStyle.symbol.GetUIColors();
    ASSERT_FALSE(uiColorResults.empty());
    EXPECT_FLOAT_EQ(uiColorResults[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetBlue(), 0.3f);
    auto csResults = textStyle.symbol.GetColorSpaces();
    ASSERT_FALSE(csResults.empty());
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);
}

/*
 * @tc.name: GetUIColors_NoneGradient001
 * @tc.desc: test GetUIColors returns value for NONE_GRADIENT with UIColor set
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetUIColors_NoneGradient001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor };
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::SRGB };
    symbolTxt.SetRenderUIColor(uiColors, colorSpaces);
    auto result = symbolTxt.GetUIColors();
    ASSERT_FALSE(result.empty());
    EXPECT_FLOAT_EQ(result[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(result[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(result[0].GetBlue(), 0.3f);
}

/*
 * @tc.name: GetUIColors_NonNoneGradient001
 * @tc.desc: test GetUIColors returns empty for LINE_GRADIENT
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetUIColors_NonNoneGradient001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    // Create a SymbolLineGradient (LINE_GRADIENT), GetUIColors should return empty
    auto lineGradient = std::make_shared<SymbolLineGradient>(45.0f); // 45.0f is angle
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    lineGradient->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    OHOS::Rosen::SymbolColor symbolColor = { SymbolColorType::COLOR_TYPE, { lineGradient } };
    symbolTxt.SetSymbolColor(symbolColor);
    auto result = symbolTxt.GetUIColors();
    EXPECT_FALSE(result.empty());
}

/*
 * @tc.name: GetUIColors_RadialGradient001
 * @tc.desc: test GetUIColors returns empty for RADIAL_GRADIENT
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetUIColors_RadialGradient001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    // Create a SymbolRadialGradient (RADIAL_GRADIENT), GetUIColors should return empty
    auto radialGradient = std::make_shared<SymbolRadialGradient>(Drawing::Point(0.5f, 0.5f), 0.6f);
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    radialGradient->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    OHOS::Rosen::SymbolColor symbolColor = { SymbolColorType::COLOR_TYPE, { radialGradient } };
    symbolTxt.SetSymbolColor(symbolColor);
    auto result = symbolTxt.GetUIColors();
    EXPECT_FALSE(result.empty());
}

/*
 * @tc.name: GetUIColors_EmptyGradients001
 * @tc.desc: test GetUIColors returns empty when gradients are empty
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetUIColors_EmptyGradients001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    // no gradients set
    auto result = symbolTxt.GetUIColors();
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: GetColorSpaces_NoneGradient001
 * @tc.desc: test GetColorSpaces for NONE_GRADIENT with UIColor set
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetColorSpaces_NoneGradient001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);
    auto csResults = symbolTxt.GetColorSpaces();
    ASSERT_FALSE(csResults.empty());
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);

    std::vector<SymbolColorSpace> colorSpaces2 = { SymbolColorSpace::BT2020 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces2);
    auto csResults2 = symbolTxt.GetColorSpaces();
    ASSERT_FALSE(csResults2.empty());
    EXPECT_EQ(csResults2[0], SymbolColorSpace::BT2020);
}

/*
 * @tc.name: GetColorSpaces_NonNoneGradient001
 * @tc.desc: test GetColorSpaces returns SRGB for LINE_GRADIENT
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, GetColorSpaces_NonNoneGradient001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    // Create a SymbolLineGradient (LINE_GRADIENT), GetColorSpaces should return SRGB
    auto lineGradient = std::make_shared<SymbolLineGradient>(45.0f); // 45.0f is angle
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    lineGradient->SetUIColors({ uiColor }, SymbolColorSpace::DISPLAY_P3);
    OHOS::Rosen::SymbolColor symbolColor = { SymbolColorType::COLOR_TYPE, { lineGradient } };
    symbolTxt.SetSymbolColor(symbolColor);
    auto csResults = symbolTxt.GetColorSpaces();
    ASSERT_FALSE(csResults.empty());
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);
    EXPECT_EQ(csResults[1], SymbolColorSpace::SRGB);
}

/*
 * @tc.name: SetRenderUIColors_SingleMode001
 * @tc.desc: test SetRenderUIColors with SINGLE render mode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_SingleMode001, TestSize.Level0)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces2 = { SymbolColorSpace::SRGB };
    style.symbol.SetRenderUIColor({ uiColor }, colorSpaces2);
    style.symbol.SetRenderMode(0); // 0 is SINGLE

    SPText::TextStyle textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    auto uiColorResults = textStyle.symbol.GetUIColors();
    ASSERT_FALSE(uiColorResults.empty());
    EXPECT_FLOAT_EQ(uiColorResults[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetBlue(), 0.3f);
}

/*
 * @tc.name: SetRenderUIColors_MultipleColorMode001
 * @tc.desc: test SetRenderUIColors with MULTIPLE_COLOR render mode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_MultipleColorMode001, TestSize.Level0)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    Drawing::UIColor uiColor1(1.0f, 0.0f, 0.0f, 1.0f);
    Drawing::UIColor uiColor2(0.0f, 1.0f, 0.0f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces2 = { SymbolColorSpace::SRGB, SymbolColorSpace::DISPLAY_P3 };
    style.symbol.SetRenderUIColor({ uiColor1, uiColor2 }, colorSpaces2);
    style.symbol.SetRenderMode(1); // 1 is MULTIPLE_COLOR

    SPText::TextStyle textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    auto symbolColor = textStyle.symbol.GetSymbolColor();
    EXPECT_EQ(symbolColor.gradients.size(), 2); // 2 is the size of gradients
    EXPECT_TRUE(symbolColor.gradients[0]->HasUIColor());
    EXPECT_TRUE(symbolColor.gradients[1]->HasUIColor());
    auto csResults = textStyle.symbol.GetColorSpaces();
    ASSERT_EQ(csResults.size(), 2); // 2 is the size of colorSpaces
    EXPECT_EQ(csResults[0], SymbolColorSpace::SRGB);
    EXPECT_EQ(csResults[1], SymbolColorSpace::DISPLAY_P3);
}

/*
 * @tc.name: SetRenderUIColors_MultipleOpacityMode001
 * @tc.desc: test SetRenderUIColors with MULTIPLE_OPACITY render mode
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_MultipleOpacityMode001, TestSize.Level0)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    style.symbol.SetRenderUIColor({ uiColor }, colorSpaces);
    style.symbol.SetRenderMode(2); // 2 is MULTIPLE_OPACITY

    SPText::TextStyle textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    auto csResults = textStyle.symbol.GetColorSpaces();
    ASSERT_FALSE(csResults.empty());
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);
}

/*
 * @tc.name: SetRenderColorThenUIColor001
 * @tc.desc: test SetRenderColor then SetRenderUIColors, UIColor replaces Color
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderColorThenUIColor001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    // First set Drawing::Color
    Drawing::Color color1 = Drawing::Color::COLOR_BLUE;
    std::vector<Drawing::Color> colors = { color1 };
    symbolTxt.SetRenderColor(colors);

    // Then set UIColor
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::BT2020 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);

    // Verify UIColor replaced Color
    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.gradients.size(), 1); // 1 is the size of gradients
    EXPECT_TRUE(symbolColor.gradients[0]->HasUIColor());
    EXPECT_EQ(symbolColor.gradients[0]->GetColorSpace(), SymbolColorSpace::BT2020);
}

/*
 * @tc.name: SetUIColorThenRenderColor001
 * @tc.desc: test SetRenderUIColors then SetRenderColor, Color replaces UIColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetUIColorThenRenderColor001, TestSize.Level0)
{
    OHOS::Rosen::HMSymbolTxt symbolTxt;
    // First set UIColor
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::BT2020 };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);

    // Then set Drawing::Color
    Drawing::Color color1 = Drawing::Color::COLOR_BLUE;
    std::vector<Drawing::Color> colors = { color1 };
    symbolTxt.SetRenderColor(colors);

    // Verify Color replaced UIColor
    auto symbolColor = symbolTxt.GetSymbolColor();
    EXPECT_EQ(symbolColor.gradients.size(), 1); // 1 is the size of gradients
    EXPECT_FALSE(symbolColor.gradients[0]->HasUIColor());
}

/*
 * @tc.name: SetRenderUIColors_GradientColor001
 * @tc.desc: test SetRenderUIColors replaces gradient color on adapter layer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_GradientColor001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    // First set gradient color
    auto lineGradient = std::make_shared<SymbolLineGradient>(90.0f); // 90.0f is angle
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    lineGradient->SetColors(colors);
    OHOS::Rosen::SymbolColor symbolColor = { SymbolColorType::GRADIENT_TYPE, { lineGradient } };
    symbolTxt.SetSymbolColor(symbolColor);

    // Then set UIColor, should replace gradient
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::SRGB };
    symbolTxt.SetRenderUIColor({ uiColor }, colorSpaces);
    auto result = symbolTxt.GetUIColors();
    ASSERT_FALSE(result.empty());
    EXPECT_FLOAT_EQ(result[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(result[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(result[0].GetBlue(), 0.3f);
}

/*
 * @tc.name: SetRenderUIColors_MultipleColorSpaces001
 * @tc.desc: test SetRenderUIColors with different colorSpaces per gradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_MultipleColorSpaces001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor1(1.0f, 0.0f, 0.0f, 1.0f);
    Drawing::UIColor uiColor2(0.0f, 1.0f, 0.0f, 1.0f);
    Drawing::UIColor uiColor3(0.0f, 0.0f, 1.0f, 1.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor1, uiColor2, uiColor3 };
    std::vector<SymbolColorSpace> colorSpaces = {
        SymbolColorSpace::DISPLAY_P3,
        SymbolColorSpace::BT2020,
        SymbolColorSpace::SRGB
    };
    symbolTxt.SetRenderUIColor(uiColors, colorSpaces);

    auto result = symbolTxt.GetUIColors();
    ASSERT_EQ(result.size(), 3); // 3 is the size of uiColors
    EXPECT_FLOAT_EQ(result[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(result[1].GetGreen(), 1.0f);
    EXPECT_FLOAT_EQ(result[2].GetBlue(), 1.0f);

    auto csResults = symbolTxt.GetColorSpaces();
    ASSERT_EQ(csResults.size(), 3); // 3 is the size of colorSpaces
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);
    EXPECT_EQ(csResults[1], SymbolColorSpace::BT2020);
    EXPECT_EQ(csResults[2], SymbolColorSpace::SRGB);
}

/*
 * @tc.name: SetRenderUIColors_HDR_Headroom_Min001
 * @tc.desc: test SetRenderUIColors with default headroom=1.0 (minimum)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_HDR_Headroom_Min001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor(0.5f, 0.3f, 0.1f, 1.0f);
    symbolTxt.SetRenderUIColor({ uiColor }, { SymbolColorSpace::SRGB });

    auto result = symbolTxt.GetUIColors();
    ASSERT_FALSE(result.empty());
    EXPECT_FLOAT_EQ(result[0].GetHeadroom(), 1.0f);
    EXPECT_FLOAT_EQ(result[0].GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(result[0].GetGreen(), 0.3f);
    EXPECT_FLOAT_EQ(result[0].GetBlue(), 0.1f);
}

/*
 * @tc.name: SetRenderUIColors_HDR_Headroom_Max001
 * @tc.desc: test SetRenderUIColors with headroom=4.0 (maximum) via SetHeadroom
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_HDR_Headroom_Max001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor(3.0f, 2.5f, 1.8f, 1.0f);
    uiColor.SetHeadroom(4.0f);
    symbolTxt.SetRenderUIColor({ uiColor }, { SymbolColorSpace::BT2020 });

    auto result = symbolTxt.GetUIColors();
    ASSERT_FALSE(result.empty());
    EXPECT_FLOAT_EQ(result[0].GetHeadroom(), 4.0f);
    EXPECT_FLOAT_EQ(result[0].GetRed(), 3.0f);
    EXPECT_FLOAT_EQ(result[0].GetGreen(), 2.5f);
    EXPECT_FLOAT_EQ(result[0].GetBlue(), 1.8f);
}

/*
 * @tc.name: SetRenderUIColors_HDR_MultiHeadroom001
 * @tc.desc: test SetRenderUIColors with multiple HDR colors having different headroom
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_HDR_MultiHeadroom001, TestSize.Level0)
{
    SPText::HMSymbolTxt symbolTxt;
    Drawing::UIColor uiColor1(1.5f, 0.8f, 0.5f, 1.0f);
    uiColor1.SetHeadroom(-1.0f);
    Drawing::UIColor uiColor2(2.0f, 1.0f, 0.3f, 1.0f);
    uiColor2.SetHeadroom(0.0f);
    std::vector<Drawing::UIColor> uiColors = { uiColor1, uiColor2 };
    std::vector<SymbolColorSpace> colorSpaces = {
        SymbolColorSpace::DISPLAY_P3,
        SymbolColorSpace::BT2020
    };
    symbolTxt.SetRenderUIColor(uiColors, colorSpaces);

    auto result = symbolTxt.GetUIColors();
    ASSERT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0].GetRed(), 1.5f);
    EXPECT_FLOAT_EQ(result[0].GetHeadroom(), 1.0f);
    EXPECT_FLOAT_EQ(result[1].GetRed(), 2.0f);
    EXPECT_FLOAT_EQ(result[1].GetHeadroom(), 1.0f);

    auto csResults = symbolTxt.GetColorSpaces();
    ASSERT_EQ(csResults.size(), 2);
    EXPECT_EQ(csResults[0], SymbolColorSpace::DISPLAY_P3);
    EXPECT_EQ(csResults[1], SymbolColorSpace::BT2020);
}

/*
 * @tc.name: SetRenderUIColors_Convert_HDR001
 * @tc.desc: test HDR UIColor with headroom through Convert
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, SetRenderUIColors_Convert_HDR001, TestSize.Level0)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    Drawing::UIColor uiColor(-1.0f, -1.0f, -1.0f, 1.5f);
    uiColor.SetHeadroom(0.9f);
    std::vector<SymbolColorSpace> colorSpaces = { SymbolColorSpace::DISPLAY_P3 };
    style.symbol.SetRenderUIColor({ uiColor }, colorSpaces);

    SPText::TextStyle textStyle = AdapterTxt::Convert(style);
    auto uiColorResults = textStyle.symbol.GetUIColors();
    ASSERT_FALSE(uiColorResults.empty());
    EXPECT_FLOAT_EQ(uiColorResults[0].GetRed(), 0.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetGreen(), 0.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetBlue(), 0.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetAlpha(), 1.0f);
    EXPECT_FLOAT_EQ(uiColorResults[0].GetHeadroom(), 1.0f);
}
} // namespace Rosen
} // namespace OHOS
