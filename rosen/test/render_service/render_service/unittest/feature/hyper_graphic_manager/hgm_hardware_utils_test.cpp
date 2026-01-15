/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/hyper_graphic_manager/hgm_hardware_utils.h"
#include "hgm_core.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t MAX_SETRATE_RETRY_COUNT = 20;
constexpr ScreenId SCREEN_ID = 12;
constexpr ScreenId SCREEN_IDINVALID = 21;
}

class HgmHardwareUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmHardwareUtilsTest::SetUpTestCase() {}
void HgmHardwareUtilsTest::TearDownTestCase() {}
void HgmHardwareUtilsTest::SetUp() {}
void HgmHardwareUtilsTest::TearDown() {}

/**
 * @tc.name: ExecuteSwitchRefreshRateTest
 * @tc.desc: Test HgmHardwareUtils.ExecuteSwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, ExecuteSwitchRefreshRateTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    OutputPtr output = HdiOutput::CreateHdiOutput(SCREEN_ID);
    ASSERT_NE(output, nullptr);
    auto& hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);
    hgmCore.hgmFrameRateMgr_ = nullptr;
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);

    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);

    //  设置屏幕尺寸为1080p，物理屏尺寸包含1080p即可
    ScreenSize sSize = {720, 1080, 685, 1218};
    bool isSelfOwnedScreen = false;
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, isSelfOwnedScreen);
    auto screen = hgmCore.GetScreen(SCREEN_ID);
    screen->SetSelfOwnedScreenFlag(true);
    hgmCore.SetScreenRefreshRateImme(1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);

    hgmHardwareUtils->setRateRetryMap_[SCREEN_ID] = std::make_pair(true, 1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);
    hgmHardwareUtils->setRateRetryMap_[SCREEN_ID] = std::make_pair(false, 0);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);
    hgmCore.GetFrameRateMgr()->curScreenId_.store(hgmCore.GetFrameRateMgr()->GetLastCurScreenId());
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);
    hgmCore.GetFrameRateMgr()->curScreenId_.store(-1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(output);
    int32_t status = hgmCore.SetScreenRefreshRate(0, SCREEN_ID, 0);
    EXPECT_TRUE(status < EXEC_SUCCESS);
}

/**
 * @tc.name: UpdateRetrySetRateStatusTest
 * @tc.desc: Test HgmHardwareUtils.UpdateRetrySetRateStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, UpdateRetrySetRateStatusTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    hgmHardwareUtils->setRateRetryMap_.clear();
    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_.size(), 0);
    setRateRetryMap_.try_emplace(SCREEN_ID, std::make_pair(false, 0));
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, 0);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SUCCESS);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, 0);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, true);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, 1);
    for (int i = 1; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, true);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, i);
    }
    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, MAX_SETRATE_RETRY_COUNT);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SUCCESS);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
    ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, 0);
}

/**
 * @tc.name: PerformSetActiveModeTest
 * @tc.desc: Test HgmHardwareUtils.PerformSetActiveMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, PerformSetActiveModeTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    OutputPtr output = HdiOutput::CreateHdiOutput(SCREEN_ID);
    ASSERT_NE(output, nullptr);
    OutputPtr outputInvalid = HdiOutput::CreateHdiOutput(SCREEN_IDINVALID);
    ASSERT_NE(outputInvalid, nullptr);
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManager, nullptr);

    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).WillRepeatedly(testing::Return(-1));
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(SCREEN_ID));
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    rsScreen->supportedModes_.resize(6);
    screenManager->screens_[SCREEN_ID] = rsScreen;
    
    hgmHardwareUtils->PerformSetActiveMode(output, screenManager);

    hgmHardwareUtils->hgmRefreshRates_ = HgmRefreshRates::SET_RATE_120;
    hgmHardwareUtils->setRateRetryMap_.erase(SCREEN_ID);
    if (hgmCore.modeListToApply_ == nullptr) { 
        hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>(); 
    } 
    hgmCore.modeListToApply_->try_emplace(SCREEN_IDINVALID_, rate); 
    hardwareThread.hgmHardwareUtils_.PerformSetActiveMode(outputInvalid, screenManager);
    ASSERT_EQ(hgmCore.modeListToApply_, nullptr);

    auto &hgmCore = hgmHardwareUtils->hgmCore_;
    // 1. The number of consecutive failed retries donsnot exceed MAX_SETRATE_RETRY_COUNT
    for (int i = 0; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        if (hgmCore.modeListToApply_ == nullptr) {
            hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
        }
        hgmCore.modeListToApply_->try_emplace(SCREEN_ID, rate);
        hgmHardwareUtils->PerformSetActiveMode(output, screenManager);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, true);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, i + 1);
    }
    // 2. Disable consecutive failure retry
    for (int i = 0; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        if (hgmCore.modeListToApply_ == nullptr) {
            hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
        }
        hgmCore.modeListToApply_->try_emplace(SCREEN_ID, rate);
        hgmHardwareUtils->PerformSetActiveMode(output, screenManager);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
        ASSERT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, MAX_SETRATE_RETRY_COUNT);
    }
}

/**
 * @tc.name: TransactRefreshRateParamTest
 * @tc.desc: Test HgmHardwareUtils.TransactRefreshRateParam
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, TransactRefreshRateParamTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    uint32_t currentRate = 0;
    hgmHardwareUtils.TransactRefreshRateParam(currentRate, 60, 1, 2);
    EXPECT_NE(hgmHardwareUtils.refreshRateParam_.rate, 60);
    EXPECT_NE(hgmHardwareUtils.refreshRateParam_.frameTimestamp, 1);
    EXPECT_NE(hgmHardwareUtils.refreshRateParam_.constraintRelativeTime, 2);
}

/**
 * @tc.name: SwitchRefreshRateTest
 * @tc.desc: Test HgmHardwareUtils.SwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, SwitchRefreshRateTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    OutputPtr output = HdiOutput::CreateHdiOutput(SCREEN_ID);
    ASSERT_NE(output, nullptr);
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManager, nullptr);
    RSScreenManager* orgScmFromHgm = hgmCore.GetScreenManager();

    uint32_t currentRate = 0;
    hgmHardwareUtils.TransactRefreshRateParam(currentRate, 0, 0, 0);
    hgmHardwareUtils.SwitchRefreshRate(hdiOutput);

    hgmHardwareUtils.TransactRefreshRateParam(currentRate, 60, 0, 0);
    hgmHardwareUtils.SwitchRefreshRate(hdiOutput);
    
    ScreenSize sSize = {720, 1080, 685, 1218};
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, false);
    hgmHardwareUtils.SwitchRefreshRate(hdiOutput);

    hgmCore.RemoveScreen(SCREEN_ID, 0, sSize, false);
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, true);
    hgmHardwareUtils.SwitchRefreshRate(hdiOutput);

    hgmCore.SetScreenManager(screenManager.GetRefPtr());
    hgmHardwareUtils.SwitchRefreshRate(hdiOutput);

    if (RSSystemProperties::IsFoldDeviceOfOldDss()) {
        hgmHardwareUtils.SwitchRefreshRate(hdiOutput);

        auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(SCREEN_ID));
        rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(0);
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_ON);
        screenManager->screens_[SCREEN_ID] = rsScreen;
        hgmHardwareUtils.SwitchRefreshRate(hdiOutput);
        
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_OFF);
        hgmHardwareUtils.SwitchRefreshRate(hdiOutput);
        
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_SUSPEND);
        hgmHardwareUtils.SwitchRefreshRate(hdiOutput);
    }
    hgmCore.SetScreenManager(orgScmFromHgm);
}

/**
 * @tc.name: AddRefreshRateCountTest
 * @tc.desc: Test RSHardwareUnitTest.AddRefreshRateCount
 * @tc.type: FUNC
 * @tc.require: issueI8K4HE
 */
