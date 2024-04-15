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
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 end" ;
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
    symbolAnimation.ScaleAnimationBase(canvasNode, bounceSecondPhaseParas, groupAnimation);
        /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_FALSE(groupAnimation.empty());
    EXPECT_FALSE(groupAnimation[0] == nullptr);
    groupAnimation[0]->Start(canvasNode);
    EXPECT_TRUE(groupAnimation[0]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 end" ;
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
    symbolAnimation.ScaleAnimationBase(canvasNode, testParas, groupAnimation);
        /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_TRUE(groupAnimation.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 end" ;
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
     * @tc.steps: step2. start BounceAnimation test
     */
    symbolAnimation.AppearAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 end" ;
}
} // namespace Rosen
} // namespace OHOS