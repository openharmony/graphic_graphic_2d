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
#include "uifirst_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class UIFirstParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UIFirstParamParseTest::SetUpTestCase() {}
void UIFirstParamParseTest::TearDownTestCase() {}
void UIFirstParamParseTest::SetUp() {}
void UIFirstParamParseTest::TearDown() {}

/**
 * @tc.name: GetUIFirstSwitchTypeTest
 * @tc.desc: Test GetUIFirstSwitchType
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(UIFirstParamParseTest, GetUIFirstSwitchTypeTest, TestSize.Level1)
{
    UIFirstParamParse uifirstParamParse;
    auto res = uifirstParamParse.GetUIFirstSwitchType("unknow");
    ASSERT_EQ(res, UIFirstSwitchType::UNKNOWN);
}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(UIFirstParamParseTest, ParseFeatureParamTest, TestSize.Level1)
{
    UIFirstParamParse uifirstParamParse;
    FeatureParamMapType paramMapType;
    xmlNode node;
    auto res = uifirstParamParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseUIFirstInternalTest
 * @tc.desc: Test ParseUIFirstInternal
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(UIFirstParamParseTest, ParseUIFirstInternalTest, TestSize.Level1)
{
    UIFirstParamParse uifirstParamParse;
    xmlNode node;
    auto res = uifirstParamParse.ParseUIFirstInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = uifirstParamParse.ParseUIFirstInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseUIFirstSingleParamTest
 * @tc.desc: Test ParseUIFirstSingleParam
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(UIFirstParamParseTest, ParseUIFirstSingleParamTest, TestSize.Level1)
{
    UIFirstParamParse uifirstParamParse;
    string name = "UIFirstEnabledWindowThreshold";
    string value = "test";
    auto res = uifirstParamParse.ParseUIFirstSingleParam(name, value);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "UIFirstType";
    res = uifirstParamParse.ParseUIFirstSingleParam(name, value);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
}