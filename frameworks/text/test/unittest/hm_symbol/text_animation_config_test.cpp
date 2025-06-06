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
#include "symbol_engine/text_animation_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class OHTextAnimationConfigTest : public testing::Test {
public:
    static bool SetSymbolAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
};

bool OHTextAnimationConfigTest::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig == nullptr) {
        return false;
    }
    if (symbolAnimationConfig->parameters.size() < 2) {
        return false;
    }
    return true;
}

/*
 * @tc.name: DrawTextEffect001
 * @tc.desc: test for DrawTextEffect witch canvas is nullptr and no parametor
 * @tc.type: FUNC
 */
HWTEST_F(OHTextAnimationConfigTest, DrawTextEffect001, TestSize.Level1)
{
    // init data
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    Drawing::Point offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    TextEngine::TextEffectElement effectElement;
    effectElement.path = path;
    effectElement.offset = offset;
    SPText::TextAnimationConfig textEffect;
    textEffect.SetUniqueId(19); // set UniqueId as 19
    std::vector<TextEngine::TextEffectElement> effectElements;

    // test DrawTestEffect: canvas: nullptr, effectElements: empty
    bool flag1 = textEffect.DrawTextEffect(nullptr, effectElements);
    EXPECT_FALSE(flag1);

    // test DrawTestEffect: canvas: not nullptr, effectElements: empty
    bool flag2 = textEffect.DrawTextEffect(rsCanvas.get(), effectElements);
    EXPECT_FALSE(flag2);

    // test DrawTestEffect: canvas: not nullptr, effectElements: size > 0
    effectElements.push_back(effectElement);
    bool flag3 = textEffect.DrawTextEffect(rsCanvas.get(), effectElements);
    EXPECT_FALSE(flag3);

    // test DrawTestEffect: set animationStart as true
    textEffect.SetAnimationStart(true);
    bool flag4 = textEffect.DrawTextEffect(rsCanvas.get(), effectElements);
    EXPECT_FALSE(flag4);

    // test DrawTestEffect: set SetAnimation
    textEffect.SetAnimation(&SetSymbolAnimation);
    bool flag5 = textEffect.DrawTextEffect(rsCanvas.get(), effectElements);
    EXPECT_FALSE(flag5);
}

/*
 * @tc.name: DrawTextEffect002
 * @tc.desc: Test for DrawTextEffect by animation config
 * @tc.type: FUNC
 */
HWTEST_F(OHTextAnimationConfigTest, DrawTextEffect002, TestSize.Level1)
{
    // init data
    std::shared_ptr<RSCanvas> rsCanvas = std::make_shared<RSCanvas>();
    Drawing::Point offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    TextEngine::TextEffectElement effectElement;
    effectElement.path = path;
    effectElement.offset = offset;
    SPText::TextAnimationConfig textEffect;
    std::vector<TextEngine::TextEffectElement> effectElements;
    effectElements.push_back(effectElement);
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    Drawing::DrawingPiecewiseParameter flipParameter1 = {
        OHOS::Rosen::Drawing::DrawingCurveType::LINEAR, // animation curve type
        {},
        100, 0,                        // 100 is animation duration, 0 is animation delay
        {{"alpha", {1.0, 0.0}}}        // alpha is from 1 to 0
    };
    Drawing::DrawingPiecewiseParameter flipParameter2 = {
        OHOS::Rosen::Drawing::DrawingCurveType::LINEAR, // animation curve type
        {},
        150, 0,                        // 100 is animation duration, 0 is animation delay
        {{"alpha", {0, 1.0}}}        // alpha is from 1 to 0
    };
    parameters.push_back({flipParameter1});
    parameters.push_back({flipParameter2});

    // set the parameters required for the text effect
    textEffect.SetAnimationStart(true);
    textEffect.SetAnimation(&SetSymbolAnimation);
    textEffect.SetEffectStrategy(Drawing::DrawingEffectStrategy::TEXT_FLIP);
    textEffect.SetEffectConfig(parameters);
    bool flag = textEffect.DrawTextEffect(rsCanvas.get(), effectElements);
    EXPECT_TRUE(flag);
}

/*
 * @tc.name: SetColor
 * @tc.desc: Test whether the color was set successfully
 * @tc.type: FUNC
 */
HWTEST_F(OHTextAnimationConfigTest, SetColor, TestSize.Level1)
{
    // init data
    SPText::TextAnimationConfig textEffect;
    RSColor color = RSColor(255, 0, 255, 255); // set color r 255, g 0, b 255, a 255
   
    // test setColor
    textEffect.SetColor(color);
    EXPECT_TRUE(color == textEffect.color_);
}

/*
 * @tc.name: ClearAllTextAnimation
 * @tc.desc: Test ClearAllTextAnimation with invalid and valid animationFunc_
 * @tc.type: FUNC
 */
HWTEST_F(OHTextAnimationConfigTest, ClearAllTextAnimation, TestSize.Level1)
{
    // test animationFunc_ is nullptr
    SPText::TextAnimationConfig textEffect;
    textEffect.ClearAllTextAnimation();
    EXPECT_EQ(textEffect.animationFunc_, nullptr);
   
    // test animationFunc_ not is nullptr
    textEffect.SetAnimation(&SetSymbolAnimation);
    textEffect.ClearAllTextAnimation();
    EXPECT_EQ(textEffect.animationFunc_, nullptr);
}

} // namespace OHOS::Rosen