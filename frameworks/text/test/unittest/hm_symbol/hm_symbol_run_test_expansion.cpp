/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
class OHHmSymbolRunTestExpansion : public testing::Test {
public:
    static void SetSymbolDataOne(RSHMSymbolData& symbol);
    static void SetSymbolDataOneMask(RSHMSymbolData& symbol);
    static void SetSymbolDataTwo(RSHMSymbolData& symbol);
    static HMSymbolRun GetHMSymbolRunFirst();
    static HMSymbolRun GetHMSymbolRunSecond();
};

void OHHmSymbolRunTestExpansion::SetSymbolDataOne(RSHMSymbolData& symbol)
{
    RSPath path;
    path.AddCircle(100.0f, 100.0f, 50.0f); // 100.0f x, 100.0f y, 50.0f radius
    symbol.path_ = path;
    symbol.symbolInfo_.layers = {{0}}; // this one layers;
    symbol.symbolInfo_.renderGroups = {{{{{0}}}}}; // the {0} is layerIndexes of one group
}

void OHHmSymbolRunTestExpansion::SetSymbolDataOneMask(RSHMSymbolData& symbol)
{
    RSPath path;
    path.AddCircle(100, 100, 65); // 100 x, 100 y, 65 radius
    path.AddCircle(100, 100, 45); // 100 x, 100 y, 45 radius
    symbol.path_ = path;
    symbol.symbolInfo_.layers = {{0}, {1}}; // this two layers
    // the {0} is layerIndexes, {1} is maskIndexes of one group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsOneMask = {{{{{0}, {1}}}}};
    symbol.symbolInfo_.renderGroups = renderGroupsOneMask;
}

void OHHmSymbolRunTestExpansion::SetSymbolDataTwo(RSHMSymbolData& symbol)
{
    RSPath path;
    path.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100 y, 30 radius
    symbol.path_ = path;
    symbol.symbolInfo_.layers = {{0}, {1}}; // this two layers
    // the {0} {1} is layerIndexes of two group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsTwo = {{{{{0}}}}, {{{{1}}}}};
    symbol.symbolInfo_.renderGroups = renderGroupsTwo;
}

HMSymbolRun OHHmSymbolRunTestExpansion::GetHMSymbolRunFirst()
{
    const char* str = "A";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    return hmSymbolRun;
}

HMSymbolRun OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond()
{
    const char* str = "C";
    Drawing::Font font;
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return false;
        };
    HMSymbolRun hmSymbolRun = HMSymbolRun(0, symbolTxt, textblob, animationFunc);
    return hmSymbolRun;
}

