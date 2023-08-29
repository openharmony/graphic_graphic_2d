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
#include "hgm_frame_rate_tool.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmFrameRateMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmFrameRateMgrTest::SetUpTestCase() {}
void HgmFrameRateMgrTest::TearDownTestCase() {}
void HgmFrameRateMgrTest::SetUp() {}
void HgmFrameRateMgrTest::TearDown() {}

/**
 * @tc.name: UniProcessData
 * @tc.desc: Verify the result of UniProcessData function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, UniProcessData, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    auto &instance = HgmCore::Instance();
    ScreenId screenId2 = 8;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    uint32_t rate3 = 90;
    int32_t mode = 1;
    int32_t mode2 = 2;
    int32_t mode3 = 3;
    instance.AddScreen(screenId2, 1);
    instance.AddScreenInfo(screenId2, width, height, rate, mode);
    instance.AddScreenInfo(screenId2, width, height, rate2, mode2);
    instance.AddScreenInfo(screenId2, width, height, rate3, mode3);
    PART("CaseDescription") {
        STEP("1. get a HgmFrameRateManager") {
            STEP_ASSERT_NE(mgr, nullptr);
        }
        STEP("2. check the result of UniProcessData") {
            FrameRateRangeData data;
            mgr->UniProcessData(data);
        }
        STEP("3. check the result of UniProcessData CalcRefreshRate with special value") {
            FrameRateRangeData data;
            data.screenId = screenId2;

            data.rsRange.Set(150, 150, 150);
            mgr->UniProcessData(data);

            data.rsRange.Set(0, 120, 90);
            mgr->UniProcessData(data);

            data.rsRange.Set(0, 60, 60);
            mgr->UniProcessData(data);

            data.rsRange.Set(0, 80, 80);
            mgr->UniProcessData(data);

            data.rsRange.Set(0, 70, 80);
            mgr->UniProcessData(data);

            data.rsRange.Set(0, 50, 50);
            mgr->UniProcessData(data);
        }
    }
}

/**
 * @tc.name: HgmFrameRateTool.RemoveScreenProfile
 * @tc.desc: Verify the result of v function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, RemoveScreenProfileTest, Function | SmallTest | Level1)
{
    auto hgmFrameRateTool = HgmFrameRateTool::GetInstance();
    ScreenId screenId = 2;
    int32_t width = 720;
    int32_t height = 1080;
    int32_t phyWidth = 685;
    int32_t phyHeight = 1218;
    ASSERT_EQ(0, hgmFrameRateTool->RemoveScreenProfile(screenId));
    ASSERT_EQ(0, hgmFrameRateTool->AddScreenProfile(screenId, width, height, phyWidth, phyHeight));
    ASSERT_EQ(0, hgmFrameRateTool->RemoveScreenProfile(screenId));
}

/**
 * @tc.name: HgmFrameRateTool.CalModifierPreferred
 * @tc.desc: Verify the result of v function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, CalModifierPreferredTest, Function | SmallTest | Level1)
{
    auto hgmFrameRateTool = HgmFrameRateTool::GetInstance();
    ScreenId screenId = 2;
    int32_t width = 720;
    int32_t height = 1080;
    int32_t phyWidth = 685;
    int32_t phyHeight = 1218;

    HgmModifierProfile hgmModifierProfile;
    hgmModifierProfile.xSpeed = 0.0;
    hgmModifierProfile.ySpeed = 0.0;
    hgmModifierProfile.hgmModifierType = HgmModifierType::TRANSLATE;

    auto parsedConfigData = std::make_shared<ParsedConfigData>();

    PART("CaseDescription") {
        STEP("1. check CalModifierPreferred when ret == -1") {
            ASSERT_EQ(-1, hgmFrameRateTool->CalModifierPreferred(screenId, hgmModifierProfile, parsedConfigData));
            ASSERT_EQ(0, hgmFrameRateTool->AddScreenProfile(screenId, width, height, phyWidth, phyHeight));
            ASSERT_EQ(-1, hgmFrameRateTool->CalModifierPreferred(screenId, hgmModifierProfile, parsedConfigData));
        }

        STEP("2. chech CalModifierPreferred when ret != -1") {
            ParsedConfigData::AnimationDynamicSetting animationDynamicSetting;
            animationDynamicSetting.min = 60;
            animationDynamicSetting.max = -1;
            animationDynamicSetting.preferred_fps = 60;
            parsedConfigData->dynamicSetting_["translate"]["animationDynamicSetting"] = animationDynamicSetting;
            ASSERT_EQ(-1, hgmFrameRateTool->CalModifierPreferred(screenId, hgmModifierProfile, parsedConfigData));
            ParsedConfigData::AnimationDynamicSetting animationDynamicSetting2;
            animationDynamicSetting2.min = 0;
            animationDynamicSetting2.max = -1;
            animationDynamicSetting2.preferred_fps = 60;
            parsedConfigData->dynamicSetting_["translate"]["animationDynamicSetting2"] = animationDynamicSetting2;
            ASSERT_EQ(60, hgmFrameRateTool->CalModifierPreferred(screenId, hgmModifierProfile, parsedConfigData));
        }
    }
}
} // namespace Rosen
} // namespace OHOS