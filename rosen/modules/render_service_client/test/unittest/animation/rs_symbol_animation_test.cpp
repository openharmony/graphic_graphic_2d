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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_symbol_animation.h"
#include "animation/rs_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSymbolAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: InitSupportAnimationTableTest
 * @tc.desc: Verify Init of SupportAnimationTable
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, InitSupportAnimationTableTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable start";
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.InitSupportAnimationTable();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable end";
}

/**
 * @tc.name: ReplaceAnimationTest001
 * @tc.desc: Verify the basic ability, ReplaceAnimation of ReplaceAnimationTest001
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ReplaceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> disappearGroupParas =
        {DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS};
    std::vector<Drawing::DrawingPiecewiseParameter> appearGroupParas =
        {APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS};
    std::shared_ptr<RSCanvasNode> appearCanvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start BounceAnimation test
     */
    symbolAnimation.AppearAnimation(canvasNode, disappearGroupParas);
    symbolAnimation.AppearAnimation(canvasNode, appearGroupParas);

    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 end";
}

/**
 * @tc.name: PopNodeFromReplaceListTest001
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromReplaceListTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 1;
    rootNode->canvasNodesListMap[symbolSpanId] = {{canvasNode->GetId(), canvasNode}};
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 end";
}

/**
 * @tc.name: BounceAnimationTest001
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test
     */
    symbolAnimation.BounceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 end";
}

/**
 * @tc.name: BounceAnimationTest002
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.BounceAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 end";
}

/**
 * @tc.name: BounceAnimationTest003
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.BounceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 end";
}

/**
 * @tc.name: ScaleAnimationBase001
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    auto bounceSecondPhaseParas = BOUNCE_SECOND_PHASE_PARAS;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.AddScaleBaseModifier(canvasNode, bounceSecondPhaseParas, scaleProperty);
    symbolAnimation.ScaleAnimationBase(scaleProperty, bounceSecondPhaseParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_FALSE(groupAnimation.empty());
    EXPECT_FALSE(groupAnimation[0] == nullptr);
    groupAnimation[0]->Start(canvasNode);
    EXPECT_TRUE(groupAnimation[0]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 end";
}

/**
 * @tc.name: ScaleAnimationBase002
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    Drawing::DrawingPiecewiseParameter testParas = {};
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.AddScaleBaseModifier(canvasNode, testParas, scaleProperty);
    symbolAnimation.ScaleAnimationBase(scaleProperty, testParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_TRUE(groupAnimation.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 end";
}

/**
 * @tc.name: AppearAnimation001
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test
     */
    symbolAnimation.AppearAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 end";
}

/**
 * @tc.name: AppearAnimation002
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.AppearAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 end";
}

/**
 * @tc.name: AppearAnimation003
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.AppearAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation001
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(animation == nullptr);
    EXPECT_FALSE(animation->IsStarted());
    animation->Start(canvasNode);
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation002
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 start";
    /**
     * @tc.steps: step1. if no alpha data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation003
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 start";
    /**
     * @tc.steps: step1. if alphaPropertyStages_.size() != TIME_PERCENTS.size()
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size() - 1; i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(symbolAnimation.alphaPropertyStages_.size() == TIME_PERCENTS.size());
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation004
 * @tc.desc:Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(nullptr, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation005
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    EXPECT_FALSE(totalDuration == 0);
    EXPECT_FALSE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation006
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 start";
    /**
     * @tc.steps: step1. if oneGroupParas is empty
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(totalDuration == 0);
    EXPECT_TRUE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation007
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    EXPECT_FALSE(timePercents.size() == 1);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation008
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 start";
    /**
     * @tc.steps: step1. if totalDuration <= 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, 0, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation009
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 start";
    /**
     * @tc.steps: step1. if interval < 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_FIRST_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 2);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0010
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 start";
    /**
     * @tc.steps: step1.1 init data if effectStrategy is variablecolor
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(VARIABLE_COLOR_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag1 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.2 if node is nullptr
     */
    auto flag2 = symbolAnimation.SetKeyframeAlphaAnimation(nullptr, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.3 if symbolAnimationConfig is nullptr
     */
    auto flag3 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, nullptr);
    /**
     * @tc.steps: step1.4 if parameters is empty
     */
    std::vector<Drawing::DrawingPiecewiseParameter> emptyParameters = {};
    auto flag4 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, emptyParameters, symbolAnimationConfig);
    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag1 == true);
    EXPECT_TRUE(flag2 == false);
    EXPECT_TRUE(flag3 == false);
    EXPECT_TRUE(flag4 == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0011
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 start";
    /**
     * @tc.steps: step1 init data if effectStrategy is pulse
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(PULSE_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);

    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 end";
}

/**
 * @tc.name: SetSymbolGeometryTest001
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(canvasNode, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_FALSE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 end";
}

/**
 * @tc.name: SetSymbolGeometryTest002
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(nullptr, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_TRUE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 end";
}

/**
 * @tc.name: SetNodePivotTest001
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 start";
    /**
     * @tc.steps: step1. init data if scale center equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNodePivot(canvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 end";
}

/**
 * @tc.name: SetNodePivotTest002
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 start";
    /**
     * @tc.steps: step1. init data if scale center not equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto newCanvasNode = RSCanvasNode::Create();
    Vector2f pivotOffset = Vector2f(0.1f, 0.1f); // 0.1f is offset on x-axis and y-axis
    auto pivotProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(pivotOffset);
    auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty);
    newCanvasNode->AddModifier(pivotModifier);
    symbolAnimation.SetNodePivot(newCanvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 end";
}
} // namespace Rosen
} // namespace OHOS