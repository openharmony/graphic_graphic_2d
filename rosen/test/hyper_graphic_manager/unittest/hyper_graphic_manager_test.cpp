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

#include <gtest/gtest.h>
#include <test_header.h>

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"

#include "screen_manager/screen_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    int32_t width = 720;
    int32_t height = 1080;
    int32_t phyWidth = 685;
    int32_t phyHeight = 1218;
    ScreenSize screenSize = {width, height, phyWidth, phyHeight};

    constexpr int32_t settingMode1 = 1;
    constexpr int32_t settingMode2 = 2;
    constexpr int32_t settingMode3 = 3;
}
class HyperGraphicManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HyperGraphicManagerTest::SetUpTestCase() {}
void HyperGraphicManagerTest::TearDownTestCase() {}
void HyperGraphicManagerTest::SetUp() {}
void HyperGraphicManagerTest::TearDown() {}

/**
 * @tc.name: Instance
 * @tc.desc: Verify the independency of HgmCore instance
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, Instance, Function | SmallTest | Level4)
{
    PART("CaseDescription") {
        STEP("1. call GetInstance twice") {
            auto &instance1 = HgmCore::Instance();
            auto &instance2 = HgmCore::Instance();
            STEP("2. check the result of configuration") {
                STEP_ASSERT_EQ(&instance1, &instance2);
            }
        }
    }
}

/**
 * @tc.name: IsInit
 * @tc.desc: Verify the result of initialization
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, IsInit, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();

    PART("CaseDescription") {
        STEP("1. check if IsInit() is true") {
            bool init = instance.IsInit();
            STEP_ASSERT_EQ(init, true);
            bool enabled = instance.IsEnabled();
            STEP_ASSERT_EQ(enabled, true);
            instance.GetPolicyConfigData();
            auto hgmFrameRateMgr_ = instance.GetFrameRateMgr();
            STEP_ASSERT_NE(hgmFrameRateMgr_, nullptr);
        }
    }
}

/**
 * @tc.name: AddScreen
 * @tc.desc: Verify the result of AddScreen function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, AddScreen, Function | MediumTest | Level2)
{
    auto &instance = HgmCore::Instance();
    int sizeListBefore = 0;
    int sizeListAfter = 0;
    int sizeScreenIds = 0;

    PART("EnvConditions") {
        STEP("get Instance") {
            bool init = instance.IsInit();
            STEP_ASSERT_EQ(init, true);
        }
    }

    PART("CaseDescription") {
        STEP("1. mark screenList_ size before add") {
            sizeListBefore = instance.GetScreenListSize();
            STEP_ASSERT_GE(sizeListBefore, 0);
        }

        STEP("2. add new screen") {
            ScreenId screenId = 2;
            auto addScreen = instance.AddScreen(screenId, 0, screenSize);
            STEP_ASSERT_EQ(addScreen, 0);
        }

        STEP("3. mark screenList_ size after add") {
            sizeListAfter = instance.GetScreenListSize();
        }

        STEP("4. check screenList_ size") {
            STEP_ASSERT_EQ(sizeListAfter, sizeListBefore + 1);
        }

        STEP("5. mark screenIds Size") {
            sizeScreenIds  = instance.GetScreenIds().size();
            STEP_ASSERT_EQ(sizeScreenIds, sizeListAfter);
        }
    }
}

/**
 * @tc.name: GetScreen
 * @tc.desc: Verify the result of GetScreen function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, GetScreen, Function | SmallTest | Level2)
{
    auto &instance5 = HgmCore::Instance();
    sptr<HgmScreen> screen = nullptr;
    ScreenId screenId = 3;
    ScreenId screenId2 = 4;
    

    PART("EnvConditions") {
        STEP("get Instance and call Init and add a screen") {
            auto addScreen = instance5.AddScreen(screenId, 0, screenSize);
            auto activeScreen = instance5.GetActiveScreen();

            instance5.SetActiveScreenId(screenId);
            activeScreen = instance5.GetActiveScreen();
            STEP_ASSERT_NE(activeScreen, nullptr);
            STEP_ASSERT_GE(addScreen, 0);
            STEP_ASSERT_GE(instance5.GetActiveScreenId(), screenId);
        }
    }

    PART("CaseDescription") {
        STEP("1. try get the previously added screen") {
            screen = instance5.GetScreen(screenId);
        }

        STEP("2. check the pointer") {
            STEP_ASSERT_NE(screen, nullptr);
        }

        STEP("3. try get the non-existence screen") {
            screen = instance5.GetScreen(screenId2);
            STEP_ASSERT_EQ(screen, nullptr);
        }
    }
}

/**
 * @tc.name: AddScreenModeInfo
 * @tc.desc: Verify the result of AddScreenModeInfo function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, AddScreenModeInfo, Function | SmallTest | Level3)
{
    auto &instance6 = HgmCore::Instance();
    int addMode = 0;
    ScreenId screenId = 4;

    PART("EnvConditions") {
        STEP("get Instance and add a screen") {
            auto addScreen = instance6.AddScreen(screenId, 0, screenSize);
            STEP_ASSERT_GE(addScreen, 0);
        }
    }

    PART("CaseDescription") {
        STEP("1. try get the previously added screen") {
            auto screen = instance6.GetScreen(screenId);
            STEP_ASSERT_NE(screen, nullptr);
        }

        STEP("2. add a supported config to the new screen") {
            addMode = instance6.AddScreen(screenId, addMode, screenSize);
        }

        STEP("3. verify adding result") {
            STEP_ASSERT_EQ(addMode, 0);
        }
    }
}

/**
 * @tc.name: RemoveScreen
 * @tc.desc: Verify the result of RemoveScreen function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, RemoveScreen, Function | MediumTest | Level2)
{
    auto &instance7 = HgmCore::Instance();
    int sizeListBefore = 0;
    int sizeListAfter = 0;
    ScreenId screenId = 6;

    PART("EnvConditions") {
        STEP("get Instance and call Init and add a screen") {
            bool init = instance7.IsInit();
            STEP_ASSERT_EQ(init, true);
            auto addScreen = instance7.AddScreen(screenId, 0, screenSize);
            STEP_ASSERT_EQ(addScreen, 0);
        }
    }

    PART("CaseDescription") {
        STEP("1. mark screenList_ size before remove") {
            sizeListBefore = instance7.GetScreenListSize();
            STEP_ASSERT_GE(sizeListBefore, 1);
        }

        STEP("2. add new screen") {
            ScreenId screenId = 2;
            auto removeScreen = instance7.RemoveScreen(screenId);
            STEP_ASSERT_EQ(removeScreen, 0);
        }

        STEP("3. mark screenList_ size after remove") {
            sizeListAfter = instance7.GetScreenListSize();
        }

        STEP("3. check screenList_ size") {
            STEP_ASSERT_EQ(sizeListAfter, sizeListBefore - 1);
        }
    }
}

/**
 * @tc.name: SetScreenRefreshRate
 * @tc.desc: Verify the result of SetScreenRefreshRate function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, SetScreenRefreshRate, Function | MediumTest | Level2)
{
    auto &instance8 = HgmCore::Instance();
    ScreenId screenId = 7;
    ScreenId screenId2 = 1;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    int32_t mode = 1;
    int32_t width0 = 1344;
    int32_t height0 = 2772;
    uint32_t rate0 = 60;
    int32_t mode0 = 0;
    int32_t timestamp = 1704038400; // 2024-01-01 00:00:00

    PART("CaseDescription") {
        STEP("1. add a new screen") {
            auto addScreen = instance8.AddScreen(screenId, 0, screenSize);
            STEP_ASSERT_EQ(addScreen, 0);
            auto addScreenProfile = instance8.AddScreenInfo(screenId, width, height, rate, mode);
            STEP_ASSERT_EQ(addScreenProfile, 0);
            auto addScreenProfile0 = instance8.AddScreenInfo(screenId, width0, height0, rate0, mode0);
            STEP_ASSERT_EQ(addScreenProfile0, 0);
            addScreenProfile0 = instance8.AddScreenInfo(screenId2, width0, height0, rate0, mode0);
            STEP_ASSERT_EQ(addScreenProfile0, 100);
            auto setRate500 = instance8.SetScreenRefreshRate(screenId, 0, 500);
            STEP_ASSERT_EQ(setRate500, -1);
            screen = instance8.GetScreen(screenId);
            STEP_ASSERT_NE(screen->GetActiveRefreshRate(), 500);
            auto setRate120 = instance8.SetScreenRefreshRate(screenId, 0, 120);
            STEP_ASSERT_NE(setRate120, -1);
            auto setRate60 = instance8.SetScreenRefreshRate(screenId2, 0, 60);
            STEP_ASSERT_EQ(setRate60, -1);
            auto setRateNegative = instance8.SetScreenRefreshRate(screenId, 0, -1);
            STEP_ASSERT_EQ(setRateNegative, -1);
            screen = instance8.GetScreen(screenId);
            STEP_ASSERT_EQ(screen->GetActiveRefreshRate(), 120);
            auto modeListToApply = instance8.GetModesToApply();
            STEP_ASSERT_NE(modeListToApply->size(), 0);
            instance8.SetPendingScreenRefreshRate(rate0);
            STEP_ASSERT_EQ(instance8.GetPendingScreenRefreshRate(), rate0);
            instance8.SetTimestamp(timestamp);
            STEP_ASSERT_EQ(instance8.GetCurrentTimestamp(), timestamp);
        }
    }
}

/**
 * @tc.name: SetRefreshRateMode
 * @tc.desc: Verify the result of SetRefreshRateMode function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, SetRefreshRateMode, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();
    ScreenId screenId = 7;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    int32_t mode = 1;
    int32_t modeToSet = 2;

    PART("EnvConditions") {
        STEP("get Instance") {
            bool init = instance.IsInit();
            STEP_ASSERT_EQ(init, true);
        }
    }

    PART("CaseDescription") {
        STEP("1. add a new screen") {
            auto addScreen = instance.AddScreen(screenId, 0, screenSize);
            STEP_ASSERT_GE(addScreen, 0);
        }

        STEP("2. add a a supported config to the new screen") {
            auto addScreenProfile = instance.AddScreenInfo(screenId, width, height, rate, mode);
            STEP_ASSERT_EQ(addScreenProfile, 0);
        }

        STEP("3. set the refreshrate mode") {
            auto setMode2 = instance.SetRefreshRateMode(modeToSet);
            STEP_ASSERT_EQ(setMode2, 0);
        }
    }
}

/**
 * @tc.name: HgmScreenTests
 * @tc.desc: Others functions in HgmScreen
 * @tc.type: FUNC
 * @tc.require: I7NJ2G
 */
