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
    void LoadXmlContent1();

    std::shared_ptr<PolicyConfigVisitorImpl> visitor_;
};

void HgmCommandTest::SetUp()
{
    auto& hgmCore = HgmCore::Instance();
    visitor_ = std::make_shared<PolicyConfigVisitorImpl>(*hgmCore.mPolicyConfigData_);
}

void HgmCommandTest::LoadXmlContent1()
{
    // test xml file TEST_XML_CONTENT_1;
    auto& hgmCore = HgmCore::Instance();
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
    parser->mParsedData_ = std::make_unique<PolicyConfigData>();
    ASSERT_NE(parser->mParsedData_, nullptr);
    parser->xmlDocument_ = StringToXmlDoc(TEST_XML_CONTENT_1);
    ASSERT_NE(parser->xmlDocument_, nullptr);
    parser->Parse();
    hgmCore.mPolicyConfigData_ = parser->GetParsedData();
    hgmCore.mParser_ = std::move(parser);
    visitor_ = std::make_shared<PolicyConfigVisitorImpl>(*hgmCore.mPolicyConfigData_);
}

/**
 * @tc.name: Init001
 * @tc.desc: Verify the result of Init001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, Init001, Function | SmallTest | Level0)
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
    frameRateMgr->HandleAppStrategyConfigEvent(1, "", {}); // pid=1
    EXPECT_EQ(&(configVisitorImpl->GetXmlData()), configData.get());
    EXPECT_EQ(configVisitorImpl->xmlModeId_, std::to_string(hgmCore.customFrameRateMode_));
    EXPECT_EQ(configVisitorImpl->screenConfigType_, frameRateMgr->curScreenStrategyId_);

    // data null; visitor null
    hgmCore.mPolicyConfigData_ = nullptr;
    hgmCore.mPolicyConfigVisitor_ = nullptr;
    frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
    frameRateMgr->curScreenStrategyId_ = "10"; // id donot existed
    frameRateMgr->HandleScreenExtStrategyChange(false, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);
    hgmCore.SetRefreshRateMode(-2); // mode donot existed
    hgmCore.InitXmlConfig();

    // data not null; visitor null
    hgmCore.mPolicyConfigData_ = configData;
    hgmCore.mPolicyConfigVisitor_ = nullptr;
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);

    // data null; visitor not null
    hgmCore.mPolicyConfigVisitor_ = configVisitor;
    hgmCore.mPolicyConfigData_ = nullptr;
    hgmCore.SetRefreshRateMode(-2); // mode donot existed
    frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
    frameRateMgr->curScreenStrategyId_ = "11"; // donot existed id
    frameRateMgr->HandleScreenExtStrategyChange(false, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
    frameRateMgr->HandleScreenFrameRate("");
    frameRateMgr->Init(nullptr, nullptr, nullptr, nullptr);

    // test xmlCompatibleMode_ equal false
    hgmCore.mPolicyConfigData_ = configData;
    hgmCore.mPolicyConfigVisitor_ = configVisitor;
    configData->xmlCompatibleMode_ = false;
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
HWTEST_F(HgmCommandTest, SimpleGet, Function | SmallTest | Level0)
{
    auto dynamicSettingMap = visitor_->GetAceSceneDynamicSettingMap();
    EXPECT_NE(dynamicSettingMap.find("aaa"), dynamicSettingMap.end());

    // pkgName, nodeName
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
HWTEST_F(HgmCommandTest, GetRefreshRateModeName, Function | SmallTest | Level0)
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
HWTEST_F(HgmCommandTest, XmlModeId2SettingModeId, Function | SmallTest | Level0)
{
    std::vector<std::pair<std::string, std::optional<int32_t>>> testCase = {
        // <xmlModeId, settingModeId>
        { "-1", -1 },
        { "0", std::optional<int32_t>() },
        { "1", 1 },
        { "2", 2 },
        { "3", std::optional<int32_t>() },
        { "0", std::optional<int32_t>() },
    };
    for (const auto& [xmlModeId, settingModeId] : testCase) {
        EXPECT_EQ(visitor_->XmlModeId2SettingModeId(xmlModeId), settingModeId);
    }

    // for no auto mode test
    using namespace std;
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    LoadXmlContent1();
    EXPECT_NE(hgmCore.mPolicyConfigData_, nullptr);
    vector<pair<int32_t, int32_t>> noAutoConfig = {
        {60, 101},
        {120, 102}
    };
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("testmode_no_auto");
    EXPECT_EQ(visitor_->configData_.refreshRateForSettings_.size(), noAutoConfig.size());
    for (size_t i = 0; i < noAutoConfig.size(); ++i) {
        EXPECT_EQ(noAutoConfig[i], visitor_->configData_.refreshRateForSettings_[i]);
    }
    testCase = {
        { "-1", std::optional<int32_t>() },
        { "0", std::optional<int32_t>() },
        { "101", 1 },
        { "102", 2 },
        { "3", std::optional<int32_t>() },
        { "0", std::optional<int32_t>() },
    };
    for (const auto& [xmlModeId, settingModeId] : testCase) {
        EXPECT_EQ(visitor_->XmlModeId2SettingModeId(xmlModeId), settingModeId);
    }
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("");
}

/**
 * @tc.name: SetSettingModeId
 * @tc.desc: Verify the result of SetSettingModeId function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SetSettingModeId, Function | SmallTest | Level0)
{
    visitor_->SetSettingModeId(0);
    EXPECT_EQ(visitor_->settingModeId_, 0);
    visitor_->SetSettingModeId(0);
    EXPECT_EQ(visitor_->settingModeId_, 0);
    visitor_->SetSettingModeId(1);
    EXPECT_EQ(visitor_->settingModeId_, 1);

    visitor_->SetXmlModeId("0");
    EXPECT_EQ(visitor_->xmlModeId_, "1");
    visitor_->SetXmlModeId("0");
    EXPECT_EQ(visitor_->xmlModeId_, "1");
    visitor_->SetXmlModeId("1");
    EXPECT_EQ(visitor_->xmlModeId_, "1");
    visitor_->SetXmlModeId("-1");
    EXPECT_EQ(visitor_->xmlModeId_, "-1");
    visitor_->SetXmlModeId("-1");
    EXPECT_EQ(visitor_->xmlModeId_, "-1");
}

/**
 * @tc.name: GetScreenSetting
 * @tc.desc: Verify the result of GetScreenSetting function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, GetScreenSetting, Function | SmallTest | Level0)
{
    visitor_->screenConfigType_ = "unknown";
    EXPECT_TRUE(visitor_->GetScreenSetting().appList.empty());
    
    visitor_->screenConfigType_ = "LTPO-DEFAULT";
    visitor_->xmlModeId_ = "5"; // id dont existed
    EXPECT_TRUE(visitor_->GetScreenSetting().appList.empty());

    visitor_->xmlModeId_ = "-1"; // auto mode
    EXPECT_FALSE(visitor_->GetScreenSetting().appList.empty());
}

/**
 * @tc.name: GetAppStrategyConfig
 * @tc.desc: Verify the result of GetAppStrategyConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, GetAppStrategyConfig, Function | SmallTest | Level0)
{
    int32_t appType = 111;
    int32_t appType1 = 222;
    int32_t appType2 = 333;

    PolicyConfigData::StrategyConfig settingStrategy;
    // aaa/bbb means pkgName
    EXPECT_EQ(visitor_->GetAppStrategyConfig("aaa", appType, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType1, settingStrategy), EXEC_SUCCESS);
    EXPECT_EQ(visitor_->GetAppStrategyConfig("bbb", appType2, settingStrategy), HGM_ERROR);
}

/**
 * @tc.name: SetRefreshRateMode
 * @tc.desc: Verify the result of SetRefreshRateMode function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SetRefreshRateMode, Function | SmallTest | Level0)
{
    const std::vector<std::tuple<int32_t, int32_t, std::string>> testCase = {
        // -1:dynamic 1:standard 2:high
        // <input, settingModeId, xmlModeId>
        { -1, -1, "-1" },
        { 0, 0, "-1" },
        { 1, 1, "1" },
        { 2, 2, "2" },
        { -1, -1, "-1" },
        { 2, 2, "2" },
        { 0, 0, "-1" },
        { 3, 0, "-1" },
    };

    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    auto configVisitorImpl = static_cast<PolicyConfigVisitorImpl*>(hgmCore.mPolicyConfigVisitor_.get());
    ASSERT_NE(configVisitorImpl, nullptr);

    for (const auto& [input, settingModeId, xmlModeId] : testCase) {
        hgmCore.SetRefreshRateMode(input);
        EXPECT_EQ(configVisitorImpl->settingModeId_, settingModeId);
        EXPECT_EQ(configVisitorImpl->xmlModeId_, xmlModeId);
    }
}


/**
 * @tc.name: UpdateRefreshRateForSettings
 * @tc.desc: Verify the result of UpdateRefreshRateForSettings function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, UpdateRefreshRateForSettings001, Function | SmallTest | Level0)
{
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    LoadXmlContent1();
    // padmode
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("padmode");
    const auto& settings = hgmCore.mPolicyConfigData_->refreshRateForSettings_;
    std::vector<std::pair<int32_t, int32_t>> padSettings = {
        {-1, -1}, {60, 1}, {120, 2}, {144, 3}
    };
    ASSERT_EQ(settings.size(), padSettings.size());
    for (size_t i = 0; i < settings.size(); ++i) {
        EXPECT_EQ(settings[i], padSettings[i]);
    }
    // pcmode
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("pcmode");
    std::vector<std::pair<int32_t, int32_t>> pcSettings = {
        {-1, -101}, {60, 101}, {120, 102}
    };
    ASSERT_EQ(settings.size(), pcSettings.size());
    for (size_t i = 0; i < settings.size(); ++i) {
        EXPECT_EQ(settings[i], pcSettings[i]);
    }
    // test unknowmode
    auto originSettings = settings;
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("unknownMode");
    ASSERT_EQ(settings.size(), originSettings.size());
    for (size_t i = 0; i < settings.size(); ++i) {
        EXPECT_EQ(settings[i], originSettings[i]);
    }
}

/**
 * @tc.name: AddParamWatcher
 * @tc.desc: Verify the result of AddParamWatcher function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, AddParamWatcher001, Function | SmallTest | Level0)
{
    auto& hgmCore = HgmCore::Instance();
    auto ret = hgmCore.AddParamWatcher();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: SysModeChangeProcess
 * @tc.desc: Verify the result of SysModeChangeProcess function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SysModeChangeProcess001, Function | SmallTest | Level0)
{
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    ASSERT_NE(hgmCore.mPolicyConfigData_, nullptr);
    auto policyConfigData = reinterpret_cast<PolicyConfigData*>(hgmCore.mPolicyConfigData_.get());
    EXPECT_NE(policyConfigData, nullptr);
    HgmCore::SysModeChangeProcess("persist.sys.mode", "testmode", policyConfigData);

    HgmCore::SysModeChangeProcess("persist.sys.mode", "testmode", nullptr);
}

/**
 * @tc.name: SettingModeId2XmlModeId
 * @tc.desc: Verify the result of SettingModeId2XmlModeId function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmCommandTest, SettingModeId2XmlModeId, Function | SmallTest | Level0)
{
    using namespace std;
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    LoadXmlContent1();
    // test when auto mode exists
    EXPECT_NE(hgmCore.mPolicyConfigData_, nullptr);
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("pcmode");
    vector<pair<int, optional<string>>> testConfig = {
        {-1, "-101"},
        {-10, optional<string>()},
        {10, optional<string>()},
        {2, "102"}
    };
    for (const auto& p : testConfig) {
        auto ret = visitor_->SettingModeId2XmlModeId(p.first);
        EXPECT_EQ(ret, p.second);
    }
    // no auto mode
    hgmCore.mPolicyConfigData_->UpdateRefreshRateForSettings("testmode_no_auto");
    testConfig = {
        {-10, optional<string>()},
        {-1, optional<string>()},
        {10, optional<string>()},
        {1, "101"},
        {2, "102"}
    };
    for (const auto& p : testConfig) {
        auto ret = visitor_->SettingModeId2XmlModeId(p.first);
        EXPECT_EQ(ret, p.second);
    }
}
} // namespace Rosen
} // namespace OHOS