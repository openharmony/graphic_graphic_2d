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

#include "screen_manager/screen_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
            auto addScreen = instance.AddScreen(screenId, 0);
            STEP_ASSERT_EQ(addScreen, 0);
        }

        STEP("3. mark screenList_ size after add") {
            sizeListAfter = instance.GetScreenListSize();
        }

        STEP("3. check screenList_ size") {
            STEP_ASSERT_EQ(sizeListAfter, sizeListBefore + 1);
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

    PART("EnvConditions") {
        STEP("get Instance and call Init and add a screen") {
            auto addScreen = instance5.AddScreen(screenId, 0);
            STEP_ASSERT_GE(addScreen, 0);
        }
    }

    PART("CaseDescription") {
        STEP("1. try get the previously added screen") {
            screen = instance5.GetScreen(screenId);
        }

        STEP("2. check the pointer") {
            STEP_ASSERT_NE(screen, nullptr);
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
            auto addScreen = instance6.AddScreen(screenId, 0);
            STEP_ASSERT_GE(addScreen, 0);
        }
    }

    PART("CaseDescription") {
        STEP("1. try get the previously added screen") {
            auto screen = instance6.GetScreen(screenId);
            STEP_ASSERT_NE(screen, nullptr);
        }

        STEP("2. add a supported config to the new screen") {
            addMode = instance6.AddScreen(screenId, addMode);
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
            auto addScreen = instance7.AddScreen(screenId, 0);
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
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    int32_t mode = 1;
    int32_t width0 = 1344;
    int32_t height0 = 2772;
    uint32_t rate0 = 60;
    int32_t mode0 = 0;

    PART("EnvConditions") {
        STEP("get Instance") {
            bool init = instance8.IsInit();
            STEP_ASSERT_EQ(init, true);
        }
    }

    PART("CaseDescription") {
        STEP("1. add a new screen") {
            auto addScreen = instance8.AddScreen(screenId, 0);
            STEP_ASSERT_EQ(addScreen, 0);
        }

        STEP("2. add a config mode - rate : 1, 120 to the new screen") {
            auto addScreenProfile = instance8.AddScreenInfo(screenId, width, height, rate, mode);
            STEP_ASSERT_EQ(addScreenProfile, 0);
            auto addScreenProfile0 = instance8.AddScreenInfo(screenId, width0, height0, rate0, mode0);
            STEP_ASSERT_EQ(addScreenProfile0, 0);
        }

        STEP("3. set the rate of default screen to 500") {
            auto setRate500 = instance8.SetScreenRefreshRate(screenId, 0, 500);
            STEP_ASSERT_EQ(setRate500, -1);
        }

        STEP("4. chech that the result is fail") {
            screen = instance8.GetScreen(screenId);
            STEP_ASSERT_NE(screen->GetActiveRefreshRate(), 500);
        }

        STEP("5. set the rate of default screen to 120") {
            auto setRate120 = instance8.SetScreenRefreshRate(screenId, 0, 120);
            STEP_ASSERT_NE(setRate120, -1);
        }

        STEP("6. check that the result is success") {
            screen = instance8.GetScreen(screenId);
            STEP_ASSERT_EQ(screen->GetActiveRefreshRate(), 120);
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
    RefreshRateMode modeToSet = HGM_REFRESHRATE_MODE_MEDIUM;

    PART("EnvConditions") {
        STEP("get Instance") {
            bool init = instance.IsInit();
            STEP_ASSERT_EQ(init, true);
        }
    }

    PART("CaseDescription") {
        STEP("1. add a new screen") {
            auto addScreen = instance.AddScreen(screenId, 0);
            STEP_ASSERT_GE(addScreen, 0);
        }

        STEP("2. add a a supported config to the new screen") {
            auto addScreenProfile = instance.AddScreenInfo(screenId, width, height, rate, mode);
            STEP_ASSERT_EQ(addScreenProfile, 0);
        }

        STEP("3. set the refreshrate mode") {
            auto setMode2 = instance.SetRefreshRateMode(modeToSet);
            STEP_ASSERT_EQ(setMode2, -1);
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
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    int32_t mode = 1;
    int32_t mode2 = 2;
    instance.AddScreen(screenId2, 1);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);
    sptr<HgmScreen> screen2 = instance.GetScreen(screenId2);

    PART("Prepare") {
        STEP("screen tests") {
            sptr<HgmScreen> screen1 = new HgmScreen();
            delete screen1;
            screen1 = nullptr;
            STEP_ASSERT_EQ(screen1, nullptr);
        }
    }

    PART("HgmScreen") {
        STEP("1. add a new screen") {
            instance.AddScreen(screenId1, 0);
            screen = instance.GetScreen(screenId1);
            uint32_t modeGot = screen->GetActiveRefreshRate();
            STEP_ASSERT_NE(modeGot, rate);
        }

        STEP("2. set rate and resolution") {
            int32_t setResult = screen2->SetActiveRefreshRate(screenId2, rate2);
            STEP_ASSERT_EQ(setResult, 2);
        }

        STEP("3. set the refreshrate mode") {
            int32_t setResult = screen2->SetRateAndResolution(screenId2, rate2, width, height);
            STEP_ASSERT_NE(setResult, mode2);
            setResult = screen2->SetRateAndResolution(screenId2, rate, width, height);
            STEP_ASSERT_EQ(setResult, mode);
        }

        STEP("4. mode already exists") {
            int32_t addResult = screen2->AddScreenModeInfo(width, height, rate, mode);
            STEP_ASSERT_NE(addResult, 0);
        }

        STEP("5. setrange") {
            int32_t setResult = screen2->SetRefreshRateRange(rate2, rate);
            STEP_ASSERT_EQ(setResult, 0);
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
    instance.AddScreen(screenId2, 1);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);

    PART("HgmCore") {
        STEP("1. set active mode") {
            int32_t setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_HIGH);
            setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_LOW);
            setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_HIGH);
            setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_HIGH);
            STEP_ASSERT_NE(setResult, 0);
        }

        STEP("2. set refresh rate via core") {
            int32_t setResult = instance.SetScreenRefreshRate(screenId3, 0, rate);
            setResult = instance.SetScreenRefreshRate(screenId2, 0, rate3);
            setResult = instance.SetScreenRefreshRate(screenId2, 0, rate2);
            setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_HIGH);
            setResult = instance.SetRefreshRateMode(HGM_REFRESHRATE_MODE_HIGH);
            STEP_ASSERT_GE(setResult, -1);
        }

        STEP("3. set rate and resolution") {
            int32_t setResult = instance.SetRateAndResolution(screenId2, 0, 0, 0, 0);
            STEP_ASSERT_EQ(setResult, -1);
            int32_t addResult = instance.AddScreen(screenId2, 1);
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
    }
}
} // namespace Rosen
} // namespace OHOS