HWTEST_F(HyperGraphicManagerTest, HgmScreenTests, Function | MediumTest | Level2)
{
    auto &instance = HgmCore::Instance();
    ScreenId screenId1 = 7;
    ScreenId screenId2 = 8;
    int32_t width = 1344;
    int32_t height = 2772;
    int32_t width2 = 640;
    int32_t height2 = 1320;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    uint32_t rate3 = 150;
    uint32_t rate4 = 0;
    uint32_t rate5 = 80;
    int32_t mode = 1;
    int32_t mode2 = 2;
    instance.AddScreen(screenId2, 1, screenSize);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);
    sptr<HgmScreen> screen = instance.GetScreen(screenId1);
    sptr<HgmScreen> screen2 = instance.GetScreen(screenId2);

    instance.AddScreen(screenId1, 0, screenSize);
    EXPECT_GE(screen->GetActiveRefreshRate(), 0);
    EXPECT_EQ(screen2->SetActiveRefreshRate(screenId2, rate2), 2);
    EXPECT_EQ(screen2->SetActiveRefreshRate(screenId2, rate3), -1);
    screen2->SetRateAndResolution(screenId2, rate2, width, height);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate, width, height), mode);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate3, width, height), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate4, width, height), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate5, width, height), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate5, width2, height2), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate5, width, height2), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate5, width2, height), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate, width2, height2), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate, width, height2), -1);
    EXPECT_EQ(screen2->SetRateAndResolution(screenId2, rate, width2, height), -1);
    screen2->AddScreenModeInfo(width, height, rate, mode);
    EXPECT_EQ(screen2->SetRefreshRateRange(rate2, rate), 0);
}

