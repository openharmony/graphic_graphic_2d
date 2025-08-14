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

#include "hgm_test_base.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmXmlParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void LoadXmlContent1();
    static constexpr char config[] = "/sys_prod/etc/graphic/hgm_policy_config.xml";
    static constexpr char invalidConfig[] = "/sys_prod/etc/graphic/invalid_config.xml";
};

void HgmXmlParserTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}
void HgmXmlParserTest::TearDownTestCase() {}
void HgmXmlParserTest::SetUp() {}
void HgmXmlParserTest::TearDown() {}

void HgmXmlParserTest::LoadXmlContent1()
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
}

/**
 * @tc.name: LoadConfiguration
 * @tc.desc: Verify the result of LoadConfiguration function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmXmlParserTest, LoadConfiguration, Function | SmallTest | Level0)
{
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();

    PART("CaseDescription") {
        STEP("1. get an xml parser") {
            STEP_ASSERT_NE(parser, nullptr);
        }
        STEP("2. check the result of configuration") {
            int32_t load = parser->LoadConfiguration(invalidConfig);
            STEP_ASSERT_EQ(load, static_cast<int32_t>(HgmErrCode::XML_FILE_LOAD_FAIL));
            STEP_ASSERT_EQ(parser->xmlDocument_, nullptr);
            STEP_ASSERT_EQ(parser->Parse(), static_cast<int32_t>(HgmErrCode::HGM_ERROR));
            STEP_ASSERT_EQ(parser->mParsedData_, nullptr);
            load = parser->LoadConfiguration(config);
            STEP_ASSERT_GE(load, 0);
            if (parser->mParsedData_ == nullptr) {
                return;
            }
            STEP_ASSERT_NE(parser->mParsedData_, nullptr);
            load = parser->LoadConfiguration(config);
            STEP_ASSERT_GE(load, 0);
        }
    }
}

/**
 * @tc.name: Parse
 * @tc.desc: Verify the result of parsing functions
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmXmlParserTest, Parse, Function | SmallTest | Level0)
{
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
    int32_t load = parser->LoadConfiguration(config);
    EXPECT_GE(load, EXEC_SUCCESS);
    auto parserData = std::move(parser->mParsedData_);
    EXPECT_EQ(parser->mParsedData_, nullptr);
    EXPECT_EQ(parser->Parse(), XML_PARSE_INTERNAL_FAIL);
    parser->mParsedData_ = std::move(parserData);
    EXPECT_NE(parser->mParsedData_, nullptr);
    EXPECT_EQ(parser->Parse(), EXEC_SUCCESS);
    parser->GetParsedData();
}

/**
 * @tc.name: IsNumber
 * @tc.desc: Verify the result of IsNumber function
 * @tc.type: FUNC
 * @tc.require: IBCFDD
 */
HWTEST_F(HgmXmlParserTest, IsNumber, Function | SmallTest | Level0)
{
    std::vector<std::pair<std::string, bool>> cases = {
        { "", false },
        { "123456789", false },
        { "a023", false },
        { "02a3", false },
        { "023a", false },
        { "123", true },
        { "-123", true },
        { "023", true },
        { "12345678", true }
    };
    for (const auto& [str, res] : cases) {
        EXPECT_EQ(XMLParser::IsNumber(str), res);
    }
}

/**
 * @tc.name: StringToVector001
 * @tc.desc: Verify the result of StringToVector001 functions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmXmlParserTest, StringToVector001, Function | SmallTest | Level0)
{
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
    std::string emptyInput = "";
    std::vector<uint32_t> result = parser->StringToVector(emptyInput);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: StringToVector002
 * @tc.desc: Verify the result of StringToVector002 functions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmXmlParserTest, StringToVector002, Function | SmallTest | Level0)
{
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
    std::string spacesBetweenNumbersInput = "1 2   3  45 ";
    std::vector<uint32_t> expected = { 1, 2, 3, 45 };
    std::vector<uint32_t> result = parser->StringToVector(spacesBetweenNumbersInput);
    EXPECT_EQ(expected, result);
}

/**
 * @tc.name: StringToVector003
 * @tc.desc: Verify the result of StringToVector003 functions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmXmlParserTest, StringToVector003, Function | SmallTest | Level0)
{
    std::unique_ptr<XMLParser> parser = std::make_unique<XMLParser>();
    std::string invalidInput = "abc";
    std::vector<uint32_t> result = parser->StringToVector(invalidInput);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: ParseParams001
 * @tc.desc: Verify the result of ParseParams function
 * @tc.type: FUNC
 * @tc.require: IBCFDD
 */
HWTEST_F(HgmXmlParserTest, ParseParams001, Function | SmallTest | Level1)
{
    LoadXmlContent1();
    auto& hgmCore = HgmCore::Instance();
    const auto& settingMap = hgmCore.mPolicyConfigData_->refreshRateForSettingsMap_;
    // test config for pcmode parse OK
    auto it = settingMap.find("pcmode");
    EXPECT_TRUE(it != settingMap.end());
    auto pcmodeSettings = it->second;
    EXPECT_GT(pcmodeSettings.size(), 0);
    // test member refreshRateForSettings_ has been aligned
    const auto& settings = hgmCore.mPolicyConfigData_->refreshRateForSettings_;
    ASSERT_TRUE(settings.size() > 0);
    int32_t name = settings.at(2).first;
    EXPECT_EQ(name, 120);
    int32_t id = settings.at(2).second;
    EXPECT_EQ(id, 2);
}

/**
 * @tc.name: ParseRefreshRate4Settings001
 * @tc.desc: Verify the result of ParseRefreshRate4Settings function
 * @tc.type: FUNC
 * @tc.require: IBCFDD
 */
HWTEST_F(HgmXmlParserTest, ParseRefreshRate4Settings001, Function | SmallTest | Level1)
{
    // test xml file TEST_XML_CONTENT_1;
    LoadXmlContent1();
    auto& hgmCore = HgmCore::Instance();
    const auto& settingMap = hgmCore.mPolicyConfigData_->refreshRateForSettingsMap_;
    // test config parse OK
    auto it = settingMap.find("pcmode");
    EXPECT_TRUE(it != settingMap.end());
    // test member refreshRateForSettings_ has been aligned
    const auto& settings = hgmCore.mPolicyConfigData_->refreshRateForSettings_;
    EXPECT_GT(settings.size(), 0);
}
} // namespace Rosen
} // namespace OHOS