/*
 * @tc.name: OnDrawSymbolTestShadow001
 * @tc.desc: test DrawSymbolShadow with radius = 0
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, OnDrawSymbolTestShadow001, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {10.0f, 10.0f}; // 10.0f, 10.0f is the offset
    RSHMSymbolData symbol;
    OHHmSymbolRunTestExpansion::SetSymbolDataOne(symbol);

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {5.0f, 5.0f} is offsetXY, 0.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {5.0f, 5.0f}, 0.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: OnDrawSymbolTestShadow002
 * @tc.desc: test DrawSymbolShadow with radius < 1
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, OnDrawSymbolTestShadow002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {10.0f, 10.0f}; // 10.0f, 10.0f is the offset
    RSHMSymbolData symbol;
    OHHmSymbolRunTestExpansion::SetSymbolDataOne(symbol);

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {5.0f, 5.0f} is offsetXY, -0.5f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {5.0f, 5.0f}, -0.5f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: OnDrawSymbolTestShadow003
 * @tc.desc: test DrawSymbolShadow with radius > 1
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, OnDrawSymbolTestShadow003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {10.0f, 10.0f}; // 10.0f, 10.0f is the offset
    RSHMSymbolData symbol;
    OHHmSymbolRunTestExpansion::SetSymbolDataOneMask(symbol);

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {30.0f, 30.0f} is offsetXY, 10.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {30.0f, 30.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());

    // test blur radius < 1.0f
    shadow.blurRadius = 0.5f;
    symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: OnDrawSymbolTestShadow004
 * @tc.desc: test DrawSymbolShadow with two groups
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, OnDrawSymbolTestShadow004, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSHMSymbolData symbol;
    OHHmSymbolRunTestExpansion::SetSymbolDataTwo(symbol);

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {10.0f, 10.0f} is offsetXY, 8.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLUE, {10.0f, 10.0f}, 8.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: OnDrawSymbolTestShadow005
 * @tc.desc: test DrawSymbolShadow with empty shadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, OnDrawSymbolTestShadow005, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    RSPoint offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSHMSymbolData symbol;
    OHHmSymbolRunTestExpansion::SetSymbolDataTwo(symbol);

    HMSymbolRun hmSymbolRun = HMSymbolRun();
    std::optional<SymbolShadow> symbolShadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    hmSymbolRun.OnDrawSymbol(rsCanvas.get(), symbol, offset);
    EXPECT_FALSE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: SetSymbolEffectByShadow001
 * @tc.desc: test SetSymbolEffect by shadow with DrawingEffectStrategy::NONE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolEffectByShadow001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond();;
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::NONE);
    hmSymbolRun.currentAnimationHasPlayed_ = true;

    // {0.0f, 0.0f} is offsetXY, 20.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {0.0f, 0.0f}, 20.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffectByShadow002
 * @tc.desc: test SetSymbolEffect by shadow with DrawingEffectStrategy::SCALE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolEffectByShadow002, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunFirst();
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::SCALE);
    RSPoint paint = {50, 50}; // 50, 50 is the offset
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);

    // {5.0f, 5.0f} is offsetXY, 10.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {5.0f, 5.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffectByShadow003
 * @tc.desc: test SetSymbolEffect by invlid shadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolEffectByShadow003, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunFirst();
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::BOUNCE);
    hmSymbolRun.currentAnimationHasPlayed_ = false;
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::BOUNCE);


    // {5.0f, 5.0f} is offsetXY, -10.0f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {5.0f, 5.0f}, -10.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    RSPoint paint = {100, 100}; // 100, 100 is the offset
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint);
    EXPECT_TRUE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetSymbolEffectByShadow004
 * @tc.desc: test SetSymbolEffect by shadow with DrawingEffectStrategy::REPLACE_APPEAR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolEffectByShadow004, TestSize.Level1)
{
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunFirst();
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(Drawing::DrawingEffectStrategy::REPLACE_APPEAR);
    RSPoint paint = {100, 100}; // 100, 100 is the offset
    hmSymbolRun.DrawSymbol(rsCanvas.get(), paint);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetEffectStrategy(), Drawing::DrawingEffectStrategy::REPLACE_APPEAR);

    // {5.0f, 5.0f} is offsetXY, 0.5f is blur radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {5.0f, 5.0f}, 0.5f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
}

/*
 * @tc.name: SetSymbolShadow001
 * @tc.desc: test SetSymbolShadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolShadow001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {10.0f, 10.0f} is offsetXY, 1.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {10.0f, 10.0f}, 1.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    ASSERT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());

    // {20.0f, 20.0f} is offsetXY, 0.0f is radius
    SymbolShadow shadow1 = {Drawing::Color::COLOR_RED, {20.0f, 20.0f}, 10.0f};
    std::optional<SymbolShadow> symbolShadow1 = shadow1;
    hmSymbolRun.SetSymbolShadow(symbolShadow1);
    ASSERT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetSymbolShadow().value().color, Drawing::Color::COLOR_RED);
}

/*
 * @tc.name: SetSymbolShadow002
 * @tc.desc: test SetSymbolShadow
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetSymbolShadow002, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = HMSymbolRun();
    // {0.0f, 0.0f} is offsetXY, 0.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLUE, {0.0f, 0.0f}, 0.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    ASSERT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());

    // {2.0f, 2.0f} is offsetXY, -1.0f is radius
    SymbolShadow shadow1 = {Drawing::Color::COLOR_YELLOW, {2.0f, 2.0f}, -1.0f};
    std::optional<SymbolShadow> symbolShadow1 = shadow1;
    hmSymbolRun.SetSymbolShadow(symbolShadow1);
    ASSERT_TRUE(hmSymbolRun.symbolTxt_.GetSymbolShadow().has_value());
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetSymbolShadow().value().color, Drawing::Color::COLOR_YELLOW);
}

/*
 * @tc.name: SetRenderModeByShadow001
 * @tc.desc: test SetRenderMode by shadow with SINGLE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetRenderModeByShadow001, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond();
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);

    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::SINGLE);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::SINGLE);

    // {10.0f, 01.0f} is offsetXY, 0.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLUE, {10.0f, 10.0f}, 0.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetRenderMode002
 * @tc.desc: test SetRenderMode by shadow with MULTIPLE_COLOR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetRenderMode002, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond();
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);

    // {10.0f, 10.0f} is offsetXY, 1.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLUE, {10.0f, 10.0f}, 1.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetRenderMode003
 * @tc.desc: test SetRenderMode by shadow with MULTIPLE_COLOR
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetRenderMode003, TestSize.Level1)
{
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond();
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR);

    // {10.0f, 10.0f} is offsetXY, -1.0f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLUE, {10.0f, 10.0f}, -1.0f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}

/*
 * @tc.name: SetRenderMode004
 * @tc.desc: test SetRenderMode by shadow with MULTIPLE_OPACITY
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolRunTestExpansion, SetRenderMode004, TestSize.Level0)
{
    HMSymbolRun hmSymbolRun = OHHmSymbolRunTestExpansion::GetHMSymbolRunSecond();
    hmSymbolRun.SetAnimationStart(true);
    hmSymbolRun.SetSymbolEffect(RSEffectStrategy::BOUNCE);
    hmSymbolRun.currentAnimationHasPlayed_ = true;
    hmSymbolRun.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    EXPECT_EQ(hmSymbolRun.symbolTxt_.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);

    // {30.0f, 30.0f} is offsetXY, 0.5f is radius
    SymbolShadow shadow = {Drawing::Color::COLOR_BLACK, {30.0f, 30.0f}, 0.5f};
    std::optional<SymbolShadow> symbolShadow = shadow;
    hmSymbolRun.SetSymbolShadow(symbolShadow);
    EXPECT_FALSE(hmSymbolRun.currentAnimationHasPlayed_);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS