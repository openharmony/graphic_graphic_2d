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

#include "render_server/rs_render_mode_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr GroupId TEST_GROUP_ID_0 = 0;
constexpr GroupId DEFAULT_GROUP_ID = 0;
constexpr int32_t PARSE_EXEC_SUCCESS = 0;
constexpr int32_t PARSE_ERROR = -1;
constexpr int32_t TEST_SCREEN_ID_100 = 100;
constexpr int32_t TEST_SCREEN_ID_101 = 101;
constexpr GroupId TEST_GROUP_ID_1 = 1;
constexpr GroupId TEST_GROUP_ID_3 = 3;
constexpr GroupId TEST_GROUP_ID_5 = 5;
constexpr size_t EXPECTED_TWO_GROUPS = 2;

xmlDocPtr StringToXmlDoc(const std::string& xmlContent)
{
    return xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
}
} // namespace

class RSRenderModeConfigParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::unique_ptr<RSRenderModeConfigParser> parser_;
};

std::unique_ptr<RSRenderModeConfigParser> RSRenderModeConfigParserTest::parser_ = nullptr;

void RSRenderModeConfigParserTest::SetUpTestCase()
{
    parser_ = std::make_unique<RSRenderModeConfigParser>();
}

void RSRenderModeConfigParserTest::TearDownTestCase()
{
    parser_.reset();
}

void RSRenderModeConfigParserTest::SetUp() {}
void RSRenderModeConfigParserTest::TearDown() {}

