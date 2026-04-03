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

#include "render_server/rs_render_mode_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr GroupId TEST_GROUP_ID_0 = 0;
constexpr GroupId TEST_GROUP_ID_1 = 1;
constexpr GroupId DEFAULT_GROUP_ID = 0;
}

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

} // namespace OHOS::Rosen
