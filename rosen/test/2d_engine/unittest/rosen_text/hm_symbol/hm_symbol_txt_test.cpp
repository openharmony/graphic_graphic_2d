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
    SPText::HMSymbolRun hmSymbolRun = SPText::HMSymbolRun();
    auto symbolInfo = hmSymbolRun.GetSymbolLayers(glyphId, textStyle.symbol);
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
 * @tc.desc: test for symbol SetAminationStart
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolTxtTest, OHHmSymbolTxtTest008, TestSize.Level1)
{
    TextStyle style;
    style.isSymbolGlyph = true;
    SPText::TextStyle textStyle;
    style.symbol.SetAminationStart(true);
    textStyle = AdapterTxt::Convert(style);
    EXPECT_EQ(textStyle.symbol.GetAminationStart(), true);
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
} // namespace Rosen
} // namespace OHOS