/**
 * @tc.name: HgmScreenTests2
 * @tc.desc: Others functions in HgmScreen
 * @tc.type: FUNC
 * @tc.require: I7NJ2G
 */
HWTEST_F(HyperGraphicManagerTest, HgmScreenTests2, Function | MediumTest | Level2) {
    PART("HgmScreen") {
        STEP("screen tests") {
            sptr<HgmScreen> screen1 = new HgmScreen();
            STEP_ASSERT_EQ(screen1->GetId(), 0);
            STEP_ASSERT_EQ(screen1->GetActiveMode(), 0);
            STEP_ASSERT_EQ(screen1->GetSupportedRates().size(), 0);
            STEP_ASSERT_EQ(screen1->IsSupportRate(OLED_30_HZ), false);
            STEP_ASSERT_EQ(screen1->GetActiveRefreshRateMode(), -1);
            STEP_ASSERT_EQ(screen1->GetWidth(), 0);
            STEP_ASSERT_EQ(screen1->GetHeight(), 0);
            STEP_ASSERT_EQ(screen1->GetPhyWidth(), 0);
            STEP_ASSERT_EQ(screen1->GetPhyHeight(), 0);
            STEP_ASSERT_EQ(screen1->GetPpi(), 0);
            STEP_ASSERT_EQ(screen1->GetXDpi(), 0);
            STEP_ASSERT_EQ(screen1->GetYDpi(), 0);

            delete screen1;
            screen1 = nullptr;
            STEP_ASSERT_EQ(screen1, nullptr);
        }
    }
}