/**
 * @tc.name: CreateParserTest001
 * @tc.desc: Test creating RSRenderModeConfigParser instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, CreateParserTest001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    ASSERT_NE(parser, nullptr);
}

/**
 * @tc.name: BuildRenderConfigTest001
 * @tc.desc: Test BuildRenderConfig method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, BuildRenderConfigTest001, TestSize.Level1)
{
    ASSERT_NE(parser_, nullptr);

    auto config = parser_->BuildRenderConfig();
    // Note: If config file is not found, it should return a valid config with default values
    ASSERT_NE(config, nullptr);
}

/**
 * @tc.name: BuildRenderConfigTest002
 * @tc.desc: Test BuildRenderConfig returns valid config structure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, BuildRenderConfigTest002, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    ASSERT_NE(parser, nullptr);

    auto config = parser->BuildRenderConfig();
    ASSERT_NE(config, nullptr);

    // Verify default group ID is set
    auto defaultGroup = config->GetDefaultRenderProcess();
    ASSERT_GE(defaultGroup, DEFAULT_GROUP_ID);
}

/**
 * @tc.name: MultipleParserInstancesTest001
 * @tc.desc: Test creating multiple parser instances
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, MultipleParserInstancesTest001, TestSize.Level1)
{
    auto parser1 = std::make_unique<RSRenderModeConfigParser>();
    auto parser2 = std::make_unique<RSRenderModeConfigParser>();

    ASSERT_NE(parser1, nullptr);
    ASSERT_NE(parser2, nullptr);

    auto config1 = parser1->BuildRenderConfig();
    auto config2 = parser2->BuildRenderConfig();

    ASSERT_NE(config1, nullptr);
    ASSERT_NE(config2, nullptr);
}

/**
 * @tc.name: ParserDestructionTest001
 * @tc.desc: Test parser destructor properly cleans up resources
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParserDestructionTest001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    ASSERT_NE(parser, nullptr);

    auto config = parser->BuildRenderConfig();
    ASSERT_NE(config, nullptr);

    // Parser destruction should be handled properly
    parser.reset();
    ASSERT_EQ(parser, nullptr);
}

/**
 * @tc.name: ConfigDefaultValueTest001
 * @tc.desc: Test config has valid default values when file is missing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ConfigDefaultValueTest001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    auto config = parser->BuildRenderConfig();

    ASSERT_NE(config, nullptr);

    // When config file is not found, should use defaults
    auto defaultGroup = config->GetDefaultRenderProcess();
    ASSERT_GE(defaultGroup, TEST_GROUP_ID_0);
}

/**
 * @tc.name: BuildRenderConfigMultipleTimesTest001
 * @tc.desc: Test calling BuildRenderConfig multiple times
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, BuildRenderConfigMultipleTimesTest001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();

    auto config1 = parser->BuildRenderConfig();
    auto config2 = parser->BuildRenderConfig();

    ASSERT_NE(config1, nullptr);
    ASSERT_NE(config2, nullptr);
}

/**
 * @tc.name: ParseInternal_ValueIsZero001
 * @tc.desc: Test ParseInternal with value="0" disables multi-process mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ValueIsZero001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="0"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    EXPECT_FALSE(parser->isMultiProcessModeEnabled_);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ValueIsOne001
 * @tc.desc: Test ParseInternal with value="1" enables multi-process mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ValueIsOne001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="1"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    EXPECT_TRUE(parser->isMultiProcessModeEnabled_);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ValueIsOtherNumber001
 * @tc.desc: Test ParseInternal with value="5" degenerates to disabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ValueIsOtherNumber001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="5"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    EXPECT_FALSE(parser->isMultiProcessModeEnabled_);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ValueIsNegative001
 * @tc.desc: Test ParseInternal with value="-1" degenerates to disabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ValueIsNegative001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="-1"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    EXPECT_FALSE(parser->isMultiProcessModeEnabled_);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ValueIsNotNumber001
 * @tc.desc: Test ParseInternal with non-numeric value returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ValueIsNotNumber001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="abc"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_FALSE(result);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_EmptyValue001
 * @tc.desc: Test ParseInternal with no value attribute returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_EmptyValue001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_FALSE(result);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_WithDefaultGroup001
 * @tc.desc: Test ParseInternal with default_group attribute sets correct default
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_WithDefaultGroup001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1" default_group="3">)"
        R"(<rp id="3" name="g3"><screen id="100" name="s0"/></rp></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->GetDefaultRenderProcess(), TEST_GROUP_ID_3);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_FirstGroupBecomesDefault001
 * @tc.desc: Test first child id becomes default when default_group is absent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_FirstGroupBecomesDefault001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1">)"
        R"(<render_process id="5" name="g5"><screen id="100" name="s0"/></render_process></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->GetDefaultRenderProcess(), TEST_GROUP_ID_5);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ChildWithNoIdUsesDefault001
 * @tc.desc: Test child without id attribute uses defaultGroupId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ChildWithNoIdUsesDefault001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1" default_group="3">)"
        R"(<render_process name="gnoid"><screen id="100" name="s0"/></render_process></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    const auto& groupConfigs = config->GetGroupInfoConfigs();
    EXPECT_NE(groupConfigs.find(TEST_GROUP_ID_3), groupConfigs.end());
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_RootNameIsRenderProcess001
 * @tc.desc: Test children are skipped when root name is render_process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_RootNameIsRenderProcess001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_process value="1">)"
        R"(<rp id="5" name="g5"><screen id="100" name="s0"/></rp></render_process>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->GetDefaultRenderProcess(), DEFAULT_RENDER_PROCESS);
    EXPECT_TRUE(config->GetGroupInfoConfigs().empty());
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_ParseGroupFails001
 * @tc.desc: Test ParseInternal returns false when ParseGroup fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_ParseGroupFails001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1">)"
        R"(<render_process id="1" name="g1"><screen id="abc" name="s0"/></render_process></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_FALSE(result);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInternal_MultipleGroups001
 * @tc.desc: Test ParseInternal with multiple group children
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_MultipleGroups001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1" default_group="1">)"
        R"(<render_process id="1" name="g1"><screen id="100" name="s0"/></render_process>)"
        R"(<render_process id="2" name="g2"><screen id="101" name="s1"/></render_process></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->GetDefaultRenderProcess(), TEST_GROUP_ID_1);
    const auto& groupConfigs = config->GetGroupInfoConfigs();
    EXPECT_NE(groupConfigs.size(), EXPECTED_TWO_GROUPS);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseGroup_ValidScreenInfo001
 * @tc.desc: Test ParseGroup correctly parses screen name and id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseGroup_ValidScreenInfo001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><rp id="1" name="g1">)"
        R"(<screen id="100" name="s0"/><screen id="101" name="s1"/></rp>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    int32_t result = parser->ParseGroup(*root, TEST_GROUP_ID_1);
    EXPECT_EQ(result, PARSE_EXEC_SUCCESS);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    const auto& screenMap = config->GetScreenIdToGroupId();
    EXPECT_NE(screenMap.find(TEST_SCREEN_ID_100), screenMap.end());
    EXPECT_NE(screenMap.find(TEST_SCREEN_ID_101), screenMap.end());
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseGroup_InvalidScreenId001
 * @tc.desc: Test ParseGroup returns error when screen id is not a number
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseGroup_InvalidScreenId001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><rp id="1" name="g1">)"
        R"(<screen id="notanumber" name="s0"/></rp>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    int32_t result = parser->ParseGroup(*root, TEST_GROUP_ID_1);
    EXPECT_EQ(result, PARSE_ERROR);
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseGroup_NoChildNodes001
 * @tc.desc: Test ParseGroup with no screen children succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ParseGroup_NoChildNodes001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><rp id="1" name="g1"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    int32_t result = parser->ParseGroup(*root, TEST_GROUP_ID_1);
    EXPECT_EQ(result, PARSE_EXEC_SUCCESS);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    const auto& groupConfigs = config->GetGroupInfoConfigs();
    EXPECT_NE(groupConfigs.find(TEST_GROUP_ID_1), groupConfigs.end());
    EXPECT_TRUE(groupConfigs.at(TEST_GROUP_ID_1).screenInfos.empty());
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ExtractPropertyValue_PropertyExists001
 * @tc.desc: Test ExtractPropertyValue returns value when property exists
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ExtractPropertyValue_PropertyExists001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><node name="testName"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    std::string result = parser->ExtractPropertyValue("name", *root);
    EXPECT_EQ(result, "testName");
    xmlFreeDoc(doc);
}

/**
 * @tc.name: ExtractPropertyValue_PropertyNotExists001
 * @tc.desc: Test ExtractPropertyValue returns empty when property does not exists
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderModeConfigParserTest, ExtractPropertyValue_PropertyNotExists001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><node name="testName"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    std::string result = parser->ExtractPropertyValue("nonexistent", *root);
    EXPECT_TRUE(result.empty());
    xmlFreeDoc(doc);
}

// Newly Added Phase2
HWTEST_F(RSRenderModeConfigParserTest, Parse_RootNodeIsNull001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="0"></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    auto config = parser->BuildRenderConfig();
    ASSERT_NE(config, nullptr);
    auto defaultGroup = config->GetDefaultRenderProcess();
    ASSERT_GE(defaultGroup, DEFAULT_GROUP_ID);
    xmlFreeDoc(doc);
}

HWTEST_F(RSRenderModeConfigParserTest, ParseInternal_NonElementNodes001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent =
        R"(<?xml version="1.0"?><render_mode value="1">)"
        R"(<!-- this is a comment -->)"
        R"(    <text>whitespace text</text>)"
        R"(<render_process id="5" name="g5"><screen id="100" name="s0"/></render_process></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->GetDefaultRenderProcess(), TEST_GROUP_ID_5);
    const auto& groupConfigs = config->GetGroupInfoConfigs();
    EXPECT_EQ(groupConfigs.size(), 1u);
    EXPECT_NE(groupConfigs.find(TEST_GROUP_ID_5), groupConfigs.end());
    xmlFreeDoc(doc);
}

HWTEST_F(RSRenderModeConfigParserTest, Parse_MultipleBranches001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="1"><rp id="1" name="g1"/></render_mode>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    bool result = parser->ParseInternal(*root);
    EXPECT_TRUE(result);
    EXPECT_TRUE(parser->isMultiProcessModeEnabled_);
    auto config = parser->builder.Build();
    ASSERT_NE(config, nullptr);
    xmlFreeDoc(doc);
}

HWTEST_F(RSRenderModeConfigParserTest, Parse_ParseInternalFails001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParser>();
    const std::string xmlContent = R"(<?xml version="1.0"?><render_mode value="invalid"/>)";
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNode* root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    bool result = parser->ParseInternal(*root);
    EXPECT_FALSE(result);
    xmlFreeDoc(doc);
}

class RSRenderModeConfigParserMock : public RSRenderModeConfigParser {
public:
    int32_t LoadConfigurationsMock()
    {
        std::string configPath = "/invalid/path/that/does/not/exist.xml";
        xmlDoc* doc = xmlReadFile(configPath.c_str(), nullptr, XML_PARSE_NONET);
        if (!doc) {
            return RSRenderModeConfigParser::RENDER_MODE_PARSE_FILE_LOAD_FAIL;
        }
        xmlDocument_ = doc;
        return RSRenderModeConfigParser::RENDER_MODE_PARSE_EXEC_SUCCESS;
    }
};

HWTEST_F(RSRenderModeConfigParserTest, LoadConfigurations_XmlReadFileFails001, TestSize.Level1)
{
    auto parser = std::make_unique<RSRenderModeConfigParserMock>();
    int32_t result = parser->LoadConfigurationsMock();
    EXPECT_EQ(result, RSRenderModeConfigParser::RENDER_MODE_PARSE_FILE_LOAD_FAIL);
    auto config = parser->BuildRenderConfig();
    ASSERT_NE(config, nullptr);
}

} // namespace OHOS::Rosen
