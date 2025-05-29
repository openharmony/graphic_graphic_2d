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

#include "prevalidate_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class PrevalidateParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PrevalidateParamParseTest::SetUpTestCase() {}
void PrevalidateParamParseTest::TearDownTestCase() {}
void PrevalidateParamParseTest::SetUp() {}
void PrevalidateParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest001
 * @tc.desc: Verify the ParseFeatureParam function
 * @tc.type: FUNC
 * @tc.require: issueIBZ2P9
 */
HWTEST_F(PrevalidateParamParseTest, ParseFeatureParamTest001, Function | SmallTest | Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;
    PrevalidateParamParse parse;
    auto ret = parse.ParseFeatureParam(featureMap, node);;
    ASSERT_EQ(ret, PARSE_GET_CHILD_FAIL);
}
} // namespace Rosen
} // namespace OHOS