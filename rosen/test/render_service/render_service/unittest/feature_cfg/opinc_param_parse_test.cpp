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
#include <unordered_set>
#include "common/rs_common_hook.h"
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

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("LayerPartRenderEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(OPIncParam::IsLayerPartRenderEnable(), true);

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

    name = "Feature";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = opincParamParse.ParseOPIncInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "FeatureMultiParam";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = opincParamParse.ParseOPIncInternal(node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    node.xmlChildrenNode = &childNode;
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("children"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("0"));
    res = opincParamParse.ParseOPIncInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureMultiParamTest001
 * @tc.desc: Test ParseFeatureMultiParam with no children nodes
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(OpincParamParseTest, ParseFeatureMultiParamTest001, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    auto res = opincParamParse.ParseFeatureMultiParam(node);
    EXPECT_EQ(res, PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureMultiParamTest002
 * @tc.desc: Test ParseFeatureMultiParam with empty bundleName
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(OpincParamParseTest, ParseFeatureMultiParamTest002, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    node.xmlChildrenNode = &childNode;
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)(""));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("1"));

    auto res = opincParamParse.ParseFeatureMultiParam(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    EXPECT_TRUE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList(""));

    RsCommonHook::Instance().SetLayerPartRenderWhiteList({});
}

/**
 * @tc.name: ParseFeatureMultiParamTest003
 * @tc.desc: Test ParseFeatureMultiParam with bundleName exceeding limit (100 chars)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(OpincParamParseTest, ParseFeatureMultiParamTest003, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    node.xmlChildrenNode = &childNode;
    std::string longName(101, 'a');
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)(longName.c_str()));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("1"));

    auto res = opincParamParse.ParseFeatureMultiParam(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    EXPECT_TRUE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList(longName));

    RsCommonHook::Instance().SetLayerPartRenderWhiteList({});
}

/**
 * @tc.name: ParseFeatureMultiParamTest004
 * @tc.desc: Test ParseFeatureMultiParam with multiple whitelist items
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(OpincParamParseTest, ParseFeatureMultiParamTest004, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;

    xmlNode childNode1;
    childNode1.type = xmlElementType::XML_ELEMENT_NODE;
    node.xmlChildrenNode = &childNode1;
    xmlSetProp(&childNode1, (const xmlChar*)("name"), (const xmlChar*)("com.example.app1"));
    xmlSetProp(&childNode1, (const xmlChar*)("value"), (const xmlChar*)("1"));

    xmlNode childNode2;
    childNode2.type = xmlElementType::XML_ELEMENT_NODE;
    childNode1.next = &childNode2;
    xmlSetProp(&childNode2, (const xmlChar*)("name"), (const xmlChar*)("com.example.app2"));
    xmlSetProp(&childNode2, (const xmlChar*)("value"), (const xmlChar*)("1"));

    xmlNode childNode3;
    childNode3.type = xmlElementType::XML_ELEMENT_NODE;
    childNode2.next = &childNode3;
    xmlSetProp(&childNode3, (const xmlChar*)("name"), (const xmlChar*)("com.example.app3"));
    xmlSetProp(&childNode3, (const xmlChar*)("value"), (const xmlChar*)("0"));

    auto res = opincParamParse.ParseFeatureMultiParam(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    EXPECT_TRUE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList("com.example.app1"));
    EXPECT_TRUE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList("com.example.app2"));
    EXPECT_FALSE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList("com.example.app3"));

    RsCommonHook::Instance().SetLayerPartRenderWhiteList({});
}

/**
 * @tc.name: ParseFeatureMultiParamTest005
 * @tc.desc: Test ParseFeatureMultiParam with no-element child node type
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(OpincParamParseTest, ParseFeatureMultiParamTest005, TestSize.Level1)
{
    OPIncParamParse opincParamParse;
    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;

    auto res = opincParamParse.ParseFeatureMultiParam(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    EXPECT_TRUE(RsCommonHook::Instance().IsInLayerPartRenderWhiteList(""));

    RsCommonHook::Instance().SetLayerPartRenderWhiteList({});
}
}