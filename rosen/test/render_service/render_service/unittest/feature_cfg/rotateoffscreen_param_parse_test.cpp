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
#include "rotateoffscreen_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RotateOffScreenParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RotateOffScreenParamParseTest::SetUpTestCase() {}
void RotateOffScreenParamParseTest::TearDownTestCase() {}
void RotateOffScreenParamParseTest::SetUp() {}
void RotateOffScreenParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#IBZZ8D
 */
HWTEST_F(RotateOffScreenParamParseTest, ParseFeatureParamTest, TestSize.Level1)
{
    RotateOffScreenParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSingleParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("RotateOffScreenDisplayNodeEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("RotateOffScreenSurfaceNodeEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("RotateOffScreenDowngradeEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
}