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

#include "hgm_command.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmCommandTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}

    std::shared_ptr<PolicyConfigVisitorImpl> visitor_;
};

void HgmCommandTest::SetUp()
{
    auto& hgmCore = HgmCore::Instance();
    visitor_ = std::make_shared<PolicyConfigVisitorImpl>(*hgmCore.mPolicyConfigData_);
}

/**
 * @tc.name: Init001
 * @tc.desc: Verify the result of Init001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, Init001, Function | SmallTest | Level1)
{
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    // bk
    auto configData = hgmCore.mPolicyConfigData_;
    ASSERT_NE(configData, nullptr);
    auto configVisitor = hgmCore.mPolicyConfigVisitor_;
    ASSERT_NE(configVisitor, nullptr);
    auto configVisitorImpl = static_cast<PolicyConfigVisitorImpl*>(configVisitor.get());
    ASSERT_NE(configVisitorImpl, nullptr);

    // data not null; visitor not null
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);
    EXPECT_NE(&(configVisitorImpl->GetXmlData()), configData.get());
    EXPECT_EQ(configVisitorImpl->xmlModeId_, std::to_string(hgmCore.customFrameRateMode_));
    EXPECT_EQ(configVisitorImpl->screenConfigType_, frameRateMgr->curScreenStrategyId_);

    // data null; visitor null
    hgmCore.mPolicyConfigData_ = nullptr;
    hgmCore.mPolicyConfigVisitor_ = nullptr;
    frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
    frameRateMgr->curScreenStrategyId_ = "10"; // donot existed id
    frameRateMgr->HandleScreenExtStrategyChange(false, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);
    hgmCore.SetRefreshRateMode(-2);
    hgmCore.InitXmlConfig();

    // data not null; visitor null
    hgmCore.mPolicyConfigData_ = configData;
    hgmCore.mPolicyConfigVisitor_ = nullptr;
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);

    // data null; visitor not null
    hgmCore.mPolicyConfigVisitor_ = configVisitor;
    hgmCore.mPolicyConfigData_ = nullptr;
    hgmCore.SetRefreshRateMode(-2);
    frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
    frameRateMgr->curScreenStrategyId_ = "11"; // donot existed id
    frameRateMgr->HandleScreenExtStrategyChange(false, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);

    // recovery
    hgmCore.mPolicyConfigData_ = configData;
    hgmCore.mPolicyConfigVisitor_ = configVisitor;
}

/**
 * @tc.name: SimpleGet
 * @tc.desc: Verify the result of SimpleGet function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SimpleGet, Function | SmallTest | Level1)
{
    auto dynamicSettingMap = visitor_->GetAceSceneDynamicSettingMap();
    EXPECT_NE(dynamicSettingMap.find("aaa"), dynamicSettingMap.end());

    EXPECT_EQ(visitor_->GetGameNodeName("bbb"), "12");
    EXPECT_EQ(visitor_->GetGameNodeName("aaa"), "");

    visitor_->ChangeScreen("LTPS-DEFAULT");
    EXPECT_EQ(visitor_->screenConfigType_, "LTPS-DEFAULT");
}

/**
 * @tc.name: GetRefreshRateModeName
 * @tc.desc: Verify the result of GetRefreshRateModeName function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, GetRefreshRateModeName, Function | SmallTest | Level1)
{
    std::vector<std::pair<int32_t, int32_t>> testCase = {
        // <refreshRateModeId, fps>
        { -1, -1 },
        { 1, OLED_60_HZ },
        { 2, OLED_120_HZ },
        { 3, 0 },
    };
    for (const auto& [refreshRateModeId, fps] : testCase) {
        EXPECT_EQ(visitor_->GetRefreshRateModeName(refreshRateModeId), fps);
    }
}

/**
 * @tc.name: XmlModeId2SettingModeId
 * @tc.desc: Verify the result of XmlModeId2SettingModeId function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, XmlModeId2SettingModeId, Function | SmallTest | Level1)
{
    const std::vector<std::pair<std::string, int32_t>> partId = {
        // <xmlModeId, settingModeId>
        {"-1", 0},
        {"0", 0},
        {"1", 1},
        {"2", 2},
        {"3", 0},
        {"0", 0},
    };
    for (const auto& [xmlModeId, settingModeId] : partId) {
        EXPECT_EQ(visitor_->XmlModeId2SettingModeId(xmlModeId), settingModeId);
    }
}

/**
 * @tc.name: SetSettingModeId
 * @tc.desc: Verify the result of SetSettingModeId function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SetSettingModeId, Function | SmallTest | Level1)
{
    visitor_->SetSettingModeId(0);
    EXPECT_EQ(visitor_->settingModeId_, 0);
    visitor_->SetSettingModeId(0);
    EXPECT_EQ(visitor_->settingModeId_, 0);
    visitor_->SetSettingModeId(1);
    EXPECT_EQ(visitor_->settingModeId_, 1);

    visitor_->SetXmlModeId("0");
    EXPECT_EQ(visitor_->xmlModeId_, "0");
    visitor_->SetXmlModeId("0");
    EXPECT_EQ(visitor_->xmlModeId_, "0");
    visitor_->SetXmlModeId("1");
    EXPECT_EQ(visitor_->xmlModeId_, "1");
}

/**
 * @tc.name: GetScreenSetting
 * @tc.desc: Verify the result of GetScreenSetting function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, GetScreenSetting, Function | SmallTest | Level1)
{
    visitor_->screenConfigType_ = "unknown";
    EXPECT_TRUE(visitor_->GetScreenSetting().appList.empty());
    
    visitor_->screenConfigType_ = "LTPO-DEFAULT";
    visitor_->xmlModeId_ = "5";
    EXPECT_TRUE(visitor_->GetScreenSetting().appList.empty());

    visitor_->xmlModeId_ = "-1";
    EXPECT_FALSE(visitor_->GetScreenSetting().appList.empty());
}

/**
 * @tc.name: GetAppStrategyConfig
 * @tc.desc: Verify the result of GetAppStrategyConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, GetAppStrategyConfig, Function | SmallTest | Level1)
{
    int32_t appType = 111;
    int32_t appType1 = 222;
    int32_t appType2 = 333;

    PolicyConfigData::StrategyConfig settingStrategy;
    EXPECT_EQ(visitor_->GetAppStrategyConfig("aaa", appType, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType1, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType2, settingStrategy), HGM_ERROR);
}
} // namespace Rosen
} // namespace OHOS