/**
 * @tc.name: HgmCoreTests
 * @tc.desc: Others functions in HgmCore
 * @tc.type: FUNC
 * @tc.require: I7NJ2G
 */
HWTEST_F(HyperGraphicManagerTest, HgmCoreTests, Function | MediumTest | Level2)
{
    auto &instance = HgmCore::Instance();
    ScreenId screenId2 = 8;
    ScreenId screenId3 = 9;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    uint32_t rate3 = -1;
    int32_t mode = 1;
    int32_t mode2 = 2;
    instance.AddScreen(screenId2, 1, screenSize);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);

    PART("HgmCore") {
        STEP("1. set active mode") {
            int32_t setResult = instance.SetRefreshRateMode(settingMode1);
            setResult = instance.SetRefreshRateMode(settingMode3);
            setResult = instance.SetRefreshRateMode(settingMode1);
            STEP_ASSERT_EQ(setResult, 0);
        }

        STEP("2. set refresh rate via core") {
            int32_t setResult = instance.SetScreenRefreshRate(screenId3, 0, rate);
            setResult = instance.SetScreenRefreshRate(screenId2, 0, rate3);
            setResult = instance.SetScreenRefreshRate(screenId2, 0, rate2);
            setResult = instance.SetRefreshRateMode(settingMode1);
            STEP_ASSERT_GE(setResult, -1);
        }

        STEP("3. set rate and resolution") {
            int32_t setResult = instance.SetRateAndResolution(screenId2, 0, 0, 0, 0);
            STEP_ASSERT_EQ(setResult, -1);
            int32_t addResult = instance.AddScreen(screenId2, 1, screenSize);
            STEP_ASSERT_GE(addResult, -1);
        }

        STEP("4. add screen info, screen does not exist") {
            int32_t setResult = instance.AddScreenInfo(screenId3, 0, 0, 0, 0);
            STEP_ASSERT_NE(setResult, 0);
            uint32_t getResult = instance.GetScreenCurrentRefreshRate(screenId3);
            STEP_ASSERT_EQ(getResult, 0);
            std::vector<uint32_t> getVResult = instance.GetScreenSupportedRefreshRates(screenId3);
            STEP_ASSERT_EQ(getVResult.size(), 0);
        }

        STEP("5. add screen info, screen exist") {
            instance.GetScreenCurrentRefreshRate(screenId2);
            std::vector<uint32_t> getVResult = instance.GetScreenSupportedRefreshRates(screenId2);
            auto screenComponentRefreshRates = instance.GetScreenComponentRefreshRates(screenId2);
        }
    }
}

