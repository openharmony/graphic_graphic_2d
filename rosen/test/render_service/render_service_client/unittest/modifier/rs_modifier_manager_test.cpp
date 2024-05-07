/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
class RSModifierManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierManagerTest::SetUpTestCase() {}
void RSModifierManagerTest::TearDownTestCase() {}
void RSModifierManagerTest::SetUp() {}
void RSModifierManagerTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, CreateDrawingContextTest, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    rsModifierManager.Draw();
}

/**
 * @tc.name: RemoveAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RemoveAnimationTest, TestSize.Level1)
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
    rsModifierManager.SetFrameRateGetFunc(
        [PREFERRED](const RSPropertyUnit unit, float velocity) -> int32_t {return PREFERRED;});
    rsModifierManager.AddAnimation(renderCurveAnimation);
    ASSERT_EQ(rsModifierManager.HasUIAnimation(), true);
    rsModifierManager.RemoveAnimation(key);
    ASSERT_EQ(rsModifierManager.HasUIAnimation(), false);
}

/**
 * @tc.name: IsDisplaySyncEnabled
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, IsDisplaySyncEnabled, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), false);
    rsModifierManager.SetDisplaySyncEnable(true);
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), true);
}

/**
 * @tc.name: JudgeAnimateWhetherSkip
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, JudgeAnimateWhetherSkip, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    AnimationId animId = 0;
    int64_t time = 1111111111111;
    int64_t vsyncPeriod = 11718750;
    auto isSkip = rsModifierManager.JudgeAnimateWhetherSkip(animId, time, vsyncPeriod);
    ASSERT_EQ(isSkip, false);
}

/**
 * @tc.name: UnregisterSpringAnimationTest001
 * @tc.desc: test results of UnregisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierManagerTest, UnregisterSpringAnimationTest001, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsModifierManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}
} // namespace OHOS::Rosen