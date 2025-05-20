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

#include "hwc_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class HwcParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HwcParamParseTest::SetUpTestCase() {}
void HwcParamParseTest::TearDownTestCase() {}
void HwcParamParseTest::SetUp() {}
void HwcParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureMultiParamForApp001
 * @tc.desc: Verify the ParseFeatureMultiParamForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamParseTest, ParseFeatureMultiParamForApp001, Function | SmallTest | Level1)
{
    HWCParamParse hwcParamParse;
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    std::string name;
    int32_t result = hwcParamParse.ParseFeatureMultiParamForApp(node, name);
    EXPECT_EQ(result, PARSE_GET_CHILD_FAIL);
}
}
}