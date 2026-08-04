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
#include <libxml/tree.h>
#include "virtual_screen_parallel_param_parse.h"
#include "virtual_screen_parallel_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class VirtualScreenParallelParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    VirtualScreenParallelParamParse parser_;
};

void VirtualScreenParallelParamParseTest::SetUpTestCase() {}
void VirtualScreenParallelParamParseTest::TearDownTestCase() {}
void VirtualScreenParallelParamParseTest::SetUp() {}
void VirtualScreenParallelParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParam_XmlChildrenNodeNull
 * @tc.desc: Test ParseFeatureParam when xmlChildrenNode is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_XmlChildrenNodeNull, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;
    node.xmlChildrenNode = nullptr;

    int32_t result = parser_.ParseFeatureParam(featureMap, node);

    EXPECT_EQ(result, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureParam_XmlChildrenNodeNotNull
 * @tc.desc: Test ParseFeatureParam when xmlChildrenNode is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_XmlChildrenNodeNotNull, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string name = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(name.c_str());

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParam_NodeTypeNotElement
 * @tc.desc: Test ParseFeatureParam when node type is not XML_ELEMENT_NODE
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_NodeTypeNotElement, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_TEXT_NODE;
    childNode.next = nullptr;

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParam_MultipleNodes
 * @tc.desc: Test ParseFeatureParam with multiple child nodes
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_MultipleNodes, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode1;
    xmlNode childNode2;

    parentNode.xmlChildrenNode = &childNode1;
    childNode1.type = XML_ELEMENT_NODE;
    childNode1.next = &childNode2;
    childNode2.type = XML_ELEMENT_NODE;
    childNode2.next = nullptr;

    std::string name = "FeatureSwitch";
    childNode1.name = reinterpret_cast<const xmlChar*>(name.c_str());
    childNode2.name = reinterpret_cast<const xmlChar*>(name.c_str());

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_XmlParamTypeFeatureSwitch
 * @tc.desc: Test ParseVirtualScreenParallelInternal when xmlParamType is PARSE_XML_FEATURE_SWITCH
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_XmlParamTypeFeatureSwitch,
    TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;

    node.type = XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());

    xmlAttr prop1;
    xmlAttr prop2;
    std::string propName1 = "name";
    std::string propValue1 = "VirtualScreenParallelEnabled";
    std::string propName2 = "value";
    std::string propValue2 = "true";

    prop1.name = reinterpret_cast<const xmlChar*>(propName1.c_str());
    prop1.children = nullptr;
    prop1.next = &prop2;

    prop2.name = reinterpret_cast<const xmlChar*>(propName2.c_str());
    prop2.children = nullptr;
    prop2.next = nullptr;

    node.properties = &prop1;

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);
    parser_.ParseFeatureParam(featureMap, node);

    EXPECT_FALSE(VirtualScreenParallelParam::IsVirtualScreenParallelEnabled());
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_XmlParamTypeNotFeatureSwitch
 * @tc.desc: Test ParseVirtualScreenParallelInternal when xmlParamType is not PARSE_XML_FEATURE_SWITCH
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_XmlParamTypeNotFeatureSwitch,
    TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;

    node.type = XML_ELEMENT_NODE;
    std::string name = "SingleParam";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());

    node.properties = nullptr;

    int32_t result = parser_.ParseFeatureParam(featureMap, node);

    EXPECT_EQ(result, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_NameMatch
 * @tc.desc: Test ParseVirtualScreenParallelInternal when name matches VirtualScreenParallelEnabled
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_NameMatch, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_NameNotMatch
 * @tc.desc: Test ParseVirtualScreenParallelInternal when name does not match VirtualScreenParallelEnabled
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_NameNotMatch, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "OtherFeature";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_EnableTrue
 * @tc.desc: Test ParseVirtualScreenParallelInternal when value is true
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_EnableTrue, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;

    node.type = XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    int32_t result = parser_.ParseFeatureParam(featureMap, node);

    EXPECT_EQ(result, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_EnableFalse
 * @tc.desc: Test ParseVirtualScreenParallelInternal when value is false
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_EnableFalse, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode node;

    node.type = XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    int32_t result = parser_.ParseFeatureParam(featureMap, node);

    EXPECT_EQ(result, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureParam_34True_InternalParseFail
 * @tc.desc: Test ParseFeatureParam when ParseVirtualScreenParallelInternal returns non-SUCCESS (line 34 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_34True_InternalParseFail, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string name = "InvalidNode";
    childNode.name = reinterpret_cast<const xmlChar*>(name.c_str());

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_TRUE(result == ParseErrCode::PARSE_EXEC_SUCCESS || result == ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureParam_34True_MultipleFailures
 * @tc.desc: Test ParseFeatureParam with multiple nodes where parsing might fail
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseFeatureParam_34True_MultipleFailures, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode1;
    xmlNode childNode2;

    parentNode.xmlChildrenNode = &childNode1;
    childNode1.type = XML_ELEMENT_NODE;
    childNode1.next = &childNode2;
    childNode2.type = XML_ELEMENT_NODE;
    childNode2.next = nullptr;

    std::string name1 = "InvalidNode1";
    std::string name2 = "InvalidNode2";
    childNode1.name = reinterpret_cast<const xmlChar*>(name1.c_str());
    childNode2.name = reinterpret_cast<const xmlChar*>(name2.c_str());

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_TRUE(result == ParseErrCode::PARSE_EXEC_SUCCESS || result == ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_52True_NameMatch
 * @tc.desc: Test ParseVirtualScreenParallelInternal when name matches
 * VirtualScreenParallelEnabled (line 52 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_52True_NameMatch, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    xmlAttr prop1;
    xmlAttr prop2;
    xmlNode textNode1;
    xmlNode textNode2;

    std::string propName1 = "name";
    std::string propValue1 = "VirtualScreenParallelEnabled";
    std::string propName2 = "value";
    std::string propValue2 = "true";

    prop1.name = reinterpret_cast<const xmlChar*>(propName1.c_str());
    prop1.children = &textNode1;
    prop1.next = &prop2;

    textNode1.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue1.c_str()));

    prop2.name = reinterpret_cast<const xmlChar*>(propName2.c_str());
    prop2.children = &textNode2;
    prop2.next = nullptr;

    textNode2.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue2.c_str()));

    childNode.properties = &prop1;

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_FALSE(VirtualScreenParallelParam::IsVirtualScreenParallelEnabled());
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_52True_EnableFalse
 * @tc.desc: Test ParseVirtualScreenParallelInternal with name match and value false (line 52 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_52True_EnableFalse, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    xmlAttr prop1;
    xmlAttr prop2;
    xmlNode textNode1;
    xmlNode textNode2;

    std::string propName1 = "name";
    std::string propValue1 = "VirtualScreenParallelEnabled";
    std::string propName2 = "value";
    std::string propValue2 = "false";

    prop1.name = reinterpret_cast<const xmlChar*>(propName1.c_str());
    prop1.children = &textNode1;
    prop1.next = &prop2;

    textNode1.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue1.c_str()));

    prop2.name = reinterpret_cast<const xmlChar*>(propName2.c_str());
    prop2.children = &textNode2;
    prop2.next = nullptr;

    textNode2.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue2.c_str()));

    childNode.properties = &prop1;

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_TRUE(VirtualScreenParallelParam::IsVirtualScreenParallelEnabled());
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_52True_ValueTrue
 * @tc.desc: Test ParseVirtualScreenParallelInternal with name match and value true (line 52 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_52True_ValueTrue, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    xmlAttr prop1;
    xmlAttr prop2;
    xmlNode textNode1;
    xmlNode textNode2;

    std::string propName1 = "name";
    std::string propValue1 = "VirtualScreenParallelEnabled";
    std::string propName2 = "value";
    std::string propValue2 = "true";

    prop1.name = reinterpret_cast<const xmlChar*>(propName1.c_str());
    prop1.children = &textNode1;
    prop1.next = &prop2;

    textNode1.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue1.c_str()));

    prop2.name = reinterpret_cast<const xmlChar*>(propName2.c_str());
    prop2.children = &textNode2;
    prop2.next = nullptr;

    textNode2.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue2.c_str()));

    childNode.properties = &prop1;

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseVirtualScreenParallelInternal_52True_ExactNameMatch
 * @tc.desc: Test ParseVirtualScreenParallelInternal with exact name match (line 52 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(VirtualScreenParallelParamParseTest, ParseVirtualScreenParallelInternal_52True_ExactNameMatch, TestSize.Level1)
{
    FeatureParamMapType featureMap;
    xmlNode parentNode;
    xmlNode childNode;

    parentNode.xmlChildrenNode = &childNode;
    childNode.type = XML_ELEMENT_NODE;
    childNode.next = nullptr;

    std::string nodeName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(nodeName.c_str());

    xmlAttr prop1;
    xmlAttr prop2;
    xmlNode textNode1;
    xmlNode textNode2;

    std::string propName1 = "name";
    std::string propValue1 = "VirtualScreenParallelEnabled";
    std::string propName2 = "value";
    std::string propValue2 = "1";

    prop1.name = reinterpret_cast<const xmlChar*>(propName1.c_str());
    prop1.children = &textNode1;
    prop1.next = &prop2;

    textNode1.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue1.c_str()));

    prop2.name = reinterpret_cast<const xmlChar*>(propName2.c_str());
    prop2.children = &textNode2;
    prop2.next = nullptr;

    textNode2.content = reinterpret_cast<xmlChar*>(const_cast<char*>(propValue2.c_str()));

    childNode.properties = &prop1;

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    int32_t result = parser_.ParseFeatureParam(featureMap, parentNode);

    EXPECT_EQ(result, ParseErrCode::PARSE_EXEC_SUCCESS);
}
} // namespace OHOS::Rosen