/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "hdi_output.h"
#include "hgm_core.h"
#include "pipeline/mock/mock_hdi_device.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_manager.h"

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
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void HgmHardwareUtilsTest::SetUpTestCase()
{
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
}

void HgmHardwareUtilsTest::TearDownTestCase()
{
    hdiDeviceMock_ = nullptr;
}

void HgmHardwareUtilsTest::SetUp() {}
void HgmHardwareUtilsTest::TearDown() {}

/**
 * @tc.name: ExecuteSwitchRefreshRateTest
 * @tc.desc: test HgmHardwareUtils.ExecuteSwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, ExecuteSwitchRefreshRateTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    auto& hgmCore = hgmHardwareUtils->hgmCore_;
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    hgmCore.hgmFrameRateMgr_ = nullptr;
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);

    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);

    //  设置屏幕尺寸为1080p，物理屏尺寸包含1080p即可
    ScreenSize sSize = {720, 1080, 685, 1218};
    bool isSelfOwnedScreen = false;
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, isSelfOwnedScreen);
    hgmCore.SetScreenRefreshRateImme(1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);

    hgmHardwareUtils->setRateRetryMap_[SCREEN_ID] = std::make_pair(true, 1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);
    hgmHardwareUtils->setRateRetryMap_[SCREEN_ID] = std::make_pair(false, 0);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);
    hgmCore.GetFrameRateMgr()->curScreenId_.store(hgmCore.GetFrameRateMgr()->GetLastCurScreenId());
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);
    hgmCore.GetFrameRateMgr()->curScreenId_.store(-1);
    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);
    int32_t status = hgmCore.SetScreenRefreshRate(0, SCREEN_ID, 0);
    EXPECT_TRUE(status < EXEC_SUCCESS);
}

/**
 * @tc.name: UpdateRetrySetRateStatusTest
 * @tc.desc: test HgmHardwareUtils.UpdateRetrySetRateStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, UpdateRetrySetRateStatusTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils->setRateRetryMap_.clear();
    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    EXPECT_TRUE(hgmHardwareUtils->setRateRetryMap_.empty());
    auto [rateRetryMapIter, success] =
        hgmHardwareUtils->setRateRetryMap_.try_emplace(SCREEN_ID, std::make_pair(false, 0));
    auto& rateRetryData = rateRetryMapIter->second;
    EXPECT_EQ(rateRetryData.first, false);
    EXPECT_EQ(rateRetryData.second, 0);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SUCCESS);
    EXPECT_EQ(rateRetryData.first, false);
    EXPECT_EQ(rateRetryData.second, 0);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    EXPECT_EQ(rateRetryData.first, true);
    EXPECT_EQ(rateRetryData.second, 1);
    for (int i = 1; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
        EXPECT_EQ(rateRetryData.first, true);
        EXPECT_EQ(rateRetryData.second, i + 1);
    }
    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SET_RATE_ERROR);
    EXPECT_EQ(rateRetryData.first, false);
    EXPECT_EQ(rateRetryData.second, MAX_SETRATE_RETRY_COUNT);

    hgmHardwareUtils->UpdateRetrySetRateStatus(SCREEN_ID, 1, StatusCode::SUCCESS);
    EXPECT_EQ(rateRetryData.first, false);
    EXPECT_EQ(rateRetryData.second, 0);
}

/**
 * @tc.name: PerformSetActiveModeTest
 * @tc.desc: test HgmHardwareUtils.PerformSetActiveMode
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
    auto& hgmCore = hgmHardwareUtils->hgmCore_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).WillRepeatedly(testing::Return(-1));

    auto rsScreen = std::make_shared<RSScreen>(SCREEN_ID);
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(SCREEN_ID);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    rsScreen->supportedModes_.resize(6);
    screenManager->screens_[SCREEN_ID] = rsScreen;
    
    hgmHardwareUtils->PerformSetActiveMode(output, screenManager);

    hgmHardwareUtils->hgmRefreshRates_ = HgmRefreshRates::SET_RATE_120;
    hgmHardwareUtils->setRateRetryMap_.erase(SCREEN_ID);
    if (hgmCore.modeListToApply_ == nullptr) {
        hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
    } 
    hgmCore.modeListToApply_->try_emplace(SCREEN_IDINVALID, 3);
    hgmHardwareUtils->PerformSetActiveMode(outputInvalid, screenManager);
    EXPECT_EQ(hgmCore.modeListToApply_, nullptr);

    // 1. The number of consecutive failed retries donsnot exceed MAX_SETRATE_RETRY_COUNT
    for (int i = 0; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        if (hgmCore.modeListToApply_ == nullptr) {
            hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
        }
        hgmCore.modeListToApply_->try_emplace(SCREEN_ID, 3);
        hgmHardwareUtils->PerformSetActiveMode(output, screenManager);
        EXPECT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, true);
        EXPECT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, i + 1);
    }
    // 2. Disable consecutive failure retry
    for (int i = 0; i < MAX_SETRATE_RETRY_COUNT; ++i) {
        if (hgmCore.modeListToApply_ == nullptr) {
            hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
        }
        hgmCore.modeListToApply_->try_emplace(SCREEN_ID, 3);
        hgmHardwareUtils->PerformSetActiveMode(output, screenManager);
        EXPECT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].first, false);
        EXPECT_EQ(hgmHardwareUtils->setRateRetryMap_[SCREEN_ID].second, MAX_SETRATE_RETRY_COUNT);
    }
}

/**
 * @tc.name: TransactRefreshRateParamTest
 * @tc.desc: test HgmHardwareUtils.TransactRefreshRateParam
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, TransactRefreshRateParamTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    uint32_t currentRate = 0;
    hgmHardwareUtils->TransactRefreshRateParam(currentRate, 60, 1, 2);
    EXPECT_EQ(hgmHardwareUtils->refreshRateParam_.rate, 60);
    EXPECT_EQ(hgmHardwareUtils->refreshRateParam_.frameTimestamp, 1);
    EXPECT_EQ(hgmHardwareUtils->refreshRateParam_.constraintRelativeTime, 2);
}

/**
 * @tc.name: SwitchRefreshRateTest
 * @tc.desc: test HgmHardwareUtils.SwitchRefreshRate
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
    auto& hgmCore = hgmHardwareUtils->hgmCore_;
    RSScreenManager* orgScmFromHgm = hgmCore.GetScreenManager();

    uint32_t currentRate = 0;
    hgmHardwareUtils->TransactRefreshRateParam(currentRate, 0, 0, 0);
    hgmHardwareUtils->SwitchRefreshRate(output);

    hgmHardwareUtils->TransactRefreshRateParam(currentRate, 60, 0, 0);
    hgmHardwareUtils->SwitchRefreshRate(output);
    bool isSelfOwnedScreen = false;
    ScreenSize sSize = {720, 1080, 685, 1218};
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, isSelfOwnedScreen);
    hgmHardwareUtils->SwitchRefreshRate(output);

    hgmCore.RemoveScreen(SCREEN_ID);
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, isSelfOwnedScreen);
    auto screen = hgmCore.GetScreen(SCREEN_ID);
    screen->SetSelfOwnedScreenFlag(true);
    hgmHardwareUtils->SwitchRefreshRate(output);

    hgmCore.SetScreenManager(screenManager.GetRefPtr());
    hgmHardwareUtils->SwitchRefreshRate(output);

    if (RSSystemProperties::IsFoldDeviceOfOldDss()) {
        hgmHardwareUtils->SwitchRefreshRate(output);

        auto rsScreen = std::make_shared<RSScreen>(SCREEN_ID);
        rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(SCREEN_ID);
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_ON);
        screenManager->screens_[SCREEN_ID] = rsScreen;
        hgmHardwareUtils->SwitchRefreshRate(output);
        
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_OFF);
        hgmHardwareUtils->SwitchRefreshRate(output);
        
        rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_SUSPEND);
        hgmHardwareUtils->SwitchRefreshRate(output);
    }
    hgmCore.SetScreenManager(orgScmFromHgm);
}

/**
 * @tc.name: AddRefreshRateCountTest
 * @tc.desc: test HgmHardwareUtils.AddRefreshRateCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, AddRefreshRateCountTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    auto& hgmCore = hgmHardwareUtils->hgmCore_;
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    hgmHardwareUtils->refreshRateCounts_.clear();
    uint32_t currentRefreshRate = hgmCore.GetScreenCurrentRefreshRate(SCREEN_ID);
    hgmHardwareUtils->AddRefreshRateCount(SCREEN_ID);
    EXPECT_EQ(hgmHardwareUtils->refreshRateCounts_[currentRefreshRate], 1);
    hgmCore.hgmFrameRateMgr_ = nullptr;
    hgmHardwareUtils->AddRefreshRateCount(SCREEN_ID);
    EXPECT_EQ(hgmHardwareUtils->refreshRateCounts_[currentRefreshRate], 2);

    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
}

/*
 * @tc.name: RefreshRateCountsTest
 * @tc.desc: test HgmHardwareUtils.RefreshRateCounts
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, RefreshRateCountsTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils->refreshRateCounts_.clear();
    std::string dumpString = "";
    hgmHardwareUtils->RefreshRateCounts(dumpString);
    EXPECT_EQ(dumpString, "");
    hgmHardwareUtils->AddRefreshRateCount(SCREEN_ID);
    hgmHardwareUtils->RefreshRateCounts(dumpString);
    EXPECT_NE(dumpString, "");
}

/*
 * @tc.name: ClearRefreshRateCountsTest
 * @tc.desc: test HgmHardwareUtils.ClearRefreshRateCounts
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsTest, ClearRefreshRateCountsTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils->refreshRateCounts_.clear();
    std::string dumpString = "";
    hgmHardwareUtils->ClearRefreshRateCounts(dumpString);
    EXPECT_EQ(dumpString, "");
    hgmHardwareUtils->AddRefreshRateCount(SCREEN_ID);
    EXPECT_FALSE(hgmHardwareUtils->refreshRateCounts_.empty());
    hgmHardwareUtils->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(hgmHardwareUtils->refreshRateCounts_.empty());
    EXPECT_NE(dumpString, "");
}
} // namespace OHOS::Rosen