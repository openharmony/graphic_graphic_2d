/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_manager.h"
#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierManagerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierManagerUnitTest::SetUpTestCase() {}
void RSModifierManagerUnitTest::TearDownTestCase() {}
void RSModifierManagerUnitTest::SetUp() {}
void RSModifierManagerUnitTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContextTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerUnitTest, CreateDrawingContextTest001, TestSize.Level1)
{
    auto manager = std::make_shared<RSModifierManager>();
    manager->Draw();
    EXPECT_TRUE(manager);
}

/**
 * @tc.name: GetFrameRateRangeTest001
 * @tc.desc: test results of GetFrameRateRange
 * @tc.type: FUNC
 * @tc.require: issueI9VXLH
 */
HWTEST_F(RSModifierManagerUnitTest, GetFrameRateRangeTest001, TestSize.Level1)
{
    auto manager = std::make_shared<RSModifierManager>();
    manager->GetFrameRateRange();
    EXPECT_FALSE(manager->rateDecider_.frameRateRange_.min_);

    manager->GetFrameRateRange();
    EXPECT_TRUE(manager);
}

/**
 * @tc.name: RemoveAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerUnitTest, RemoveAnimationTest001, TestSize.Level1)
{
    uint64_t ANIMATION_ID = 100;
    uint64_t PROPERTY_ID = 101;
    uint64_t PREFERRED = 120;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    AnimationId key = renderCurveAnimation->GetAnimationId();

    RSModifierManager rsModifierManager;
    rsModifierManager.SetFrameRateGetFunc([PREFERRED](const RSPropertyUnit unit,
        float velocity, int32_t area, int32_t length) -> int32_t { return PREFERRED; });
    rsModifierManager.AddAnimation(renderCurveAnimation);
    ASSERT_TRUE(rsModifierManager.GetAndResetFirstFrameAnimationState());
    ASSERT_FALSE(rsModifierManager.GetAndResetFirstFrameAnimationState());
    renderCurveAnimation->Start();
    ASSERT_EQ(rsModifierManager.HasUIRunningAnimation(), true);
    rsModifierManager.RemoveAnimation(key);
    ASSERT_EQ(rsModifierManager.HasUIRunningAnimation(), false);
}

/**
 * @tc.name: IsDisplaySyncEnabled001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerUnitTest, IsDisplaySyncEnabled001, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), false);
    rsModifierManager.SetDisplaySyncEnable(true);
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), true);
}

/**
 * @tc.name: JudgeAnimateWhetherSkip001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerUnitTest, JudgeAnimateWhetherSkip001, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    AnimationId animId = 0;
    int64_t time = 1111111111111;
    int64_t vsyncPeriod = 11718750;
    auto isSkip = rsModifierManager.JudgeAnimateWhetherSkip(animId, time, vsyncPeriod);
    ASSERT_EQ(isSkip, false);
}

/**
 * @tc.name: UnregisterSpringAnimationTest002
 * @tc.desc: test results of UnregisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierManagerUnitTest, UnregisterSpringAnimationTest002, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsModifierManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}
} // namespace OHOS::Rosen