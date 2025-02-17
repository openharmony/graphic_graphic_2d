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
#include <test_header.h>

#include "xml_parser_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class XmlParserBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static constexpr char config[] = "/sys_prod/etc/graphic/graphic_config.xml";
    static constexpr char invalidConfig[] = "/sys_prod/etc/graphic/invalid_config.xml";
};

void XmlParserBaseTest::SetUpTestCase() {}
void XmlParserBaseTest::TearDownTestCase() {}
void XmlParserBaseTest::SetUp() {}
void XmlParserBaseTest::TearDown() {}

/**
 * @tc.name: LoadGraphicConfiguration
 * @tc.desc: Verify the result of LoadGraphicConfiguration function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(XmlParserBaseTest, LoadGraphicConfiguration, Function | SmallTest | Level1)
{
    std::unique_ptr<XMLParserBase> parser = std::make_unique<XMLParserBase>();

    PART("CaseDescription") {
        STEP("1. get an graphic xml parser") {
            STEP_ASSERT_NE(parser, nullptr);
        }
        STEP("2. check the result of configuration") {
            int32_t load = parser->LoadGraphicConfiguration(invalidConfig);
            STEP_ASSERT_EQ(load, static_cast<int32_t>(PARSE_FILE_LOAD_FAIL));
            STEP_ASSERT_EQ(parser->xmlDocument_, nullptr);
            STEP_ASSERT_EQ(parser->Parse(), static_cast<int32_t>(PARSE_FILE_LOAD_FAIL));
            load = parser->LoadGraphicConfiguration(config);
            STEP_ASSERT_GE(load, 0);
        }
    }
}

/**
 * @tc.name: Parse
 * @tc.desc: Verify the result of parsing functions
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(XmlParserBaseTest, Parse, Function | SmallTest | Level1)
{
    std::unique_ptr<XMLParserBase> parser = std::make_unique<XMLParserBase>();
    parser->LoadGraphicConfiguration(config);
    int parseSuccess = parser->Parse();
    EXPECT_GE(parseSuccess, PARSE_EXEC_SUCCESS);
}
} // namespace Rosen
} // namespace OHOS