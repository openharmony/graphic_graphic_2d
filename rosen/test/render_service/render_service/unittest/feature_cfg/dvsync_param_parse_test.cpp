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

#include "dvsync_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DVSyncParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DVSyncParamParseTest::SetUpTestCase() {}
void DVSyncParamParseTest::TearDownTestCase() {}
void DVSyncParamParseTest::SetUp() {}
void DVSyncParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParam
 * @tc.desc: Verify the ParseFeatureParam function
 * @tc.type: FUNC
 * @tc.require: issueIBX8OW
 */
HWTEST_F(DVSyncParamParseTest, ParseFeatureParam, Function | SmallTest | Level1)
{
    FeatureParamMapType featureMap;
    DVSyncParamParse paramParse;
    xmlNode node;
    auto res = paramParse.ParseFeatureParam(featureMap, node);
    ASSERT_EQ(res, PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureMultiParam
 * @tc.desc: Verify the ParseFeatureMultiParam function
 * @tc.type: FUNC
 * @tc.require: issueIBX8OW
 */
HWTEST_F(DVSyncParamParseTest, ParseFeatureMultiParam, Function | SmallTest | Level1)
{
    DVSyncParamParse paramParse;
    xmlNode node;
    string name;
    auto res = paramParse.ParseFeatureMultiParam(node, name);
    ASSERT_EQ(res, PARSE_GET_CHILD_FAIL);
}
} // namespace Rosen
} // namespace OHOS