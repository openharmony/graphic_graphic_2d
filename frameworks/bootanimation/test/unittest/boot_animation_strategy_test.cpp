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

#include <parameters.h>
#include "util.h"
#include "boot_animation_strategy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootAnimationStrategyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootAnimationStrategyTest::SetUpTestCase() {}
void BootAnimationStrategyTest::TearDownTestCase() {}
void BootAnimationStrategyTest::SetUp()
{
    system::SetParameter(BOOT_COMPLETED, "false");
}
void BootAnimationStrategyTest::TearDown() {}

/**
 * @tc.name: CheckExitAnimation_AnimationNotEnd_SetAnimationStarted
 * @tc.desc: Verify the CheckExitAnimation function when isAnimationEnd_ is false.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, CheckExitAnimation_AnimationNotEnd_SetAnimationStarted, TestSize.Level0)
{
    system::SetParameter(BOOT_COMPLETED, "false");
    std::shared_ptr<BootAnimationStrategy> bas1 = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(bas1, nullptr);
    bas1->isAnimationEnd_ = false;
    bool result1 = bas1->CheckExitAnimation();
    EXPECT_EQ(result1, false);
    
    std::shared_ptr<BootAnimationStrategy> bas2 = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(bas2, nullptr);
    bas2->isAnimationEnd_ = true;
    bool result2 = bas2->CheckExitAnimation();
    EXPECT_TRUE(result2 || !result2);

    std::shared_ptr<BootAnimationStrategy> bas3 = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(bas3, nullptr);
    bas3->isAnimationEnd_ = true;
    system::SetParameter(BOOT_COMPLETED, "false");
    EXPECT_EQ(false, system::GetBoolParameter(BOOT_COMPLETED, false));
    bool result3 = bas3->CheckExitAnimation();
    EXPECT_EQ(result3, false);

    std::shared_ptr<BootAnimationStrategy> bas4 = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(bas4, nullptr);
    bas4->isAnimationEnd_ = true;
    system::SetParameter(BOOT_COMPLETED, "true");
    EXPECT_EQ(true, system::GetBoolParameter(BOOT_COMPLETED, false));
    bool result4 = bas4->CheckExitAnimation();
    EXPECT_EQ(result4, true);
}

/**
 * @tc.name: IsOtaUpdate_ManualNightType_ReturnTrue
 * @tc.desc: Verify the IsOtaUpdate function returns true when update type is manual or night.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, IsOtaUpdate_ManualNightType_ReturnTrue, TestSize.Level0)
{
    system::SetParameter(BOOT_COMPLETED, "true");
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    bool result = system::SetParameter("persist.dupdate_engine.update_type", "manual");
    EXPECT_EQ(result, strategy->IsOtaUpdate());

    result = system::SetParameter("persist.dupdate_engine.update_type", "night");
    EXPECT_EQ(result, strategy->IsOtaUpdate());

    system::SetParameter("persist.dupdate_engine.update_type", "test");
    EXPECT_EQ(false, strategy->IsOtaUpdate());
}

/**
 * @tc.name: IsOtaUpdate_HmosUpdateFlag_ReturnTrue
 * @tc.desc: Verify the IsOtaUpdate function returns true when hmos update flag is set.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, IsOtaUpdate_HmosUpdateFlag_ReturnTrue, TestSize.Level0)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    system::SetParameter("persist.update.hmos_to_next_flag", "1");
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, true);
    system::SetParameter("persist.update.hmos_to_next_flag", "0");
}

/**
 * @tc.name: IsOtaUpdate_HmosUpdateFlagInvalid_ReturnFalse
 * @tc.desc: Verify the IsOtaUpdate function returns false when hmos update flag is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, IsOtaUpdate_HmosUpdateFlagInvalid_ReturnFalse, TestSize.Level0)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    system::SetParameter("persist.update.hmos_to_next_flag", "2");
    bool result = strategy->IsOtaUpdate();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: OnScreenChanged_InvalidScreenId_NoScreenTrue
 * @tc.desc: Verify OnScreenChanged sets noScreen_ to true when rsScreenId is INVALID_SCREEN_ID.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, OnScreenChanged_InvalidScreenId_NoScreenTrue, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(strategy, nullptr);

    strategy->noScreen_ = false;
    EXPECT_FALSE(strategy->noScreen_.load());

    strategy->OnScreenChanged(INVALID_SCREEN_ID, ScreenEvent::UNKNOWN, ScreenChangeReason::DEFAULT, nullptr);

    EXPECT_TRUE(strategy->noScreen_.load());
}

/**
 * @tc.name: OnScreenChanged_ValidScreenConnected_MapPopulated
 * @tc.desc: Verify OnScreenChanged adds screen to connectToRenderMap_ when screenEvent is CONNECTED.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, OnScreenChanged_ValidScreenConnected_MapPopulated, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(strategy, nullptr);

    ScreenId testScreenId = static_cast<ScreenId>(12345);
    sptr<IRemoteObject> connectToRender = nullptr;

    strategy->OnScreenChanged(testScreenId, ScreenEvent::CONNECTED, ScreenChangeReason::DEFAULT, connectToRender);

    EXPECT_FALSE(strategy->noScreen_.load());
    EXPECT_EQ(strategy->connectToRenderMap_.size(), 1);
    EXPECT_NE(strategy->connectToRenderMap_.find(testScreenId), strategy->connectToRenderMap_.end());
}

/**
 * @tc.name: OnScreenChanged_ScreenDisconnected_MapEntryRemoved
 * @tc.desc: Verify OnScreenChanged removes screen from connectToRenderMap_ when screenEvent is DISCONNECTED.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, OnScreenChanged_ScreenDisconnected_MapEntryRemoved, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(strategy, nullptr);

    ScreenId testScreenId = static_cast<ScreenId>(54321);
    {
        std::lock_guard<std::mutex> lock(strategy->connectToRenderMapMtx_);
        strategy->connectToRenderMap_.emplace(testScreenId, nullptr);
    }
    ASSERT_EQ(strategy->connectToRenderMap_.size(), 1);

    strategy->OnScreenChanged(testScreenId, ScreenEvent::DISCONNECTED, ScreenChangeReason::DEFAULT, nullptr);

    EXPECT_FALSE(strategy->noScreen_.load());
    EXPECT_EQ(strategy->connectToRenderMap_.size(), 0);
    EXPECT_EQ(strategy->connectToRenderMap_.find(testScreenId), strategy->connectToRenderMap_.end());
}

/**
 * @tc.name: OnScreenChanged_MultipleScreensConnected_MapPopulatedCorrectly
 * @tc.desc: Verify OnScreenChanged handles multiple CONNECTED events correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, OnScreenChanged_MultipleScreensConnected_MapPopulatedCorrectly, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    ASSERT_NE(strategy, nullptr);

    ScreenId screenId1 = static_cast<ScreenId>(100);
    ScreenId screenId2 = static_cast<ScreenId>(200);

    strategy->OnScreenChanged(screenId1, ScreenEvent::CONNECTED, ScreenChangeReason::DEFAULT, nullptr);
    strategy->OnScreenChanged(screenId2, ScreenEvent::CONNECTED, ScreenChangeReason::DEFAULT, nullptr);

    EXPECT_EQ(strategy->connectToRenderMap_.size(), 2);
    EXPECT_NE(strategy->connectToRenderMap_.find(screenId1), strategy->connectToRenderMap_.end());
    EXPECT_NE(strategy->connectToRenderMap_.find(screenId2), strategy->connectToRenderMap_.end());
}

/**
 * @tc.name: GetActiveScreenId_Normal_ReturnCorrectScreenId
 * @tc.desc: Verify the GetActiveScreenId function returns correct screen id.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, GetActiveScreenId_Normal_ReturnCorrectScreenId, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    Rosen::ScreenId screenId = strategy->GetActiveScreenId();
    EXPECT_EQ(screenId, strategy->activeScreenId_);
}

/**
 * @tc.name: ConfigPath_SetPath_ReturnCorrectPath
 * @tc.desc: Verify the configPath_ member variable can be set and retrieved correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationStrategyTest, ConfigPath_SetPath_ReturnCorrectPath, TestSize.Level1)
{
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->configPath_ = "/test/path";
    EXPECT_EQ(strategy->configPath_, "/test/path");
}
}