HWTEST_F(HgmHardwareUtilsTest, AddRefreshRateCountTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils.refreshRateCounts_.clear();
    hgmHardwareUtils.AddRefreshRateCount(SCREEN_ID);
    EXPECT_EQ(hgmHardwareUtils.refreshRateCounts_[SCREEN_ID], 1);
    auto& hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);
    hgmCore.hgmFrameRateMgr_ = nullptr;
    hgmHardwareUtils.AddRefreshRateCount(SCREEN_ID);
    EXPECT_EQ(hgmHardwareUtils.refreshRateCounts_[SCREEN_ID], 2);
    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
}

/*
 * @tc.name: RefreshRateCountsTest
 * @tc.desc: Test HgmHardwareUtils.RefreshRateCounts
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, RefreshRateCountsTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils.refreshRateCounts_.clear();
    std::string dumpString = "";
    hgmHardwareUtils.RefreshRateCounts(dumpString);
    EXPECT_EQ(dumpString, "");
    hgmHardwareUtils.AddRefreshRateCount(SCREEN_ID);
    hgmHardwareUtils.RefreshRateCounts(dumpString);
    EXPECT_NE(dumpString, "");
}

/*
 * @tc.name: ClearRefreshRateCountsTest
 * @tc.desc: Test HgmHardwareUtils.ClearRefreshRateCounts
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(HgmHardwareUtilsTest, ClearRefreshRateCountsTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils.refreshRateCounts_.clear();
    std::string dumpString = "";
    hgmHardwareUtils.ClearRefreshRateCounts(dumpString);
    EXPECT_EQ(dumpString, "");
    hgmHardwareUtils.AddRefreshRateCount(SCREEN_ID);
    EXPECT_EQ(hgmHardwareUtils.refreshRateCounts_.size(), 1);
    hgmHardwareUtils.ClearRefreshRateCounts(dumpString);
    EXPECT_EQ(hgmHardwareUtils.refreshRateCounts_.size(), 0);
    EXPECT_NE(dumpString, "");
}
} // namespace OHOS::Rosen