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
#include "opinc_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class OpincParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void OpincParamParseTest::SetUpTestCase() {}
void OpincParamParseTest::TearDownTestCase() {}
void OpincParamParseTest::SetUp() {}
void OpincParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(OpincParamParseTest, ParseFeatureParamTest, TestSize.Level1)
{
    OPIncParamParse paramParse;
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
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("OPIncEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(OPIncParam::IsOPIncEnable(), false);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("ImageAliasEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(OPIncParam::IsImageAliasEnable(), false);

    name = "FeatureSingleParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("CacheWidthThresholdPercentValue"));
    xmlSetProp(&nextNode, (const xmlChar*)("type"), (const xmlChar*)("int"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("100"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(OPIncParam::GetCacheWidthThresholdPercentValue(), 100);
}

/**
 * @tc.name: ParseOPIncInternalTest
 * @tc.desc: Test ParseOPIncInternal
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(OpincParamParseTest, ParseOPIncInternalTest, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    auto res = opincParamParse.ParseOPIncInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = opincParamParse.ParseOPIncInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "FeatureSingleParam";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = opincParamParse.ParseOPIncInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
}