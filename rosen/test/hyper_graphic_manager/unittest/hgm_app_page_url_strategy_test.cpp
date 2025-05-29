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

#include <gtest/gtest.h>
#include <limits>
#include <test_header.h>

#include "hgm_app_page_url_strategy.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int32_t DEF_PID = 1000;
    constexpr int32_t MIN_FPS = 60;
    constexpr int32_t MAX_FPS = 90;
    const std::string STRATEGY = "5";
    const std::string PKG_NAME = "com.pkg.other";
    const std::string PAGE_NAME = "other";
    const std::string PAGE_NAME1 = "other1";
}

class HgmAppPageUrlStrategyTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}
    HgmErrCode GetPageUrlVoteInfo(uint32_t pid);

    std::shared_ptr<HgmAppPageUrlStrategy> appPageUrlStrategy_;
    PolicyConfigData::ScreenSetting screenSetting_;
};

void HgmAppPageUrlStrategyTest::SetUp()
{
    appPageUrlStrategy_ = std::make_shared<HgmAppPageUrlStrategy>();
    PolicyConfigData::PageUrlConfig pageUrlConfig;
    pageUrlConfig[PAGE_NAME] = STRATEGY;
    screenSetting_.pageUrlConfig[PKG_NAME] = pageUrlConfig;
}

HgmErrCode HgmAppPageUrlStrategyTest::GetPageUrlVoteInfo(uint32_t pid)
{
    for (auto& pageUrlVoter : appPageUrlStrategy_->pageUrlVoterInfo_) {
        auto voterPid = pageUrlVoter.first;
        auto voterInfo = pageUrlVoter.second;
        if (voterPid == pid && voterInfo.hasVoter) {
            return EXEC_SUCCESS;
        }
    }
    return HGM_ERROR;
}

/**
 * @tc.name: NotifyPageName001
 * @tc.desc: Verify the result of NotifyPageName001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmAppPageUrlStrategyTest, NotifyPageName001, Function | SmallTest | Level1)
{
    PolicyConfigData::ScreenSetting screenSetting;
    appPageUrlStrategy_->SetPageUrlConfig(screenSetting.pageUrlConfig);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, true);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, false);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
}

/**
 * @tc.name: NotifyPageName002
 * @tc.desc: Verify the result of NotifyPageName002 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmAppPageUrlStrategyTest, NotifyPageName002, Function | SmallTest | Level1)
{
    appPageUrlStrategy_->SetPageUrlConfig(screenSetting_.pageUrlConfig);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, true);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), EXEC_SUCCESS);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, false);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
}

/**
 * @tc.name: NotifyScreenSettingChange001
 * @tc.desc: Verify the result of NotifyScreenSettingChange001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmAppPageUrlStrategyTest, NotifyScreenSettingChange001, Function | SmallTest | Level1)
{
    PolicyConfigData::ScreenSetting screenSetting;

    appPageUrlStrategy_->SetPageUrlConfig(screenSetting.pageUrlConfig);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, true);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);

    appPageUrlStrategy_->SetPageUrlConfig(screenSetting_.pageUrlConfig);
    appPageUrlStrategy_->NotifyScreenSettingChange();
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), EXEC_SUCCESS);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, false);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
}

/**
 * @tc.name: NotifyScreenSettingChange002
 * @tc.desc: Verify the result of NotifyScreenSettingChange002 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmAppPageUrlStrategyTest, NotifyScreenSettingChange002, Function | SmallTest | Level1)
{
    pid_t pid = 1;
    PolicyConfigData::ScreenSetting screenSetting;

    appPageUrlStrategy_->SetPageUrlConfig(screenSetting_.pageUrlConfig);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, true);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), EXEC_SUCCESS);

    appPageUrlStrategy_->SetPageUrlConfig(screenSetting.pageUrlConfig);
    appPageUrlStrategy_->NotifyScreenSettingChange();
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, false);
    ASSERT_EQ(GetPageUrlVoteInfo(DEF_PID), HGM_ERROR);
    appPageUrlStrategy_->CleanPageUrlVote(pid);
    HgmAppPageUrlStrategy::VoterInfo voterInfo = {false, PKG_NAME, PAGE_NAME};
    appPageUrlStrategy_->pageUrlVoterInfo_.try_emplace(0, voterInfo);
    std::unordered_map<std::string, std::string> pageUrlConfig;
    pageUrlConfig.try_emplace(PAGE_NAME1, PAGE_NAME1);
    appPageUrlStrategy_->pageUrlConfig_.try_emplace(PKG_NAME, pageUrlConfig);
    ASSERT_LE(appPageUrlStrategy_->pageUrlConfig_[PKG_NAME].count(PAGE_NAME), 0);
    appPageUrlStrategy_->NotifyScreenSettingChange();
    appPageUrlStrategy_->NotifyPageName(DEF_PID, PKG_NAME, PAGE_NAME, false);
    ASSERT_LE(appPageUrlStrategy_->pageUrlConfig_[PKG_NAME].count(PAGE_NAME), 0);
}
} // namespace Rosen
} // namespace OHOS