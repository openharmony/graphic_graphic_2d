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
#include "subtree_parallel_param_parse.h"

#include "graphic_feature_param_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SubtreeParallelParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SubtreeParallelParamParseTest::SetUpTestCase() {}
void SubtreeParallelParamParseTest::TearDownTestCase() {}
void SubtreeParallelParamParseTest::SetUp() {}
void SubtreeParallelParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#ICR4Z9
 */
HWTEST_F(SubtreeParallelParamParseTest, ParseFeatureParamTest, TestSize.Level1)
{
    SubtreeParallelParamParse SubtreeParallelParamParse;
    FeatureParamMapType paramMapType;

    GraphicFeatureParamManager::GetInstance().Init();

    auto subtreeParam = std::static_pointer_cast<SubtreeParallelParam>(
        GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[SUBTREEPARALLEL]));

    bool subtreeenable = subtreeParam->GetSubtreeEnable();
    std::cout << "DFX:Subtree Parallel GetSubtreeEnable:" <<  subtreeenable << std::endl;

    bool subtreeRbEnable =  subtreeParam->GetRBPolicyEnabled();
    std::cout << "DFX:Subtree Parallel GetRBPolicyEnabled:" <<  subtreeRbEnable << std::endl;

    bool multiwinEnable = subtreeParam->GetMultiWinPolicyEnabled();
    std::cout << "DFX:Subtree Parallel GetMultiWinPolicyEnabled:" <<  multiwinEnable << std::endl;

    int multiwinSurfaceNum =  subtreeParam->GetMutliWinSurfaceNum();
    std::cout << "DFX:Subtree Parallel GetMutliWinSurfaceNum:" <<  multiwinSurfaceNum << std::endl;

    int subtreeChildWeight = subtreeParam->GetRBChildrenWeight();
    std::cout << "DFX:Subtree Parallel GetRBChildrenWeight:" <<  subtreeChildWeight << std::endl;

    int subtreeRbWeight = subtreeParam->GetRBSubtreeWeight();
    std::cout << "DFX:Subtree Parallel GetRBSubtreeWeight:" <<  subtreeRbWeight << std::endl;

    std::unordered_map<std::string, std::string> subtreeScene = subtreeParam->GetSubtreeScene();
    
    for (auto& pair : subtreeScene) {
        std::cout << "DFX:Subtree Parallel subtreeScene:" <<  pair.first << ":" << pair.second  << std::endl;
    }
    EXPECT_NE(subtreeParam, nullptr);
}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#ICR4Z9
 */
HWTEST_F(SubtreeParallelParamParseTest, ParseFeatureParamTest001, TestSize.Level1)
{
    SubtreeParallelParamParse paramParse;
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

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("SubtreeEnabled"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("true"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetSubtreeEnable(), true);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("MutliWinPolicyEnabled"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("true"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetMultiWinPolicyEnabled(), true);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("RBPolicyEnabled"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("true"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetRBPolicyEnabled(), true);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"), reinterpret_cast<const xmlChar*>("UnknownName"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#ICR4Z9
 */
HWTEST_F(SubtreeParallelParamParseTest, ParseFeatureParamTest002, TestSize.Level1)
{
    SubtreeParallelParamParse paramParse;
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
    node.xmlChildrenNode = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("IsNotNumber"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("MutliWinPolicySurfaceNumber"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("2"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetMutliWinSurfaceNum(), 2);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("RBPolicyChildrenWeight"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("4"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetRBChildrenWeight(), 4);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("RBPolicySubtreeWeight"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("100"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetRBSubtreeWeight(), 100);

    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("UnknownName"));
    xmlSetProp(&nextNode, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("1"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);
}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#ICR4Z9
 */
HWTEST_F(SubtreeParallelParamParseTest, ParseFeatureParamTest003, TestSize.Level1)
{
    SubtreeParallelParamParse paramParse;
    FeatureParamMapType paramMapType;
    xmlNode node;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);

    xmlNode attributeNode;
    attributeNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &attributeNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureMultiParam";
    childNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode = &childNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);

    xmlNode childNode1;
    childNode1.type = xmlElementType::XML_ELEMENT_NODE;
    childNode1.name = reinterpret_cast<const xmlChar*>("SubtreeEnableScene");
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("IsNotNumber"));
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("false"));
    childNode.xmlChildrenNode = &childNode1;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);

    childNode1.type = xmlElementType::XML_ATTRIBUTE_NODE;
    childNode1.name = reinterpret_cast<const xmlChar*>("SubtreeEnableScene");
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("NoElementNode"));
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("1"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    childNode1.type = xmlElementType::XML_ELEMENT_NODE;
    childNode1.name = reinterpret_cast<const xmlChar*>("SubtreeEnableScene");
    std::cout << childNode1.name << std::endl;
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("name"), reinterpret_cast<const xmlChar*>("test"));
    xmlSetProp(&childNode1, reinterpret_cast<const xmlChar*>("value"), reinterpret_cast<const xmlChar*>("1"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_NE(SubtreeParallelParam::GetSubtreeScene()["test"], "1");

    xmlSetProp(&childNode, reinterpret_cast<const xmlChar*>("name"),
        reinterpret_cast<const xmlChar*>("SubtreeEnableScene"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(SubtreeParallelParam::GetSubtreeScene()["test"], "1");
}

/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require: issue#ICR4Z9
 */
HWTEST_F(SubtreeParallelParamParseTest, ParseFeatureParamTest004, TestSize.Level1)
{
    SubtreeParallelParamParse paramParse;
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
    string name = "FeatureUnknownParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
}