/**
 * @tc.name: SetRefreshRateMode002
 * @tc.desc: HgmCore.SetRefreshRateMode002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HyperGraphicManagerTest, SetRefreshRateMode002, Function | MediumTest | Level2)
{
    auto &instance = HgmCore::Instance();
    ScreenId screenId2 = 8;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    int32_t mode = 1;
    int32_t mode2 = 2;
    instance.AddScreen(screenId2, 1, screenSize);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);

    PART("HgmCore") {
        STEP("1. set active mode") {
            int32_t setResult = instance.SetRefreshRateMode(settingMode1);
            STEP_ASSERT_EQ(setResult, 0);
            auto refreshRateMode = instance.GetCurrentRefreshRateMode();
            STEP_ASSERT_EQ(refreshRateMode, settingMode1);

            setResult = instance.SetRefreshRateMode(settingMode2);
            STEP_ASSERT_EQ(setResult, 0);
            refreshRateMode = instance.GetCurrentRefreshRateMode();
            STEP_ASSERT_EQ(refreshRateMode, settingMode2);

            setResult = instance.SetRefreshRateMode(settingMode1);
            STEP_ASSERT_EQ(setResult, 0);
            refreshRateMode = instance.GetCurrentRefreshRateMode();
            STEP_ASSERT_EQ(refreshRateMode, settingMode1);
        }
    }
}

/**
 * @tc.name: RefreshBundleName
 * @tc.desc: Verify the result of RefreshBundleName
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HyperGraphicManagerTest, RefreshBundleName, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();

    PART("EnvConditions") {
        STEP("get Instance") {
            bool init = instance.IsInit();
            STEP_ASSERT_EQ(init, true);
        }
    }

    PART("CaseConditions") {
        STEP("1. test RefreshBundleName is true") {
            auto setMode = instance.RefreshBundleName("test hgm_core");
            STEP_ASSERT_EQ(setMode, 0);
            instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_AUTO);
            setMode = instance.RefreshBundleName("test hgm_core2");
            STEP_ASSERT_EQ(setMode, 0);
        }
    }
}

/**
 * @tc.name: GetIdealPeriod
 * @tc.desc: Test GetIdealPeriod
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HyperGraphicManagerTest, GetIdealPeriod, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();
    EXPECT_EQ(instance.GetIdealPeriod(30), 0);
    EXPECT_EQ(instance.GetIdealPeriod(60), 16666666);
}

/**
 * @tc.name: GetLtpoEnabled
 * @tc.desc: Test GetLtpoEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HyperGraphicManagerTest, GetLtpoEnabled, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();
    instance.SetLtpoEnabled(true);
    instance.SetSupportedMaxTE(VSYNC_MAX_REFRESHRATE);
    instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_AUTO);
    EXPECT_EQ(instance.IsLTPOSwitchOn(), true);
    EXPECT_EQ(instance.GetSupportedMaxTE(), VSYNC_MAX_REFRESHRATE);
    EXPECT_EQ(instance.GetCurrentRefreshRateMode(), static_cast<int32_t>(HGM_REFRESHRATE_MODE_AUTO));
    EXPECT_EQ(instance.GetLtpoEnabled(), true);
}

/**
 * @tc.name: NotifyScreenPowerStatus
 * @tc.desc: Test NotifyScreenPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HyperGraphicManagerTest, NotifyScreenPowerStatus, Function | SmallTest | Level2)
{
    ScreenId screenId = 8;
    auto &instance = HgmCore::Instance();
    instance.NotifyScreenPowerStatus(screenId, POWER_STATUS_ON);
}


/**
 * @tc.name: RefreshRateModeChangeCallback
 * @tc.desc: Test RefreshRateModeChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HyperGraphicManagerTest, RefreshRateModeChangeCallback, Function | SmallTest | Level2)
{
    auto &instance = HgmCore::Instance();
    instance.RegisterRefreshRateModeChangeCallback([](int32_t num) {return;});
    EXPECT_NE(instance.GetRefreshRateModeChangeCallback(), nullptr);
}

} // namespace Rosen
} // namespace OHOS