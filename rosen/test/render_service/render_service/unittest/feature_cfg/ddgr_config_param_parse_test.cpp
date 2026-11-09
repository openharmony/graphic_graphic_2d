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

#include <gtest/gtest.h>
#include "ddgr_config_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class DDGRConfigParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DDGRConfigParamParseTest::SetUpTestCase() {}
void DDGRConfigParamParseTest::TearDownTestCase() {}
void DDGRConfigParamParseTest::SetUp() {}
void DDGRConfigParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamNoChild
 * @tc.desc: Test ParseFeatureParam without children nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamNoChild, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureParamNonElementChild
 * @tc.desc: Test ParseFeatureParam with a non element child node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamNonElementChild, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParamNoName
 * @tc.desc: Test ParseFeatureParam with an element node which has no name property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamNoName, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("1"));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParamNoValue
 * @tc.desc: Test ParseFeatureParam with an element node which has no value property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamNoValue, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_no_value"));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(DDGRConfigParam::GetParams().count("ddgr_no_value"), 0);
}

/**
 * @tc.name: ParseFeatureParamEmptyName
 * @tc.desc: Test ParseFeatureParam with an empty name property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamEmptyName, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)(""));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("ddgr_empty_name"));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParamEmptyValue
 * @tc.desc: Test ParseFeatureParam with an empty value property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamEmptyValue, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_empty_value"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)(""));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    ASSERT_EQ(DDGRConfigParam::GetParams().count("ddgr_empty_value"), 0);
}

/**
 * @tc.name: ParseFeatureParamSuccess
 * @tc.desc: Test ParseFeatureParam with a valid param
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamSuccess, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_valid_param"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("ddgr_valid_value"));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_valid_param"), 1);
    ASSERT_EQ(params["ddgr_valid_param"], "ddgr_valid_value");
}

/**
 * @tc.name: ParseFeatureParamUnknownNodeName
 * @tc.desc: Test ParseFeatureParam with an unrecognized xml node name
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamUnknownNodeName, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode childNode {};
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string nodeName = "UnknownNode";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_unknown_node"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("1"));
    node.xmlChildrenNode = &childNode;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_unknown_node"), 1);
    ASSERT_EQ(params["ddgr_unknown_node"], "1");
}

/**
 * @tc.name: ParseFeatureParamMultiChildren
 * @tc.desc: Test ParseFeatureParam with multiple valid children nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamMultiChildren, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode firstNode {};
    xmlNode secondNode {};
    std::string firstNodeName = "FeatureSwitch";
    std::string secondNodeName = "FeatureMultiParam";
    firstNode.type = xmlElementType::XML_ELEMENT_NODE;
    firstNode.name = reinterpret_cast<const xmlChar*>(firstNodeName.c_str());
    xmlSetProp(&firstNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_multi_first"));
    xmlSetProp(&firstNode, (const xmlChar*)("value"), (const xmlChar*)("first"));
    secondNode.type = xmlElementType::XML_ELEMENT_NODE;
    secondNode.name = reinterpret_cast<const xmlChar*>(secondNodeName.c_str());
    xmlSetProp(&secondNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_multi_second"));
    xmlSetProp(&secondNode, (const xmlChar*)("value"), (const xmlChar*)("second"));
    node.xmlChildrenNode = &firstNode;
    firstNode.next = &secondNode;

    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_multi_first"), 1);
    ASSERT_EQ(params["ddgr_multi_first"], "first");
    ASSERT_EQ(params.count("ddgr_multi_second"), 1);
    ASSERT_EQ(params["ddgr_multi_second"], "second");
}

/**
 * @tc.name: ParseFeatureParamStopOnFirstFailure
 * @tc.desc: Test ParseFeatureParam stops parsing once a child node is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamParseTest, ParseFeatureParamStopOnFirstFailure, TestSize.Level1)
{
    DDGRConfigParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node {};
    xmlNode invalidNode {};
    xmlNode validNode {};
    std::string invalidNodeName = "FeatureSingleParam";
    std::string validNodeName = "FeatureSingleParam";
    invalidNode.type = xmlElementType::XML_ELEMENT_NODE;
    invalidNode.name = reinterpret_cast<const xmlChar*>(invalidNodeName.c_str());
    xmlSetProp(&invalidNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_stop_invalid"));
    validNode.type = xmlElementType::XML_ELEMENT_NODE;
    validNode.name = reinterpret_cast<const xmlChar*>(validNodeName.c_str());
    xmlSetProp(&validNode, (const xmlChar*)("name"), (const xmlChar*)("ddgr_stop_valid"));
    xmlSetProp(&validNode, (const xmlChar*)("value"), (const xmlChar*)("valid"));
    node.xmlChildrenNode = &invalidNode;
    invalidNode.next = &validNode;

    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_stop_invalid"), 0);
    ASSERT_EQ(params.count("ddgr_stop_valid"), 1);
}
} // namespace OHOS::Rosen
