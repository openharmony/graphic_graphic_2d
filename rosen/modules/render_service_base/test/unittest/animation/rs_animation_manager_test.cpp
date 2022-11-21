/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_render_curve_animation.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAnimationManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t ANIMATION_ID_2 = 123456;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
};

void RSAnimationManagerTest::SetUpTestCase() {}
void RSAnimationManagerTest::TearDownTestCase() {}
void RSAnimationManagerTest::SetUp() {}
void RSAnimationManagerTest::TearDown() {}

/**
 * @tc.name: AddAnimation001
 * @tc.desc: Verify the AddAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, AddAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest AddAnimation001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    RSAnimationManager animationManager;
    animationManager.AddAnimation(renderCurveAnimation);
    EXPECT_TRUE(animationManager.GetAnimation(ANIMATION_ID) == renderCurveAnimation);
    animationManager.AddAnimation(renderCurveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest AddAnimation001 end";
}

/**
 * @tc.name: RemoveAnimation001
 * @tc.desc: Verify the RemoveAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RemoveAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RemoveAnimation001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    RSAnimationManager animationManager;
    animationManager.AddAnimation(renderCurveAnimation);
    EXPECT_TRUE(animationManager.GetAnimation(ANIMATION_ID) == renderCurveAnimation);
    animationManager.RemoveAnimation(ANIMATION_ID);
    EXPECT_TRUE(animationManager.GetAnimation(ANIMATION_ID) == nullptr);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RemoveAnimation001 end";
}

/**
 * @tc.name: RemoveAnimation002
 * @tc.desc: Verify the RemoveAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RemoveAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RemoveAnimation001 start";
    RSAnimationManager animationManager;
    animationManager.RemoveAnimation(ANIMATION_ID);
    EXPECT_TRUE(animationManager.GetAnimation(ANIMATION_ID) == nullptr);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RemoveAnimation001 end";
}

/**
 * @tc.name: FilterAnimationByPid001
 * @tc.desc: Verify the FilterAnimationByPid
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, FilterAnimationByPid001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RemoveAnimation001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    RSAnimationManager animationManager;
    animationManager.AddAnimation(renderCurveAnimation);
    animationManager.FilterAnimationByPid(ANIMATION_ID_2);
    EXPECT_TRUE(animationManager.GetAnimation(ANIMATION_ID) == renderCurveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest FilterAnimationByPid001 end";
}

/**
 * @tc.name: RegisterSpringAnimation001
 * @tc.desc: Verify the RegisterSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RegisterSpringAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RegisterSpringAnimation001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    RSAnimationManager animationManager;
    animationManager.AddAnimation(renderCurveAnimation);
    animationManager.RegisterSpringAnimation(PROPERTY_ID, ANIMATION_ID);
    auto animation = animationManager.QuerySpringAnimation(PROPERTY_ID);
    EXPECT_TRUE(animation == renderCurveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest RegisterSpringAnimation001 end";
}

/**
 * @tc.name: UnRegisterSpringAnimation001
 * @tc.desc: Verify the UnRegisterSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, UnRegisterSpringAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest UnRegisterSpringAnimation001 start";
    RSAnimationManager animationManager;
    animationManager.UnregisterSpringAnimation(PROPERTY_ID, ANIMATION_ID);
    auto animation = animationManager.QuerySpringAnimation(PROPERTY_ID);
    EXPECT_TRUE(animation == nullptr);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest UnRegisterSpringAnimation001 end";
}

/**
 * @tc.name: UnRegisterSpringAnimation002
 * @tc.desc: Verify the UnRegisterSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, UnRegisterSpringAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationManagerTest UnRegisterSpringAnimation002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    RSAnimationManager animationManager;
    animationManager.AddAnimation(renderCurveAnimation);
    animationManager.RegisterSpringAnimation(PROPERTY_ID, ANIMATION_ID);
    animationManager.UnregisterSpringAnimation(PROPERTY_ID, ANIMATION_ID);
    auto animation = animationManager.QuerySpringAnimation(PROPERTY_ID);
    EXPECT_TRUE(animation == nullptr);
    GTEST_LOG_(INFO) << "RSAnimationManagerTest UnRegisterSpringAnimation002 end";
}
} // namespace Rosen
} // namespace OHOS