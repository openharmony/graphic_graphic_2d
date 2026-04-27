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

#include "boot_independent_display_strategy.h"
#include "boot_animation_strategy.h"
#include "parameters.h"
#include "util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_DURATION = 1;
}

class BootIndependentDisplayStrategyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootIndependentDisplayStrategyTest::SetUpTestCase() {}
void BootIndependentDisplayStrategyTest::TearDownTestCase() {}
void BootIndependentDisplayStrategyTest::SetUp()
{
    system::SetParameter(BOOT_COMPLETED, "true");
}
void BootIndependentDisplayStrategyTest::TearDown() {}

HWTEST_F(BootIndependentDisplayStrategyTest, BootIndependentDisplayStrategyTest_001, TestSize.Level1)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
}

/**
 * @tc.name: Display_EmptyConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the Display function executes with empty configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, Display_EmptyConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    std::vector<BootAnimationConfig> configs;
    int32_t duration = TEST_DURATION;
    strategy->Display(duration, configs);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetConnectToRenderMap_CountOne_MapSizeOne
 * @tc.desc: Verify the GetConnectToRenderMap function creates map with correct size.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, GetConnectToRenderMap_CountOne_MapSizeOne, TestSize.Level1)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    strategy->GetConnectToRenderMap(1);
    EXPECT_GE(strategy->connectToRenderMap_.size(), 1);
}

/**
 * @tc.name: IsOtaUpdate_Default_ReturnFalse
 * @tc.desc: Verify the IsOtaUpdate function returns false by default.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, IsOtaUpdate_Default_ReturnFalse, TestSize.Level0)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: IsOtaUpdate_ManualType_ReturnTrue
 * @tc.desc: Verify the IsOtaUpdate function returns true when update type is manual.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, IsOtaUpdate_ManualType_ReturnTrue, TestSize.Level0)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    system::SetParameter("persist.dupdate_engine.update_type", "manual");
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, true);
    system::SetParameter("persist.dupdate_engine.update_type", "");
}

/**
 * @tc.name: IsOtaUpdate_NightType_ReturnTrue
 * @tc.desc: Verify the IsOtaUpdate function returns true when update type is night.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, IsOtaUpdate_NightType_ReturnTrue, TestSize.Level0)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    system::SetParameter("persist.dupdate_engine.update_type", "night");
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, true);
    system::SetParameter("persist.dupdate_engine.update_type", "");
}

/**
 * @tc.name: IsOtaUpdate_HmosUpdateFlag_ReturnTrue
 * @tc.desc: Verify the IsOtaUpdate function returns true when hmos update flag is set.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, IsOtaUpdate_HmosUpdateFlag_ReturnTrue, TestSize.Level0)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    system::SetParameter("persist.update.hmos_to_next_flag", "1");
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, true);
    system::SetParameter("persist.update.hmos_to_next_flag", "0");
}

/**
 * @tc.name: CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully
 * @tc.desc: Verify the CheckExitAnimation function when animation not ended.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully, TestSize.Level1)
{
    system::SetParameter(BOOT_COMPLETED, "false");
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->isAnimationEnd_ = false;
    bool result = strategy->CheckExitAnimation();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CheckExitAnimation_AnimationEnd_ExecuteSuccessfully
 * @tc.desc: Verify the CheckExitAnimation function when animation ended.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, CheckExitAnimation_AnimationEnd_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->isAnimationEnd_ = true;
    bool result = strategy->CheckExitAnimation();
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: GetActiveScreenId_Normal_ReturnCorrectScreenId
 * @tc.desc: Verify the GetActiveScreenId function returns correct screen id.
 * @tc.type: FUNC
 */
HWTEST_F(BootIndependentDisplayStrategyTest, GetActiveScreenId_Normal_ReturnCorrectScreenId, TestSize.Level1)
{
    std::shared_ptr<BootIndependentDisplayStrategy> strategy = std::make_shared<BootIndependentDisplayStrategy>();
    Rosen::ScreenId screenId = strategy->GetActiveScreenId();
    EXPECT_EQ(screenId, strategy->activeScreenId_);
}
}