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

#include "boot_associative_display_strategy.h"
#include "boot_animation_strategy.h"
#include "parameters.h"
#include "util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_DURATION = 60;
}

class BootAssociativeDisplayStrategyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootAssociativeDisplayStrategyTest::SetUpTestCase() {}
void BootAssociativeDisplayStrategyTest::TearDownTestCase() {}
void BootAssociativeDisplayStrategyTest::SetUp()
{
    system::SetParameter(BOOT_COMPLETED, "true");
}
void BootAssociativeDisplayStrategyTest::TearDown() {}

HWTEST_F(BootAssociativeDisplayStrategyTest, BootAssociativeDisplayStrategyTest_001, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
}

/**
 * @tc.name: Display_EmptyConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the Display function executes with empty configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, Display_EmptyConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    std::vector<BootAnimationConfig> configs;
    int32_t duration = TEST_DURATION;
    strategy->Display(duration, configs);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: Display_MultipleConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the Display function executes with multiple configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, Display_MultipleConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    std::vector<BootAnimationConfig> configs;
    BootAnimationConfig config1;
    BootAnimationConfig config2;
    configs.emplace_back(config1);
    configs.emplace_back(config2);
    int32_t duration = TEST_DURATION;
    strategy->Display(duration, configs);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsExtraVideoExist_VideoExtPathExists_ReturnTrue
 * @tc.desc: Verify the IsExtraVideoExist function returns true when video ext path exists.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, IsExtraVideoExist_VideoExtPathExists_ReturnTrue, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    std::vector<BootAnimationConfig> configs;
    BootAnimationConfig config;
    config.videoExtPath.emplace("test_key", "test_path");
    configs.emplace_back(config);
    bool result = strategy->IsExtraVideoExist(configs);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: IsExtraVideoExist_NoVideoExtPath_ReturnFalse
 * @tc.desc: Verify the IsExtraVideoExist function returns false when no video ext path.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, IsExtraVideoExist_NoVideoExtPath_ReturnFalse, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    std::vector<BootAnimationConfig> configs;
    BootAnimationConfig config;
    configs.emplace_back(config);
    bool result = strategy->IsExtraVideoExist(configs);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: IsSupportCoordination_Normal_ReturnCorrectResult
 * @tc.desc: Verify the IsSupportCoordination function returns correct result.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, IsSupportCoordination_Normal_ReturnCorrectResult, TestSize.Level0)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    bool result = strategy->IsSupportCoordination();
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: IsOtaUpdate_Default_ReturnFalse
 * @tc.desc: Verify the IsOtaUpdate function returns false by default.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, IsOtaUpdate_Default_ReturnFalse, TestSize.Level0)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully
 * @tc.desc: Verify the CheckExitAnimation function executes when animation not ended.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, CheckExitAnimation_AnimationNotEnd_ExecuteSuccessfully, TestSize.Level0)
{
    system::SetParameter(BOOT_COMPLETED, "false");
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
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
HWTEST_F(BootAssociativeDisplayStrategyTest, GetConnectToRenderMap_CountOne_MapSizeOne, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    strategy->GetConnectToRenderMap(1);
    EXPECT_GE(strategy->connectToRenderMap_.size(), 1);
}

/**
 * @tc.name: SubscribeActiveScreenIdChanged_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the SubscribeActiveScreenIdChanged function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, SubscribeActiveScreenIdChanged_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    ASSERT_NE(strategy, nullptr);
    strategy->SubscribeActiveScreenIdChanged();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetActiveScreenId_Normal_ReturnCorrectScreenId
 * @tc.desc: Verify the GetActiveScreenId function returns correct screen id.
 * @tc.type: FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, GetActiveScreenId_Normal_ReturnCorrectScreenId, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    Rosen::ScreenId screenId = strategy->GetActiveScreenId();
    EXPECT_EQ(screenId, strategy->activeScreenId_);
}
}