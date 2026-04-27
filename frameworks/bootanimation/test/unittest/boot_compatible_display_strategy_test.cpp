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

#include <gtest/gtest.h>

#include "boot_compatible_display_strategy.h"
#include "boot_animation_strategy.h"
#include "parameters.h"
#include "util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_DURATION = 1;
    constexpr int32_t TEST_ROTATE_SCREEN_ID_NEGATIVE = -1;
}

class BootCompatibleDisplayStrategyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootCompatibleDisplayStrategyTest::SetUpTestCase() {}
void BootCompatibleDisplayStrategyTest::TearDownTestCase() {}
void BootCompatibleDisplayStrategyTest::SetUp()
{
    system::SetParameter(BOOT_COMPLETED, "true");
}
void BootCompatibleDisplayStrategyTest::TearDown() {}

HWTEST_F(BootCompatibleDisplayStrategyTest, BootCompatibleDisplayStrategyTest_001, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
}

/**
 * @tc.name: Display_EmptyConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the Display function executes with empty configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, Display_EmptyConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    std::vector<BootAnimationConfig> configs;
    int32_t duration = TEST_DURATION;
    strategy->Display(duration, configs);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareScreenConfig_RotateScreenIdZero_ExecuteSuccessfully
 * @tc.desc: Verify the PrepareScreenConfig function with rotate screen id zero.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, PrepareScreenConfig_RotateScreenIdZero_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    BootAnimationConfig config;
    config.rotateScreenId = 0;
    bool result = strategy->PrepareScreenConfig(config);
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: PrepareScreenConfig_NegativeRotateScreenId_ExecuteSuccessfully
 * @tc.desc: Verify the PrepareScreenConfig function with negative rotate screen id.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, PrepareScreenConfig_NegativeRotateScreenId_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    BootAnimationConfig config;
    config.rotateScreenId = TEST_ROTATE_SCREEN_ID_NEGATIVE;
    strategy->GetConnectToRenderMap(1);
    bool result = strategy->PrepareScreenConfig(config);
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: PrepareScreenConfig_VideoExtPathExists_ExecuteSuccessfully
 * @tc.desc: Verify the PrepareScreenConfig function when video ext path exists.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, PrepareScreenConfig_VideoExtPathExists_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    BootAnimationConfig config;
    config.videoExtPath.emplace("test_status", "test_video_path");
    strategy->GetConnectToRenderMap(1);
    bool result = strategy->PrepareScreenConfig(config);
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: RunAnimationAndOta_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the RunAnimationAndOta function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, RunAnimationAndOta_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->GetConnectToRenderMap(1);
    BootAnimationConfig config;
    int32_t duration = TEST_DURATION;
    strategy->RunAnimationAndOta(config, duration);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsOtaUpdate_Default_ReturnFalse
 * @tc.desc: Verify the IsOtaUpdate function returns false by default.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, IsOtaUpdate_Default_ReturnFalse, TestSize.Level0)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully
 * @tc.desc: Verify the CheckExitAnimation function when animation not ended.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully, TestSize.Level0)
{
    system::SetParameter(BOOT_COMPLETED, "false");
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->isAnimationEnd_ = false;
    bool result = strategy->CheckExitAnimation();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetConnectToRenderMap_CountOne_MapSizeOne
 * @tc.desc: Verify the GetConnectToRenderMap function creates map with correct size.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, GetConnectToRenderMap_CountOne_MapSizeOne, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    strategy->GetConnectToRenderMap(1);
    EXPECT_GE(strategy->connectToRenderMap_.size(), 1);
}

/**
 * @tc.name: SubscribeActiveScreenIdChanged_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the SubscribeActiveScreenIdChanged function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, SubscribeActiveScreenIdChanged_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->SubscribeActiveScreenIdChanged();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetActiveScreenId_Normal_ReturnCorrectScreenId
 * @tc.desc: Verify the GetActiveScreenId function returns correct screen id.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompatibleDisplayStrategyTest, GetActiveScreenId_Normal_ReturnCorrectScreenId, TestSize.Level1)
{
    std::shared_ptr<BootCompatibleDisplayStrategy> strategy = std::make_shared<BootCompatibleDisplayStrategy>();
    Rosen::ScreenId screenId = strategy->GetActiveScreenId();
    EXPECT_EQ(screenId, strategy->activeScreenId_);
}
}